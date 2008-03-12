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
import string
import time
import xivo_commandsets
import xivo_ldap
from xivo_commandsets import BaseCommand
from easyslog import *

# capabilities
CAPA_CUSTINFO    = 1 <<  0
CAPA_PRESENCE    = 1 <<  1
CAPA_HISTORY     = 1 <<  2
CAPA_DIRECTORY   = 1 <<  3
CAPA_DIAL        = 1 <<  4
CAPA_FEATURES    = 1 <<  5
CAPA_PEERS       = 1 <<  6
CAPA_MESSAGE     = 1 <<  7
CAPA_SWITCHBOARD = 1 <<  8
CAPA_AGENTS      = 1 <<  9
CAPA_FAX         = 1 << 10
CAPA_DATABASE    = 1 << 11

# this list shall be defined through more options in WEB Interface
CAPA_ALMOST_ALL = CAPA_CUSTINFO | CAPA_PRESENCE | CAPA_HISTORY  | CAPA_DIRECTORY | \
                  CAPA_DIAL | CAPA_FEATURES | CAPA_PEERS | \
                  CAPA_SWITCHBOARD | CAPA_AGENTS | CAPA_FAX | CAPA_DATABASE

map_capas = {
        'customerinfo'     : CAPA_CUSTINFO,
        'presence'         : CAPA_PRESENCE,
        'history'          : CAPA_HISTORY,
        'directory'        : CAPA_DIRECTORY,
        'dial'             : CAPA_DIAL,
        'features'         : CAPA_FEATURES,
        'peers'            : CAPA_PEERS,
        'instantmessaging' : CAPA_MESSAGE,
        'switchboard'      : CAPA_SWITCHBOARD,
        'agents'           : CAPA_AGENTS,
        'fax'              : CAPA_FAX,
        'database'         : CAPA_DATABASE
        }

XIVOVERSION = '0.3.6'
REQUIRED_CLIENT_VERSION = 2025
__revision__ = __version__.split()[1]
__alphanums__ = string.uppercase + string.lowercase + string.digits
allowed_states = ['available', 'away', 'outtolunch', 'donotdisturb', 'berightback']
ITEMS_PER_PACKET = 500
HISTSEPAR = ';'



def varlog(syslogprio, string):
        if syslogprio <= SYSLOG_NOTICE:
                syslogf(syslogprio, 'xivo_fb : ' + string)
        return 0

def log_debug(syslogprio, string):
        if syslogprio <= SYSLOG_INFO:
                print '#debug# %s' % string
        return varlog(syslogprio, string)


