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

"""
Asterisk Contexts.
"""

import logging

log = logging.getLogger('xivo_contexts')

class Contexts:
    def __init__(self):
        self.ctxlist = {}
        self.dirlist = {}

        self.displays = {}
        self.display_header = {}
        self.display_items = {}
        return

    def setdisplay(self, ctxname, xivoconf_items):
        if ctxname not in self.displays:
            self.displays[ctxname] = {}
        self.displays[ctxname] = xivoconf_items
        self.display_header[ctxname] = []
        self.display_items[ctxname] = xivoconf_items.keys()
        self.display_items[ctxname].sort()
        self.display_header[ctxname] = []
        for k in self.display_items[ctxname]:
            [title, type, defaultval, format] = self.displays[ctxname][k]
            self.display_header[ctxname].append(title)
        return

    def update(self, ctxname, dirname, xivoconf_local):
        z = Directory()
        z.setProps(xivoconf_local)

        if ctxname not in self.ctxlist:
            self.ctxlist[ctxname] = []
        self.ctxlist[ctxname].append(dirname)
        self.dirlist[dirname] = z
        return

    def updatedir(self, dirname, xivoconf_local):
        z = Directory()
        z.setProps(xivoconf_local)
        self.dirlist[dirname] = z
        return

class Directory:
    def __init__(self):
        self.uri = ''
        self.delimiter = ';'
        self.sqltable = ''
        self.name = '(noname)'
        self.display_reverse = '{db-fullname}'
        self.match_direct = []
        self.match_reverse = []
        return

    def setProps(self, xivoconf_local):
        self.fkeys = {}
        for field, value in xivoconf_local.iteritems():
            if field.startswith('field_'):
                keyword = field.split('_')[1]
                self.fkeys['db-%s' % keyword] = value
            elif field in 'uri':
                self.uri = value
            elif field == 'name':
                self.name = value
            elif field == 'delimiter':
                self.delimiter = value
            elif field == 'dir_db_sqltable':
                self.sqltable = value
            elif field == 'display_reverse':
                if value:
                    self.display_reverse = value[0]
            elif field == 'match_direct':
                self.match_direct = value
            elif field == 'match_reverse':
                self.match_reverse = value
        return
