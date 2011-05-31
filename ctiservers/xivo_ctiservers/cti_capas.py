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

import cjson
import logging
import urllib

log = logging.getLogger('capas')

class Capabilities:
    allowed_funcs = [
        # action rights
        'agents', 'conference', 'customerinfo', 'dial',
        'directory', 'fax', 'features', 'history',
        'chitchat', 'presence', 'database',
        # record campaigns acl
        'supervisor', 'administrator',
        # misc. switchboard-related actions
        'switchboard', 'pickup'
        ]

    def __init__(self, allowed_xlets):
        self.appliname = 'Client'
        self.capaxlets = []

        self.capafuncs = []
        self.capaservices = []
        self.guisettings = {}
        self.conngui = 0
        self.maxgui = -1
        self.presenceid = 'none'
        self.watchedpresenceid = 'none'
        self.allowed_xlets = allowed_xlets
        return

    def setappliname(self, appliname):
        self.appliname = appliname
        return

    def setxlets(self, capalist):
        for detail in capalist:
            capa = '-'.join(detail)
            if detail[0] in self.allowed_xlets.keys():
                if capa not in self.capaxlets:
                    self.capaxlets.append(capa)
                self.setfuncs(self.allowed_xlets[detail[0]])
        return

    def setfuncs(self, capalist):
        for capa in capalist:
            if capa in self.allowed_funcs and capa not in self.capafuncs:
                self.capafuncs.append(capa)
        return

    def setservices(self, services):
        self.capaservices = services
        return

    def getguisettings(self):
        return self.guisettings

    def setguisettings(self, guisettings):
        self.guisettings = {}
        for k, v in guisettings.iteritems():
            if k:
                if v.startswith('{') and v.endswith('}'):
                    vv = cjson.decode(v.replace('\"', '"'))
                else:
                    vv = v
                self.guisettings[k] = vv
        return

    def setpresenceid(self, presenceid):
        self.presenceid = presenceid
        if self.watchedpresenceid == 'none':
            self.watchedpresenceid = presenceid
        return

    def setwatchedpresenceid(self, watchedpresenceid):
        self.watchedpresenceid = watchedpresenceid
        return

    # maxgui's
    def setmaxgui(self, maxgui):
        if maxgui == '':
            maxgui = -1
        self.maxgui = int(maxgui)
        return

    def getmaxgui(self):
        ret = 'infinite'
        if self.maxgui > -1:
            ret = str(self.maxgui)
        return ret

    def toomuchusers(self):
        ret = True
        if self.maxgui == -1 or self.conngui < self.maxgui:
            ret = False
        return ret

    def conn_inc(self):
        self.conngui += 1
        return

    def conn_dec(self):
        self.conngui -= 1
        return

    def all(self):
        return (2 ** (len(self.allowed_funcs)) - 1)

    def tostringlist(self, capalist_int):
        lst = []
        for capa in self.capafuncs:
            n = 2 ** self.allowed_funcs.index(capa)
            if (n & capalist_int):
                lst.append(capa)
        return lst

    def match_funcs(self, ucapas, capa_str):
        """
        ucapas is intended to be a mask for some user-level defined capas
        """
        capas = capa_str.split(',')
        for cap in capas:
            if cap in self.capafuncs:
                n = 2 ** self.allowed_funcs.index(cap)
                if (n & ucapas):
                    return True
        return False
