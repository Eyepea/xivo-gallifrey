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

class Users:
        def __init__(self):
                self.list = {}
                self.commandclass = None
                return

        def setcommandclass(self, commandclass):
                self.commandclass = commandclass
                return

        def adduser(self, inparams):
                user = inparams['user']
                if self.list.has_key(user):
                        # updates
                        # self.list[user]['agentnum'] = agentnum
                        pass
                else:
                        self.list[user] = {}
                        for f in self.fields:
                                self.list[user][f] = inparams[f]
                return

        def deluser(self, username):
                if self.list.has_key(username):
                        self.list.pop(username)

        def finduser(self, username):
                return self.list.get(username)

        def listconnected(self):
                lst = {}
                for user, info in self.list.iteritems():
                        if 'login' in info:
                                lst[user] = info
                return lst

        def update(self):
                self.fields = self.commandclass.userfields
                userl = self.commandclass.getuserlist()
                for ul, vv in userl.iteritems():
                        self.adduser(vv)
                return

        # to be called sometimes (update period ?)
        def check_connected_accounts(self):
                for user, userinfo in userlist[self.astid].iteritems():
                        if 'sessiontimestamp' in userinfo:
                                if time.time() - userinfo.get('sessiontimestamp') > xivoclient_session_timeout:
                                        log_debug(SYSLOG_INFO, '%s : timeout reached for %s' %(self.astid, user))
                                        disconnect_user(userinfo)
                                        self.send_availstate_update(user, 'unknown')
