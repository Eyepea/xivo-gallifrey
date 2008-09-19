# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Alternatively, XIVO Daemon is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XIVO Daemon
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, you will find one at
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.

"""
This is the XivoCTI class.
"""

import cjson
import os
import random
import re
import socket
import sha
import string
import time
import urllib
from xivo_ctiservers import cti_capas
from xivo_ctiservers import cti_fax
from xivo_ctiservers import cti_userlist
from xivo_ctiservers import cti_urllist
from xivo_ctiservers import cti_agentlist
from xivo_ctiservers import cti_queuelist
from xivo_ctiservers import xivo_commandsets
from xivo_ctiservers import xivo_ldap
from xivo_ctiservers import xivo_phones
from xivo_ctiservers.xivo_commandsets import BaseCommand
from xivo_ctiservers.xivo_log import *
from xivo import anysql
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite

def log_debug(level, text):
        log_debug_file(level, text, 'xivocti')

XIVOVERSION = '0.4'
REQUIRED_CLIENT_VERSION = 4141
__revision__ = __version__.split()[1]
__alphanums__ = string.uppercase + string.lowercase + string.digits
allowed_states = ['available', 'away', 'outtolunch', 'donotdisturb', 'berightback']
ITEMS_PER_PACKET = 500
HISTSEPAR = ';'
DEFAULTCONTEXT = 'default'

