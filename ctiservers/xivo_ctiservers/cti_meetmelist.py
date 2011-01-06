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

import logging
from xivo_ctiservers.cti_anylist import AnyList

log = logging.getLogger('meetmelist')

class MeetmeList(AnyList):
    def __init__(self, newurls = [], useless = None):
        self.anylist_properties = { 'keywords' : [ 'roomnumber',
                                                   'roomname',
                                                   'context',
                                                   'pin',
                                                   'pinadmin',
                                                   'moderated' ],
                                    'name' : 'meetme',
                                    'action' : 'getmeetmelist',
                                    'urloptions' : (1, 5, True)
                                    }
        AnyList.__init__(self, newurls)
        return

    def update(self):
        ret = AnyList.update(self)
        self.reverse_index = {}
        for idx, ag in self.keeplist.iteritems():
            if ag['roomnumber'] not in self.reverse_index:
                self.reverse_index[ag['roomnumber']] = idx
            else:
                log.warning('2 meetme have the same roomnumber')
        return ret

    def byroomnumber(self, roomnumber):
        meetmeref = None
        meetme_id = self.reverse_index.get(roomnumber)
        if meetme_id:
            meetmeref = self.keeplist.get(meetme_id)
        return (meetmeref, meetme_id)
