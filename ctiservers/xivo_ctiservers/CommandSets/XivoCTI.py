# vim: set fileencoding=utf-8 :
# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2009 Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
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
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""
This is the XivoCTI class.
"""

import base64
import cjson
import csv
import logging
import os
import random
import re
import socket
import sha
import string
import sys
import threading
import time
import types
import urllib
import zlib
import struct
import Queue
from xivo_ctiservers import cti_capas
from xivo_ctiservers import cti_fax
from xivo_ctiservers import cti_presence
from xivo_ctiservers import cti_userlist
from xivo_ctiservers import cti_urllist

from xivo_ctiservers import cti_agentlist
from xivo_ctiservers import cti_queuelist
from xivo_ctiservers import cti_grouplist
from xivo_ctiservers import cti_phonelist
from xivo_ctiservers import cti_meetmelist
from xivo_ctiservers import cti_voicemaillist
from xivo_ctiservers import cti_incomingcalllist
from xivo_ctiservers import cti_trunklist
from xivo_ctiservers import cti_phonebook
from xivo_ctiservers.cti_sheetmanager import SheetManager

from xivo_ctiservers import xivo_commandsets
from xivo_ctiservers import xivo_ldap
from xivo_ctiservers.xivo_commandsets import BaseCommand
from xivo import anysql
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite

log = logging.getLogger('xivocti')

XIVOVERSION_NUM = '1.0'
XIVOVERSION_NAME = 'dalek'
REQUIRED_CLIENT_VERSION = 5680
__revision__ = __version__.split()[1]
__alphanums__ = string.uppercase + string.lowercase + string.digits
HISTSEPAR = ';'
AMI_ORIGINATE = 'originate'
MONITORDIR = '/var/spool/asterisk/monitor'

# some default values to display
CONTEXT_UNKNOWN = 'undefined_context'
AGENT_NO_PHONENUM = 'N.A.'
PRESENCE_UNKNOWN = 'Absent'
CALLERID_UNKNOWN_NUM = 'Inconnu'
CALLERID_UNKNOWN_NAME = 'Inconnu'

rename_phph = [False, False, True, True]
rename_trtr = [True, True, False, False]
rename_phtr = [False, True, True, False]
rename_trph = [True, False, False, True]

class XivoCTICommand(BaseCommand):

        xdname = 'XiVO Daemon'
        
        fullstat_heavies = {}
        commnames = ['login_id', 'login_pass', 'login_capas',
                     'history', 'directory-search',
                     'featuresget', 'featuresput',
                     'getguisettings',
                     'logclienterror',
                     'powerevent',
                     'phones',
                     'agents', 'agent',
                     'queues',
                     'groups',
                     'users',
                     'callcampaign',
                     'logout',
                     'faxsend',
                     'filetransfer',
                     'database',
                     'meetme',
                     'endinit',
                     'message',
                     'actionfiche',
                     'availstate',
                     'keepalive',
                     'originate', 'transfer', 'atxfer', 'hangup', 'simplehangup', 'pickup',
                     'sheet']
        astid_vars = ['stats_queues',
                      'last_agents',
                      'last_queues',
                      'attended_targetchannels',
                      'uniqueids',
                      'channels',
                      'parkedcalls',
                      'ignore_dtmf',
                      'queues_channels_list',
                      'origapplication',
                      'events_link',
                      'localchans',
                      'amirequests',
                      'getvar_requests']
        
        def __init__(self, amilist, ctiports, queued_threads_pipe):
		BaseCommand.__init__(self)
                self.amilist = amilist
                self.capas = {}
                self.ulist_ng = cti_userlist.UserList()
                self.ulist_ng.setcommandclass(self)
                self.weblist = { 'agents' : {},
                                 'queues' : {},
                                 'groups' : {},
                                 'phones' : {},
                                 'trunks' : {},
                                 'meetme' : {},
                                 'incomingcalls' : {},
                                 'voicemail' : {},
                                 'phonebook' : {} }
                self.weblistprops = {'agents' : {'classfile' : cti_agentlist, 'classname' : 'AgentList'},
                                     'queues' : {'classfile' : cti_queuelist, 'classname' : 'QueueList'},
                                     'groups' : {'classfile' : cti_grouplist, 'classname' : 'GroupList'},
                                     'phones' : {'classfile' : cti_phonelist, 'classname' : 'PhoneList'},
                                     'trunks' : {'classfile' : cti_trunklist, 'classname' : 'TrunkList'},
                                     'meetme' : {'classfile' : cti_meetmelist, 'classname' : 'MeetmeList'},
                                     'incomingcalls' : {'classfile' : cti_incomingcalllist, 'classname' : 'IncomingCallList'},
                                     'voicemail' : {'classfile' : cti_voicemaillist, 'classname' : 'VoiceMailList'},
                                     'phonebook' : {'classfile' : cti_phonebook, 'classname' : 'PhoneBook'}
                                     }
                # self.plist_ng = cti_phonelist.PhoneList()
                # self.plist_ng.setcommandclass(self)
                self.transfers_buf = {}
                self.transfers_ref = {}
                self.filestodownload = {}
                self.faxes = {}
                self.queued_threads_pipe = queued_threads_pipe
                self.timerthreads_agentlogoff_retry = {}
                self.timerthreads_login_timeout = {}
                self.sheet_actions = {}
                self.ldapids = {}
                self.tqueue = Queue.Queue()
                self.presence_sections = {}
                self.interprefix = {}
                
                self.display_hints = {}
                
                # astid indexed hashes
                self.stats_queues = {}
                self.last_agents = {}
                self.last_queues = {}
                self.attended_targetchannels = {} # leaks
                self.uniqueids = {}
                self.channels = {}
                self.parkedcalls = {}
                self.ignore_dtmf = {}
                self.queues_channels_list = {}
                self.origapplication = {}
                self.events_link = {}
                self.localchans = {} # leaks
                # astid + actionid (AMI) indexed hashes
                self.amirequests = {}
                self.getvar_requests = {}
                
                self.logintimeout = 5
                self.save_memused = 0
                
                self.parting_astid = False
                self.parting_context = False
                
                self.required_client_version = REQUIRED_CLIENT_VERSION

                # new style
                self.sheetmanager = {}
                
                return
        
        def apnoea_tosave(self):
                # tosave = self.uniqueids
                tosave = {}
                return tosave
        
        def apnoea_rescue(self, torescue):
                # self.uniqueids = torescue
                # read a 'datetime' field in order to decide whether
                # it is worth to set the data or not
                return
        
        def get_list_commands(self):
                return ['json']
        
        def parsecommand(self, linein):
                params = linein.split()
                cmd = xivo_commandsets.Command(params[0], params[1:])
                cmd.struct = {}
                cmd.type = xivo_commandsets.CMD_OTHER
                try:
                        cmd.struct = cjson.decode(linein)
                        cmd.name = 'json'
                        if cmd.struct.get('class') == 'login_id':
                                cmd.type = xivo_commandsets.CMD_LOGIN_ID
                        elif cmd.struct.get('class') == 'login_pass':
                                cmd.type = xivo_commandsets.CMD_LOGIN_PASS
                        elif cmd.struct.get('class') == 'login_capas':
                                cmd.type = xivo_commandsets.CMD_LOGIN_CAPAS
                        elif cmd.struct.get('class') == 'filetransfer':
                                cmd.type = xivo_commandsets.CMD_TRANSFER
                except cjson.DecodeError:
                        log.warning('<%s> does not look like JSON' % linein)
                        cmd.struct = {}
                except Exception:
                        log.exception('parsing json for <%s>' % linein)
                        cmd.struct = {}
                return cmd
        
        def reset(self, mode, conn = None):
                """
                Tells a CTI client that the server will shortly be down.
                """
                if conn is None:
                        self.__fill_ctilog__('daemon stop', mode)
                else:
                        tosend = { 'class' : 'serverdown',
                                   'mode' : mode }
                        conn.sendall(self.__cjson_encode__(tosend) + '\n')
                return
        
        def transfer_addbuf(self, req, buf):
                self.transfers_buf[req].append(buf)
                return
        
        def transfer_addref(self, connid, commandstruct):
                requester = '%s:%d' % connid.getpeername()
                fileid = commandstruct.get('fileid')
                tdir = commandstruct.get('tdirection')
                
                if connid in self.timerthreads_login_timeout:
                        self.timerthreads_login_timeout[connid].cancel()
                        del self.timerthreads_login_timeout[connid]
                if tdir == 'upload':
                        self.transfers_ref[requester] = fileid
                        self.transfers_buf[requester] = []
                        tosend = { 'class' : 'fileref',
                                   'fileid' : fileid }
                        connid.sendall(self.__cjson_encode__(tosend) + '\n')
                else:
                        if fileid in self.filestodownload:
                                fname = self.filestodownload[fileid]
                                data = urllib.urlopen('file:%s' % fname).read()
                                tosend = { 'class' : 'fileref',
                                           'filename' : fname,
                                           'payload' : base64.b64encode(data) }
                                connid.sendall(self.__cjson_encode__(tosend) + '\n')
                                del self.filestodownload[fileid]
                        else:
                                log.warning('fileid %s does not exist' % fileid)
                return
        
        def transfer_endbuf(self, req):
                log.info('full buffer received for %s : len=%d %s'
                          % (req, len(''.join(self.transfers_buf[req])), self.transfers_ref))
                if req in self.transfers_ref:
                        ref = self.transfers_ref[req]
                        if ref in self.faxes:
                                uinfo = self.faxes[ref].uinfo
                                astid = uinfo.get('astid')
                                reply = self.faxes[ref].sendfax(''.join(self.transfers_buf[req]),
                                                                self.configs[astid].faxcallerid,
                                                                self.amilist.ami[astid])
                                tosend = { 'class' : 'faxprogress',
                                           'status' : reply.split(';')[0],
                                           'reason' : reply.split(';')[1] }
                                self.__send_msg_to_cti_client__(uinfo, self.__cjson_encode__(tosend))
                                del self.transfers_ref[req]
                                del self.transfers_buf[req]
                return
        
        def get_login_params(self, command, astid, connid):
                return command.struct
        
        def manage_login(self, loginparams, phase, uinfo):
                if phase == xivo_commandsets.CMD_LOGIN_ID:
                        missings = []
                        # warns that the former session did not exit correctly (on a given computer)
                        if 'lastlogout-stopper' in loginparams and 'lastlogout-datetime' in loginparams:
                                if not loginparams['lastlogout-stopper'] or not loginparams['lastlogout-datetime']:
                                        log.warning('lastlogout stopper=%s datetime=%s'
                                                    % (loginparams['lastlogout-stopper'], loginparams['lastlogout-datetime']))
                        for argum in ['company', 'userid', 'ident', 'xivoversion', 'version']:
                                if argum not in loginparams:
                                        missings.append(argum)
                        if len(missings) > 0:
                                log.warning('missing args in loginparams : %s' % ','.join(missings))
                                return 'missing:%s' % ','.join(missings)
                        
                        # trivial checks (version, client kind) dealing with the software used
                        xivoversion = loginparams.get('xivoversion')
                        if xivoversion != XIVOVERSION_NUM:
                                return 'xivoversion_client:%s;%s' % (xivoversion, XIVOVERSION_NUM)
                        svnversion = loginparams.get('version')
                        ident = loginparams.get('ident')
                        if len(ident.split('@')) == 2:
                                [whoami, whatsmyos] = ident.split('@')
                                # return 'wrong_client_identifier:%s' % whoami
                                if whatsmyos[:3] not in ['X11', 'WIN', 'MAC']:
                                        return 'wrong_os_identifier:%s' % whatsmyos
                        else:
                                return 'wrong_client_os_identifier:%s' % ident
                        if (not svnversion.isdigit()) or int(svnversion) < self.required_client_version:
                                return 'version_client:%s;%d' % (svnversion, self.required_client_version)
                        
                        # user match
                        userinfo = self.ulist_ng.finduser(loginparams.get('userid'), loginparams.get('company'))
                        if userinfo == None:
                                return 'user_not_found'
                        userinfo['prelogin'] = {'cticlientos' : whatsmyos,
                                                'version' : svnversion,
                                                'sessionid' : ''.join(random.sample(__alphanums__, 10))}
                        
                elif phase == xivo_commandsets.CMD_LOGIN_PASS:
                        # user authentication
                        missings = []
                        for argum in ['hashedpassword']:
                                if argum not in loginparams:
                                        missings.append(argum)
                        if len(missings) > 0:
                                log.warning('missing args in loginparams : %s' % ','.join(missings))
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
                        
                elif phase == xivo_commandsets.CMD_LOGIN_CAPAS:
                        missings = []
                        for argum in ['state', 'capaid', 'lastconnwins', 'loginkind']:
                                if argum not in loginparams:
                                        missings.append(argum)
                        if len(missings) > 0:
                                log.warning('missing args in loginparams : %s' % ','.join(missings))
                                return 'missing:%s' % ','.join(missings)
                        
                        if uinfo is not None:
                                userinfo = uinfo
                        # settings (in agent mode for instance)
                        # userinfo['agent']['phonenum'] = phonenum
                        
                        state = loginparams.get('state')
                        capaid = loginparams.get('capaid')
                        subscribe = loginparams.get('subscribe')
                        lastconnwins = loginparams.get('lastconnwins')
                        
                        iserr = self.__check_capa_connection__(userinfo, capaid)
                        if iserr is not None:
                                return iserr
                        
                        self.__connect_user__(userinfo, state, capaid, lastconnwins)
                        
                        loginkind = loginparams.get('loginkind')
                        if loginkind == 'agent':
                                userinfo['agentphonenum'] = loginparams.get('phonenumber')
                                # if loginparams.get('agentlogin'):
                                # self.__login_agent__(userinfo)
                        if subscribe is not None:
                                userinfo['subscribe'] = 0
                else:
                        userinfo = None
                return userinfo
        
        def manage_logout(self, userinfo, when):
                log.info('logout (%s) user:%s/%s'
                         % (when, userinfo.get('astid'), userinfo.get('xivo_userid')))
                userinfo['last-logouttimestamp'] = time.time()
                self.__logout_agent__(userinfo)
                self.__disconnect_user__(userinfo)
                self.__fill_user_ctilog__(userinfo, 'cti_logout')
                return
        
        def __check_user_connection__(self, userinfo):
                if userinfo.has_key('login') and userinfo['login'].has_key('sessiontimestamp'):
                        dt = time.time() - userinfo['login']['sessiontimestamp']
                        log.warning('user %s already connected from %s (%.1f s)'
                                    % (userinfo['user'], userinfo['login']['connection'].getpeername(), dt))
                        # TODO : make it configurable.
                        lastconnectedwins = True
                        #lastconnectedwins = False
                        if lastconnectedwins:
                            self.manage_logout(userinfo, 'newconnection')
                        else:
                            return 'already_connected:%s:%d' % userinfo['login']['connection'].getpeername()
                return None
        
        def __check_capa_connection__(self, userinfo, capaid):
                if capaid in self.capas and capaid in userinfo.get('capaids'):
                        if self.capas[capaid].toomuchusers():
                                return 'toomuchusers:%s' % self.capas[capaid].getmaxgui()
                else:
                        return 'capaid_undefined:%s' % capaid
                return None
        
        def __connect_user__(self, userinfo, state, capaid, lastconnwins):
                try:
                        userinfo['capaid'] = capaid
                        userinfo['login'] = {}
                        userinfo['login']['sessiontimestamp'] = userinfo['login']['logintimestamp'] = time.time()
                        for v, vv in userinfo['prelogin'].iteritems():
                                userinfo['login'][v] = vv
                        del userinfo['prelogin']
                        # lastconnwins was introduced in the aim of forcing a new connection to take on for
                        # a given user, however it might breed problems if the previously logged-in process
                        # tries to reconnect ... unless we send something asking to Kill the process
                        userinfo['lastconnwins'] = lastconnwins
                        
                        # we first check if 'state' has already been set for this customer, in which case
                        # the CTI clients will be sent back this previous state
                        presenceid = self.capas[capaid].presenceid
                        if 'state' in userinfo:
                                futurestate = userinfo.get('state')
                                # only if it was a "defined" state anyway
                                if presenceid in self.presence_sections and futurestate in self.presence_sections[presenceid].getstates():
                                        state = futurestate
                                        
                        if presenceid in self.presence_sections:
                                if state in self.presence_sections[presenceid].getstates() and state not in ['onlineoutgoing', 'onlineincoming']:
                                        userinfo['state'] = state
                                else:
                                        log.warning('(user %s) : state <%s> is not an allowed one => <%s>'
                                                    % (userinfo.get('user'), state, self.presence_sections[presenceid].getdefaultstate()))
                                        userinfo['state'] = self.presence_sections[presenceid].getdefaultstate()
                        else:
                                userinfo['state'] = 'xivo_unknown'
                        
                        self.__presence_action__(userinfo['astid'], self.__agentnum__(userinfo), userinfo)
                        
                        self.capas[capaid].conn_inc()
                except Exception:
                        log.exception('connect_user %s' % userinfo)
                return
        
        def __disconnect_user__(self, userinfo, type='force'):
                try:
                        # state is unchanged
                        if 'login' in userinfo:
                                capaid = userinfo.get('capaid')
                                self.capas[capaid].conn_dec()
                                userinfo['last-version'] = userinfo['login']['version']
                                # ask the client to disconnect
                                if not userinfo['login']['connection'].isClosed:
                                    userinfo['login']['connection'].send(self.__cjson_encode__({'class' : 'disconnect', 'type' : type}) + '\n')
                                    # make sure everything is sent before closing the connection
                                    userinfo['login']['connection'].toClose = True
                                del userinfo['login']
                                userinfo['state'] = 'xivo_unknown'
                                self.__update_availstate__(userinfo, userinfo.get('state'))
                                # do not remove 'capaid' in order to keep track of it
                                # del userinfo['capaid'] # after __update_availstate__
                        else:
                                log.warning('userinfo does not contain login field : %s' % userinfo)
                except Exception:
                        log.exception('disconnect_user %s' % userinfo)
                return
        
        def loginko(self, loginparams, errorstring, connid):
                log.warning('user can not connect (%s) : sending %s' % (loginparams, errorstring))
                if connid in self.timerthreads_login_timeout:
                        self.timerthreads_login_timeout[connid].cancel()
                        del self.timerthreads_login_timeout[connid]
                tosend = { 'class' : 'loginko',
                           'errorstring' : errorstring }
                connid.sendall('%s\n' % self.__cjson_encode__(tosend))
                return
        
        def telldisconn(self, connid):
                tosend = { 'class' : 'disconn' }
                connid.sendall('%s\n' % self.__cjson_encode__(tosend))
                return
        
        def loginok(self, loginparams, userinfo, connid, phase):
                if phase == xivo_commandsets.CMD_LOGIN_ID:
                        tosend = { 'class' : 'login_id_ok',
                                   'xivoversion' : XIVOVERSION_NUM,
                                   'version' : __revision__,
                                   'sessionid' : userinfo['prelogin']['sessionid'] }
                        repstr = self.__cjson_encode__(tosend)
                elif phase == xivo_commandsets.CMD_LOGIN_PASS:
                        tosend = { 'class' : 'login_pass_ok',
                                   'capalist' : userinfo.get('capaids') }
                        repstr = self.__cjson_encode__(tosend)
                elif phase == xivo_commandsets.CMD_LOGIN_CAPAS:
                        astid = userinfo.get('astid')
                        context = userinfo.get('context')
                        capaid = userinfo.get('capaid')
                        
                        presenceid = self.capas[capaid].presenceid
                        if presenceid in self.presence_sections:
                                allowed = self.presence_sections[presenceid].allowed(userinfo.get('state'))
                                details = self.presence_sections[presenceid].getdisplaydetails()
                                statedetails = self.presence_sections[presenceid].displaydetails[userinfo.get('state')]
                        else:
                                allowed = {}
                                details = {}
                                statedetails = {}
                        wpid = self.capas[capaid].watchedpresenceid
                        if wpid in self.presence_sections:
                                cstatus = self.presence_sections[wpid].countstatus(self.__counts__(astid, context, wpid))
                        else:
                                cstatus = {}
                        
                        tosend = { 'class' : 'login_capas_ok',
                                   'astid' : astid,
                                   'xivo_userid' : userinfo.get('xivo_userid'),
                                   'capafuncs' : self.capas[capaid].tostringlist(self.capas[capaid].all()),
                                   'capaxlets' : self.capas[capaid].capadisps,
                                   'capaservices' : self.capas[capaid].capaservices,
                                   'appliname' : self.capas[capaid].appliname,
                                   'guisettings' : self.capas[capaid].getguisettings(),
                                   'capapresence' : { 'names'   : details,
                                                      'state'   : statedetails,
                                                      'allowed' : allowed },
                                   'presencecounter' : cstatus
                                   }
                        repstr = self.__cjson_encode__(tosend)
                        # if 'features' in capa_user:
                        # repstr += ';capas_features:%s' %(','.join(configs[astid].capafeatures))
                        if connid in self.timerthreads_login_timeout:
                                self.timerthreads_login_timeout[connid].cancel()
                                del self.timerthreads_login_timeout[connid]
                        self.__fill_user_ctilog__(userinfo, 'cti_login',
                                                  'ip=%s,port=%d,version=%s,os=%s,capaid=%s'
                                                  % (connid.getpeername()[0],
                                                     connid.getpeername()[1],
                                                     userinfo.get('login').get('version'),
                                                     userinfo.get('login').get('cticlientos'),
                                                     userinfo.get('capaid')
                                                     ))
                connid.sendall(repstr + '\n')
                
                if phase == xivo_commandsets.CMD_LOGIN_CAPAS:
                        self.__update_availstate__(userinfo, userinfo.get('state'))
                
                return
        
        @staticmethod
        def __cjson_encode__(object):
                if 'class' in object:
                        object['direction'] = 'client'
                        object['timenow'] = time.time()
                oencoded = cjson.encode(object)
                # if 'class' in object:
                # log.info('__cjson_encode__ : %s %d' % (object['class'], len(oencoded)))
                return oencoded
        
        def set_logintimeout(self, logintimeout):
                self.logintimeout = logintimeout
                return
        
        def set_partings(self, parting_astid_context):
                if parting_astid_context:
                        partings = parting_astid_context.split(',')
                        self.parting_astid = ('astid' in partings)
                        self.parting_context = ('context' in partings)
                return
        
        def set_cticonfig(self, lconf):
                self.lconf = lconf
                
                self.sheet_actions = {}
                for where, sheetaction in lconf.read_section('sheet_events', 'sheet_events').iteritems():
                        if where in self.sheet_allowed_events and len(sheetaction) > 0:
                                self.sheet_actions[where] = lconf.read_section('sheet_action', sheetaction)
                return
        
        def set_ctilog(self, ctilog):
                self.ctilog_conn = None
                self.ctilog_cursor = None
                if ctilog is not None:
                        try:
                                self.ctilog_conn = anysql.connect_by_uri(str(ctilog))
                                self.ctilog_cursor = self.ctilog_conn.cursor()
                                self.__fill_ctilog__('daemon start', __revision__)
                        except Exception:
                                log.exception('(set_ctilog)')
                return
        
        def __fill_ctilog__(self, what, options = ''):
                if self.ctilog_conn is not None and self.ctilog_cursor is not None:
                        try:
                                datetime = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
                                columns = ('eventdate', 'loginclient', 'company', 'status', 'action', 'arguments', 'callduration')
                                self.ctilog_cursor.query("INSERT INTO ctilog (${columns}) "
                                                         "VALUES (%s, NULL, NULL, NULL, %s, %s, NULL)",
                                                         columns,
                                                         (datetime, what, options))
                        except Exception:
                                log.exception('(__fill_ctilog__)')
                        self.ctilog_conn.commit()
                return
        
        def __fill_user_ctilog__(self, uinfo, what, options = '', callduration = None):
                if self.ctilog_conn is not None and self.ctilog_cursor is not None:
                        try:
                                datetime = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
                                columns = ('eventdate', 'loginclient', 'company', 'status', 'action', 'arguments', 'callduration')
                                self.ctilog_cursor.query("INSERT INTO ctilog (${columns}) "
                                                         "VALUES (%s, %s, %s, %s, %s, %s, %s)",
                                                         columns,
                                                         (datetime, uinfo.get('user'), uinfo.get('company'), uinfo.get('state'),
                                                          what, options, callduration))
                        except Exception:
                                log.exception('(__fill_user_ctilog__)')
                        self.ctilog_conn.commit()
                return
        
        def set_options(self, xivoconf, allconf):
                self.xivoconf = xivoconf
                for var, val in self.xivoconf.iteritems():
                        if var.find('-') > 0 and val:
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
                                elif prop == 'guisettings':
                                        self.capas[name].setguisettings(val)
                                elif prop == 'services':
                                        self.capas[name].setservices(val.split(','))
                                elif prop == 'presence':
                                        self.capas[name].setpresenceid(val)
                                        if val not in self.presence_sections:
                                                self.presence_sections[val] = cti_presence.Presence(allconf.read_section('presence', val))
                                elif prop == 'watchedpresence':
                                        self.capas[name].setwatchedpresenceid(val)
                                        if val not in self.presence_sections:
                                                self.presence_sections[val] = cti_presence.Presence(allconf.read_section('presence', val))
                for v, vv in allconf.read_section('phonehints', 'phonehints').iteritems():
                        vvv = vv.split(',')
                        if len(vvv) > 1:
                                self.display_hints[v] = { 'longname' : vvv[0],
                                                          'color' : vvv[1] }
                if 'required_client_version' in self.xivoconf:
                        self.required_client_version = int(self.xivoconf['required_client_version'])
                return
        
        def set_configs(self, configs):
                self.configs = configs
                return
        
        def set_urllist(self, astid, listname, urllist):
                if listname == 'phones':
                        # XXX
                        for astid_hash in [self.uniqueids, self.channels,
                                           self.queues_channels_list, self.attended_targetchannels,
                                           self.ignore_dtmf, self.parkedcalls, self.origapplication,
                                           self.amirequests, self.getvar_requests,
                                           self.events_link, self.localchans]:
                                if astid not in astid_hash:
                                        astid_hash[astid] = {}
                if listname in self.weblistprops:
                        cf = self.weblistprops[listname]['classfile']
                        cn = self.weblistprops[listname]['classname']
                        self.weblist[listname][astid] = getattr(cf, cn)(urllist)
                        self.weblist[listname][astid].setcommandclass(self)
                if listname == 'phones':
                        # XXX
                        self.weblist['phones'][astid].setdisplayhints(self.display_hints)
                return
        
        def set_contextlist(self, ctxlist):
                self.ctxlist = ctxlist
                return
        
        def getprofilelist(self):
                ret = {}
                for capaid in self.capas.keys():
                        ret[capaid] = self.capas[capaid].appliname
                return cjson.encode(ret)
        
        def getmem(self, where = None):
                try:
                        pid = os.getpid()
                        t = open('/proc/%s/stat' % pid, 'r')
                        u = t.read().strip().split()
                        t.close()
                        # utime = int(u[13])
                        # ktime = int(u[14])
                        td = int(u[22])
                        if where:
                                log.info('(%s) memory %d bytes' % (where, td))
                except:
                        log.exception('getmem')
                        td = 0
                return td
        
        def updates(self):
                self.ulist_ng.update()
                for astid, plist in self.weblist['phones'].iteritems():
                        for itemname in self.weblist.keys():
                                try:
                                        updatestatus = self.weblist[itemname][astid].update()
                                        for function in ['del', 'add']:
                                                if updatestatus[function]:
                                                        log.info('%s %s %s : %s' % (astid, itemname, function, updatestatus[function]))
                                                        # TODO: do one deltalist per context and send them only to that context ?
                                                        tosend = { 'class' : itemname,
                                                                   'function' : function,
                                                                   'astid' : astid,
                                                                   'deltalist' : updatestatus[function] }
                                                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
                                        if itemname in ['queues', 'groups']:
                                                for qname, qq in self.weblist[itemname][astid].keeplist.iteritems():
                                                        qq['queuestats']['Xivo-Join'] = 0
                                                        qq['queuestats']['Xivo-Link'] = 0
                                                        qq['queuestats']['Xivo-Lost'] = 0
                                                        qq['queuestats']['Xivo-Rate'] = -1
                                                        qq['queuestats']['Xivo-Chat'] = 0
                                                        qq['queuestats']['Xivo-Wait'] = 0
                                                        self.__update_queue_stats__(astid, qname, itemname)
                                except Exception:
                                        log.exception('(updates : %s)' % itemname)
                        self.askstatus(astid, self.weblist['phones'][astid].keeplist)
                        
                m2 = self.getmem()
                if m2 != self.save_memused:
                    log.info('memory = %10d ; delta = %10d' % (m2, m2 - self.save_memused))
                    self.save_memused = m2
                # check : agentnumber should be unique
                return
        
        def set_userlist_urls(self, urls):
                self.ulist_ng.setandupdate(urls)
                return
        
        def getincomingcalllist(self, iclist):
                liclist = {}
                for icitem in iclist:
                        try:
                                if not icitem.get('commented'):
                                        icid = icitem.get('id')
                                        liclist[icid] = { 'exten' : icitem.get('exten'),
                                                          'context' : icitem.get('context'),
                                                          'destidentity' : icitem.get('destidentity'),
                                                          'action' : icitem.get('action'),
                                                          'usercontext' : icitem.get('usercontext'),
                                                          'groupcontext' : icitem.get('groupcontext'),
                                                          'queuecontext' : icitem.get('queuecontext'),
                                                          'meetmecontext' : icitem.get('meetmecontext'),
                                                          'voicemenucontext' : icitem.get('voicemenucontext'),
                                                          'voicemailcontext' : icitem.get('voicemailcontext')
                                                          }
                        except Exception:
                                log.exception('(getincomingcalllist) : %s' % icitem)
                return liclist
        
        def getagentslist(self, alist):
                lalist = {}
                for aitem in alist:
                        try:
                                if not aitem.get('commented'):
                                        aid = aitem.get('id')
                                        lalist[aid] = { 'firstname' :  aitem.get('firstname'),
                                                        'lastname' :   aitem.get('lastname'),
                                                        'number' :     aitem.get('number'),
                                                        'password' :   aitem.get('passwd'),
                                                        'context' :    aitem.get('context'),
                                                        'ackcall' :    aitem.get('ackcall'),
                                                        'wrapuptime' : aitem.get('wrapuptime'),
                                                        
                                                        'groups_by_agent' : {},
                                                        'queues_by_agent' : {},
                                                        'agentstats' : {}
                                                        }
                        except Exception:
                                log.exception('(getagentslist) : %s' % aitem)
                return lalist
        
        def getphoneslist(self, plist):
                lplist = {}
                for pitem in plist:
                        try:
                                idx = '.'.join([pitem.get('protocol'), pitem.get('context'),
                                                pitem.get('name'), pitem.get('number')])
                                lplist[idx] = { 'number' : pitem.get('number'),
                                                'tech' : pitem.get('protocol'),
                                                'context': pitem.get('context'),
                                                'enable_hint': pitem.get('enablehint'),
                                                'initialized': pitem.get('initialized'),
                                                'phoneid': pitem.get('name'),
                                                
                                                'hintstatus' : 'none',
                                                'comms' : {}
                                                }
                        except Exception:
                                log.exception('(getphoneslist : %s)' % pitem)
                return lplist
        
        def gettrunklist(self, tlist):
                ttlist = {}
                for titem in tlist:
                        try:
                                if not titem.get('commented'):
                                        tid = titem.get('id')
                                        ttlist[tid] = { 'name' :   titem.get('name'),
                                                        'tech' :   titem.get('protocol'),
                                                        'host' :   titem.get('host'),
                                                        'type' :   titem.get('type'),
                                                        'context': titem.get('context'),
                                                        
                                                        'comms' : {}
                                                        }
                        except Exception:
                                log.exception('(gettrunklist) : %s' % titem)
                return ttlist
        
        def getmeetmelist(self, mlist):
                lmlist = {}
                for mitem in mlist:
                        try:
                                if not mitem.get('commented'):
                                        lmlist[mitem.get('id')] = { 'number' :    mitem.get('number'),
                                                                    'name' :      mitem.get('name'),
                                                                    'context' :   mitem.get('context'),
                                                                    'pin' :       mitem.get('pin'),
                                                                    'admin-pin' : mitem.get('admin-pin'),

                                                                    'adminid' : None,
                                                                    'uniqueids' : {}
                                                                    }
                        except Exception:
                                log.exception('(getmeetmelist : %s)' % mitem)
                return lmlist
        
        def getphonebook(self, pbook):
            pblist = {}
            for pitem in pbook:
                pbitem = {}
                for i1, v1 in pitem.iteritems():
                    if isinstance(v1, dict):
                        for i2, v2 in v1.iteritems():
                            if isinstance(v2, dict):
                                for i3, v3 in v2.iteritems():
                                    idx = '.'.join([i1, i2, i3])
                                    pbitem[idx] = v3
                            else:
                                idx = '.'.join([i1, i2])
                                pbitem[idx] = v2
                    else:
                        pbitem[i1] = v1
                myid = pbitem.get('phonebook.id')
                pblist[myid] = pbitem
            return pblist
        
        def getvoicemaillist(self, vlist):
                lvlist = {}
                for vitem in vlist:
                        try:
                                if not vitem.get('commented'):
                                        lvlist[vitem.get('id')] = { 'mailbox' :  vitem.get('mailbox'),
                                                                    'context' :  vitem.get('context'),
                                                                    'fullname' : vitem.get('fullname'),
                                                                    'password' : vitem.get('password'),
                                                                    'email' :    vitem.get('email')
                                                                    }
                        except Exception:
                                log.exception('(getvoicemaillist : %s)' % vitem)
                return lvlist
        
        def getgroupslist(self, dlist):
                lglist = {}
                for gitem in dlist:
                        try:
                                if not gitem.get('commented'):
                                        groupname = gitem.get('name')
                                        lglist[groupname] = { 'groupname' : groupname,
                                                              'number' : gitem.get('number'),
                                                              'context' : gitem.get('context'),
                                                              'id' : gitem.get('id'),
                                                              
                                                              'agents_in_queue' : {},
                                                              'channels' : {},
                                                              'queuestats' : {}
                                                              }
                        except Exception:
                                log.exception('(getgroupslist : %s)' % gitem)
                return lglist
        
        def getqueueslist(self, dlist):
                lqlist = {}
                for qitem in dlist:
                        try:
                                if not qitem.get('commented'):
                                        queuename = qitem.get('name')
                                        lqlist[queuename] = { 'queuename' : queuename,
                                                              'number' : qitem.get('number'),
                                                              'context' : qitem.get('context'),
                                                              'id' : qitem.get('id'),
                                                              
                                                              'agents_in_queue' : {},
                                                              'channels' : {},
                                                              'queuestats' : {}
                                                              }
                        except Exception:
                                log.exception('(getqueueslist : %s)' % qitem)
                return lqlist
        
        # fields set at startup by reading informations
        userfields = ['user', 'company', 'astid', 'password', 'fullname',
                      'capaids', 'context', 'phonenum', 'techlist', 'agentid', 'xivo_userid']
        def getuserslist(self, dlist):
                lulist = {}
                for uitem in dlist:
                        try:
                                # if uitem.get('enableclient') and uitem.get('loginclient'):
                                if True:
                                        astid = uitem.get('astid', 'xivo')
                                        uid = astid + '/' + uitem.get('id')
                                        log.debug('getuserslist %s : %s' % (uid, uitem))
                                        lulist[uid] = {'user' : uitem.get('loginclient'),
                                                       'company' : uitem.get('context'),
                                                       'password' : uitem.get('passwdclient'),
                                                       'capaids' : uitem.get('profileclient').split(','),
                                                       'fullname' : uitem.get('fullname'),
                                                       'astid' : astid,
                                                       'context' : uitem.get('context'),
                                                       'techlist' : [],
                                                       'mobilenum' : uitem.get('mobilephonenumber'),
                                                       'xivo_userid' : uitem.get('id'),
                                                       
                                                       'state'    : 'xivo_unknown'
                                                       }
                                        if uitem.get('simultcalls'):
                                            lulist[uid]['simultcalls'] = int(uitem.get('simultcalls'))
                                        if uitem.get('protocol') and uitem.get('context') and uitem.get('name') and uitem.get('number'):
                                            lulist[uid]['phoneid'] = uitem.get('name')
                                            lulist[uid]['phonenum'] = uitem.get('number')
                                            lulist[uid]['techlist'] = ['.'.join([uitem.get('protocol'), uitem.get('context'),
                                                                                 uitem.get('name'), uitem.get('number')])]
                                        # set a default capaid value for users with only one capaid (most common case)
                                        if len(lulist[uid]['capaids']) == 1:
                                                lulist[uid]['capaid'] = lulist[uid]['capaids'][0]
                                        if uitem.get('enablevoicemail') and uitem.get('voicemailid'):
                                                lulist[uid]['voicemailid'] = uitem.get('voicemailid')
                                                if 'mwi' not in lulist[uid] or len(lulist[uid]['mwi']) < 3:
                                                    lulist[uid]['mwi'] = ['0', '0', '0']
                                        else:
                                                lulist[uid]['mwi'] = []
                                        if uitem.get('agentid') is not None:
                                                lulist[uid]['agentid'] = uitem.get('agentid')
                                        else:
                                                lulist[uid]['agentid'] = ''
                        except Exception:
                                log.exception('(getuserslist : %s)' % uitem)
                return lulist
        
        @staticmethod
        def version():
                return __revision__
        
        def getdetails(self, weblistitem):
                retv = {}
                if weblistitem in self.weblist:
                        retv = self.weblist[weblistitem]
                return retv
        
        def uniqueids(self):
                return self.uniqueids
        
        def users(self):
                return self.ulist_ng.users()
        
        def connected_users(self):
                return self.ulist_ng.connected_users()
        
        def askstatus(self, astid, npl):
                for a, b in npl.iteritems():
                        #log.info('askstatus %s %s' %(a, b))
                        if b['tech'] != 'custom':
                                if b['number']:
                                        self.__ami_execute__(astid, 'sendextensionstate', b['number'], b['context'])
                                        self.__ami_execute__(astid, 'mailbox', b['number'], b['context'])
                                else:
                                        log.warning('%s askstatus : not enough data for %s' % (astid, b))
                        else:
                                log.warning('%s askstatus : custom tech not managed %s' % (astid, b))
                return
        
        def __callback_timer__(self, what):
                try:
                        thisthread = threading.currentThread()
                        tname = thisthread.getName()
                        log.info('__callback_timer__ (timer finished at %s) %s %s' % (time.asctime(), tname, what))
                        thisthread.setName('%s-%s' % (what, tname))
                        self.tqueue.put(thisthread)
                        os.write(self.queued_threads_pipe[1], what)
                except Exception:
                        log.exception('__callback_timer__ %s' % what)
                return
        
        def connected(self, connid):
                """
                Send a banner at login time
                """
                requester = '%s:%d' % connid.getpeername()
                connid.sendall('XiVO CTI Server Version %s(%s) svn:%s (on %s)\n'
                               % (XIVOVERSION_NUM, XIVOVERSION_NAME,
                                  __revision__, ' '.join(os.uname()[:3])))
                self.timerthreads_login_timeout[connid] = threading.Timer(self.logintimeout,
                                                                          self.__callback_timer__,
                                                                          ('login(%s)' % requester,))
                self.timerthreads_login_timeout[connid].start()
                return
        
        def disconnected(self, connid):
                if connid in self.timerthreads_login_timeout:
                        self.timerthreads_login_timeout[connid].cancel()
                        del self.timerthreads_login_timeout[connid]
                return
        
        def checkqueue(self):
                buf = os.read(self.queued_threads_pipe[0], 1024)
                log.info('checkqueue : read buf = %s, tqueue size = %d' % (buf, self.tqueue.qsize()))
                todisconn = []
                while self.tqueue.qsize() > 0:
                        thisthread = self.tqueue.get()
                        tname = thisthread.getName()
                        if tname.startswith('login'):
                                for connid, conntimerthread in self.timerthreads_login_timeout.iteritems():
                                        if conntimerthread == thisthread:
                                                todisconn.append(connid)
                        elif tname.startswith('agentlogoff'):
                                if thisthread in self.timerthreads_agentlogoff_retry:
                                        properties = self.timerthreads_agentlogoff_retry[thisthread]
                                        astid = properties.get('astid')
                                        agentnum = properties.get('agentnumber')
                                        agentid = self.weblist['agents'][astid].reverse_index.get(agentnum)
                                        status = self.weblist['agents'][astid].keeplist[agentid]['agentstats']['status']
                                        if status != 'AGENT_LOGGEDOFF':
                                                log.warning('checkqueue : agent %s on %s was not properly logged off (%s %s)'
                                                            % (agentnum, astid, tname, status))
                                                self.__ami_execute__(astid, 'agentlogoff', agentnum)
                                        del self.timerthreads_agentlogoff_retry[thisthread]
                                        del thisthread
                        else:
                                log.warning('checkqueue : unknown action for %s' % tname)
                for connid in todisconn:
                        del self.timerthreads_login_timeout[connid]
                return { 'disconnlist-tcp'  : todisconn }
        
        def __send_msg_to_cti_client__(self, userinfo, strupdate):
                """send a string (terminate with a newline character) to a user"""
                wassent = False
                try:
                    t0 = time.time()
                    if userinfo and 'login' in userinfo and 'connection' in userinfo['login']:
                        if not userinfo['login']['connection'].toClose:
                            mysock = userinfo['login']['connection']
                            mysock.sendall(strupdate + '\n')
                            wassent = True
                except ClientConnection.CloseException, cexc:
                    # an error happened on the socket so it should be closed
                    t1 = time.time()
                    dispuserid = '%s/%s' % (userinfo.get('astid'), userinfo.get('xivo_userid'))
                    log.exception('(__send_msg_to_cti_client__) userinfo(astid/xivo_userid)=%s len=%d timespent=%f cexc=%s'
                                  % (dispuserid, len(strupdate), (t1 - t0), cexc))
                    if userinfo and 'login' in userinfo and 'connection' in userinfo['login']:
                        userinfo['login']['connection'].toClose = True
                return wassent
        
        def __check_astid_context__(self, userinfo, astid, context = None):
                mycond_astid = True
                mycond_context = True
                if self.parting_astid:
                        mycond_astid = (astid == userinfo['astid'])
                if self.parting_context and context is not None:
                        mycond_context = (context == userinfo['context'])
                return (mycond_astid and mycond_context)
        
        def __check_context__(self, context1, context2):
                mycond_context = True
                if self.parting_context:
                        mycond_context = (context1 == context2)
                return mycond_context
        
        def __send_msg_to_cti_clients__(self, strupdate, astid, context = None):
                log.debug('__send_msg_to_cti_clients__ astid=%s context=%s %s' % (astid, context, strupdate))
                try:
                        if strupdate is not None:
                                for userinfo in self.ulist_ng.keeplist.itervalues():
                                        # check if the user should receive this message depending on astid and context
                                        if self.__check_astid_context__(userinfo, astid, context):
                                                self.__send_msg_to_cti_client__(userinfo, strupdate)
                except Exception:
                        log.exception('(__send_msg_to_cti_clients__)')
                return
        
        def __send_msg_to_cti_clients_except__(self, uinfos, strupdate, astid, context = None):
                log.debug('__send_msg_to_cti_clients_except__ astid=%s context=%s %s' % (astid, context, strupdate))
                try:
                        if strupdate is not None:
                                for userinfo in self.ulist_ng.keeplist.itervalues():
                                        if userinfo not in uinfos:
                                                # check if the user should receive this message depending on astid and context
                                                if self.__check_astid_context__(userinfo, astid, context):
                                                        self.__send_msg_to_cti_client__(userinfo, strupdate)
                except Exception:
                        log.exception('(__send_msg_to_cti_clients_except__) userinfo(astid/xivo_userid) = %s/%s'
                                      % (userinfo.get('astid'), userinfo.get('xivo_userid')))
                return
        
        
        sheet_allowed_events = ['incomingqueue', 'incominggroup', 'incomingdid',
                                'agentcalled', 'agentlinked', 'agentunlinked',
                                'dial', 'link', 'unlink', 'hangup',
                                'faxreceived',
                                'callmissed', # see GG (in order to tell a user that he missed a call)
                                'localphonecalled', 'outgoing']
        
        def __build_xmlqtui__(self, sheetkind, actionopt, inputvars):
                linestosend = []
                whichitem = actionopt.get(sheetkind)
                if whichitem is not None and len(whichitem) > 0:
                        for k, v in self.lconf.read_section('sheet_qtui', whichitem).iteritems():
                                try:
                                        r = urllib.urlopen(v)
                                        t = r.read()
                                        r.close()
                                except Exception, exc: # conscious limited exception output ("No such file or directory")
                                        log.error('__build_xmlqtui__ %s %s : %s' % (sheetkind, whichitem, exc))
                                        t = None
                                if t is not None:
                                        linestosend.append('<%s name="%s"><![CDATA[%s]]></%s>' % (sheetkind, k, t, sheetkind))
                return linestosend
        
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
                                if vv is None:
                                    log.warning('__build_xmlsheet__ %s %s : for the key %s, the value is None'
                                                % (sheetkind, whichitem, kk))
                                else:
                                    try:
                                        basestr = basestr.replace('{%s}' % kk, vv.decode('utf8'))
                                    except UnicodeEncodeError:
                                        basestr = basestr.replace('{%s}' % kk, vv)
                            basestr = re.sub('{[a-z\-]*}', defaultval, basestr)
                            linestosend.append('<%s order="%s" name="%s" type="%s"><![CDATA[%s]]></%s>'
                                               % (sheetkind, k, title, type, basestr, sheetkind))
                        else:
                            log.warning('__build_xmlsheet__ wrong number of fields in definition for %s %s %s'
                                        % (sheetkind, whichitem, k))
                    except Exception:
                        log.exception('(__build_xmlsheet__) %s %s' % (sheetkind, whichitem))
            return linestosend
        
        def findreverse(self, dirlist, number):
            log.info('findreverse %s %s' % (dirlist, number))
            itemdir = {}
            if dirlist:
                dirs_to_lookup = []
                for dirname in dirlist.split(','):
                    ddef = self.lconf.read_section('directory', dirname)
                    if ddef:
                        dirs_to_lookup.append(dirname)
                        self.ctxlist.updatedir(dirname, ddef)
                for dirname in dirs_to_lookup:
                    dirdef = self.ctxlist.dirlist[dirname]
                    try:
                        y = self.__build_customers_bydirdef__(dirname, number, dirdef, True)
                    except Exception:
                        log.exception('(findreverse) %s %s %s' % (dirlist, dirname, number))
                        y = []
                    if y:
                        itemdir['xivo-reverse-nresults'] = str(len(y))
                        for g, gg in y[0].iteritems():
                            itemdir[g] = gg
            return itemdir
        
        def findcountry(self, numtolookup):
            """
            Looks up the country code and name according to the prefix database.
            """
            nchars = 4
            notfound = True
            notfinished = True
            pf = '(unknown)'
            country = '(unknown)'
            while notfound and notfinished:
                pf = numtolookup[:nchars]
                if pf in self.interprefix:
                    notfound = False
                    country = self.interprefix[pf]
                    break
                nchars = nchars - 1
                if nchars == 0:
                    notfinished = False
                    pf = '(unknown)'
                    break
            return [pf, country]
        
        def __sheet_construct__(self, where, astid, context, event, extraevent):
            actionopt = self.sheet_actions.get(where)
            linestosend = ['<?xml version="1.0" encoding="utf-8"?>',
                           '<profile>',
                           '<user>']
            itemdir = {'xivo-where' : where,
                       'xivo-astid' : astid,
                       'xivo-context' : context,
                       'xivo-time' : time.strftime('%H:%M:%S', time.localtime()),
                       'xivo-date' : time.strftime('%Y-%m-%d', time.localtime())}
            if actionopt.get('focus') == 'no':
                linestosend.append('<internal name="nofocus"></internal>')
            linestosend.append('<internal name="astid"><![CDATA[%s]]></internal>' % astid)
            linestosend.append('<internal name="context"><![CDATA[%s]]></internal>' % context)
            linestosend.append('<internal name="kind"><![CDATA[%s]]></internal>' % where)
                        
            # fill a dict with the appropriate values + set the concerned users' list
            if where == 'outgoing':
                exten = event.get('Extension')
                application = event.get('Application')
                if application == 'Dial' and exten.isdigit():
                    pass
                            
            elif where == 'faxreceived':
                itemdir['xivo-calleridnum'] = event.get('CallerID')
                itemdir['xivo-faxpages'] = event.get('PagesTransferred')
                itemdir['xivo-faxfile'] = event.get('FileName')
                itemdir['xivo-faxstatus'] = event.get('PhaseEString')
                
            elif where in ['agentlinked', 'agentunlinked']:
                dstnum = event.get('Channel2')[6:]
                chan = event.get('Channel1')
                queuename = extraevent.get('xivo_queuename')
                itemdir['xivo-channel'] = chan
                itemdir['xivo-queuename'] = queuename
                itemdir['xivo-calleridnum'] = event.get('CallerID1')
                itemdir['xivo-calleridname'] = event.get('CallerIDName1', 'NOXIVO-CID1')
                itemdir['xivo-calledidnum'] = event.get('CallerID2')
                itemdir['xivo-calledidname'] = event.get('CallerIDName2', 'NOXIVO-CID2')
                itemdir['xivo-agentnumber'] = dstnum
                itemdir['xivo-uniqueid'] = event.get('Uniqueid1')
                            
            elif where == 'dial':
                for fieldname in extraevent.keys():
                    if not itemdir.has_key(fieldname):
                        itemdir[fieldname] = extraevent[fieldname]
                                        
            elif where in ['link', 'unlink']:
                itemdir['xivo-channel'] = event.get('Channel1')
                itemdir['xivo-channelpeer'] = event.get('Channel2')
                itemdir['xivo-uniqueid'] = event.get('Uniqueid1')
                itemdir['xivo-calleridnum'] = event.get('CallerID1')
                itemdir['xivo-calleridname'] = event.get('CallerIDName1', 'NOXIVO-CID1')
                itemdir['xivo-calledidnum'] = event.get('CallerID2')
                itemdir['xivo-calledidname'] = event.get('CallerIDName2', 'NOXIVO-CID2')
                                    
            elif where == 'hangup':
                # print where, event
                itemdir['xivo-channel'] = event.get('Channel')
                itemdir['xivo-uniqueid'] = event.get('Uniqueid')
                # find which userinfo's to contact ...
                            
            elif where == 'incomingdid':
                for fieldname in extraevent.keys():
                    if not itemdir.has_key(fieldname):
                        itemdir[fieldname] = extraevent[fieldname]
                    # userinfos.append() : users matching the SDA ?
                                
            elif where in ['incomingqueue', 'incominggroup']:
                clid = event.get('CallerID')
                chan = event.get('Channel')
                uid = event.get('Uniqueid')
                queuename = event.get('Queue')
#                log.info('ALERT %s %s (%s) uid=%s %s %s queue=(%s %s %s) ndest = %d' % (astid, where, time.asctime(),
#                                                                                        uid, clid, chan,
#                                                                                        queue, event.get('Position'),
#                                                                                        event.get('Count'),
#                                                                                        len(userinfos)))
                for fieldname in extraevent.keys():
                    if not itemdir.has_key(fieldname):
                        itemdir[fieldname] = extraevent[fieldname]
                itemdir['xivo-queuename'] = queuename
                itemdir['xivo-queueorgroup'] = where[8:] + 's'
                
            if 'xivo-channel' in itemdir:
                linestosend.append('<internal name="channel"><![CDATA[%s]]></internal>'
                                   % itemdir['xivo-channel'])
            if 'xivo-uniqueid' in itemdir:
                linestosend.append('<internal name="uniqueid"><![CDATA[%s]]></internal>'
                                   % itemdir['xivo-uniqueid'])
            linestosend.extend(self.__build_xmlqtui__('sheet_qtui', actionopt, itemdir))
            linestosend.extend(self.__build_xmlsheet__('action_info', actionopt, itemdir))
            linestosend.extend(self.__build_xmlsheet__('sheet_info', actionopt, itemdir))
            linestosend.extend(self.__build_xmlsheet__('systray_info', actionopt, itemdir))
            linestosend.append('</user></profile>')
            return linestosend
        
        def __sheet_alert__(self, where, astid, context, event, extraevent = {}, channel = None):
                # fields to display :
                # - internal asterisk/xivo : caller, callee, queue name, sda
                # - custom ext database
                # - custom (F)AGI
                # options :
                # - urlauto, urlx
                # - popup or not
                # - actions ?
                # - dispatch to one given person / all / subscribed ones
                log.debug('%s __sheet_alert__ where=%s channel=%s' % (astid, where, channel))
                if where in self.sheet_actions:
                        userinfos = []
                        actionopt = self.sheet_actions.get(where)
                        whoms = actionopt.get('whom')
                        capaids = None
                        if 'capaids' in actionopt:
                            if actionopt['capaids']:
                                capaids = actionopt['capaids'].split(',')
                            else:
                                capaids = []
                        if whoms is None or whoms == '':
                            log.warning('%s __sheet_alert__ : whom field for %s action has not been defined'
                                        % (astid, where))
                            return
                        log.info('%s __sheet_alert__ %s %s %s %s' % (astid, where, whoms, time.asctime(), channel))
                        
                        # 1) build sheet
                        if channel is not None:
                            self.__create_new_sheet__(astid, channel)
                        linestosend = self.__sheet_construct__(where, astid, context, event, extraevent)
                        
                        # 2) build recipient list
                        if where in ['agentlinked', 'agentunlinked']:
                            userinfos.extend(self.__find_userinfos_by_agentnum__(astid, dstnum))
                        elif where == 'dial':
                            # define the receiver from the xivo-dstid data
                            if 'xivo-dstid' in extraevent:
                                xuserid = '%s/%s' % (astid, extraevent['xivo-dstid'])
                                if xuserid in self.ulist_ng.keeplist:
                                    userinfos.append(self.ulist_ng.keeplist[xuserid])
                            # define the receiver from the dialed number (dial application, that may follow an other first one)
                            if 'xivo-calledidnum' not in extraevent and 'xivo-dialednum' in extraevent:
                                for uinfo in self.ulist_ng.keeplist.itervalues():
                                    if uinfo.get('astid') == astid and uinfo.has_key('phonenum') and uinfo.get('phonenum') == extraevent['xivo-dialednum']:
                                        userinfos.append(uinfo)
                        elif where in ['link', 'unlink']:
                            for uinfo in self.ulist_ng.keeplist.itervalues():
                                if uinfo.get('astid') == astid and uinfo.has_key('phonenum') and uinfo.get('phonenum') == event.get('CallerID2'):
                                    userinfos.append(uinfo)
                        elif where in ['incomingqueue', 'incominggroup']:
                            queuename = event.get('Queue')
                            queueorgroup = where[8:] + 's'
                            # find who are the queue members
                            for agent_channel, status in self.weblist[queueorgroup][astid].keeplist[queuename]['agents_in_queue'].iteritems():
                                # XXX sip@queue
                                if status.get('Paused') == '0':
                                    userinfos.extend(self.__find_userinfos_by_agentnum__(astid, agent_channel[6:]))
                                    
                        # 3) format message and send
                        dozip = True
                        xmlustring = ''.join(linestosend)
                        xmlstring = xmlustring.encode('utf8')
                        #�build the json message
                        tosend = { 'class' : 'sheet',
                                   'whom' : whoms,
                                   'function' : 'displaysheet',
                                   'channel' : channel,
                                   'compressed' : dozip
                                   }
                        if dozip:
                            ulen = len(xmlstring)
                            # prepend the uncompressed length in big endian
                            # to the zlib compressed string to meet Qt qUncompress() function expectations
                            toencode = struct.pack(">I", ulen) + zlib.compress(xmlstring)
                            tosend['payload'] = base64.b64encode(toencode)
                        else:
                            tosend['payload'] = base64.b64encode(xmlstring)
                        jsonmsg = self.__cjson_encode__(tosend)
                        
                        # print '---------', where, whoms, fulllines
                        
                        # 4) update sheet manager
                        if astid in self.sheetmanager and self.sheetmanager[astid].has_sheet(channel):
                            self.sheetmanager[astid].setcustomersheet(channel, xmlustring)
                        
                        # 5) send the payload to the appropriate people
                        for whom in whoms.split(','):
                            nsent = 0
                            uinfostosend = []
                            if whom == 'dest':
                                for uinfo in userinfos:
                                    if capaids is None or uinfo.get('capaid') in capaids:
                                        uinfostosend.append(uinfo)
                                            
                            elif whom == 'subscribe':
                                for uinfo in self.ulist_ng.keeplist.itervalues():
                                    if capaids is None or uinfo.get('capaid') in capaids:
                                        if 'subscribe' in uinfo:
                                            uinfostosend.append(uinfo)
                                                
                            elif whom == 'all-astid-context': # match asterisks and contexts
                                for uinfo in self.ulist_ng.keeplist.itervalues():
                                    if astid == uinfo.get('astid') and context == uinfo.get('context'):
                                        if capaids is None or uinfo.get('capaid') in capaids:
                                            uinfostosend.append(uinfo)
                                                
                            elif whom == 'all-context': # accross asterisks + match contexts
                                for uinfo in self.ulist_ng.keeplist.itervalues():
                                    if context == uinfo.get('context'):
                                        if capaids is None or uinfo.get('capaid') in capaids:
                                            uinfostosend.append(uinfo)
                                                
                            elif whom == 'all-astid': # accross contexts + match asterisks
                                for uinfo in self.ulist_ng.keeplist.itervalues():
                                    if astid == uinfo.get('astid'):
                                        if capaids is None or uinfo.get('capaid') in capaids:
                                            uinfostosend.append(uinfo)
                                                
                            elif whom == 'all': # accross asterisks and contexts
                                for uinfo in self.ulist_ng.keeplist.itervalues():
                                    if capaids is None or uinfo.get('capaid') in capaids:
                                        uinfostosend.append(uinfo)
                                            
                            else:
                                log.warning('__sheet_alert__ (%s) : unknown destination <%s> in <%s>'
                                            % (astid, whom, where))
                            # send to clients 
                            lstsent = []
                            for uinfo in uinfostosend:
                                if self.__send_msg_to_cti_client__(uinfo, jsonmsg):
                                    nsent += 1
                                    userid = '%s/%s' % (uinfo.get('astid'), uinfo.get('xivo_userid'))
                                    lstsent.append(userid)
                            # mark clients as "viewing users"
                            if astid in self.sheetmanager and self.sheetmanager[astid].has_sheet(channel):
                                self.sheetmanager[astid].addviewingusers(channel, lstsent)
                            if lstsent:
                                log.info('%s (whom=%s, where=%s) %d sheet(s) sent to %s'
                                         % (astid, whom, where, nsent, lstsent))
                                    
                return

        def __dialplan_fill_src__(self, dialplan_data):
            origin = dialplan_data.get('xivo-origin')
            if origin == 'did':
                self.__did__(dialplan_data)
            elif origin == 'internal':
                self.__internal__(dialplan_data)
            else:
                pass
            log.info('__dialplan_fill_src__ : %s' % dialplan_data)
            return
        
        def __dialplan_fill_dst__(self, dialplan_data):
            eventname = dialplan_data.get('xivo-userevent')
            if eventname == 'MacroUser':
                astid = dialplan_data.get('xivo-astid')
                userid = dialplan_data.get('xivo-dstid')
                xuserid = '%s/%s' % (astid, userid)
                if xuserid in self.ulist_ng.keeplist:
                    dialplan_data['xivo-calledidnum'] = self.ulist_ng.keeplist[xuserid].get('phonenum')
                    dialplan_data['xivo-calledidname'] = self.ulist_ng.keeplist[xuserid].get('fullname')
            else:
                pass
            log.info('__dialplan_fill_dst__ : %s' % dialplan_data)
            return
        
        def __internal__(self, dialplan_data):
            log.info('__internal__ %s' % dialplan_data)
            astid = dialplan_data.get('xivo-astid')
            userid = dialplan_data.get('xivo-userid')
            xuserid = '%s/%s' % (astid, userid)
            if xuserid in self.ulist_ng.keeplist:
                dialplan_data['dbr-display'] = self.ulist_ng.keeplist[xuserid].get('fullname')
                dialplan_data['xivo-calleridnum'] = self.ulist_ng.keeplist[xuserid].get('phonenum')
                dialplan_data['xivo-calleridname'] = self.ulist_ng.keeplist[xuserid].get('fullname')
                try:
                    e = self.lconf.read_section('directory_internal', 'directory_internal')
                    for k, v in e.iteritems():
                        if k.startswith('field_'):
                            nk = 'db-%s' % k[6:]
                            v2 = v
                            for kk, vv in self.ulist_ng.keeplist[xuserid].iteritems():
                                pattern = '{internal-%s}' % kk
                                if v2.find(pattern) >= 0:
                                    v2 = v2.replace('{internal-%s}' % kk, vv)
                            dialplan_data[nk] = v2
                    if 'name' in e:
                        dialplan_data['xivo-dir'] = e.get('name')
                except Exception:
                    log.exception('__internal__ %s' % xuserid)
            return
        
        def __did__(self, dialplan_data):
            log.info('__did__ %s' % dialplan_data)
            calleridnum = dialplan_data.get('xivo-calleridnum')
            calleridton = dialplan_data.get('xivo-calleridton')
            context = dialplan_data.get('xivo-context')
            
            if calleridnum.isdigit(): # reverse only if digits
                ctx2dirs = self.lconf.read_section('reversedid', 'reversedid')
                dirlist = ctx2dirs.get(context)
                if dirlist:
                    reversedata = self.findreverse(dirlist, calleridnum)
                    dialplan_data.update(reversedata)
                    # dialplan_data['xivo-extra'] = {'XIVO_CTI_VIP' : '4', 'XIVO_CTI_SPEC' : 'hello'}
                    
            # agi_callington : 0x10, 0x11 : 16 (internat ?), 17 (00 + internat ?)
            #                  0x20, 0x21 : 32 (06, 09), 33 (02, 04)
            #                  0x41       : 65 (3 chiffres)
            #                  0xff       : 255 (unknown @ IAX2 ?), -1 (unknown @ Zap ?)
            #                  0 ?
            # TON:
            #  Unknown Number Type (0)
            #  International Number (1)
            #  National Number (2)
            #  Network Specific Number (3)
            #  Subscriber Number (4)
            # NPI:
            #  Unknown Number Plan (0)
            #  ISDN/Telephony Numbering Plan (E.164/E.163) (1)
            # agi_callingpres : 0, 1, 3, 35, 67, -1?
            # agi_rdnis : when transferred from another initial destination ?
            
            if calleridton:
                numtolookup = None
                if calleridton == '16':
                    numtolookup = calleridnum
                elif calleridton == '17':
                    numtolookup = calleridnum[2:]
                if numtolookup:
                    [countrycode, countrytext] = self.findcountry(numtolookup)
                    dialplan_data['db-countrycode'] = countrycode
                    dialplan_data['db-countrytext'] = countrytext
            return
        
        def __phoneid_from_channel__(self, astid, channel):
                ret = None
                tech = None
                phoneid = None
                if channel not in self.channels[astid]:
                        log.warning('%s __phoneid_from_channel__ : channel %s not found' % (astid, channel))
                # special cases : AsyncGoto/IAX2/asteriskisdn-13622<ZOMBIE>
                if channel.startswith('SIP/'):
                        tech = 'sip'
                        phoneid = channel[4:].split('-')[0]
##                elif channel.startswith('AsyncGoto/SIP/'):
##                        tech = 'sip'
##                        phoneid = channel[14:].split('-')[0]
                elif channel.startswith('Parked/SIP/'):
                        tech = 'sip'
                        phoneid = channel[11:].split('-')[0]
                elif channel.startswith('IAX2/'):
                        tech = 'iax2'
                        phoneid = channel[5:].split('-')[0]
                if tech is not None and phoneid is not None:
                        for phoneref, b in self.weblist['phones'][astid].keeplist.iteritems():
                                if b['tech'] == tech and b['phoneid'] == phoneid:
                                        ret = phoneref
                # give also : userid ...
                return ret
        
        def __trunkid_from_channel__(self, astid, channel):
            ret = None
            tech = None
            trunkid = None
            if channel not in self.channels[astid]:
                log.warning('%s __trunkid_from_channel__ : channel %s not found' % (astid, channel))
            if channel.startswith('SIP/'):
                tech = 'sip'
                trunkid = channel[4:].split('-')[0]
            elif channel.startswith('IAX2/'):
                tech = 'iax'
                trunkid = channel[5:].split('-')[0]
            if tech is not None and trunkid is not None:
                for tref, tv in self.weblist['trunks'][astid].keeplist.iteritems():
                    if tv['tech'] == tech and tv['name'] == trunkid:
                        # log.info('%s trunk : %s => %s %s => %s %s' % (astid, channel, tech, trunkid, tref, tv))
                        ret = tref
            return ret
        
        def __userinfo_from_phoneid__(self, astid, phoneid):
            uinfo = None
            for vv in self.ulist_ng.keeplist.itervalues():
                if phoneid in vv['techlist'] and astid == vv['astid']:
                    uinfo = vv
                    break
            return uinfo
        
        def __userinfo_from_agentphonenum__(self, astid, phoneid):
            uinfo = None
            if phoneid is not None:
                phonenum = phoneid.split('.')[3]
                for vv in self.ulist_ng.keeplist.itervalues():
                    if astid == vv['astid'] and 'agentphonenum' in vv and phonenum == vv['agentphonenum']:
                        uinfo = vv
                        break
            return uinfo
        
        def __fill_uniqueids__(self, astid, uid1, uid2, chan1, chan2, where):
                if uid1 in self.uniqueids[astid]:
                    (tmpchan, tmpuid, _clid, _clidn) = self.__translate_local_channel_uid__(astid, self.uniqueids[astid][uid1]['channel'], uid1)
                    if chan1 == tmpchan:
                        self.uniqueids[astid][uid1].update({where : chan2,
                                                            'channel' : chan1,
                                                            'time-%s' % where : time.time()})
                if uid2 in self.uniqueids[astid]:
                    (tmpchan, tmpuidn, _clid, _clidn) = self.__translate_local_channel_uid__(astid, self.uniqueids[astid][uid2]['channel'], uid2)
                    if chan2 == tmpchan:
                        self.uniqueids[astid][uid2].update({where : chan1,
                                                            'channel' : chan2,
                                                            'time-%s' % where : time.time()})
                return
        
        # Methods to handle Asterisk AMI events
        def ami_dial(self, astid, event):
                log.info('%s ami_dial : %s' % (astid, event))
                src     = event.get('Source')
                dst     = event.get('Destination')
                calleridnum = event.get('CallerID')
                calleridname = event.get('CallerIDName')
                uidsrc  = event.get('SrcUniqueID')
                uiddst  = event.get('DestUniqueID')
                data    = event.get('Data', 'NOXIVO-DATA').split('|')
                # actionid = self.amilist.execute(astid, 'getvar', src, 'XIVO_DSTNUM')
                # self.getvar_requests[astid][actionid] = {'channel' : src, 'variable' : 'XIVO_DSTNUM'}
                self.__link_local_channels__(astid, src, uidsrc, dst, uiddst, calleridnum, calleridname, None, None)
                
                if data:
                    if uidsrc in self.uniqueids[astid]:
                        if 'dialplan_data' in self.uniqueids[astid][uidsrc]:
                            self.uniqueids[astid][uidsrc]['dialplan_data']['xivo-dialednum'] = data[0].split('/')[1]
                            if 'context' in self.uniqueids[astid][uidsrc]:
                                context = self.uniqueids[astid][uidsrc]['context']
                                self.__sheet_alert__('dial', astid, context, {}, self.uniqueids[astid][uidsrc].get('dialplan_data'), src)
                        else:
                            log.warning('%s ami_dial : no dialplan_data defined for %s' % (astid, uidsrc))
                            
                phoneidsrc = self.__phoneid_from_channel__(astid, src)
                phoneiddst = self.__phoneid_from_channel__(astid, dst)
                trunkidsrc = self.__trunkid_from_channel__(astid, src)
                trunkiddst = self.__trunkid_from_channel__(astid, dst)
                # uinfosrc = self.__userinfo_from_phoneid__(astid, phoneidsrc)
                # uinfodst = self.__userinfo_from_phoneid__(astid, phoneiddst)
                self.__fill_uniqueids__(astid, uidsrc, uiddst, src, dst, 'dial')
                self.uniqueids[astid][uidsrc]['calleridnum'] = calleridnum
                self.uniqueids[astid][uidsrc]['calleridname'] = calleridname
                #log.debug('%s ami_dial src=%s event=%s' % (astid, self.uniqueids[astid][uidsrc], event))
                
                # update the phones statuses
                self.weblist['phones'][astid].ami_dial(phoneidsrc, phoneiddst,
                                                       uidsrc, uiddst,
                                                       self.uniqueids[astid][uidsrc],
                                                       self.uniqueids[astid][uiddst])
                self.weblist['trunks'][astid].ami_dial(trunkidsrc, trunkiddst,
                                                       uidsrc, uiddst,
                                                       self.uniqueids[astid][uidsrc],
                                                       self.uniqueids[astid][uiddst])
                
                self.__update_phones_trunks__(astid, phoneidsrc, phoneiddst, trunkidsrc, trunkiddst, 'ami_dial')
                return
        
        
        def __update_phones_trunks__(self, astid, ph1, ph2, tr1, tr2, where):
                phoneids = []
                if ph1:
                        phoneids.append(ph1)
                if ph2 and ph2 != ph1:
                        phoneids.append(ph2)
                trunkids = []
                if tr1:
                        trunkids.append(tr1)
                if tr2 and tr2 != tr1:
                        trunkids.append(tr2)
                        
                log.info('%s __update_phones_trunks__ %s %s (%s)' % (astid, phoneids, trunkids, where))
                for phid in phoneids:
                        tosend = self.weblist['phones'][astid].status(phid)
                        tosend['astid'] = astid
                        context = self.weblist['phones'][astid].keeplist[phid]['context']
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context)
                for trid in trunkids:
                        tosend = self.weblist['trunks'][astid].status(trid)
                        tosend['astid'] = astid
                        context = self.weblist['trunks'][astid].keeplist[trid]['context']
                        # XXX Beware, the trunks' contexts are usually different (from-extern vs. default)
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context)
                return
        
        def read_internatprefixes(self, internatprefixfile):
                try:
                        pref = urllib.urlopen(internatprefixfile)
                        csvreader = csv.reader(pref, delimiter = ';')
                        self.interprefix = {}
                        for line in csvreader:
                                if len(line) > 1:
                                        self.interprefix[line[0]] = line[1].decode('utf8')
                except:
                        log.exception('problem when reading from %s' % internatprefixfile)
                return
        
        def read_queuelog(self, astid, url_queuelog):
                if astid not in self.stats_queues:
                        self.stats_queues[astid] = {}
                if astid not in self.last_agents:
                        self.last_agents[astid] = {}
                if astid not in self.last_queues:
                        self.last_queues[astid] = {}
                if url_queuelog is None:
                        return
                try:
                    qlog = urllib.urlopen(url_queuelog)
                    csvreader = csv.reader(qlog, delimiter = '|')
                except Exception:
                    log.exception('read_queuelog %s' % url_queuelog)
                    return
                
                time_now = int(time.time())
                time_1ha = time_now - 3600
                
                last_start = 0
                
                qolddate = 0
                linenum = 0
                while True:
                        try:
                                linenum += 1
                                line = csvreader.next()
                        except StopIteration:
                                break
                        except Exception:
                                log.exception('%s read_queuelog (line %d) %s' % (astid, linenum, url_queuelog))
                                line = []
                        if len(line) > 3:
                                if qolddate == int(line[0]):
                                        ic += 1
                                else:
                                        ic = 1
                                qdate = float('%s.%06d' % (line[0], ic))
                                qolddate = int(line[0])
                                
                                qaction = line[4]
                                qname = line[2]
                                if qaction in ['ENTERQUEUE', 'CONNECT', 'ABANDON', 'EXITEMPTY']:
                                        if qdate > time_1ha:
                                                if qname not in self.stats_queues[astid]:
                                                        self.stats_queues[astid][qname] = { 'ENTERQUEUE' : [],
                                                                                            'CONNECT' : [],
                                                                                            'ABANDON' : [] }
                                                if qaction == 'EXITEMPTY':
                                                        qaction = 'ABANDON'
                                                self.stats_queues[astid][qname][qaction].append(int(qdate))
                                if qaction == 'QUEUESTART':
                                        last_start = qdate
                                if qaction in ['AGENTLOGIN', 'AGENTCALLBACKLOGIN', 'AGENTLOGOFF', 'AGENTCALLBACKLOGOFF']:
                                        # on asterisk 1.2, there is no Agent/ before AGENTCALLBACKLOGIN and AGENTCALLBACKLOGOFF
                                        # on asterisk 1.4, there is no Agent/ before AGENTCALLBACKLOGIN
                                        if qaction == 'AGENTCALLBACKLOGIN':
                                                agent_channel = 'Agent/' + line[3]
                                        else:
                                                agent_channel = line[3]
                                        if agent_channel.startswith('Agent/') and len(agent_channel) > 6:
                                                if agent_channel not in self.last_agents[astid]:
                                                        self.last_agents[astid][agent_channel] = {}
                                                self.last_agents[astid][agent_channel][qaction] = qdate
                                        else:
                                                # log.warning('%s sip@queue (qlogA) %s %s' % (astid, qaction, agent_channel))
                                                pass
                                if qaction in ['UNPAUSE', 'PAUSE', 'ADDMEMBER', 'REMOVEMEMBER', 'AGENTCALLED', 'CONNECT']:
                                        agent_channel = line[3]
                                        if agent_channel.startswith('Agent/') and len(agent_channel) > 6:
                                                if qname not in self.last_queues[astid]:
                                                        self.last_queues[astid][qname] = {}
                                                if agent_channel not in self.last_queues[astid][qname]:
                                                        self.last_queues[astid][qname][agent_channel] = {}
                                                self.last_queues[astid][qname][agent_channel][qaction] = qdate
                                        else:
                                                # log.warning('%s sip@queue (qlogB) %s %s' % (astid, qaction, agent_channel))
                                                pass
                qlog.close()
                for aid, v in self.last_agents[astid].iteritems():
                        toremove = []
                        for act, adate in v.iteritems():
                                if adate < last_start:
                                        toremove.append(act)
                        for tr in toremove:
                                del v[tr]
                        if v:
                                if 'AGENTCALLBACKLOGOFF' in v and 'AGENTCALLBACKLOGIN' in v:
                                        if v['AGENTCALLBACKLOGOFF'] < v['AGENTCALLBACKLOGIN']:
                                                del v['AGENTCALLBACKLOGOFF']
                                        else:
                                                del v['AGENTCALLBACKLOGIN']
                for qid, vv in self.last_queues[astid].iteritems():
                        for aid, v in vv.iteritems():
                                toremove = []
                                for act, adate in v.iteritems():
                                        if adate < last_start:
                                                toremove.append(act)
                                for tr in toremove:
                                        del v[tr]
                                if v:
                                        if 'PAUSE' in v and 'UNPAUSE' in v:
                                                if v['PAUSE'] < v['UNPAUSE']:
                                                        del v['PAUSE']
                                                else:
                                                        del v['UNPAUSE']
                                        if 'ADDMEMBER' in v and 'REMOVEMEMBER' in v:
                                                if v['ADDMEMBER'] < v['REMOVEMEMBER']:
                                                        del v['ADDMEMBER']
                                                else:
                                                        del v['REMOVEMEMBER']
                                                        
                # COMPLETEAGENT : end of successful call
                # CONFIGRELOAD, QUEUESTART
                # RINGNOANSWER
                # COMPLETECALLER : ?
                return
        
        def __agentnum__(self, userinfo):
                astid = userinfo.get('astid')
                if 'agentid' in userinfo and userinfo['agentid']:
                        agent_id = userinfo['agentid']
                        if astid in self.weblist['agents'] and agent_id in self.weblist['agents'][astid].keeplist:
                                return self.weblist['agents'][astid].keeplist[agent_id].get('number')
                return ''
        
        def __voicemailnum__(self, userinfo):
                astid = userinfo.get('astid')
                if 'voicemailid' in userinfo and userinfo['voicemailid']:
                        voicemail_id = userinfo['voicemailid']
                        if astid in self.weblist['voicemail'] and voicemail_id in self.weblist['voicemail'][astid].keeplist:
                                return self.weblist['voicemail'][astid].keeplist[voicemail_id].get('mailbox')
                return ''
        
        def __update_queue_stats__(self, astid, queuename, queueorgroup, fname = None):
                if astid not in self.stats_queues:
                        return
                if queuename not in self.stats_queues[astid]:
                        self.stats_queues[astid][queuename] = { 'ENTERQUEUE' : [],
                                                                'CONNECT' : [],
                                                                'ABANDON' : [] }
                time_now = int(time.time())
                time_1ha = time_now - 3600
                for fieldname in ['ENTERQUEUE', 'CONNECT', 'ABANDON']:
                        toremove = []
                        for t in self.stats_queues[astid][queuename][fieldname]:
                                if t < time_1ha:
                                        toremove.append(t)
                        for t in toremove:
                                self.stats_queues[astid][queuename][fieldname].remove(t)
                if fname:
                        self.stats_queues[astid][queuename][fname].append(time_now)
                        log.info('%s __update_queue_stats__ for %s %s' % (astid, queuename, fname))
                        
                self.weblist[queueorgroup][astid].fillstats(queuename, self.stats_queues[astid][queuename])
                # log.info('__update_queue_stats__ %s %s : %s' % (astid, queuename,
                # self.weblist[queueorgroup][astid].keeplist[queuename]['queuestats']))
                return
        
        def __ignore_dtmf__(self, astid, uid, where):
            ret = False
            if uid in self.ignore_dtmf[astid]:
                tnow = time.time()
                t0 = self.ignore_dtmf[astid][uid]['xivo-timestamp']
                dt = tnow - t0
                # allow 1s between each unlink/link event
                if dt < 1:
                    # print 'ignore', where, dt
                    self.ignore_dtmf[astid][uid]['xivo-timestamp'] = tnow
                    ret = True
            return ret
        
        def ami_hold(self, astid, event):
            # log.info('%s ami_hold : %s' % (astid, event))
            uid = event.get('Uniqueid')
            channel = event.get('Channel')
            if uid in self.uniqueids[astid]:
                log.info('%s ami_hold : holder:%s %s holded:%s' % (astid, channel, uid, self.uniqueids[astid][uid].get('link')))
                self.uniqueids[astid][uid]['time-hold'] = time.time()
                
                phoneid = self.__phoneid_from_channel__(astid, channel)
                if phoneid:
                    self.weblist['phones'][astid].ami_hold(phoneid, uid)
                    tosend = self.weblist['phones'][astid].status(phoneid)
                    tosend['astid'] = astid
                    context = self.weblist['phones'][astid].keeplist[phoneid]['context']
                    self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context)
            return
        
        def ami_unhold(self, astid, event):
            # log.info('%s ami_unhold : %s' % (astid, event))
            uid = event.get('Uniqueid')
            channel = event.get('Channel')
            if uid in self.uniqueids[astid]:
                log.info('%s ami_unhold : unholder:%s %s unholded:%s' % (astid, channel, uid, self.uniqueids[astid][uid].get('link')))
                if 'time-hold' in self.uniqueids[astid][uid]:
                    del self.uniqueids[astid][uid]['time-hold']
                    
                phoneid = self.__phoneid_from_channel__(astid, channel)
                if phoneid:
                    self.weblist['phones'][astid].ami_unhold(phoneid, uid)
                    tosend = self.weblist['phones'][astid].status(phoneid)
                    tosend['astid'] = astid
                    context = self.weblist['phones'][astid].keeplist[phoneid]['context']
                    self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context)
            return
        
        def ami_bridge(self, astid, event):
            log.info('%s ami_bridge : %s' % (astid, event))
            return
        
        def ami_masquerade(self, astid, event):
                originalstate = event.get('OriginalState')
                clonestate = event.get('CloneState')
                originalchannel = event.get('Original')
                clonechannel = event.get('Clone')
                
                # always implies Rename's ?
                if originalstate == 'Ringing' and clonestate == 'Down':
                        # interception
                        log.info('%s ami_masquerade : probably %s has catched a call for %s'
                                 % (astid, clonechannel, originalchannel))
                        # elif originalstate == 'Ringing' and clonestate == 'Up':
                        # # probably interception also ... XXX
                        # pass
                elif originalstate == 'Up' and clonestate == 'Up':
                        if astid in self.attended_targetchannels and originalchannel in self.attended_targetchannels[astid]:
                                # transfer
                                log.info('%s ami_masquerade : probably the other channel of %s is being transferred to %s (%s)'
                                         % (astid, originalchannel, clonechannel, self.attended_targetchannels[astid][originalchannel]))
                                del self.attended_targetchannels[astid][originalchannel]
                        else:
                                if originalchannel.startswith('AsyncGoto/'):
                                        log.info('%s ami_masquerade : probably direct transfer to a meetme room %s %s (Up+Up but no transfer)'
                                                 % (astid, originalchannel, clonechannel))
                                        # 'wait' for an ami_transfer
                                else:
                                        log.info('%s ami_masquerade : probably agent relation %s %s (Up+Up but no transfer)'
                                                 % (astid, originalchannel, clonechannel))
                        # elif originalstate == 'Ring' and clonestate == 'Up':
                        # # probably transfer also ... XXX
                        # pass
                        # elif originalstate == 'Down' and clonestate == 'Up':
                        # # {'Original': 'Parked/SIP/104-0822f658', 'Clone': 'SIP/104-0822f658', 'OriginalState': 'Down', 'CloneState': 'Up' }
                        # pass
                        # elif originalstate == 'Ringing' and clonestate == 'Up':
                        # # {'Original': 'SIP/fpotiquet-08203d00', 'Clone': 'SIP/118-081daba8', 'OriginalState': 'Ringing', 'CloneState': 'Up' }
                        # pass
                else:
                        log.info('%s ami_masquerade : %s' % (astid, event))
                return
        
        def ami_transfer(self, astid, event):
                targetchannel = event.get('TargetChannel')
                uniqueid = event.get('Uniqueid')
                targetuniqueid = event.get('TargetUniqueid')
                if event.get('TransferType') == 'Blind':
                        tcontext = event.get('TransferContext')
                        texten = event.get('TransferExten') # transferred-to extension
                        log.info('%s ami_transfer : Blind    TargetChannel=%s Exten=%s Context=%s' % (astid, targetchannel, texten, tcontext))
                        # uniqueid > target uniqueid (actually, same timestamp and .idcall++)
                        # channel = intermediate's incoming
                        # target channel = caller
                elif event.get('TransferType') == 'Attended':
                        self.attended_targetchannels[astid][targetchannel] = event
                        log.info('%s ami_transfer : Attended /  TargetChannel = %s' % (astid, targetchannel))
                        if uniqueid in self.uniqueids[astid]:
                            log.info('%s ami_transfer : Attended /       Uniqueid : %s' % (astid, self.uniqueids[astid][uniqueid]))
                        if targetuniqueid in self.uniqueids[astid]:
                            log.info('%s ami_transfer : Attended / TargetUniqueid : %s' % (astid, self.uniqueids[astid][targetuniqueid]))
                        # target uniqueid > uniqueid
                        # channel = intermediate's incoming
                        # target channel = intermediate's outgoing
                else:
                        log.info('%s ami_transfer : %s' % (astid, event))
                # SIP-Callid, TransferMethod
                # - not there when direct transfer without answering
                return
        
        def ami_link(self, astid, event):
                chan1 = event.get('Channel1')
                chan2 = event.get('Channel2')
                clid1 = event.get('CallerID1')
                clid2 = event.get('CallerID2')
                clidname1 = event.get('CallerIDName1', 'NOXIVO-CID1')
                clidname2 = event.get('CallerIDName2', 'NOXIVO-CID2')
                uid1 = event.get('Uniqueid1')
                uid2 = event.get('Uniqueid2')
                
                log.info('%s AMI_LINK : %s' % (astid, event))
                if uid1 not in self.events_link[astid]:
                    self.events_link[astid][uid1] = time.time()
                else:
                    log.warning('%s ignoring a Link event that already occured : %s' % (astid, event))
                    log.debug('%s events_link : %s' % (astid, self.events_link[astid]))
                    return
                
                # translate Local/xxx,1 channels
                self.__link_local_channels__(astid, chan1, uid1, chan2, uid2, clid1, clidname1, clid2, clidname2)
                
                (chan1, _uid1, clid1, clidname1) = self.__translate_local_channel_uid__(astid, chan1, uid1, clid1, clidname1)
                (chan2, _uid2, clid2, clidname2) = self.__translate_local_channel_uid__(astid, chan2, uid2, clid2, clidname2)
                
                if self.__ignore_dtmf__(astid, uid1, 'link'):
                    return
                if self.__ignore_dtmf__(astid, uid2, 'link'):
                    return
                self.__fill_uniqueids__(astid, uid1, uid2, chan1, chan2, 'link')
                uid1info = self.uniqueids[astid][uid1]
                
                if 'time-chanspy' in uid1info:
                    return
                
                if uid1info['link'].startswith('Agent/') and 'join' in uid1info:
                    queuename = uid1info['join'].get('queuename')
                    queueorgroup = uid1info['join'].get('queueorgroup')
                    log.info('%s STAT LINK %s %s' % (astid, queuename, uid1info['link']))
                    self.__update_queue_stats__(astid, queuename, queueorgroup, 'CONNECT')
                    
                phoneid1 = self.__phoneid_from_channel__(astid, chan1)
                phoneid2 = self.__phoneid_from_channel__(astid, chan2)
                trunkid1 = self.__trunkid_from_channel__(astid, chan1)
                trunkid2 = self.__trunkid_from_channel__(astid, chan2)
                uinfo1 = self.__userinfo_from_phoneid__(astid, phoneid1)
                uinfo2 = self.__userinfo_from_phoneid__(astid, phoneid2)
                uinfo1_ag = self.__userinfo_from_agentphonenum__(astid, phoneid1)
                uinfo2_ag = self.__userinfo_from_agentphonenum__(astid, phoneid2)
                
                duinfo1 = None
                duinfo2 = None
                if uinfo1:
                    duinfo1 = '%s/%s' % (uinfo1.get('astid'), uinfo1.get('xivo_userid'))
                    self.__update_sheet_user__(astid, uinfo1, chan2)
                if uinfo2:
                    duinfo2 = '%s/%s' % (uinfo2.get('astid'), uinfo2.get('xivo_userid'))
                    self.__update_sheet_user__(astid, uinfo2, chan1)
                log.info('%s LINK1 %s %s callerid=%s (phone trunk)=(%s %s) user=%s'
                         % (astid, uid1, chan1, clid1, phoneid1, trunkid1, duinfo1))
                log.info('%s LINK2 %s %s callerid=%s (phone trunk)=(%s %s) user=%s'
                         % (astid, uid2, chan2, clid2, phoneid2, trunkid2, duinfo2))
                
                # update the phones statuses
                self.weblist['phones'][astid].ami_link(phoneid1, phoneid2,
                                                       uid1, uid2,
                                                       self.uniqueids[astid][uid1],
                                                       self.uniqueids[astid][uid2],
                                                       clid1, clid2,
                                                       clidname1, clidname2)
                self.weblist['trunks'][astid].ami_link(trunkid1, trunkid2,
                                                       uid1, uid2,
                                                       self.uniqueids[astid][uid1],
                                                       self.uniqueids[astid][uid2])
                
                self.__update_phones_trunks__(astid, phoneid1, phoneid2, trunkid1, trunkid2, 'ami_link')
                
                if 'context' in self.uniqueids[astid][uid1]:
                    self.__sheet_alert__('link', astid, self.uniqueids[astid][uid1]['context'], event, {}, chan2)
                        
                if chan2.startswith('Agent/'):
                        # 'onlineincoming' for the agent
                        agent_number = chan2[6:]
                        status = 'onlineincoming'
                        for uinfo in self.__find_userinfos_by_agentnum__(astid, agent_number):
                            self.__update_availstate__(uinfo, status)
                            self.__presence_action__(astid, agent_number, uinfo)
                            
                        # To identify which queue a call comes from, we match a previous AMI Leave event,
                        # that involved the same channel as the one catched here.
                        # Any less-tricky-method is welcome, though.
                        qname = None
                        if chan1 in self.queues_channels_list[astid]:
                            qname = self.queues_channels_list[astid][chan1]
                            ## del self.queues_channels_list[astid][chan1]
                            extraevent = {'xivo_queuename' : qname}
                            self.__sheet_alert__('agentlinked', astid, CONTEXT_UNKNOWN, event, extraevent, chan1)
                            
                        agent_id = uinfo.get('agentid')
                        thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                        thisagent['agentstats'].update({'Xivo-Agent-StateTime' : time.time()})
                        thisagent['agentstats'].update({'Xivo-Agent-Status-Link' : { 'linkmode' : 'agentlink',
                                                                                     'dir' : status,
                                                                                     'outcall' : uid1info.get('OUTCALL'),
                                                                                     'did' : uid1info.get('DID'),
                                                                                     'linkqueue' : qname } })
                        for qval in thisagent['queues_by_agent'].itervalues():
                            qval['Xivo-QueueMember-StateTime'] = time.time()
                        for qval in thisagent['groups_by_agent'].itervalues():
                            qval['Xivo-QueueMember-StateTime'] = time.time()
                        tosend = { 'class' : 'agents',
                                   'function' : 'sendlist',
                                   'payload' : { astid : { agent_id : thisagent } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                         astid, thisagent.get('context'))
                        
                # outgoing channel
                lstinfos1 = [uinfo1]
                if uinfo1_ag not in lstinfos1:
                    lstinfos1.append(uinfo1_ag) # agent related to the phonenumber
                for uinfo in lstinfos1:
                        if uinfo:
                                status = 'onlineoutgoing'
                                self.__update_availstate__(uinfo, status)
                                agent_id = uinfo.get('agentid')
                                ag = self.__agentnum__(uinfo)
                                if ag:
                                        self.__presence_action__(astid, ag, uinfo)
                                        thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                                        thisagent['agentstats'].update({'Xivo-Agent-StateTime' : time.time()})
                                        thisagent['agentstats'].update({'Xivo-Agent-Status-Link' : { 'linkmode' : 'phonelink',
                                                                                                     'dir' : status,
                                                                                                     'outcall' : uid1info.get('OUTCALL'),
                                                                                                     'did' : uid1info.get('DID') } })
                                        for qval in thisagent['queues_by_agent'].itervalues():
                                            qval['Xivo-QueueMember-StateTime'] = time.time()
                                        for qval in thisagent['groups_by_agent'].itervalues():
                                            qval['Xivo-QueueMember-StateTime'] = time.time()
                                        tosend = { 'class' : 'agents',
                                                   'function' : 'sendlist',
                                                   'payload' : { astid : { agent_id : thisagent } }
                                                   }
                                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                                         astid, thisagent.get('context'))
                                        
                # incoming channel
                lstinfos2 = [uinfo2]
                if uinfo2_ag not in lstinfos2:
                    lstinfos2.append(uinfo2_ag) # agent related to the phonenumber
                for uinfo in lstinfos2:
                        if uinfo:
                                status = 'onlineincoming'
                                self.__update_availstate__(uinfo, status)
                                agent_id = uinfo.get('agentid')
                                ag = self.__agentnum__(uinfo)
                                if ag:
                                        self.__presence_action__(astid, ag, uinfo)
                                        thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                                        thisagent['agentstats'].update({'Xivo-Agent-StateTime' : time.time()})
                                        thisagent['agentstats'].update({'Xivo-Agent-Status-Link' : { 'linkmode' : 'phonelink',
                                                                                                     'dir' : status,
                                                                                                     'outcall' : uid1info.get('OUTCALL'),
                                                                                                     'did' : uid1info.get('DID') } })
                                        for qval in thisagent['queues_by_agent'].itervalues():
                                            qval['Xivo-QueueMember-StateTime'] = time.time()
                                        for qval in thisagent['groups_by_agent'].itervalues():
                                            qval['Xivo-QueueMember-StateTime'] = time.time()
                                        tosend = { 'class' : 'agents',
                                                   'function' : 'sendlist',
                                                   'payload' : { astid : { agent_id : thisagent } }
                                                   }
                                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                                         astid, thisagent.get('context'))
                return
        
        def ami_unlink(self, astid, event):
                chan1 = event.get('Channel1')
                chan2 = event.get('Channel2')
                clid1 = event.get('CallerID1')
                clid2 = event.get('CallerID2')
                uid1 = event.get('Uniqueid1')
                uid2 = event.get('Uniqueid2')
                where = event.get('Where')
                log.info('%s AMI_UNLINK : %s' % (astid, event))
                if uid1 not in self.events_link[astid]:
                    log.warning('%s ignoring an Unlink event (Link never happened or identifier already deleted) : %s' % (astid, event))
                    return
                else:
                    timenow = time.time()
                    deltat = timenow - self.events_link[astid][uid1]
                    log.warning('%s : deltat=%s event=%s' % (astid, deltat, event))
                    # we actually have many deltat's around 0.006, many around 0.04 and a few around 0.1
                    if deltat < 0.15:
                        return
                    else:
                        del self.events_link[astid][uid1]
                        
                (chan1, _uid1, _clid1, _clidn1) = self.__translate_local_channel_uid__(astid, chan1, uid1)
                (chan2, _uid2, _clid2, _clidn2) = self.__translate_local_channel_uid__(astid, chan2, uid2)
                
                if self.__ignore_dtmf__(astid, uid1, 'unlink'):
                    return
                if self.__ignore_dtmf__(astid, uid2, 'unlink'):
                    return
                self.__fill_uniqueids__(astid, uid1, uid2, chan1, chan2, 'unlink')
                uid1info = self.uniqueids[astid][uid1]
                phoneid1 = self.__phoneid_from_channel__(astid, chan1)
                phoneid2 = self.__phoneid_from_channel__(astid, chan2)
                trunkid1 = self.__trunkid_from_channel__(astid, chan1)
                trunkid2 = self.__trunkid_from_channel__(astid, chan2)
                uinfo1 = self.__userinfo_from_phoneid__(astid, phoneid1)
                uinfo2 = self.__userinfo_from_phoneid__(astid, phoneid2)
                uinfo1_ag = self.__userinfo_from_agentphonenum__(astid, phoneid1)
                uinfo2_ag = self.__userinfo_from_agentphonenum__(astid, phoneid2)
                
                duinfo1 = None
                duinfo2 = None
                if uinfo1:
                    duinfo1 = '%s/%s' % (uinfo1.get('astid'), uinfo1.get('xivo_userid'))
                if uinfo2:
                    duinfo2 = '%s/%s' % (uinfo2.get('astid'), uinfo2.get('xivo_userid'))
                log.info('%s UNLINK1 %s %s callerid=%s (phone trunk)=(%s %s) user=%s (%s)'
                         % (astid, uid1, chan1, clid1, phoneid1, trunkid1, duinfo1, where))
                log.info('%s UNLINK2 %s %s callerid=%s (phone trunk)=(%s %s) user=%s'
                         % (astid, uid2, chan2, clid2, phoneid2, trunkid2, duinfo2))
                
                # update the phones statuses
                self.weblist['phones'][astid].ami_unlink(phoneid1, phoneid2,
                                                         uid1, uid2,
                                                         self.uniqueids[astid][uid1],
                                                         self.uniqueids[astid][uid2])
                self.weblist['trunks'][astid].ami_unlink(trunkid1, trunkid2,
                                                         uid1, uid2,
                                                         self.uniqueids[astid][uid1],
                                                         self.uniqueids[astid][uid2])
                
                self.__update_phones_trunks__(astid, phoneid1, phoneid2, trunkid1, trunkid2, 'ami_unlink')
                
                if 'link' in uid1info:
                    if uid1info['link'].startswith('Agent/') and 'join' in uid1info:
                        queuename = uid1info['join'].get('queuename')
                        queueorgroup = uid1info['join'].get('queueorgroup')
                        clength = {'Xivo-Wait' : uid1info['time-link'] - uid1info['time-newchannel'],
                                   'Xivo-Chat' : uid1info['time-unlink'] - uid1info['time-link']
                                   }
                        log.info('%s STAT UNLINK %s %s' % (astid, queuename, clength))
                        if astid in self.stats_queues:
                                if queuename not in self.stats_queues[astid]:
                                        self.stats_queues[astid][queuename] = {}
                                time_now = int(time.time())
                                time_1ha = time_now - 3600
                                
                                for field in ['Xivo-Wait', 'Xivo-Chat']:
                                        if field not in self.stats_queues[astid][queuename]:
                                                self.stats_queues[astid][queuename].update({field : {}})
                                        self.stats_queues[astid][queuename][field][time_now] = clength[field]
                                        toremove = []
                                        for t in self.stats_queues[astid][queuename][field].keys():
                                                if t < time_1ha:
                                                        toremove.append(t)
                                        for t in toremove:
                                                del self.stats_queues[astid][queuename][field][t]
                                        ttotal = 0
                                        for val in self.stats_queues[astid][queuename][field].values():
                                                ttotal += val
                                        nvals = len(self.stats_queues[astid][queuename][field])
                                        if nvals > 0:
                                                self.weblist[queueorgroup][astid].keeplist[queuename]['queuestats'][field] = int(round(ttotal / nvals))
                                        else:
                                                self.weblist[queueorgroup][astid].keeplist[queuename]['queuestats'][field] = 0
                                                
                                tosend = { 'class' : queueorgroup,
                                           'function' : 'sendlist',
                                           'payload' : { astid : { queuename : self.weblist[queueorgroup][astid].keeplist[queuename] } }
                                           }
                                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                                 astid, self.weblist[queueorgroup][astid].getcontext(queuename))
                else:
                        log.warning('%s unlink : link not in %s' % (astid, uid1info))
                        
                if 'context' in self.uniqueids[astid][uid1]:
                        self.__sheet_alert__('unlink', astid, self.uniqueids[astid][uid1]['context'], event, {}, chan2)
                        
                if chan2.startswith('Agent/'):
                        agent_number = chan2[6:]
                        agent_id = self.weblist['agents'][astid].reverse_index.get(agent_number)
                        thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                        thisagent['agentstats'].update({'Xivo-Agent-StateTime' : time.time()})
                        thisagent['agentstats'].update({'Xivo-Agent-Status-Link' : {} })
                        for qval in thisagent['queues_by_agent'].itervalues():
                            qval['Xivo-QueueMember-StateTime'] = time.time()
                        for qval in thisagent['groups_by_agent'].itervalues():
                            qval['Xivo-QueueMember-StateTime'] = time.time()
                        tosend = { 'class' : 'agents',
                                   'function' : 'sendlist',
                                   'payload' : { astid : { agent_id : thisagent } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                         astid, thisagent.get('context'))
                        
                        # 'postcall' for the agent
                        status = 'postcall'
                        for uinfo in self.__find_userinfos_by_agentnum__(astid, agent_number):
                                self.__update_availstate__(uinfo, status)
                                self.__presence_action__(astid, agent_number, uinfo)
                                
                        if chan1 in self.queues_channels_list[astid]:
                                qname = self.queues_channels_list[astid][chan1]
                                del self.queues_channels_list[astid][chan1]
                                extraevent = {'xivo_queuename' : qname}
                                self.__sheet_alert__('agentunlinked', astid, CONTEXT_UNKNOWN, event, extraevent, chan1)
                                
                lstinfos = [uinfo1, uinfo2]
                if uinfo1_ag not in lstinfos:
                        lstinfos.append(uinfo1_ag)
                if uinfo2_ag not in lstinfos:
                        lstinfos.append(uinfo2_ag)
                for uinfo in lstinfos:
                        if uinfo:
                                status = 'postcall'
                                self.__update_availstate__(uinfo, status)
                                agent_id = uinfo.get('agentid')
                                ag = self.__agentnum__(uinfo)
                                if ag:
                                        self.__presence_action__(astid, ag, uinfo)
                                        thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                                        thisagent['agentstats'].update({'Xivo-Agent-StateTime' : time.time()})
                                        thisagent['agentstats'].update({'Xivo-Agent-Status-Link' : {} })
                                        for qval in thisagent['queues_by_agent'].itervalues():
                                            qval['Xivo-QueueMember-StateTime'] = time.time()
                                        for qval in thisagent['groups_by_agent'].itervalues():
                                            qval['Xivo-QueueMember-StateTime'] = time.time()
                                        tosend = { 'class' : 'agents',
                                                   'function' : 'sendlist',
                                                   'payload' : { astid : { agent_id : thisagent } }
                                                   }
                                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                                         astid, thisagent.get('context'))
                return
        
        def __presence_action__(self, astid, anum, userinfo):
                capaid = userinfo.get('capaid')
                status = userinfo.get('state')
                agent_channel = 'Agent/%s' % anum
                try:
                        if capaid not in self.capas:
                                return
                        presenceid = self.capas[capaid].presenceid
                        if presenceid not in self.presence_sections:
                                # useful in order to avoid internal presence keyword states to occur (postcall, incomingcall, ...)
                                return
                        presenceactions = self.presence_sections[presenceid].actions(status)
                        for paction in presenceactions:
                                params = paction.split('|')
                                if params[0] == 'queueadd' and len(params) > 2 and anum:
                                        self.__ami_execute__(astid, params[0], params[1], agent_channel, params[2])
                                elif params[0] == 'queueremove' and len(params) > 1 and anum:
                                        self.__ami_execute__(astid, params[0], params[1], agent_channel)
                                elif params[0] == 'queuepause' and len(params) > 1 and anum:
                                        self.__ami_execute__(astid, 'queuepause', params[1], agent_channel, 'true')
                                elif params[0] == 'queueunpause' and len(params) > 1 and anum:
                                        self.__ami_execute__(astid, 'queuepause', params[1], agent_channel, 'false')
                                elif params[0] == 'queuepause_all' and anum:
                                        agent_id = self.__find_agentid_by_agentnum__(astid, anum)
                                        if agent_id and agent_id in self.weblist['agents'][astid].keeplist:
                                                for qname, qv in self.weblist['agents'][astid].keeplist[agent_id]['queues_by_agent'].iteritems():
                                                        if qv.get('Paused') == '0':
                                                                self.__ami_execute__(astid, 'queuepause', qname, agent_channel, 'true')
                                elif params[0] == 'queueunpause_all' and anum:
                                        agent_id = self.__find_agentid_by_agentnum__(astid, anum)
                                        if agent_id and agent_id in self.weblist['agents'][astid].keeplist:
                                                for qname, qv in self.weblist['agents'][astid].keeplist[agent_id]['queues_by_agent'].iteritems():
                                                        if qv.get('Paused') == '1':
                                                                self.__ami_execute__(astid, 'queuepause', qname, agent_channel, 'false')
                                # features-related actions
                                elif params[0] in ['enablevoicemail', 'callrecord', 'callfilter', 'enablednd',
                                                   'enableunc', 'enablebusy', 'enablerna'] and len(params) > 1:
                                        if params[1] == 'false':
                                                booltonum = '0'
                                        else:
                                                booltonum = '1'
                                        rep = self.__build_features_put__(userinfo.get('astid') + '/' + userinfo.get('xivo_userid'),
                                                                          params[0],
                                                                          booltonum)
                                        self.__send_msg_to_cti_client__(userinfo, rep)
                except Exception:
                        log.exception('(__presence_action__) %s %s %s %s' % (astid, anum, capaid, status))
                return
        
        def __ami_execute__(self, astid, *args):
                actionid = self.amilist.execute(astid, *args)
                if actionid is not None:
                        self.amirequests[astid][actionid] = args
                        return actionid
        
        def ami_dtmf(self, astid, event):
                digit = event.get('Digit')
                direction = event.get('Direction')
                channel = event.get('Channel')
                uid = event.get('Uniqueid')
                begin = (event.get('Begin') == 'Yes')
                if direction == 'Received':
                        log.info('%s ami_dtmf <%s> %s %s %s %s' % (astid, digit, channel, uid, begin, time.time()))
                        event['xivo-timestamp'] = time.time()
                        self.ignore_dtmf[astid][uid] = event
                return
        
        def ami_hangup(self, astid, event):
                chan  = event.get('Channel')
                uid = event.get('Uniqueid')
                cause = event.get('Cause')
                causetxt = event.get('Cause-txt')
                #  0 - Unknown
                #  3 - No route to destination
                # 16 - Normal Clearing
                # 17 - User busy (see Orig #5)
                # 18 - No user responding (see Orig #1)
                # 19 - User alerting, no answer (see Orig #8, Orig #3, Orig #1 (soft hup))
                # 21 - Call rejected (attempting *8 when noone to pickup)
                # 24 - "lost" Call suspended
                # 27 - Destination out of order
                # 28 - Invalid number format (incomplete number)
                # 34 - Circuit/channel congestion
                
                if astid in self.sheetmanager and self.sheetmanager[astid].has_sheet(chan):
                        self.__sheet_disconnect__(astid, chan)

                if chan in self.parkedcalls[astid]:
                        del self.parkedcalls[astid][chan]
                if uid in self.ignore_dtmf[astid]:
                        del self.ignore_dtmf[astid][uid]
                if chan in self.queues_channels_list[astid]:
                        del self.queues_channels_list[astid][chan]
                        
                self.__remove_local_channel__(astid, chan)
                
                if uid in self.uniqueids[astid]:
                        vv = self.uniqueids[astid][uid]
                        if chan == vv['channel']:
                                vv.update({'hangup' : chan,
                                           'time-hangup' : time.time()})
                        if 'context' in vv:
                                self.__sheet_alert__('hangup', astid, vv['context'], event)
                        if 'origapplication' in vv and vv['origapplication'] == 'ChanSpy':
                                agent_id = vv['origapplication-data']['spied-agentid']
                                tosend = { 'class' : 'agentlisten',
                                           'astid' : astid,
                                           'agentid' : agent_id,
                                           'status' : 'stopped' }
                                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid,
                                                                 self.weblist['agents'][astid].keeplist[agent_id].get('context'))
                                del vv['origapplication']
                                del vv['origapplication-data']
                        if 'recorded' in vv:
                                agent_id = vv['recorded']
                                tosend = { 'class' : 'agentrecord',
                                           'astid' : astid,
                                           'agentid' : agent_id,
                                           'status' : 'stopped' }
                                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid,
                                                                 self.weblist['agents'][astid].keeplist[agent_id].get('context'))
                                del vv['recorded']
                else:
                        log.warning('%s HANGUP : uid %s has not been filled' % (astid, uid))
                        
                phoneid = self.__phoneid_from_channel__(astid, chan)
                trunkid = self.__trunkid_from_channel__(astid, chan)
                
                log.info('%s HANGUP (%s %s cause=%s (%s)) (phone trunk)=(%s %s) %s'
                         % (astid, uid, chan, cause, causetxt, phoneid, trunkid, self.uniqueids[astid][uid]))
                
                phidlist_hup = self.weblist['phones'][astid].ami_hangup(uid)
                tridlist_hup = self.weblist['trunks'][astid].ami_hangup(uid)
                for pp in phidlist_hup:
                    tosend = self.weblist['phones'][astid].status(pp)
                    tosend['astid'] = astid
                    context = self.weblist['phones'][astid].keeplist[pp]['context']
                    log.debug('   HUP phones context=%s tosend=%s' % (context, tosend))
                    self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context)
                for pp in tridlist_hup:
                    tosend = self.weblist['trunks'][astid].status(pp)
                    tosend['astid'] = astid
                    context = self.weblist['trunks'][astid].keeplist[pp]['context']
                    # XXX Beware, the trunks' contexts are usually different (from-extern vs. default)
                    log.debug('   HUP trunks tosend=%s' % (tosend))
                    self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context)
                phidlist_clear = self.weblist['phones'][astid].clear(uid)
                tridlist_clear = self.weblist['trunks'][astid].clear(uid)
                log.info('%s HANGUP (%s %s) cleared phones and trunks = %s %s' % (astid, uid, chan, phidlist_clear, tridlist_clear))
                
                if astid in self.uniqueids and uid in self.uniqueids[astid]:
                    del self.uniqueids[astid][uid]
                else:
                    log.warning('%s HANGUP : uniqueid %s not there (anymore?)' % (astid, uid))
                if astid in self.channels and chan in self.channels[astid]:
                    del self.channels[astid][chan]
                else:
                    log.warning('%s HANGUP : channel %s not there (anymore?)' % (astid, chan))
                return
        
        def ami_hanguprequest(self, astid, event):
                log.info('%s ami_hanguprequest : %s' % (astid, event))
                return
        
        def amiresponse_follows(self, astid, event, nocolon):
                actionid = event.get('ActionID')
                # log.info('%s amiresponse_follows : %s %s' % (astid, event, nocolon))
                if actionid in self.getvar_requests[astid]:
                        del self.getvar_requests[astid][actionid]
                if actionid in self.amirequests[astid]:
                        del self.amirequests[astid][actionid]
                return
        
        def amiresponse_success(self, astid, event, nocolon):
                msg = event.get('Message')
                actionid = event.get('ActionID')
                if msg is None:
                        if actionid is not None:
                                if astid in self.getvar_requests and actionid in self.getvar_requests[astid]:
                                        variable = event.get('Variable')
                                        value = event.get('Value')
                                        channel = self.getvar_requests[astid][actionid]['channel']
                                        if variable is not None and value is not None:
                                                if variable == 'MONITORED':
                                                        if value == 'true':
                                                                log.info('%s %s channel MONITORED' % (astid, channel))
                                                else:
                                                        log.info('%s AMI Response=Success (%s) : %s = %s (%s)'
                                                                 % (astid, actionid, variable, value, channel))
                        else:
                                log.warning('%s AMI Response=Success : event = %s' % (astid, event))
                elif msg == 'Extension Status':
                        # this is the reply to 'ExtensionState'
                        self.amiresponse_extensionstatus(astid, event)
                elif msg == 'Mailbox Message Count':
                        self.amiresponse_mailboxcount(astid, event)
                elif msg == 'Mailbox Status':
                        self.amiresponse_mailboxstatus(astid, event)
                elif msg == 'Authentication accepted':
                        log.info('%s : %s %s' % (astid, msg, nocolon))
                elif msg in ['Channel status will follow',
                             'Parked calls will follow',
                             'Agents will follow',
                             'Queue status will follow',
                             'Variable Set',
                             'Attended transfer started',
                             'Channel Hungup',
                             'Park successful',
                             'Meetme user list will follow',
                             'AOriginate successfully queued',
                             'Originate successfully queued',
                             'Redirect successful',
                             'Started monitoring channel',
                             'Stopped monitoring channel',
                             'Added interface to queue',
                             'Removed interface from queue',
                             'Interface paused successfully',
                             'Interface unpaused successfully',
                             'Agent logged out',
                             'Agent logged in']:
                        if actionid in self.amirequests[astid]:
                                # log.info('%s AMI Response=Success : (tracked) %s %s' % (astid, event, self.amirequests[astid][actionid]))
                                pass
                        elif actionid == '00':
                                log.debug('%s AMI Response=Success : %s (init)' % (astid, event))
                        else:
                                log.info('%s AMI Response=Success : (tracked) %s' % (astid, event))
                else:
                        log.warning('%s AMI Response=Success : untracked message (%s) <%s>' % (astid, actionid, msg))
                        
                if actionid in self.getvar_requests[astid]:
                        del self.getvar_requests[astid][actionid]
                if actionid in self.amirequests[astid]:
                        del self.amirequests[astid][actionid]
                return
        
        def amiresponse_error(self, astid, event, nocolon):
                msg = event.get('Message')
                actionid = event.get('ActionID')
                if msg == 'Originate failed':
                        if actionid in self.faxes:
                                faxid = self.faxes[actionid]
                                log.warning('%s AMI : fax not sent size=%s dest=%s hide=%s %s/%s'
                                            % (astid, faxid.size, faxid.number, faxid.hide,
                                               faxid.uinfo.get('astid'), faxid.uinfo.get('xivo_userid')))
                                tosend = { 'class' : 'faxprogress',
                                           'status' : 'ko',
                                           'reason' : 'orig' }
                                self.__send_msg_to_cti_client__(faxid.uinfo, self.__cjson_encode__(tosend))
                                del self.faxes[actionid]
                        else:
                                log.warning('%s AMI Response=Error : (%s) <%s>' % (astid, actionid, msg))
                elif msg == 'Authentication failed':
                    log.warning('%s AMI : Not allowed to connect to this Manager Interface' % astid)
                elif msg == 'Invalid/unknown command':
                    log.warning('%s AMI : Invalid/unknown command : %s' % (astid, event))
                elif msg in ['No such channel',
                             'No such agent',
                             'Permission denied',
                             'Member not dynamic',
                             'Extension not specified',
                             'Interface not found',
                             'No active conferences.',
                             'Unable to add interface: Already there',
                             'Unable to remove interface from queue: No such queue',
                             'Unable to remove interface: Not there'] :
                        if actionid in self.amirequests[astid]:
                            log.warning('%s AMI Response=Error : %s %s' % (astid, event, self.amirequests[astid][actionid]))
                        elif actionid == '00':
                            log.debug('%s AMI Response=Error : %s (init)' % (astid, event))
                        else:
                            log.warning('%s AMI Response=Error : %s' % (astid, event))
                else:
                        if actionid in self.amirequests[astid]:
                            log.warning('%s AMI Response=Error : (unknown message) %s %s' % (astid, event, self.amirequests[astid][actionid]))
                        else:
                            log.warning('%s AMI Response=Error : (unknown message) %s' % (astid, event))
                            
                if actionid in self.getvar_requests[astid]:
                    del self.getvar_requests[astid][actionid]
                if actionid in self.amirequests[astid]:
                    del self.amirequests[astid][actionid]
                return
        
        def amiresponse_mailboxcount(self, astid, event):
                mailbox = event.get('Mailbox')
                voicemailid = self.weblist['voicemail'][astid].reverse_index.get(mailbox)
                for userinfo in self.ulist_ng.keeplist.itervalues():
                        if 'voicemailid' in userinfo and userinfo.get('voicemailid') == voicemailid and userinfo.get('astid') == astid:
                                if userinfo['mwi']:
                                        # TODO : maybe send the infos if they have changed
                                        userinfo['mwi'][1] = event.get('OldMessages')
                                        userinfo['mwi'][2] = event.get('NewMessages')
                return
        
        def amiresponse_mailboxstatus(self, astid, event):
                mailbox = event.get('Mailbox')
                voicemailid = self.weblist['voicemail'][astid].reverse_index.get(mailbox)
                for userinfo in self.ulist_ng.keeplist.itervalues():
                        if 'voicemailid' in userinfo and userinfo.get('voicemailid') == voicemailid and userinfo.get('astid') == astid:
                                if userinfo['mwi']:
                                        log.debug('amiresponse_mailboxstatus voicemailid=%s user=%s %s=>%s'
                                                  % (voicemailid, userinfo.get('xivo_userid'), userinfo['mwi'][0], event.get('Waiting')))
                                        if userinfo['mwi'][0] != event.get('Waiting'):
                                            # only send if it has changed
                                            userinfo['mwi'][0] = event.get('Waiting')
                                            tosend = { 'class' : 'users',
                                                       'function' : 'update',
                                                       'user' : [userinfo.get('astid'),
                                                                 userinfo.get('xivo_userid')],
                                                       'subclass' : 'mwi',
                                                       'payload' : userinfo.get('mwi')
                                                       }
                                            self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                                             userinfo.get('astid'), userinfo.get('context'))
                return
        
        sippresence = {
                '-2' : 'Removed',
                '-1' : 'Deactivated',
                '0'  : 'Ready',
                '1'  : 'InUse', # Calling OR Online
                '2'  : 'Busy',
                '4'  : 'Unavailable',
                '8'  : 'Ringing',
                '16' : 'OnHold' }
        
        def amiresponse_extensionstatus(self, astid, event):
                # 90 seconds are needed to retrieve ~ 9000 phone statuses from an asterisk (on daemon startup)
                status  = event.get('Status')
                hint    = event.get('Hint')
                context = event.get('Context')
                exten   = event.get('Exten')
                if hint:
                        if hint.find('/') > 0:
                            # log.info('amiresponse_extensionstatus context=%s hint=%s status=%s' % (context, hint, status))
                            proto = hint.split('/')[0].lower()
                            phoneref = '.'.join([proto, context, hint.split('/')[1], exten])
                            if phoneref in self.weblist['phones'][astid].keeplist:
                                if self.weblist['phones'][astid].ami_extstatus(phoneref, status):
                                    # only sends information if the status changed
                                    tosend = self.weblist['phones'][astid].status(phoneref)
                                    tosend['astid'] = astid
                                    self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context)
                        elif hint.startswith('user:'):
                            log.warning('%s : user:xxx hint (phone %s not watched but present) : %s' % (astid, exten, event))
                            # {'Status': '4', 'Hint': 'user:218', 'Exten': '218', 'ActionID': 'pPKvoLQ97b', 'Context': 'default', ''})
                        else:
                            log.warning('%s : undefined hint : %s' % (astid, event))
                            # {'Status': '0', 'Hint': 'Custom:user:105', 'Exten': '105', 'ActionID': 'RBx7j3NSwI', 'Context': 'default'}
                else:
                        event.pop('Hint')
                        log.warning('%s : empty hint (watched or not, the phone %s is not present) : %s' % (astid, exten, event))
                        # {'Status': '-1', 'Hint': '', 'Exten': '205', 'ActionID': 'MfW1kqRV3j', 'Context': 'default', ''}
                return
        
        def ami_extensionstatus(self, astid, event):
                """
                New status for a phone (SIP only ?) (depends on hint ?),
                emitted by asterisk without request.
                """
                exten   = event.get('Exten')
                context = event.get('Context')
                status  = event.get('Status')
                
                for phoneref, b in self.weblist['phones'][astid].keeplist.iteritems():
                        if b['number'] == exten and b['context'] == context:
                            # log.info('ami_extensionstatus exten=%s context=%s status=%s' % (exten, context, status))
                            if self.weblist['phones'][astid].ami_extstatus(phoneref, status):
                                # only sends information if the status changed
                                tosend = self.weblist['phones'][astid].status(phoneref)
                                tosend['astid'] = astid
                                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context)
                return
        
        def ami_channelreload(self, astid, event):
                """
                This is an Asterisk 1.4 event.
                A few variables are available, however most of them seem useless in the realtime case.
                """
                log.info('%s ami_channelreload : %s' % (astid, event))
                return
        
        def ami_aoriginatesuccess(self, astid, event):
                event['Response'] = 'Success'
                self.ami_originateresponse(astid, event)
                return
        
        def ami_aoriginatefailure(self, astid, event):
                event['Response'] = 'Failure'
                self.ami_originateresponse(astid, event)
                return
        
        def ami_originatesuccess(self, astid, event):
                return
        def ami_originatefailure(self, astid, event):
                return
        
        def ami_originateresponse(self, astid, event):
                log.info('%s ami_originateresponse : %s' % (astid, event))
                uniqueid = event.get('Uniqueid')
                actionid = event.get('ActionID')
                reason = event.get('Reason')
                # 'Exten', 'Context', 'Channel', 'CallerID', 'CallerIDNum', 'CallerIDName'
                if uniqueid in self.uniqueids[astid]:
                        self.uniqueids[astid][uniqueid].update({'time-originateresponse' : time.time(),
                                                                'actionid' : actionid})
                        if actionid in self.origapplication[astid]:
                                self.uniqueids[astid][uniqueid].update(self.origapplication[astid][actionid])
                                
                                if self.origapplication[astid][actionid]['origapplication'] == 'ChanSpy' and reason == '4':
                                        agent_id = self.origapplication[astid][actionid]['origapplication-data']['spied-agentid']
                                        tosend = { 'class' : 'agentlisten',
                                                   'astid' : astid,
                                                   'agentid' : agent_id,
                                                   'status' : 'started' }
                                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid,
                                                                         self.weblist['agents'][astid].keeplist[agent_id].get('context'))
                                del self.origapplication[astid][actionid]
                                
                # Response = Success <=> Reason = '4'
                # Response = Failure <=>
                # Response =             Reason = '0' : (unable to request channel, phone might be unreachable)
                # Response =             Reason = '1' : (phone 'reachable' according to asterisk, but actually not reachable <-> Hangup #18
                #                                        soft hangup <-> Hangup #19)
                # Response =             Reason = '3' : timeout (phone did not answer)
                # Response =             Reason = '5' : busy (when xferring the originated phone, or all lines used (Hangup #17))
                # Response =             Reason = '8' : congestion (call rejected <-> Hangup #19)
                # for '5' and '8' reasons, Uniqueid might not be undefined ('<null>') ... (see aoriginate vs. originate, src channel Agent/ vs. SIP/)
                return
        
        def ami_messagewaiting(self, astid, event):
                """
                Instead of updating the mwi fields here, we request the current status,
                since the event returned when someone has erased one's mailbox seems to be
                incomplete.
                """
                [exten, context] = event.get('Mailbox').split('@')
                if exten and context:
                        self.__ami_execute__(astid, 'mailbox', exten, context)
                else:
                        log.warning('%s : do not request the mailbox for %s @ %s' % (exten, context))
                return
        
        def ami_newstate(self, astid, event):
                # log.info('%s STAT NEWS %s %s %s %s' % (astid, time.time(), uniqueid, event.get('Channel'), event.get('State')))
                channel = event.get('Channel')
                state = event.get('State')
                uid = event.get('Uniqueid')
                phoneid = self.__phoneid_from_channel__(astid, channel)
                state_table = {'Ringing': 'ringing',
                               'Ring': 'calling'}
#                               'Up', 'Down'
                if phoneid:
                    status = state_table.get(state)
                    if status is not None:
                        self.weblist['phones'][astid].ami_newstate(phoneid, uid, channel, status)
                return
        
        def ami_newcallerid(self, astid, event):
            # log.info('%s ami_newcallerid : %s' % (astid, event))
            uniqueid = event.get('Uniqueid')
            # event.get('CID-CallingPres') # 0 (Presentation Allowed, Not Screened)
            # warning : the second such event comes After the Dial
            if astid in self.uniqueids and uniqueid in self.uniqueids[astid]:
                self.uniqueids[astid][uniqueid].update({'calleridname' : event.get('CallerIDName'),
                                                        'calleridnum'  : event.get('CallerID')})
            return
        
        def ami_newexten(self, astid, event):
                application = event.get('Application')
                uniqueid = event.get('Uniqueid')
                if uniqueid in self.uniqueids[astid]:
                        if application == 'Dial':
                                self.uniqueids[astid][uniqueid]['context'] = event.get('Context')
                                #self.uniqueids[astid][uniqueid]['extension'] = event.get('Extension')
                                self.uniqueids[astid][uniqueid]['time-newexten-dial'] = time.time()
                        elif application == 'Macro':
                                # log.info('%s ami_newexten Macro : %s %s %s %s' % (astid, uniqueid,
                                # event.get('Context'),
                                # event.get('AppData'),
                                # event.get('Extension')))
                                pass
                        elif application == 'Park':
                                log.info('%s ami_newexten %s : %s %s %s'
                                         % (astid, application, uniqueid, event.get('Context'), event.get('Extension')))
                                self.uniqueids[astid][uniqueid]['parkexten'] = event.get('Extension')
                        elif application == 'WaitMusicOnHold':
                                log.info('%s ami_newexten %s : %s %s %s %s'
                                         % (astid, application, uniqueid, event.get('AppData'), event.get('Context'), event.get('Extension')))
                        # elif application == 'Queue': # to know how much seconds were requested
                        # log.info('%s ami_newexten %s : %s %s'
                        # % (astid, application, uniqueid, event.get('AppData').split('|')))
                        # set extension only if numeric
                        if event.get('Extension').isdigit():
                            self.uniqueids[astid][uniqueid]['extension'] = event.get('Extension')
                # TODO do not call __sheet_alert__ so often
                #self.__sheet_alert__('outgoing', astid, event.get('Context'), event)
                return
        
        def ami_newchannel(self, astid, event):
                # log.info('%s ami_newchannel : %s' % (astid, event))
                channel = event.get('Channel')
                uniqueid = event.get('Uniqueid')
                state = event.get('State')
                if channel == 'Substitution/voicemail':
                        # this kind of channel 1) seems useless for us 2) never hangups
                        return
                if channel == '':
                        # we keep a log, just in case
                        log.warning('%s empty channel name in event %s' % (astid, event))
                        return
                self.uniqueids[astid][uniqueid] = { 'channel' : channel,
                                                    'time-newchannel' : time.time(),
                                                    'dialplan_data' : {} }
                self.channels[astid][channel] = uniqueid
                if state == 'Rsrvd':
                        self.uniqueids[astid][uniqueid]['state'] = state
                # create "comm" in phone list if needed.
                phoneid = self.__phoneid_from_channel__(astid, channel)
                if phoneid:
                        self.weblist['phones'][astid].ami_newchannel(phoneid, uniqueid, channel)
                return
        
        def ami_parkedcall(self, astid, event):
                channel = event.get('Channel')
                cfrom   = event.get('From')
                exten   = event.get('Exten')
                timeout = event.get('Timeout')
                uid     = event.get('Uniqueid')
                uidfrom = event.get('UniqueidFrom') or event.get('FromUniqueid')
                calleridnum = event.get('CallerID')
                calleridname = event.get('CallerIDName')
                fromcalleridnum = event.get('FromCallerIDNum')
                fromcalleridname = event.get('FromCallerIDName')
                
                log.info('%s PARKEDCALL %s %s %s %s %s %s %s' % (astid, uidfrom, uid, cfrom, channel, exten, calleridnum, calleridname))
                if uid in self.uniqueids[astid]:
                        ctuid = self.uniqueids[astid][uid]
                        ctuid['parkexten-callback'] = exten
                        phoneid = self.__phoneid_from_channel__(astid, channel)
                        self.weblist['phones'][astid].ami_parkedcall(phoneid, uid, ctuid, exten)
                        tosend = self.weblist['phones'][astid].status(phoneid)
                        tosend['astid'] = astid
                        # TODO check context ???
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
                self.parkedcalls[astid][channel] = event
                # TODO check context ???
                tosend = { 'class' : 'parkcall',
                           'payload' : { 'status' : 'parkedcall',
                                         'astid' : astid,
                                         'channel' : channel,
                                         'exten' : exten,
                                         'fromchannel' : cfrom,
                                         'timeout' : timeout,
                                         'calleridnum' : calleridnum,
                                         'calleridname' : calleridname,
                                         'fromcalleridnum' : fromcalleridnum,
                                         'fromcalleridname' : fromcalleridname } }
                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
                return
        
        def ami_unparkedcall(self, astid, event):
                channel = event.get('Channel')
                cfrom   = event.get('From')
                exten   = event.get('Exten')
                uid     = event.get('Uniqueid')
                uidfrom = event.get('UniqueidFrom')
                (channel, _uid, clid, clidname) = self.__translate_local_channel_uid__(astid, channel, uid, None, None)
                (cfrom, _uid, clid, clidname) = self.__translate_local_channel_uid__(astid, cfrom, uidfrom, None, None)
                log.info('%s UNPARKEDCALL %s %s %s %s %s' % (astid, uidfrom, uid, cfrom, channel, exten))

                phoneidsrc = self.__phoneid_from_channel__(astid, cfrom)
                uinfo = self.__userinfo_from_phoneid__(astid, phoneidsrc)
                phoneiddst = self.__phoneid_from_channel__(astid, channel)
                
                if uidfrom in self.uniqueids[astid]:
                        ctuid = self.uniqueids[astid][uidfrom]
                        ctuid['parkexten-callback'] = event.get('CallerID')
                        ctuid['peerchannel'] = channel
                        self.weblist['phones'][astid].ami_unparkedcall(phoneidsrc, uidfrom, ctuid)
                if uid in self.uniqueids[astid]:
                        ctuid = self.uniqueids[astid][uid]
                        if uinfo is not None:
                            ctuid['parkexten-callback'] = uinfo.get('phonenum')
                        ctuid['peerchannel'] = cfrom
                        self.weblist['phones'][astid].ami_unparkedcall(phoneiddst, uid, ctuid)
                # a subsequent 'link' AMI event should make the new status transmitted
                tosend = { 'class' : 'parkcall',
                           'payload' : { 'status' : 'unparkedcall',
                                         'astid' : astid,
                                         'channel' : channel,
                                         'fromchannel' : cfrom,
                                         'exten' : exten } }
                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
                return
        
        def ami_parkedcallgiveup(self, astid, event):
            channel = event.get('Channel')
            exten   = event.get('Exten')
            uid     = event.get('Uniqueid')
            log.info('%s PARKEDCALLGIVEUP %s %s %s' % (astid, uid, channel, exten))
            tosend = { 'class' : 'parkcall',
                       'payload' : { 'status' : 'parkedcallgiveup',
                                     'astid' : astid,
                                     'channel' : channel,
                                     'exten' : exten } }
            self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
            return
        
        def ami_parkedcalltimeout(self, astid, event):
            channel = event.get('Channel')
            exten   = event.get('Exten')
            uid     = event.get('Uniqueid')
            log.info('%s PARKEDCALLTIMEOUT %s %s %s' % (astid, uid, channel, exten))
            tosend = { 'class' : 'parkcall',
                       'payload' : { 'status' : 'parkedcalltimeout',
                                     'astid' : astid,
                                     'channel' : channel,
                                     'exten' : exten } }
            self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
            return
        
        def ami_agentlogin(self, astid, event):
            log.info('%s ami_agentlogin : %s' % (astid, event))
            # {'Channel': 'SIP/103-08493360', 'Event': 'Agentlogin', 'Agent': '1001', 'Uniqueid': '1238067140.592'}
            return
        
        def ami_agentlogoff(self, astid, event):
            log.info('%s ami_agentlogoff : %s' % (astid, event))
            # {'Uniqueid': '1238067140.592', 'Event': 'Agentlogoff', 'Agent': '1001', 'Logintime': '5'}
            return
        
        def ami_agentcallbacklogin(self, astid, event):
                log.info('%s ami_agentcallbacklogin : %s' % (astid, event))
                agent = event.get('Agent')
                loginchan_split = event.get('Loginchan').split('@')
                phonenum = loginchan_split[0]
                if len(loginchan_split) > 1:
                    context = loginchan_split[1]
                else:
                    context = CONTEXT_UNKNOWN
                
                if astid in self.weblist['agents']:
                        agent_id = self.weblist['agents'][astid].reverse_index.get(agent)
                        thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                        thisagent['agentstats'].update({ 'status': 'AGENT_IDLE',
                                                         'agent_phone_number' : phonenum,
                                                         'agent_phone_context' : context,
                                                         'Xivo-AgentLoginTime' : time.time(),
                                                         'Xivo-ReceivedCalls' : 0,
                                                         'Xivo-LostCalls' : 0 })
                        tosend = { 'class' : 'agents',
                                   'function' : 'sendlist',
                                   'payload' : { astid : { agent_id : thisagent } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                         astid, thisagent.get('context'))
                return
        
        def ami_agentcallbacklogoff(self, astid, event):
                log.info('%s ami_agentcallbacklogoff : %s' % (astid, event))
                # {'Loginchan': '', 'Agent': '6101', 'Logintime': '1238067332', 'Reason': 'CommandLogoff', 'Event': 'Agentcallbacklogoff'}
                agent = event.get('Agent')
                loginchan_split = event.get('Loginchan').split('@')
                phonenum = loginchan_split[0]
                if len(loginchan_split) > 1:
                        context = loginchan_split[1]
                else:
                        context = CONTEXT_UNKNOWN
                if phonenum == 'n/a':
                        phonenum = AGENT_NO_PHONENUM
                
                if astid in self.weblist['agents']:
                        agent_id = self.weblist['agents'][astid].reverse_index.get(agent)
                        thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                        thisagent['agentstats'].update({'status': 'AGENT_LOGGEDOFF',
                                                        'agent_phone_number' : phonenum,
                                                        'agent_phone_context' : context})
                        # it looks like such events are sent even when the agent is not logged in,
                        # as replies to agentlogoff
                        if 'Xivo-ReceivedCalls' in thisagent['agentstats']:
                                del thisagent['agentstats']['Xivo-ReceivedCalls']
                        if 'Xivo-LostCalls' in thisagent['agentstats']:
                                del thisagent['agentstats']['Xivo-LostCalls']
                        if 'Xivo-AgentLoginTime' in thisagent['agentstats']:
                                del thisagent['agentstats']['Xivo-AgentLoginTime']
                        tosend = { 'class' : 'agents',
                                   'function' : 'sendlist',
                                   'payload' : { astid : { agent_id : thisagent } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                         astid, thisagent.get('context'))
                return
        
        def ami_agentcalled(self, astid, event):
                log.info('%s ami_agentcalled : %s' % (astid, event))
                queue = event.get('Queue')
                context = event.get('Context')
                agent_channel = event.get('AgentCalled')
                
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_agentcalled')
                        return
                
                queuecontext = self.weblist[queueorgroup][astid].getcontext(queue)
                if agent_channel.startswith('Agent/'):
                        agent_number = agent_channel[6:]
                        if astid in self.weblist['agents']:
                                log.info('%s ami_agentcalled (agent) %s %s' % (astid, queue, agent_channel))
                                agent_id = self.weblist['agents'][astid].reverse_index.get(agent_number)
                                thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                                if 'Xivo-ReceivedCalls' in thisagent['agentstats']:
                                        thisagent['agentstats']['Xivo-ReceivedCalls'] += 1
                                        thisagent['agentstats']['Xivo-LostCalls'] += 1
                                        tosend = { 'class' : 'agents',
                                                   'function' : 'sendlist',
                                                   'payload' : { astid : { agent_id : thisagent } }
                                                   }
                                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                                         astid, thisagent.get('context'))
                                # self.weblist['agents'][astid].keeplist[agent_id]['agentstats'] # XXX : count calls ++
                else:
                        # sip@queue
                        for uinfo in self.ulist_ng.keeplist.itervalues():
                            if uinfo.get('astid') == astid and uinfo.has_key('phonenum'):
                                exten = uinfo.get('phonenum')
                                phoneref = '.'.join([agent_channel.split('/')[0].lower(), queuecontext,
                                                     agent_channel.split('/')[1], exten])
                                if phoneref in uinfo.get('techlist'):
                                    peerchan = event.get('ChannelCalling')
                                    uid = self.channels[astid].get(peerchan)
                                    infos = {'peerchannel': peerchan,
                                             'status': 'ringing',
                                             'timestamp-dial': time.time()}
                                    if uid is not None and uid in self.uniqueids[astid]:
                                        infos['calleridnum'] = self.uniqueids[astid][uid].get('calleridnum')
                                        infos['calleridname'] = self.uniqueids[astid][uid].get('calleridname')
                                    self.weblist['phones'][astid].updatechan(phoneref, infos, self.channels[astid].get(event.get('ChannelCalled')))
                                    tosend = self.weblist['phones'][astid].status(phoneref)
                                    tosend['astid'] = astid
                                    self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context)
                                    td = '%s ami_agentcalled (phone) %s %s %s' % (astid, queue, agent_channel, uinfo.get('fullname'))
                                    log.info(td.encode('utf8'))
                                    
                # {'Extension': 's', 'CallerID': 'unknown', 'Priority': '2', 'ChannelCalling': 'IAX2/test-13', 'Context': 'macro-incoming_queue_call', 'CallerIDName': 'Comm. ', 'AgentCalled': 'iax2/192.168.0.120/101'}
                # {'Extension': '6678', 'CallerID': '102', 'CallerIDName': 'User2', 'Priority': '2', 'ChannelCalling': 'SIP/102-081cd460', 'Context': 'default', 'AgentName': 'Agent/6101', 'Event': 'AgentCalled', 'AgentCalled': 'Agent/6101'}
                return
        
        def ami_agentdump(self, astid, event):
            log.info('%s ami_agentdump : %s' % (astid, event))
            return
        
        def ami_agentconnect(self, astid, event):
                log.info('%s ami_agentconnect : %s' % (astid, event))
                agent_channel = event.get('Member')
                if agent_channel.startswith('Agent/'):
                        agent_number = agent_channel[6:]
                        if astid in self.weblist['agents']:
                                agent_id = self.weblist['agents'][astid].reverse_index.get(agent_number)
                                thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                                if 'Xivo-ReceivedCalls' in thisagent['agentstats']:
                                        thisagent['agentstats']['Xivo-LostCalls'] -= 1
                                        tosend = { 'class' : 'agents',
                                                   'function' : 'sendlist',
                                                   'payload' : { astid : { agent_id : thisagent } }
                                                   }
                                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend),
                                                                         astid, thisagent.get('context'))
                else:
                        log.warning('%s sip@queue (ami_agentconnect) %s' % (astid, event))
                # {'BridgedChannel': '1228753144.217', 'Member': 'SIP/103', 'MemberName': 'permmember', 'Queue': 'martinique', 'Uniqueid': '1228753144.216', 'Holdtime': '4', 'Event': 'AgentConnect', 'Channel': 'SIP/103-081d7358'}
                # {'Member': 'SIP/108', 'Queue': 'commercial', 'Uniqueid': '1215006134.1166', 'Holdtime': '9', 'Event': 'AgentConnect', 'Channel': 'SIP/108-08190098'}
                return
        
        def ami_agents(self, astid, event):
                # log.info('%s ami_agents : %s' % (astid, event))
                agent_number = event.get('Agent')
                if astid in self.weblist['agents']:
                        loginchan_split = event.get('LoggedInChan').split('@')
                        phonenum = loginchan_split[0]
                        if len(loginchan_split) > 1:
                                context = loginchan_split[1]
                        else:
                                context = CONTEXT_UNKNOWN
                        if phonenum == 'n/a':
                                phonenum = AGENT_NO_PHONENUM
                        # AGENT_ONCALL => LoggedInChan is SIP/102-094c67c8, LoggedInTime and TalkingTo are defined
                        # AGENT_LOGGEDOFF => LoggedInChan is 'n/a'
                        # AGENT_IDLE + really idle => LoggedInChan is 102@default, LoggedInTime is defined
                        # AGENT_IDLE + being called => LoggedInChan is Local/102@default-6417,1, LoggedInTime is defined
                        agent_id = self.weblist['agents'][astid].reverse_index.get(agent_number)
                        agent_channel = 'Agent/%s' % agent_number
                        # We don't use the 'Name' field since it is already given by XIVO properties
                        if agent_id:
                                nreceived = 0
                                ngot = 0
                                if agent_channel in self.last_agents[astid]:
                                        if 'AGENTCALLBACKLOGIN' in self.last_agents[astid][agent_channel] \
                                               and 'AGENTCALLBACKLOGOFF' not in self.last_agents[astid][agent_channel]:
                                                log.info('%s ami_agents %s %s' % (astid, agent_channel, self.last_agents[astid][agent_channel]))
                                                for z, zz in self.last_queues[astid].iteritems():
                                                        if agent_channel in zz and zz[agent_channel]:
                                                                if 'AGENTCALLED' in zz[agent_channel]:
                                                                        nreceived += 1
                                                                if 'CONNECT' in zz[agent_channel]:
                                                                        ngot += 1
                                nlost = nreceived - ngot
                                thisagentstats = self.weblist['agents'][astid].keeplist[agent_id]['agentstats']
                                thisagentstats.update( { 'status' : event.get('Status'),
                                                         'loggedintime' : event.get('LoggedInTime'),
                                                         'talkingto' : event.get('TalkingTo'),
                                                         
                                                         'Xivo-ReceivedCalls' : nreceived,
                                                         'Xivo-LostCalls' : nlost
                                                         } )
                                # to avoid the displayed values to be changed from 102@defaut to Local/102@default-6417,1
                                # or SIP/102-094c67c8, during the updates ... only sets the values when starting
                                if 'agent_phone_number' not in thisagentstats:
                                        thisagentstats['agent_phone_number'] = phonenum
                                if 'agent_phone_context' not in thisagentstats:
                                        thisagentstats['agent_phone_context'] = context
                                
                                if 'Xivo-Agent-Status-Recorded' not in thisagentstats:
                                        thisagentstats['Xivo-Agent-Status-Recorded'] = False
                                if 'Xivo-Agent-Status-Link' not in thisagentstats:
                                        thisagentstats['Xivo-Agent-Status-Link'] = {}
                                for xivofield in ['Xivo-NQJoined', 'Xivo-NQPaused', 'Xivo-NGJoined', 'Xivo-NGPaused']:
                                        if xivofield not in thisagentstats:
                                                thisagentstats[xivofield] = 0
                        else:
                                log.warning('%s : received statuses for agent <%s> unknown by XiVO' % (astid, agent_number))
                return
        
        def __queuemismatch__(self, astid, queuename, origin):
            log.warning('%s %s : no such queue (or group) %s (probably mismatch asterisk/XiVO)' % (astid, origin, queuename))
            # XXX todo : show queue 'queuename' to clean
            return
        
        def ami_queuecallerabandon(self, astid, event):
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_queuecallerabandon : no queue list has been defined' % astid)
                        return
                queue = event.get('Queue')
                uniqueid = event.get('Uniqueid')
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_queuecallerabandon')
                        return
                
                log.info('%s STAT ABANDON %s %s' % (astid, queue, uniqueid))
                self.__update_queue_stats__(astid, queue, queueorgroup, 'ABANDON')
                # Asterisk 1.4 event
                # {'Queue': 'qcb_00000', 'OriginalPosition': '1', 'Uniqueid': '1213891256.41', 'Position': '1', 'HoldTime': '2', 'Event': 'QueueCallerAbandon'}
                # it should then go to AMI leave and send the update
                return
        
        def ami_queueentry(self, astid, event):
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_queueentry : no queue list has been defined' % astid)
                        return
                queue = event.get('Queue')
                position = event.get('Position')
                wait = int(event.get('Wait'))
                channel = event.get('Channel')
                uniqueid = event.get('Uniqueid')
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_queueentry')
                        return
                
                calleridnum = None
                calleridname = None
                if uniqueid in self.uniqueids[astid]:
                        vv = self.uniqueids[astid][uniqueid]
                        if vv['channel'] == channel:
                                calleridnum = vv.get('calleridnum')
                                calleridname = vv.get('calleridname')
                # print 'AMI QueueEntry', astid, queue, position, wait, channel, event
                self.weblist[queueorgroup][astid].queueentry_update(queue, channel, position, time.time() - wait, calleridnum, calleridname)
                tosend = { 'class' : queueorgroup,
                           'function' : 'sendlist',
                           'payload' : { astid : { queue : self.weblist[queueorgroup][astid].keeplist[queue] } }
                           }
                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid,
                                                 self.weblist[queueorgroup][astid].getcontext(queue))
                return
        
        def ami_queuememberadded(self, astid, event):
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_queuememberadded : no queue list has been defined' % astid)
                        return
                queue = event.get('Queue')
                location = event.get('Location')
                paused = event.get('Paused')
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_queuememberadded')
                        return
                
                context_queue = self.weblist[queueorgroup][astid].getcontext(queue)
                if location.startswith('Agent/'):
                        if self.weblist['agents'].has_key(astid):
                                agent_id = self.weblist['agents'][astid].reverse_index.get(location[6:])
                                thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                                context_member = thisagent.get('context')
                        else:
                                context_member = None
                else:
                        context_member = context_queue
                        
                if not self.__check_context__(context_member, context_queue):
                        return
                
                event['Xivo-QueueMember-StateTime'] = time.time()
                if self.weblist[queueorgroup][astid].queuememberupdate(queue, location, event):
                        tosend = { 'class' : queueorgroup,
                                   'function' : 'update',
                                   'payload' : { astid :
                                                 { queue :
                                                   { 'agents_in_queue' :
                                                     { location : self.weblist[queueorgroup][astid].keeplist[queue]['agents_in_queue'][location]
                                                       } } } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context_queue)
                        
                if location.startswith('Agent/'):
                        self.weblist['agents'][astid].queuememberadded(queue, queueorgroup, location[6:], event)
                        self.__peragent_queue_summary__(astid, queueorgroup, agent_id, 'ami_queuememberadded')
                        tosend = { 'class' : 'agents',
                                   'function' : 'sendlist',
                                   'payload' : { astid : { agent_id : thisagent } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context_member)
                else:
                        log.warning('%s sip@queue (ami_queuememberadded) %s' % (astid, event))
                return
        
        def ami_queuememberremoved(self, astid, event):
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_queuememberremoved : no queue list has been defined' % astid)
                        return
                queue = event.get('Queue')
                location = event.get('Location')
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_queuememberremoved')
                        return
                
                context_queue = self.weblist[queueorgroup][astid].getcontext(queue)
                if location.startswith('Agent/'):
                        if self.weblist['agents'].has_key(astid):
                                agent_id = self.weblist['agents'][astid].reverse_index.get(location[6:])
                                thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                                context_member = thisagent.get('context')
                        else:
                                context_member = None
                else:
                        context_member = context_queue
                        
                if not self.__check_context__(context_member, context_queue):
                        return
                
                if self.weblist[queueorgroup][astid].queuememberremove(queue, location):
                        tosend = { 'class' : queueorgroup,
                                   'function' : 'sendlist',
                                   'payload' : { astid : { queue : self.weblist[queueorgroup][astid].keeplist[queue] } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context_queue)
                        
                if location.startswith('Agent/'):
                        self.weblist['agents'][astid].queuememberremoved(queue, queueorgroup, location[6:], event)
                        self.__peragent_queue_summary__(astid, queueorgroup, agent_id, 'ami_queuememberremoved')
                        tosend = { 'class' : 'agents',
                                   'function' : 'sendlist',
                                   'payload' : { astid : { agent_id : thisagent } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context_member)
                else:
                        log.warning('%s sip@queue (ami_queuememberremoved) %s' % (astid, event))
                return
        
        def ami_queuememberstatus(self, astid, event):
                # log.info('%s ami_queuememberstatus : %s' % (astid, event))
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_queuememberstatus : no queue list has been defined' % astid)
                        return
                status = event.get('Status')
                queue = event.get('Queue')
                location = event.get('Location')
                paused = event.get('Paused')
                
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_queuememberstatus')
                        return
                
                context_queue = self.weblist[queueorgroup][astid].getcontext(queue)
                if location.startswith('Agent/'):
                        if self.weblist['agents'].has_key(astid):
                                agent_id = self.weblist['agents'][astid].reverse_index.get(location[6:])
                                thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                                context_member = thisagent.get('context')
                        else:
                                context_member = None
                else:
                        context_member = context_queue
                        
                if not self.__check_context__(context_member, context_queue):
                        return
                
                if self.weblist[queueorgroup][astid].queuememberupdate(queue, location, event):
##                        tosend = { 'class' : queueorgroup,
##                                   'function' : 'sendlist',
##                                   'payload' : { astid :
##                                                 { queue :
##                                                   { 'agents_in_queue' :
##                                                     { location : self.weblist[queueorgroup][astid].keeplist[queue]['agents_in_queue'][location]
##                                                       } } } }
##                                   }
                        tosend = { 'class' : queueorgroup,
                                   'function' : 'sendlist',
                                   'payload' : { astid : { queue : self.weblist[queueorgroup][astid].keeplist[queue] } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context_queue)
                        
                if location.startswith('Agent/'):
                        if self.weblist['agents'][astid].queuememberupdate(queue, queueorgroup, location[6:], event):
                                self.__peragent_queue_summary__(astid, queueorgroup, agent_id, 'ami_queuememberstatus')
                                tosend = { 'class' : 'agents',
                                           'function' : 'sendlist',
                                           'payload' : { astid : { agent_id : thisagent } }
                                           }
                                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context_member)
                else:
                        # sip@queue
                        for uinfo in self.ulist_ng.keeplist.itervalues():
                            if uinfo.get('astid') == astid and uinfo.has_key('phonenum'):
                                exten = uinfo.get('phonenum')
                                phoneref = '.'.join([location.split('/')[0].lower(), context_queue,
                                                     location.split('/')[1], exten])
                                if phoneref in uinfo.get('techlist'):
                                    td = '%s ami_queuememberstatus : (%s %s %s) %s' % (astid, status, queue, location, uinfo.get('fullname'))
                                    log.info(td.encode('utf8'))
                # status = 3 => ringing
                # 2
                # status = 1 => do not ring anymore => the one who has not gone to '1' among the '3's is the one who answered ...
                # 4
                # 5 is received when unavailable members of a queue are attempted to be joined ... use agentcallbacklogoff to detect exit instead
                # 6
                # 7
                # + Link
                return
        
        def __peragent_queue_summary__(self, astid, queueorgroup, agent_id, wherefrom):
                try:
                        thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                        # context_agent = thisagent.get('context')
                        
                        if queueorgroup == 'queues':
                                thisagent['agentstats']['Xivo-NQJoined'] = 0
                                thisagent['agentstats']['Xivo-NQPaused'] = 0
                                for qname, astatus in self.weblist['agents'][astid].keeplist[agent_id]['queues_by_agent'].iteritems():
                                        if qname in self.weblist['queues'][astid].keeplist:
                                                # context_queue = self.weblist['queues'][astid].keeplist[qname]['context']
                                                # if context_agent == context_queue:
                                                if astatus.get('Status') in ['1', '3', '4', '5']:
                                                        thisagent['agentstats']['Xivo-NQJoined'] += 1
                                                if astatus.get('Paused') == '1':
                                                        thisagent['agentstats']['Xivo-NQPaused'] += 1
                        if queueorgroup == 'groups':
                                thisagent['agentstats']['Xivo-NGJoined'] = 0
                                thisagent['agentstats']['Xivo-NGPaused'] = 0
                                for qname, astatus in self.weblist['agents'][astid].keeplist[agent_id]['groups_by_agent'].iteritems():
                                        if qname in self.weblist['groups'][astid].keeplist:
                                                # context_queue = self.weblist['groups'][astid].keeplist[qname]['context']
                                                # if context_agent == context_queue:
                                                if astatus.get('Status') in ['1', '3', '4', '5']:
                                                        thisagent['agentstats']['Xivo-NGJoined'] += 1
                                                if astatus.get('Paused') == '1':
                                                        thisagent['agentstats']['Xivo-NGPaused'] += 1
                except Exception:
                        log.exception('(__peragent_queue_summary__) %s %s %s' % (astid, agent_id, wherefrom))
                return
        
        def ami_queuememberpaused(self, astid, event):
                # log.info('%s ami_queuememberpaused : %s' % (astid, event))
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_queuememberpaused : no queue list has been defined' % astid)
                        return
                queue = event.get('Queue')
                paused = event.get('Paused')
                location = event.get('Location')
                
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_queuememberpaused')
                        return
                
                context_queue = self.weblist[queueorgroup][astid].getcontext(queue)
                if location.startswith('Agent/'):
                        if self.weblist['agents'].has_key(astid):
                                agent_id = self.weblist['agents'][astid].reverse_index.get(location[6:])
                                thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                                context_member = thisagent.get('context')
                        else:
                                context_member = None
                else:
                        context_member = context_queue
                        
                if not self.__check_context__(context_member, context_queue):
                        return
                
                event['Xivo-QueueMember-StateTime'] = time.time()
                if self.weblist[queueorgroup][astid].queuememberupdate(queue, location, event):
                        tosend = { 'class' : queueorgroup,
                                   'function' : 'update',
                                   'payload' : { astid :
                                                 { queue :
                                                   { 'agents_in_queue' :
                                                     { location : self.weblist[queueorgroup][astid].keeplist[queue]['agents_in_queue'][location]
                                                       } } } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context_queue)
                        
                if location.startswith('Agent/'):
                        if self.weblist['agents'][astid].queuememberupdate(queue, queueorgroup, location[6:], event):
                                self.__peragent_queue_summary__(astid, queueorgroup, agent_id, 'ami_queuememberpaused')
                                qorg = '%s_by_agent' % queueorgroup
                                tosend = { 'class' : 'agents',
                                           'function' : 'update',
                                           'payload' : { astid :
                                                         { agent_id :
                                                           { 'agentstats' : thisagent['agentstats'],
                                                             qorg : { queue : thisagent[qorg][queue] } } } }
                                           }
                                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context_member)
                else:
                        log.warning('%s sip@queue (ami_queuememberpaused) %s' % (astid, event))
                return
        
        def ami_queueparams(self, astid, event):
                # log.info('%s ami_queueparams : %s' % (astid, event))
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_queueparams : no queue list has been defined' % astid)
                        return
                queue = event.get('Queue')
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_queueparams')
                        return
                
                if self.weblist[queueorgroup][astid].update_queuestats(queue, event):
                        # send an update only if something changed
                        log.info('%s ami_queueparams : %s' % (astid, event))
                        tosend = { 'class' : queueorgroup,
                                   'function' : 'sendlist',
                                   'payload' : { astid : { queue : self.weblist[queueorgroup][astid].keeplist[queue] } }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid,
                                                         self.weblist[queueorgroup][astid].getcontext(queue))
                return
        
        def ami_queuemember(self, astid, event):
                # log.info('%s ami_queuemember : %s' % (astid, event))
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_queuemember : no queue list has been defined' % astid)
                        return
                queue = event.get('Queue')
                location = event.get('Location')
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_queuemember')
                        return
                
                context_queue = self.weblist[queueorgroup][astid].getcontext(queue)
                if location.startswith('Agent/'):
                        if self.weblist['agents'].has_key(astid):
                                agent_id = self.weblist['agents'][astid].reverse_index.get(location[6:])
                                thisagent = self.weblist['agents'][astid].keeplist[agent_id]
                                context_member = thisagent.get('context')
                        else:
                                context_member = None
                else:
                        context_member = context_queue
                        
                if not self.__check_context__(context_member, context_queue):
                        return
                
                if location.startswith('Agent/'):
                        self.weblist[queueorgroup][astid].queuememberupdate(queue, location, event)
                        if self.weblist['agents'][astid].queuememberupdate(queue, queueorgroup, location[6:], event):
                                self.__peragent_queue_summary__(astid, queueorgroup, agent_id, 'ami_queuemember')
                                tosend = { 'class' : 'agents',
                                           'function' : 'sendlist',
                                           'payload' : { astid : { agent_id : thisagent } }
                                           }
                                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid, context_member)
                else:
                        # sip@queue
                        nlocations = self.__nlocations__(astid, location, context_queue)
                        if nlocations:
                                self.weblist[queueorgroup][astid].queuememberupdate(queue, nlocations[0], event)
                return
        
        def __nlocations__(self, astid, location, context):
            nlocations = []
            for uinfo in self.ulist_ng.keeplist.itervalues():
                if uinfo.get('astid') == astid and uinfo.has_key('phonenum'):
                    exten = uinfo.get('phonenum')
                    phoneref = '.'.join([location.split('/')[0].lower(), context,
                                         location.split('/')[1], exten])
                    if phoneref in uinfo.get('techlist'):
                        nlocations.append(phoneref)
            return nlocations
        
        def ami_queuestatuscomplete(self, astid, event):
                # log.info('%s ami_queuestatuscomplete : %s' % (astid, event))
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_queuestatuscomplete : no queue list has been defined' % astid)
                        return
                for qname in self.weblist['queues'][astid].get_queues():
                        self.__ami_execute__(astid, 'sendcommand', 'Command', [('Command', 'queue show %s' % qname)])
                        
                for aid, v in self.last_agents[astid].iteritems():
                        if v:
                                log.info('%s ami_queuestatuscomplete last agents = %s %s' % (astid, aid, v))
                for qid, vv in self.last_queues[astid].iteritems():
                        for aid, v in vv.iteritems():
                                if v:
                                        if qid in self.weblist['queues'][astid].keeplist and aid in self.weblist['queues'][astid].keeplist[qid]['agents_in_queue']:
                                                qastatus = self.weblist['queues'][astid].keeplist[qid]['agents_in_queue'][aid]
                                                qastatus['Xivo-QueueMember-StateTime'] = -1
                                                if qastatus.get('Paused') == '1':
                                                        if 'PAUSE' in v:
                                                                qpausetime = int(v['PAUSE'])
                                                                qastatus['Xivo-QueueMember-StateTime'] = qpausetime
                return
        
        def ami_userevent(self, astid, event):
                eventname = event.get('UserEvent')
                if eventname == 'MacroDid':
                        channel = event.get('CHANNEL')
                        uniqueid = event.get('UNIQUEID')
                        if uniqueid not in self.uniqueids[astid]:
                            log.warning('%s AMI UserEvent %s undefined %s' % (astid, eventname, uniqueid))
                            return
                        
                        calleridnum = event.get('XIVO_SRCNUM')
                        calleridname = event.get('XIVO_SRCNAME')
                        calleridton = event.get('XIVO_SRCTON')
                        calleridrdnis = event.get('XIVO_SRCRDNIS')
                        didnumber = event.get('XIVO_EXTENPATTERN')
                        context = None
                        
                        log.info('%s AMI UserEvent %s %s %s' % (astid, uniqueid, eventname, self.uniqueids[astid][uniqueid]))
                        self.uniqueids[astid][uniqueid]['DID'] = True # XXX to merge with xivo-origin
                        
                        dialplan_data = { 'xivo-astid' : astid,
                                          'xivo-origin' : 'did',
                                          'xivo-direction' : 'Entrant', # XXX to be read from a configuration
                                          'xivo-channel' : channel,
                                          'xivo-uniqueid' : uniqueid,
                                          'xivo-did' : didnumber,
                                          'xivo-calleridnum' : calleridnum,
                                          'xivo-calleridname' : calleridname,
                                          'xivo-calleridrdnis' : calleridrdnis,
                                          'xivo-calleridton' : calleridton
                                          }
                        self.uniqueids[astid][uniqueid]['dialplan_data'] = dialplan_data
                        for v, vv in self.weblist['incomingcalls'][astid].keeplist.iteritems():
                            if vv['exten'] == didnumber:
                                for ctxpart in ['usercontext', 'groupcontext', 'queuecontext',
                                                'meetmecontext',
                                                'voicemailcontext', 'voicemenucontext']:
                                    if vv.get(ctxpart):
                                        context = vv.get(ctxpart)
                                        break
                                log.info('%s ami_userevent %s' % (astid, vv))
                                break
                        
                        dialplan_data.update({'xivo-context' : context})
                        self.__dialplan_fill_src__(dialplan_data)
                        
                        # actions involving didnumber/callerid on channel could be carried out here
                        # did_takeovers = { '<incoming_callerid>' : { '<sda>' : {'number' : '<localnum>', 'context' : '<context>'} } }
                        did_takeovers = {}
                        if calleridnum in did_takeovers and didnumber in did_takeovers[calleridnum]:
                            destdetails = did_takeovers[calleridnum][didnumber]
                            # check channel !
                            self.__ami_execute__(astid, 'transfer', channel,
                                                 destdetails.get('number'),
                                                 destdetails.get('context'))
                        #self.__create_new_sheet__(astid, self.uniqueids[astid][uniqueid]['channel'])
                        self.__sheet_alert__('incomingdid', astid, context, event, dialplan_data, channel)
                        
                elif eventname in ['MacroUser', 'MacroGroup', 'MacroQueue', 'MacroOutcall', 'MacroMeetme']:
                    channel = event.get('CHANNEL')
                    uniqueid = event.get('UNIQUEID')
                    xivo_userid = event.get('XIVO_USERID')
                    xivo_dstid = event.get('XIVO_DSTID')
                    log.info('%s AMI UserEvent %s %s %s %s' % (astid, uniqueid, eventname, xivo_userid, xivo_dstid))
                    if uniqueid not in self.uniqueids[astid]:
                        log.warning('%s AMI UserEvent %s : uniqueid %s is undefined' % (astid, eventname, uniqueid))
                        return
                    
                    if xivo_userid:
                        dialplan_data = { 'xivo-astid' : astid,
                                          'xivo-origin' : 'internal',
                                          'xivo-userevent' : eventname,
                                          'xivo-channel' : channel,
                                          'xivo-uniqueid' : uniqueid,
                                          'xivo-userid' : xivo_userid,
                                          'xivo-dstid' : xivo_dstid
                                          }
                        if eventname not in ['MacroOutcall']:
                            dialplan_data['xivo-direction'] = 'Interne' # XXX to be read from a configuration
                        self.uniqueids[astid][uniqueid]['dialplan_data'] = dialplan_data
                        self.__dialplan_fill_src__(dialplan_data)
                        self.__dialplan_fill_dst__(dialplan_data)
                    else:
                        log.warning('%s AMI UserEvent %s : xivo_userid is not set' % (astid, eventname))
                        
                elif eventname == 'dialplan2cti':
                    channel = event.get('CHANNEL')
                    uniqueid = event.get('UNIQUEID')
                    cti_varname = event.get('VARIABLE')
                    dp_value = event.get('VALUE')
                    log.info('%s AMI UserEvent %s %s %s' % (astid, uniqueid, eventname, channel))
                    if self.uniqueids.has_key(astid):
                        if self.uniqueids[astid].has_key(uniqueid):
                            if self.uniqueids[astid][uniqueid].has_key('dialplan_data'):
                                dialplan_data = self.uniqueids[astid][uniqueid]['dialplan_data']
                                dialplan_data['dp-%s' % cti_varname] = dp_value
                            else:
                                log.warning('%s AMI UserEvent %s : no dialplan_data field (yet ?)' % (astid, eventname))
                                
                elif eventname == 'Custom':
                    uniqueid = event.get('UNIQUEID')
                    callerid = event.get('XIVO_SRCNUM')
                    channel = event.get('CHANNEL')
                    context = event.get('XIVO_REAL_CONTEXT', CONTEXT_UNKNOWN)
                    # XXX
                    # event in order to send a custom sheet almost anywhere in the dialplan
                    # TBD : define different kinds + define the destinations for each
                    # and later : define any other sheet event as a special case of this one (?)
                    # self.__sheet_alert__('custom', astid, context, event)
                    
                elif eventname == 'OUTCALL':
                        uniqueid = event.get('UNIQUEID')
                        context = event.get('XIVO_CONTEXT', CONTEXT_UNKNOWN)
                        
                        if uniqueid in self.uniqueids[astid]:
                                log.info('%s AMI UserEvent %s %s' % (astid, eventname, self.uniqueids[astid][uniqueid]))
                                self.uniqueids[astid][uniqueid]['OUTCALL'] = True
                        self.__sheet_alert__('outcall', astid, context, event)
                        
                elif eventname == 'Feature':
                        log.info('%s AMI UserEvent %s %s' % (astid, eventname, event))
                        function = event.get('Function')
                        if function in ['busy', 'rna', 'unc', 'vm', 'dnd',
                                        'callrecord', 'callfilter', 'bsfilter']:
                                repstr = { function : { 'enabled' : bool(int(event.get('Status'))),
                                                        'number' : event.get('Value') } }
                                userid = '%s/%s' % (astid, event.get('XIVO_USERID'))
                                tosend = { 'class' : 'features',
                                           'function' : 'update',
                                           'userid' : userid,
                                           'payload' : repstr }
                                # XXX context to be defined with the userid ?
                                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
                        elif function in ['agentstaticlogin', 'agentstaticlogoff', 'agentdynamiclogin']:
                                # events defined some time ago (r3546), unused yet
                                pass
                        else:
                                log.warning('%s ami_userevent : unknown UserEvent for Feature / %s'
                                            % (astid, function))
                        
                elif eventname == 'LocalCall':
                        log.info('%s AMI UserEvent %s %s' % (astid, eventname, event))
                        uniqueid = event.get('UNIQUEID')
                        appli = event.get('XIVO_ORIGAPPLI')
                        actionid = event.get('XIVO_ORIGACTIONID')
                        if uniqueid in self.uniqueids[astid]:
                                if appli == 'ChanSpy':
                                        self.uniqueids[astid][uniqueid].update({'time-chanspy' : time.time(),
                                                                                'actionid' : actionid})
                else:
                        log.info('%s AMI untracked UserEvent %s' % (astid, event))
                return
        
        def ami_faxsent(self, astid, event):
            log.info('%s : %s' % (astid, event))
            filename = event.get('FileName')
            if filename and os.path.isfile(filename):
                os.unlink(filename)
                log.info('faxsent event handler : removed %s' % filename)
                
            if event.get('PhaseEStatus') == '0':
                tosend = { 'class' : 'faxprogress',
                           'status' : 'ok' }
            else:
                tosend = { 'class' : 'faxprogress',
                           'status' : 'ko',
                           'reason' : '%s (code %s)' % (event.get('PhaseEString'),
                                                        event.get('PhaseEStatus')) }
            repstr = self.__cjson_encode__(tosend)
            # spandsp statuses (full list in /usr/include/spandsp/t30.h)
            # 22 Invalid response after sending a page
            # 50 Disconnected after permitted retries
            # 51 The call dropped prematurely
            
            # 'FileName': '/var/spool/asterisk/fax/astfaxsend-q6yZAKTJvU-0x48be7930.tif'
            faxid = filename[len('/var/spool/asterisk/fax/astfaxsend-'):-4]
            if faxid in self.faxes:
                self.__send_msg_to_cti_client__(self.faxes[faxid].uinfo, self.__cjson_encode__(tosend))
                del self.faxes[faxid]
            return
        
        def ami_faxreceived(self, astid, event):
            log.info('%s : %s' % (astid, event))
            context = event.get('Context', CONTEXT_UNKNOWN)
            self.__sheet_alert__('faxreceived', astid, context, event)
            return
        
        def ami_meetmejoin(self, astid, event):
                #log.debug('%s ami_meetmejoin %s' % (astid, event))
                meetmenum = event.get('Meetme')
                channel = event.get('Channel')
                uniqueid = event.get('Uniqueid')
                usernum = event.get('Usernum')
                isadmin = (event.get('Admin') == 'Yes')
                pseudochan = event.get('PseudoChan')
                calleridnum = event.get('CallerIDnum')
                calleridname = event.get('CallerIDname')
                
                (meetmeref, meetmeid) = self.weblist['meetme'][astid].byroomnum(meetmenum)
                if meetmeref is None:
                        log.warning('%s ami_meetmejoin : unable to find room %s' % (astid, meetmenum))
                        return
                
                if uniqueid not in meetmeref['uniqueids']:
                        phoneid = self.__phoneid_from_channel__(astid, channel)
                        if phoneid:
                                self.weblist['phones'][astid].ami_meetmejoin(phoneid, uniqueid, meetmenum)
                                self.__update_phones_trunks__(astid, phoneid, None, None, None, 'ami_meetmejoin')

                        uinfo = self.__userinfo_from_phoneid__(astid, phoneid)
                        if uinfo:
                                userid = '%s/%s' % (uinfo.get('astid'), uinfo.get('xivo_userid'))
                        else:
                                userid = ''
                        if isadmin:
                                meetmeref['adminid'] = userid
                        meetmeref['uniqueids'][uniqueid] = { 'usernum' : usernum,
                                                             'mutestatus' : 'off',
                                                             'recordstatus' : 'off',
                                                             'time_start' : time.time(),
                                                             'userid' : userid,
                                                             'fullname' : calleridname,
                                                             'phonenum' : calleridnum }
                        log.info('%s ami_meetmejoin : (%s) channel %s added to meetme %s'
                                 % (astid, uniqueid, channel, meetmenum))
                        tosend = { 'class' : 'meetme',
                                   'function' : 'update',
                                   'payload' : { 'action' : 'join',
                                                 'astid' : astid,
                                                 'meetmeid': meetmeid,
                                                 'roomnum' : meetmenum,
                                                 'roomname' : meetmeref['name'],
                                                 'adminid' : meetmeref['adminid'],
                                                 'uniqueid' : uniqueid,
                                                 'details' : meetmeref['uniqueids'][uniqueid] }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
                else:
                        log.warning('%s ami_meetmejoin : (%s) channel %s already in meetme %s'
                                    % (astid, uniqueid, channel, meetmenum))
                return
        
        def ami_meetmeleave(self, astid, event):
                meetmenum = event.get('Meetme')
                channel = event.get('Channel')
                uniqueid = event.get('Uniqueid')
                usernum = event.get('Usernum')
                
                (meetmeref, meetmeid) = self.weblist['meetme'][astid].byroomnum(meetmenum)
                if meetmeref is None:
                        log.warning('%s ami_meetmeleave : unable to find room %s' % (astid, meetmenum))
                        return
                
                if uniqueid in meetmeref['uniqueids']:
                        tosend = { 'class' : 'meetme',
                                   'function' : 'update',
                                   'payload' : { 'action' : 'leave',
                                                 'astid' : astid,
                                                 'meetmeid' : meetmeid,
                                                 'roomnum' : meetmenum,
                                                 'uniqueid' : uniqueid,
                                                 'details' : meetmeref['uniqueids'][uniqueid] }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
                        del meetmeref['uniqueids'][uniqueid]
                        log.info('%s ami_meetmeleave : (%s) channel %s removed from meetme %s'
                                 % (astid, uniqueid, channel, meetmenum))
                else:
                        log.warning('%s ami_meetmeleave : (%s) channel %s not in meetme %s'
                                    % (astid, uniqueid, channel, meetmenum))
                return
        
        def ami_meetmemute(self, astid, event):
                meetmenum = event.get('Meetme')
                channel = event.get('Channel')
                uniqueid = event.get('Uniqueid')
                usernum = event.get('Usernum')
                
                (meetmeref, meetmeid) = self.weblist['meetme'][astid].byroomnum(meetmenum)
                if meetmeref is None:
                        log.warning('%s ami_meetmemute : unable to find room %s' % (astid, meetmenum))
                        return
                
                mutestatus = event.get('Status')
                if uniqueid in meetmeref['uniqueids']:
                        meetmeref['uniqueids'][uniqueid]['mutestatus'] = mutestatus
                        tosend = { 'class' : 'meetme',
                                   'function' : 'update',
                                   'payload' : { 'action' : 'mutestatus',
                                                 'astid' : astid,
                                                 'meetmeid' : meetmeid,
                                                 'roomnum' : meetmenum,
                                                 'uniqueid' : uniqueid,
                                                 'details' : meetmeref['uniqueids'][uniqueid] }
                                   }
                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
                else:
                        log.warning('%s ami_meetmemute : (%s) channel %s not in meetme %s'
                                    % (astid, uniqueid, channel, meetmenum))
                return
        
        def ami_meetmetalking(self, astid, event):
                log.info('%s ami_meetmetalking : %s' % (astid, event))
                return
        
        def ami_meetmelist(self, astid, event):
                meetmenum = event.get('Conference')
                channel = event.get('Channel')
                uniqueid = event.get('Uniqueid')
                usernum = event.get('UserNumber')
                mutestatus = 'off'
                if event.get('Muted') in ['Yes', 'By admin']:
                        mutestatus = 'on'
                recordstatus = 'off' # XXX how do I actually know that ?
                isadmin = (event.get('Admin') == 'Yes')
                pseudochan = event.get('PseudoChan')
                calleridnum = event.get('CallerIDNum')
                calleridname = event.get('CallerIDName')
                
                (meetmeref, meetmeid) = self.weblist['meetme'][astid].byroomnum(meetmenum)
                if meetmeref is None:
                        log.warning('%s ami_meetmelist : unable to find room %s' % (astid, meetmenum))
                        return
                
                if uniqueid not in meetmeref['uniqueids']:
                        phoneid = self.__phoneid_from_channel__(astid, channel)
                        uinfo = self.__userinfo_from_phoneid__(astid, phoneid)
                        if uinfo:
                                userid = '%s/%s' % (uinfo.get('astid'), uinfo.get('xivo_userid'))
                        else:
                                userid = ''
                        if isadmin:
                                meetmeref['adminid'] = userid
                        meetmeref['uniqueids'][uniqueid] = { 'usernum' : usernum,
                                                             'mutestatus' : mutestatus,
                                                             'recordstatus' : recordstatus,
                                                             'userid' : userid,
                                                             'fullname' : calleridname,
                                                             'phonenum' : calleridnum }
                else:
                        log.warning('%s ami_meetmelist : (%s) channel %s already in meetme %s'
                                    % (astid, uniqueid, channel, meetmenum))
                
                # {'Talking': 'Not monitored', 'Admin': 'No', 'MarkedUser': 'No', 'Role': 'Talk and listen'}
                return
        
        def ami_status(self, astid, event):
                # log.info('%s ami_status : %s' % (astid, event))
                state = event.get('State')
                appliname = event.get('Application')
                applidata = event.get('AppData').split('|')
                uniqueid = event.get('Uniqueid')
                channel = event.get('Channel')
                calleridnum = event.get('CallerIDNum')
                calleridname = event.get('CallerIDName')
                link = event.get('Link')
                log.debug('%s ami_status : %s, %s, %s, %s, %s, %s' % (astid, uniqueid, channel, appliname, calleridnum, calleridname, state))
                
                # warning : channel empty when appliname is 'VoiceMail'
                actionid = self.amilist.execute(astid, 'getvar', channel, 'MONITORED')
                self.getvar_requests[astid][actionid] = {'channel' : channel, 'variable' : 'MONITORED'}
                
                if uniqueid not in self.uniqueids[astid]:
                        self.uniqueids[astid][uniqueid] = { 'channel' : channel,
                                                            'application' : appliname,
                                                            'calleridname' : calleridname,
                                                            'calleridnum' : calleridnum }
                        self.channels[astid][channel] = uniqueid
                else:
                        log.warning('%s : uid %s already in uniqueids list, updating callerid' % (astid, uniqueid))
                        log.debug('%s : uid %s = %s' % (astid, uniqueid, self.uniqueids[astid][uniqueid]))
                        self.uniqueids[astid][uniqueid].update({ 'calleridname' : calleridname, 'calleridnum' : calleridnum })
                        log.debug('%s : uid %s = %s' % (astid, uniqueid, self.uniqueids[astid][uniqueid]))
                        return
                
                if appliname in ['Parked Call']:
                        # these cases should be properly handled by the ParkedCalls requests
                        return
                elif appliname == 'MeetMe':
                        # this case should have already been handled by the MeetMeList request
                        # however knowing how much time has been spent might be useful here
                        meetmenum = applidata[0]
                        seconds = int(event.get('Seconds'))
                        (meetmeref, meetmeid) = self.weblist['meetme'][astid].byroomnum(meetmenum)
                        if meetmeref is not None:
                                if uniqueid in meetmeref['uniqueids']:
                                        meetmeref['uniqueids'][uniqueid]['time_start'] = time.time() - seconds
                        #link = 'dummy'
                        phoneid = self.__phoneid_from_channel__(astid, channel)
                        if phoneid in self.weblist['phones'][astid].keeplist:
                                if seconds is None:
                                        status = 'linked-called'
                                else:
                                        status = 'linked-caller'
                                self.weblist['phones'][astid].keeplist[phoneid]['comms'][uniqueid] = { 'status' : status,
                                                                                                       'thischannel' : channel,
                                                                                                       #'peerchannel' : link,
                                                                                                       'time-link' : seconds,
                                                                                                       'timestamp-link' : time.time() - seconds,
                                                                                                       'calleridnum' : applidata[0],
                                                                                                       'calleridname' : '<meetme>' }
                                self.weblist['phones'][astid].setlinenum(phoneid, uniqueid)
                        return
                elif appliname == 'Playback':
                        log.info('%s ami_status : %s %s' % (astid, appliname, applidata))
                        # context.startswith('macro-phonestatus'):
                        # *10
                        return
                elif appliname in ['BackGround',
                                   'VoiceMail', 'VoiceMailMain',
                                   'AgentLogin', 'Transferred Call', 'RetryDial']:
                        log.info('%s ami_status : %s %s' % (astid, appliname, applidata))
                        return
                elif appliname in ['Queue', 'AppQueue']:
                        log.debug('%s ami_status  %s' % (astid, event))
                        #if not applidata == '(Outgoing Line)':
                        #    return
                elif appliname == '':
                        log.warning('%s ami_status (empty appliname) : %s' % (astid, event))
                        if state == 'Rsrvd':
                                self.uniqueids[astid][uniqueid]['state'] = state
                                # Zap/pseudo-xxx or DAHDI/pseudo-xxx are here when there is a meetme going on
                        return
                elif appliname not in ['AppDial', 'Dial', 'RxFAX']:
                        log.warning('%s ami_status : %s %s (untracked)' % (astid, appliname, applidata))
                        
                if state == 'Up':
                        # warning : very rarely, link seems to be None here
                        seconds = event.get('Seconds')
                        priority = event.get('Priority')
                        context = event.get('Context')
                        extension = event.get('Extension')
                        log.info('%s ami_status %s %s %s %s / %s %s %s %s / %s %s'
                                 % (astid,
                                    state, uniqueid, channel, link,
                                    priority, context, extension, seconds,
                                    calleridnum, calleridname))
                        self.uniqueids[astid][uniqueid].update({'link' : link})
                        self.events_link[astid][uniqueid] = time.time()
#                        phoneid = self.__phoneid_from_channel__(astid, channel)
#                        if phoneid in self.weblist['phones'][astid].keeplist:
#                                if seconds is None:
#                                        status = 'linked-called'
#                                else:
#                                        status = 'linked-caller'
#                                self.weblist['phones'][astid].keeplist[phoneid]['comms'][uniqueid] = { 'status' : status,
#                                                                                                       'thischannel' : channel,
#                                                                                                       'peerchannel' : link,
#                                                                                                       'time-link' : 0 }
                        #inversed stuff
                        if link is not None:
                            otherphoneid = self.__phoneid_from_channel__(astid, link)
                            if otherphoneid in self.weblist['phones'][astid].keeplist:
                                if seconds is None:
                                        status = 'linked-caller'
                                else:
                                        status = 'linked-called'
                                ts = int(uniqueid.split('.')[0])
                                self.weblist['phones'][astid].keeplist[otherphoneid]['comms'][uniqueid] = { 'status' : status,
                                                                                                            'thischannel' : link,
                                                                                                            'peerchannel' : channel,
                                                                                                            'time-link' : time.time() - ts,
                                                                                                            'timestamp-link' : ts,
                                                                                                            'calleridnum' : calleridnum,
                                                                                                            'calleridname' : calleridname }
                                self.weblist['phones'][astid].setlinenum(otherphoneid, uniqueid)
                        if context is not None:
                                if context == 'macro-user':
                                        # ami_status xivo-obelisk Up 1222872105.4001 SIP/fpotiquet-085d8238 IAX2/asteriskisdn-11652 / None None None None
                                        # ami_status xivo-obelisk Up 1222872013.3986 IAX2/asteriskisdn-11652 SIP/fpotiquet-085d8238 / 31 macro-user s 178
                                        pass
                                elif context == 'macro-outcall':
                                        # ami_status xivo-obelisk Up 1222872070.3997 SIP/115-085dd7b8 IAX2/asteriskisdn-9917 / 4 macro-outcall dial 121
                                        # ami_status xivo-obelisk Up 1222872070.3998 IAX2/asteriskisdn-9917 SIP/115-085dd7b8 / None None None None
                                        pass
                                elif context == 'macro-pickup':
                                        # *21
                                        pass
                                elif context == 'macro-recsnd':
                                        pass
                                elif context == 'macro-did':
                                        pass
                                elif context == 'macro-forward':
                                        pass
                                elif context == 'macro-voicemenu':
                                        pass
                elif state == 'Ring': # Caller
                        seconds = event.get('Seconds')
                        context = event.get('Context')
                        extension = event.get('Extension')
                        log.info('%s ami_status %s %s %s %s %s %s' % (astid,
                                                                      state, uniqueid, channel, context, extension, seconds))
                elif state == 'Ringing': # Callee
                        log.info('%s ami_status %s %s %s' % (astid, state, uniqueid, channel))
                        # ami_status noxivo-clg Ringing 1227551513.1501 SIP/102-081ce708
                        # ami_status noxivo-clg Ring 1227551513.1500 SIP/103-b6b12cc8 2 default 102
                elif state == 'Dialing': # ???
                        log.info('%s ami_status %s %s %s' % (astid, state, uniqueid, channel))
                else:
                        log.warning('%s ami_status : unknown state %s' % (astid, event))
                return
        
        def ami_statuscomplete(self, astid, event):
                log.info('%s ami_statuscomplete' % astid)
                return
        
        def ami_join(self, astid, event):
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_join : no queue list has been defined' % astid)
                        return
                # log.info('%s ami_join : %s' % (astid, event))
                chan  = event.get('Channel')
                clidnum  = event.get('CallerID')
                clidname = event.get('CallerIDName')
                queue = event.get('Queue')
                count = event.get('Count')
                position = event.get('Position')
                uniqueid = event.get('Uniqueid')
                
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_join')
                        return
                
                queuecontext = self.weblist[queueorgroup][astid].getcontext(queue)
                if uniqueid in self.uniqueids[astid]:
                    self.uniqueids[astid][uniqueid]['join'] = {'queuename' : queue,
                                                               'queueorgroup' : queueorgroup,
                                                               'time' : time.time()}
                    self.uniqueids[astid][uniqueid]['calleridnum'] = clidnum
                    self.uniqueids[astid][uniqueid]['calleridname'] = clidname
                    ddata = self.uniqueids[astid][uniqueid].get('dialplan_data')
                else:
                    ddata = {}
                log.info('%s STAT JOIN %s %s %s' % (astid, queue, chan, uniqueid))
                self.__update_queue_stats__(astid, queue, queueorgroup, 'ENTERQUEUE')
                self.__sheet_alert__('incoming' + queueorgroup[:-1], astid, queuecontext, event, ddata, chan)
                log.info('%s AMI Join (Queue) %s %s %s' % (astid, queue, chan, count))
                self.weblist[queueorgroup][astid].queueentry_update(queue, chan, position, time.time(),
                                                                    clidnum, clidname)
                event['Calls'] = count
                self.weblist[queueorgroup][astid].update_queuestats(queue, event)
                tosend = { 'class' : queueorgroup,
                           'function' : 'sendlist',
                           'payload' : { astid : { queue : self.weblist[queueorgroup][astid].keeplist[queue] } }
                           }
                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid,
                                                 self.weblist[queueorgroup][astid].getcontext(queue))
                self.__ami_execute__(astid, 'sendqueuestatus', queue)
                return
        
        def ami_leave(self, astid, event):
                if astid not in self.weblist['queues']:
                        log.warning('%s ami_leave : no queue list has been defined' % astid)
                        return
                # print 'AMI Leave (Queue)', astid, event
                chan  = event.get('Channel')
                queue = event.get('Queue')
                count = event.get('Count')
                reason = event.get('Reason')
                # -1 unknown (among which agent reached)
                # 0 unknown (among which caller abandon)
                # 1 timeout, 2 joinempty, 3 leaveempty, 6 full
                if self.weblist['queues'][astid].hasqueue(queue):
                        queueorgroup = 'queues'
                elif self.weblist['groups'][astid].hasqueue(queue):
                        queueorgroup = 'groups'
                else:
                        self.__queuemismatch__(astid, queue, 'ami_leave')
                        return
                
                queuecontext = self.weblist[queueorgroup][astid].getcontext(queue)
                log.info('%s AMI Leave (Queue) %s %s %s %s' % (astid, queue, chan, reason, event))
                
                self.weblist[queueorgroup][astid].queueentry_remove(queue, chan)
                event['Calls'] = count
                self.weblist[queueorgroup][astid].update_queuestats(queue, event)
                
                tosend = { 'class' : queueorgroup,
                           'function' : 'sendlist',
                           'payload' : { astid : { queue : self.weblist[queueorgroup][astid].keeplist[queue] } }
                           }
                self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid,
                                                 self.weblist[queueorgroup][astid].getcontext(queue))
                
                # always sets the queue information since it might not have been deleted
                self.queues_channels_list[astid][chan] = queue
                self.__ami_execute__(astid, 'sendqueuestatus', queue)
                return
        
        def ami_rename(self, astid, event):
                oldname = event.get('Oldname')
                newname = event.get('Newname')
                uid = event.get('Uniqueid')
                (oldname, _uid, _clid, _clidn) = self.__translate_local_channel_uid__(astid, oldname, uid)
                
                if oldname == newname:
                        log.info('%s AMI Rename : nothing to do %s => %s' % (astid, oldname, newname))
                        return

                oldphoneid = self.__phoneid_from_channel__(astid, oldname)
                oldtrunkid = self.__trunkid_from_channel__(astid, oldname)
                
                if uid in self.uniqueids[astid] and oldname == self.uniqueids[astid][uid]['channel']:
                        self.uniqueids[astid][uid]['channel'] = newname
                        del self.channels[astid][oldname]
                        self.channels[astid][newname] = uid
                        log.info('%s AMI Rename %s %s %s (success)' % (astid, uid, oldname, newname))
                else:
                        log.info('%s AMI Rename %s %s %s (failure)' % (astid, uid, oldname, newname))
                        
                newphoneid = self.__phoneid_from_channel__(astid, newname)
                newtrunkid = self.__trunkid_from_channel__(astid, newname)
                
                rr = [oldphoneid is None, newphoneid is None, oldtrunkid is None, newtrunkid is None]
                if rr == rename_phph:
                        self.weblist['phones'][astid].ami_rename(oldphoneid, newphoneid, oldname, newname, uid)
                elif rr == rename_trtr:
                        self.weblist['trunks'][astid].ami_rename(oldtrunkid, newtrunkid, oldname, newname, uid)
                elif rr == rename_phtr:
                        tomove = self.weblist['phones'][astid].ami_rename_totrunk(oldphoneid, oldname, newname, uid)
                        self.weblist['trunks'][astid].ami_rename_fromphone(newtrunkid, oldname, newname, uid, tomove)
                elif rr == rename_trph:
                        tomove = self.weblist['trunks'][astid].ami_rename_tophone(oldtrunkid, oldname, newname, uid)
                        self.weblist['phones'][astid].ami_rename_fromtrunk(newphoneid, oldname, newname, uid, tomove)
                else:
                        log.warning('%s AMI Rename : unknown configuration : %s %s %s %s'
                                    % (astid, oldphoneid, newphoneid, oldtrunkid, newtrunkid))
                        
                self.__update_phones_trunks__(astid, oldphoneid, newphoneid, oldtrunkid, newtrunkid, 'ami_rename')

                # for new sheet management
                if astid in self.sheetmanager and self.sheetmanager[astid].has_sheet(newname) and newtrunkid is not None:
                    for _uid, t in self.weblist['trunks'][astid].keeplist[newtrunkid]['comms'].iteritems():
                        if t['thischannel']==newname:
                            phoneid = self.__phoneid_from_channel__(astid, t['peerchannel'])
                            if phoneid:
                                uinfo = self.__userinfo_from_phoneid__(astid, phoneid)
                                if uinfo:
                                    self.__update_sheet_user__(astid, uinfo, newname)

                return
        
        def ami_alarm(self, astid, event):
            log.warning('%s ami_alarm : %s' % (astid, event))
            return
        
        def ami_alarmclear(self, astid, event):
            log.warning('%s ami_alarmclear : %s' % (astid, event))
            return
        
        def ami_dndstate(self, astid, event):
            log.info('%s ami_dndstate : %s' % (astid, event))
            return
        
        def ami_cdr(self, astid, event):
            log.info('%s ami_cdr : %s' % (astid, event))
            return
        
        def ami_atxfer(self, astid, event):
                log.info('%s ami_atxfer : %s' % (astid, event))
                chansrc = event.get('SrcChannel');
                uidsrc = event.get('SrcUniqueid');
                chandst = event.get('DstChannel');
                uiddst = event.get('DstUniqueid');
                phoneidsrc = self.__phoneid_from_channel__(astid, chansrc)
                if phoneidsrc is not None:
                    # to be checked
                    log.debug('ami_atxfer grrr %s' % self.weblist['phones'][astid].keeplist[phoneidsrc]['comms'])
                    self.weblist['phones'][astid].keeplist[phoneidsrc]['comms'][uidsrc]['atxfer'] = True
                #phoneiddst = self.__phoneid_from_channel__(astid, chandst)
                #if phoneiddst is not None:
                #    # to be checked
                #    self.weblist['phones'][astid].keeplist[phoneiddst]['comms'][uiddst]['atxfer'] = True
                return
        
        # END of AMI events
        
        def manage_cticommand(self, userinfo, icommand):
                ret = None
                repstr = None
                astid    = userinfo.get('astid')
                username = userinfo.get('user')
                context  = userinfo.get('context')
                capaid   = userinfo.get('capaid')
                ucapa    = self.capas[capaid].all() # userinfo.get('capas')
                userid = '%s/%s' % (userinfo.get('astid'), userinfo.get('xivo_userid'))
                
                try:
                        if icommand.name == 'database':
                                if self.capas[capaid].match_funcs(ucapa, 'database'):
                                        repstr = database_update(me, icommand.args)
                        elif icommand.name == 'json':
                                classcomm = icommand.struct.get('class')
                                dircomm = icommand.struct.get('direction')
                                
                                if dircomm is not None and dircomm == 'xivoserver' and classcomm in self.commnames:
                                        if 'login' in userinfo and 'sessiontimestamp' in userinfo.get('login'):
                                                userinfo['login']['sessiontimestamp'] = time.time()
                                                
                                        if classcomm not in ['keepalive', 'logclienterror', 'history', 'logout']:
                                                log.info('command attempt %s from user:%s : %s' % (classcomm, userid, icommand.struct))
                                        if classcomm not in ['keepalive', 'logclienterror', 'history', 'availstate', 'actionfiche']:
                                                self.__fill_user_ctilog__(userinfo, 'cticommand:%s' % classcomm)
                                        if classcomm == 'meetme':
                                                function = icommand.struct.get('function')
                                                argums = icommand.struct.get('functionargs')
                                                if self.capas[capaid].match_funcs(ucapa, 'conference'):
                                                        if function == 'record' and len(argums) > 3:
                                                                castid = argums[0]
                                                                meetmenum = argums[1]
                                                                uniqueid = argums[3]
                                                                if castid in self.uniqueids and uniqueid in self.uniqueids[castid]:
                                                                        channel = self.uniqueids[castid][uniqueid]['channel']
                                                                        (meetmeref, meetmeid) = self.weblist['meetme'][castid].byroomnum(meetmenum)
                                                                        if meetmeref is not None and uniqueid in meetmeref['uniqueids']:
                                                                                if userid == meetmeref['adminid']:
                                                                                        datestring = time.strftime('%Y%m%d-%H%M%S', time.localtime())
                                                                                        meetmeref['uniqueids'][uniqueid]['recordstatus'] = 'on'
                                                                                        self.__ami_execute__(castid, 'monitor', channel,
                                                                                                             'cti-meetme-%s-%s' % (meetmenum, datestring))
                                                                else:
                                                                        log.warning('(%s) either astid %s or uniqueid %s is unknown'
                                                                                    % (function, castid, uniqueid))
                                                        elif function == 'unrecord' and len(argums) > 3:
                                                                castid = argums[0]
                                                                meetmenum = argums[1]
                                                                uniqueid = argums[3]
                                                                if castid in self.uniqueids and uniqueid in self.uniqueids[castid]:
                                                                        channel = self.uniqueids[castid][uniqueid]['channel']
                                                                        (meetmeref, meetmeid) = self.weblist['meetme'][castid].byroomnum(meetmenum)
                                                                        if meetmeref is not None and uniqueid in meetmeref['uniqueids']:
                                                                                if userid == meetmeref['adminid']:
                                                                                        meetmeref['uniqueids'][uniqueid]['recordstatus'] = 'off'
                                                                                        self.__ami_execute__(castid, 'stopmonitor', channel)
                                                                else:
                                                                        log.warning('(%s) either astid %s or uniqueid %s is unknown'
                                                                                    % (function, castid, uniqueid))
                                                        elif function in ['kick', 'mute', 'unmute'] and len(argums) > 2:
                                                                castid = argums[0]
                                                                meetmenum = argums[1]
                                                                usernum = argums[2]
                                                                uniqueid = argums[3]
                                                                if castid in self.uniqueids and uniqueid in self.uniqueids[castid]:
                                                                        channel = self.uniqueids[castid][uniqueid]['channel']
                                                                        (meetmeref, meetmeid) = self.weblist['meetme'][castid].byroomnum(meetmenum)
                                                                        if meetmeref is not None and uniqueid in meetmeref['uniqueids']:
                                                                                if userid == meetmeref['adminid'] or userid == meetmeref['uniqueids'][uniqueid]['userid']:
                                                                                        self.__ami_execute__(castid, 'sendcommand',
                                                                                                             'Command', [('Command', 'meetme %s %s %s'
                                                                                                                          % (function, meetmenum, usernum))])
                                                                else:
                                                                        log.warning('(%s) either astid %s or uniqueid %s is unknown'
                                                                                    % (function, castid, uniqueid))
                                                        elif function == 'getlist':
                                                                fullstat = {}
                                                                for iastid, v in self.weblist['meetme'].iteritems():
                                                                        fullstat[iastid] = v.keeplist
                                                                tosend = { 'class' : 'meetme',
                                                                           'function' : 'sendlist',
                                                                           'payload' : fullstat }
                                                                repstr = self.__cjson_encode__(tosend)
                                                else:
                                                        log.debug('capability conference not matched')
                                                        
                                        elif classcomm == 'history':
                                                if self.capas[capaid].match_funcs(ucapa, 'history'):
                                                        repstr = self.__build_history_string__(icommand.struct.get('peer'),
                                                                                               icommand.struct.get('size'),
                                                                                               icommand.struct.get('mode'),
                                                                                               icommand.struct.get('morerecentthan'))
                                                        
                                        elif classcomm == 'directory-search':
                                                if self.capas[capaid].match_funcs(ucapa, 'directory'):
                                                        repstr = self.__build_customers__(context, icommand.struct.get('pattern'))
                                                        
                                        elif classcomm == 'keepalive':
                                                nbytes = icommand.struct.get('rate-bytes', -1)
                                                nmsec = icommand.struct.get('rate-msec', -1)
                                                nsamples = icommand.struct.get('rate-samples', -1)
                                                if nbytes > 0:
                                                        if nmsec > 0:
                                                                rate = float(nbytes) / nmsec
                                                                log.info('keepalive from user:%s (%d %d/%d = %.1f bytes/ms)'
                                                                         % (userid, nsamples, nbytes, nmsec, rate))
                                                        else:
                                                                log.info('keepalive from user:%s (%d %d/0 > %.1f bytes/ms)'
                                                                         % (userid, nsamples, nbytes, float(nbytes)))
                                                                
                                        elif classcomm == 'logclienterror':
                                                log.warning('shouldNotOccur from user:%s : %s : %s'
                                                            % (userid, icommand.struct.get('classmethod'), icommand.struct.get('message')))
                                                
                                        elif classcomm == 'faxsend':
                                                if self.capas[capaid].match_funcs(ucapa, 'fax'):
                                                        newfax = cti_fax.Fax(userinfo,
                                                                             icommand.struct.get('size'),
                                                                             icommand.struct.get('number'),
                                                                             icommand.struct.get('hide'))
                                                        self.faxes[newfax.reference] = newfax
                                                        tosend = { 'class' : 'faxsend',
                                                                   'tdirection' : 'upload',
                                                                   'fileid' : newfax.reference }
                                                        repstr = self.__cjson_encode__(tosend)
                                                        
                                        elif classcomm == 'endinit':
                                                tosend = { 'class' : 'endinit' }
                                                repstr = self.__cjson_encode__(tosend)
                                                
                                        elif classcomm == 'availstate':
                                                if self.capas[capaid].match_funcs(ucapa, 'presence'):
                                                        # updates the new status and sends it to other people
                                                        repstr = self.__update_availstate__(userinfo, icommand.struct.get('availstate'))
                                                        self.__presence_action__(astid, self.__agentnum__(userinfo), userinfo)
                                                        self.__fill_user_ctilog__(userinfo, 'cticommand:%s' % classcomm)
                                                        
                                        elif classcomm == 'getguisettings':
                                                tosend = { 'class' : 'getguisettings',
                                                           'payload' : self.capas[capaid].getguisettings()
                                                           }
                                                repstr = self.__cjson_encode__(tosend)
                                                
                                        elif classcomm == 'message':
                                                pass
                                                # if self.capas[capaid].match_funcs(ucapa, 'messages'):
                                                # tosend = { 'class' : 'message',
                                                # 'payload' : ['%s/%s' % (astid, username),
                                                # '<%s>' % icommand.struct.get('message')] }
                                                # self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid)
                                                
                                        elif classcomm == 'featuresget':
                                                if self.capas[capaid].match_funcs(ucapa, 'features'):
##                                        if username in userlist[astid]:
##                                                userlist[astid][username]['monit'] = icommand.args
                                                        repstr = self.__build_features_get__(icommand.struct.get('userid'))
                                                        
                                        elif classcomm == 'featuresput':
                                                if self.capas[capaid].match_funcs(ucapa, 'features'):
                                                        log.info('%s %s' % (classcomm, icommand.struct))
                                                        rep = self.__build_features_put__(icommand.struct.get('userid'),
                                                                                          icommand.struct.get('function'),
                                                                                          icommand.struct.get('value'),
                                                                                          icommand.struct.get('destination'))
                                                        self.__send_msg_to_cti_client__(userinfo, rep)
                                                        
                                        elif classcomm == 'logout':
                                                stopper = icommand.struct.get('stopper')
                                                log.info('logout request from user:%s : stopper=%s' % (userid, stopper))
                                                
                                        elif classcomm == 'callcampaign':
                                                argums = icommand.struct.get('command')
                                                if argums[0] == 'fetchlist':
                                                        tosend = { 'class' : 'callcampaign',
                                                                   'payload' : { 'command' : 'fetchlist',
                                                                                 'list' : [ '101', '102', '103' ] } }
                                                        repstr = self.__cjson_encode__(tosend)
                                                elif argums[0] == 'startcall':
                                                        exten = argums[1]
                                                        self.__originate_or_transfer__(AMI_ORIGINATE,
                                                                                       userinfo,
                                                                                       'user:special:me',
                                                                                       'ext:%s' % exten)
                                                        tosend = { 'class' : 'callcampaign',
                                                                   'payload' : { 'command' : 'callstarted',
                                                                                 'number' : exten } }
                                                        repstr = self.__cjson_encode__(tosend)
                                                elif argums[0] == 'stopcall':
                                                        tosend = { 'class' : 'callcampaign',
                                                                   'payload' : { 'command' : 'callstopped',
                                                                                 'number' : argums[1] } }
                                                        repstr = self.__cjson_encode__(tosend)
                                                        # self.__send_msg_to_cti_client__(userinfo,
                                                        # '{'class':"callcampaign","command":"callnext","list":["%s"]}' % icommand.args[1])
                                        elif classcomm in ['originate', 'transfer', 'atxfer']:
                                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                                        repstr = self.__originate_or_transfer__(classcomm,
                                                                                                userinfo,
                                                                                                icommand.struct.get('source'),
                                                                                                icommand.struct.get('destination'))
                                        elif classcomm in ['hangup', 'simplehangup']:
                                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                                        repstr = self.__hangup__(userinfo,
                                                                                 icommand.struct.get('source'),
                                                                                 False)
                                                        # (classcomm == 'hangup'))
                                        elif classcomm == 'pickup':
                                                if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                                        # on Thomson, it picks up the last received call
                                                        self.__ami_execute__(userinfo.get('astid'),
                                                                             'sendcommand',
                                                                             'Command',
                                                                             [('Command',
                                                                               'sip notify event-talk %s'
                                                                               % userinfo.get('phonenum'))])
                                        elif classcomm == 'actionfiche':
                                                infos = icommand.struct.get('infos')
                                                if infos:
                                                        uniqueid = infos.get('uniqueid')
                                                        channel = infos.get('channel')
                                                        locastid = infos.get('astid')
                                                        
                                                        timestamps = infos.get('timestamps')
                                                        log.info('%s %s : %s %s' % (locastid, classcomm, uniqueid, timestamps))
                                                        dtime = None
                                                        if 'agentlinked' in timestamps and 'agentunlinked' in timestamps:
                                                                dtime = timestamps['agentunlinked'] - timestamps['agentlinked']
                                                        self.__fill_user_ctilog__(userinfo, 'cticommand:%s' % classcomm, infos.get('buttonname'), dtime)
                                                        if locastid in self.uniqueids and uniqueid in self.uniqueids[locastid]:
                                                                if 'buttonname' in infos:
                                                                        buttonname = infos.get('buttonname')
                                                                        log.info('%s : buttonname=%s astid=%s channel=%s %s' % (classcomm,
                                                                                                                                buttonname,
                                                                                                                                locastid,
                                                                                                                                channel,
                                                                                                                                self.uniqueids[locastid][uniqueid]))
                                                                        if buttonname == 'answer':
                                                                                self.__ami_execute__(locastid, 'transfer',
                                                                                                     channel,
                                                                                                     userinfo.get('phonenum'),
                                                                                                     userinfo.get('context'))
                                                                        elif buttonname == 'hangup':
                                                                                self.__ami_execute__(locastid, 'hangup', channel)
                                                                        elif buttonname == 'refuse':
                                                                                self.__ami_execute__(locastid, 'transfer',
                                                                                                     channel,
                                                                                                     'BUSY',
                                                                                                     userinfo.get('context'))
                                                                else:
                                                                        log.info('%s : %s' % (classcomm, self.uniqueids[locastid][uniqueid]))
                                        elif classcomm in ['phones', 'users', 'agents', 'queues', 'groups']:
                                                function = icommand.struct.get('function')
                                                if function == 'getlist':
                                                        repstr = self.__getlist__(userinfo, classcomm, context)
                                                        
                                        elif classcomm == 'agent':
                                                argums = icommand.struct.get('command')
                                                if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                                        repstr = self.__agent__(userinfo, argums)
                                        elif classcomm == 'sheet':
                                                self.__handle_sheet_command__(userinfo, icommand.struct)
                                else:
                                        log.warning('unallowed json event %s' % icommand.struct)
                                        
                except Exception:
                        log.exception('(manage_cticommand) %s %s %s'
                                      % (icommand.name, icommand.args, userinfo.get('login').get('connection')))
                        
                if repstr is not None: # might be useful to reply sth different if there is a capa problem for instance, a bad syntaxed command
                        try:
                                userinfo.get('login').get('connection').sendall(repstr + '\n')
                        except Exception:
                                log.exception('(sendall) attempt to send <%s ...> (%d chars) failed'
                                              % (repstr[:40], len(repstr)))
                return ret
        
        def __build_history_string__(self, requester_id, nlines, kind, morerecentthan):
                userinfo = self.ulist_ng.keeplist[requester_id]
                astid = userinfo.get('astid')
                termlist = userinfo.get('techlist')
                reply = []
                for termin in termlist:
                        [techno, ctx, phoneid, exten] = termin.split('.')
                        # print '__build_history_string__', requester_id, nlines, kind, techno, phoneid
                        try:
                                hist = self.__update_history_call__(self.configs[astid], techno, phoneid, nlines, kind, morerecentthan)
                                for x in hist:
                                        ritem = { 'duration' : x[10] }
                                        # 'x1' : x[1].replace('"', '')
                                        # 'x11' : x[11]
                                        try:
                                                ritem['ts'] = x[0].isoformat()
                                        except:
                                                ritem['ts'] = x[0]
                                        ritem['termin'] = termin
                                        if kind == '0':
                                                ritem['direction'] = 'OUT'
                                                num = x[3].replace('"', '')
                                                cidname = num
                                                ritem['fullname'] = cidname
                                        else:   # display callerid for incoming calls
                                                ritem['direction'] = 'IN'
                                                ritem['fullname'] = x[1].replace('"', '').decode('utf8')
                                        reply.append(ritem)
                        except Exception:
                                log.exception('history : (client %s, termin %s)'
                                              % (requester_id, termin))
                                
                if len(reply) > 0:
                        # sha1sum = sha.sha(''.join(reply)).hexdigest()
                        tosend = { 'class' : 'history',
                                   'payload' : reply }
                        return self.__cjson_encode__(tosend)
                else:
                        return
                
        def __find_agentid_by_agentnum__(self, astid, agent_number):
                agent_id = None
                if astid in self.weblist['agents']:
                        agent_id = self.weblist['agents'][astid].reverse_index.get(agent_number)
                return agent_id
        
        def __find_channel_by_agentnum__(self, astid, agent_number):
                chans = []
                for uinfo in self.__find_userinfos_by_agentnum__(astid, agent_number):
                        techref = uinfo.get('techlist')[0]
                        for v, vv in self.uniqueids[astid].iteritems(): ## XXX
                                if 'channel' in vv:
                                        if techref == self.__phoneid_from_channel__(astid, vv['channel']):
                                                chans.append(vv['channel'])
                return chans
        
        def __find_userinfos_by_agentnum__(self, astid, agent_number):
                userinfos = []
                agent_id = self.__find_agentid_by_agentnum__(astid, agent_number)
                for uinfo in self.ulist_ng.keeplist.itervalues():
                        if 'agentid' in uinfo and uinfo.get('agentid') == agent_id and uinfo.get('astid') == astid:
                                userinfos.append(uinfo)
                return userinfos
        
        def __agent__(self, userinfo, commandargs):
                myastid = None
                myagentnum = None
                if 'agentid' in userinfo:
                        myastid = userinfo['astid']
                        myagentnum = self.__agentnum__(userinfo)
                        
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
                                if astid is not None and anum:
                                        for queuename in queuenames:
                                                self.__ami_execute__(astid, 'queueremove', queuename, 'Agent/%s' % anum)
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
                                if astid is not None and anum:
                                        for queuename in queuenames:
                                                self.__ami_execute__(astid, 'queueadd', queuename, 'Agent/%s' % anum, spause)
                elif subcommand == 'pause':
                        if len(commandargs) > 1:
                                queuenames = commandargs[1].split(',')
                                if len(commandargs) > 3:
                                        astid = commandargs[2]
                                        anum = commandargs[3]
                                else:
                                        astid = myastid
                                        anum = myagentnum
                                if astid is not None and anum:
                                        for queuename in queuenames:
                                                self.__ami_execute__(astid, 'queuepause', queuename, 'Agent/%s' % anum, 'true')
                elif subcommand == 'unpause':
                        if len(commandargs) > 1:
                                queuenames = commandargs[1].split(',')
                                if len(commandargs) > 3:
                                        astid = commandargs[2]
                                        anum = commandargs[3]
                                else:
                                        astid = myastid
                                        anum = myagentnum
                                agent_channel = 'Agent/%s' % anum
                                if astid is not None and anum:
                                        for queuename in queuenames:
                                                self.__ami_execute__(astid, 'queuepause', queuename, agent_channel, 'false')
                                                
                elif subcommand == 'pause_all':
                        if len(commandargs) > 2:
                                astid = commandargs[1]
                                anum = commandargs[2]
                        else:
                                astid = myastid
                                anum = myagentnum
                        agent_channel = 'Agent/%s' % anum
                        agent_id = self.__find_agentid_by_agentnum__(astid, anum)
                        if agent_id and agent_id in self.weblist['agents'][astid].keeplist:
                                for qname, qv in self.weblist['agents'][astid].keeplist[agent_id]['queues_by_agent'].iteritems():
                                        if qv.get('Paused') == '0':
                                                self.__ami_execute__(astid, 'queuepause', qname, agent_channel, 'true')
                                                
                elif subcommand == 'unpause_all':
                        if len(commandargs) > 2:
                                astid = commandargs[1]
                                anum = commandargs[2]
                        else:
                                astid = myastid
                                anum = myagentnum
                        agent_channel = 'Agent/%s' % anum
                        agent_id = self.__find_agentid_by_agentnum__(astid, anum)
                        if agent_id and agent_id in self.weblist['agents'][astid].keeplist:
                                for qname, qv in self.weblist['agents'][astid].keeplist[agent_id]['queues_by_agent'].iteritems():
                                        if qv.get('Paused') == '1':
                                                self.__ami_execute__(astid, 'queuepause', qname, agent_channel, 'false')
                                                
                elif subcommand in ['login', 'logout',
                                    'record', 'stoprecord',
                                    'listen', 'stoplisten',
                                    'getfile', 'getfilelist']:
                        agentphonenum = None
                        if len(commandargs) > 2:
                            astid = commandargs[1]
                            anum = commandargs[2]
                            uinfo = None
                            agent_id = self.__find_agentid_by_agentnum__(astid, anum)
                            for uinfo_iter in self.ulist_ng.keeplist.itervalues():
                                if 'agentid' in uinfo_iter and uinfo_iter.get('agentid') == agent_id and uinfo_iter.get('astid') == astid:
                                    uinfo = uinfo_iter
                                    break
                            if len(commandargs) > 3:
                                agentphonenum = commandargs[3]
                        else:
                            uinfo = userinfo
                        
                        if subcommand == 'login':
                            self.__login_agent__(uinfo, agentphonenum)
                        elif subcommand == 'logout':
                            self.__logout_agent__(uinfo)
                        elif subcommand == 'record':
                                datestring = time.strftime('%Y%m%d-%H%M%S', time.localtime())
                                agent_id = self.weblist['agents'][astid].reverse_index.get(anum)
                                channels = self.__find_channel_by_agentnum__(astid, anum)
                                for channel in channels:
                                        self.__ami_execute__(astid, 'monitor', channel, 'cti-agent-%s-%s' % (datestring, anum))
                                        self.weblist['agents'][astid].keeplist[agent_id]['agentstats'].update({'Xivo-Agent-Status-Recorded' : True})
                                        log.info('started monitor on %s %s (agent %s)' % (astid, channel, anum))
                                        tosend = { 'class' : 'agentrecord',
                                                   'astid' : astid,
                                                   'agentid' : agent_id,
                                                   'status' : 'started' }
                                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid,
                                                                         self.weblist['agents'][astid].keeplist[agent_id].get('context'))
                                        if channel in self.channels[astid]:
                                                uniqueid = self.channels[astid][channel]
                                                self.uniqueids[astid][uniqueid].update({'recorded' : agent_id})
                        elif subcommand == 'stoprecord':
                                agent_id = self.weblist['agents'][astid].reverse_index.get(anum)
                                channels = self.__find_channel_by_agentnum__(astid, anum)
                                for channel in channels:
                                        self.__ami_execute__(astid, 'stopmonitor', channel)
                                        self.weblist['agents'][astid].keeplist[agent_id]['agentstats'].update({'Xivo-Agent-Status-Recorded' : False})
                                        log.info('stopped monitor on %s %s (agent %s)' % (astid, channel, anum))
                                        tosend = { 'class' : 'agentrecord',
                                                   'astid' : astid,
                                                   'agentid' : agent_id,
                                                   'status' : 'stopped' }
                                        self.__send_msg_to_cti_clients__(self.__cjson_encode__(tosend), astid,
                                                                         self.weblist['agents'][astid].keeplist[agent_id].get('context'))
                        elif subcommand == 'listen':
                                if astid == userinfo.get('astid'):
                                        agent_id = self.weblist['agents'][astid].reverse_index.get(anum)
                                        channels = self.__find_channel_by_agentnum__(astid, anum)
                                        for channel in channels:
                                                aid = self.__ami_execute__(userinfo.get('astid'),
                                                                           'origapplication',
                                                                           'ChanSpy',
                                                                           '%s|q' % channel,
                                                                           'Local',
                                                                           userinfo.get('phonenum'),
                                                                           userinfo.get('context'))
                                                log.info('started listening on %s %s (agent %s) aid = %s' % (astid, channel, anum, aid))
                                                self.origapplication[astid][aid] = { 'origapplication' : 'ChanSpy',
                                                                                     'origapplication-data' : { 'spied-channel' : channel,
                                                                                                                'spied-agentid' : agent_id } }
                        elif subcommand == 'stoplisten':
                                agent_id = self.weblist['agents'][astid].reverse_index.get(anum)
                                channels = self.__find_channel_by_agentnum__(astid, anum)
                                for channel in channels:
                                        for uid, vv in self.uniqueids[astid].iteritems():
                                                if 'origapplication' in vv and vv['origapplication'] == 'ChanSpy':
                                                        if channel == vv['origapplication-data']['spied-channel']:
                                                                self.__ami_execute__(astid, 'hangup', vv['channel'])
                                        log.info('stopped listening on %s %s (agent %s)' % (astid, channel, anum))
                                        
                        elif subcommand == 'getfilelist':
                                lst = os.listdir(MONITORDIR)
                                monitoredfiles = []
                                for monitoredfile in lst:
                                        if monitoredfile.startswith('cti-agent-') and monitoredfile.endswith('%s.wav' % anum):
                                                monitoredfiles.append(monitoredfile)
                                monitoredfiles.sort()
                                tosend = { 'class' : 'filelist',
                                           'filelist' : monitoredfiles }
                                return self.__cjson_encode__(tosend)
                        
                        elif subcommand == 'getfile':
                                filename = '%s/%s' % (MONITORDIR, commandargs[3])
                                fileid = ''.join(random.sample(__alphanums__, 10))
                                self.filestodownload[fileid] = filename
                                tosend = { 'class' : 'faxsend',
                                           'tdirection' : 'download',
                                           'fileid' : fileid }
                                return self.__cjson_encode__(tosend)
                        
                elif subcommand == 'transfer':
                        astid = commandargs[1]
                        agentid = commandargs[2]
                        qname = commandargs[3]
                        for chan, vchan in self.weblist['queues'][astid].keeplist[qname]['channels'].iteritems():
                                uinfos = self.__find_userinfos_by_agentnum__(astid, agentid)
                                if uinfos:
                                        self.__ami_execute__(astid, 'transfer',
                                                             chan,
                                                             uinfos[0].get('agentphonenum'), uinfos[0].get('context'))
                                        break
                elif subcommand == 'lists':
                        pass
                else:
                        log.warning('__agent__ : unknown subcommand %s' % subcommand)
                return
        
        
        def __login_agent__(self, uinfo, agphonenum = None):
                if uinfo is None:
                    return
                astid = uinfo.get('astid')
                # if phonenum is None:
                # phonenum = userinfo.get('phonenum')
                
                # override the previously set value
                if agphonenum:
                    uinfo['agentphonenum'] = agphonenum
                    
                if 'agentid' in uinfo and 'agentphonenum' in uinfo and astid is not None:
                    agentnum = self.__agentnum__(uinfo)
                    agentphonenum = uinfo['agentphonenum']
                    if agentnum and agentphonenum:
                        agprops = self.weblist['agents'][astid].keeplist[uinfo['agentid']]
                        wrapuptime = agprops.get('wrapuptime')
                        self.__ami_execute__(astid, 'agentcallbacklogin',
                                             agentnum, agentphonenum,
                                             agprops.get('context'), agprops.get('ackcall'))
                        # self.__agent__(uinfo, ['unpause_all'])
                        # chan_agent.c:2318 callback_deprecated: AgentCallbackLogin is deprecated and will be removed in a future release.
                        # chan_agent.c:2319 callback_deprecated: See doc/queues-with-callback-members.txt for an example of how to achieve
                        # chan_agent.c:2320 callback_deprecated: the same functionality using only dialplan logic.
                        self.__ami_execute__(astid, 'setvar', 'AGENTBYCALLERID_%s' % agentphonenum, agentnum)
                        self.__fill_user_ctilog__(uinfo, 'agent_login')
                return
            
        def __logout_agent__(self, uinfo):
                if uinfo is None:
                    return
                astid = uinfo.get('astid')
                if 'agentid' in uinfo and astid is not None:
                    agentnum = self.__agentnum__(uinfo)
                    if 'agentphonenum' in uinfo:
                        agentphonenum = uinfo['agentphonenum']
                        if agentphonenum:
                            self.__ami_execute__(astid, 'setvar', 'AGENTBYCALLERID_%s' % agentphonenum, '')
                            # del uinfo['agentphonenum']
                    if agentnum:
                        ## self.__agent__(uinfo, ['pause_all'])
                        self.__ami_execute__(astid, 'agentlogoff', agentnum)
                        agentlogoff_retry = threading.Timer(0.1, self.__callback_timer__, ('agentlogoff',))
                        agentlogoff_retry.start()
                        self.timerthreads_agentlogoff_retry[agentlogoff_retry] = {'astid' : astid,
                                                                                  'agentnumber' : agentnum}
                        self.__fill_user_ctilog__(uinfo, 'agent_logout')
                return
            
        def logout_all_agents(self):
                for userinfo in self.ulist_ng.keeplist.itervalues():
                        self.__logout_agent__(userinfo)
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
                                        log.info('(%2d h %2d min) => %s' % (thour, tmin, regactions[2]))
                                        if regactions[2] == 'logoutagents':
                                                self.logout_all_agents()
                                else:
                                        log.info('(%2d h %2d min) => no action' % (thour, tmin))
                except Exception:
                        log.exception('(regular update)')
                return
        
        def __getlist__(self, userinfo, ccomm, context=None):
                capaid = userinfo.get('capaid')
                ucapa = self.capas[capaid].all()
                if ccomm == 'users':
                        # XXX define capas ?
                        fullstat = []
                        for uinfo in self.ulist_ng.keeplist.itervalues():
                            if self.__check_astid_context__(userinfo, uinfo.get('astid')):
                                duinfo = '%s/%s' % (uinfo.get('astid'), uinfo.get('xivo_userid'))
                                icapaid = uinfo.get('capaid')
                                statedetails = {'color' : 'grey',
                                                'longname' : PRESENCE_UNKNOWN,
                                                'stateid' : 'xivo_unknown'}
                                if icapaid and icapaid in self.capas:
                                        presenceid = self.capas[uinfo.get('capaid')].presenceid
                                        if presenceid in self.presence_sections:
                                                if uinfo.get('state') in self.presence_sections[presenceid].displaydetails:
                                                        statedetails = self.presence_sections[presenceid].displaydetails[uinfo.get('state')]
                                                elif uinfo.get('state') != 'xivo_unknown':
                                                        log.warning('%s : %s not in details for %s'
                                                                    % (duinfo, uinfo.get('state'), presenceid))
                                        else:
                                                if presenceid != 'none':
                                                        log.warning('%s : presenceid=%s not in presence_sections'
                                                                    % (duinfo, presenceid))
                                elif icapaid:
                                        log.warning('%s : capaid=%s not in capas'
                                                    % (duinfo, icapaid))
                                
                                senduinfo = {}
                                for kw in ['astid', 'user', 'company', 'fullname', 'simultcalls',
                                           'context', 'phonenum', 'mobilenum', 'agentid',
                                           'techlist', 'mwi', 'xivo_userid']:
                                        senduinfo[kw] = uinfo.get(kw)
                                senduinfo['statedetails'] = statedetails
                                senduinfo['agentnumber'] = self.__agentnum__(uinfo)
                                senduinfo['voicemailnum'] = self.__voicemailnum__(uinfo)
                                if context == None or context == senduinfo['context']:
                                        fullstat.append(senduinfo)
                                
                elif ccomm == 'phones':
                        fullstat = {}
                        if self.capas[capaid].match_funcs(ucapa, 'dial'):
                                fullstat = self.__parting_astid_context__(userinfo, 'phones')
                elif ccomm == 'agents':
                        fullstat = {}
                        if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                fullstat = self.__parting_astid_context__(userinfo, 'agents')
                elif ccomm == 'queues':
                        fullstat = {}
                        if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                fullstat = self.__parting_astid_context__(userinfo, 'queues')
                elif ccomm == 'groups':
                        fullstat = {}
                        if self.capas[capaid].match_funcs(ucapa, 'agents'):
                                fullstat = self.__parting_astid_context__(userinfo, 'groups')
                tosend = { 'class' : ccomm,
                           'function' : 'sendlist',
                           'payload' : fullstat }
                tsencoded = self.__cjson_encode__(tosend)
                log.info('__getlist__ : sendlist size is %d for %s' % (len(tsencoded), ccomm))
                return tsencoded
        
        def __parting_astid_context__(self, userinfo, listname):
                fullstat = {}
                try:
                        for astid, xlist in self.weblist[listname].iteritems():
                                fullstat[astid] = {}
                                for xid, xvalue in xlist.keeplist.iteritems():
                                        context = xvalue.get('context')
                                        if self.__check_astid_context__(userinfo, astid, context):
                                                fullstat[astid][xid] = xvalue
                except Exception:
                        log.exception('(__parting_astid_context__) %s/%s %s'
                                      % (userinfo.get('astid'), userinfo.get('xivo_userid'), listname))
                return fullstat
        
        # \brief Builds the features_get reply.
        def __build_features_get__(self, userid):
                userinfo = self.ulist_ng.keeplist[userid]
                user = userinfo.get('user')
                astid = userinfo.get('astid')
                company = userinfo.get('company')
                context = userinfo.get('context')
                srcnum = userinfo.get('phonenum')
                phoneid = userinfo.get('phoneid')
                repstr = {}
                
                cursor = self.configs[astid].userfeatures_db_conn.cursor()
                params = [srcnum, phoneid, context]
                query = 'SELECT ${columns} FROM userfeatures WHERE number = %s AND name = %s AND context = %s'
                
                for key in ['enablevoicemail', 'callrecord', 'callfilter', 'enablednd']:
                        try:
                                columns = (key,)
                                cursor.query(query, columns, params)
                                results = cursor.fetchall()
                                if len(results) > 0:
                                        repstr[key] = {'enabled' : bool(results[0][0])}
                                else:
                                        log.warning('%s __build_features_get__ : nobody matches (%s %s)'
                                                    % (astid, key, params))
                        except Exception:
                                log.exception('features_get(bool) id=%s key=%s' % (userid, key))
                for key in ['unc', 'busy', 'rna']:
                        try:
                                columns = ('enable' + key,)
                                cursor.query(query, columns, params)
                                resenable = cursor.fetchall()

                                columns = ('dest' + key,)
                                cursor.query(query, columns, params)
                                resdest = cursor.fetchall()

                                if len(resenable) > 0 and len(resdest) > 0:
                                        repstr[key] = { 'enabled' : bool(resenable[0][0]),
                                                        'number' : resdest[0][0] }
                                else:
                                        log.warning('%s __build_features_get__ : nobody matches (%s %s)'
                                                    % (astid, key, params))
                        except Exception:
                                log.exception('features_get(str) id=%s key=%s' % (userid, key))
                tosend = { 'class' : 'features',
                           'function' : 'get',
                           'userid' : userid,
                           'payload' : repstr }
                return self.__cjson_encode__(tosend)
        
        
        # \brief Builds the features_put reply.
        def __build_features_put__(self, userid, key, value, destination = None):
                userinfo = self.ulist_ng.keeplist[userid]
                user = userinfo.get('user')
                astid = userinfo.get('astid')
                company = userinfo.get('company')
                context = userinfo.get('context')
                srcnum = userinfo.get('phonenum')
                phoneid = userinfo.get('phoneid')
                try:
                        query = 'UPDATE userfeatures SET ' + key + ' = %s WHERE number = %s AND name = %s AND context = %s'
                        params = [value, srcnum, phoneid, context]
                        cursor = self.configs[astid].userfeatures_db_conn.cursor()
                        cursor.query(query, parameters = params)
                        self.configs[astid].userfeatures_db_conn.commit()
                        repstr = { key : { 'enabled' : bool(int(value)) } }
                        if destination:
                                query = 'UPDATE userfeatures SET ' + 'dest' + key[6:] + ' = %s WHERE number = %s AND name = %s AND context = %s'
                                params = [destination, srcnum, phoneid, context]
                                cursor = self.configs[astid].userfeatures_db_conn.cursor()
                                cursor.query(query, parameters = params)
                                self.configs[astid].userfeatures_db_conn.commit()
                                repstr[key]['number'] = destination
                        # log.info('__build_features_put__ : %s : %s => %s' % (params, key, value))
                except Exception:
                        repstr = {}
                        log.exception('features_put id=%s %s %s' % (userid, key, value))
                tosend = { 'class' : 'features',
                           'function' : 'put',
                           'userid' : userid,
                           'payload' : repstr }
                return self.__cjson_encode__(tosend)
        
        
        # \brief Originates / transfers.
        def __originate_or_transfer__(self, commname, userinfo, src, dst):
                log.info('%s %s %s %s' % (commname, userinfo, src, dst))
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
                                        srcuinfo = self.ulist_ng.keeplist.get(whosrc)
                                if srcuinfo is not None:
                                        astid_src = srcuinfo.get('astid')
                                        context_src = srcuinfo.get('context')
                                        techdetails = srcuinfo.get('techlist')[0]
                                        proto_src = techdetails.split('.')[0]
                                        # XXXX 'local' might break the XIVO_ORIGSRCNUM mechanism (trick for thomson)
                                        phonenum_src = techdetails.split('.')[2]
                                        ### srcuinfo.get('phonenum')
                                        # if termlist empty + agentphonenum not empty => call this one
                                        cidname_src = srcuinfo.get('fullname')
                        # 'agent:', 'queue:', 'group:', 'meetme:' ?
                        elif typesrc == 'ext':
                                context_src = userinfo['context']
                                astid_src = userinfo['astid']
                                proto_src = 'local'
                                phonenum_src = whosrc
                                cidname_src = whosrc
                        else:
                                log.warning('unknown typesrc <%s>' % typesrc)
                                return
                                
                        # dst
                        if typedst == 'ext':
                                context_dst = context_src
                                # this string will appear on the caller's phone, before he calls someone
                                # for internal calls, one could solve the name of the called person,
                                # but it could be misleading with an incoming call from the given person
                                cidname_dst = whodst
                                exten_dst = whodst
                        # 'agent:', 'queue:', 'group:', 'meetme:' ?
                        elif typedst == 'user':
                                if whodst == 'special:me':
                                        dstuinfo = userinfo
                                else:
                                        dstuinfo = self.ulist_ng.keeplist[whodst]
                                if dstuinfo is not None:
                                        astid_dst = dstuinfo.get('astid')
                                        exten_dst = dstuinfo.get('phonenum')
                                        cidname_dst = dstuinfo.get('fullname')
                                        context_dst = dstuinfo.get('context')
                        else:
                                log.warning('unknown typedst <%s>' % typedst)
                                return
                                
                        if typesrc == typedst and typedst == 'ext' and len(whosrc) > 8 and len(whodst) > 8:
                                log.warning('ORIGINATE : Trying to call two external phone numbers (%s and %s). Canceling' % (whosrc, whodst))
                                return

                        try:
                                if len(exten_dst) > 0:
                                        ret = self.__ami_execute__(astid_src, AMI_ORIGINATE,
                                                                   proto_src, phonenum_src, cidname_src,
                                                                   exten_dst, cidname_dst,  context_dst,
                                                                   {'XIVO_USERID' : userinfo.get('xivo_userid')})
                        except Exception:
                                log.exception('unable to originate')
                                
                elif commname in ['transfer', 'atxfer']:
                        [typesrc, whosrc] = srcsplit
                        [typedst, whodst] = dstsplit
                        
                        if typesrc == 'chan':
                                if whosrc.startswith('special:me:'):
                                        srcuinfo = userinfo
                                        chan_src = whosrc[len('special:me:'):]
                                else:
                                        [uid, chan_src] = whosrc.split(':')
                                        srcuinfo = self.ulist_ng.keeplist[uid]
                                if srcuinfo is not None:
                                        astid_src = srcuinfo.get('astid')
                                        context_src = srcuinfo.get('context')
                                        proto_src = 'local'
                                        phonenum_src = srcuinfo.get('phonenum')
                                        # if termlist empty + agentphonenum not empty => call this one
                                        cidname_src = srcuinfo.get('fullname')
                        else:
                                log.warning('unknown typesrc %s for %s' % (typesrc, commname))
                        
                        if typedst == 'ext':
                                exten_dst = whodst
                                if whodst == 'special:parkthecall':
                                        for uid, vuid in self.uniqueids[astid_src].iteritems():
                                                if 'dial' in vuid and vuid['dial'] == chan_src and 'channel' in vuid:
                                                        nchan = vuid['channel']
                                                if 'link' in vuid and vuid['link'] == chan_src and 'channel' in vuid:
                                                        nchan = vuid['channel']
                                        chan_park = nchan
                                        #exten_dst = '700'   # cheat code !
                                        #chan_src = nchan    # cheat code !
                        elif typedst == 'user':
                                if whodst == 'special:me':
                                        dstuinfo = userinfo
                                else:
                                        dstuinfo = self.ulist_ng.keeplist[whodst]
                                if dstuinfo is not None:
                                        astid_dst = dstuinfo.get('astid')
                                        exten_dst = dstuinfo.get('phonenum')
                                        cidname_dst = dstuinfo.get('fullname')
                                        context_dst = dstuinfo.get('context')
                        elif typedst == 'voicemail':
                                if whodst == 'special:me':
                                        dstuinfo = userinfo
                                else:
                                        dstuinfo = self.ulist_ng.keeplist[whodst]
                                        
                                if dstuinfo.get('voicemailid'):
                                        voicemail_id = dstuinfo['voicemailid']
                                        self.__ami_execute__(astid_src, 'setvar', 'XIVO_VMBOXID', voicemail_id, chan_src)
                                        exten_dst = 's'
                                        context_src = 'macro-voicemail'
                                else:
                                        log.warning('no voicemail allowed or defined for %s' % dstuinfo)
                        else:
                                log.warning('unknown typedst %s for %s' % (typedst, commname))
                                
                        # print astid_src, commname, chan_src, exten_dst, context_src
                        ret = False
                        try:
                                # cheat code !
                                if whodst == 'special:parkthecall':
                                        ret = self.__ami_execute__(astid_src, 'park', chan_park, chan_src)
                                else:
                                        if exten_dst:
                                                ret = self.__ami_execute__(astid_src, commname,
                                                                           chan_src,
                                                                           exten_dst, context_src)
                        except Exception:
                                log.exception('unable to %s' % commname)
                else:
                        log.warning('unallowed command %s' % commargs)
                return
        
        # \brief Hangs up.
        def __hangup__(self, userinfo, source, peer_hangup):
                [typesrc, whosrc] = source.split(':', 1)
                if typesrc != 'chan':
                        return
                [userid, channel] = whosrc.split(':', 1)
                if not channel:
                        log.warning('channel undefined for %s' % source)
                        return
                uinfo = self.ulist_ng.keeplist.get(userid)
                astid = uinfo.get('astid')
                if astid in self.configs:
                        channel_peer = ''
                        log.info('%s : a HANGUP is attempted for %s on channel %s' % (astid, uinfo.get('fullname'), channel))
                        phoneid = uinfo.get('techlist')[0]
                        if phoneid in self.weblist['phones'][astid].keeplist:
                                phonedetails = self.weblist['phones'][astid].keeplist[phoneid]
                                for c, v in phonedetails['comms'].iteritems():
                                        if v['thischannel'] == channel:
                                                if peer_hangup and 'peerchannel' in v:
                                                        channel_peer = v['peerchannel']
                        ret = self.__ami_execute__(astid, 'hangup', channel, channel_peer)
                        # ret to be checked on other replies
                else:
                        ret_message = 'hangup KO : no such asterisk id <%s>' % astid
                return
        
        
        # \brief Function that fetches the call history from a database
        # \param cfg the asterisk's config
        # \param techno technology (SIP/IAX/ZAP/etc...)
        # \param phoneid phone id
        # \param nlines the number of lines to fetch for the given phone
        # \param kind kind of list (ingoing, outgoing, missed calls)
        def __update_history_call__(self, cfg, techno, phoneid, nlines, kind, morerecentthan=None):
                results = []
                if cfg.cdr_db_conn is None:
                        log.warning('%s : no CDR uri defined for this asterisk - see cdr_db_uri parameter' % cfg.astid)
                else:
                        try:
                                cursor = cfg.cdr_db_conn.cursor()
                                columns = ('calldate', 'clid', 'src', 'dst', 'dcontext', 'channel', 'dstchannel',
                                           'lastapp', 'lastdata', 'duration', 'billsec', 'disposition', 'amaflags',
                                           'accountcode', 'uniqueid', 'userfield')
                                likestring = '%s/%s-%%' %(techno, phoneid)
                                orderbycalldate = "ORDER BY calldate DESC LIMIT %s" % nlines
                                datecond = ''
                                if morerecentthan is not None:
                                    log.debug('morerecentthan = %s' % (morerecentthan))
                                    datecond = " AND calldate > '%s' " % (morerecentthan)
                                
                                if kind == "0": # outgoing calls (all)
                                        cursor.query("SELECT ${columns} FROM cdr WHERE channel LIKE %s " + datecond + orderbycalldate,
                                                     columns,
                                                     (likestring,))
                                elif kind == "1": # incoming calls (answered)
                                        cursor.query("SELECT ${columns} FROM cdr WHERE disposition='ANSWERED' AND dstchannel LIKE %s " + datecond + orderbycalldate,
                                                     columns,
                                                     (likestring,))
                                else: # missed calls (received but not answered)
                                        cursor.query("SELECT ${columns} FROM cdr WHERE disposition!='ANSWERED' AND dstchannel LIKE %s " + datecond + orderbycalldate,
                                                     columns,
                                                     (likestring,))
                                results = cursor.fetchall()
                        except Exception:
                                log.exception('%s : Connection to DataBase failed in History request' % cfg.astid)
                return results
        
        
        def __update_availstate__(self, userinfo, state):
                company = userinfo['company']
                username = userinfo['user']
                xivo_userid = userinfo['xivo_userid']
                astid = userinfo.get('astid')
                context = userinfo.get('context')
                capaid = userinfo.get('capaid')
                
                if userinfo['state'] == 'xivo_unknown' and state in ['onlineincoming', 'onlineoutgoing', 'postcall']:
                        # we forbid 'asterisk-related' presence events to change the status of unlogged users
                        return None
                if not state:
                        return None
                
                if capaid:
                        if state == 'xivo_unknown' or state in self.presence_sections[self.capas[capaid].presenceid].getstates():
                                userinfo['state'] = state
                        else:
                                log.warning('(user %s) : state <%s> is not an allowed one => keeping current <%s>'
                                            % (username, state, userinfo['state']))
                else:
                        userinfo['state'] = 'xivo_unknown'
                        
                cstatus = {}
                statedetails = {'color' : 'grey',
                                'longname' : PRESENCE_UNKNOWN,
                                'stateid' : 'xivo_unknown'}
                
                if capaid and capaid in self.capas:
                        allowed = {}
                        presenceid = self.capas[capaid].presenceid
                        if presenceid in self.presence_sections:
                                allowed = self.presence_sections[presenceid].allowed(userinfo['state'])
                                if userinfo['state'] in self.presence_sections[presenceid].displaydetails:
                                        statedetails = self.presence_sections[presenceid].displaydetails[userinfo['state']]
                        wpid = self.capas[capaid].watchedpresenceid
                        if wpid in self.presence_sections:
                                cstatus = self.presence_sections[wpid].countstatus(self.__counts__(astid, context, wpid))
                                
                        tosend = { 'class' : 'presence',
                                   'company' : company,
                                   'userid' : username,
                                   'xivo_userid' : xivo_userid,
                                   'astid' : astid,
                                   'capapresence' : { 'state' : statedetails,
                                                      'allowed' : allowed },
                                   'presencecounter' : cstatus
                                   }
                        self.__send_msg_to_cti_client__(userinfo, self.__cjson_encode__(tosend))
                
                tosend = { 'class' : 'presence',
                           'company' : company,
                           'userid' : username,
                           'xivo_userid' : xivo_userid,
                           'astid' : astid,
                           'capapresence' : { 'state' : statedetails },
                           'presencecounter' : cstatus
                           }
                self.__send_msg_to_cti_clients_except__([userinfo], self.__cjson_encode__(tosend), astid, context)
                return None
        
        
        # \brief Builds the full list of customers in order to send them to the requesting client.
        # This should be done after a command called "customers".
        # \return a string containing the full customers list
        # \sa manage_tcp_connection
        def __build_customers__(self, ctx, searchpattern):
                fulllist = []
                if ctx in self.ctxlist.ctxlist:
                        dirlist = self.ctxlist.ctxlist[ctx]
                        for dirsec in dirlist:
                                dirdef = self.ctxlist.dirlist[dirsec]
                                try:
                                        y = self.__build_customers_bydirdef__(dirsec, searchpattern, dirdef, False)
                                        fulllist.extend(y)
                                except Exception:
                                        log.exception('__build_customers__ (%s)' % dirsec)
                else:
                        log.warning('there has been no section defined for context %s : can not proceed directory search' % ctx)
                        
                mylines = []
                for itemdir in fulllist:
                        myitems = []
                        for k in self.ctxlist.display_items[ctx]:
                                [title, type, defaultval, format] = self.ctxlist.displays[ctx][k].split('|')
                                basestr = format
                                for k, v in itemdir.iteritems():
                                        try:
                                                basestr = basestr.replace('{%s}' % k, v.decode('utf8'))
                                        except UnicodeEncodeError:
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
                           'headers' : self.ctxlist.display_header[ctx],
                           'resultlist' : mylines }
                return self.__cjson_encode__(tosend)
        
        def __build_customers_bydirdef__(self, dirname, searchpattern, z, reversedir):
                fullstatlist = []
                
                if searchpattern == '':
                    return []
                
                dbkind = z.uri.split(':')[0]
                if dbkind in ['ldap', 'ldaps']:
                        selectline = []
                        for fname in z.match_direct:
                            if searchpattern == '*':
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
                                        results = ldapid.getldap('(|%s)' % ''.join(selectline),
                                                                 z.match_direct)
                                if results is not None:
                                        for result in results:
                                                futureline = {'xivo-dir' : z.name}
                                                for keyw, dbkeys in z.fkeys.iteritems():
                                                        for dbkey in dbkeys:
                                                                if dbkey in result[1]:
                                                                        futureline[keyw] = result[1][dbkey][0]
                                                fullstatlist.append(futureline)
                        except Exception:
                                log.exception('ldaprequest (directory)')
                
                elif dbkind == 'file':
                        f = urllib.urlopen(z.uri)
                        n = 0
                        if reversedir:
                            matchkeywords = z.match_reverse
                        else:
                            matchkeywords = z.match_direct
                        for line in f:
                                if n == 0:
                                        header = line
                                        headerfields = header.strip().split(z.delimiter)
                                        revindex = []
                                        for mr in matchkeywords:
                                            if mr in headerfields:
                                                revindex.append(headerfields.index(mr))
                                else:
                                        ll = line.strip()
                                        t = ll.split(z.delimiter)
                                        matchme = False
                                        for ri in revindex:
                                            if t[ri].lower().find(searchpattern.lower()) >= 0:
                                                matchme = True
                                        if matchme:
                                            # XXX problem when badly set delimiter + index()
                                            futureline = {'xivo-dir' : z.name}
                                            for keyw, dbkeys in z.fkeys.iteritems():
                                                for dbkey in dbkeys:
                                                    idx = headerfields.index(dbkey)
                                                    futureline[keyw] = t[idx]
                                            fullstatlist.append(futureline)
                                n += 1
                        if n == 0:
                            log.warning('WARNING : %s is empty' % z.uri)
                        elif n == 1:
                            log.warning('WARNING : %s contains only one line (the header one)' % z.uri)
                            
                elif dbkind == 'phonebook':
                    if reversedir:
                        matchkeywords = z.match_reverse
                    else:
                        matchkeywords = z.match_direct
                    for iastid in self.weblist['phonebook'].keys():
                        for k, v in self.weblist['phonebook'][iastid].keeplist.iteritems():
                            matchme = False
                            for tmatch in matchkeywords:
                                if v.has_key(tmatch):
                                    if reversedir:
                                        if v[tmatch].lstrip('0') == searchpattern.lstrip('0'):
                                            matchme = True
                                    else:
                                        if searchpattern == '*' or v[tmatch].lower().find(searchpattern.lower()) >= 0:
                                            matchme = True
                            if matchme:
                                futureline = {'xivo-dir' : z.name}
                                for keyw, dbkeys in z.fkeys.iteritems():
                                    for dbkey in dbkeys:
                                        if dbkey in v.keys():
                                            futureline[keyw] = v[dbkey]
                                fullstatlist.append(futureline)
                                
                elif dbkind == 'http':
                        if not reversedir:
                                fulluri = z.uri
                                # add an ending slash if needed
                                if fulluri[8:].find('/') == -1:
                                    fulluri += '/'
                                fulluri += '?' + '&'.join([key + '=' + urllib.quote(searchpattern.encode('utf-8')) for key in z.match_direct])
                                n = 0
                                try:
                                    f = urllib.urlopen(fulluri)
                                    # use f.info() to detect charset
                                    charset = 'utf-8'
                                    s = f.info().getheader('Content-Type')
                                    k = s.lower().find('charset=')
                                    if k >= 0:
                                        charset = s[k:].split(' ')[0].split('=')[1]                                    
                                    for line in f:
                                        if n == 0:
                                            header = line
                                            headerfields = header.strip().split(z.delimiter)
                                        else:
                                            ll = line.strip()
                                            if isinstance(ll, str): # dont try to decode unicode string.
                                                ll = ll.decode(charset)
                                            t = ll.split(z.delimiter)
                                            futureline = {'xivo-dir' : z.name}
                                            # XXX problem when badly set delimiter + index()
                                            for keyw, dbkeys in z.fkeys.iteritems():
                                                for dbkey in dbkeys:
                                                    idx = headerfields.index(dbkey)
                                                    futureline[keyw] = t[idx]
                                            fullstatlist.append(futureline)
                                        n += 1
                                    f.close()
                                except Exception:
                                        log.exception('__build_customers_bydirdef__ (http) %s' % fulluri)
                                if n == 0:
                                        log.warning('WARNING : %s is empty' % z.uri)
                                # we don't warn about "only one line" here since the filter has probably already been applied
                        else:
                                fulluri = z.uri
                                # add an ending slash if needed
                                if fulluri[8:].find('/') == -1:
                                    fulluri += '/'
                                fulluri += '?' + '&'.join([key + '=' + urllib.quote(searchpattern) for key in z.match_reverse])
                                f = urllib.urlopen(fulluri)
                                # TODO : use f.info() to detect charset
                                fsl = f.read().strip()
                                if fsl:
                                    fullstatlist = [{'xivo-dir' : z.name, 'db-fullname' : fsl}]
                                else:
                                    fullstatlist = []
                                    
                elif dbkind in ['sqlite', 'mysql']:
                        if searchpattern == '*':
                            whereline = ''
                        else:
                            # prevent SQL injection and make use of '*' wildcard possible
                            esc_searchpattern = searchpattern.replace("'", "\\'").replace('%', '\\%').replace('*', '%')
                            wl = ["%s LIKE '%%%s%%'" % (fname, esc_searchpattern) for fname in z.match_direct]
                            whereline = 'WHERE ' + ' OR '.join(wl)
                            
                        results = []
                        try:
                            conn = anysql.connect_by_uri(str(z.uri))
                            cursor = conn.cursor()
                            sqlrequest = 'SELECT ${columns} FROM %s %s' % (z.sqltable, whereline)
                            cursor.query(sqlrequest,
                                         tuple(z.match_direct),
                                         None)
                            results = cursor.fetchall()
                            conn.close()
                        except Exception:
                            log.exception('sqlrequest for %s' % z.uri)
                            
                        for result in results:
                            futureline = {'xivo-dir' : z.name}
                            for keyw, dbkeys in z.fkeys.iteritems():
                                for dbkey in dbkeys:
                                    if dbkey in z.match_direct:
                                        n = z.match_direct.index(dbkey)
                                        futureline[keyw] = result[n]
                            fullstatlist.append(futureline)
                else:
                    log.warning('wrong or no database method defined (%s) - please fill the uri field of the directory <%s> definition'
                                % (dbkind, dirname))
                
                if reversedir:
                    display_reverse = z.display_reverse
                    if fullstatlist:
                        for k, v in fullstatlist[0].iteritems():
                            if isinstance(v, unicode):
                                display_reverse = display_reverse.replace('{%s}' % k, v)
                            elif isinstance(v, str):
                                # decoding utf8 data as we know the DB is storing utf8 so some bug may lead this data to come here still utf8 encoded
                                # in the future, this code could be removed, once we are sure encoding is properly handled "up there" (in sqlite client)
                                display_reverse = display_reverse.replace('{%s}' % k, v.decode('utf8'))
                            else:
                                log.warning('__build_customers_bydirdef__ %s is neither unicode nor str' % k)
                        e = fullstatlist[0]
                        e.update({'dbr-display' : display_reverse})
                return fullstatlist
        
        def __counts__(self, astid, context, presenceid):
                # input : presenceid, i.e. 'presence-xivo', 'presence-agent', ...
                # output : number of userinfo's in each presenceid's presence state, like :
                # {'available': 0, 'onlineoutgoing': 0, 'fastpickup': 0, 'postcall': 0, 'backoffice': 0, 'onlineincoming': 0}
                
                counts = {}
                if presenceid not in self.presence_sections:
                        return counts
                for istate in self.presence_sections[presenceid].getstates():
                        counts[istate] = 0
                for iuserinfo in self.ulist_ng.keeplist.itervalues():
                        rightastidcontext = (astid == iuserinfo.get('astid') and context == iuserinfo.get('context'))
                        if rightastidcontext and iuserinfo.has_key('capaid'):
                                capaid = iuserinfo['capaid']
                                if capaid in self.capas:
                                        if self.capas[capaid].presenceid == presenceid:
                                                if iuserinfo['state'] in self.presence_sections[presenceid].getstates():
                                                        counts[iuserinfo['state']] += 1
                return counts
        
        def handle_fagi(self, astid, fastagi):
                """
                Events coming from Fast AGI.
                """
                # check capas !
                try:
                        function = fastagi.env['agi_network_script']
                        uniqueid = fastagi.get_variable('UNIQUEID')
                        channel = fastagi.get_variable('CHANNEL')
                        
                        context = fastagi.get_variable('XIVO_REAL_CONTEXT')
                        log.info('handle_fagi %s : (%s) context=%s uid=%s chan=%s'
                                 % (astid, function, context, uniqueid, channel))
                except Exception:
                        log.exception('%s handle_fagi %s' % (astid, fastagi.env))
                        return
                
                if function == 'presence':
                        try:
                                if len(fastagi.args) > 0:
                                        presenceid = fastagi.args[0]
                                        aststatus = []
                                        for var, val in self.__counts__(astid, context, presenceid).iteritems():
                                                aststatus.append('%s:%d' % (var, val))
                                        fastagi.set_variable('XIVO_PRESENCE', ','.join(aststatus))
                        except Exception:
                                log.exception('handle_fagi %s %s : %s %s' % (astid, function, astid, fastagi.args))
                        return
                
                elif function == 'queuestatus':
                        try:
                                if len(fastagi.args) > 0:
                                        queuename = fastagi.args[0]
                                        if self.weblist['queues'][astid].hasqueue(queuename):
                                                qprops = self.weblist['queues'][astid].keeplist[queuename]['agents_in_queue']
                                                lst = []
                                                for ag, agc in qprops.iteritems():
                                                        sstatus = 'unknown'
                                                        status = agc.get('Status')
                                                        if status == '1':
                                                                if agc.get('Paused') == '1':
                                                                        sstatus = 'paused'
                                                                else:
                                                                        sstatus = 'available'
                                                        elif status == '3':
                                                                sstatus = 'busy'
                                                        elif status == '5':
                                                                sstatus = 'away'
                                                        lst.append('%s:%s' % (ag, sstatus))
                                                fastagi.set_variable('XIVO_QUEUESTATUS', ','.join(lst))
                                                fastagi.set_variable('XIVO_QUEUEID', self.weblist['queues'][astid].keeplist[queuename]['id'])
                        except Exception:
                                log.exception('handle_fagi %s %s : %s %s' % (astid, function, astid, fastagi.args))
                        return
                
                elif function == 'queueentries':
                        try:
                                if len(fastagi.args) > 0:
                                        queuename = fastagi.args[0]
                                        if self.weblist['queues'][astid].hasqueue(queuename):
                                                qentries = self.weblist['queues'][astid].keeplist[queuename]['channels']
                                                lst = []
                                                for chan, chanprops in qentries.iteritems():
                                                        lst.append('%s:%d' % (chan, int(round(time.time() - chanprops.get('entrytime')))))
                                                fastagi.set_variable('XIVO_QUEUEENTRIES', ','.join(lst))
                        except Exception:
                                log.exception('handle_fagi %s %s : %s %s' % (astid, function, astid, fastagi.args))
                        return
                
                elif function == 'queueholdtime':
                        try:
                                if len(fastagi.args) > 0:
                                        queuename = fastagi.args[0]
                                        if self.weblist['queues'][astid].hasqueue(queuename):
                                                fastagi.set_variable('XIVO_QUEUEHOLDTIME',
                                                                     self.weblist['queues'][astid].keeplist[queuename]['queuestats']['Holdtime'])
                                else:
                                        lst = []
                                        for queuename, qprops in self.weblist['queues'][astid].keeplist.iteritems():
                                                if 'Holdtime' in qprops['queuestats']:
                                                        lst.append('%s:%s' % (queuename, qprops['queuestats']['Holdtime']))
                                                else:
                                                        log.warning('handle_fagi %s %s : no Holdtime defined in queuestats for %s'
                                                                    % (astid, function, queuename))
                                        fastagi.set_variable('XIVO_QUEUEHOLDTIME', ','.join(lst))
                        except Exception:
                                log.exception('handle_fagi %s %s : %s' % (astid, function, fastagi.args))
                        return
                
                
                elif function == 'callerid_extend':
                    if 'agi_callington' in fastagi.env:
                        fastagi.set_variable('XIVO_SRCTON', fastagi.env['agi_callington'])
                    return
                
                elif function == 'callerid_forphones':
                    if self.uniqueids[astid].has_key(uniqueid):
                        uniqueiddefs = self.uniqueids[astid][uniqueid]
                        if uniqueiddefs.has_key('dialplan_data'):
                            dialplan_data = uniqueiddefs['dialplan_data']
                            calleridsolved = dialplan_data.get('dbr-display')
                        else:
                            log.warning('%s handle_fagi %s no dialplan_data received yet'
                                        % (astid, function))
                            calleridsolved = None
                    else:
                        log.warning('%s handle_fagi %s no such uniqueid received yet : %s %s'
                                    % (astid, function, uniqueid, channel))
                        calleridsolved = None
                        
                    calleridnum  = fastagi.env['agi_callerid']
                    calleridname = fastagi.env['agi_calleridname']
                    if calleridsolved:
                        td = '%s handle_fagi %s : calleridsolved="%s"' % (astid, function, calleridsolved)
                        log.info(td.encode('utf8'))
                        if calleridname in ['', 'unknown']:
                            calleridname = calleridsolved
                            
                    # to set according to os.getenv('LANG') or os.getenv('LANGUAGE') later on ?
                    if calleridnum in ['', 'unknown']:
                        calleridnum = CALLERID_UNKNOWN_NUM
                    if calleridname in ['', 'unknown']:
                        calleridname = calleridnum
                        
                    calleridtoset = '"%s"<%s>' % (calleridname, calleridnum)
                    td = 'handle_fagi %s :   the callerid will be set to %s' % (astid, calleridtoset.decode('utf8'))
                    log.info(td.encode('utf8'))
                    fastagi.set_callerid(calleridtoset)
                    return
                
                elif function == 'cti2dialplan':
                    if len(fastagi.args) > 2:
                        dp_varname = fastagi.args[0]
                        cti_varname = fastagi.args[1]
                    else:
                        log.warning('%s handle_fagi %s not enough arguments : %s'
                                    % (astid, function, fastagi.args))
                        return
                    if self.uniqueids[astid].has_key(uniqueid):
                        uniqueiddefs = self.uniqueids[astid][uniqueid]
                        if uniqueiddefs.has_key('dialplan_data'):
                            dialplan_data = uniqueiddefs['dialplan_data']
                            if dialplan_data.has_key(cti_varname):
                                fastagi.set_variable(dp_varname, dialplan_data[cti_varname])
                            else:
                                log.warning('%s handle_fagi %s no such variable %s in dialplan data'
                                            % (astid, function, cti_varname))
                                ## XXX fastagi.set_variable(empty)
                        else:
                            log.warning('%s handle_fagi %s no dialplan_data received yet'
                                        % (astid, function))
                            ## XXX fastagi.set_variable(not yet)
                    else:
                        log.warning('%s handle_fagi %s no such uniqueid received yet : %s %s'
                                    % (astid, function, uniqueid, channel))
                        ## XXX fastagi.set_variable(not yet (uniqueid))
                    return
                
                else:
                    log.warning('%s handle_fagi %s : unknown function' % (astid, function))
                return
        
        def __link_local_channels__(self, astid, chan1, uid1, chan2, uid2, clid1, clidn1, clid2, clidn2):
                if chan1.startswith('Local/') and not chan2.startswith('Local/'):
                    localchan = chan1
                    otherchan = (chan2, uid2, clid2, clidn2)
                elif not chan1.startswith('Local/') and chan2.startswith('Local/'):
                    otherchan = (chan1, uid1, clid1, clidn1)
                    localchan = chan2
                else:
                    return
                #log.debug('__link_local_channels__ 1=(%s %s %s %s) 2=(%s %s %s %s)' % (chan1, uid1, clid1, clidn1, chan2, uid2, clid2, clidn2))
                log.debug('__link_local_channels__ %s => %s' % (localchan, otherchan))
                if localchan in self.localchans[astid]:
                    log.debug('__link_local_channels__ %s already known : %s' % (localchan, self.localchans[astid][localchan]))
                    if self.localchans[astid][localchan][2] is None:
                        self.localchans[astid][localchan] = otherchan
                self.localchans[astid][localchan] = otherchan
                
        def __translate_local_channel_uid__(self, astid, chan, uid, clid=None, clidn=None):
                chan = chan.replace('<ZOMBIE>', '')
                if not chan.startswith('Local/'):# or chan.endswith('<ZOMBIE>'):
                    return (chan, uid, clid, clidn)
                t = chan.split(',')
                friendchan = '%s,%d' % (t[0], 3-int(t[1]))
                if self.localchans[astid].has_key(friendchan):
                    log.debug('__translate_local_channel_uid__ %s => %s' % ((chan, uid), self.localchans[astid][friendchan]))
                    #return self.localchans[astid][friendchan]
                    (chan, uid, _clid, _clidn) = self.localchans[astid][friendchan]
                    if _clid is not None:
                        clid = _clid
                    if _clidn is not None:
                        clidn = _clidn
                return (chan, uid, clid, clidn)
        
        def __remove_local_channel__(self, astid, chan):
                chan = chan.replace('<ZOMBIE>', '')
                if not chan.startswith('Local/'):
                    return
                if chan in self.localchans[astid]:
                    del self.localchans[astid][chan]
                    
        def __create_new_sheet__(self, astid, channel):
                """create new sheet object if needed"""
                if not self.sheetmanager.has_key(astid):
                    self.sheetmanager[astid] = SheetManager(astid)
                if not self.sheetmanager[astid].sheets.has_key(channel):
                    self.sheetmanager[astid].new_sheet(channel)
                
        def __handle_sheet_command__(self, userinfo, command):
                """process sheetcommands from users"""
                log.debug('__handle_sheet_command__ user=%s command=%s' % (userinfo, command))
                astid = userinfo.get('astid')
                username = userinfo.get('user')
                function = command.get('function')
                chan = command.get('channel')
                if not astid in self.sheetmanager or not self.sheetmanager[astid].has_sheet(chan):
                    log.warning('%s __handle_sheet_command__ sheet %s not found' % (astid, chan))
                    return
                # TODO check user rights
                if function=='addentry':
                    type = command.get('type')
                    data = command.get('data')
                    if command.has_key('text'):
                        type = 'text'
                        data = command.get('text')
                    self.sheetmanager[astid].addentry(chan, type, data)
                    tosend = { 'class': 'sheet',
                               'function': 'entryadded',
                               'astid': astid,
                               'channel': chan,
                               'entry': self.sheetmanager[astid].get_sheet(chan).entries[-1].todict()
                             }
                    # send the entry to all users watching the sheet
                    for user in self.sheetmanager[astid].get_sheet(chan).viewingusers:
                        self.__send_msg_to_cti_client__(self.ulist_ng.keeplist[user], self.__cjson_encode__(tosend))
                        
        def __update_sheet_user__(self, astid, newuinfo, channel):
                """update connected user and send sheet to the new user if needed"""
                if astid in self.sheetmanager and self.sheetmanager[astid].has_sheet(channel):
                    newuser = '%s/%s' % (newuinfo.get('astid'), newuinfo.get('xivo_userid'))
                    log.debug('%s __update_sheet_user__ %s from user %s to %s (%s)' % (astid, channel, self.sheetmanager[astid].get_sheet(channel).currentuser, newuser, newuinfo.get('fullname')))
                    if self.sheetmanager[astid].get_sheet(channel).currentuser == newuser:
                        # nothing to update !
                        return
                    #log.debug('%s __update_sheet_user__ %s from user %s to %s (%s)' % (astid, channel, self.sheetmanager[astid].get_sheet(channel).currentuser, newuser, newuinfo))
                    if self.sheetmanager[astid].get_sheet(channel).currentuser is not None:
                        olduinfo = self.ulist_ng.keeplist[self.sheetmanager[astid].get_sheet(channel).currentuser]
                        #log.debug('%s __update_sheet_user__ olduinfo=%s' % (astid, olduinfo))
                        tosend = { 'class': 'sheet',
                                   'function': 'looseownership',
                                   'astid': astid,
                                   'channel': channel,
                                 }
                        self.__send_msg_to_cti_client__(olduinfo, self.__cjson_encode__(tosend))
                    # faut-il tout d'abord envoyer la fiche a l'utilisateur ???
                    if newuser not in self.sheetmanager[astid].get_sheet(channel).viewingusers:
                        log.debug('%s __update_sheet_user_ forwarding sheet to new user : %s' % (astid, self.sheetmanager[astid].get_sheet(channel).sheet))
                        tosend = { 'class' : 'sheet',
                                   #'whom' : whoms,
                                   'function' : 'displaysheet',
                                   'astid': astid,
                                   'channel' : channel,
                                   'payload' : base64.b64encode(self.sheetmanager[astid].get_sheet(channel).sheet.encode('utf8'))
                                   }
                        jsonmsg = self.__cjson_encode__(tosend)
                        self.__send_msg_to_cti_client__(newuinfo, self.__cjson_encode__(tosend))
                    # update current user
                    self.sheetmanager[astid].update_currentuser(channel, newuser)
                    # on informe le nouveau user qu'il a la propriete de la fiche
                    tosend = { 'class': 'sheet',
                               'function': 'getownership',
                               'astid': astid,
                               'channel': channel,
                             }
                    self.__send_msg_to_cti_client__(newuinfo, self.__cjson_encode__(tosend))
                    # afficher les stuff
                    for entry in self.sheetmanager[astid].get_sheet(channel).entries:
                        tosend = { 'class': 'sheet',
                                   'function': 'entryadded',
                                   'astid': astid,
                                   'channel': channel,
                                   'entry': entry.todict()
                                 }
                        self.__send_msg_to_cti_client__(newuser, self.__cjson_encode__(tosend))
                        
        def __sheet_disconnect__(self, astid, channel):
                """close the sheet if it is open on a user screen"""
                if self.sheetmanager[astid].get_sheet(channel).currentuser is not None:
                    olduinfo = self.ulist_ng.keeplist[self.sheetmanager[astid].get_sheet(channel).currentuser]
                    log.debug('%s __sheet_disconnect__ olduser=%s' % (astid, self.sheetmanager[astid].get_sheet(channel).currentuser))
                    tosend = { 'class': 'sheet',
                               'function': 'looseownership',
                               'astid': astid,
                               'channel': channel,
                             }
                    self.__send_msg_to_cti_client__(olduinfo, self.__cjson_encode__(tosend))
                self.sheetmanager[astid].del_sheet(channel)
                
xivo_commandsets.CommandClasses['xivocti'] = XivoCTICommand