class XivoCTICommand(BaseCommand):

        xdname = 'XIVO Daemon'
        xivoclient_session_timeout = 60 # XXX

        fullstat_heavies = {}
        queues_channels_list = {}
        agents_list = {}

        def __init__(self, amilist, ctiports, queued_threads_pipe):
		BaseCommand.__init__(self)
                self.amilist = amilist
                self.capas = {}
                self.ulist_ng = cti_userlist.UserList()
                self.ulist_ng.setcommandclass(self)
                self.plist = {}
                self.qlist = {}
                self.alist = {}
                # self.plist_ng = cti_phonelist.PhoneList()
                # self.plist_ng.setcommandclass(self)
                self.transfers_buf = {}
                self.transfers_ref = {}
                self.faxes = {}
                self.queued_threads_pipe = queued_threads_pipe
                self.disconnlist = []
                self.sheet_actions = {}
                self.ldapids = {}
                self.chans_incomingqueue = []
                self.chans_incomingdid = []
                self.meetme = {}
                return

        def get_list_commands(self):
                return ['login_id', 'login_pass', 'login_capas',
                        'history', 'directory-search',
                        'featuresget', 'featuresput',
                        'phones-list', 'phones-add', 'phones-del',
                        'agents-list', 'agents-status', 'agent-status', 'agent',
                        'queues-list', 'queue-status',
                        'users-list',
                        'callcampaign',
                        'faxsend',
                        'faxdata',
                        'database',
                        'meetme',
                        'message',
                        'json',
                        'availstate',
                        'originate', 'transfer', 'atxfer', 'hangup', 'simplehangup', 'pickup']

        def parsecommand(self, linein):
                params = linein.split()
                cmd = xivo_commandsets.Command(params[0], params[1:])
                cmd.struct = {}
                if cmd.name == 'login_id':
                        cmd.type = xivo_commandsets.CMD_LOGIN_ID
                elif cmd.name == 'login_pass':
                        cmd.type = xivo_commandsets.CMD_LOGIN_PASS
                elif cmd.name == 'login_capas':
                        cmd.type = xivo_commandsets.CMD_LOGIN_CAPAS
                elif cmd.name == 'faxdata':
                        cmd.type = xivo_commandsets.CMD_TRANSFER
                else:
                        cmd.type = xivo_commandsets.CMD_OTHER
                        try:
                                cmd.struct = cjson.decode(linein)
                                cmd.name = 'json'
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- parsing json for <%s> : %s' % (linein, exc))
                                cmd.struct = {}
                return cmd

        def transfer_addbuf(self, req, buf):
                self.transfers_buf[req].append(buf)
                return

        def transfer_addref(self, req, ref):
                self.transfers_ref[req] = ref
                self.transfers_buf[req] = []
                return

        def transfer_endbuf(self, req):
                log_debug(SYSLOG_INFO, 'full buffer received for %s : len=%d %s'
                          % (req, len(''.join(self.transfers_buf[req])), self.transfers_ref))
                if req in self.transfers_ref:
                        ref = self.transfers_ref[req]
                        if ref in self.faxes:
                                uinfo = self.faxes[ref].uinfo
                                astid = uinfo.get('astid')
                                self.faxes[ref].sendfax(''.join(self.transfers_buf[req]),
                                                        self.configs[astid].faxcallerid,
                                                        self.amilist.ami[astid])
                                del self.transfers_ref[req]
                                del self.transfers_buf[req]
                                del self.faxes[ref]
                return

        def get_login_params(self, command, astid, connid):
                """
                Login syntax awaited : "login astid=xivo;...=..."
                """
                cfg = {}
                if len(command.args) > 0:
                        arglist = command.args[0].split(';')
                        for argm in arglist:
                                argms = argm.split('=')
                                if len(argms) == 2:
                                        [param, value] = argms
                                        cfg[param] = value
                return cfg

        def manage_login(self, loginparams, phase, uinfo):
                if phase == xivo_commandsets.CMD_LOGIN_ID:
                        missings = []
                        for argum in ['company', 'userid', 'ident', 'xivoversion', 'version']:
                                if argum not in loginparams:
                                        missings.append(argum)
                        if len(missings) > 0:
                                log_debug(SYSLOG_WARNING, 'missing args in loginparams : %s' % ','.join(missings))
                                return 'missing:%s' % ','.join(missings)

                        # trivial checks (version, client kind) dealing with the software used
                        xivoversion = loginparams.get('xivoversion')
                        if xivoversion != XIVOVERSION:
                                return 'xivoversion_client:%s;%d' % (xivoversion, XIVOVERSION)
                        svnversion = loginparams.get('version')
                        ident = loginparams.get('ident')
                        if len(ident.split('@')) == 2:
                                [whoami, whatsmyos] = ident.split('@')
                                # return 'wrong_client_identifier:%s' % whoami
                                if whatsmyos[:3] not in ['X11', 'WIN', 'MAC']:
                                        return 'wrong_os_identifier:%s' % whatsmyos
                        else:
                                return 'wrong_client_os_identifier:%s' % ident
                        if (not svnversion.isdigit()) or int(svnversion) < REQUIRED_CLIENT_VERSION:
                                return 'version_client:%s;%d' % (svnversion, REQUIRED_CLIENT_VERSION)

                        # user match
                        username = '%s@%s' % (loginparams.get('userid'), loginparams.get('company'))
                        userinfo = self.ulist_ng.finduser(username)
                        if userinfo == None:
                                return 'user_not_found'
                        userinfo['prelogin'] = {'cticlienttype' : whoami,
                                                'cticlientos' : whatsmyos,
                                                'version' : svnversion,
                                                'sessionid' : ''.join(random.sample(__alphanums__, 10))}
                        return userinfo

                elif phase == xivo_commandsets.CMD_LOGIN_PASS:
                        # user authentication
                        missings = []
                        for argum in ['hashedpassword']:
                                if argum not in loginparams:
                                        missings.append(argum)
                        if len(missings) > 0:
                                log_debug(SYSLOG_WARNING, 'missing args in loginparams : %s' % ','.join(missings))
                                return 'missing:%s' % ','.join(missings)

                        if uinfo is not None:
                                userinfo = uinfo
                        hashedpassword = loginparams.get('hashedpassword')
                        tohash = '%s:%s' % (userinfo['prelogin']['sessionid'], userinfo.get('password'))
                        sha1sum = sha.sha(tohash).hexdigest()
                        if sha1sum != hashedpassword:
                                return 'login_password'
                        
                        iserr = self.__check_user_connection__(userinfo)
                        if iserr is not None:
                                return iserr
                        return userinfo

                elif phase == xivo_commandsets.CMD_LOGIN_CAPAS:
                        missings = []
                        for argum in ['state', 'capaid', 'lastconnwins', 'loginkind']:
                                if argum not in loginparams:
                                        missings.append(argum)
                        if len(missings) > 0:
                                log_debug(SYSLOG_WARNING, 'missing args in loginparams : %s' % ','.join(missings))
                                return 'missing:%s' % ','.join(missings)

                        if uinfo is not None:
                                userinfo = uinfo
                        # settings (in agent mode for instance)
                        # userinfo['agent']['phonenum'] = phonenum

                        state = loginparams.get('state')
                        capaid = loginparams.get('capaid')
                        subscribe = loginparams.get('subscribe')
                        lastconnwins = (loginparams.get('lastconnwins') == 'true')

                        iserr = self.__check_capa_connection__(userinfo, capaid)
                        if iserr is not None:
                                return iserr
                        
                        self.__connect_user__(userinfo, state, capaid, lastconnwins)
                        
                        loginkind = loginparams.get('loginkind')
                        if loginkind == 'agent':
                                userinfo['agentphonenum'] = loginparams.get('phonenumber')
                                # self.amilist.execute(astid, 'agentcallbacklogin', agentnum, phonenum)
                        if subscribe is not None:
                                userinfo['subscribe'] = 0
                        return userinfo


        def manage_logoff(self, userinfo, when):
                log_debug(SYSLOG_INFO, 'logoff (%s) %s'
                          % (when, userinfo))
                userinfo['logofftime-ascii-last'] = time.asctime()
                if 'agentnum' in userinfo:
                        agentnum = userinfo['agentnum']
                        astid = userinfo['astid']
                        if 'phonenum' in userinfo:
                                phonenum = userinfo['phonenum']
                                self.amilist.execute(astid, 'setvar', 'AGENTBYCALLERID_%s' % phonenum, '')
                        if agentnum is not None and len(agentnum) > 0:
                                self.amilist.execute(astid, 'agentlogoff', agentnum)
                if 'agentphonenum' in userinfo:
                        del userinfo['agentphonenum']
                self.__disconnect_user__(userinfo)
                return


        def __check_user_connection__(self, userinfo):
                #if userinfo.has_key('init'):
                #       if not userinfo['init']:
                #              return 'uninit_phone'
                if userinfo.has_key('login') and userinfo['login'].has_key('sessiontimestamp'):
                        if time.time() - userinfo['login'].get('sessiontimestamp') < self.xivoclient_session_timeout:
                                log_debug(SYSLOG_WARNING, 'user %s already connected from %s'
                                          % (userinfo['user'], userinfo['login']['connection'].getpeername()))
                                if 'lastconnwins' in userinfo:
                                        if userinfo['lastconnwins']:
                                                # one should then disconnect the already connected instance
                                                pass
                                        else:
                                                return 'already_connected:%s:%d' % userinfo['login']['connection'].getpeername()
                                else:
                                        return 'already_connected:%s:%d' % userinfo['login']['connection'].getpeername()
                return None


        def __check_capa_connection__(self, userinfo, capaid):
                if capaid in self.capas and capaid in userinfo.get('capaids').split(','):
                        if self.capas[capaid].toomuchusers():
                                return 'toomuchusers:%s' % self.capas[capaid].maxgui()
                else:
                        return 'capaid_undefined'
                return None


        def __connect_user__(self, userinfo, state, capaid, lastconnwins):
                try:
                        userinfo['capaid'] = capaid
                        userinfo['login'] = {}
                        userinfo['login']['sessiontimestamp'] = time.time()
                        userinfo['login']['logintimestamp'] = time.time()
                        userinfo['login']['logintime-ascii'] = time.asctime()
                        for v, vv in userinfo['prelogin'].iteritems():
                                userinfo['login'][v] = vv
                        del userinfo['prelogin']
                        # lastconnwins was introduced in the aim of forcing a new connection to take on for
                        # a given user, however it might breed problems if the previously logged-in process
                        # tries to reconnect ... unless we send something asking to Kill the process
                        userinfo['lastconnwins'] = lastconnwins

                        # we first check if 'state' has already been set for this customer, in which case
                        # the CTI clients will be sent back this previous state
                        if 'state' in userinfo:
                                futurestate = userinfo.get('state')
                                # only if it was a "defined" state anyway
                                if futurestate in allowed_states:
                                        state = futurestate

                        if state in allowed_states:
                                userinfo['state'] = state
                        else:
                                log_debug(SYSLOG_WARNING, '(user %s) : state <%s> is not an allowed one => undefinedstate-connect'
                                          % (userinfo.get('user'), state))
                                userinfo['state'] = 'undefinedstate-connect'

                        self.capas[capaid].conn_inc()
                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- connect_user %s : %s" %(userinfo, exc))


        def __disconnect_user__(self, userinfo):
                try:
                        # state is unchanged
                        if 'login' in userinfo:
                                capaid = userinfo.get('capaid')
                                self.capas[capaid].conn_dec()
                                del userinfo['capaid']
                                userinfo['last-version'] = userinfo['login']['version']
                                del userinfo['login']
                                userinfo['state'] = 'unknown'
                                self.__update_availstate__(userinfo, userinfo.get('state'))
                        else:
                                log_debug(SYSLOG_WARNING, 'userinfo does not contain login field : %s' % userinfo)
                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- disconnect_user %s : %s" %(userinfo, exc))


        def loginko(self, loginparams, errorstring, connid):
                log_debug(SYSLOG_WARNING, 'user can not connect (%s) : sending %s' % (loginparams, errorstring))
                tosend = { 'class' : 'loginko',
                           'direction' : 'client',
                           'errorstring' : errorstring }
                connid.sendall('%s\n' % cjson.encode(tosend))
                return
        
        def loginok(self, loginparams, userinfo, connid, phase):
                if phase == xivo_commandsets.CMD_LOGIN_ID:
                        tosend = { 'class' : 'login_id_ok',
                                   'direction' : 'client',
                                   'xivoversion' : XIVOVERSION,
                                   'version' : __revision__,
                                   'sessionid' : userinfo['prelogin']['sessionid'] }
                        repstr = cjson.encode(tosend)
                elif phase == xivo_commandsets.CMD_LOGIN_PASS:
                        tosend = { 'class' : 'login_pass_ok',
                                   'direction' : 'client',
                                   'capalist' : userinfo.get('capaids') }
                        repstr = cjson.encode(tosend)
                elif phase == xivo_commandsets.CMD_LOGIN_CAPAS:
                        capaid = userinfo.get('capaid')
                        tosend = { 'class' : 'login_capas_ok',
                                   'direction' : 'client',
                                   'capafuncs' : self.capas[capaid].tostring(self.capas[capaid].all()),
                                   'capaxlets' : self.capas[capaid].capadisps,
                                   'appliname' : self.capas[capaid].appliname,
                                   'state' : userinfo.get('state') }
                        repstr = cjson.encode(tosend)
                        # if 'features' in capa_user:
                        # repstr += ';capas_features:%s' %(','.join(configs[astid].capafeatures))
                connid.sendall(repstr + '\n')

                if phase == xivo_commandsets.CMD_LOGIN_CAPAS:
                        self.__update_availstate__(userinfo, userinfo.get('state'))
                
                return

        def set_cticonfig(self, lconf):
                self.lconf = lconf

                self.sheet_actions = {}
                for where, sheetaction in lconf.read_section('sheet_events', 'sheet_events').iteritems():
                        if where in self.sheet_allowed_events and len(sheetaction) > 0:
                                self.sheet_actions[where] = lconf.read_section('sheet_action', sheetaction)
                return


        def set_options(self, xivoconf):
                self.xivoconf = xivoconf
                for var, val in self.xivoconf.iteritems():
                        if var.find('-') > 0:
                                [name, prop] = var.split('-', 1)
                                if name not in self.capas:
                                        self.capas[name] = cti_capas.Capabilities()
                                if prop == 'xlets':
                                        self.capas[name].setxlets(val.split(','))
                                elif prop == 'funcs':
                                        self.capas[name].setfuncs(val.split(','))
                                elif prop == 'maxgui':
                                        self.capas[name].setmaxgui(val)
                                elif prop == 'appliname':
                                        self.capas[name].setappliname(val)
                return

        def set_configs(self, configs):
                self.configs = configs
                return

        def set_phonelist(self, astid, urllist_phones):
                self.plist[astid] = xivo_phones.PhoneList(astid, self, urllist_phones)
                return

        def set_agentlist(self, astid, urllist_agents):
                self.alist[astid] = cti_agentlist.AgentList(urllist_agents)
                self.alist[astid].setcommandclass(self)
                return

        def set_queuelist(self, astid, urllist_queues):
                self.qlist[astid] = cti_queuelist.QueueList(urllist_queues)
                self.qlist[astid].setcommandclass(self)
                return

        def set_contextlist(self, ctxlist):
                self.ctxlist = ctxlist
                return

        def updates(self):
                self.ulist_ng.update()
                # self.plist_ng.update()
                for astid, plist in self.plist.iteritems():
                        self.alist[astid].update()
                        self.qlist[astid].update()
                        npl = self.plist[astid].update_phonelist()
                        self.askstatus(astid, npl)

                # check : agentnumber should be unique
                return

        def set_userlist_urls(self, urls):
                self.ulist_ng.setandupdate(urls)
                return

        def getagentslist(self, dlist):
                lalist = {}
                return lalist

        def getqueueslist(self, dlist):
                lqlist = {}
                for queuename, d in dlist.iteritems():
                        if d[0] == 'queue' and d[4] == '0':
                                lqlist[queuename] = {'queuename' : d[1],
                                                     'number' : d[2],
                                                     'context' : d[3],
                                                     
                                                     'agents' : {},
                                                     'channels' : {},
                                                     'stats' : {}}
                return lqlist

        # fields set at startup by reading informations
        userfields = ['user', 'company', 'astid', 'password', 'fullname', 'capaids', 'context', 'phonenum', 'techlist', 'agentnum', 'xivo_userid']
        def getuserslist(self, dlist):
                lulist = {}
                for c, d in dlist.iteritems():
                        if len(d) > 10:
                                lulist[c] = {'user'     : d[0].split('@')[0],
                                             'company'  : d[0].split('@')[1],
                                             'password' : d[1],
                                             'capaids'  : d[2],
                                             'fullname' : d[3] + ' ' + d[4],
                                             'astid'    : d[5],
                                             'agentnum' : d[9],
                                             'techlist' : d[7],
                                             'context'  : d[8],
                                             'phonenum' : d[6],
                                             'xivo_userid' : d[10],
                                             
                                             'state'    : 'unknown',
                                             'mwi-waiting' : '0',
                                             'mwi-old' : '0',
                                             'mwi-new' : '0'}
                return lulist

        def getuserslist_compat(self, dlist):
                lulist = {}
                for c, d in dlist.iteritems():
                        if len(d) > 11:
                                userid = d[1]
                                company = 'acme'
                                fieldname = '%s@%s' % (userid, company)
                                lulist[fieldname] = {'user'     : userid,
                                                     'company'  : company,
                                                     'password' : d[2],
                                                     'capaids'  : 'default',
                                                     'fullname' : d[8] + ' ' + d[9],
                                                     'astid'    : 'xivo',
                                                     'agentnum' : '',
                                                     'techlist' : d[0].upper() + '/' + d[4],
                                                     'context'  : d[10],
                                                     'phonenum' : d[4],
                                                     'xivo_userid' : '-1',

                                                     'state'    : 'unknown',
                                                     'mwi-waiting' : '0',
                                                     'mwi-old' : '0',
                                                     'mwi-new' : '0'}
                return lulist

        def users(self):
                return self.ulist_ng.users()
        def connected_users(self):
                return self.ulist_ng.connected_users()


        def askstatus(self, astid, npl):
                for a, b in npl.iteritems():
                        self.amilist.execute(astid, 'sendextensionstate', b[0], b[1])
                        self.amilist.execute(astid, 'mailbox', b[0], b[1])
                return


        def checkqueue(self):
                buf = os.read(self.queued_threads_pipe[0], 1024)
                log_debug(SYSLOG_WARNING, 'checkqueue : read buf = %s' % buf)
                return self.disconnlist


        def clear_disconnlist(self):
                self.disconnlist = []
                return


        def __send_msg_to_cti_client__(self, userinfo, strupdate):
                try:
                        if 'login' in userinfo and 'connection' in userinfo.get('login'):
                                mysock = userinfo.get('login')['connection']
                                mysock.sendall(strupdate + '\n', socket.MSG_WAITALL)
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- (__send_msg_to_cti_client__) : %s (%s) userinfo = %s'
                                  % (exc, exc.__class__, userinfo))
                        if userinfo not in self.disconnlist:
                                self.disconnlist.append(userinfo)
                                os.write(self.queued_threads_pipe[1], 'uinfo\n')
                return


        def __send_msg_to_cti_client_byagentid__(self, agentnum, strupdate):
                try:
                        for userinfo in self.ulist_ng.userlist.itervalues():
                                if 'agentnum' in userinfo and userinfo.get('agentnum') == agentnum:
                                        self.__send_msg_to_cti_client__(userinfo, strupdate)
                except Exception, exc:
                        log_debug(SYSLOG_WARNING, '--- exception --- (__send_msg_to_cti_client_byagentid__) : %s' % exc)
                return


        def __send_msg_to_cti_clients__(self, strupdate):
                try:
                        if strupdate is not None:
                                for userinfo in self.ulist_ng.userlist.itervalues():
                                        self.__send_msg_to_cti_client__(userinfo, strupdate)
                except Exception, exc:
                        log_debug(SYSLOG_WARNING, '--- exception --- (__send_msg_to_cti_clients__) : %s' % exc)
                return


        sheet_allowed_events = ['incomingqueue', 'incomingdid',
                                'agentcalled', 'agentselected',
                                'agi', 'link', 'unlink', 'hangup',
                                'callmissed', # see GG (in order to tell a user that he missed a call)
                                'localphonecalled', 'outgoing']


        def __build_xmlsheet__(self, sheetkind, actionopt, inputvars):
                linestosend = []
                whichitem = actionopt.get(sheetkind)
                if whichitem is not None and len(whichitem) > 0:
                        for k, v in self.lconf.read_section('sheet_action', whichitem).iteritems():
                                try:
                                        vsplit = v.split('|')
                                        if len(vsplit) == 4:
                                                [title, type, defaultval, format] = v.split('|')
                                                basestr = format
                                                for kk, vv in inputvars.iteritems():
                                                        basestr = basestr.replace('{%s}' % kk, vv)
                                                basestr = re.sub('{[a-z\-]*}', defaultval, basestr)
                                                linestosend.append('<%s order="%s" name="%s" type="%s"><![CDATA[%s]]></%s>'
                                                                   % (sheetkind, k, title, type, basestr, sheetkind))
                                        else:
                                                log_debug(SYSLOG_WARNING, '__build_xmlsheet__ wrong number of fields in definition for %s %s %s'
                                                          % (sheetkind, whichitem, k))
                                except Exception, exc:
                                        log_debug(SYSLOG_ERR, '--- exception --- __build_xmlsheet__ %s %s : %s' % (sheetkind, whichitem, exc))
                return linestosend


        def __sheet_alert__(self, where, astid, context, event, extraevent = {}):
                # fields to display :
                # - internal asterisk/xivo : caller, callee, queue name, sda
                # - custom ext database
                # - custom (F)AGI
                # options :
                # - urlauto, urlx
                # - popup or not
                # - actions ?
                # - dispatch to one given person / all / subscribed ones
                if where in self.sheet_actions:
                        userinfos = []
                        actionopt = self.sheet_actions.get(where)
                        whoms = actionopt.get('whom')
                        if whoms is None or whoms == '':
                                log_debug(SYSLOG_WARNING, '__sheet_alert__ (%s) : whom field for %s action has not been defined'
                                          % (astid, where))
                                return

                        linestosend = ['<?xml version="1.0" encoding="utf-8"?>',
                                       '<profile sessionid="sessid">',
                                       '<user>']
                        # XXX : sessid => uniqueid, in order to update (did => queue => ... hangup ...)
                        linestosend.append('<internal name="datetime"><![CDATA[%s]]></internal>' % time.asctime())
                        itemdir = {'xivo-where' : where,
                                   'xivo-astid' : astid,
                                   'xivo-context' : context,
                                   'xivo-time' : time.strftime('%H:%M:%S', time.localtime()),
                                   'xivo-date' : time.strftime('%Y-%m-%d', time.localtime())}
                        if actionopt.get('focus') == 'no':
                                linestosend.append('<internal name="nofocus"></internal>')

                        # 1/4
                        # fill a dict with the appropriate values + set the concerned users' list
                        if where == 'outgoing':
                                exten = event.get('Extension')
                                application = event.get('Application')
                                if application == 'Dial' and exten.isdigit():
                                        pass

                        elif where == 'agentselected':
                                dst = event.get('Channel2')[6:]
                                src = event.get('CallerID1')
                                chan = event.get('Channel1')
                                queuename = extraevent.get('xivo_queuename')

                                itemdir['xivo-channel'] = chan
                                itemdir['xivo-queuename'] = queuename
                                itemdir['xivo-callerid'] = src
                                itemdir['xivo-agentid'] = dst

                                for uinfo in self.ulist_ng.userlist.itervalues():
                                        if 'agentnum' in uinfo and uinfo.get('agentnum') == agentnum:
                                                userinfos.append(uinfo)
                                                break

                        elif where == 'agi':
                                r_caller = extraevent.get('caller_num')
                                r_called = extraevent.get('called_num')
                                for uinfo in self.ulist_ng.userlist.itervalues():
                                        if uinfo.get('astid') == astid and uinfo.get('phonenum') == r_called:
                                                userinfos.append(uinfo)
                                itemdir['xivo-callerid'] = r_caller
                                itemdir['xivo-calledid'] = r_called
                                itemdir['xivo-tomatch-callerid'] = r_caller
                                itemdir['xivo-channel'] = extraevent.get('channel')
                                itemdir['xivo-uniqueid'] = extraevent.get('uniqueid')

                                linestosend.append('<internal name="called"><![CDATA[%s]]></internal>' % r_called)

                        elif where == 'link':
                                itemdir['xivo-channel'] = event.get('Channel1')
                                itemdir['xivo-channelpeer'] = event.get('Channel2')
                                itemdir['xivo-uniqueid'] = event.get('Uniqueid1')
                                itemdir['xivo-callerid'] = event.get('CallerID1')
                                itemdir['xivo-calledid'] = event.get('CallerID2')
                                for uinfo in self.ulist_ng.userlist.itervalues():
                                        if uinfo.get('astid') == astid and uinfo.get('phonenum') == itemdir['xivo-calledid']:
                                                userinfos.append(uinfo)

                        elif where == 'unlink':
                                itemdir['xivo-channel'] = event.get('Channel1')
                                itemdir['xivo-channelpeer'] = event.get('Channel2')
                                itemdir['xivo-uniqueid'] = event.get('Uniqueid1')
                                itemdir['xivo-callerid'] = event.get('CallerID1')
                                itemdir['xivo-calledid'] = event.get('CallerID2')
                                for uinfo in self.ulist_ng.userlist.itervalues():
                                        if uinfo.get('astid') == astid and uinfo.get('phonenum') == itemdir['xivo-calledid']:
                                                userinfos.append(uinfo)

                        elif where == 'hangup':
                                # print where, event
                                itemdir['xivo-channel'] = event.get('Channel')
                                itemdir['xivo-uniqueid'] = event.get('Uniqueid')
                                # find which userinfo's to contact ...

                        elif where == 'incomingdid':
                                chan = event.get('CHANNEL', '')
                                uid = event.get('UNIQUEID', '')
                                clid = event.get('XIVO_SRCNUM')
                                did  = event.get('XIVO_EXTENPATTERN')
                                
                                print 'ALERT %s %s (%s) uid=%s %s %s did=%s' % (astid, where, time.asctime(),
                                                                                uid, clid, chan, did)
                                self.chans_incomingdid.append(chan)

                                itemdir['xivo-channel'] = chan
                                itemdir['xivo-uniqueid'] = uid
                                itemdir['xivo-did'] = did
                                itemdir['xivo-callerid'] = clid
                                if len(clid) > 7 and clid != '<unknown>':
                                        itemdir['xivo-tomatch-callerid'] = clid
                                linestosend.append('<internal name="uniqueid"><![CDATA[%s]]></internal>' % uid)

                                # userinfos.append() : users matching the SDA ?

                                # interception :
                                # self.amilist.execute(astid, 'transfer', chan, shortphonenum, 'default')
                                
                        elif where == 'incomingqueue':
                                clid = event.get('CallerID')
                                chan = event.get('Channel')
                                uid = event.get('Uniqueid')
                                queue = event.get('Queue')
                                # userinfos.append() # find who are the queue memebers
                                print 'ALERT %s %s (%s) uid=%s %s %s queue=(%s %s %s)' % (astid, where, time.asctime(), uid, clid, chan,
                                                                                          queue, event.get('Position'), event.get('Count'))
                                self.chans_incomingqueue.append(chan)
                                
                                itemdir['xivo-channel'] = chan
                                itemdir['xivo-uniqueid'] = uid
                                itemdir['xivo-queuename'] = queue
                                itemdir['xivo-callerid'] = clid
                                if len(clid) > 7 and clid != '<unknown>':
                                        itemdir['xivo-tomatch-callerid'] = clid

                        # 2/4
                        # call a database for xivo-callerid matching (or another pattern to set somewhere)
                        dirlist = actionopt.get('directories')
                        if 'xivo-tomatch-callerid' in itemdir:
                                callingnum = itemdir['xivo-tomatch-callerid']
                                if dirlist is not None:
                                        for dirname in dirlist.split(','):
                                                if context in self.ctxlist.ctxlist and dirname in self.ctxlist.ctxlist[context]:
                                                        dirdef = self.ctxlist.ctxlist[context][dirname]
                                                        try:
                                                                y = self.__build_customers_bydirdef__(dirname, [callingnum], dirdef)
                                                        except Exception, exc:
                                                                log_debug(SYSLOG_ERR, '--- exception --- (xivo-tomatch-callerid : %s, %s) : %s'
                                                                          % (dirname, context, exc))
                                                                y = []
                                                        if len(y) > 0:
                                                                for g, gg in y[0].iteritems():
                                                                        itemdir[g] = gg
                                if callingnum[:2] == '00':
                                        internatprefix = callingnum[2:6]
                        # print itemdir

                        # 3/4
                        # build XML items from daemon-config + filled-in items
                        if 'xivo-channel' in itemdir:
                                linestosend.append('<internal name="channel"><![CDATA[%s]]></internal>'
                                                   % itemdir['xivo-channel'])
                        if 'xivo-uniqueid' in itemdir:
                                linestosend.append('<internal name="sessionid"><![CDATA[%s]]></internal>'
                                                   % itemdir['xivo-uniqueid'])
                        linestosend.extend(self.__build_xmlsheet__('action_info', actionopt, itemdir))
                        linestosend.extend(self.__build_xmlsheet__('sheet_info', actionopt, itemdir))
                        linestosend.extend(self.__build_xmlsheet__('systray_info', actionopt, itemdir))
                        linestosend.append('<internal name="kind"><![CDATA[%s]]></internal>' % where)
                        linestosend.append('</user></profile>')
                        fulllines = ''.join(linestosend)

                        # print '---------', where, whoms, fulllines

                        # 4/4
                        # send the payload to the appropriate people
                        for whom in whoms.split(','):
                                if whom == 'dest':
                                        for userinfo in userinfos:
                                                self.__send_msg_to_cti_client__(userinfo, fulllines)
                                elif whom == 'subscribe':
                                        for uinfo in self.ulist_ng.userlist.itervalues():
                                                if 'subscribe' in uinfo:
                                                        self.__send_msg_to_cti_client__(uinfo, fulllines)
                                elif whom == 'all':
                                        for uinfo in self.ulist_ng.userlist.itervalues():
                                                if astid == uinfo.get('astid'):
                                                        self.__send_msg_to_cti_client__(uinfo, fulllines)
                                elif whom == 'reallyall':
                                        for uinfo in self.ulist_ng.userlist.itervalues():
                                                self.__send_msg_to_cti_client__(uinfo, fulllines)
                                else:
                                        log_debug(SYSLOG_WARNING, '__sheet_alert__ (%s) : unknown destination <%s> in <%s>'
                                                  % (astid, whom, where))
                return


        # Methods to handle Asterisk AMI events
        def ami_dial(self, astid, event):
                src     = event.get('Source')
                dst     = event.get('Destination')
                clid    = event.get('CallerID')
                clidn   = event.get('CallerIDName')
                context = event.get('Context')
                self.plist[astid].handle_ami_event_dial(src, dst, clid, clidn)
                return


        def __clidlist_from_event__(self, chan1, chan2, clid1, clid2):
                # backport from autoescape, to be checked/improved/fixed
                LOCALNUMSIZE = 3
                clidlist = []
                if len(clid1) == LOCALNUMSIZE and clid1 not in clidlist:
                        clidlist.append(clid1)
                if len(clid2) == LOCALNUMSIZE and clid2 not in clidlist:
                        clidlist.append(clid2)
                if chan1.startswith('SIP/'):
                        num1 = chan1[4:].split('-')[0]
                        if len(num1) == LOCALNUMSIZE and num1 not in clidlist:
                                clidlist.append(num1)
                if chan2.startswith('SIP/'):
                        num2 = chan2[4:].split('-')[0]
                        if len(num2) == LOCALNUMSIZE and num2 not in clidlist:
                                clidlist.append(num2)
                return clidlist


        def ami_link(self, astid, event):
                chan1 = event.get('Channel1')
                chan2 = event.get('Channel2')
                clid1 = event.get('CallerID1')
                clid2 = event.get('CallerID2')
                self.__sheet_alert__('link', astid, DEFAULTCONTEXT, event, {})
                if chan2.startswith('Agent/'):
                        msg = self.__build_agupdate__(['agentlink', astid, chan2])
                        self.__send_msg_to_cti_clients__(msg)
                        # To identify which queue a call comes from, we match a previous AMI Leave event,
                        # that involved the same channel as the one catched here.
                        # Any less-tricky-method is welcome, though.
                        if chan1 in self.queues_channels_list[astid]:
                                qname = self.queues_channels_list[astid][chan1]
                                del self.queues_channels_list[astid][chan1]
                                extraevent = {'xivo_queuename' : qname}
                                self.__sheet_alert__('agentselected', astid, DEFAULTCONTEXT, event, extraevent)
                else:
                        ag1 = None
                        ag2 = None
                        for uinfo in self.ulist_ng.userlist.itervalues():
                                if uinfo.get('astid') == astid:
                                        if uinfo.get('phonenum') == event.get('CallerID1'):
                                                ag1 = uinfo.get('agentnum')
                                        if uinfo.get('phonenum') == event.get('CallerID2'):
                                                ag2 = uinfo.get('agentnum')
                        if ag1 is not None:
                                msg = self.__build_agupdate__(['phonelink', astid, 'Agent/%s' % ag1])
                                self.__send_msg_to_cti_clients__(msg)
                        if ag2 is not None:
                                msg = self.__build_agupdate__(['phonelink', astid, 'Agent/%s' % ag2])
                                self.__send_msg_to_cti_clients__(msg)
                self.plist[astid].handle_ami_event_link(chan1, chan2, clid1, clid2)

                return

        def ami_unlink(self, astid, event):
                chan1 = event.get('Channel1')
                chan2 = event.get('Channel2')
                clid1 = event.get('CallerID1')
                clid2 = event.get('CallerID2')
                self.__sheet_alert__('unlink', astid, DEFAULTCONTEXT, event, {})
                if chan2.startswith('Agent/'):
                        msg = self.__build_agupdate__(['agentunlink', astid, chan2])
                        self.__send_msg_to_cti_clients__(msg)
                else:
                        ag1 = None
                        ag2 = None
                        for uinfo in self.ulist_ng.userlist.itervalues():
                                if uinfo.get('astid') == astid:
                                        if uinfo.get('phonenum') == event.get('CallerID1'):
                                                ag1 = uinfo.get('agentnum')
                                        if uinfo.get('phonenum') == event.get('CallerID2'):
                                                ag2 = uinfo.get('agentnum')
                        if ag1 is not None:
                                msg = self.__build_agupdate__(['phoneunlink', astid, 'Agent/%s' % ag1])
                                self.__send_msg_to_cti_clients__(msg)
                        if ag2 is not None:
                                msg = self.__build_agupdate__(['phoneunlink', astid, 'Agent/%s' % ag2])
                                self.__send_msg_to_cti_clients__(msg)
                self.plist[astid].handle_ami_event_unlink(chan1, chan2, clid1, clid2)
                return

        def ami_hangup(self, astid, event):
                chan  = event.get('Channel')
                uid = event.get('Uniqueid')
                cause = event.get('Cause-txt')
                self.__sheet_alert__('hangup', astid, DEFAULTCONTEXT, event)
                self.plist[astid].handle_ami_event_hangup(chan, cause)
                if chan in self.chans_incomingqueue or chan in self.chans_incomingdid:
                        print 'HANGUP : (%s) %s uid=%s %s' % (time.asctime(), astid, uid, chan)
                        if chan in self.chans_incomingqueue:
                                self.chans_incomingqueue.remove(chan)
                        if chan in self.chans_incomingdid:
                                self.chans_incomingdid.remove(chan)
                return

        def amiresponse_success(self, astid, event):
                msg = event.get('Message')
                if msg == 'Extension Status':
                        self.amiresponse_extensionstatus(astid, event)
                elif msg == 'Mailbox Message Count':
                        self.amiresponse_mailboxcount(astid, event)
                elif msg == 'Mailbox Status':
                        self.amiresponse_mailboxstatus(astid, event)
                elif msg in ['Channel status will follow',
                             'Parked calls will follow',
                             'Agents will follow',
                             'Queue status will follow',
                             'Authentication accepted',
                             'Variable Set',
                             'Attended transfer started',
                             'Channel Hungup',
                             'Originate successfully queued',
                             'Redirect successful',
                             'Added interface to queue',
                             'Removed interface from queue',
                             'Interface paused successfully',
                             'Interface unpaused successfully',
                             'Agent logged out',
                             'Agent logged in']:
                        pass
                else:
                        log_debug(SYSLOG_WARNING, 'AMI %s Response=Success : untracked message <%s>' % (astid, msg))
                return

        def amiresponse_error(self, astid, event):
                log_debug(SYSLOG_WARNING, 'AMI %s Response=Error : %s' % (astid, event))
                return

        def amiresponse_mailboxcount(self, astid, event):
                exten = event.get('Mailbox').split('@')[0]
                for userinfo in self.ulist_ng.userlist.itervalues():
                        if 'phonenum' in userinfo and userinfo.get('phonenum') == exten and userinfo.get('astid') == astid:
                                userinfo['mwi-new'] = event.get('NewMessages')
                                userinfo['mwi-old'] = event.get('OldMessages')
                return

        def amiresponse_mailboxstatus(self, astid, event):
                exten = event.get('Mailbox').split('@')[0]
                for userinfo in self.ulist_ng.userlist.itervalues():
                        if 'phonenum' in userinfo and userinfo.get('phonenum') == exten and userinfo.get('astid') == astid:
                                userinfo['mwi-waiting'] = event.get('Waiting')
                                tosend = { 'class' : 'users-list-update',
                                           'direction' : 'client',
                                           'payload' : [userinfo.get('company'),
                                                        userinfo.get('user'),
                                                        'mwi',
                                                        userinfo.get('mwi-waiting'),
                                                        userinfo.get('mwi-old'),
                                                        userinfo.get('mwi-new')] }
                                self.__send_msg_to_cti_clients__(cjson.encode(tosend))
                return

        def amiresponse_extensionstatus(self, astid, event):
                # 90 seconds are needed to retrieve ~ 9000 phone statuses from an asterisk (on daemon startup)
                status  = event.get('Status')
                hint    = event.get('Hint')
                context = event.get('Context')
                exten   = event.get('Exten')
                plist_thisast = self.plist[astid]
                if hint in plist_thisast.normal:
                        normv = plist_thisast.normal[hint]
                        normv.set_lasttime(time.time())
                        sippresence = 'Timeout'
                        if status == '-1':
                                sippresence = 'Fail'
                        elif status == '0':
                                sippresence = 'Ready'
                        elif status == '1':
                                sippresence = 'On the phone'
                        elif status == '4':
                                sippresence = 'Unavailable'
                        elif status == '8':
                                sippresence = 'Ringing'
                        normv.set_hintstatus(sippresence)
                        plist_thisast.update_gui_clients(hint, "SIP-NTFY")
                return


        def ami_extensionstatus(self, astid, event):
                exten   = event.get('Exten')
                status  = event.get('Status')
                sipphone = 'SIP/%s' % exten
                plist_thisast = self.plist[astid]
                if sipphone in plist_thisast.normal:
                        normv = plist_thisast.normal[sipphone]
                        normv.set_lasttime(time.time())
                        sippresence = 'Timeout'
                        if status == '-1':
                                sippresence = 'Fail'
                        elif status == '0':
                                sippresence = 'Ready'
                        elif status == '1':
                                sippresence = 'On the phone'
                        elif status == '4':
                                sippresence = 'Unavailable'
                        elif status == '8':
                                sippresence = 'Ringing'
                        normv.set_hintstatus(sippresence)
                        plist_thisast.update_gui_clients(sipphone, "SIP-NTFY")
                return

        def ami_channelreload(self, astid, event):
                # Asterisk 1.4 event
                print astid, event
                # {'User_Count': '12', 'Peer_Count': '12', 'Registry_Count': '0', 'Privilege': 'system,all', 'Event': 'ChannelReload', 'Channel': 'SIP', 'ReloadReason': 'RELOAD (Channel module reload)'}
                return
        
        def ami_aoriginatesuccess(self, astid, event):
                return
        def ami_originatesuccess(self, astid, event):
                return
        def ami_aoriginatefailure(self, astid, event):
                return
        def ami_originatefailure(self, astid, event):
                return
        
        def ami_originateresponse(self, astid, event):
                return
        # {'Uniqueid': '1213955764.88', 'CallerID': '6101', 'Exten': '6101', 'CallerIDNum': '6101', 'Response': 'Success', 'Reason': '4', 'Context': 'ctx-callbooster-agentlogin', 'CallerIDName': 'operateur', 'Privilege': 'call,all', 'Event': 'OriginateResponse', 'Channel': 'SIP/102-081f6730'}

        def ami_messagewaiting(self, astid, event):
                exten = event.get('Mailbox').split('@')[0]
                context = event.get('Mailbox').split('@')[1]
                # instead of updating the mwi fields here, we request the current status,
                # since the event returned when someone has erased one's mailbox seems to be false,
                # or incomplete at least
                self.amilist.execute(astid, 'mailbox', exten, context)
                return

        def ami_newcallerid(self, astid, event):
                return

        def ami_newexten(self, astid, event):
                self.__sheet_alert__('outgoing', astid, event.get('Context'), event)
                return

        def ami_newchannel(self, astid, event):
                return

        def ami_parkedcall(self, astid, event):
                channel = event.get('Channel')
                cfrom   = event.get('From')
                exten   = event.get('Exten')
                timeout = event.get('Timeout')
                tosend = { 'class' : 'parkcall',
                           'direction' : 'client',
                           'payload' : { 'status' : 'parkedcall',
                                         'args' : [astid, channel, cfrom, exten, timeout]}}
                self.__send_msg_to_cti_clients__(cjson.encode(tosend))
                return
        
        def ami_unparkedcall(self, astid, event):
                channel = event.get('Channel')
                cfrom   = event.get('From')
                exten   = event.get('Exten')
                tosend = { 'class' : 'parkcall',
                           'direction' : 'client',
                           'payload' : { 'status' : 'unparkedcall',
                                         'args' : [astid, channel, cfrom, exten]}}
                self.__send_msg_to_cti_clients__(cjson.encode(tosend))
                return
        
        def ami_parkedcallgiveup(self, astid, event):
                channel = event.get('Channel')
                exten   = event.get('Exten')
                tosend = { 'class' : 'parkcall',
                           'direction' : 'client',
                           'payload' : { 'status' : 'parkedcallgiveup',
                                         'args' : [astid, channel, exten]}}
                self.__send_msg_to_cti_clients__(cjson.encode(tosend))
                return
        
        def ami_parkedcalltimeout(self, astid, event):
                channel = event.get('Channel')
                exten   = event.get('Exten')
                tosend = { 'class' : 'parkcall',
                           'direction' : 'client',
                           'payload' : { 'status' : 'parkedcalltimeout',
                                         'args' : [astid, channel, exten]}}
                self.__send_msg_to_cti_clients__(cjson.encode(tosend))
                return
        
        def ami_agentlogin(self, astid, event):
                print 'AMI AgentLogin', astid, event
                return
        def ami_agentlogoff(self, astid, event):
                print 'AMI AgentLogoff', astid, event
                return

        def ami_agentcallbacklogin(self, astid, event):
                agent = event.get('Agent')
                loginchan = event.get('Loginchan')
                if astid in self.agents_list and agent in self.agents_list[astid]:
                        self.agents_list[astid][agent]['status'] = 'AGENT_IDLE'
                        self.agents_list[astid][agent]['phonenum'] = event.get('Loginchan')
                msg = self.__build_agupdate__(['agentlogin', astid, 'Agent/%s' % agent, loginchan])
                print 'ami_agentcallbacklogin', msg
                self.__send_msg_to_cti_clients__(msg)
                return

        def ami_agentcallbacklogoff(self, astid, event):
                agent = event.get('Agent')
                loginchan = event.get('Loginchan')
                if loginchan == 'n/a':
                        loginchan = ''
                if astid in self.agents_list and agent in self.agents_list[astid]:
                        self.agents_list[astid][agent]['status'] = 'AGENT_LOGGEDOFF'
                        self.agents_list[astid][agent]['phonenum'] = loginchan
                msg = self.__build_agupdate__(['agentlogout', astid, 'Agent/%s' % agent, loginchan])
                print 'ami_agentcallbacklogoff', msg
                self.__send_msg_to_cti_clients__(msg)
                return

        def ami_agentcalled(self, astid, event):
                print 'AMI AgentCalled', astid, event
                # {'Extension': 's', 'CallerID': 'unknown', 'Priority': '2', 'ChannelCalling': 'IAX2/test-13', 'Context': 'macro-incoming_queue_call', 'CallerIDName': 'Comm. ', 'AgentCalled': 'iax2/192.168.0.120/101'}

                return
        def ami_agentcomplete(self, astid, event):
                print 'AMI AgentComplete', astid, event
                return
        def ami_agentdump(self, astid, event):
                print 'AMI AgentDump', astid, event
                return
        def ami_agentconnect(self, astid, event):
                print 'AMI AgentConnect', astid, event
                # {'Member': 'SIP/108', 'Queue': 'commercial', 'Uniqueid': '1215006134.1166', 'Privilege': 'agent,all', 'Holdtime': '9', 'Event': 'AgentConnect', 'Channel': 'SIP/108-08190098'}
                return

        def ami_agents(self, astid, event):
                agent = event.get('Agent')
                # TalkingTo ?
                if astid not in self.agents_list:
                        self.agents_list[astid] = {}
                if agent not in self.agents_list[astid]:
                        linchan = event.get('LoggedInChan')
                        if linchan == 'n/a':
                                linchan = ''
                        self.agents_list[astid][agent] = {}
                        self.agents_list[astid][agent]['status'] = event.get('Status')
                        self.agents_list[astid][agent]['phonenum'] = linchan
                        self.agents_list[astid][agent]['name'] = event.get('Name')
                        self.agents_list[astid][agent]['loggedintime'] = event.get('LoggedInTime')
                return

        def ami_agentscomplete(self, astid, event):
                print 'AMI AgentsComplete', astid, event
                if astid in self.agents_list:
                        for aname, aargs in self.agents_list[astid].iteritems():
                                print ' (a) ', aname, aargs
                return

        # XIVO-WEBI: beg-data
        # "category"|"name"|"number"|"context"|"commented"
        # "queue"|"callcenter"|"330"|"proformatique"|"0"
        # ""|""|""|""|"0"
        # XIVO-WEBI: end-data
        def ami_queuecallerabandon(self, astid, event):
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_queuecallerabandon : no queue list has been defined for %s' % astid)
                        return
                # Asterisk 1.4 event
                # {'Queue': 'qcb_00000', 'OriginalPosition': '1', 'Uniqueid': '1213891256.41', 'Privilege': 'agent,all', 'Position': '1', 'HoldTime': '2', 'Event': 'QueueCallerAbandon'}
                return
        
        def ami_queueentry(self, astid, event):
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_queueentry : no queue list has been defined for %s' % astid)
                        return
                queue = event.get('Queue')
                position = event.get('Position')
                wait = event.get('Wait')
                channel = event.get('Channel')
                # print 'AMI QueueEntry', astid, queue, position, wait, channel, event
                self.qlist[astid].queueentry_update(queue, channel, position, wait)
                self.__send_msg_to_cti_clients__(self.__build_queue_status__(astid, queue))
                return
        
        def ami_queuememberadded(self, astid, event):
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_queuememberadded : no queue list has been defined for %s' % astid)
                        return
                queue = event.get('Queue')
                location = event.get('Location')
                paused = event.get('Paused')
                self.qlist[astid].queuememberupdate(queue, location, event)
                msg = self.__build_agupdate__(['joinqueue', astid, location, queue, paused])
                self.__send_msg_to_cti_clients__(msg)
                return
        
        def ami_queuememberremoved(self, astid, event):
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_queuememberremoved : no queue list has been defined for %s' % astid)
                        return
                queue = event.get('Queue')
                location = event.get('Location')
                self.qlist[astid].queuememberremove(queue, location)
                msg = self.__build_agupdate__(['leavequeue', astid, location, queue])
                self.__send_msg_to_cti_clients__(msg)
                return
        
        def __build_agupdate__(self, arrgs):
                tosend = { 'class' : 'update-agents',
                           'direction' : 'client',
                           'payload' : arrgs }
                return cjson.encode(tosend)
        
        def ami_queuememberstatus(self, astid, event):
                print 'AMI_QUEUEMEMBERSTATUS', event
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_queuememberstatus : no queue list has been defined for %s' % astid)
                        return
                status = event.get('Status')
                queue = event.get('Queue')
                location = event.get('Location')
                paused = event.get('Paused')
                self.qlist[astid].queuememberupdate(queue, location, event)
                msg = self.__build_agupdate__(['queuememberstatus', astid, location, queue, status, paused])
                self.__send_msg_to_cti_clients__(msg)
                
                # status = 3 => ringing
                # status = 1 => do not ring anymore => the one who has not gone to '1' among the '3's is the one who answered ...
                # 5 is received when unavailable members of a queue are attempted to be joined ... use agentcallbacklogoff to detect exit instead
                # + Link
                return

        def ami_queuememberpaused(self, astid, event):
                print 'AMI_QUEUEMEMBERPAUSED', event
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_queuememberpaused : no queue list has been defined for %s' % astid)
                        return
                queue = event.get('Queue')
                paused = event.get('Paused')
                location = event.get('Location')
                self.qlist[astid].queuememberupdate(queue, location, event)
                if location.startswith('Agent/'):
                        if paused == '0':
                                msg = self.__build_agupdate__(['unpaused', astid, location, queue])
                                self.__send_msg_to_cti_clients__(msg)
                        else:
                                msg = self.__build_agupdate__(['paused', astid, location, queue])
                                self.__send_msg_to_cti_clients__(msg)
                return

        def ami_queueparams(self, astid, event):
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_queueparams : no queue list has been defined for %s' % astid)
                        return
                queue = event.get('Queue')
                self.qlist[astid].update_queuestats(queue, event)
                tosend = { 'class' : 'queues-list',
                           'direction' : 'client',
                           'payload' : { 'astid' : astid,
                                         'queuestats' : self.qlist[astid].get_queuestats(queue) } }
                self.__send_msg_to_cti_clients__(cjson.encode(tosend))
                return

        def ami_queuemember(self, astid, event):
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_queuemember : no queue list has been defined for %s' % astid)
                        return
                queue = event.get('Queue')
                location = event.get('Location')
                self.qlist[astid].queuememberupdate(queue, location, event)
                return

        def ami_queuestatuscomplete(self, astid, event):
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_queuestatuscomplete : no queue list has been defined for %s' % astid)
                        return
                print 'AMI QueueStatusComplete', astid
                for qname in self.qlist[astid].get_queues():
                        self.amilist.execute(astid, 'sendcommand', 'Command', [('Command', 'show queue %s' % qname)])
                return

        def ami_userevent(self, astid, event):
                eventname = event.get('UserEvent')
                if eventname == 'DID':
                        self.__sheet_alert__('incomingdid', astid,
                                             event.get('XIVO_CONTEXT', DEFAULTCONTEXT),
                                             event)
                elif eventname == 'Feature':
                        log_debug(SYSLOG_INFO, 'AMI %s UserEventFeature %s' % (astid, event))
                        # 'XIVO_CONTEXT', 'CHANNEL', 'Function', 'Status', 'Value'
                else:
                        log_debug(SYSLOG_INFO, 'AMI %s UserEvent %s' % (astid, event))
                return

        def ami_faxsent(self, astid, event):
                filename = event.get('FileName')
                # debug# (xivocti) xivo-obelisk : {'PhaseEString': 'OK', 'CallerID': '00141389970', 'Exten': 's', 'LocalStationID': '', 'PagesTransferred': '1', 'TransferRate': '14400', 'RemoteStationID': '', 'PhaseEStatus': '0', 'Privilege': 'call,all', 'FileName': '/var/spool/asterisk/fax/astfaxsend-q6yZAKTJvU-0x48be7930.tif', 'Resolution': '3850', 'Event': 'FaxSent', 'Channel': 'IAX2/asteriskisdn-1363'}
                
                if filename and os.path.isfile(filename):
                        os.unlink(filename)
                        log_debug(SYSLOG_INFO, 'faxsent event handler : removed %s' % filename)

                if event.get('phaseestatus') == '0':
                        tosend = { 'class' : 'faxsent',
                                   'direction' : 'client',
                                   'payload' : 'ok;' }
                else:
                        tosend = { 'class' : 'faxsent',
                                   'direction' : 'client',
                                   'payload' : 'ko;%s' % event.get('phaseestring', 'Unknown') }
                repstr = cjson.encode(tosend)
                # TODO: Send the result to XIVO Client.
                return

        def ami_faxreceived(self, astid, event):
                log_debug(SYSLOG_INFO, '%s : %s' % (astid, event))
                # debug# (xivocti) xivo-obelisk : {'PhaseEString': 'OK', 'CallerID': '0141389960', 'Exten': 's', 'LocalStationID': '', 'PagesTransferred': '1', 'TransferRate': '14400', 'RemoteStationID': '', 'PhaseEStatus': '0', 'Privilege': 'call,all', 'FileName': '/var/spool/asterisk/fax/0141389960-1220442150.tif', 'Resolution': '3850', 'Event': 'FaxReceived', 'Channel': 'IAX2/asteriskisdn-7665'}
                return

        def ami_meetmejoin(self, astid, event):
                meetmenum = event.get('Meetme')
                channel = event.get('Channel')
                num = event.get('Usernum')
                
                if astid not in self.meetme:
                        self.meetme[astid] = {}
                if meetmenum not in self.meetme[astid]:
                        self.meetme[astid][meetmenum] = []
                if channel not in self.meetme[astid][meetmenum]:
                        self.meetme[astid][meetmenum].append(channel)
                        tosend = { 'class' : 'meetme',
                                   'direction' : 'client',
                                   'payload' : [ 'join', meetmenum, num, channel,
                                                 str(len(self.meetme[astid][meetmenum])) ]
                                   }
                        self.__send_msg_to_cti_clients__(cjson.encode(tosend))
                else:
                        log_debug(SYSLOG_WARNING, '%s : channel %s already in meetme %s' % (astid, channel, meetmenum))
                return

        def ami_meetmeleave(self, astid, event):
                meetmenum = event.get('Meetme')
                channel = event.get('Channel')
                num = event.get('Usernum')
                
                if astid not in self.meetme:
                        self.meetme[astid] = {}
                if meetmenum not in self.meetme[astid]:
                        self.meetme[astid][meetmenum] = []
                if channel in self.meetme[astid][meetmenum]:
                        self.meetme[astid][meetmenum].remove(channel)
                        self.__send_msg_to_cti_clients__('meetme=leave;%s;%s;%s;%s;%d'
                                                         % (astid, meetmenum, num, channel, len(self.meetme[astid][meetmenum])))
                else:
                        log_debug(SYSLOG_WARNING, '%s : channel %s not in meetme %s' % (astid, channel, meetmenum))
                return

        def ami_status(self, astid, event):
                return

        def ami_join(self, astid, event):
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_join : no queue list has been defined for %s' % astid)
                        return
                # print 'AMI Join (Queue)', event
                chan  = event.get('Channel')
                clid  = event.get('CallerID')
                queue = event.get('Queue')
                count = event.get('Count')
                position = event.get('Position')
                self.__sheet_alert__('incomingqueue', astid, DEFAULTCONTEXT, event)
                log_debug(SYSLOG_INFO, 'AMI Join (Queue) %s %s %s' % (queue, chan, count))
                self.qlist[astid].queueentry_update(queue, chan, position, '0')
                event['Calls'] = count
                self.qlist[astid].update_queuestats(queue, event)
                tosend = { 'class' : 'update-queues',
                           'direction' : 'client',
                           'payload' : 'queuechannels;%s;%s;%s' % (astid, queue, count) }
                self.__send_msg_to_cti_clients__(cjson.encode(tosend))
                self.amilist.execute(astid, 'sendqueuestatus', queue)
                self.__send_msg_to_cti_clients__(self.__build_queue_status__(astid, queue))
                return

        def ami_leave(self, astid, event):
                if astid not in self.qlist:
                        log_debug(SYSLOG_WARNING, 'ami_leave : no queue list has been defined for %s' % astid)
                        return
                # print 'AMI Leave (Queue)', event
                chan  = event.get('Channel')
                queue = event.get('Queue')
                count = event.get('Count')
                log_debug(SYSLOG_INFO, 'AMI Leave (Queue) %s %s %s' % (queue, chan, count))
                
                self.qlist[astid].queueentry_remove(queue, chan)
                event['Calls'] = count
                self.qlist[astid].update_queuestats(queue, event)
                tosend = { 'class' : 'update-queues',
                           'direction' : 'client',
                           'payload' : 'queuechannels;%s;%s;%s' % (astid, queue, count) }
                self.__send_msg_to_cti_clients__(cjson.encode(tosend))
                
                if astid not in self.queues_channels_list:
                        self.queues_channels_list[astid] = {}
                # always sets the queue information since it might not have been deleted
                self.queues_channels_list[astid][chan] = queue
                self.amilist.execute(astid, 'sendqueuestatus', queue)
                self.__send_msg_to_cti_clients__(self.__build_queue_status__(astid, queue))
                return

        def ami_rename(self, astid, event):
                print 'AMI Rename', event.get('Uniqueid'), event.get('Oldname'), event.get('Newname')
                return
        # END of AMI events

        def message_srv2clt(self, sender, message):
                tosend = { 'class' : 'message',
                           'direction' : 'client',
                           'payload' : [sender, message] }
                return cjson.encode(tosend)

        def dmessage_srv2clt(self, message):
                return self.message_srv2clt('daemon-announce', message)


        def phones_update(self, function, args):
                strupdate = ''
                if function == 'update':
                        tosend = { 'class' : 'phones',
                                   'function' : 'update',
                                   'direction' : 'client',
                                   'payload' : args }
                        strupdate = cjson.encode(tosend)
                elif function == 'noupdate':
                        tosend = { 'class' : 'phones',
                                   'function' : 'noupdate',
                                   'direction' : 'client',
                                   'payload' : args }
                        strupdate = cjson.encode(tosend)
                elif function == 'signal-deloradd':
                        [astid, ndel, nadd, ntotal] = args
                        tosend = { 'class' : 'phones',
                                   'function' : 'signal-deloradd',
                                   'direction' : 'client',
                                   'payload' : [astid, str(ndel), str(nadd), str(ntotal)] }
                        strupdate = cjson.encode(tosend)
                self.__send_msg_to_cti_clients__(strupdate)
                return


        def manage_cticommand(self, userinfo, icommand):
                ret = None
                repstr = None
                astid    = userinfo.get('astid')
                username = userinfo.get('user')
                context  = userinfo.get('context')
                capaid   = userinfo.get('capaid')
                ucapa    = self.capas[capaid].all() # userinfo.get('capas')
                
                try:
                        if icommand.name == 'history':
                                if self.capas[capaid].match_funcs(ucapa, 'history'):
                                        repstr = self.__build_history_string__(icommand.args[0],
                                                                               icommand.args[1],
                                                                               icommand.args[2])
                        elif icommand.name == 'directory-search':
                                if self.capas[capaid].match_funcs(ucapa, 'directory'):
                                        repstr = self.__build_customers__(context, icommand.args)

                        elif icommand.name == 'availstate':
                                if self.capas[capaid].match_funcs(ucapa, 'presence'):
                                        repstr = self.__update_availstate__(userinfo, icommand.args[0])
                        elif icommand.name == 'database':
                                if self.capas[capaid].match_funcs(ucapa, 'database'):
                                        repstr = database_update(me, icommand.args)
                        elif icommand.name == 'featuresget':
                                if self.capas[capaid].match_funcs(ucapa, 'features'):
