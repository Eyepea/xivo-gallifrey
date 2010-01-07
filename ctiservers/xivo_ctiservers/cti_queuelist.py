# vim: set fileencoding=utf-8 :
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

log = logging.getLogger('queuelist')

class QueueList(AnyList):
    def __init__(self, newurls = [], virtual = False):
        self.anylist_properties = {'keywords' : ['number', 'context', 'queuename'],
                                   'name' : 'queues',
                                   'action' : 'getqueueslist',
                                   'urloptions' : (1, 5, True)}
        AnyList.__init__(self, newurls)
        return
    
    queuelocationprops = ['Paused', 'Status', 'Membership', 'Penalty', 'LastCall', 'CallsTaken',
                          'Xivo-QueueMember-StateTime']
    queuestats = ['Abandoned', 'Max', 'Completed', 'ServiceLevel', 'Weight', 'Holdtime',
                  'Xivo-Join', 'Xivo-Link', 'Xivo-Lost', 'Xivo-Wait', 'Xivo-Chat', 'Xivo-Rate',
                  'Calls']
    
    def update(self):
        ret = AnyList.update(self)
        self.reverse_index = {}
        for idx, ag in self.keeplist.iteritems():
            if ag['queuename'] not in self.reverse_index:
                self.reverse_index[ag['queuename']] = idx
            else:
                log.warning('2 queues have the same name')
        return ret
    
    def hasqueue(self, queuename):
        return self.reverse_index.has_key(queuename)
    
    def getcontext(self, queueid):
        return self.keeplist[queueid]['context']
    
    def fillstats(self, queueid, statin):
        self.keeplist[queueid]['queuestats']['Xivo-Join'] = len(statin['ENTERQUEUE'])
        self.keeplist[queueid]['queuestats']['Xivo-Link'] = len(statin['CONNECT'])
        self.keeplist[queueid]['queuestats']['Xivo-Lost'] = len(statin['ABANDON'])
        nj = self.keeplist[queueid]['queuestats']['Xivo-Join']
        nl = self.keeplist[queueid]['queuestats']['Xivo-Link']
        if nj > 0:
            self.keeplist[queueid]['queuestats']['Xivo-Rate'] = (nl * 100) / nj
        else:
            self.keeplist[queueid]['queuestats']['Xivo-Rate'] = -1
        return
    
    def queueentry_update(self, queueid, channel, position, entrytime, calleridnum, calleridname):
        if queueid in self.keeplist:
            self.keeplist[queueid]['channels'][channel] = { 'position' : position,
                                                            'entrytime' : entrytime,
                                                            'calleridnum' : calleridnum,
                                                            'calleridname' : calleridname }
        else:
            log.warning('queueentry_update : no such queueid %s' % queueid)
        return
    
    def queueentry_remove(self, queueid, channel):
        if queueid in self.keeplist:
            if channel in self.keeplist[queueid]['channels']:
                del self.keeplist[queueid]['channels'][channel]
        else:
            log.warning('queueentry_remove : no such queueid %s' % queueid)
        return
    
    def queuememberupdate(self, queueid, location, event):
        changed = False
        if queueid in self.keeplist:
            if location not in self.keeplist[queueid]['agents_in_queue']:
                self.keeplist[queueid]['agents_in_queue'][location] = {}
                changed = True
            thisqueuelocation = self.keeplist[queueid]['agents_in_queue'][location]
            for prop in self.queuelocationprops:
                if prop in event:
                    if prop in thisqueuelocation:
                        if thisqueuelocation[prop] != event.get(prop):
                            thisqueuelocation[prop] = event.get(prop)
                            changed = True
                    else:
                        thisqueuelocation[prop] = event.get(prop)
                        changed = True
            if 'Xivo-QueueMember-StateTime' not in thisqueuelocation:
                thisqueuelocation['Xivo-QueueMember-StateTime'] = time.time()
                changed = True
        else:
            log.warning('queuememberupdate : no such queueid %s' % queueid)
        return changed
    
    def queuememberremove(self, queueid, location):
        changed = False
        if queueid in self.keeplist:
            if location in self.keeplist[queueid]['agents_in_queue']:
                del self.keeplist[queueid]['agents_in_queue'][location]
                changed = True
        else:
            log.warning('queuememberremove : no such queueid %s' % queueid)
        return changed
    
    def update_queuestats(self, queueid, event):
        changed = False
        if queueid in self.keeplist:
            thisqueuestats = self.keeplist[queueid]['queuestats']
            for statfield in self.queuestats:
                if statfield in event:
                    if statfield in thisqueuestats:
                        if thisqueuestats[statfield] != event.get(statfield):
                            thisqueuestats[statfield] = event.get(statfield)
                            changed = True
                    else:
                        thisqueuestats[statfield] = event.get(statfield)
                        changed = True
        else:
            log.warning('update_queuestats : no such queueid %s' % queueid)
        return changed
    
    def get_queues(self):
        return self.keeplist.keys()
    
    def get_queues_byagent(self, agid):
        queuelist = {}
        for qref, ql in self.keeplist.iteritems():
            lst = {}
            if agid in ql['agents_in_queue']:
                agprop = ql['agents_in_queue'][agid]
                for v in self.queuelocationprops:
                    if v in agprop:
                        lst[v] = agprop[v]
                    else:
                        log.warning('get_queues_byagent : no property %s for agent %s in queue %s'
                                    % (v, agid, qref))
            lst['context'] = ql['context']
            queuelist[qref] = lst
        return queuelist
