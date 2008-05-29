# XIVO Daemon

__version__   = '$Revision: 2730 $'
__date__      = '$Date: 2008-03-31 19:34:50 +0200 (lun, 31 mar 2008) $'
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

class QueueList:
        def __init__(self, newurls = []):
                self.list = {}
                self.commandclass = None
                self.requested_list = {}
                self.queuelist = {}
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

        def getqueueslist(self, newurls = []):
                self.__seturls__(newurls)

                if len(self.requested_list) == 0:
                        return -1

                self.update()
                return

        def update(self):
                for url, urllist in self.requested_list.iteritems():
                        gl = urllist.getlist(0, 10)
                        # print url, urllist, gl, urllist.list
                        for j, k in urllist.list.iteritems():
                                self.list[j] = k
                newqueuelist = self.commandclass.getqueueslist(self.list)
                for a, b in newqueuelist.iteritems():
                        if a not in self.queuelist:
                                self.queuelist[a] = b
                return

        def findqueue(self, queuename):
                if queuename in self.queuelist:
                        return self.queuelist.get(queuename)
                else:
                        return None


##        def adduser(self, inparams):
##                user = inparams['user']
##                if self.list.has_key(user):
##                        # updates
##                        # self.list[user]['agentnum'] = agentnum
##                        pass
##                else:
##                        self.list[user] = {}
##                        for f in self.fields:
##                                self.list[user][f] = inparams[f]
##                return

##        def deluser(self, username):
##                if self.list.has_key(username):
##                        self.list.pop(username)

##        def listconnected(self):
##                lst = {}
##                for user, info in self.list.iteritems():
##                        if 'login' in info:
##                                lst[user] = info
##                return lst

##        def update(self):
##                self.fields = self.commandclass.userfields
##                userl = self.commandclass.getuserlist()
##                for ul, vv in userl.iteritems():
##                        self.adduser(vv)
##                return
