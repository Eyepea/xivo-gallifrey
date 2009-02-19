# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2009 Proformatique'
__author__    = 'Corentin Le Gall'

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

"""
Asterisk Contexts.
"""

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
                self.display_header[ctxname] = ['%s' % len(self.display_items[ctxname])]
                for k in self.display_items[ctxname]:
                        [title, type, defaultval, format] = self.displays[ctxname][k].split('|')
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
                self.sqltable = ''
                self.name = '(noname)'
                self.match_direct = []
                self.match_reverse = []
                return
        
        def setProps(self, xivoconf_local):
                self.fkeys = {}
                for field in xivoconf_local:
                        if field.startswith('field_'):
                                keyword = field.split('_')[1]
                                self.fkeys['db-%s' % keyword] = xivoconf_local[field].split(',')
                        elif field in 'uri':
                                self.uri = xivoconf_local[field]
                        elif field == 'name':
                                self.name = xivoconf_local[field]
                        elif field == 'dir_db_sqltable':
                                self.sqltable = xivoconf_local[field]
                        elif field == 'match_direct':
                                self.match_direct = str(xivoconf_local[field]).split(',')
                        elif field == 'match_reverse':
                                self.match_reverse = str(xivoconf_local[field]).split(',')
                return