##                                        if username in userlist[astid]:
##                                                userlist[astid][username]['monit'] = icommand.args
                                        repstr = self.__build_features_get__(icommand.args)
                        elif icommand.name == 'featuresput':
                                if self.capas[capaid].match_funcs(ucapa, 'features'):
                                        repstr = self.__build_features_put__(icommand.args)
                        elif icommand.name == 'faxsend':
                                if self.capas[capaid].match_funcs(ucapa, 'fax'):
                                        newfax = cti_fax.Fax(userinfo, icommand.args)
                                        self.faxes[newfax.reference] = newfax
                                        tosend = { 'class' : 'faxsend',
                                                   'direction' : 'client',
                                                   'payload' : newfax.reference }
                                        repstr = cjson.encode(tosend)
                        elif icommand.name == 'message':
                                if self.capas[capaid].match_funcs(ucapa, 'messages'):
                                        self.__send_msg_to_cti_clients__(self.message_srv2clt('%s/%s' %(astid, username),
                                                                                             '<%s>' % icommand.args[0]))
                        elif icommand.name == 'json':
                                classcomm = icommand.struct.get('class')
                                dircomm = icommand.struct.get('direction')
                                argums = icommand.struct.get('command').split()
                                
                                if dircomm is not None and dircomm == 'xivoserver':
                                        if classcomm == 'meetme':
                                                if self.capas[capaid].match_funcs(ucapa, 'conference'):
                                                        if argums[0] == 'kick':
                                                                astid = argums[1]
                                                                room = argums[2]
                                                                num = argums[3]
                                                                self.amilist.execute(astid, 'sendcommand',
                                                                                     'Command', [('Command', 'meetme kick %s %s' % (room, num))])
                                        elif classcomm == 'callcampaign':
                                                if argums[0] == 'fetchlist':
                                                        tosend = { 'class' : 'callcampaign',
                                                                   'direction' : 'client',
                                                                   'payload' : { 'command' : 'fetchlist',
                                                                                 'list' : [ '101', "102", "103" ] } }
                                                        repstr = cjson.encode(tosend)
                                                elif argums[0] == 'startcall':
                                                        exten = argums[1]
                                                        self.__originate_or_transfer__(userinfo,
                                                                                       ['originate', 'user:special:me', 'ext:%s' % exten])
                                                        tosend = { 'class' : 'callcampaign',
                                                                   'direction' : 'client',
                                                                   'payload' : { 'command' : 'callstarted',
                                                                                 'number' : exten } }
                                                        repstr = cjson.encode(tosend)
                                                elif argums[0] == 'stopcall':
                                                        tosend = { 'class' : 'callcampaign',
                                                                   'direction' : 'client',
                                                                   'payload' : { 'command' : 'callstopped',
                                                                                 'number' : argums[1] } }
                                                        repstr = cjson.encode(tosend)
                                        # self.__send_msg_to_cti_client__(userinfo,
                                        # '{'class':"callcampaign","direction":"client","command":"callnext","list":["%s"]}' % icommand.args[1])

                                        elif classcomm == 'agent':
                                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                                        repstr = self.__agent__(userinfo, argums)

                        elif icommand.name in ['originate', 'transfer', 'atxfer']:
                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                        repstr = self.__originate_or_transfer__(userinfo,
                                                                                [icommand.name, icommand.args[0], icommand.args[1]])
                        elif icommand.name == 'hangup':
                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                        repstr = self.__hangup__(userinfo, icommand.args, True)
                        elif icommand.name == 'simplehangup':
                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                        repstr = self.__hangup__(userinfo, icommand.args, False)
                        elif icommand.name == 'pickup':
                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                        z = icommand.args[0].split('/')
                                        # on Thomson, it picks up the last received call
                                        self.amilist.execute(z[0], 'sendcommand', 'Command', [('Command', 'sip notify event-talk %s' % z[1])])

                        elif icommand.name in ['phones-list', 'phones-add', 'phones-del']:
                                if True: # XXX define when it would not be compulsory
                                        # repstr = self.__build_callerids_hints__(icommand)
                                        if icommand.name == 'phones-list':
                                                repstr = self.__phlist__()

                        elif icommand.name == 'users-list':
                                uinfo = userinfo
                                f = [uinfo.get('user'),
                                     uinfo.get('company'),
                                     uinfo.get('fullname'),
                                     uinfo.get('state'),
                                     str(1),
                                     uinfo.get('astid'),
                                     uinfo.get('context'),
                                     uinfo.get('phonenum'),
                                     uinfo.get('techlist'),
                                     uinfo.get('agentnum'),
                                     uinfo.get('mwi-waiting'),
                                     uinfo.get('mwi-old'),
                                     uinfo.get('mwi-new')]
                                for uinfo in self.ulist_ng.userlist.itervalues():
                                        f.extend([uinfo.get('user'),
                                                  uinfo.get('company'),
                                                  uinfo.get('fullname'),
                                                  uinfo.get('state'),
                                                  str(1), # add/del/update (new fullname)/other ...
                                                  uinfo.get('astid'),
                                                  uinfo.get('context'),
                                                  uinfo.get('phonenum'),
                                                  uinfo.get('techlist'),
                                                  uinfo.get('agentnum'),
                                                  uinfo.get('mwi-waiting'),
                                                  uinfo.get('mwi-old'),
                                                  uinfo.get('mwi-new')])
                                tosend = { 'class' : 'users-list',
                                           'direction' : 'client',
                                           'payload' : f }
                                self.__send_msg_to_cti_client__(userinfo, cjson.encode(tosend))
                                repstr = None

                        elif icommand.name == 'queues-list':
                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                        for astid, qlist in self.qlist.iteritems():
                                                tosend = { 'class' : 'queues-list',
                                                           'direction' : 'client',
                                                           'payload' : { 'astid' : astid,
                                                                         'queuestats' : qlist.get_queuestats_long() } }
                                                self.__send_msg_to_cti_client__(userinfo, cjson.encode(tosend))
                                repstr = None

                        elif icommand.name == 'queue-status':
                                # issued towards a user when he wants to monitor a new queue
                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                        astid = icommand.args[0]
                                        qname = icommand.args[1]
                                        self.__send_msg_to_cti_client__(userinfo, self.__build_queue_status__(astid, qname))
                                repstr = None

                        elif icommand.name == 'agents-list':
                                # issued by one user when he logs in
                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                        for astid, aglist in self.agents_list.iteritems():
                                                if astid in self.qlist:
                                                        lst = []
                                                        for agname, agprop in aglist.iteritems():
                                                                lstatus = '1'
                                                                if agprop['status'] == 'AGENT_LOGGEDOFF':
                                                                        lstatus = '0'
                                                                lst.append('%s:%s:%s:%s:%s' % (agname, lstatus,
                                                                                               agprop['name'],
                                                                                               agprop['phonenum'],
                                                                                               self.qlist[astid].get_queues_byagent('Agent/%s' % agname)))
                                                        tosend = { 'class' : 'agents-list',
                                                                   'direction' : 'client',
                                                                   'payload' : { 'astid' : astid,
                                                                                 'list' : lst
                                                                                 }
                                                                   }
                                                        self.__send_msg_to_cti_client__(userinfo, cjson.encode(tosend))
                                repstr = None

                        elif icommand.name == 'agent-status':
                                # issued by one user when he requests the status for one given agent
                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                        astid = icommand.args[0]
                                        agname = icommand.args[1] # agname = userinfo.get('agentnum')
                                        agid = 'Agent/%s' % agname
                                        
                                        if astid in self.qlist and astid in self.agents_list and agname in self.agents_list[astid]:
                                                agprop = self.agents_list[astid][agname]
                                                # lookup the logged in/out status of agent agname and sends it back to the requester
                                                lstatus = '1'
                                                if agprop['status'] == 'AGENT_LOGGEDOFF':
                                                        lstatus = '0'
                                                tosend = { 'class' : 'agent-status',
                                                           'direction' : 'client',
                                                           'payload' : [astid, agname, lstatus,
                                                                        agprop['name'],
                                                                        agprop['phonenum'],
                                                                        self.qlist[astid].get_queues_byagent(agid)]
                                                           }
                                                self.__send_msg_to_cti_client__(userinfo, cjson.encode(tosend))
                                repstr = None

                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- (manage_cticommand) %s %s %s : %s'
                                  % (icommand.name, icommand.args, userinfo.get('login').get('connection'), exc))

                if repstr is not None: # might be useful to reply sth different if there is a capa problem for instance, a bad syntaxed command
                        try:
                                userinfo.get('login').get('connection').sendall(repstr + '\n')
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- (sendall) attempt to send <%s ...> (%d chars) failed : %s'
                                          % (repstr[:40], len(repstr), exc))
                return ret


        def __build_history_string__(self, requester_id, nlines, kind):
                # p/xivo/x/sip/103/103
                [company, userid] = requester_id.split('/')
                userinfo = self.ulist_ng.finduser(userid + '@' + company)
                astid = userinfo.get('astid')
                termlist = userinfo.get('techlist').split(',')
                reply = []
                for termin in termlist:
                        [techno, phoneid] = termin.split('/')
                        print requester_id, nlines, kind, techno, phoneid
                        try:
                                hist = self.__update_history_call__(self.configs[astid], techno, phoneid, nlines, kind)
                                for x in hist:
                                        try:
                                                ry1 = HISTSEPAR.join([x[0].isoformat(),
                                                                      x[1].replace('"', ''),
                                                                      str(x[10]),
                                                                      x[11]])
                                        except:
                                                ry1 = HISTSEPAR.join([x[0],
                                                                      x[1].replace('"', ''),
                                                                      str(x[10]),
                                                                      x[11]])
                                        if kind == '0':
                                                num = x[3].replace('"', '')
                                                cidname = num
                                                ry2 = HISTSEPAR.join([cidname, 'OUT', termin])
                                        else:   # display callerid for incoming calls
                                                ry2 = HISTSEPAR.join([x[1].replace('"', ''), 'IN', termin])
                                                
                                        reply.append(HISTSEPAR.join([ry1, ry2]))
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- error : history : (client %s, termin %s) : %s'
                                          % (requester_id, termin, exc))

                if len(reply) > 0:
                        # sha1sum = sha.sha(''.join(reply)).hexdigest()
                        tosend = { 'class' : 'history',
                                   'direction' : 'client',
                                   'payload' : reply}
                        return cjson.encode(tosend)
                else:
                        return


        def __build_queue_status__(self, astid, qname):
                lst_agents = []
                lst_entries = []
                if astid in self.qlist and qname in self.qlist[astid].queuelist:
                        for agid, agprop in self.qlist[astid].queuelist[qname]['agents'].iteritems():
                                lst_agents.append('%s,%s,%s' % (agid, agprop['Paused'], agprop['Status']))
                        for chan, chanprop in self.qlist[astid].queuelist[qname]['channels'].iteritems():
                                lst_entries.append('%s,%s,%s' % (chan, chanprop[0], chanprop[1]))
                        payload = [astid, qname]
                        payload.append(str(len(lst_agents)))
                        payload.extend(lst_agents)
                        payload.append(str(len(lst_entries)))
                        payload.extend(lst_entries)
                        tosend = { 'class' : 'queue-status',
                                   'direction' : 'client',
                                   'payload' : payload }
                        return cjson.encode(tosend)
                else:
                        return None

        def __agent__(self, userinfo, commandargs):
                myastid = None
                myagentnum = None
                if 'agentnum' in userinfo:
                        myastid = userinfo['astid']
                        myagentnum = userinfo['agentnum']

                subcommand = commandargs[0]
                if subcommand == 'leave':
                        if len(commandargs) > 1:
                                queuenames = commandargs[1].split(',')
                                if len(commandargs) > 3:
                                        astid = commandargs[2]
                                        anum = commandargs[3]
                                else:
                                        astid = myastid
                                        anum = myagentnum
                                if astid is not None and anum is not None:
                                        for queuename in queuenames:
                                                self.amilist.execute(astid, 'queueremove', queuename, 'Agent/%s' % anum)
                elif subcommand == 'join':
                        if len(commandargs) > 1:
                                queuenames = commandargs[1].split(',')
                                if len(commandargs) > 4:
                                        astid = commandargs[2]
                                        anum = commandargs[3]
                                        if commandargs[4] == 'pause':
                                                spause = 'true'
                                        else:
                                                spause = 'false'
                                else:
                                        astid = myastid
                                        anum = myagentnum
                                        spause = 'false' # unpauses by default for user-requests
                                if astid is not None and anum is not None:
                                        for queuename in queuenames:
                                                self.amilist.execute(astid, 'queueadd', queuename, 'Agent/%s' % anum, spause)
                elif subcommand == 'pause':
                        if len(commandargs) > 1:
                                queuenames = commandargs[1].split(',')
                                if len(commandargs) > 3:
                                        astid = commandargs[2]
                                        anum = commandargs[3]
                                else:
                                        astid = myastid
                                        anum = myagentnum
                                if astid is not None and anum is not None:
                                        for queuename in queuenames:
                                                self.amilist.execute(astid, 'queuepause', queuename, 'Agent/%s' % anum, 'true')
                elif subcommand == 'unpause':
                        if len(commandargs) > 1:
                                queuenames = commandargs[1].split(',')
                                if len(commandargs) > 3:
                                        astid = commandargs[2]
                                        anum = commandargs[3]
                                else:
                                        astid = myastid
                                        anum = myagentnum
                                if astid is not None and anum is not None:
                                        for queuename in queuenames:
                                                self.amilist.execute(astid, 'queuepause', queuename, 'Agent/%s' % anum, 'false')
                elif subcommand == 'login':
                        if len(commandargs) > 2:
                                astid = commandargs[1]
                                anum = commandargs[2]
                                phonenum = None
                                for userinfo in self.ulist_ng.userlist.itervalues():
                                        if 'agentnum' in userinfo and userinfo.get('agentnum') == anum and userinfo.get('astid') == astid:
                                                phonenum = userinfo.get('agentphonenum')
                                                if phonenum is None:
                                                        phonenum = userinfo.get('phonenum')
                                                break
                        else:
                                astid = myastid
                                anum = myagentnum
                                phonenum = userinfo.get('agentphonenum')
                        if astid is not None and anum is not None and phonenum is not None:
                                self.amilist.execute(astid, 'agentcallbacklogin', anum, phonenum)
                                # chan_agent.c:2318 callback_deprecated: AgentCallbackLogin is deprecated and will be removed in a future release.
                                # chan_agent.c:2319 callback_deprecated: See doc/queues-with-callback-members.txt for an example of how to achieve
                                # chan_agent.c:2320 callback_deprecated: the same functionality using only dialplan logic.
                                self.amilist.execute(astid, 'setvar', 'AGENTBYCALLERID_%s' % phonenum, anum)
                        else:
                                log_debug(SYSLOG_WARNING, 'cannot login agent since astid,anum,phonenum = %s,%s,%s (%s)'
                                          % (astid, anum, phonenum, commandargs))
                elif subcommand == 'logout':
                        if len(commandargs) > 2:
                                astid = commandargs[1]
                                anum = commandargs[2]
                                phonenum = str(int(anum) - 6000)
                        else:
                                astid = myastid
                                anum = myagentnum
                                phonenum = userinfo['phonenum']
                        if astid is not None and anum is not None:
                                self.amilist.execute(astid, 'setvar', 'AGENTBYCALLERID_%s' % phonenum, '')
                                if len(anum) > 0:
                                        self.amilist.execute(astid, 'agentlogoff', anum)
                elif subcommand == 'lists':
                        pass
                else:
                        pass
                return


        def logoff_all_agents(self):
                for userinfo in self.ulist_ng.userlist.itervalues():
                        astid = userinfo.get('astid')
                        if 'agentnum' in userinfo and astid is not None:
                                agentnum = userinfo['agentnum']
                                if 'phonenum' in userinfo:
                                        phonenum = userinfo['phonenum']
                                        self.amilist.execute(astid, 'setvar', 'AGENTBYCALLERID_%s' % phonenum, '')
                                if agentnum is not None and len(agentnum) > 0:
                                        self.amilist.execute(astid, 'agentlogoff', agentnum)
                return


        def regular_update(self):
                """
                Define here the tasks one would like to complete on a regular basis.
                """
                try:
                        ntime = time.localtime()
                        thour = ntime[3]
                        tmin = ntime[4]
                        if 'regupdate' in self.xivoconf:
                                regactions = self.xivoconf['regupdate'].split(';')
                                if len(regactions) == 3 and thour == int(regactions[0]) and tmin < int(regactions[1]):
                                        log_debug(SYSLOG_INFO, '(%2d h %2d min) => %s' % (thour, tmin, regactions[2]))
                                        if regactions[2] == 'logoffagents':
                                                self.logoff_all_agents()
                                else:
                                        log_debug(SYSLOG_INFO, '(%2d h %2d min) => no action' % (thour, tmin))
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- (regular update) : %s' % exc)


        def __phlist__(self):
                fullstat = []
                for astid, iplist in self.plist.iteritems():
                        for idx, pidx in iplist.normal.iteritems():
                                bstatus = [pidx.context,
                                           pidx.tech,
                                           pidx.phoneid,
                                           pidx.hintstatus,
                                           '']
                                if pidx.towatch:
                                        phoneinfo = ['ful',
                                                     astid]
                                        phoneinfo.append(bstatus)
                                        phoneinfo.append(pidx.build_fullstatlist())
                                        fullstat.append(phoneinfo)
                tosend = { 'class' : 'phones',
                           'function' : 'list',
                           'direction' : 'client',
                           'payload' : fullstat }
                return cjson.encode(tosend)


        # \brief Builds the full list of callerIDNames/hints in order to send them to the requesting client.
        # This should be done after a command called "callerid".
        # \return a string containing the full callerIDs/hints list
        # \sa manage_tcp_connection
        def __build_callerids_hints__(self, icommand):
            kind = icommand.name
            if len(icommand.args) == 0:
                    reqid = kind + '-' + ''.join(random.sample(__alphanums__, 10)) + "-" + hex(int(time.time()))
                    log_debug(SYSLOG_INFO, 'transaction ID for %s is %s' % (kind, reqid))
                    self.fullstat_heavies[reqid] = []
                    if kind == 'phones-list':
                            for userinfo in self.ulist_ng.list.itervalues():
                                    if 'agentnum' in userinfo:
				    	    lstatus = '0'
                                            for ast, qlist in self.agents_list.iteritems():
                                                    if userinfo.get('agentnum') in qlist:
                                                            lstatus = '1'
                                                            if qlist[userinfo.get('agentnum')]['status'] == 'AGENT_LOGGEDOFF':
                                                                    lstatus = '0'
                                                            break
                                            astqueues = []
                                            for ast, qlist in self.qlist.iteritems():
                                                    for q, qq in qlist.queuelist.iteritems():
                                                            if len(qq['agents']) > 0 and ('Agent/%s' % userinfo.get('agentnum')) in qq['agents']:
                                                                    astqueues.append(q)
                                            ag = '/'.join(['agentstatus', userinfo.get('astid'), userinfo.get('agentnum'), lstatus, ','.join(astqueues)])
                                    else:
                                            ag = ''
                                    phonenum = 'SIP/' + userinfo['user']
                                    astid = userinfo.get('astid')
                                    if phonenum in self.plist[astid].normal:
                                            phone = self.plist[astid].normal[phonenum]
                                            userinfo['phonenum'] = userinfo['user']
                                            bstatus = ':'.join(['sip',
                                                                userinfo['user'],
                                                                userinfo['phonenum'],
                                                                userinfo['context'],
                                                                phone.imstat,
                                                                phone.hintstatus,
                                                                '',
                                                                ag])
                                            if phone.towatch:
                                                    phoneinfo = ('ful',
                                                                 astid,
                                                                 bstatus,
                                                                 'rm:rm:rm',
                                                                 phone.build_fullstatlist() + ";")
                                                    self.fullstat_heavies[reqid].append(':'.join(phoneinfo))