class XivoCTICommand(BaseCommand):

        xdname = 'XIVO Daemon'
        capalist_server = 0
        capabilities_list = ['customerinfo',
                             'presence',
                             'history',
                             'directory',
                             'dial',
                             'features',
                             'peers',
                             'instantmessaging',
                             'switchboard',
                             'agents',
                             'fax',
                             'database'] # XXX replace with config result
        for capa in capabilities_list:
                if capa in map_capas: capalist_server |= map_capas[capa]
        conngui_sb = 0
        conngui_xc = 0
        maxgui_sb  = 3
        maxgui_xc  = 3
        xivoclient_session_timeout = 60 # XXX


        fullstat_heavies = {}




        def __init__(self, amis, ctiports, queued_threads_pipe):
		BaseCommand.__init__(self)
                self.amis  = amis

        def get_list_commands(self):
                return ['login',
                        'history', 'directory-search',
                        'featuresget', 'featuresput',
                        'phones-list', 'phones-add', 'phones-del',
                        'faxsend',
                        'database',
                        'message',
                        'availstate',
                        'originate', 'transfer', 'atxfer', 'hangup',
                        'agent']

        def parsecommand(self, linein):
                params = linein.split()
                cmd = xivo_commandsets.Command(params[0], params[1:])
                if cmd.name == 'login':
                        cmd.type = xivo_commandsets.CMD_LOGIN
                else:
                        cmd.type = xivo_commandsets.CMD_OTHER
                return cmd

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
        userfields = ['user', 'phonenum', 'passwd', 'init', 'fullname', 'capas', 'context']

        # fields set at login time by a user, some are a id-dimension, others are auth-related
        # TBD : fields set once logged in ...
        def required_login_params(self):
                return ['loginkind', 'astid', 'proto', 'userid', # identification
                        'state', 'passwd', 'ident', 'version'] # authentication & information


        def manage_login(self, loginparams):
                missings = []
                print loginparams
                for argum in self.required_login_params():
                        if argum not in loginparams:
                                missings.append(argum)
                if len(missings) > 0:
                        return 'missing:%s' % ','.join(missings)


                # trivial checks (version, client kind) dealing with the software used
                version = loginparams.get('version')
                ident = loginparams.get('ident')
                if len(ident.split("@")) == 2:
                        [whoami, whatsmyos] = ident.split("@")
                        if whoami not in ['XC', 'SB']:
                                return 'wrong_client_identifier:%s' % whoami
                        if whatsmyos not in ['X11', 'WIN', 'MAC']:
                                return 'wrong_os_identifier:%s' % whatsmyos
                else:
                        return 'wrong_client_os_identifier:%s' % ident
                if int(version) < REQUIRED_CLIENT_VERSION:
                        return 'version_client:%s;%d' % (version, REQUIRED_CLIENT_VERSION)


                # user match and authentication
                username = loginparams.get('userid')
                userinfo = None
                for astid, ulist in self.ulist.byast.iteritems():
                        userinfo = ulist.finduser(username)
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
                self.__connect_user__(userinfo, whoami, whatsmyos, version, state, False)

                loginkind = loginparams.get('loginkind')
                if loginkind == 'agent':
                        agentnum = loginparams.get('agentid')
                        phonenum = loginparams.get('agentphone')
                        self.amis[astid].agentcallbacklogin(agentnum, phonenum)
                        userinfo['agentnum'] = agentnum
                return userinfo


        def manage_logoff(self, userinfo):
                if 'agentnum' in userinfo:
                        agentnum = userinfo['agentnum']
                        astid = userinfo['astid']
                        self.amis[astid].agentlogoff(agentnum)
                        del userinfo['agentnum']
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

                if whoami == 'XC':
                        if self.conngui_xc >= self.maxgui_xc:
                                return 'xcusers:%d' % self.maxgui_xc
                elif whoami == 'SB':
                        if self.conngui_sb >= self.maxgui_sb:
                                return 'sbusers:%d' % self.maxgui_sb
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

                        if whoami == 'XC':
                                self.conngui_xc += 1
                        elif whoami == 'SB':
                                self.conngui_sb += 1
                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- connect_user %s : %s" %(str(userinfo), str(exc)))


        def __disconnect_user__(self, userinfo):
                try:
                        # state is unchanged
                        whoami = userinfo.get('login').get('cticlienttype')
                        if whoami == 'XC':
                                self.conngui_xc -= 1
                        elif whoami == 'SB':
                                self.conngui_sb -= 1
                        del userinfo['login']

                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- disconnect_user %s : %s" %(str(userinfo), str(exc)))


        def loginko(self, loginparams, errorstring, connid):
                connid.send('loginko=%s\n' % errorstring)
                return


        def loginok(self, loginparams, userinfo):
                capa_user = []
                for capa in self.capabilities_list:
                        if capa in map_capas and (map_capas[capa] & userinfo.get('capas')):
                                capa_user.append(capa)
                repstr = "loginok=" \
                         "context:%s;phonenum:%s;capas:%s;" \
                         "xivoversion:%s;version:%s;state:%s" \
                         %(userinfo.get('context'),
                           userinfo.get('phonenum'),
                           ",".join(capa_user),
                           XIVOVERSION,
                           __revision__,
                           userinfo.get('state'))
