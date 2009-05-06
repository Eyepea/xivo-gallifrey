# vim: set fileencoding=utf-8 :
# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2009 Proformatique'
__author__    = 'Thomas Bernard'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
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
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import logging
import time

log = logging.getLogger('sheetmanager')

class SheetManager:
    
    class Sheet:
        class SheetEntry:
            def __init(self, user, text):
                self.time = time.time()
                self.user = user
                self.text = text

        def __init__(self, channel):
            self.channel = channel
            self.entries = []
            self.currentuser = None
            self.sheet = '' # initial customer sheet
        
        def addentry(self, text):
            self.entries.append(SheetEntry(self.currentuser, text))

    def __init__(self):
        self.sheets = {}

    def get_sheet(self, channel):
        return self.sheets.get(channel)

    def new_sheet(self, channel):
        self.sheets[channel] = Sheet(channel)

    def del_sheet(self, channel):
        del self.sheets[channel]



