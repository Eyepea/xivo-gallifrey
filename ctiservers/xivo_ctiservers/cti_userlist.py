# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2011 Proformatique'
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

import logging
from xivo_ctiservers.cti_anylist import AnyList

log = logging.getLogger('userlist')

class UserList(AnyList):
    def __init__(self, newurls = []):
        self.anylist_properties = { 'keywords' : ['capaids', 'user', 'password', 'fullname',
                                                  'agentid', 'voicemailid', 'simultcalls', #'mwi',
                                                  'techlist', 'phoneid', 'phonenum', 'mobilenum',
                                                  'context'],
                                    'name' : 'users',
                                    'action' : 'getuserslist',
                                    'urloptions' : (0, 11, True) }
        AnyList.__init__(self, newurls)
        return

    def update_noinput(self):
        newuserlist = self.commandclass.getuserslist()
        for a, b in newuserlist.iteritems():
            if a not in self.keeplist:
                self.keeplist[a] = b
        return

    def finduser(self, userid, company = None):
        if company:
            uinfo = None
            for userinfo in self.keeplist.itervalues():
                if userinfo['user'] == userid and userinfo['company'] == company:
                    uinfo = userinfo
                    break
            if uinfo == None:
                for userinfo in self.keeplist.itervalues():
                    if userinfo['user'] == userid:
                        uinfo = userinfo
                        break

            return uinfo
        else:
            if userid in self.keeplist:
                return self.keeplist.get(userid)
            else:
                return None

    def users(self):
        return self.keeplist

    def connected_users(self):
        lst = {}
        for username, userinfo in self.keeplist.iteritems():
            if 'login' in userinfo:
                lst[username] = userinfo
        return lst

    def adduser(self, inparams):
        username = inparams['user']
        if self.keeplist.has_key(username):
            # updates
            pass
        else:
            self.keeplist[username] = {}
            for f in self.commandclass.userfields:
                self.keeplist[username][f] = inparams[f]
        return

    def deluser(self, username):
        if self.keeplist.has_key(username):
            self.keeplist.pop(username)
        return
