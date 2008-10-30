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

import logging
import time
from xivo_ctiservers.cti_anylist import AnyList

log = logging.getLogger('queuelist')

class QueueList(AnyList):
        def __init__(self, newurls = [], virtual = False):
                self.anylist_properties = {'keywords' : ['number', 'context'],
                                           'name' : 'queues',
                                           'action' : 'getqueueslist',
                                           'urloptions' : (1, 5, True)}
                AnyList.__init__(self, newurls)
                return
        
        queuelocationprops = ['Paused', 'Status', 'Membership', 'Penalty', 'LastCall', 'CallsTaken']
        queuestats = ['Abandoned', 'Max', 'Completed', 'ServiceLevel', 'Weight', 'Holdtime',
                      'Xivo-Join', 'Xivo-Link', 'Xivo-Lost', 'Xivo-Wait', 'Xivo-Chat', 'Xivo-Rate',
                      'Calls']

        def queueentry_update(self, queue, channel, position, wait, calleridnum, calleridname):
                if queue in self.keeplist:
                        self.keeplist[queue]['channels'][channel] = { 'position' : position,
                                                                       'wait' : wait,
                                                                       'updatetime' : time.time(),
                                                                       'calleridnum' : calleridnum,
                                                                       'calleridname' : calleridname }
                else:
                        log.warning('queueentry_update : no such queue %s' % queue)
                return
        
        def queueentry_remove(self, queue, channel):
                if queue in self.keeplist:
                        if channel in self.keeplist[queue]['channels']:
                                del self.keeplist[queue]['channels'][channel]
                else:
                        log.warning('queueentry_remove : no such queue %s' % queue)
                return
        
        def queuememberupdate(self, queue, location, event):
                if queue in self.keeplist:
                        if location not in self.keeplist[queue]['agents']:
                                self.keeplist[queue]['agents'][location] = {}
                        for prop in self.queuelocationprops:
                                if prop in event:
                                        self.keeplist[queue]['agents'][location][prop] = event.get(prop)
                else:
                        log.warning('queuememberupdate : no such queue %s' % queue)
                return
        
        def queuememberremove(self, queue, location):
                if queue in self.keeplist:
                        if location in self.keeplist[queue]['agents']:
                                del self.keeplist[queue]['agents'][location]
                else:
                        log.warning('queuememberremove : no such queue %s' % queue)
                return

        def update_queuestats(self, queue, event):
                if queue in self.keeplist:
                        for statfield in self.queuestats:
                                if statfield in event:
                                        self.keeplist[queue]['stats'][statfield] = event.get(statfield)
                else:
                        log.warning('update_queuestats : no such queue %s' % queue)
                return

        def get_queues(self):
                return self.keeplist.keys()

        def get_queuestats(self, queuename):
                lst = {}
                if queuename in self.keeplist:
                        lst[queuename] = self.keeplist[queuename]['stats']
                return lst
        
        def get_queuestats_long(self):
                lst = {}
                for queuename, queueprops in self.keeplist.iteritems():
                        lst[queuename] = queueprops['stats']
                return lst
        
        def get_queues_byagent(self, agid):
                queuelist = []
                for qref, ql in self.keeplist.iteritems():
                        lst = [qref]
                        if agid in ql['agents']:
                                agprop = ql['agents'][agid]
                                for v in self.queuelocationprops:
                                        lst.append(agprop[v])
                        queuelist.append(lst)
                return queuelist
        
        def findqueue(self, queuename):
                if queuename in self.keeplist:
                        return self.keeplist.get(queuename)
                else:
                        return None
