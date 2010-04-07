# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2010 Proformatique'
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

import cjson
import logging
import urllib

log = logging.getLogger('capas')

class Capabilities:
    allowed_funcs = ['agents',

        'conference',
        'customerinfo',
        'dial',
        'directory',
        'fax',
        'features',
        'history',
        'messages',

        'chitchat',

        'presence',
        'database',
        'switchboard']

    allowed_xlets_base = {'void' : {}}

    def __init__(self, allowed_xlets):
        self.capafuncs = []
        self.capadisps = ['void-grid-0']
        self.appliname = 'Client'
        self.guisettings = {}
        self.conngui = 0
        self.maxgui = -1
        self.presenceid = 'none'
        self.watchedpresenceid = 'none'
        self.guiurl = None
        self.allowed_xlets = allowed_xlets
        self.allowed_xlets.update(self.allowed_xlets_base)
        return

    def setfuncs(self, capalist):
        for capa in capalist:
            if capa in self.allowed_funcs and capa not in self.capafuncs:
                self.capafuncs.append(capa)
        return

    def setxlets(self, capalist):
        for capa in capalist:
            detail = capa.split('-')
            if len(detail) > 2 and detail[1] == 'grid' and detail[2] == '0' and 'void-grid-0' in self.capadisps:
                self.capadisps.remove('void-grid-0')
            if detail[0] in self.allowed_xlets.keys():
                if capa not in self.capadisps:
                    self.capadisps.append(capa)
                self.setfuncs(self.allowed_xlets[detail[0]])
        return

    def setappliname(self, appliname):
        self.appliname = appliname
        return

    def setpresenceid(self, presenceid):
        self.presenceid = presenceid
        if self.watchedpresenceid == 'none':
            self.watchedpresenceid = presenceid
        return

    def setwatchedpresenceid(self, watchedpresenceid):
        self.watchedpresenceid = watchedpresenceid
        return

    def getguisettings(self):
        guisettings = {}
        if self.guiurl is not None:
            try:
                gui = urllib.urlopen(self.guiurl)
                guisettings = cjson.decode(gui.read())
                gui.close()
            except Exception:
                log.exception('problem when reading guisettings from %s' % self.guiurl)
        return guisettings

    def setguisettings(self, urlsettings):
        self.guiurl = urlsettings
        return

    # maxgui's
    def setmaxgui(self, maxgui):
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
        capas = capa_str.split(',')
        for cap in capas:
            if cap in self.capafuncs:
                n = 2 ** self.allowed_funcs.index(cap)
                if (n & ucapas):
                    return True
        return False
