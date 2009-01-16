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

log = logging.getLogger('grouplist')

class GroupList(AnyList):
        def __init__(self, newurls = [], virtual = False):
                self.anylist_properties = {'keywords' : ['number', 'context'],
                                           'name' : 'groups',
                                           'action' : 'getgroupslist',
                                           'urloptions' : (1, 5, True)}
                AnyList.__init__(self, newurls)
                return
        
        grouplocationprops = ['Paused', 'Status', 'Membership', 'Penalty', 'LastCall', 'CallsTaken', 'Xivo-StateTime']
        groupstats = ['Abandoned', 'Max', 'Completed', 'ServiceLevel', 'Weight', 'Holdtime',
                      'Xivo-Join', 'Xivo-Link', 'Xivo-Lost', 'Xivo-Wait', 'Xivo-Chat', 'Xivo-Rate',
                      'Calls']
        
        def groupentry_update(self, group, channel, position, wait, calleridnum, calleridname):
                if group in self.keeplist:
                        self.keeplist[group]['channels'][channel] = { 'position' : position,
                                                                       'wait' : wait,
                                                                       'updatetime' : time.time(),
                                                                       'calleridnum' : calleridnum,
                                                                       'calleridname' : calleridname }
                else:
                        log.warning('groupentry_update : no such group %s' % group)
                return
        
        def groupentry_remove(self, group, channel):
                if group in self.keeplist:
                        if channel in self.keeplist[group]['channels']:
                                del self.keeplist[group]['channels'][channel]
                else:
                        log.warning('groupentry_remove : no such group %s' % group)
                return
        
        def groupmemberupdate(self, group, location, event):
                if group in self.keeplist:
                        if location not in self.keeplist[group]['agents']:
                                self.keeplist[group]['agents'][location] = {}
                        for prop in self.grouplocationprops:
                                if prop in event:
                                        self.keeplist[group]['agents'][location][prop] = event.get(prop)
                else:
                        log.warning('groupmemberupdate : no such group %s' % group)
                return
        
        def groupmemberremove(self, group, location):
                if group in self.keeplist:
                        if location in self.keeplist[group]['agents']:
                                del self.keeplist[group]['agents'][location]
                else:
                        log.warning('groupmemberremove : no such group %s' % group)
                return

        def update_groupstats(self, group, event):
                if group in self.keeplist:
                        for statfield in self.groupstats:
                                if statfield in event:
                                        self.keeplist[group]['stats'][statfield] = event.get(statfield)
                else:
                        log.warning('update_groupstats : no such group %s' % group)
                return

        def get_groups(self):
                return self.keeplist.keys()

        def get_groupstats(self, groupname):
                lst = {}
                if groupname in self.keeplist:
                        lst[groupname] = self.keeplist[groupname]['stats']
                return lst
        
        def get_groupstats_long(self):
                lst = {}
                for groupname, groupprops in self.keeplist.iteritems():
                        lst[groupname] = groupprops['stats']
                return lst
        
        def get_groups_byagent(self, agid):
                grouplist = {}
                for qref, ql in self.keeplist.iteritems():
                        lst = {}
                        if agid in ql['agents']:
                                agprop = ql['agents'][agid]
                                for v in self.grouplocationprops:
                                        if v in agprop:
                                                lst[v] = agprop[v]
                                        else:
                                                log.warning('get_groups_byagent : no property %s for agent %s in group %s'
                                                            % (v, agid, qref))
                        grouplist[qref] = lst
                return grouplist
        
        def findgroup(self, groupname):
                if groupname in self.keeplist:
                        return self.keeplist.get(groupname)
                else:
                        return None
