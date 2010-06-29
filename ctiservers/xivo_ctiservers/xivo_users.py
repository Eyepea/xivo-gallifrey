# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2010 Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, XiVO CTI Server is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XiVO CTI Server
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

class Users:
    def __init__(self):
        self.list = {}
        self.commandclass = None
        return

    def setcommandclass(self, commandclass):
        self.commandclass = commandclass
        return

    def adduser(self, inparams):
        username = inparams.get('user')
        if self.list.has_key(username):
            # updates
            # self.list[username]['agentnum'] = agentnum
            pass
        else:
            self.list[username] = {}
            for f in self.fields:
                self.list[username][f] = inparams[f]
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
