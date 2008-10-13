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
Base class for miscellaneous Command Sets
"""

CMD_OTHER = 1 << 0
CMD_LOGIN_ID = 1 << 1
CMD_LOGIN_PASS = 1 << 2
CMD_LOGIN_CAPAS = 1 << 3
CMD_TRANSFER = 1 << 10

class Command:
        def __init__(self, commandname, commandargs):
                self.name = commandname
                self.args = commandargs
                self.type = None

class BaseCommand:
        
        separator = '\n'
        xdname = 'XIVO Base'
        
        def __init__(self):
                self.transfers_ref = {}
                return
        
        def handle_outsock(self, astid, msg):
                return
        def checkqueue(self):
                return
        def clear_disconnlist(self):
                return

        def reset(self, mode, conn):
                return
        def transfer_addbuf(self, req, buf):
                return
        def transfer_addref(self, req, ref):
                return
        def transfer_endbuf(self, req):
                return
        
        # inits / updates
        def extrasock(self, extraconn):
                return
        def set_options(self, xivoconf):
                return
        def set_cticonfig(self, cticonfig):
                return

        userfields = []
        def getuserslist(self):
                return
        def users(self):
                return {}
        def connected_users(self):
                return {}
        def getqueueslist(self, qlist):
                return {}
        def getqueueslist_json(self, qlist):
                return {}

        def set_cticdr(self, cticdr):
                return
        def set_presence(self, config_presence):
                return
        def set_configs(self, configs):
                return
        def set_phonelist(self, astid, urllist_phones):
                return
        def set_agentlist(self, astid, urllist_agents):
                return
        def set_queuelist(self, astid, urllist_queues):
                return
        def set_userlist_urls(self, urls):
                return
        def set_userlist(self, ulist):
                return
        def set_userlist_ng(self, ulist):
                return
        def set_contextlist(self, ctxlist):
                return
        def updates(self):
                return
        
        def getprofilelist(self):
                return []
        # connection
        def connected(self, connid):
                return

        # login
        def required_login_params(self):
                return
        def get_login_params(self, astid, command, connid):
                return
        def manage_login(self, loginparams):
                return
        def manage_logout(self, userinfo, when):
                return
        def loginko(self, loginparams, connid, errorstring):
                return
        def loginok(self, loginparams, userinfo):
                return
        def telldisconn(self, connid):
                return

        # command events (~ CTI)
        def get_list_commands(self):
                return []
        def parsecommand(self, linein):
                return
        def manage_cticommand(self, userinfo, parsedcommand):
                return

        def regular_update(self):
                return

        # AGI events
        def handle_agi(self, astid, msg):
                return
        def handle_fagi(self, astid, msg):
                return

        def phones_update(self, function, args):
                return

        def cliaction(self, connid, command):
                return

        def askstatus(self, astid, npl):
                return

##        def update_srv2clt(self, phoneinfo):
##                return None
##        def message_srv2clt(self, sender, message):
##                return None
        def dmessage_srv2clt(self, message):
                return None

        # Methods to handle Asterisk AMI events
        def ami_channelreload(self, astid, event):
                return
        def ami_reload(self, astid, event):
                return
        def ami_shutdown(self, astid, event):
                return
        def ami_registry(self, astid, event):
                return
        def ami_alarm(self, astid, event):
                return
        def ami_alarmclear(self, astid, event):
                return
        def ami_cdr(self, astid, event):
                return
	def ami_faxsent(self, astid, event):
		return
        def ami_faxreceived(self, astid, event):
                return
        def ami_dial(self, astid, event):
                return
        def ami_link(self, astid, event):
                return
        def ami_unlink(self, astid, event):
                return
        def ami_hangup(self, astid, event):
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
        def ami_messagewaiting(self, astid, event):
                return
        def ami_newcallerid(self, astid, event):
                return
        def ami_parkedcall(self, astid, event):
                return
        def ami_unparkedcall(self, astid, event):
                return
        def ami_parkedcallgiveup(self, astid, event):
                return
        def ami_parkedcalltimeout(self, astid, event):
                return
        def ami_parkedcallscomplete(self, astid, event):
                return

        # Agents' AMI Events
        def ami_agentlogin(self, astid, event):
                return
        def ami_agentlogoff(self, astid, event):
                return
        def ami_agentcallbacklogin(self, astid, event):
                return
        def ami_agentcallbacklogoff(self, astid, event):
                return
        def ami_agentcalled(self, astid, event):
                return
        def ami_agentcomplete(self, astid, event):
                return
        def ami_agentdump(self, astid, event):
                return
        def ami_agentconnect(self, astid, event):
                return
        def ami_agents(self, astid, event):
                return

        def ami_userevent(self, astid, event):
                return
        def ami_meetmejoin(self, astid, event):
                return
        def ami_meetmeleave(self, astid, event):
                return
        def ami_status(self, astid, event):
                return

        def ami_queueentry(self, astid, event):
                return
        def ami_queuemember(self, astid, event):
                return
        def ami_queuememberadded(self, astid, event):
                return
        def ami_queuememberremoved(self, astid, event):
                return
        def ami_queuememberstatus(self, astid, event):
                return
        def ami_queuememberpaused(self, astid, event):
                return
        def ami_queueparams(self, astid, event):
                return
        def ami_queuecallerabandon(self, astid, event):
                return
        def ami_queuestatuscomplete(self, astid, event):
                print 'AMI %s QueueStatusComplete' % astid
                return

        def ami_join(self, astid, event):
                return
        def ami_leave(self, astid, event):
                return

        def ami_rename(self, astid, event):
                return
        def ami_peerstatus(self, astid, event):
                return
        def ami_newexten(self, astid, event):
                return
        def ami_extensionstatus(self, astid, event):
                return
        def ami_newstate(self, astid, event):
                return
        def ami_newchannel(self, astid, event):
                return
        def ami_statuscomplete(self, astid, event):
                print 'AMI %s StatusComplete' % astid
                return

        def ami_agentscomplete(self, astid, event):
                print 'AMI %s AgentsComplete' % astid
                return

        def amiresponse_success(self, astid, event):
                return
        def amiresponse_error(self, astid, event):
                return

        # QueueMemberStatus ExtensionStatus
        #                 0                  AST_DEVICE_UNKNOWN
        #                 1               0  AST_DEVICE_NOT_INUSE  /  libre
        #                 2               1  AST_DEVICE IN USE     / en ligne
        #                 3                  AST_DEVICE_BUSY
        #                                 4  AST_EXTENSION_UNAVAILABLE ?
        #                 5                  AST_DEVICE_UNAVAILABLE
        #                 6 AST_EXTENSION_RINGING = 8  appele

        # failure reasons (?)
        #define AST_CONTROL_HANGUP              1
        #define AST_CONTROL_RING                2
        #define AST_CONTROL_RINGING             3
        #define AST_CONTROL_ANSWER              4
        #define AST_CONTROL_BUSY                5
        #define AST_CONTROL_TAKEOFFHOOK         6
        #define AST_CONTROL_OFFHOOK             7
        #define AST_CONTROL_CONGESTION          8
        #define AST_CONTROL_FLASH               9
        #define AST_CONTROL_WINK                10
        
        # END of AMI events

        # XIVO synchronization methods
        def pre_reload(self):
                return
        def pre_moh_reload(self):
                return

CommandClasses = {}
