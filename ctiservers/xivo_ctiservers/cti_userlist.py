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

import logging
from xivo_ctiservers.cti_anylist import AnyList

log = logging.getLogger('userlist')

class UserList(AnyList):
        def __init__(self, newurls = []):
                self.anylist_properties = { 'keywords' : ['capaids', 'password', 'fullname',
                                                          'agentid', 'techlist', 'phoneid', 'phonenum',
                                                          'context', 'mwi'],
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
                        return uinfo
                else:
                        if username in self.keeplist:
                                return self.keeplist.get(username)
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
