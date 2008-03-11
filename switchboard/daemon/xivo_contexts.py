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
        ctxlist = {}

        def __init__(self):
                return

        def update(self, ctxname, xivoconf_local):
                dir_db_uri = ''
                dir_db_sqltable = ''
                dir_db_sheetui = ''

                if 'dir_db_uri' in xivoconf_local:
                        dir_db_uri = xivoconf_local['dir_db_uri']
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
                                        elif len(ffs) == 2 and ffs[1] == 'callidmatch':
                                                z.setSheetCallidMatch(xivoconf_local[field].split(','))

                search_vfields = []
                search_mfields = []
                for fname in fnames.itervalues():
                        if 'display' in fname and 'match' in fname:
                                dbnames = fname['match']
                                if dbnames != '':
                                                dbnames_list = dbnames.split(',')
                                                for dbn in dbnames_list:
                                                        if dbn not in search_mfields:
                                                                search_mfields.append(dbn)
                                                keepspaces = getboolean(fname, 'space')
                                                search_vfields.append([fname['display'], dbnames_list, keepspaces])
                z.setSearchValidFields(search_vfields)
                z.setSearchMatchingFields(search_mfields)
                        
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

                self.ctxlist[ctxname] = z
                return


class Context:
        uri = ''
        sqltable = ''
        sheetui = ''
        search_titles = []
        search_valid_fields = []
        search_matching_fields = []
        sheet_valid_fields = []
        sheet_matching_fields = []
        sheet_callidmatch = []

        def __init__(self):
                return
        
        def setUri(self, uri):
                self.uri = uri
        def setSqlTable(self, sqltable):
                self.sqltable = sqltable
        def setSheetUi(self, sheetui):
                self.sheetui = sheetui

        def setSearchValidFields(self, vf):
                self.search_valid_fields = vf
                for x in vf:
                        self.search_titles.append(x[0])
        def setSearchMatchingFields(self, mf):
                self.search_matching_fields = mf

        def setSheetValidFields(self, vf):
                self.sheet_valid_fields = vf
        def setSheetMatchingFields(self, mf):
                self.sheet_matching_fields = mf
        def setSheetCallidMatch(self, cidm):
                self.sheet_callidmatch = cidm

        def result_by_valid_field(self, result):
                reply_by_field = []
                for [dummydispname, dbnames_list, keepspaces] in self.search_valid_fields:
                        field_value = ""
                        for dbname in dbnames_list:
                                if dbname in result and field_value is "":
                                        field_value = result[dbname]
                        if keepspaces:
                                reply_by_field.append(field_value)
                        else:
                                reply_by_field.append(field_value.replace(' ', ''))
                return reply_by_field