##                            for astid in self.configs:
##                                    plist_n = self.plist[astid]
##                                    plist_normal_keys = filter(lambda j: plist_n.normal[j].towatch, plist_n.normal.iterkeys())
##                                    plist_normal_keys.sort()
##                                    for phonenum in plist_normal_keys:
##                                            bstatus = plist_n.normal[phonenum].build_basestatus()
##                                            phoneinfo = ('ful',
##                                                         plist_n.astid,
##                                                         bstatus,
##                                                         plist_n.normal[phonenum].build_cidstatus(),
##                                                         plist_n.normal[phonenum].build_fullstatlist() + ";")
##                                            #    + "groupinfos/technique"
##                                            self.fullstat_heavies[reqid].append(':'.join(phoneinfo))
                    elif kind == 'phones-add':
                            for astid in self.configs:
                                    self.fullstat_heavies[reqid].extend(self.plist.lstadd[astid])
                    elif kind == 'phones-del':
                            for astid in self.configs:
                                    self.fullstat_heavies[reqid].extend(self.plist.lstdel[astid])
            else:
                        reqid = icommand.args[0]

            if reqid in self.fullstat_heavies:
                    fullstat = []
                    nstat = len(self.fullstat_heavies[reqid])/ITEMS_PER_PACKET
                    for j in xrange(ITEMS_PER_PACKET):
                        if len(self.fullstat_heavies[reqid]) > 0:
                                fullstat.append(self.fullstat_heavies[reqid].pop())
                    if nstat > 0:
                            rtab = '%s=%s;%s' %(kind, reqid, ''.join(fullstat))
                    else:
                            del self.fullstat_heavies[reqid]
                            rtab = '%s=0;%s'  %(kind, ''.join(fullstat))
                            log_debug(SYSLOG_INFO, 'building last packet reply for <%s ...>' %(rtab[0:40]))
                    return rtab
            else:
                    log_debug(SYSLOG_INFO, 'reqid <%s> not defined for %s reply' %(reqid, kind))
                    return ''


        # \brief Builds the features_get reply.
        def __build_features_get__(self, reqlist):
                [company, user] = reqlist[0].split('/')
                userinfo = self.ulist_ng.finduser(user + '@' + company)
                astid = userinfo.get('astid')
                context = userinfo.get('context')
                srcnum = userinfo.get('phonenum')
                repstr = ''

                cursor = self.configs[astid].userfeatures_db_conn.cursor()
                params = [srcnum, context]
                query = 'SELECT ${columns} FROM userfeatures WHERE number = %s AND context = %s'

                for key in ['enablevoicemail', 'callrecord', 'callfilter', 'enablednd']:
                        try:
                                columns = (key,)
                                cursor.query(query, columns, params)
                                results = cursor.fetchall()
                                if len(results) > 0:
                                        repstr += "%s;%s:;" %(key, str(results[0][0]))
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- features_get(bool) id=%s key=%s : %s'
                                          %(str(reqlist), key, exc))
                                tosend = { 'class' : 'features',
                                           'function' : 'get',
                                           'direction' : 'client',
                                           'payload' : [ reqlist[0], 'KO' ] }
                                return cjson.encode(tosend)

                for key in ['unc', 'busy', 'rna']:
                        try:
                                columns = ('enable' + key,)
                                cursor.query(query, columns, params)
                                resenable = cursor.fetchall()

                                columns = ('dest' + key,)
                                cursor.query(query, columns, params)
                                resdest = cursor.fetchall()

                                if len(resenable) > 0 and len(resdest) > 0:
                                        repstr += '%s;%s:%s;' % (key, str(resenable[0][0]), str(resdest[0][0]))

                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- features_get(str) id=%s key=%s : %s'
                                          %(str(reqlist), key, exc))
                                tosend = { 'class' : 'features',
                                           'function' : 'get',
                                           'direction' : 'client',
                                           'payload' : [ reqlist[0], 'KO' ] }
                                return cjson.encode(tosend)

                if len(repstr) == 0:
                        repstr = 'KO'
                tosend = { 'class' : 'features',
                           'function' : 'get',
                           'direction' : 'client',
                           'payload' : [ reqlist[0], repstr ] }
                return cjson.encode(tosend)


        # \brief Builds the features_put reply.
        def __build_features_put__(self, reqlist):
                [company, user] = reqlist[0].split('/')
                userinfo = self.ulist_ng.finduser(user + '@' + company)
                astid = userinfo.get('astid')
                context = userinfo.get('context')
                srcnum = userinfo.get('phonenum')
                try:
                        len_reqlist = len(reqlist)
                        if len_reqlist >= 2:
                                key = reqlist[1]
                                value = ''
                                if len_reqlist >= 3:
                                        value = reqlist[2]
                                query = 'UPDATE userfeatures SET ' + key + ' = %s WHERE number = %s AND context = %s'
                                params = [value, srcnum, context]
                                cursor = self.configs[astid].userfeatures_db_conn.cursor()
                                cursor.query(query, parameters = params)
                                self.configs[astid].userfeatures_db_conn.commit()
                                tosend = { 'class' : 'features',
                                           'function' : 'put',
                                           'direction' : 'client',
                                           'payload' : [ reqlist[0], 'OK', key, value ] }
                        else:
                                tosend = { 'class' : 'features',
                                           'function' : 'put',
                                           'direction' : 'client',
                                           'payload' : [ reqlist[0], 'KO' ] }
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- features_put id=%s : %s'
                                  %(str(reqlist), exc))
                        tosend = { 'class' : 'features',
                                   'function' : 'put',
                                   'direction' : 'client',
                                   'payload' : [ reqlist[0], 'KO' ] }
                return cjson.encode(tosend)


        # \brief Originates / transfers.
        def __originate_or_transfer__(self, userinfo, commargs):
                log_debug(SYSLOG_INFO, '%s %s' % (userinfo, commargs))
                if len(commargs) != 3:
                        return
                [commname, src, dst] = commargs
                srcsplit = src.split(':', 1)
                dstsplit = dst.split(':', 1)

                if commname == 'originate' and len(srcsplit) == 2 and len(dstsplit) == 2:
                        [typesrc, whosrc] = srcsplit
                        [typedst, whodst] = dstsplit

                        # others than 'user:special:me' should only be allowed to switchboard-like users
                        if typesrc == 'user':
                                if whosrc == 'special:me':
                                        srcuinfo = userinfo
                                else:
                                        tofind = whosrc.split('/')[1] + '@' + whosrc.split('/')[0]
                                        srcuinfo = self.ulist_ng.finduser(tofind)
                                if srcuinfo is not None:
                                        astid_src = srcuinfo.get('astid')
                                        context_src = srcuinfo.get('context')
                                        proto_src = 'SIP'
                                        # 'local' might break the XIVO_ORIGSRCNUM mechanism (trick for thomson)
                                        # XXX (+ dialplan) since 'SIP' is not the solution either
                                        
                                        phonenum_src = srcuinfo.get('phonenum')
                                        # if termlist empty + agentphonenum not empty => call this one
                                        cidname_src = srcuinfo.get('fullname')

                        elif typesrc == 'term':
                                pass
                        else:
                                log_debug(SYSLOG_WARNING, 'unknown typesrc <%s>' % typesrc)

                        # dst
                        if typedst == 'ext':
                                context_dst = context_src
                                cidname_dst = 'direct number'
                                if whodst == 'special:parkthecall':
                                        exten_dst = self.configs[astid_src].parkingnumber
                                else:
                                        exten_dst = whodst
                        elif typedst == 'user':
                                if whodst == 'special:me':
                                        dstuinfo = userinfo
                                elif whodst == 'special:intercept':
                                        dstuinfo = {'phonenum' : '*8',
                                                    'fullname' : 'intercept',
                                                    'context' : 'parkedcalls'}
                                else:
                                        tofind = whodst.split('/')[1] + '@' + whodst.split('/')[0]
                                        dstuinfo = self.ulist_ng.finduser(tofind)
                                if dstuinfo is not None:
                                        exten_dst = dstuinfo.get('phonenum')
                                        cidname_dst = dstuinfo.get('fullname')
                                        context_dst = dstuinfo.get('context')
                        elif typedst == 'term':
                                pass
                        else:
                                log_debug(SYSLOG_WARNING, 'unknown typedst <%s>' % typedst)


                        ret = False
                        try:
                                if len(exten_dst) > 0:
                                        ret = self.amilist.execute(astid_src, 'originate',
                                                                   proto_src, phonenum_src, cidname_src,
                                                                   exten_dst, cidname_dst,  context_dst)
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- unable to originate ... %s' % exc)
                        if ret:
                                ret_message = 'originate OK'
                        else:
                                ret_message = 'originate KO'
                elif commname in ['transfer', 'atxfer']:
                        [typesrc, whosrc] = srcsplit
                        [typedst, whodst] = dstsplit

                        if typesrc == 'chan':
                                if whosrc.startswith('special:me:'):
                                        srcuinfo = userinfo
                                        chan_src = whosrc[len('special:me:'):]
                                if srcuinfo is not None:
                                        astid_src = srcuinfo.get('astid')
                                        context_src = srcuinfo.get('context')
                                        proto_src = 'local'
                                        phonenum_src = srcuinfo.get('phonenum')
                                        # if termlist empty + agentphonenum not empty => call this one
                                        cidname_src = srcuinfo.get('fullname')

                        if typedst == 'ext':
                                if whodst == 'special:parkthecall':
                                        exten_dst = self.configs[astid_src].parkingnumber
                                else:
                                        exten_dst = whodst

                        print astid_src, commname, chan_src, exten_dst, context_src
                        ret = False
                        try:
                                if len(exten_dst) > 0:
                                        ret = self.amilist.execute(astid_src, commname,
                                                                   chan_src,
                                                                   exten_dst, context_src)
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- unable to %s ... %s' % (commname, exc))
                        if ret:
                                ret_message = '%s OK' % commname
                        else:
                                ret_message = '%s KO' % commname
                else:
                        log_debug(SYSLOG_WARNING, 'unallowed command %s' % commargs)


        # \brief Originates / transfers.
        def __originate_or_transfer_old__(self, requester, l):
         src_split = l[1].split("/")
         dst_split = l[2].split("/")
         ret_message = 'originate_or_transfer KO from %s' % requester

         if len(src_split) == 5:
                [dummyp, astid_src, context_src, proto_src, userid_src] = src_split
         elif len(src_split) == 6:
                [dummyp, astid_src, context_src, proto_src, userid_src, dummy_exten_src] = src_split

         if len(dst_split) == 6:
                [p_or_a, astid_dst, context_dst, proto_dst, userid_dst, exten_dst] = dst_split
