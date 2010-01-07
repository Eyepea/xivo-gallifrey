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

log = logging.getLogger('trunklist')

class TrunkList(AnyList):
        def __init__(self, newurls = []):
                self.anylist_properties = {'keywords' : ['tech', 'name', 'ip',
                                                         'type', 'context'],
                                           'name' : 'trunk',
                                           'action' : 'gettrunklist',
                                           'urloptions' : (1, 5, True)}
                AnyList.__init__(self, newurls)
                return
        
        def update(self):
                ret = AnyList.update(self)
                # self.reverse_index = {}
                return ret
        
        def ami_dial(self, trunkidsrc, trunkiddst, uidsrc, uiddst, puidsrc, puiddst):
                if trunkidsrc in self.keeplist:
                        if uidsrc in self.keeplist[trunkidsrc]['comms']:
                                pass
                        else:
                                infos = {'thischannel' : puidsrc.get('channel'),
                                         'peerchannel' : puidsrc.get('dial'),
                                         'status' : 'calling',
                                         'time-dial' : 0,
                                         'timestamp-dial' : time.time(),
                                         #'calleridname' : puidsrc.get('calleridname'),
                                         'calleridnum' : puidsrc.get('extension')
                                         }
                                self.keeplist[trunkidsrc]['comms'][uidsrc] = infos
                if trunkiddst in self.keeplist:
                        if uiddst in self.keeplist[trunkiddst]['comms']:
                                pass
                        else:
                                infos = {'thischannel' : puiddst.get('channel'),
                                         'peerchannel' : puiddst.get('dial'),
                                         'status' : 'ringing',
                                         'time-dial' : 0,
                                         'timestamp-dial' : time.time(),
                                         'calleridname' : puidsrc.get('calleridname'),
                                         'calleridnum' : puidsrc.get('calleridnum')
                                         }
                                self.keeplist[trunkiddst]['comms'][uiddst] = infos
                return
        
        def ami_link(self, trunkidsrc, trunkiddst, uidsrc, uiddst, puidsrc, puiddst):
                if trunkidsrc in self.keeplist:
                        if uidsrc in self.keeplist[trunkidsrc]['comms']:
                                infos = {'status' : 'linked-caller',
                                         'time-link' : 0,
                                         'timestamp-link' : time.time()
                                         }
                                self.keeplist[trunkidsrc]['comms'][uidsrc].update(infos)
                if trunkiddst in self.keeplist:
                        if uiddst in self.keeplist[trunkiddst]['comms']:
                                infos = {'status' : 'linked-called',
                                         'time-link' : 0,
                                         'timestamp-link' : time.time()
                                         }
                                self.keeplist[trunkiddst]['comms'][uiddst].update(infos)
                return
        
        def ami_unlink(self, trunkidsrc, trunkiddst, uidsrc, uiddst, puidsrc, puiddst):
                if trunkidsrc in self.keeplist:
                        if uidsrc in self.keeplist[trunkidsrc]['comms']:
                                infos = {'status' : 'unlinked-caller',
                                         'time-link' : 0,
                                         'timestamp-link' : time.time()
                                         }
                                self.keeplist[trunkidsrc]['comms'][uidsrc].update(infos)
                if trunkiddst in self.keeplist:
                        if uiddst in self.keeplist[trunkiddst]['comms']:
                                infos = {'status' : 'unlinked-called',
                                         'time-link' : 0,
                                         'timestamp-link' : time.time()
                                         }
                                self.keeplist[trunkiddst]['comms'][uiddst].update(infos)
                return
        
        def ami_hangup(self, uid):
                trunkidlist = []
                for trunkid, trunkprops in self.keeplist.iteritems():
                        if uid in trunkprops['comms']:
                                trunkprops['comms'][uid]['status'] = 'hangup'
                                if trunkid not in trunkidlist:
                                        trunkidlist.append(trunkid)
                return trunkidlist
        
        def clear(self, uid):
                trunkidlist = []
                for trunkid, trunkprops in self.keeplist.iteritems():
                        if uid in trunkprops['comms']:
                                del trunkprops['comms'][uid]
                                if trunkid not in trunkidlist:
                                        trunkidlist.append(trunkid)
                return trunkidlist
        
        def ami_rename(self, oldtrunkid, newtrunkid, oldname, newname, uid):
                for trunkid, v in self.keeplist.iteritems():
                        for k, kk in v['comms'].iteritems():
                                if kk.get('thischannel') == oldname:
                                        kk['thischannel'] = newname
                                if kk.get('peerchannel') == oldname:
                                        kk['peerchannel'] = newname
                if oldtrunkid and newtrunkid and oldtrunkid != newtrunkid:
                        if uid in self.keeplist[oldtrunkid]['comms'] and uid not in self.keeplist[newtrunkid]['comms']:
                                self.keeplist[newtrunkid]['comms'][uid] = self.keeplist[oldtrunkid]['comms'][uid]
                                del self.keeplist[oldtrunkid]['comms'][uid]
                        else:
                                log.warning('(ami_rename) %s : could not move from %s to %s' % (uid, oldtrunkid, newtrunkid))
                return
        
        def ami_rename_tophone(self, oldtrunkid, oldname, newname, uid):
                tomove = None
                for trunkid, v in self.keeplist.iteritems():
                        for k, kk in v['comms'].iteritems():
                                if kk.get('thischannel') == oldname:
                                        kk['thischannel'] = newname
                                if kk.get('peerchannel') == oldname:
                                        kk['peerchannel'] = newname
                if uid in self.keeplist[oldtrunkid]['comms']:
                        tomove = self.keeplist[oldtrunkid]['comms'][uid]
                        # do not remove the reference at once, because, the client side needs to
                        # know that the uid has been hanged-up
                        # del self.keeplist[oldtrunkid]['comms'][uid]
                        self.keeplist[oldtrunkid]['comms'][uid]['status'] = 'hangup'
                else:
                        log.warning('(ami_rename_tophone) %s : could not remove %s' % (uid, oldtrunkid))
                return tomove
        
        def ami_rename_fromphone(self, newtrunkid, oldname, newname, uid, tomove):
                for trunkid, v in self.keeplist.iteritems():
                        for k, kk in v['comms'].iteritems():
                                if kk.get('thischannel') == oldname:
                                        kk['thischannel'] = newname
                                if kk.get('peerchannel') == oldname:
                                        kk['peerchannel'] = newname
                if tomove and uid not in self.keeplist[newtrunkid]['comms']:
                        self.keeplist[newtrunkid]['comms'][uid] = tomove
                else:
                        log.warning('(ami_rename_fromphone) %s : could not set %s' % (uid, newtrunkid))
                return
        
        def status(self, trunkid):
                tosend = {}
                if trunkid in self.keeplist:
                        tosend = { 'class' : 'trunks',
                                   'direction' : 'client',
                                   'function' : 'update',
                                   'trunkid' : trunkid,
                                   'status' : self.keeplist[trunkid] }
                return tosend
