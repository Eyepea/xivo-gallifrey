# vim: set fileencoding=utf-8 :
# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2009-2010 Proformatique'
__author__    = 'Thomas Bernard'

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
import time

log = logging.getLogger('sheetmanager')

class SheetManager:
    
    class Sheet:
        class SheetEntry:
            def __init__(self, user, type, data):
                self.time = time.time()
                self.user = user
                self.type = type
                self.data = data

            def todict(self):
                return {'time': self.time, 'user': self.user, 'type': self.type, 'data': self.data}

        def __init__(self, channel):
            self.channel = channel
            self.entries = []
            self.currentuser = None
            self.viewingusers = []
            self.sheet = '' # initial customer sheet
        
        def addentry(self, type, data):
            self.entries.append(self.SheetEntry(self.currentuser, type, data))

        def addviewinguser(self, user):
            if not user in self.viewingusers:
                self.viewingusers.append(user)

    def __init__(self, astid=None):
        self.astid = astid
        self.sheets = {}

    def get_sheet(self, channel):
        return self.sheets.get(channel)

    def new_sheet(self, channel):
        log.debug('new_sheet %s' % (channel))
        self.sheets[channel] = self.Sheet(channel)

    def del_sheet(self, channel):
        log.debug('del_sheet channel=%s' % (channel))
        del self.sheets[channel]

    def has_sheet(self, channel):
        log.debug('has_sheet channel=%s channelist=%s' % (channel, self.sheets.keys()))
        return self.sheets.has_key(channel)

    def update_currentuser(self, channel, user):
        log.debug('update_currentuser channel=%s user=%s' % (channel, user))
        self.sheets[channel].currentuser = user
        self.sheets[channel].addviewinguser(user)

    def addentry(self, channel, type, data):
        log.debug('addentry %s (%s)"%s"' % (channel, type, data))
        self.sheets[channel].addentry(type, data)

    def addviewingusers(self, channel, users):
        for user in users:
            self.sheets[channel].addviewinguser(user)

    def setcustomersheet(self, channel, sheet):
        self.sheets[channel].sheet = sheet
