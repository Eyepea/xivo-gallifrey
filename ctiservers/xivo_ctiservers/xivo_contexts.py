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

"""
Asterisk Contexts.
"""


def getboolean(fname, string):
        if string not in fname:
                return True
        else:
                value = fname.get(string)
                if value in ['false', '0', 'False']:
                        return False
                else:
                        return True




class Contexts:
        def __init__(self):
                self.ctxlist = {}
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
                        print self.displays[ctxname][k].split('|')
                        [title, type, defaultval, format] = self.displays[ctxname][k].split('|')
                        self.display_header[ctxname].append(title)
                return

        def update(self, ctxname, dirname, xivoconf_local):
                dir_db_uri = ''
                dir_db_sqltable = ''
                dir_db_sheetui = ''

                if 'uri' in xivoconf_local:
                        dir_db_uri = xivoconf_local['uri']
                if 'dir_db_sqltable' in xivoconf_local:
                        dir_db_sqltable = xivoconf_local['dir_db_sqltable']
                if 'dir_db_sheetui' in xivoconf_local:
                        dir_db_sheetui = xivoconf_local['dir_db_sheetui']

                z = Context()
                z.setUri(dir_db_uri)
                z.setSqlTable(dir_db_sqltable)
                z.setSheetUi(dir_db_sheetui)

                fnames = {}
                snames = {}
                fkeys = {}
                for field in xivoconf_local:
                        if field.find('dir_db_search') == 0:
                                ffs = field.split('.')
                                if len(ffs) == 3:
                                        if ffs[1] not in fnames:
                                                fnames[ffs[1]] = {}
                                        fnames[ffs[1]][ffs[2]] = xivoconf_local[field]
                        elif field.find('dir_db_sheet') == 0:
                                ffs = field.split('.')
                                if len(ffs) == 3:
                                        if ffs[1] not in snames:
                                                snames[ffs[1]] = {}
                                        snames[ffs[1]][ffs[2]] = xivoconf_local[field]
                        elif field.startswith('field_'):
                                keyword = field.split('_')[1]
                                fkeys['db-%s' % keyword] = xivoconf_local[field].split(',')
                        elif field == 'match_direct':
                                z.setDirectMatch(xivoconf_local[field].split(','))
                        elif field == 'match_reverse':
                                z.setReverseMatch(xivoconf_local[field].split(','))

                z.setKeys(fkeys)

                sheet_vfields = []
                sheet_mfields = []
                for fname in snames.itervalues():
                        if 'field' in fname and 'match' in fname:
                                dbnames = fname['match']
                                if dbnames != '':
                                        dbnames_list = dbnames.split(',')
                                        for dbn in dbnames_list:
                                                if dbn not in sheet_mfields:
                                                        sheet_mfields.append(dbn)
                                        sheet_vfields.append([fname['field'], dbnames_list, False])

                z.setSheetValidFields(sheet_vfields)
                z.setSheetMatchingFields(sheet_mfields)

                if ctxname not in self.ctxlist:
                        self.ctxlist[ctxname] = {}
                self.ctxlist[ctxname][dirname] = z
                return


class Context:
        def __init__(self):
                self.uri = ''
                self.sqltable = ''
                self.sheetui = ''
                self.search_matching_fields = []
                self.sheet_valid_fields = []
                self.sheet_matching_fields = []
                self.match_direct = []
                self.match_reverse = []
                self.fkeys = {}
                return

        def setUri(self, uri):
                self.uri = uri
        def setSqlTable(self, sqltable):
                self.sqltable = sqltable
        def setSheetUi(self, sheetui):
                self.sheetui = sheetui

        def setSheetValidFields(self, vf):
                self.sheet_valid_fields = vf
        def setSheetMatchingFields(self, mf):
                self.sheet_matching_fields = mf
        def setDirectMatch(self, cidm):
                self.match_direct = cidm
        def setReverseMatch(self, cidm):
                self.match_reverse = cidm
        def setKeys(self, keys):
                self.fkeys = keys