##                if p_or_a == 'a': find userid_dst => agentnum => phone num
         else:
                [dummyp, astid_dst, context_dst, proto_dst, userid_dst, exten_dst] = src_split
                exten_dst = l[2]
         if astid_src in self.configs and astid_src == astid_dst:
                if exten_dst == 'special:parkthecall':
                        exten_dst = self.configs[astid_dst].parkingnumber
                if astid_src in self.plist:
                        if l[0] == 'originate':
                                log_debug(SYSLOG_INFO, "%s is attempting an ORIGINATE : %s" %(requester, str(l)))
                                if astid_dst != '':
                                        sipcid_src = "SIP/%s" % userid_src
                                        sipcid_dst = "SIP/%s" % userid_dst
                                        cidname_src = userid_src
                                        cidname_dst = userid_dst
                                        if sipcid_src in self.plist[astid_src].normal:
                                                cidname_src = '%s %s' %(self.plist[astid_src].normal[sipcid_src].calleridfirst,
                                                                        self.plist[astid_src].normal[sipcid_src].calleridlast)
                                        if sipcid_dst in self.plist[astid_dst].normal:
                                                cidname_dst = '%s %s' %(self.plist[astid_dst].normal[sipcid_dst].calleridfirst,
                                                                        self.plist[astid_dst].normal[sipcid_dst].calleridlast)
                                        ret = self.amilist.execute(astid_src, 'originate',
                                                                   proto_src, userid_src, cidname_src,
                                                                   exten_dst, cidname_dst, context_dst)
                                else:
                                        ret = False
                                if ret:
                                        ret_message = 'originate OK (%s) %s %s' %(astid_src, l[1], l[2])
                                else:
                                        ret_message = 'originate KO (%s) %s %s' %(astid_src, l[1], l[2])
                        elif l[0] == 'transfer':
                                log_debug(SYSLOG_INFO, "%s is attempting a TRANSFER : %s" %(requester, str(l)))
                                phonesrc, phonesrcchan = split_from_ui(l[1])
                                if phonesrc == phonesrcchan:
                                        ret_message = 'transfer KO : %s not a channel' % phonesrcchan
                                else:
                                        if phonesrc in self.plist[astid_src].normal:
                                                channellist = self.plist[astid_src].normal[phonesrc].chann
                                                nopens = len(channellist)
                                                if nopens == 0:
                                                        ret_message = 'transfer KO : no channel opened on %s' % phonesrc
                                                else:
                                                        tchan = channellist[phonesrcchan].getChannelPeer()
                                                        ret = self.amilist.execute(astid_src, 'transfer',
                                                                                   tchan, exten_dst, context_dst)
                                                        if ret:
                                                                ret_message = 'transfer OK (%s) %s %s' %(astid_src, l[1], l[2])
                                                        else:
                                                                ret_message = 'transfer KO (%s) %s %s' %(astid_src, l[1], l[2])
                        elif l[0] == 'atxfer':
                                log_debug(SYSLOG_INFO, "%s is attempting an ATXFER : %s" %(requester, str(l)))
                                phonesrc, phonesrcchan = split_from_ui(l[1])
                                if phonesrc == phonesrcchan:
                                        ret_message = 'atxfer KO : %s not a channel' % phonesrcchan
                                else:
                                        if phonesrc in self.plist[astid_src].normal:
                                                channellist = self.plist[astid_src].normal[phonesrc].chann
                                                nopens = len(channellist)
                                                if nopens == 0:
                                                        ret_message = 'atxfer KO : no channel opened on %s' % phonesrc
                                                else:
                                                        tchan = channellist[phonesrcchan].getChannelPeer()
                                                        ret = self.amilist.execute(astid_src, 'atxfer',
                                                                                   tchan, exten_dst, context_dst)
                                                        if ret:
                                                                ret_message = 'atxfer OK (%s) %s %s' %(astid_src, l[1], l[2])
                                                        else:
                                                                ret_message = 'atxfer KO (%s) %s %s' %(astid_src, l[1], l[2])
                                        else:
                                                log_debug(SYSLOG_WARNING, "%s not in my phone list" % phonesrc)
         else:
                ret_message = 'originate or transfer KO : asterisk id mismatch (%s %s)' %(astid_src, astid_dst)
         return self.dmessage_srv2clt(ret_message)



        # \brief Hangs up.
        def __hangup__(self, uinfo, args, peer_hangup):
                print uinfo, args, peer_hangup
                username = uinfo.get('fullname')
                astid = args[0]
                chan = args[1]
                ret_message = 'hangup KO from %s' % username
                if astid in self.configs:
                        log_debug(SYSLOG_INFO, "%s is attempting a HANGUP : %s" % (username, chan))
                        channel = chan
                        phone = chan.split('-')[0]
                        if phone in self.plist[astid].normal:
                                if channel in self.plist[astid].normal[phone].chann:
                                        if peer_hangup:
                                                channel_peer = self.plist[astid].normal[phone].chann[channel].getChannelPeer()
                                                log_debug(SYSLOG_INFO, "UI action : %s : hanging up <%s> and <%s>"
                                                          %(astid , channel, channel_peer))
                                        else:
                                                channel_peer = ''
                                                log_debug(SYSLOG_INFO, "UI action : %s : hanging up <%s>"
                                                          %(astid , channel))
                                        ret = self.amilist.execute(astid, 'hangup', channel, channel_peer)
                                        if ret > 0:
                                                ret_message = 'hangup OK (%d) <%s>' %(ret, chan)
                                        else:
                                                ret_message = 'hangup KO : socket request failed'
                                else:
                                        ret_message = 'hangup KO : no such channel <%s>' % channel
                        else:
                                ret_message = 'hangup KO : no such phone <%s>' % phone
                else:
                        ret_message = 'hangup KO : no such asterisk id <%s>' % astid
                return self.dmessage_srv2clt(ret_message)


        # \brief Function that fetches the call history from a database
        # \param cfg the asterisk's config
        # \param techno technology (SIP/IAX/ZAP/etc...)
        # \param phoneid phone id
        # \param nlines the number of lines to fetch for the given phone
        # \param kind kind of list (ingoing, outgoing, missed calls)
        def __update_history_call__(self, cfg, techno, phoneid, nlines, kind):
                results = []
                if cfg.cdr_db_conn is None:
                        log_debug(SYSLOG_WARNING, '%s : no CDR uri defined for this asterisk - see cdr_db_uri parameter' % cfg.astid)
                else:
                        try:
                                cursor = cfg.cdr_db_conn.cursor()
                                columns = ('calldate', 'clid', 'src', 'dst', 'dcontext', 'channel', 'dstchannel',
                                           'lastapp', 'lastdata', 'duration', 'billsec', 'disposition', 'amaflags',
                                           'accountcode', 'uniqueid', 'userfield')
                                likestring = '%s/%s-%%' %(techno, phoneid)
                                orderbycalldate = "ORDER BY calldate DESC LIMIT %s" % nlines
                                
                                if kind == "0": # outgoing calls (all)
                                        cursor.query("SELECT ${columns} FROM cdr WHERE channel LIKE %s " + orderbycalldate,
                                                     columns,
                                                     (likestring,))
                                elif kind == "1": # incoming calls (answered)
                                        cursor.query("SELECT ${columns} FROM cdr WHERE disposition='ANSWERED' AND dstchannel LIKE %s " + orderbycalldate,
                                                     columns,
                                                     (likestring,))
                                else: # missed calls (received but not answered)
                                        cursor.query("SELECT ${columns} FROM cdr WHERE disposition!='ANSWERED' AND dstchannel LIKE %s " + orderbycalldate,
                                                     columns,
                                                     (likestring,))
                                results = cursor.fetchall()
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- %s : Connection to DataBase failed in History request : %s'
                                          %(cfg.astid, exc))
                return results


        def __update_availstate__(self, userinfo, state):
                company = userinfo['company']
                username = userinfo['user']

                if 'login' in userinfo and 'sessiontimestamp' in userinfo.get('login'):
                        userinfo['login']['sessiontimestamp'] = time.time()
                if state in allowed_states:
                        userinfo['state'] = state
                else:
                        log_debug(SYSLOG_WARNING, '(user %s) : state <%s> is not an allowed one => undefinedstate-updated'
                                  % (username, state))
                        userinfo['state'] = 'undefinedstate-updated'

                tosend = { 'class' : 'presence',
                           'direction' : 'client',
                           'payload' : [ company, username, userinfo['state'] ] }
                self.__send_msg_to_cti_clients__(cjson.encode(tosend))

                return None


        # \brief Builds the full list of customers in order to send them to the requesting client.
        # This should be done after a command called "customers".
        # \return a string containing the full customers list
        # \sa manage_tcp_connection
        def __build_customers__(self, ctx, searchpatterns):
                fulllist = []
                if ctx in self.ctxlist.ctxlist:
                        for dirsec, dirdef in self.ctxlist.ctxlist[ctx].iteritems():
                                try:
                                        y = self.__build_customers_bydirdef__(dirsec, searchpatterns, dirdef)
                                        fulllist.extend(y)
                                except Exception, exc:
                                        log_debug(SYSLOG_ERR, '--- exception --- __build_customers__ (%s) : %s'
                                                  % (dirsec, exc))
                else:
                        log_debug(SYSLOG_WARNING, 'there has been no section defined for context %s : can not proceed directory search' % ctx)

                mylines = []
                for itemdir in fulllist:
                        myitems = []
                        for k in self.ctxlist.display_items[ctx]:
                                [title, type, defaultval, format] = self.ctxlist.displays[ctx][k].split('|')
                                basestr = format
                                for k, v in itemdir.iteritems():
                                        basestr = basestr.replace('{%s}' % k, v)
                                myitems.append(basestr)
                        mylines.append(';'.join(myitems))

                mylines.sort()
