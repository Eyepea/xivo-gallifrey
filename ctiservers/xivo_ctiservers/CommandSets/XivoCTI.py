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

import os
import random
import re
import socket
import string
import time
import urllib
from xivo_ctiservers import cti_capas
from xivo_ctiservers import cti_fax
from xivo_ctiservers import cti_userlist
from xivo_ctiservers import cti_agentlist
from xivo_ctiservers import cti_queuelist
from xivo_ctiservers import xivo_commandsets
from xivo_ctiservers import xivo_ldap
from xivo_ctiservers.xivo_commandsets import BaseCommand
from xivo_ctiservers.xivo_log import *
from xivo import anysql
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite

def log_debug(level, text):
        log_debug_file(level, text, 'xivocti')

SHEET_EVENT_INCOMING    = 1 << 0
SHEET_EVENT_OUTGOING    = 1 << 1
SHEET_EVENT_AGENTLINK   = 1 << 2
SHEET_EVENT_AGI         = 1 << 3
SHEET_EVENT_PHONELINK   = 1 << 4
SHEET_EVENT_AGENTCALLED = 1 << 5

SHEET_ACTION_XCFULL  = 1 << 0
SHEET_ACTION_XCPOPUP = 1 << 1
SHEET_ACTION_BOT     = 1 << 2
SHEET_ACTION_URL     = 1 << 3

XIVOVERSION = '0.4'
REQUIRED_CLIENT_VERSION = 3150
__revision__ = __version__.split()[1]
__alphanums__ = string.uppercase + string.lowercase + string.digits
allowed_states = ['available', 'away', 'outtolunch', 'donotdisturb', 'berightback']
ITEMS_PER_PACKET = 500
HISTSEPAR = ';'

class XivoCTICommand(BaseCommand):

        xdname = 'XIVO Daemon'
        xivoclient_session_timeout = 60 # XXX

        fullstat_heavies = {}
        queues_list = {}
        queues_channels_list = {}
        agents_list = {}

        def __init__(self, amilist, ctiports, queued_threads_pipe):
		BaseCommand.__init__(self)
                self.amilist = amilist
                self.capas = {}
                self.ulist_ng = cti_userlist.UserList()
                self.ulist_ng.setcommandclass(self)
                self.qlist_ng = cti_queuelist.QueueList()
                self.qlist_ng.setcommandclass(self)
                self.alist_ng = cti_agentlist.AgentList()
                self.alist_ng.setcommandclass(self)
                self.transfers_buf = {}
                self.transfers_ref = {}
                self.faxes = {}
                self.queued_threads_pipe = queued_threads_pipe
                self.disconnlist = []
                return

        def get_list_commands(self):
                return ['login',
                        'history', 'directory-search',
                        'featuresget', 'featuresput',
                        'phones-list', 'phones-add', 'phones-del',
                        'agents-list', 'agents-status', 'agent-status', 'agent',
                        'queues-list', 'queue-status',
                        'users-list',
                        'faxsend',
                        'faxdata',
                        'database',
                        'message',
                        'availstate',
                        'originate', 'transfer', 'atxfer', 'hangup', 'simplehangup', 'pickup']

        def parsecommand(self, linein):
                params = linein.split()
                cmd = xivo_commandsets.Command(params[0], params[1:])
                if cmd.name == 'login':
                        cmd.type = xivo_commandsets.CMD_LOGIN
                elif cmd.name == 'faxdata':
                        cmd.type = xivo_commandsets.CMD_TRANSFER
                else:
                        cmd.type = xivo_commandsets.CMD_OTHER
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
                                self.faxes[ref].send(''.join(self.transfers_buf[req]), self.configs[astid].faxcallerid, self.amilist.ami[astid])
                                del self.transfers_ref[req]
                                del self.transfers_buf[req]
                                del self.faxes[ref]
                return

        def get_login_params(self, astid, command, connid):
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


        # fields set at startup by reading informations
        userfields = ['astid', 'user', 'phonenum', 'passwd', 'init', 'fullname', 'company', 'capaid', 'context']

        # fields set at login time by a user, some are a id-dimension, others are auth-related
        # TBD : fields set once logged in ...
        def required_login_params(self):
                return ['loginkind', 'company', 'userid', # identification
                        'state', 'passwd', 'ident', 'xivoversion', 'version'] # authentication & information

        def manage_login(self, loginparams):
                missings = []
                for argum in self.required_login_params():
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
                if len(ident.split("@")) == 2:
                        [whoami, whatsmyos] = ident.split("@")
                        # return 'wrong_client_identifier:%s' % whoami
                        if whatsmyos[:3] not in ['X11', 'WIN', 'MAC']:
                                return 'wrong_os_identifier:%s' % whatsmyos
                else:
                        return 'wrong_client_os_identifier:%s' % ident
                if (not svnversion.isdigit()) or int(svnversion) < REQUIRED_CLIENT_VERSION:
                        return 'version_client:%s;%d' % (svnversion, REQUIRED_CLIENT_VERSION)


                # user match and authentication
                username = '%s@%s' % (loginparams.get('userid'), loginparams.get('company'))
                userinfo = self.ulist_ng.finduser(username)
                print 'userinfo', userinfo
                if userinfo == None:
                        return 'user_not_found'
                password = loginparams.get('passwd') 
                if  password != userinfo.get('passwd'):
                        return 'login_passwd'

                iserr = self.__check_user_connection__(userinfo, whoami)
                if iserr is not None:
                        return iserr

                # settings (in agent mode for instance)
                ## userinfo['agent']['phonenum'] = phonenum
                state = loginparams.get('state')
                self.__connect_user__(userinfo, whoami, whatsmyos, svnversion, state, False)

                loginkind = loginparams.get('loginkind')
                if loginkind == 'agent':
                        userinfo['agentphonenum'] = loginparams.get('phonenumber')
                        # self.amilist.execute(astid, 'agentcallbacklogin', agentnum, phonenum)
                return userinfo


        def manage_logoff(self, userinfo, when):
                log_debug(SYSLOG_INFO, 'logoff (%s) %s'
                          % (when, userinfo))
                if 'agentnum' in userinfo:
                        agentnum = userinfo['agentnum']
                        astid = userinfo['astid']
                        if 'phonenum' in userinfo:
                                phonenum = userinfo['phonenum']
                                self.amilist.execute(astid, 'setvar', 'AGENTBYCALLERID_%s' % phonenum, '')
                        if agentnum is not None:
                                self.amilist.execute(astid, 'agentlogoff', agentnum)
                if 'agentphonenum' in userinfo:
                        del userinfo['agentphonenum']
                self.__disconnect_user__(userinfo)
                return


        def __check_user_connection__(self, userinfo, whoami):
                #if userinfo.has_key('init'):
                #       if not userinfo['init']:
                #              return 'uninit_phone'
                if userinfo.has_key('login') and userinfo['login'].has_key('sessiontimestamp'):
                        if time.time() - userinfo['login'].get('sessiontimestamp') < self.xivoclient_session_timeout:
                                if 'lastconnwins' in userinfo:
                                        if userinfo['lastconnwins'] is True:
                                                # one should then disconnect the first peer
                                                pass
                                        else:
                                                return 'already_connected'
                                else:
                                        return 'already_connected'

                capaid = userinfo.get('capaid')
                if self.capas[capaid].toomuchusers():
                        return 'toomuchusers:%s' % self.capas[capaid].maxgui()

                return None


        def __connect_user__(self, userinfo,
                             whoami, whatsmyos, version, state,
                             lastconnwins):
                try:
                        userinfo['login'] = {}
                        userinfo['login']['sessionid'] = ''.join(random.sample(__alphanums__, 10))
                        userinfo['login']['sessiontimestamp'] = time.time()
                        userinfo['login']['logintimestamp'] = time.time()
                        userinfo['login']['cticlienttype'] = whoami
                        userinfo['login']['cticlientos'] = whatsmyos
                        userinfo['login']['version'] = version
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

                        capaid = userinfo.get('capaid')
                        self.capas[capaid].conn_inc()
                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- connect_user %s : %s" %(str(userinfo), str(exc)))


        def __disconnect_user__(self, userinfo):
                try:
                        # state is unchanged
                        if 'login' in userinfo:
                                capaid = userinfo.get('capaid')
                                self.capas[capaid].conn_dec()
                                del userinfo['login']
                                userinfo['state'] = 'unknown'
                                self.__update_availstate__(userinfo, userinfo.get('state'))
                        else:
                                log_debug(SYSLOG_WARNING, 'userinfo does not contain login field : %s' % userinfo)
                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- disconnect_user %s : %s" %(str(userinfo), str(exc)))


        def loginko(self, loginparams, errorstring, connid):
                log_debug(SYSLOG_WARNING, 'user can not connect (%s) : sending %s' % (loginparams, errorstring))
                connid.sendall('loginko=%s\n' % errorstring)
                return


        def loginok(self, loginparams, userinfo):
                capaid = userinfo.get('capaid')
                repstr = "loginok=" \
                         "context:%s;phonenum:%s;" \
                         "capas:%s;capadisp:%s;capaappli:%s;" \
                         "xivoversion:%s;version:%s;state:%s" \
                         %(userinfo.get('context'),
                           userinfo.get('phonenum'),
                           self.capas[capaid].tostring(self.capas[capaid].all()),
                           self.capas[capaid].capadisp,
                           self.capas[capaid].appliname,
                           XIVOVERSION,
                           __revision__,
                           userinfo.get('state'))
