# vim: set fileencoding=utf-8 :
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
import time
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
        
        def __createorupdate_comm__(self, phoneid, commid, infos):
                comms = self.keeplist[phoneid]['comms']
                if commid in self.keeplist[phoneid]['comms']:
                    if infos.has_key('calleridnum') and comms[commid].get('calleridnum') != infos.get('calleridnum'):
                        log.debug('  __createorupdate_comm__ changed calleridnum[%s %s] %s => %s'
                                  % (commid, comms[commid].get('thischannel'), comms[commid].get('calleridnum'), infos.get('calleridnum')))
                    self.keeplist[phoneid]['comms'][commid].update(infos)
                else:
                    if infos.has_key('calleridnum'):
                        log.debug('  __createorupdate_comm__ new calleridnum[%s %s] : %s' % (commid, infos.get('thischannel'), infos.get('calleridnum')))
                    self.keeplist[phoneid]['comms'][commid] = infos
                    self.setlinenum(phoneid, commid)
                    
        def setlinenum(self, phoneid, commid):
            """
            Define a line number for the phone, according to the currently 'busy'
            channels/uniqueids.
            This can not (at the time being, at least) be directly related
            to the line numbers on the physical phones.
            """
            usedlines = []
            for cid, cinfo in self.keeplist[phoneid]['comms'].iteritems():
                if 'linenum' in cinfo:
                    usedlines.append(cinfo.get('linenum'))
            linenum = 1
            while (linenum in usedlines):
                linenum += 1
            self.keeplist[phoneid]['comms'][commid]['linenum'] = linenum
            return
        
        def updatechan(self, phoneid, infos, commid=None):
                log.debug('phone::updatechan %s %s' % (phoneid, infos))
                # we are gessing which "comm" because there is only one !
                if commid is None and len(self.keeplist[phoneid]['comms'].keys()) == 1:
                        commid = self.keeplist[phoneid]['comms'].keys()[0]
                if commid is not None:
                        self.__createorupdate_comm__(phoneid, commid, infos)
                        
        def ami_newchannel(self, phoneid, uid, channel):
                # we could store the "callerid" in order to use it later.
                self.__createorupdate_comm__(phoneid, uid, {'thischannel':channel, 'calleridname':'<unknown>'})
                
        def ami_newstate(self, phoneid, uid, channel, status):
                self.__createorupdate_comm__(phoneid, uid, {'status' : status})
                
        def ami_dial(self, phoneidsrc, phoneiddst, uidsrc, uiddst, puidsrc, puiddst):
                if phoneidsrc in self.keeplist:
                        infos = {'thischannel' : puidsrc.get('channel'),
                                 'peerchannel' : puidsrc.get('dial'),
                                 'status' : 'calling',
                                 'time-dial' : 0,
                                 'timestamp-dial' : time.time(),
                                 #'calleridname' : puidsrc.get('calleridname'),
                                 'calleridnum' : puidsrc.get('extension')
                                 }
                        self.__createorupdate_comm__(phoneidsrc, uidsrc, infos)
                if phoneiddst in self.keeplist:
                        infos = {'thischannel' : puiddst.get('channel'),
                                 'peerchannel' : puiddst.get('dial'),
                                 'status' : 'ringing',
                                 'time-dial' : 0,
                                 'timestamp-dial' : time.time(),
                                 'calleridname' : puidsrc.get('calleridname'),
                                 'calleridnum' : puidsrc.get('calleridnum')
                                 }
                        self.__createorupdate_comm__(phoneiddst, uiddst, infos)
                return
        
        def ami_link(self, phoneidsrc, phoneiddst, uidsrc, uiddst, puidsrc, puiddst, clidsrc, cliddst, clidnamesrc, clidnamedst):
                log.debug(u'phonelist::ami_link(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)'
                          % (phoneidsrc, phoneiddst, uidsrc, uiddst, puidsrc, puiddst, clidsrc, cliddst, clidnamesrc, clidnamedst))
                if phoneidsrc in self.keeplist:
                        infos = {'time-link' : 0,
                                 'peerchannel' : puidsrc['link'],
                                 'status' : 'linked-caller',
                                 'timestamp-link' : time.time()
                                 }
                        if clidnamedst is not None:# and not self.keeplist[phoneidsrc]['comms'][uidsrc].has_key('calleridname'):
                            infos['calleridname'] = clidnamedst
                        if cliddst is not None:# and not self.keeplist[phoneidsrc]['comms'][uidsrc].has_key('calleridnum'):
                            infos['calleridnum'] = cliddst
                        if uidsrc in self.keeplist[phoneidsrc]['comms']:
                                self.__createorupdate_comm__(phoneidsrc, uidsrc, infos)
                        else:
                                infos['thischannel'] = puidsrc['channel']
                                self.__createorupdate_comm__(phoneidsrc, uidsrc, infos)
                                log.debug('phonelist::ami_link %s not found (src)' % (uidsrc))
                        log.debug('phonelist::ami_link gruik %s %s' % (phoneidsrc, self.keeplist[phoneidsrc]['comms']))
                if phoneiddst in self.keeplist:
                        infos = {'time-link' : 0,
                                 'peerchannel' : puiddst['link'],
                                 'status' : 'linked-called',
                                 'timestamp-link' : time.time()
                                 }
                        if clidnamesrc is not None:# and not self.keeplist[phoneiddst]['comms'][uiddst].has_key('calleridname'):
                            infos['calleridname'] = clidnamesrc
                        if clidsrc is not None:# and not self.keeplist[phoneiddst]['comms'][uiddst].has_key('calleridnum'):
                            infos['calleridnum'] = clidsrc
                        if uiddst in self.keeplist[phoneiddst]['comms']:
                                self.__createorupdate_comm__(phoneiddst, uiddst, infos)
                        else:
                                infos['thischannel'] = puiddst['channel']
                                self.__createorupdate_comm__(phoneiddst, uiddst, infos)
                                log.debug('phonelist::ami_link %s not found (dst)' % (uiddst))
                        log.debug('phonelist::ami_link gruik %s %s' % (phoneiddst, self.keeplist[phoneiddst]['comms']))
                return
        
        def ami_unlink(self, phoneidsrc, phoneiddst, uidsrc, uiddst, puidsrc, puiddst):
                if phoneidsrc in self.keeplist:
                        if uidsrc in self.keeplist[phoneidsrc]['comms']:
                                infos = {'status' : 'unlinked-caller',
                                         'time-link' : 0,
                                         'timestamp-link' : time.time(),
                                         }
                                self.keeplist[phoneidsrc]['comms'][uidsrc].update(infos)
                if phoneiddst in self.keeplist:
                        if uiddst in self.keeplist[phoneiddst]['comms']:
                                infos = {'status' : 'unlinked-called',
                                         'time-link' : 0,
                                         'timestamp-link' : time.time(),
                                         }
                                self.keeplist[phoneiddst]['comms'][uiddst].update(infos)
                return
        
        def ami_rename(self, oldphoneid, newphoneid, oldname, newname, uid):
                # rename channels
                for phoneid, v in self.keeplist.iteritems():
                    for k, kk in v['comms'].iteritems():
                        if kk.get('thischannel') == oldname:
                            kk['thischannel'] = newname
                        if kk.get('peerchannel') == oldname:
                            kk['peerchannel'] = newname
                # move channels from one phone to another
                if oldphoneid and newphoneid and oldphoneid != newphoneid:
                    if uid in self.keeplist[oldphoneid]['comms'] and uid not in self.keeplist[newphoneid]['comms']:
                        self.keeplist[newphoneid]['comms'][uid] = self.keeplist[oldphoneid]['comms'][uid]
                        self.setlinenum(newphoneid, uid)
                        del self.keeplist[oldphoneid]['comms'][uid]
                    else:
                        log.warning('(ami_rename) %s : could not move from %s to %s' % (uid, oldphoneid, newphoneid))
                        #log.warning('(ami_rename) %s : %s ---- %s' % (uid, self.keeplist[oldphoneid]['comms'], self.keeplist[oldphoneid]['comms']))
                return
        
        def ami_rename_totrunk(self, oldphoneid, oldname, newname, uid):
                tomove = None
                for phoneid, v in self.keeplist.iteritems():
                        for k, kk in v['comms'].iteritems():
                                if kk.get('thischannel') == oldname:
                                        kk['thischannel'] = newname
                                if kk.get('peerchannel') == oldname:
                                        kk['peerchannel'] = newname
                if uid in self.keeplist[oldphoneid]['comms']:
                        tomove = self.keeplist[oldphoneid]['comms'][uid]
                        # do not remove the reference at once, because, the client side needs to
                        # know that the uid has been hanged-up
                        # del self.keeplist[oldphoneid]['comms'][uid]
                        self.keeplist[oldphoneid]['comms'][uid]['status'] = 'hangup'
                else:
                        log.warning('(ami_rename_totrunk) %s : could not remove %s' % (uid, oldphoneid))
                return tomove
        
        def ami_rename_fromtrunk(self, newphoneid, oldname, newname, uid, tomove):
                for phoneid, v in self.keeplist.iteritems():
                    for k, kk in v['comms'].iteritems():
                        if kk.get('thischannel') == oldname:
                            kk['thischannel'] = newname
                        if kk.get('peerchannel') == oldname:
                            kk['peerchannel'] = newname
                if tomove and uid not in self.keeplist[newphoneid]['comms']:
                    self.keeplist[newphoneid]['comms'][uid] = tomove
                    self.setlinenum(newphoneid, uid)
                else:
                    log.warning('(ami_rename_fromtrunk) %s : could not set %s' % (uid, newphoneid))
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
                        #changed = not (self.keeplist[phoneid]['hintstatus'] == self.display_hints.get(status))
                        changed = not isinstance(self.keeplist[phoneid]['hintstatus'], dict) or not (self.keeplist[phoneid]['hintstatus'].get('code') == status)
                        #log.debug('ami_extstatus : %s %s => %s' % (changed, self.keeplist[phoneid]['hintstatus'], self.display_hints.get(status)) )
                        if changed:
                            self.keeplist[phoneid]['hintstatus'] = self.display_hints.get(status)
                            self.keeplist[phoneid]['hintstatus']['code'] = status
                        return changed
                return False
        
        def ami_parkedcall(self, phoneid, uid, ctuid, exten):
                if phoneid in self.keeplist:
                        # write "parque" in unicode !
                        infos = {'status' : 'linked-caller',
                                 'time-link' : 0,
                                 'timestamp-link' : time.time(),
                                 'calleridnum' : exten,
                                 'calleridname' : '<parked>'
                                 }
                        self.__createorupdate_comm__(phoneid, uid, infos)
                return
        
        def ami_unparkedcall(self, phoneid, uid, ctuid):
                if phoneid in self.keeplist:
                        log.debug('phone::ami_unparkedcall %s %s %s' % (phoneid, uid, ctuid))
                        if uid in self.keeplist[phoneid]['comms']:
                                # parked channel
                                infos = {'status' : 'linked-called',
                                         'thischannel' : ctuid['channel'],
                                         'peerchannel' : ctuid['peerchannel'],
                                         'time-link' : 0,
                                         'timestamp-link' : time.time(),
                                         #'calleridnum' : ctuid['parkexten-callback']
                                         }
                                self.keeplist[phoneid]['comms'][uid].update(infos)
                        else:
                                # cfrom
                                infos = {'status' : 'linked-caller',
                                         'thischannel' : ctuid['channel'],
                                         'peerchannel' : ctuid['peerchannel'],
                                         'time-link' : 0,
                                         'timestamp-link' : time.time(),
                                         #'calleridnum' : ctuid['parkexten-callback']
                                         }
                                self.keeplist[phoneid]['comms'][uid] = infos
                                self.setlinenum(phoneid, uid)
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
        
        def ami_meetmejoin(self, phoneid, uid, meetmenum):
            if phoneid in self.keeplist:
                if uid in self.keeplist[phoneid]['comms']:
                    infos = { 'timestamp-link' : time.time(),
                              'calleridname' : '<meetme>',
                              'calleridnum' : meetmenum
                              }
                    self.keeplist[phoneid]['comms'][uid].update(infos)
            return