##                uniq = {}
##                fullstat_body = []
##                for fsl in [uniq.setdefault(e,e) for e in fullstatlist if e not in uniq]:
##                        fullstat_body.append(fsl)
##                return fullstat_body
                tosend = { 'class' : 'directory',
                           'direction' : 'client',
                           'payload' : ';'.join(self.ctxlist.display_header[ctx]) + ';' + ';'.join(mylines) }
                return cjson.encode(tosend)


        def __build_customers_bydirdef__(self, dirname, searchpatterns, z):
                searchpattern = ' '.join(searchpatterns)
                fullstatlist = []

                if searchpattern == "":
                        return []

                dbkind = z.uri.split(':')[0]
                if dbkind in ['ldap', 'ldaps']:
                        selectline = []
                        for fname in z.match_direct:
                                if searchpattern == "*":
                                        selectline.append("(%s=*)" % fname)
                                else:
                                        selectline.append("(%s=*%s*)" %(fname, searchpattern))

                        try:
                                results = None
                                if z.uri in self.ldapids:
                                        ldapid = self.ldapids[z.uri]
                                else:
                                        ldapid = xivo_ldap.xivo_ldap(z.uri)
                                        if ldapid.l is not None:
                                                self.ldapids[z.uri] = ldapid
                                if ldapid.l is not None:
                                        results = ldapid.getldap("(|%s)" % ''.join(selectline),
                                                                 z.match_direct)
                                if results is not None:
                                        for result in results:
                                                futureline = {'xivo-dir' : z.name}
                                                for keyw, dbkeys in z.fkeys.iteritems():
                                                        for dbkey in dbkeys:
                                                                if dbkey in result[1]:
                                                                        futureline[keyw] = result[1][dbkey][0]
                                                fullstatlist.append(futureline)
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- ldaprequest (directory) : %s' % exc)

                elif dbkind == 'file':
                        f = urllib.urlopen(z.uri)
                        delimit = ':'
                        n = 0
                        for line in f:
                                if n == 0:
                                        header = line
                                        headerfields = header.strip().split(delimit)
                                else:
                                        ll = line.strip()
                                        if ll.lower().find(searchpattern.lower()) >= 0:
                                                t = ll.split(delimit)
                                                futureline = {'xivo-dir' : z.name}
                                                for keyw, dbkeys in z.fkeys.iteritems():
                                                        for dbkey in dbkeys:
                                                                idx = headerfields.index(dbkey)
                                                                futureline[keyw] = t[idx]
                                                fullstatlist.append(futureline)
                                n += 1
                        if n == 0:
                                log_debug(SYSLOG_WARNING, 'WARNING : %s is empty' % z.uri)
                        elif n == 1:
                                log_debug(SYSLOG_WARNING, 'WARNING : %s contains only one line (the header one)' % z.uri)

                elif dbkind == 'http':
                        f = urllib.urlopen('%s%s' % (z.uri, searchpattern))
                        delimit = ':'
                        n = 0
                        for line in f:
                                if n == 0:
                                        header = line
                                        headerfields = header.strip().split(delimit)
                                else:
                                        ll = line.strip()
                                        t = ll.split(delimit)
                                        futureline = {'xivo-dir' : z.name}
                                        for keyw, dbkeys in z.fkeys.iteritems():
                                                for dbkey in dbkeys:
                                                        idx = headerfields.index(dbkey)
                                                        futureline[keyw] = t[idx]
                                        fullstatlist.append(futureline)
                                n += 1
                        if n == 0:
                                log_debug(SYSLOG_WARNING, 'WARNING : %s is empty' % z.uri)
                        # we don't warn about "only one line" here since the filter has probably already been applied

                elif dbkind != '':
                        if searchpattern == '*':
                                whereline = ''
                        else:
                                wl = []
                                for fname in z.match_direct:
                                        wl.append("%s REGEXP '%s'" %(fname, searchpattern))
                                whereline = 'WHERE ' + ' OR '.join(wl)

                        try:
                                conn = anysql.connect_by_uri(z.uri)
                                cursor = conn.cursor()
                                cursor.query("SELECT ${columns} FROM " + z.sqltable + " " + whereline,
                                             tuple(z.match_direct),
                                             None)
                                results = cursor.fetchall()
                                conn.close()
                                for result in results:
                                        futureline = {'xivo-dir' : z.name}
                                        for keyw, dbkeys in z.fkeys.iteritems():
                                                for dbkey in dbkeys:
                                                        if dbkey in z.match_direct:
                                                                n = z.match_direct.index(dbkey)
                                                                futureline[keyw] = result[n]
                                        fullstatlist.append(futureline)
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- sqlrequest : %s' % exc)
                else:
                        log_debug(SYSLOG_WARNING, 'no database method defined - please fill the uri field of the directory <%s> definition' % dirname)

                return fullstatlist


        def handle_fagi(self, astid, fastagi):
                """
                Previously known as 'xivo_push'
                """
                # check capas !
                # fastagi.get_variable('XIVO_INTERFACE') # CHANNEL
                callednum = fastagi.get_variable('XIVO_DSTNUM')
                context = fastagi.get_variable('XIVO_CONTEXT')
                uniqueid = fastagi.get_variable('UNIQUEID')
                channel = fastagi.get_variable('CHANNEL')
                calleridnum  = fastagi.env['agi_callerid']
                calleridname = fastagi.env['agi_calleridname']

                log_debug(SYSLOG_INFO, 'handle_fagi : %s : context=%s %s %s <%s>' % (astid, context, callednum, calleridnum, calleridname))

                extraevent = {'caller_num' : calleridnum,
                              'called_num' : callednum,
                              'uniqueid' : uniqueid,
                              'channel' : channel}
                clientstate = 'available'
                calleridsolved = calleridname

                self.__sheet_alert__('agi', astid, context, {}, extraevent)

                if calleridnum == 'unknown':
                        # to set according to os.getenv('LANG') or os.getenv('LANGUAGE') later on ?
                        calleridnum = 'Inconnu'
                if calleridname == 'unknown':
                        calleridname = ''

                calleridtoset = '"%s"<%s>' %(calleridsolved, calleridnum)
                # if calleridsolved not found
                # calleridtoset = '"%s"<%s>' %(calleridname, calleridnum)
                print 'The Caller Id will be set to %s' % calleridtoset
                fastagi.set_callerid(calleridtoset)

##                if clientstate == 'available' or clientstate == 'nopresence':
##                        fastagi.set_variable('XIVO_AIMSTATUS', 0)
##                elif clientstate == 'away':
##                        fastagi.set_variable('XIVO_AIMSTATUS', 1)
##                elif clientstate == 'donotdisturb':
##                        fastagi.set_variable('XIVO_AIMSTATUS', 2)
##                elif clientstate == 'outtolunch':
##                        fastagi.set_variable('XIVO_AIMSTATUS', 3)
##                elif clientstate == 'berightback':
##                        fastagi.set_variable('XIVO_AIMSTATUS', 4)
##                else:
##                        print "Unknown user's availability status : <%s>" % clientstate
                return


xivo_commandsets.CommandClasses['xivocti'] = XivoCTICommand


def channel_splitter(channel):
        sp = channel.split('-')
        if len(sp) > 1:
                sp.pop()
        return '-'.join(sp)


def split_from_ui(fullname):
        phone = ""
        channel = ""
        s1 = fullname.split('/')
        if len(s1) == 5:
                phone = s1[3] + "/" + channel_splitter(s1[4])
                channel = s1[3] + "/" + s1[4]
        return [phone, channel]