##                if 'features' in capa_user:
##                        repstr += ';capas_features:%s' %(','.join(configs[astid].capafeatures))
                userinfo['login']['connection'].send(repstr + '\n')
                return


        def set_configs(self, configs):
                self.configs = configs
                return

        def set_phonelist(self, plist):
                self.plist = plist
                return

        def set_userlist(self, ulist):
                self.ulist = ulist
                return

        def set_contextlist(self, ctxlist):
                self.ctxlist = ctxlist
                return

        def getuserlist(self):
                lulist = {}
                for astid in self.configs:
                        a = self.plist.roughlist(astid)
                        if a is not None:
                                for t, y in a.iteritems():
                                        phonenum = y[3]
                                        fname = (y[1] + ' ' + y[2]).strip()
                                        lulist[phonenum] =  {'user'     : phonenum,
                                                             'passwd'   : phonenum,
                                                             'phonenum' : phonenum,
                                                             'init'     : True,
                                                             'fullname' : fname,
                                                             'capas'    : CAPA_ALMOST_ALL,
                                                             'context'  : y[4]}
                return lulist


        def __send_msg_to_cti_client__(self, userinfo, strupdate):
                if 'login' in userinfo and 'connection' in userinfo.get('login'):
                        mysock = userinfo.get('login')['connection']
                        mysock.send(strupdate + '\n')
                return


        def __send_msg_to_cti_clients__(self, strupdate):
                try:
                        for astid in self.ulist.byast:
                                for user, userinfo in self.ulist.byast[astid].listusers().iteritems():
                                        self.__send_msg_to_cti_client__(userinfo, strupdate)
                except Exception, exc:
                        print '--- exception --- (__send_msg_to_cti_clients__)', exc
                return


        # Methods to handle Asterisk AMI events
        def ami_dial(self, astid, event):
                print astid, event
                src     = event.get("Source")
                dst     = event.get("Destination")
                clid    = event.get("CallerID")
                clidn   = event.get("CallerIDName")
                context = event.get("Context")
                self.plist[astid].handle_ami_event_dial(src, dst, clid, clidn)
                return

        def ami_link(self, astid, event):
                src     = event.get("Channel1")
                dst     = event.get("Channel2")
                clid1   = event.get("CallerID1")
                clid2   = event.get("CallerID2")
                self.plist[astid].handle_ami_event_link(src, dst, clid1, clid2)
                return

        def ami_unlink(self, astid, event):
                src   = event.get("Channel1")
                dst   = event.get("Channel2")
                clid1 = event.get("CallerID1")
                clid2 = event.get("CallerID2")
                self.plist[astid].handle_ami_event_unlink(src, dst, clid1, clid2)
                return

        def ami_hangup(self, astid, event):
                chan  = event.get("Channel")
                cause = event.get("Cause-txt")
                self.plist[astid].handle_ami_event_hangup(chan, cause)
                return

        def ami_aoriginatesuccess(self, astid, event):
                return
        def ami_originatesuccess(self, astid, event):
                return
        def ami_aoriginatefailure(self, astid, event):
                return
        def ami_originatefailure(self, astid, event):
                return
        def ami_messagewaiting(self, astid, event):
                return
        def ami_newcallerid(self, astid, event):
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
                return
        def ami_agentlogoff(self, astid, event):
                return
        def ami_agentcallbacklogoff(self, astid, event):
                return
        def ami_userevent(self, astid, event):
                return
        def ami_agents(self, astid, event):
                return
        def ami_meetmejoin(self, astid, event):
                return
        def ami_meetmeleave(self, astid, event):
                return
        def ami_status(self, astid, event):
                return

        def ami_queuememberstatus(self, astid, event):
                qname    = event.get('Queue')
                location = event.get('Location')
                status   = int(event.get('Status'))
                return

        def ami_join(self, astid, event):
                chan  = event.get('Channel')
                clid  = event.get('CallerID')
                qname = event.get('Queue')
                count = int(event.get('Count'))
                # {'Count': '1', 'CallerID': '103', 'Queue': 'qcb_00000', 'CallerIDName': 'User3', 'Privilege': 'call,all', 'Position': '1', 'Event': 'Join', 'Channel': 'SIP/103-081e4060'}
                return

        def ami_leave(self, astid, event):
                chan  = event.get('Channel')
                qname = event.get('Queue')
                count = int(event.get('Count'))
                return
        
        def ami_rename(self, astid, event):
                return
        # END of AMI events



        def directory_srv2clt(self, context, results):
                header = 'directory-response=%d;%s' %(len(context.search_valid_fields), ';'.join(context.search_titles))
                if len(results) == 0:
                        return header
                else:
                        return header + ';' + ';'.join(results)
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


        def manage_cticommand(self, userinfo, myconn, icommand):
                repstr = ""
                astid    = userinfo['astid']
                username = userinfo['user']
                context  = userinfo['context']
                ucapa    = userinfo['capas']

                try:
                        capalist = (ucapa & self.capalist_server)
                        if icommand.name == 'history':
                                if (capalist & CAPA_HISTORY):
                                        print icommand.args
                                        repstr = self.__build_history_string__(icommand.args[0],
                                                                               icommand.args[1],
                                                                               icommand.args[2])
                        elif icommand.name == 'directory-search':
                                if (capalist & CAPA_DIRECTORY):
                                        repstr = self.__build_customers__(context, icommand.args)
                        elif icommand.name in ['phones-list', 'phones-add', 'phones-del']:
                                if (capalist & (CAPA_PEERS | CAPA_HISTORY)):
                                        repstr = self.__build_callerids_hints__(icommand)
                        elif icommand.name == 'availstate':
                                if (capalist & CAPA_PRESENCE):
                                        repstr = self.__update_availstate__(userinfo, icommand.args[0])
                        elif icommand.name == 'database':
                                if (capalist & CAPA_DATABASE):
                                        repstr = database_update(me, icommand.args)
                        elif icommand.name == 'featuresget':
                                if (capalist & CAPA_FEATURES):