##                if 'features' in capa_user:
##                        repstr += ';capas_features:%s' %(','.join(configs[astid].capafeatures))
                userinfo['login']['connection'].sendall(repstr + '\n')
                self.__update_availstate__(userinfo, userinfo.get('state'))
                return


        def set_options(self, xivoconf):
                self.xivoconf = xivoconf
                if 'xivo_db_uri' in self.xivoconf:
                        xivo_db_uri = self.xivoconf['xivo_db_uri']
                        self.xivo_conn = anysql.connect_by_uri(xivo_db_uri)
                        self.xivo_cursor = self.xivo_conn.cursor()
                        columns = ('name',)
                        self.xivo_cursor.query("SELECT ${columns} FROM queue",
                                               columns)
                        results = self.xivo_cursor.fetchall()

                        astid = 'xivo'
                        if astid not in self.queues_list:
                                self.queues_list[astid] = {}
                        for queue in results:
                                if queue[0] not in self.queues_list[astid]:
                                        self.queues_list[astid][queue[0]] = {'agents' : {},
                                                                             'channels' : []}
                                        for field in self.QueueStats:
                                                self.queues_list[astid][queue[0]][field] = ''

                for var, val in self.xivoconf.iteritems():
                        if var.find('-') > 0:
                                [name, prop] = var.split('-', 1)
                                if name not in self.capas:
                                        self.capas[name] = cti_capas.Capabilities()
                                if prop == 'display':
                                        self.capas[name].setdisplay(val)
                                elif prop == 'xlets':
                                        self.capas[name].setxlets(val)
                                elif prop == 'funcs':
                                        self.capas[name].setfuncs(val)
                                elif prop == 'maxgui':
                                        self.capas[name].setmaxgui(val)
                                elif prop == 'appliname':
                                        self.capas[name].setappliname(val)
                return

        def set_configs(self, configs):
                self.configs = configs
                return

        def set_phonelist(self, plist):
                self.plist = plist
                return

        def set_contextlist(self, ctxlist):
                self.ctxlist = ctxlist
                return

        def updates(self):
                self.alist_ng.update()
                self.qlist_ng.update()
                self.ulist_ng.update()
                # check : agentnumber should be unique
                return

        def set_userlist_urls(self, urls):
                self.ulist_ng.getuserslist(urls)

        def getagentslist(self, dlist):
                lalist = {}
                return lalist

        def getqueueslist(self, dlist):
                lqlist = {}
                return lqlist

        def getuserslist(self, dlist):
                lulist = {}
                for c, d in dlist.iteritems():
                        if len(d) > 9:
                                lulist[c] = {'user'     : d[0].split('@')[0],
                                             'company'  : d[0].split('@')[1],
                                             'passwd'   : d[1],
                                             'capaid'   : d[2],
                                             'fullname' : d[3] + ' ' + d[4],
                                             'astid'    : d[5],
                                             'phonenum' : d[6],
                                             'init'     : True,
                                             'state'    : 'unknown',
                                             'agentnum' : d[9],
                                             'techlist' : d[7],
                                             'context'  : d[8]}
                return lulist

        def users(self):
                return self.ulist_ng.users()
        def connected_users(self):
                return self.ulist_ng.connected_users()


        def askstatus(self, astid, npl):
                for a, b in npl.iteritems():
                        self.amilist.execute(astid, 'sendextensionstate', b[0], b[1])
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
                        log_debug(SYSLOG_WARNING, '--- exception --- (__send_msg_to_cti_client_byagentid__) : %s' % str(exc))
                return


        def __send_msg_to_cti_clients__(self, strupdate):
                try:
                        if strupdate is not None:
                                for userinfo in self.ulist_ng.userlist.itervalues():
                                        self.__send_msg_to_cti_client__(userinfo, strupdate)
                except Exception, exc:
                        log_debug(SYSLOG_WARNING, '--- exception --- (__send_msg_to_cti_clients__) : %s' % str(exc))
                return


