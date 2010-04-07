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

class Presence:
    def __init__(self, config):
        self.details = {}
        self.states = []
        self.displaydetails = {}
        self.defaultstate = 'available'
        if config is not  None:
            for stateid, stateprops in config.iteritems():
                splitprops = stateprops.split(',')
                if len(splitprops) > 3:
                    longname = splitprops[0]
                    allowednext = splitprops[1]
                    action = splitprops[2]
                    color = splitprops[3]

                    if len(allowednext) > 0:
                        allowednexts = allowednext.split(':')
                    else:
                        allowednexts = []
                    if len(action) > 0:
                        actions = action.split(':')
                    else:
                        actions = []

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
        return []