##                                        if username in userlist[astid]:
##                                                userlist[astid][username]['monit'] = icommand.args
                                        repstr = self.__build_features_get__(icommand.args)
                        elif icommand.name == 'featuresput':
                                if (capalist & CAPA_FEATURES):
                                        repstr = self.__build_features_put__(icommand.args)
                        elif icommand.name == 'faxsend':
                                if (capalist & CAPA_FAX):
                                        if astid in faxbuffer:
                                                faxbuffer[astid].append([me, myconn])
                                        repstr = "faxsend=%d" % port_fax
                        elif icommand.name == 'message':
                                if (capalist & CAPA_MESSAGE):
                                        self.__send_msg_to_cti_clients__(self.message_srv2clt('%s/%s' %(astid, username),
                                                                                             '<%s>' % icommand.args[0]))
                        elif icommand.name in ['originate', 'transfer', 'atxfer']:
                                if (capalist & CAPA_DIAL):
                                        repstr = self.__originate_or_transfer__("%s/%s" %(astid, username),
                                                                                [icommand.name, icommand.args[0], icommand.args[1]])
                        elif icommand.name == 'hangup':
                                if (capalist & CAPA_DIAL):
                                        repstr = self.__hangup__("%s/%s" %(astid, username),
                                                                 icommand.args[0])
                        elif icommand.name == 'agent':
                                if (capalist & CAPA_AGENTS):
                                        repstr = self.__agent__(userinfo, icommand.args)
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- (manage_cticommand) %s %s %s %s'
                                  %(icommand.name, str(icommand.args), str(myconn), str(exc)))

                if repstr is not None: # might be useful to reply sth different if there is a capa problem for instance, a bad syntaxed command
                        myconn.send(repstr + '\n')
                return


        def __build_history_string__(self, requester_id, nlines, kind):
                # p/xivo/x/sip/103/103
                [dummyp, astid_src, dummyx, techno, phoneid, phonenum] = requester_id.split('/')
                if astid_src in self.configs:
                        try:
                                reply = []
                                hist = self.__update_history_call__(self.configs[astid_src], techno, phoneid, phonenum, nlines, kind)
                                for x in hist:
                                        try:
                                                ry1 = x[0].isoformat() + HISTSEPAR + x[1].replace('"', '') \
                                                      + HISTSEPAR + str(x[10]) + HISTSEPAR + x[11]
                                        except:
                                                ry1 = x[0] + HISTSEPAR + x[1].replace('"', '') \
                                                      + HISTSEPAR + str(x[10]) + HISTSEPAR + x[11]

                                        if kind == '0':
                                                num = x[3].replace('"', '')
                                                sipcid = "SIP/%s" % num
                                                cidname = num
                                                if sipcid in self.plist[astid_src].normal:
                                                        cidname = '%s %s <%s>' %(self.plist[astid_src].normal[sipcid].calleridfirst,
                                                                                 self.plist[astid_src].normal[sipcid].calleridlast,
                                                                                 num)
                                                ry2 = HISTSEPAR + cidname + HISTSEPAR + 'OUT'
                                        else:   # display callerid for incoming calls
                                                ry2 = HISTSEPAR + x[1].replace('"', '') + HISTSEPAR + 'IN'

                                        reply.append(ry1)
                                        reply.append(ry2)
                                        reply.append(';')
                                return 'history=%s' % ''.join(reply)
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- (%s) error : history : (client %s) : %s'
                                          %(astid_src, requester_id, str(exc)))
                else:
                        return self.dmessage_srv2clt('history KO : no such asterisk id <%s>' % astid_src)
                return


        def __agent__(self, userinfo, commandargs):