##        # actions = { SHEET_EVENT_INCOMING    : SHEET_ACTION_BOT | SHEET_ACTION_XCPOPUP,
##        actions = { SHEET_EVENT_AGI         : SHEET_ACTION_XCFULL,

        actions = { SHEET_EVENT_AGI         : SHEET_ACTION_XCFULL,
                    SHEET_EVENT_INCOMING    : SHEET_ACTION_BOT | SHEET_ACTION_XCPOPUP,
                    SHEET_EVENT_OUTGOING    : SHEET_ACTION_BOT,
                    SHEET_EVENT_AGENTLINK   : SHEET_ACTION_URL,
                    SHEET_EVENT_AGENTCALLED : SHEET_ACTION_XCPOPUP
                    }
        # SHEET_ACTION_WHOCALLED

        def __sheet_alert__(self, where, event):
            if where in self.actions:
                if where == SHEET_EVENT_OUTGOING:
                        exten = event.get('Extension')
                        application = event.get('Application')
                        if application == 'Dial' and exten.isdigit():
                                print self.actions
                                print event

                elif where == SHEET_EVENT_AGENTLINK:
                        dst = event.get('Channel2')[6:]
                        src = event.get('CallerID1')
                        chan = event.get('Channel1')
                        queuename = event.get('xivo_queuename')
                        linestosend = ['<?xml version="1.0" encoding="utf-8"?>',
                                       '<profile sessionid="47"><user>',
                                       '<info name="File d Attente" type="text"><![CDATA[<h1><b>%s</b></h1>]]></info>' % queuename,
                                       '<info name="Numero Appelant" type="phone"><![CDATA[%s]]></info>' % src]
                        if 'sheeturl' in self.xivoconf:
                                linestosend.append('<info name="Site" type="urlauto"><![CDATA[%s%s]]></info>'
                                                   % (self.xivoconf['sheeturl'], ''.join(random.sample('12345', 4))))
                        linestosend.extend(['<info name="channel" type="internal"><![CDATA[%s]]></info>' % chan,
                                            '<info name="nopopup" type="internal"></info>',
                                            '<message>help</message>',
                                            '</user></profile>'])
                        self.__send_msg_to_cti_client_byagentid__(dst, ''.join(linestosend))

##                        try:
##                                if 'sheeturl' in self.xivoconf:
##                                        f = urllib.urlopen('%s%s' % (self.xivoconf['sheeturl'], src))
##                        except Exception, exc:
##                                print '--- exception ---', exc, exc.__class__

                elif where == SHEET_EVENT_AGI:
                        pass

                elif where == SHEET_EVENT_PHONELINK:
                        pass

                elif where == SHEET_EVENT_AGENTCALLED:
                        pass

                elif where == SHEET_EVENT_INCOMING:
                        pass
##                ts = None
##                if not (clid == '' or (clid == '<unknown>' and is_normal_channel(chan))):
##                        if len(clid) > 7 and clid != '<unknown>':
##                                r1 = self.__build_customers__('default', clid)
##                                r = r1.split(';')
##                                if len(r) > 7:
##                                        ts = 'E;%s;%s;%s;%s' % (clid, r[6], r[7], chan)
##                                else:
##                                        ts = 'E;%s;;;%s' % (clid, chan)
##                        else:
##                                ts = 'I;%s;%s' % (clid, chan)
##                else:
##                        if clid == '<unknown>':
##                                ts = 'E;Numero Cache;;;%s' % chan
##                if ts is not None and astid == 'xivo':
##                        try:
##                                nsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
##                                nsock.connect(('127.0.0.1', 5151))
##                                nsock.sendall('%s\n' % ts)
##                                nsock.close()
##                        except Exception, exc:
##                                print '--- exception --- in nsock :', exc
                return


        # Methods to handle Asterisk AMI events
        def ami_dial(self, astid, event):
                src     = event.get("Source")
                dst     = event.get("Destination")
                clid    = event.get("CallerID")
                clidn   = event.get("CallerIDName")
                context = event.get("Context")
