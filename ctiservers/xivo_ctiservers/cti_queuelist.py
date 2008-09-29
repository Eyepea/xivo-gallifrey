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
from xivo_ctiservers.cti_anylist import AnyList

log = logging.getLogger('queuelist')

class QueueList(AnyList):
        def __init__(self, newurls = []):
                AnyList.__init__(self, newurls)
                self.queuelist = {}
                return
        
        def update(self):
                lstadd = []
                lstdel = []
                oldqueuelist = self.queuelist
                newqueuelist = {}
                for url, urllist in self.requested_list.iteritems():
                        gl = urllist.getlist(1, 5, True)
                        newqueuelist.update(self.commandclass.getqueueslist(urllist.list))
                for a, b in newqueuelist.iteritems():
                        if a not in oldqueuelist:
                                self.queuelist[a] = b
                                lstadd.append(a)
                for a, b in oldqueuelist.iteritems():
                        if a not in newqueuelist:
                                lstdel.append(a)
                for a in lstdel:
                        del self.queuelist[a]
                return { 'add' : lstadd,
                         'del' : lstdel }

        queuelocationprops = ['Paused', 'Status', 'Membership', 'Penalty', 'LastCall', 'CallsTaken']
        queuestats = ['ServicelevelPerf', 'Abandoned', 'Max', 'Completed', 'ServiceLevel', 'Weight', 'Holdtime', 'Calls']

        def queueentry_update(self, queue, channel, position, wait):
                if queue in self.queuelist:
                        self.queuelist[queue]['channels'][channel] = [position, wait]
                else:
                        log.warning('queueentry_update : no such queue %s' % queue)
                return
        
        def queueentry_remove(self, queue, channel):
                if queue in self.queuelist:
                        if channel in self.queuelist[queue]['channels']:
                                del self.queuelist[queue]['channels'][channel]
                else:
                        log.warning('queueentry_remove : no such queue %s' % queue)
                return
        
        def queuememberupdate(self, queue, location, event):
                if queue in self.queuelist:
                        if location not in self.queuelist[queue]['agents']:
                                self.queuelist[queue]['agents'][location] = {}
                        for prop in self.queuelocationprops:
                                if prop in event:
                                        self.queuelist[queue]['agents'][location][prop] = event.get(prop)
                else:
                        log.warning('queuememberupdate : no such queue %s' % queue)
                return
        
        def queuememberremove(self, queue, location):
                if queue in self.queuelist:
                        if location in self.queuelist[queue]['agents']:
                                del self.queuelist[queue]['agents'][location]
                else:
                        log.warning('queuememberremove : no such queue %s' % queue)
                return

        def update_queuestats(self, queue, event):
                if queue in self.queuelist:
                        for statfield in self.queuestats:
                                if statfield in event:
                                        self.queuelist[queue]['stats'][statfield] = event.get(statfield)
                else:
                        log.warning('update_queuestats : no such queue %s' % queue)
                return

        def get_queues(self):
                return self.queuelist.keys()

        def get_queuestats(self, queuename):
                lst = []
                if queuename in self.queuelist:
                        lst.append(queuename)
                        for prop in self.queuestats:
                                if prop in self.queuelist[queuename]['stats']:
                                        lst.append('%s:%s' % (prop, self.queuelist[queuename]['stats'][prop]))
                return ':'.join(lst)
        
        def get_queuestats_long(self):
                lst_byqueue = []
                for queuename, queueprops in self.queuelist.iteritems():
                        lst = [queuename]
                        for prop in self.queuestats:
                                if prop in queueprops['stats']:
                                        lst.append('%s:%s' % (prop, queueprops['stats'][prop]))
                        lst_byqueue.append(':'.join(lst))
                return lst_byqueue
        
        def get_queues_byagent(self, agid):
                queuelist = []
                for qref, ql in self.queuelist.iteritems():
                        lst = [qref]
                        if agid in ql['agents']:
                                agprop = ql['agents'][agid]
                                for v in self.queuelocationprops:
                                        lst.append(agprop[v])
                        queuelist.append('-'.join(lst))
                return ','.join(queuelist)
        
        def findqueue(self, queuename):
                if queuename in self.queuelist:
                        return self.queuelist.get(queuename)
                else:
                        return None
