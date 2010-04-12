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

import logging
import time
from xivo_ctiservers.cti_anylist import AnyList

log = logging.getLogger('agentlist')

class AgentList(AnyList):
    def __init__(self, newurls = [], useless = None):
        self.anylist_properties = {
            'keywords' : ['firstname', 'lastname', 'number', 'password',
                'context', 'ackcall', 'wrapuptime'],
            'name' : 'agents',
            'action' : 'getagentslist',
            'urloptions' : (1, 4, True) }
        AnyList.__init__(self, newurls)
        return

    queuelocationprops = ['Paused', 'Status', 'Membership', 'Penalty', 'LastCall', 'CallsTaken',
        'Xivo-QueueMember-StateTime']

    def update(self):
        ret = AnyList.update(self)
        self.reverse_index = {}
        for idx, ag in self.keeplist.iteritems():
            if ag['number'] not in self.reverse_index:
                self.reverse_index[ag['number']] = idx
            else:
                log.warning('2 agents have the same number')
        return ret

    def queuememberupdate(self, queuename, queueorgroup, agentnumber, event):
        changed = False
        qorg = '%s_by_agent' % queueorgroup
        if agentnumber in self.reverse_index:
            idx = self.reverse_index[agentnumber]
            if idx in self.keeplist:
                if queuename not in self.keeplist[idx][qorg]:
                    self.keeplist[idx][qorg][queuename] = {}
                    changed = True
                thisagentqueueprops = self.keeplist[idx][qorg][queuename]
                for prop in self.queuelocationprops:
                    if prop in event:
                        if prop in thisagentqueueprops:
                            if thisagentqueueprops[prop] != event.get(prop):
                                thisagentqueueprops[prop] = event.get(prop)
                                changed = True
                        else:
                            thisagentqueueprops[prop] = event.get(prop)
                            changed = True
                if 'Xivo-QueueMember-StateTime' not in thisagentqueueprops:
                    thisagentqueueprops['Xivo-QueueMember-StateTime'] = time.time()
                    changed = True
        return changed

    def queuememberadded(self, queuename, queueorgroup, agentnumber, event):
        qorg = '%s_by_agent' % queueorgroup
        if agentnumber in self.reverse_index:
            idx = self.reverse_index[agentnumber]
            if idx in self.keeplist:
                if queuename not in self.keeplist[idx][qorg]:
                    self.keeplist[idx][qorg][queuename] = {}
                    for prop in self.queuelocationprops:
                        if prop in event:
                            self.keeplist[idx][qorg][queuename][prop] = event.get(prop)
                else:
                    log.warning('queuememberadded : %s already there' % queuename)
        return

    def queuememberremoved(self, queuename, queueorgroup, agentnumber, event):
        qorg = '%s_by_agent' % queueorgroup
        if agentnumber in self.reverse_index:
            idx = self.reverse_index[agentnumber]
            if idx in self.keeplist:
                if queuename in self.keeplist[idx][qorg]:
                    del self.keeplist[idx][qorg][queuename]
                else:
                    log.warning('queuememberremoved : %s not there' % queuename)
        return

    def byagentnumber(self, agentnumber):
        if agentnumber in self.reverse_index:
            idx = self.reverse_index[agentnumber]
            if idx in self.keeplist:
                return self.keeplist[idx]
        return