##                if clidn in self.queues_list[astid]:
##                        print event
                self.plist[astid].handle_ami_event_dial(src, dst, clid, clidn)
                return

        def ami_link(self, astid, event):
                chan1 = event.get("Channel1")
                chan2 = event.get("Channel2")
                clid1 = event.get("CallerID1")
                clid2 = event.get("CallerID2")
                if chan2.startswith('Agent/'):
                        msg = self.__build_agupdate__(['agentlink', astid, chan2[6:]])
                        self.__send_msg_to_cti_clients__(msg)
                        # To identify which queue a call comes from, we match a previous AMI Leave event, not too far away
                        # in the past (1 second here), that involved the same channel as the one catched here.
                        # Any less-tricky-method is welcome, though.
                        if chan1 in self.queues_channels_list[astid]:
                                t1 = time.time()
                                [qname, t0] = self.queues_channels_list[astid][chan1]
                                del self.queues_channels_list[astid][chan1]
                                # if (t1 - t0) < 1:
                                event['xivo_queuename'] = qname
                                self.__sheet_alert__(SHEET_EVENT_AGENTLINK, event)
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
                                msg = self.__build_agupdate__(['phonelink', astid, ag1])
                                self.__send_msg_to_cti_clients__(msg)
                        if ag2 is not None:
                                msg = self.__build_agupdate__(['phonelink', astid, ag2])
                                self.__send_msg_to_cti_clients__(msg)
                self.plist[astid].handle_ami_event_link(chan1, chan2, clid1, clid2)

                return

        def ami_unlink(self, astid, event):
                chan1 = event.get("Channel1")
                chan2 = event.get("Channel2")
                clid1 = event.get("CallerID1")
                clid2 = event.get("CallerID2")
                if chan2.startswith('Agent/'):
                        msg = self.__build_agupdate__(['agentunlink', astid, chan2[6:]])
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
                                msg = self.__build_agupdate__(['phoneunlink', astid, ag1])
                                self.__send_msg_to_cti_clients__(msg)
                        if ag2 is not None:
                                msg = self.__build_agupdate__(['phoneunlink', astid, ag2])
                                self.__send_msg_to_cti_clients__(msg)
                self.plist[astid].handle_ami_event_unlink(chan1, chan2, clid1, clid2)
                return

        def ami_hangup(self, astid, event):
                chan  = event.get("Channel")
                cause = event.get("Cause-txt")
                self.plist[astid].handle_ami_event_hangup(chan, cause)
                return

        def ami_response_extensionstatus(self, astid, event):
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
        # seem to be superseded by OriginateResponse in asterisk 1.4 :
        # {'Uniqueid': '1213955764.88', 'CallerID': '6101', 'Exten': '6101', 'CallerIDNum': '6101', 'Response': 'Success', 'Reason': '4', 'Context': 'ctx-callbooster-agentlogin', 'CallerIDName': 'operateur', 'Privilege': 'call,all', 'Event': 'OriginateResponse', 'Channel': 'SIP/102-081f6730'}

        def ami_messagewaiting(self, astid, event):
                return
        def ami_newcallerid(self, astid, event):
                return

        def ami_newexten(self, astid, event):
                self.__sheet_alert__(SHEET_EVENT_OUTGOING, event)
                return

        def ami_newchannel(self, astid, event):
                self.__sheet_alert__(SHEET_EVENT_INCOMING, event)
                return

        def ami_parkedcall(self, astid, event):
                channel = event.get('Channel')
                cfrom   = event.get('From')
                exten   = event.get('Exten')
                timeout = event.get('Timeout')
                strupdate = 'parkedcall=%s;%s;%s;%s;%s' %(astid, channel, cfrom, exten, timeout)
                self.__send_msg_to_cti_clients__(strupdate)
                return
        
        def ami_unparkedcall(self, astid, event):
                channel = event.get('Channel')
                cfrom   = event.get('From')
                exten   = event.get('Exten')
                strupdate = 'unparkedcall=%s;%s;%s;%s;unpark' %(astid, channel, cfrom, exten)
                self.__send_msg_to_cti_clients__(strupdate)
                return
        
        def ami_parkedcallgiveup(self, astid, event):
                channel = event.get('Channel')
                exten   = event.get('Exten')
                strupdate = 'parkedcallgiveup=%s;%s;;%s;giveup' %(astid, channel, exten)
                self.__send_msg_to_cti_clients__(strupdate)
                return
        
        def ami_parkedcalltimeout(self, astid, event):
                channel = event.get('Channel')
                exten   = event.get('Exten')
                strupdate = 'parkedcalltimeout=%s;%s;;%s;timeout' %(astid, channel, exten)
                self.__send_msg_to_cti_clients__(strupdate)
                return
        
        def ami_agentlogin(self, astid, event):
                print 'AMI Agent', astid, event
                return
        def ami_agentlogoff(self, astid, event):
                print 'AMI Agent', astid, event
                return

        def ami_agentcallbacklogin(self, astid, event):
                agent = event.get('Agent')
                loginchan = event.get('Loginchan')
                if astid in self.agents_list and agent in self.agents_list[astid]:
                        self.agents_list[astid][agent]['status'] = 'AGENT_IDLE'
                        self.agents_list[astid][agent]['phonenum'] = event.get('Loginchan')
                msg = self.__build_agupdate__(['agentlogin', astid, agent, loginchan])
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
                msg = self.__build_agupdate__(['agentlogout', astid, agent, loginchan])
                print 'ami_agentcallbacklogoff', msg
                self.__send_msg_to_cti_clients__(msg)
                return

        def ami_agentcalled(self, astid, event):
                print 'AMI Agent', astid, event
                return
        def ami_agentcomplete(self, astid, event):
                print 'AMI Agent', astid, event
                return
        def ami_agentdump(self, astid, event):
                print 'AMI Agent', astid, event
                return
        def ami_agentconnect(self, astid, event):
                print 'AMI Agent', astid, event
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
                print 'AMI AgentsComplete', astid
                if astid in self.agents_list:
                        for aname, aargs in self.agents_list[astid].iteritems():
                                print ' (a) ', aname, aargs
                return

        def ami_queuecallerabandon(self, astid, event):
                # Asterisk 1.4 event
                # {'Queue': 'qcb_00000', 'OriginalPosition': '1', 'Uniqueid': '1213891256.41', 'Privilege': 'agent,all', 'Position': '1', 'HoldTime': '2', 'Event': 'QueueCallerAbandon'}
                return
        
        def ami_queueentry(self, astid, event):
                queue = event.get('Queue')
                if astid not in self.queues_list:
                        self.queues_list[astid] = {}
                if queue not in self.queues_list[astid]:
                        self.queues_list[astid][queue] = {'agents' : {},
                                                          'channels' : []}
                        for field in self.QueueStats:
                                self.queues_list[astid][queue][field] = ''
                # {'CallerID': '102', 'CallerIDName': 'qcb_00000', 'Position': '1', 'Channel': 'SIP/102-081e5f00', 'Wait': '8'}
                return
                
        def ami_queuememberadded(self, astid, event):
                queue = event.get('Queue')
                location = event.get('Location')
                paused = event.get('Paused')
                msg = self.__build_agupdate__(['joinqueue', astid, location[6:], queue, paused])
                self.__send_msg_to_cti_clients__(msg)
                if astid not in self.queues_list:
                        self.queues_list[astid] = {}
                if queue not in self.queues_list[astid]:
                        self.queues_list[astid][queue] = {'agents' : {},
                                                          'channels' : []}
                        for field in self.QueueStats:
                                self.queues_list[astid][queue][field] = ''
                if location not in self.queues_list[astid][queue]['agents']:
                        self.queues_list[astid][queue]['agents'][location] = [event.get('Paused'), event.get('Status'), event.get('Membership')]
                return

        def ami_queuememberremoved(self, astid, event):
                queue = event.get('Queue')
                location = event.get('Location')
                msg = self.__build_agupdate__(['leavequeue', astid, location[6:], queue])
                self.__send_msg_to_cti_clients__(msg)
                if astid in self.queues_list and queue in self.queues_list[astid] and location in self.queues_list[astid][queue]['agents']:
                        del self.queues_list[astid][queue]['agents'][location]
                return


        def __build_agupdate__(self, arrgs):
                arg = ':'.join(['agu',
                                '/'.join(arrgs)])
                return 'update-agents=%s' % arg


        def ami_queuememberstatus(self, astid, event):
                status = event.get('Status')
                queue = event.get('Queue')
                location = event.get('Location')
                paused = event.get('Paused')

                agentnum = location[6:]

                if astid in self.queues_list and queue in self.queues_list[astid] and location in self.queues_list[astid][queue]['agents']:
                        self.queues_list[astid][queue]['agents'][location] = [paused, status, event.get('Membership')]

                msg = self.__build_agupdate__(['queuememberstatus', astid, agentnum, queue, status, paused])
                self.__send_msg_to_cti_clients__(msg)

                # status = 3 => ringing
                # status = 1 => do not ring anymore => the one who has not gone to '1' among the '3's is the one who answered ...
                # 5 is received when unavailable members of a queue are attempted to be joined ... use agentcallbacklogoff to detect exit instead
                # + Link
                return

        def ami_queuememberpaused(self, astid, event):
                paused = event.get('Paused')
                queue = event.get('Queue')
                location = event.get('Location')
                if location.startswith('Agent/'):
                        if astid in self.queues_list and queue in self.queues_list[astid] and location in self.queues_list[astid][queue]['agents']:
                                self.queues_list[astid][queue]['agents'][location][0] = event.get('Paused')
                        agname = location[6:]
                        if paused == '0':
                                msg = self.__build_agupdate__(['unpaused', astid, agname, queue])
                                self.__send_msg_to_cti_clients__(msg)
                        else:
                                msg = self.__build_agupdate__(['paused', astid, agname, queue])
                                self.__send_msg_to_cti_clients__(msg)
                return

        QueueStats = ['ServicelevelPerf', 'Abandoned', 'Max', 'Completed', 'ServiceLevel', 'Weight', 'Holdtime', 'Calls']

        def ami_queueparams(self, astid, event):
                queue = event.get('Queue')
                # 'ServicelevelPerf': '0.0', 'Abandoned': '0', 'Max': '0', 'Completed': '0', 'ServiceLevel': '0', 'Weight': '0', 'Holdtime': '0'
                # qcb_00001    has 0 calls (max unlimited) in 'roundrobin' strategy (1s holdtime), W:0, C:1, A:1, SL:0.0% within 0s
                if astid not in self.queues_list:
                        self.queues_list[astid] = {}
                if queue not in self.queues_list[astid]:
                        self.queues_list[astid][queue] = {'agents' : {},
                                                          'channels' : []}
                        for field in self.QueueStats:
                                self.queues_list[astid][queue][field] = event.get(field)
                return

        def ami_queuemember(self, astid, event):
                queue = event.get('Queue')
                location = event.get('Location')
                if astid not in self.queues_list:
                        self.queues_list[astid] = {}
                if queue not in self.queues_list[astid]:
                        self.queues_list[astid][queue] = {'agents' : {},
                                                          'channels' : []}
                        for field in self.QueueStats:
                                self.queues_list[astid][queue][field] = ''
                if location not in self.queues_list[astid][queue]['agents']:
                        self.queues_list[astid][queue]['agents'][location] = [event.get('Paused'), event.get('Status'), event.get('Membership')]
                return

        def ami_queuestatuscomplete(self, astid, event):
                print 'AMI QueueStatusComplete', astid
                if astid in self.queues_list:
                        for qname, qarg in self.queues_list[astid].iteritems():
                                print ' (q) ', qname, qarg
                                self.amilist.execute(astid, 'sendcommand', 'Command', [('Command', 'show queue %s' % qname)])
                return

        def ami_userevent(self, astid, event):
                return
        def ami_meetmejoin(self, astid, event):
                return
        def ami_meetmeleave(self, astid, event):
                return
        def ami_status(self, astid, event):
                return


        def ami_join(self, astid, event):
                print 'AMI Queue', event
                chan  = event.get('Channel')
                clid  = event.get('CallerID')
                queue = event.get('Queue')
                count = event.get('Count')
                
                if astid not in self.queues_list:
                        self.queues_list[astid] = {}
                if queue not in self.queues_list[astid]:
                        self.queues_list[astid][queue] = {'agents' : {},
                                                          'channels' : []}
                        for field in self.QueueStats:
                                self.queues_list[astid][queue][field] = ''
                if chan not in self.queues_list[astid][queue]['channels']:
                        self.queues_list[astid][queue]['channels'].append(chan)
                self.queues_list[astid][queue]['Calls'] = count
                self.__send_msg_to_cti_clients__('update-queues=queuechannels/%s/%s/%s' % (astid, queue, count))

                print 'AMI Queue JOIN ', queue, chan, count
                return

        def ami_leave(self, astid, event):
                # print 'AMI Queue', event
                chan  = event.get('Channel')
                queue = event.get('Queue')
                count = event.get('Count')

                if astid in self.queues_list and queue in self.queues_list[astid] and chan in self.queues_list[astid][queue]['channels']:
                        self.queues_list[astid][queue]['channels'].remove(chan)
                        print 'AMI Queue LEAVE', len(self.queues_list[astid][queue]['channels']), count
                self.queues_list[astid][queue]['Calls'] = count
                self.__send_msg_to_cti_clients__('update-queues=queuechannels/%s/%s/%s' % (astid, queue, count))

                if astid not in self.queues_channels_list:
                        self.queues_channels_list[astid] = {}
                if chan not in self.queues_channels_list[astid]:
                        self.queues_channels_list[astid][chan] = [queue, time.time()]

                print 'AMI Queue LEAVE', queue, chan, count
                self.amilist.execute(astid, 'sendqueuestatus', queue)
                return
        
        def ami_rename(self, astid, event):
                return
        # END of AMI events

        def handle_agi(self, astid, message):
                m = re.match('PUSH (\S+) (\S+) <(\S*)> ?(.*)', message.strip())
                if m != None and SHEET_EVENT_AGI in self.actions:
                        called = m.group(1)[3:]
                        callerid = m.group(2)
                        callerctx = m.group(3)
                        msg = m.group(4)
                        print called, callerid, callerctx, msg
                        linestosend = ['<?xml version="1.0" encoding="utf-8"?>',
                                       '<profile sessionid="47"><user>',
                                       '<info name="Numero Appelant" type="phone"><![CDATA[%s]]></info>' % callerid,
                                       '<info name="Numero Appele" type="text"><![CDATA[<b>%s</b>]]></info>' % called,
                                       '<info name="called" type="internal"><![CDATA[%s]]></info>' % called,
                                       '<message>help %s</message>' % called,
                                       '</user></profile>']
                        userinfo = self.ulist_ng.finduser(called)
                        self.__send_msg_to_cti_client__(userinfo, ''.join(linestosend))
                        return 'USER %s STATE available CIDNAME %s' % (called, 'tobedefined')


        def message_srv2clt(self, sender, message):
                return 'message=%s::%s' %(sender, message)
        def dmessage_srv2clt(self, message):
                return self.message_srv2clt('daemon-announce', message)

        
        def phones_update(self, function, args):
                strupdate = ''
                if function == 'update':
                        strupdate = 'phones-update=' + ':'.join(args)
                elif function == 'noupdate':
                        strupdate = 'phones-noupdate=' + ':'.join(args)
                elif function == 'signal-deloradd':
                        [astid, ndel, nadd, ntotal] = args
                        strupdate = 'phones-signal-deloradd=%s;%d;%d;%d' % (astid, ndel, nadd, ntotal)
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
                                        repstr = 'faxsend=%s' % newfax.reference
                        elif icommand.name == 'message':
                                if self.capas[capaid].match_funcs(ucapa, 'messages'):
                                        self.__send_msg_to_cti_clients__(self.message_srv2clt('%s/%s' %(astid, username),
                                                                                             '<%s>' % icommand.args[0]))
                        elif icommand.name in ['originate', 'transfer', 'atxfer']:
                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                        repstr = self.__originate_or_transfer__(userinfo,
                                                                                # "%s/%s" %(astid, username),
                                                                                [icommand.name, icommand.args[0], icommand.args[1]])
                        elif icommand.name == 'hangup':
                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                        repstr = self.__hangup__("%s/%s" %(astid, username),
                                                                 icommand.args[0], True)
                        elif icommand.name == 'simplehangup':
                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                        repstr = self.__hangup__("%s/%s" %(astid, username),
                                                                 icommand.args[0], False)
                        elif icommand.name == 'pickup':
                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                        z = icommand.args[0].split('/')
                                        self.amilist.execute(z[1], 'sendcommand', 'Command', [('Command', 'sip notify event-talk %s' % z[3])])

                        elif icommand.name in ['phones-list', 'phones-add', 'phones-del']:
                                if self.capas[capaid].match_funcs(ucapa, 'calls,switchboard,search,history'):
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
                                     uinfo.get('agentnum')]
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
                                                  uinfo.get('agentnum')])
                                self.__send_msg_to_cti_client__(userinfo, 'users-list=%d;%s' % (len(f), ';'.join(f)))
                                repstr = None

                        elif icommand.name == 'queues-list':
                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                        allowed = 1
                                        if 'agentnum' in userinfo:
                                                agnum = userinfo['agentnum']
                                                if 'agents_unallowed' in self.xivoconf:
                                                        unallowed = self.xivoconf['agents_unallowed'].split(',')
                                                        if agnum in unallowed:
                                                                allowed = 0
                                        for astid, qlist in self.queues_list.iteritems():
                                                lst = []
                                                for qname, qprop in qlist.iteritems():
                                                        lstt = []
                                                        for prop in self.QueueStats:
                                                                lstt.append('%s:%s' % (prop, qprop.get(prop)))
                                                        lst.append('%s:%s' % (qname, ':'.join(lstt)))
                                                self.__send_msg_to_cti_client__(userinfo,
                                                                                'queues-list=%s;%d;%s' %(astid, allowed,
                                                                                                         ','.join(lst)))
                                repstr = None

                        elif icommand.name == 'agents-list':
                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                        for astid, aglist in self.agents_list.iteritems():
                                                lst = []
                                                for agname, agprop in aglist.iteritems():
                                                        agid = 'Agent/%s' % agname
                                                        qjoined = []
                                                        qunpaused = []
                                                        if astid in self.queues_list:
                                                                for qref, ql in self.queues_list[astid].iteritems():
                                                                        if agid in ql['agents']:
                                                                                qjoined.append(qref)
                                                                                agpropq = ql['agents'][agid]
                                                                                if agpropq[0] == '0':
                                                                                        qunpaused.append(qref)
                                                                        else:
                                                                                pass
                                                        if agprop['status'] == 'AGENT_LOGGEDOFF':
                                                                lst.append('%s:0:%s:%s:%s' % (agname, agprop['name'], ','.join(qjoined), ','.join(qunpaused)))
                                                        else:
                                                                lst.append('%s:1:%s:%s:%s' % (agname, agprop['name'], ','.join(qjoined), ','.join(qunpaused)))
                                                self.__send_msg_to_cti_client__(userinfo,
                                                                                'agents-list=%s;%s' %(astid,
                                                                                                      ';'.join(lst)))
                                repstr = None

                        elif icommand.name == 'queue-status':
                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                        astid = icommand.args[0]
                                        qname = icommand.args[1]
                                        lst = []
                                        if astid in self.queues_list and qname in self.queues_list[astid]:
                                                for agid, agprop in self.queues_list[astid][qname]['agents'].iteritems():
                                                        if agid.startswith('Agent/'):
                                                                lst.append('%s,%s,%s' % (agid[6:], agprop[0], agprop[1]))
                                                        else:
                                                                lst.append('%s,%s,%s' % (agid, agprop[0], agprop[1]))
                                        self.__send_msg_to_cti_client__(userinfo,
                                                                        'queue-status=%s;%s;%s' %(astid, qname, ';'.join(lst)))

                                repstr = None


                        elif icommand.name == 'agents-status':
                                # issued by one user when he logs in
                                if self.capas[capaid].match_funcs(ucapa, 'agents') and len(userinfo.get('agentnum')) > 0:
                                    for astid in self.queues_list:
                                        agname = userinfo.get('agentnum')
                                        agid = 'Agent/%s' % agname
                                        qref_joined = []
                                        qref_unjoined = []

                                        for qref, ql in self.queues_list[astid].iteritems():
                                                if agid in ql['agents']:
                                                        qref_joined.append(qref)
                                                else:
                                                        qref_unjoined.append(qref)

                                        for qref in qref_unjoined:
                                                msg = self.__build_agupdate__(['leavequeue', astid, agname, qref])
                                                self.__send_msg_to_cti_client__(userinfo, msg)
                                        for qref in qref_joined:
                                                msg = self.__build_agupdate__(['joinqueue', astid, agname, qref])
                                                self.__send_msg_to_cti_client__(userinfo, msg)

                                        # lookup the logged in/out status of agent agname and sends it back to the requester
                                        if astid in self.agents_list and agname in self.agents_list[astid]:
                                                agprop = self.agents_list[astid][agname]
                                                if agprop['status'] == 'AGENT_LOGGEDOFF':
                                                        msg = self.__build_agupdate__(['agentlogout', astid, agname, agprop['phonenum']])
                                                else:
                                                        msg = self.__build_agupdate__(['agentlogin', astid, agname, agprop['phonenum']])
                                                self.__send_msg_to_cti_client__(userinfo, msg)
                                repstr = None


                        elif icommand.name == 'agent-status':
                                # issued by one user when he requests the status for one given agent
                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                        astid = icommand.args[0]
                                        agname = icommand.args[1]
                                        agid = 'Agent/%s' % agname
                                        qref_joined = []
                                        qref_unjoined = []

                                        if astid in self.queues_list:
                                                for qref, ql in self.queues_list[astid].iteritems():
                                                        if agid in ql['agents']:
                                                                agprop = ql['agents'][agid]
                                                                qref_joined.append(':'.join([qref, agprop[0], agprop[1]]))
                                                        else:
                                                                qref_unjoined.append(qref)

                                        # lookup the logged in/out status of agent agname and sends it back to the requester
                                        if astid in self.agents_list and agname in self.agents_list[astid]:
                                                agprop = self.agents_list[astid][agname]
                                                if agprop['status'] == 'AGENT_LOGGEDOFF':
                                                        msg = 'agent-status=%s;%s;0;%s;%s' %(astid, agname,
                                                                                             ','.join(qref_joined),
                                                                                             ','.join(qref_unjoined))
                                                else:
                                                        msg = 'agent-status=%s;%s;%s;%s;%s' %(astid, agname,
                                                                                              agprop['phonenum'],
                                                                                              ','.join(qref_joined),
                                                                                              ','.join(qref_unjoined))
                                                self.__send_msg_to_cti_client__(userinfo, msg)
                                repstr = None

                        elif icommand.name == 'agent':
                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                        repstr = self.__agent__(userinfo, icommand.args)

                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- (manage_cticommand) %s %s %s %s'
                                  %(icommand.name, str(icommand.args), str(userinfo.get('login').get('connection')), str(exc)))

                if repstr is not None: # might be useful to reply sth different if there is a capa problem for instance, a bad syntaxed command
                        try:
                                userinfo.get('login').get('connection').sendall(repstr + '\n')
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- (sendall) attempt to send <%s ...> (%d chars) failed : %s'
                                          % (repstr[:40], len(repstr), str(exc)))
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
                                                sipcid = "SIP/%s" % num
                                                cidname = num
                                                if sipcid in self.plist[astid].normal:
                                                        cidname = '%s %s <%s>' %(self.plist[astid].normal[sipcid].calleridfirst,
                                                                                 self.plist[astid].normal[sipcid].calleridlast,
                                                                                 num)
                                                ry2 = HISTSEPAR.join([cidname, 'OUT', termin])
                                        else:   # display callerid for incoming calls
                                                ry2 = HISTSEPAR.join([x[1].replace('"', ''), 'IN', termin])
                                                
                                        reply.append(ry1)
                                        reply.append(HISTSEPAR)
                                        reply.append(ry2)
                                        reply.append(HISTSEPAR)
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- error : history : (client %s, termin %s) : %s'
                                          % (requester_id, termin, str(exc)))

                if len(reply) > 0:
                        return 'history=%s' % ''.join(reply)
                else:
                        return


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
                                if agentnum is not None:
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
                        log_debug(SYSLOG_ERR, '--- exception --- (regular update) : %s' % str(exc))


        def __phlist__(self):
                fullstat = []
                for astid, iplist in self.plist.iteritems():
                        for idx, pidx in iplist.normal.iteritems():
                                bstatus = ':'.join([pidx.context,
                                                    pidx.tech,
                                                    pidx.phoneid,
                                                    pidx.hintstatus])
                                if pidx.towatch:
                                        phoneinfo = ("ful",
                                                     astid,
                                                     bstatus,
                                                     pidx.build_fullstatlist() + ";")
                                        fullstat.append(':'.join(phoneinfo))
                return 'phones-list=%s' % ''.join(fullstat)


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
				    	    status = '0'
                                            for ast, qlist in self.agents_list.iteritems():
                                                    if userinfo.get('agentnum') in qlist:
                                                            if qlist[userinfo.get('agentnum')]['status'] != 'AGENT_LOGGEDOFF':
                                                                    status = '1'
                                                            else:
                                                                    status = '0'
                                                            break
                                            astqueues = []
                                            for ast, qlist in self.queues_list.iteritems():
                                                    for q, qq in qlist.iteritems():
                                                            if len(qq['agents']) > 0 and ('Agent/%s' % userinfo.get('agentnum')) in qq['agents']:
                                                                    astqueues.append(q)
                                            ag = '/'.join(['agentstatus', userinfo.get('astid'), userinfo.get('agentnum'), status, ','.join(astqueues)])
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
                                                    phoneinfo = ("ful",
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
##                                            phoneinfo = ("ful",
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
                                          %(str(reqlist), key, str(exc)))
                                return 'featuresget=%s;KO' % reqlist[0]

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
                                          %(str(reqlist), key, str(exc)))
                                return 'featuresget=%s;KO' % reqlist[0]

                if len(repstr) == 0:
                        repstr = 'KO'
                return 'featuresget=%s;%s' % (reqlist[0], repstr)


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
                                response = 'featuresput=%s;OK;%s;%s;' %(reqlist[0], key, value)
                        else:
                                response = 'featuresput=%s;KO' % reqlist[0]
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- features_put id=%s : %s'
                                  %(str(reqlist), str(exc)))
                        response = 'featuresput=%s;KO' % reqlist[0]
                return response


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
                                        proto_src = 'local'
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
                                log_debug(SYSLOG_ERR, '--- exception --- unable to originate ...')
                        if ret:
                                ret_message = 'originate OK'
                        else:
                                ret_message = 'originate KO'
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
        def __hangup__(self, requester, chan, peer_hangup):
                astid_src = chan.split("/")[1]
                ret_message = 'hangup KO from %s' % requester
                if astid_src in self.configs:
                        log_debug(SYSLOG_INFO, "%s is attempting a HANGUP : %s" %(requester, chan))
                        phone, channel = split_from_ui(chan)
                        if phone in self.plist[astid_src].normal:
                                if channel in self.plist[astid_src].normal[phone].chann:
                                        if peer_hangup:
                                                channel_peer = self.plist[astid_src].normal[phone].chann[channel].getChannelPeer()
                                                log_debug(SYSLOG_INFO, "UI action : %s : hanging up <%s> and <%s>"
                                                          %(self.configs[astid_src].astid , channel, channel_peer))
                                        else:
                                                channel_peer = ''
                                                log_debug(SYSLOG_INFO, "UI action : %s : hanging up <%s>"
                                                          %(self.configs[astid_src].astid , channel))
                                        ret = self.amilist.execute(astid_src, 'hangup', channel, channel_peer)
                                        if ret > 0:
                                                ret_message = 'hangup OK (%d) <%s>' %(ret, chan)
                                        else:
                                                ret_message = 'hangup KO : socket request failed'
                                else:
                                        ret_message = 'hangup KO : no such channel <%s>' % channel
                        else:
                                ret_message = 'hangup KO : no such phone <%s>' % phone
                else:
                        ret_message = 'hangup KO : no such asterisk id <%s>' % astid_src
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
                                          %(cfg.astid, str(exc)))
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

                self.__send_msg_to_cti_clients__('presence=%s;%s;%s' % (company, username, userinfo['state']))

                return None


        # \brief Builds the full list of customers in order to send them to the requesting client.
        # This should be done after a command called "customers".
        # \return a string containing the full customers list
        # \sa manage_tcp_connection
        def __build_customers__(self, ctx, searchpatterns):
                fulllist = []
                header = ''
                if ctx in self.ctxlist.ctxlist:
                        for dirsec, dirdef in self.ctxlist.ctxlist[ctx].iteritems():
                                y = self.__build_customers_bydirdef__(dirsec, searchpatterns, dirdef)
                                header = 'directory-response=%d;%s' %(len(dirdef.search_valid_fields), ';'.join(dirdef.search_titles))
                                fulllist.extend(y)
                else:
                        log_debug(SYSLOG_WARNING, 'there has been no section defined for context %s : can not proceed directory search' % ctx)
                if len(fulllist) == 0:
                        return header
                else:
                        return header + ';' + ';'.join(fulllist)


        def __build_customers_bydirdef__(self, dirname, searchpatterns, z):
                searchpattern = ' '.join(searchpatterns)
                fullstatlist = []

                if searchpattern == "":
                        return []

                dbkind = z.uri.split(":")[0]
                if dbkind == 'ldap':
                        selectline = []
                        for fname in z.search_matching_fields:
                                if searchpattern == "*":
                                        selectline.append("(%s=*)" % fname)
                                else:
                                        selectline.append("(%s=*%s*)" %(fname, searchpattern))

                        try:
                                ldapid = xivo_ldap.xivo_ldap(z.uri)
                                results = ldapid.getldap("(|%s)" % ''.join(selectline),
                                                         z.search_matching_fields)
                                if results is not None:
                                        for result in results:
                                                result_v = {}
                                                for f in z.search_matching_fields:
                                                        if f in result[1]:
                                                                result_v[f] = result[1][f][0]
                                                fullstatlist.append(';'.join(z.result_by_valid_field(result_v)))
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- ldaprequest : %s' % str(exc))

                elif dbkind == 'file':
                        f = urllib.urlopen(z.uri)
                        delimit = ':'
                        header = f.next()
                        headerfields = header.strip().split(delimit)
                        for line in f:
                                ll = line.strip()
                                if ll.lower().find(searchpattern.lower()) >= 0:
                                        t = ll.split(delimit)
                                        futureline = []
                                        for mf in z.search_matching_fields:
                                                idx = headerfields.index(mf)
                                                futureline.append(t[idx])
                                        fullstatlist.append(';'.join(futureline))

                elif dbkind == 'http':
                        f = urllib.urlopen('%s%s' % (z.uri, searchpattern))
                        delimit = ':'
                        header = f.next()
                        headerfields = header.strip().split(delimit)
                        for line in f:
                                ll = line.strip()
                                t = ll.split(delimit)
                                futureline = []
                                for mf in z.search_matching_fields:
                                        idx = headerfields.index(mf)
                                        futureline.append(t[idx])
                                fullstatlist.append(';'.join(futureline))

                elif dbkind != '':
                        if searchpattern == '*':
                                whereline = ''
                        else:
                                wl = []
                                for fname in z.search_matching_fields:
                                        wl.append("%s REGEXP '%s'" %(fname, searchpattern))
                                whereline = 'WHERE ' + ' OR '.join(wl)

                        try:
                                conn = anysql.connect_by_uri(z.uri)
                                cursor = conn.cursor()
                                cursor.query("SELECT ${columns} FROM " + z.sqltable + " " + whereline,
                                             tuple(z.search_matching_fields),
                                             None)
                                results = cursor.fetchall()
                                conn.close()
                                for result in results:
                                        result_v = {}
                                        n = 0
                                        for f in z.search_matching_fields:
                                                result_v[f] = result[n]
                                                n += 1
                                        fullstatlist.append(';'.join(z.result_by_valid_field(result_v)))
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- sqlrequest : %s' % str(exc))
                else:
                        log_debug(SYSLOG_WARNING, "no database method defined - please fill the dir_db_uri field of the <%s> context" % dirname)

                uniq = {}
                fullstatlist.sort()
                fullstat_body = []
                for fsl in [uniq.setdefault(e,e) for e in fullstatlist if e not in uniq]:
                        fullstat_body.append(fsl)
                return fullstat_body


        def handle_fagi(self, fastagi):
                """
                Previously known as 'xivo_push'
                """
                # check capas !
                proto   = fastagi.get_variable('XIVO_INTERFACE').split('/')[0].lower()
                exten   = fastagi.get_variable('REAL_DSTNUM')
                context = fastagi.get_variable('REAL_CONTEXT')
                calleridnum  = fastagi.env['agi_callerid']
                calleridname = fastagi.env['agi_calleridname']
                msgext = fastagi.get_variable('CALLTYPE')
                user = proto + exten
                msg = 'PUSH %s %s <%s> %s' % (user, calleridnum, context, msgext)

                list = self.handle_agi('xivo', msg).strip().split(' ', 5)

                if len(list) < 5 or list[0] == 'ERROR':
                        print "Received an ERROR for user <%s> : %s" % (user, str(list))
                else:
                        # USER xxx STATE xxx CIDNAME xxx
                        clientstate = list[3]

                        if calleridnum == 'unknown':
                                # to set according to os.getenv('LANG') or os.getenv('LANGUAGE') later on ?
                                calleridnum = 'Inconnu'
                        if calleridname == 'unknown':
                                calleridname = ''

                        if len(list) > 5:
                                calleridtoset = '"%s"<%s>' %(list[5], calleridnum)
                        else:
                                calleridtoset = '"%s"<%s>' %(calleridname, calleridnum)
                        print 'The Caller Id will be set to %s' % calleridtoset
                        fastagi.set_callerid(calleridtoset)

                if clientstate == 'available' or clientstate == 'nopresence':
                        fastagi.set_variable('XIVO_AIMSTATUS', 0)
                elif clientstate == 'away':
                        fastagi.set_variable('XIVO_AIMSTATUS', 1)
                elif clientstate == 'donotdisturb':
                        fastagi.set_variable('XIVO_AIMSTATUS', 2)
                elif clientstate == 'outtolunch':
                        fastagi.set_variable('XIVO_AIMSTATUS', 3)
                elif clientstate == 'berightback':
                        fastagi.set_variable('XIVO_AIMSTATUS', 4)
                else:
                        print_verbose("Unknown user's availability status : <%s>" % clientstate)
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
        s1 = fullname.split("/")
        if len(s1) == 5:
                phone = s1[3] + "/" + channel_splitter(s1[4])
                channel = s1[3] + "/" + s1[4]
        return [phone, channel]
