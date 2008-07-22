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

import csv
import md5
import urllib
from xivo_log import *
import cti_urllist

def log_debug(level, text):
        log_debug_file(level, text, 'userlist')

class UserList:
        def __init__(self, newurls = []):
                self.list = {}
                self.commandclass = None
                self.requested_list = {}
                self.userlist = {}
                self.__seturls__(newurls)
                return

        def setcommandclass(self, commandclass):
                self.commandclass = commandclass
                return

        def __seturls__(self, newurls):
                for url in newurls:
                        if url not in self.requested_list:
                                self.requested_list[url] = cti_urllist.UrlList(url)
                return

        def getuserslist(self, newurls = []):
                self.__seturls__(newurls)

                if len(self.requested_list) == 0:
                        return -1

                self.update()
                return

        def update(self):
                for url, urllist in self.requested_list.iteritems():
                        gl = urllist.getlist(0, 10, True)
                        if len(urllist.list) == 0:
                                gl = urllist.getlist(1, 12, False)
                                newuserlist = self.commandclass.getuserslist_compat(urllist.list)
                        else:
                                newuserlist = self.commandclass.getuserslist(urllist.list)
                        for a, b in newuserlist.iteritems():
                                if a not in self.userlist:
                                        self.userlist[a] = b
                return

        def finduser(self, username):
                if username in self.userlist:
                        return self.userlist.get(username)
                else:
                        return None

        def users(self):
                return self.userlist

        def connected_users(self):
                lst = {}
                for username, userinfo in self.userlist.iteritems():
                        if 'login' in userinfo:
                                lst[username] = userinfo
                return lst

        def adduser(self, inparams):
                username = inparams['user']
                if self.userlist.has_key(username):
                        # updates
                        # self.list[user]['agentnum'] = agentnum
                        pass
                else:
                        self.userlist[username] = {}
                        for f in self.commandclass.userfields:
                                self.userlist[username][f] = inparams[f]
                return

        def deluser(self, username):
                if self.userlist.has_key(username):
                        self.userlist.pop(username)
