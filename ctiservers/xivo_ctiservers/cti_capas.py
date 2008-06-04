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

class Capabilities:
        allowed_funcs = ['agents',
                         'agentdetails',
                         'calls',
                         'customerinfo',
                         'dial',
                         'directory',
                         'fax',
                         'features',
                         'history',
                         'identity',
                         'messages',
                         'presence',
                         'parking',
                         'queues',
                         'queuedetails',
                         'search',
                         'switchboard',
                         'video']

        allowed_xlets = ['agents',
                         'agentdetails',
                         'calls',
                         'customerinfo',
                         'dial',
                         'directory',
                         'fax',
                         'features',
                         'history',
                         'identity',
                         'messages',
                         'operator',
                         'parking',
                         'queues',
                         'queuedetails',
                         'search',
                         'switchboard',
                         'video']

        def __init__(self):
                self.capafuncs = []
                self.capaxlets = []
                self.capadisp = ''
                self.appliname = 'Client'
                self.conngui = 0
                self.maxgui = -1
                return

        def setfuncs(self, capalist_str):
                lst = capalist_str.split(',')
                for capa in lst:
                        if capa in self.allowed_funcs:
                                self.capafuncs.append(capa)

        def setxlets(self, capalist_str):
                lst = capalist_str.split(',')
                for capa in lst:
                        if capa in self.allowed_xlets:
                                self.capaxlets.append(capa)
                return
        
        def setdisplay(self, capadisp):
                self.capadisp = capadisp
                return

        def setappliname(self, appliname):
                self.appliname = appliname
                return

        # maxgui's
        def setmaxgui(self, maxgui):
                self.maxgui = int(maxgui)
                return

        def maxgui(self):
                if self.maxgui == -1:
                        return 'infinite'
                else:
                        return str(self.maxgui)

        def toomuchusers(self):
                if self.maxgui == -1 or self.conngui < self.maxgui:
                        return False
                else:
                        return True

        def conn_inc(self):
                self.conngui += 1
                return

        def conn_dec(self):
                self.conngui -= 1
                return



        def all(self):
                return (2 ** (len(self.allowed_funcs)) - 1)

        def tostring(self, capalist_int):
                lst = []
                for capa in self.capafuncs:
                        n = 2 ** self.allowed_funcs.index(capa)
                        if (n & capalist_int):
                                lst.append('func-' + capa)
                for capa in self.capaxlets:
                        n = 2 ** self.allowed_xlets.index(capa)
                        if (n & capalist_int):
                                lst.append('xlet-' + capa)
                return ','.join(lst)

        def match_funcs(self, ucapas, capa_str):
                capas = capa_str.split(',')
                for cap in capas:
                        if cap in self.capafuncs:
                                n = 2 ** self.allowed_funcs.index(capa_str)
                                if (n & ucapas):
                                        return True
                return False
