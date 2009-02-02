# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2009 Proformatique'
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
from xivo_ctiservers.cti_anylist import AnyList

log = logging.getLogger('phonelist')

class PhoneList(AnyList):
        def __init__(self, newurls = []):
                self.anylist_properties = { 'keywords' : [],
                                            'name' : 'phones',
                                            'action' : 'getphoneslist',
                                            'urloptions' : (1, 12, False) }
                AnyList.__init__(self, newurls)
                return
        
        def ami_dial(self, phoneidsrc, phoneiddst, uidsrc, uiddst, puidsrc, puiddst):
                if phoneidsrc in self.keeplist:
                        if uidsrc in self.keeplist[phoneidsrc]['comms']:
                                pass
                        else:
                                infos = {'thischannel' : puidsrc.get('channel'),
                                         'peerchannel' : puidsrc.get('dial'),
                                         'status' : 'calling',
                                         'time-dial' : 0,
                                         #'calleridname' : puidsrc.get('calleridname'),
                                         'calleridnum' : puidsrc.get('extension')
                                         }
                                self.keeplist[phoneidsrc]['comms'][uidsrc] = infos
                if phoneiddst in self.keeplist:
                        if uiddst in self.keeplist[phoneiddst]['comms']:
                                pass
                        else:
                                infos = {'thischannel' : puiddst.get('channel'),
                                         'peerchannel' : puiddst.get('dial'),
                                         'status' : 'ringing',
                                         'time-dial' : 0,
                                         'calleridname' : puidsrc.get('calleridname'),
                                         'calleridnum' : puidsrc.get('calleridnum')
                                         }
                                self.keeplist[phoneiddst]['comms'][uiddst] = infos
                return
        
        def ami_link(self, phoneidsrc, phoneiddst, uidsrc, uiddst, puidsrc, puiddst):
                if phoneidsrc in self.keeplist:
                        if uidsrc in self.keeplist[phoneidsrc]['comms']:
                                infos = {'status' : 'linked-caller',
                                         'time-link' : 0
                                         }
                                self.keeplist[phoneidsrc]['comms'][uidsrc].update(infos)
                if phoneiddst in self.keeplist:
                        if uiddst in self.keeplist[phoneiddst]['comms']:
                                infos = {'status' : 'linked-called',
                                         'time-link' : 0
                                         }
                                self.keeplist[phoneiddst]['comms'][uiddst].update(infos)
                return
        
        def ami_unlink(self, phoneidsrc, phoneiddst, uidsrc, uiddst, puidsrc, puiddst):
                if phoneidsrc in self.keeplist:
                        if uidsrc in self.keeplist[phoneidsrc]['comms']:
                                infos = {'status' : 'unlinked-caller',
                                         'time-link' : 0
                                         }
                                self.keeplist[phoneidsrc]['comms'][uidsrc].update(infos)
                if phoneiddst in self.keeplist:
                        if uiddst in self.keeplist[phoneiddst]['comms']:
                                infos = {'status' : 'unlinked-called',
                                         'time-link' : 0
                                         }
                                self.keeplist[phoneiddst]['comms'][uiddst].update(infos)
                return
        
        def ami_rename(self, oldphoneid, newphoneid, oldname, newname, uid):
                for phoneid, v in self.keeplist.iteritems():
                        for k, kk in v['comms'].iteritems():
                                if kk['thischannel'] == oldname:
                                        kk['thischannel'] = newname
                                if kk['peerchannel'] == oldname:
                                        kk['peerchannel'] = newname
                if oldphoneid != newphoneid:
                        if uid in self.keeplist[oldphoneid]['comms'] and uid not in self.keeplist[newphoneid]['comms']:
                                self.keeplist[newphoneid]['comms'][uid] = self.keeplist[oldphoneid]['comms'][uid]
                                # self.keeplist[oldphoneid]['comms'][uid]['status'] = 'hangup'
                                del self.keeplist[oldphoneid]['comms'][uid]
                                log.info('%s moved from %s to %s' % (uid, oldphoneid, newphoneid))
                return
        
        def ami_hangup(self, uid):
                phoneidlist = []
                for phoneid, phoneprops in self.keeplist.iteritems():
                        if uid in phoneprops['comms']:
                                phoneprops['comms'][uid]['status'] = 'hangup'
                                if phoneid not in phoneidlist:
                                        phoneidlist.append(phoneid)
                return phoneidlist
        
        def clear(self, uid):
                phoneidlist = []
                for phoneid, phoneprops in self.keeplist.iteritems():
                        if uid in phoneprops['comms']:
                                del phoneprops['comms'][uid]
                                if phoneid not in phoneidlist:
                                        phoneidlist.append(phoneid)
                return phoneidlist
        
        def setdisplayhints(self, dh):
                self.display_hints = dh
                return
        
        def ami_extstatus(self, phoneid, status):
                if phoneid in self.keeplist:
                        if status not in self.display_hints:
                                status = '-2'
                        self.keeplist[phoneid]['hintstatus'] = self.display_hints.get(status)
                return
        
        def ami_parkedcall(self, phoneid, uid, ctuid):
                if phoneid in self.keeplist:
                        infos = {'status' : 'linked-caller',
                                 'time-link' : 0,
                                 'calleridnum' : 'Parqué'
                                 }
                        if uid in self.keeplist[phoneid]['comms']:
                                self.keeplist[phoneid]['comms'][uid].update(infos)
                        else:
                                self.keeplist[phoneid]['comms'][uid] = infos
                return
        
        def ami_unparkedcall(self, phoneid, uid, ctuid):
                if phoneid in self.keeplist:
                        if uid in self.keeplist[phoneid]['comms']:
                                # parked channel
                                infos = {'status' : 'linked-called',
                                         'time-link' : 0,
                                         'calleridnum' : ctuid['parkexten-callback']
                                         }
                                self.keeplist[phoneid]['comms'][uid].update(infos)
                        else:
                                # cfrom
                                infos = {'status' : 'linked-caller',
                                         'thischannel' : ctuid['channel'],
                                         'time-link' : 0,
                                         'calleridnum' : ctuid['parkexten-callback']
                                         }
                                self.keeplist[phoneid]['comms'][uid] = infos
                return
        
        def status(self, phoneid):
                tosend = {}
                if phoneid in self.keeplist:
                        tosend = { 'class' : 'phones',
                                   'direction' : 'client',
                                   'function' : 'update',
                                   'phoneid' : phoneid,
                                   'status' : self.keeplist[phoneid] }
                return tosend
