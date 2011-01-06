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

log = logging.getLogger('presence')

class Presence:
    def __init__(self, config):
        self.details = {}
        self.states = []
        self.displaydetails = {}
        self.defaultstate = 'available'
        if config is not  None:
            for stateid, stateprops in config.iteritems():
                longname = stateprops.get('display')
                allowednexts = stateprops.get('status')
                actions = stateprops.get('actions')
                color = stateprops.get('color')

                self.details.update( { stateid :
                                       { 'allowednexts' : allowednexts,
                                         'actions' : actions}
                                       } )
                self.displaydetails[stateid] = {'longname' : longname,
                                                'color' : color,
                                                'stateid' : stateid}
                self.states.append(stateid)
        # to add : actions associated with status : change queue / remove or pause from queue
        return

    def getstates(self):
        return self.states

    def getdisplaydetails(self):
        return self.displaydetails

    def getcolors(self):
        return self.colors

    def getdefaultstate(self):
        return self.defaultstate

    def countstatus(self, counts):
        ntot = 0
        for statename, count in counts.iteritems():
            ntot += count
        return {'connected' : ntot }

    def allowed(self, status):
        rep = {}
        if status is not None and status in self.details:
            w = self.details[status]
            for u, v in self.details.iteritems():
                rep[u] = (u in w['allowednexts'] or u == status)
        return rep

    def actions(self, status):
        if status is not None and status in self.details:
            w = self.details[status]
            return w['actions']
        else:
            log.warning('status <%s> is None or not in detailed definitions' % status)
            return {}