##                subcommand = commandargs[0]
##                if subcommand == 'queueenter':
####                self.amis[astid].queueadd(incall.queuename, GHOST_AGENT)
####                self.amis[astid].queuepause(incall.queuename, GHOST_AGENT, 'false')
##                        queuename = commandargs[1]
##                        print queuename
##                elif subcommand == 'queueleave':
####                self.amis[astid].queueremove(incall.queuename, GHOST_AGENT)
####                self.amis[astid].queuepause(incall.queuename, GHOST_AGENT, 'false')
##                        queuename = commandargs[1]
##                        print queuename
##                elif subcommand == 'login':
####                                        self.amis[astid].aoriginate_var('sip', phonenum, 'Log %s' % phonenum,
####                                                agentnum, username, 'ctx-callbooster-agentlogin',
####                                                {'CB_MES_LOGAGENT' : username,
####                                                 'CB_AGENT_NUMBER' : agentnum}, 100)

##                        print subcommand
##                elif subcommand == 'logoff':
##                        # os.popen('asterisk -rx "agent logoff %s"' % agentid)
##                        print subcommand
##                elif subcommand == 'list':
##                        # show the list of available queues ?
##                        pass
##                print userinfo
                return


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
                        for astid in self.configs:
                                plist_n = self.plist[astid]
                                plist_normal_keys = filter(lambda j: plist_n.normal[j].towatch, plist_n.normal.iterkeys())
                                plist_normal_keys.sort()
                                for phonenum in plist_normal_keys:
                                        phoneinfo = ("ful",
                                                     plist_n.astid,
                                                     plist_n.normal[phonenum].build_basestatus(),
                                                     plist_n.normal[phonenum].build_cidstatus(),
                                                     plist_n.normal[phonenum].build_fullstatlist() + ";")
                                        #    + "groupinfos/technique"
                                        self.fullstat_heavies[reqid].append(':'.join(phoneinfo))
                elif kind == 'phones-add':
                        for astid in self.configs:
                                self.fullstat_heavies[reqid].extend(lstadd[astid])
                elif kind == 'phones-del':
                        for astid in self.configs:
                                self.fullstat_heavies[reqid].extend(lstdel[astid])
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
                context = reqlist[1]
                srcnum = reqlist[2]
                repstr = ''

                cursor = self.configs[reqlist[0]].userfeatures_db_conn.cursor()
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
                                return 'featuresget=KO'

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
                                return 'featuresget=KO'

                if len(repstr) == 0:
                        repstr = 'KO'
                return 'featuresget=%s' % repstr


        # \brief Builds the features_put reply.
        def __build_features_put__(self, reqlist):
                context = reqlist[1]
                srcnum = reqlist[2]
                try:
                        len_reqlist = len(reqlist)
                        if len_reqlist >= 4:
                                key = reqlist[3]
                                value = ''
                                if len_reqlist >= 5:
                                        value = reqlist[4]
                                query = 'UPDATE userfeatures SET ' + key + ' = %s WHERE number = %s AND context = %s'
                                params = [value, srcnum, context]
                                cursor = self.configs[reqlist[0]].userfeatures_db_conn.cursor()
                                cursor.query(query, parameters = params)
                                self.configs[reqlist[0]].userfeatures_db_conn.commit()
                                response = 'featuresput=OK;%s;%s;' %(key, value)
                        else:
                                response = 'featuresput=KO'
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- features_put id=%s : %s'
                                  %(str(reqlist), str(exc)))
                        response = 'featuresput=KO'
                return response


        # \brief Originates / transfers.
        def __originate_or_transfer__(self, requester, l):
         src_split = l[1].split("/")
         dst_split = l[2].split("/")
         ret_message = 'originate_or_transfer KO from %s' % requester

         if len(src_split) == 5:
                [dummyp, astid_src, context_src, proto_src, userid_src] = src_split
         elif len(src_split) == 6:
                [dummyp, astid_src, context_src, proto_src, userid_src, dummy_exten_src] = src_split

         if len(dst_split) == 6:
                [dummyp, astid_dst, context_dst, proto_dst, userid_dst, exten_dst] = dst_split
         else:
                [dummyp, astid_dst, context_dst, proto_dst, userid_dst, exten_dst] = src_split
                exten_dst = l[2]
         if astid_src in self.configs and astid_src == astid_dst:
                if exten_dst == 'special:parkthecall':
                        exten_dst = self.configs[astid_dst].parkingnumber
                if astid_src in self.amis and self.amis[astid_src]:
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
                                        ret = self.amis[astid_src].originate(proto_src,
                                                                                           userid_src,
                                                                                           cidname_src,
                                                                                           exten_dst,
                                                                                           cidname_dst,
                                                                                           context_dst)
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
                                                        ret = self.amis[astid_src].transfer(tchan,
                                                                                                          exten_dst,
                                                                                                          context_dst)
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
                                                        ret = self.amis[astid_src].atxfer(tchan,
                                                                                                      exten_dst,
                                                                                                      context_dst)
                                                        if ret:
                                                                ret_message = 'atxfer OK (%s) %s %s' %(astid_src, l[1], l[2])
                                                        else:
                                                                ret_message = 'atxfer KO (%s) %s %s' %(astid_src, l[1], l[2])
         else:
                ret_message = 'originate or transfer KO : asterisk id mismatch (%s %s)' %(astid_src, astid_dst)
         return self.dmessage_srv2clt(ret_message)



        # \brief Hangs up.
        def __hangup__(self, requester, chan):
         astid_src = chan.split("/")[1]
         ret_message = 'hangup KO from %s' % requester
         if astid_src in self.configs:
                log_debug(SYSLOG_INFO, "%s is attempting a HANGUP : %s" %(requester, chan))
                phone, channel = split_from_ui(chan)
                if phone in self.plist[astid_src].normal:
                        if channel in self.plist[astid_src].normal[phone].chann:
                                channel_peer = self.plist[astid_src].normal[phone].chann[channel].getChannelPeer()
                                log_debug(SYSLOG_INFO, "UI action : %s : hanging up <%s> and <%s>"
                                          %(self.configs[astid_src].astid , channel, channel_peer))
                                if astid_src in self.amis and self.amis[astid_src]:
                                        ret = self.amis[astid_src].hangup(channel, channel_peer)
                                        if ret > 0:
                                                ret_message = 'hangup OK (%d) <%s>' %(ret, chan)
                                        else:
                                                ret_message = 'hangup KO : socket request failed'
                                else:
                                        ret_message = 'hangup KO : no socket available'
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
        # \param phonenum the phone number
        # \param nlines the number of lines to fetch for the given phone
        # \param kind kind of list (ingoing, outgoing, missed calls)
        def __update_history_call__(self, cfg, techno, phoneid, phonenum, nlines, kind):
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


        def __update_availstate__(self,userinfo, state):
                astid    = userinfo['astid']
                username = userinfo['user']
                if 'sessiontimestamp' in userinfo:
                        userinfo['sessiontimestamp'] = time.time()
                if state in allowed_states:
                        userinfo['state'] = state
                else:
                        log_debug(SYSLOG_WARNING, '%s : (user %s) : state <%s> is not an allowed one => undefinedstate-updated'
                                  % (astid, username, state))
                        userinfo['state'] = 'undefinedstate-updated'

                self.plist[astid].send_availstate_update(username, state)
                return ""


        # \brief Builds the full list of customers in order to send them to the requesting client.
        # This should be done after a command called "customers".
        # \return a string containing the full customers list
        # \sa manage_tcp_connection
        def __build_customers__(self, ctx, searchpatterns):
         searchpattern = ' '.join(searchpatterns)
         if ctx in self.ctxlist.ctxlist:
                 z = self.ctxlist.ctxlist[ctx]
         else:
                 log_debug(SYSLOG_WARNING, 'there has been no section defined for context %s : can not proceed directory search' % ctx)
                 z = Context()

         fullstatlist = []

         if searchpattern == "":
                return self.directory_srv2clt(z, [])

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
                        for result in results:
                                result_v = {}
                                for f in z.search_matching_fields:
                                        if f in result[1]:
                                                result_v[f] = result[1][f][0]
                                fullstatlist.append(';'.join(z.result_by_valid_field(result_v)))
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- ldaprequest : %s' % str(exc))

         elif dbkind == 'file' or dbkind == 'http':
                log_debug(SYSLOG_WARNING, 'the URI <%s> is not supported yet for directory search queries' %(dbkind))

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
                log_debug(SYSLOG_WARNING, "no database method defined - please fill the dir_db_uri field of the <%s> context" % ctx)

         uniq = {}
         fullstatlist.sort()
         fullstat_body = []
         for fsl in [uniq.setdefault(e,e) for e in fullstatlist if e not in uniq]:
                fullstat_body.append(fsl)
         return self.directory_srv2clt(z, fullstat_body)


xivo_commandsets.CommandClasses['xivocti'] = XivoCTICommand
