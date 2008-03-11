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
                self.byast = {}
                self.commandclass = None
                return


        def setcommandclass(self, commandclass):
                self.commandclass = commandclass
                return


        def adduser(self, astid, userparams):
                self.byast[astid].adduser(userparams)
                return


        def finduser(self, username):
                uinfo = None
                for astid, user_byast in self.byast.iteritems():
                        uinfo = user_byast.finduser(username)
                        if uinfo is not None:
                                break
                return uinfo


        def update(self, astid):
                if astid not in self.byast:
                        self.byast[astid] = Users_byast(astid, self.commandclass.userfields)
                userl = self.commandclass.getuserlist()
                for ul, vv in userl.iteritems():
                        self.adduser(astid, vv)
                return


        # to be called sometimes (update period ?)
        def check_connected_accounts(self):
                for user,userinfo in userlist[self.astid].iteritems():
                        if 'sessiontimestamp' in userinfo:
                                if time.time() - userinfo.get('sessiontimestamp') > xivoclient_session_timeout:
                                        log_debug(SYSLOG_INFO, '%s : timeout reached for %s' %(self.astid, user))
                                        disconnect_user(userinfo)
                                        self.send_availstate_update(user, 'unknown')


## \class Users_byast
# \brief Properties of Users, sorted by Asterisk id
class Users_byast:
        def __init__(self, iastid, ifields):
                self.astid = iastid
                self.list = {}
                self.fields = ifields
                return

        def adduser(self, inparams):
                user = inparams['user']
                if self.list.has_key(user):
                        # updates
                        # self.list[user]['agentnum'] = agentnum
                        pass
                else:
                        self.list[user] = {}
                        self.list[user]['astid'] = self.astid
                        for f in self.fields:
                                self.list[user][f] = inparams[f]
        def deluser(self, user):
                if self.list.has_key(user):
                        self.list.pop(user)

        def finduser(self, user):
                return self.list.get(user)

        def listusers(self):
                return self.list

        def listconnected(self):
                lst = {}
                for user, info in self.list.iteritems():
                        if 'login' in info:
                                lst[user] = info
                return lst
