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
                AnyList.__init__(self, newurls)
                self.queuelist = {}
                self.virtual = virtual
                return
        
        def update(self):
                lstadd = []
                lstdel = []
                oldqueuelist = self.queuelist
                newqueuelist = {}
                for url, urllist in self.requested_list.iteritems():
                        gl = urllist.getlist(1, 5, True)
                        if gl == 1:
                                newqueuelist.update(self.commandclass.getqueueslist(urllist.list))
                        elif gl == 2:
                                newqueuelist.update(self.commandclass.getqueueslist_json(urllist.jsonreply, self.virtual))
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
        queuestats = ['Abandoned', 'Max', 'Completed', 'ServiceLevel', 'Weight', 'Holdtime',
                      'Xivo-Join', 'Xivo-Link', 'Xivo-Wait', 'Xivo-Chat',
                      'Calls']

        def queueentry_update(self, queue, channel, position, wait, calleridnum, calleridname):
                if queue in self.queuelist:
                        self.queuelist[queue]['channels'][channel] = { 'position' : position,
                                                                       'wait' : wait,
                                                                       'updatetime' : time.time(),
                                                                       'calleridnum' : calleridnum,
                                                                       'calleridname' : calleridname }
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
                lst = {}
                if queuename in self.queuelist:
                        lst[queuename] = self.queuelist[queuename]['stats']
                return lst
        
        def get_queuestats_long(self):
                lst = {}
                for queuename, queueprops in self.queuelist.iteritems():
                        lst[queuename] = queueprops['stats']
                return lst
        
        def get_queues_byagent(self, agid):
                queuelist = []
                for qref, ql in self.queuelist.iteritems():
                        lst = [qref]
                        if agid in ql['agents']:
                                agprop = ql['agents'][agid]
                                for v in self.queuelocationprops:
                                        lst.append(agprop[v])
                        queuelist.append(lst)
                return queuelist
        
        def findqueue(self, queuename):
                if queuename in self.queuelist:
                        return self.queuelist.get(queuename)
                else:
                        return None
