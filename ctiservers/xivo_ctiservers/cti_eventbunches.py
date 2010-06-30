# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2010 Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, XiVO CTI Server is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XiVO CTI Server
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# kinds of events :
# - "expected in all cases", otherwise failure : post_xxx_expected
# - "multi choices" : linked or not, refused, ...
# - "distinction" between those coming directly from AMI and those from CTI server
#   after the processing of an other bunch

import logging
import re
log = logging.getLogger('eventbunches')

def match_event(astid, ami_event, event, seea):
    found = False
    if astid in seea:
        try:
            log.info('lookup %s (length %d)' % (ami_event, len(seea[astid])))
            for toexp in seea[astid]:
                g = match_one(ami_event, event, toexp)
                if g:
                    log.info('matched %s' % ami_event)
                    toexp.update(event)
                    if toexp.empty():
                        log.info('looks like a requested event did finish (%s)' % toexp.matched_event)
                        log.info('variables set then : %s' % toexp.variables)
                        found = toexp.variables
                    break
        except Exception:
            log.exception('match_event %s' % ami_event)
    if found:
        seea[astid].remove(toexp)
        if toexp.matched_event == 'Atxfer':
            # to set according to previous event
            found.update(toexp.event)
            seea[astid].append(post_ami_atxfer_answered(found))
            seea[astid].append(post_ami_atxfer_refused(found))
            seea[astid].append(post_ami_atxfer_timedout(found))
            
        elif toexp.matched_event == 'AtxferAnswered':
            found.update(toexp.event)
            # XXX : remove 'refused' + 'timedout'
            seea[astid].append(post_ami_atxfer_linked(found))
            seea[astid].append(post_ami_atxfer_denied(found))
        elif toexp.matched_event == 'AtxferRefused':
            found.update(toexp.event)
            # XXX : remove 'answered' + 'timedout'
        elif toexp.matched_event == 'AtxferTimedOut':
            found.update(toexp.event)
            # XXX : remove 'refused' + 'answered'
            
        elif toexp.matched_event == 'AtxferLinked':
            found.update(toexp.event)
            # XXX : remove 'denied'
        elif toexp.matched_event == 'AtxferDenied':
            found.update(toexp.event)
            # XXX : remove 'linked'
    return False

def match_one(ami_event, event, toexp):
    match = False
    matched_event = toexp.matched_event
    try:
        if toexp.expected[0].get('Event') == ami_event:
            match = True
            for k, v in toexp.expected[0].iteritems():
                if k != 'Event' and v is not None:
                    if isinstance(v, tuple):
                        # the syntax for event variables values is, for 'Channel', for instance :
                        # 'Channel' : ('varfield', 'string', re.compile('Local/%s@%s-.*,1' % (exten, context)))
                        # i.e. (key, string value, regexep)
                        # - each of these can be None, and if not set, are treated like if they were so
                        # - if the first is defined, it's the key of a variable that will be set later on
                        # - if the 3rd (regexp) is defined, the value will be checked against it
                        # - if the 2nd (string) is defined, the value will be checked against it
                        var_toset = v[0]
                        if len(v) > 1:
                            string_tomatch = v[1]
                            if len(v) > 2:
                                re_tomatch = v[2]
                            else:
                                re_tomatch = None
                        else:
                            string_tomatch = None
                            re_tomatch = None
                            
                        eventvarvalue = event.get(k)
                        matched_string = None
                        if re_tomatch:
                            vmatch = re_tomatch.match(eventvarvalue)
                            if vmatch:
                                matched_string = vmatch.group()
                            else:
                                match = False
                                break
                        elif string_tomatch:
                            if string_tomatch == eventvarvalue:
                                matched_string = eventvarvalue
                            else:
                                match = False
                                break
                        else:
                            # this is the case of variables that we just want to get
                            # ('uidN', None, None) cases
                            matched_string = eventvarvalue
                        
                        if var_toset and matched_string:
                            toexp.variables[var_toset] = matched_string
                    else:
                        log.warning('match_one : bad compliance %s (%s)' % (ami_event, matched_event))
    except Exception:
        log.exception('match_one : %s (%s)' % (ami_event, matched_event))
    return match

class post_events():
    def __init__(self):
        self.consumed = []
        self.received = []
        self.variables = {}
        self.currentposition = 0
        return
    
    def update(self, event):
        self.consumed.append(self.expected.pop(0))
        self.received.append(event)
        return
    
    def empty(self):
        return (self.expected == [])

class post_ami_atxfer(post_events):
    matched_event = 'Atxfer'
    def __init__(self, event):
        post_events.__init__(self)
        channel = event.get('Channel')
        exten = event.get('Exten')
        context = event.get('Context')
        # self.consumedindex
        
        self.event = event
        self.expected = [ { 'Event' : 'Unlink',
                            'Channel2' : ('tech_std', channel),
                            'Channel1' : ('tech_orig',),
                            'Uniqueid2' : ('uid2',),
                            'Uniqueid1' : ('uid1',),
                            },
                          { 'Event' : 'Newchannel',
                            'Channel' : ('local1', None, re.compile('Local/%s@%s-.*,1' % (exten, context))),
                            'Uniqueid' : ('uid3',),
                            },
                          { 'Event' : 'Newchannel',
                            'Channel' : ('local2', None, re.compile('Local/%s@%s-.*,2' % (exten, context))),
                            'Uniqueid' : ('uid4',),
                            },
                          { 'Event' : 'Newchannel',
                            'CallerIDNum' : (None, exten),
                            'Uniqueid' : ('uid5',),
                            'Channel' : ('tech_dest',),
                            },
                          { 'Event' : 'Dial',
                            'Source' : ('local2bis', None, re.compile('Local/%s@%s-.*,2' % (exten, context))),
                            }
                          ]
        return

##            ph1['Unlink'] = {'Uniqueid2': ${UID2}, 'Uniqueid1': ${UID1},
##                             'CallerID2': '1178', 'CallerID1': '1431',
##                             'CallerIDName2': ${CIDNB}, 'CallerIDName1': ${CIDNH},
##                             'Channel2': ${TECH_STD}, 'Channel1': ${SIPH1},
##                             'Where': 'loop'}
##            ph1['Newchannel'] = {'CallerIDNum': '<unknown>', 'State': 'Down',
##                                 'Uniqueid': ${UID3}, 'CallerIDName': '<unknown>',
##                                 'Channel': ${LOCAL1}}
##            ph1['Newchannel'] = {'CallerIDNum': '<unknown>', 'State': 'Ring', 'Uniqueid': ${UID4}, 'CallerIDName': '<unknown>', 'Channel': ${LOCAL2}}
##            Newchannel {'CallerIDNum': '1647', 'State': 'Down', 'Uniqueid': ${UID5}, 'CallerIDName': ${CIDNZ}, 'Channel': ${TECH_DEST}}
##            Dial {'CallerID': '1178', 'SrcUniqueID': ${UID4}, 'Destination': ${TECH_DEST}, 'DestUniqueID': ${UID5}, 'Source': ${LOCAL2}, 'CallerIDName': ${CIDNB}, 'Data': 'SIP/zlosfbpeajsxrn|30|'}

class post_ami_atxfer_answered(post_events):
    matched_event = 'AtxferAnswered'
    def __init__(self, event):
        post_events.__init__(self)
        
        local1 = event.get('local1')
        local2 = event.get('local2')
        uid2 = event.get('uid2')
        uid3 = event.get('uid3')
        uid4 = event.get('uid4')
        uid5 = event.get('uid5')
        tech_std = event.get('tech_std')
        tech_dest = event.get('tech_dest')
        tech_destm = tech_dest + '<MASQ>'
        local1z = local1 + '<ZOMBIE>'
        
        self.event = event
        self.expected = [ { 'Event' : 'Link',
                            'Channel1' : (None, local2),
                            'Uniqueid1' : (None, uid4),
                            'Channel2' : (None, tech_dest),
                            'Uniqueid2' : (None, uid5),
                            },
                          { 'Event' : 'Atxfer',
                            'SrcChannel' : ('tech_std',), # (None, tech_std),
                            'SrcUniqueid' : ('uid2',), # (None, uid2),
                            'DstUniqueid' : (None, uid3),
                            'DstChannel' : (None, local1),
                            },
                          { 'Event' : 'Link',
                            'Uniqueid2' : (None, uid3),
                            'Uniqueid1' : ('uid2',), # (None, uid2),
                            'Channel2' : (None, local1),
                            'Channel1' : ('tech_std',), # (None, tech_std),
                            },
                          { 'Event' : 'Unlink',
                            'Uniqueid2' : (None, uid3),
                            'Uniqueid1' : ('uid2',), # (None, uid2),
                            'Channel2' : (None, local1),
                            'Channel1' : ('tech_std',), # (None, tech_std),
                            },
                          { 'Event' : 'Link',
                            'Uniqueid2' : (None, uid3),
                            'Uniqueid1' : ('uid2',), # (None, uid2),
                            'Channel2' : (None, local1),
                            'Channel1' : ('tech_std',), # (None, tech_std),
                            },
                          { 'Event' : 'Masquerade',
                            'Original' : (None, local1),
                            'Clone' : (None, tech_dest),
                            'OriginalState' : (None, 'Up'),
                            'CloneState' : (None, 'Up'),
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, tech_destm),
                            'Oldname' : (None, tech_dest),
                            'Uniqueid' : (None, uid5),
                            'Where' : (None, 'ToMasq'),
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, tech_dest),
                            'Oldname' : (None, local1),
                            'Uniqueid' : (None, uid3),
                            'Where' : (None, 'New'),
                            },
                          { 'Event' : 'HangupRequest',
                            'Channel' : (None, local2),
                            'Uniqueid' : (None, uid4),
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, local1z),
                            'Oldname' : (None, tech_destm),
                            'Uniqueid' : (None, uid5),
                            'Where' : (None, 'Zombie')
                            },
                          { 'Event' : 'Unlink',
                            'Uniqueid2' : (None, uid5),
                            'Uniqueid1' : (None, uid4),
                            'Channel2' : (None, local1z),
                            'Channel1' : (None, local2),
                            'Where' : (None, 'loopisover')
                            },
                          { 'Event' : 'Hangup',
                            'Uniqueid' : (None, uid5),
                            # 'Cause': None,
                            'Channel' : (None, local1z)
                            },
                          { 'Event' : 'Hangup',
                            'Uniqueid' : (None, uid4),
                            # 'Cause': None,
                            'Channel' : (None, local2)
                            }
                          ]
        return

class post_ami_atxfer_refused(post_events):
    matched_event = 'AtxferRefused'
    def __init__(self, event):
        post_events.__init__(self)
        
        local1 = event.get('local1')
        uid2 = event.get('uid2')
        uid3 = event.get('uid3')
        uid5 = event.get('uid5')
        tech_std = event.get('tech_std')
        tech_dest = event.get('tech_dest')
        
        self.event = event
        self.expected = [ { 'Event' : 'Hangup',
                            'Channel' : (None, tech_dest),
                            'Uniqueid' : (None, uid5),
                            'Cause': (None, '21'),
                            },
                          { 'Event' : 'Atxfer',
                            'SrcChannel' : (None, tech_std),
                            'SrcUniqueid' : (None, uid2),
                            'DstChannel' : (None, local1),
                            'DstUniqueid' : (None, uid3),
                            },
                          { 'Event' : 'Link',
                            'Channel1' : (None, tech_std),
                            'Uniqueid1' : (None, uid2),
                            'Channel2' : (None, local1),
                            'Uniqueid2' : (None, uid3),
                            },
                          { 'Event' : 'Unlink',
                            'Channel1' : (None, tech_std),
                            'Uniqueid1' : (None, uid2),
                            'Channel2' : (None, local1),
                            'Uniqueid2' : (None, uid3),
                            },
                          { 'Event' : 'Link',
                            'Channel1' : (None, tech_std),
                            'Uniqueid1' : (None, uid2),
                            'Channel2' : (None, local1),
                            'Uniqueid2' : (None, uid3),
                            }
                          ]
        return

class post_ami_atxfer_timedout(post_events):
    matched_event = 'AtxferTimedOut'
    def __init__(self, event):
        post_events.__init__(self)
        
        local1 = event.get('local1')
        local2 = event.get('local2')
        uid1 = event.get('uid1')
        uid2 = event.get('uid2')
        uid3 = event.get('uid3')
        uid4 = event.get('uid4')
        uid5 = event.get('uid5')
        tech_orig = event.get('tech_orig')
        tech_std = event.get('tech_std')
        tech_dest = event.get('tech_dest')
        
        self.event = event
        self.expected = [ { 'Event' : 'HangupRequest',
                            'Channel' : (None, local2),
                            'Uniqueid' : (None, uid4),
                            },
                          { 'Event' : 'Hangup',
                            'Channel' : (None, local1),
                            'Uniqueid' : (None, uid3),
                            },
                          { 'Event' : 'Hangup',
                            'Channel' : (None, tech_dest),
                            'Uniqueid' : (None, uid5),
                            },
                          { 'Event' : 'Hangup',
                            'Channel' : (None, local2),
                            'Uniqueid' : (None, uid4),
                            },
                          { 'Event' : 'Link',
                            'Channel1' : (None, tech_orig),
                            'Uniqueid1' : (None, uid1),
                            'Channel2' : (None, tech_std),
                            'Uniqueid2' : (None, uid2),
                            }
                          ]
        return

class post_ami_atxfer_denied(post_events):
    matched_event = 'AtxferDenied'
    def __init__(self, event):
        post_events.__init__(self)
        
        uid1 = event.get('uid1')
        uid2 = event.get('uid2')
        uid3 = event.get('uid3')
        tech_orig = event.get('tech_orig')
        tech_std = event.get('tech_std')
        tech_dest = event.get('tech_dest')
        
        self.event = event
        self.expected = [ { 'Event' : 'Unlink',
                            'Channel1' : (None, tech_std),
                            'Uniqueid1' : (None, uid2),
                            'Channel2' : (None, tech_dest),
                            'Uniqueid2' : (None, uid3),
                            },
                          { 'Event' : 'Hangup',
                            'Channel' : (None, tech_dest),
                            'Uniqueid' : (None, uid3),
                            },
                          { 'Event' : 'Link',
                            'Channel1' : (None, tech_orig),
                            'Uniqueid1' : (None, uid1),
                            'Channel2' : (None, tech_std),
                            'Uniqueid2' : (None, uid2),
                            }
                          ]
        return

class post_ami_atxfer_linked(post_events):
    matched_event = 'AtxferLinked'
    def __init__(self, event):
        post_events.__init__(self)
        
        uid1 = event.get('uid1')
        uid2 = event.get('uid2')
        uid3 = event.get('uid3')
        
        tech_dest = event.get('tech_dest')
        tech_std = event.get('tech_std')
        tech_orig = event.get('tech_orig')
        tech_origm = tech_orig + '<MASQ>'
        tech_origt = 'Transfered/' + tech_orig
        tech_origtz = tech_origt + '<ZOMBIE>'
        
        self.event = event
        self.expected = [ { 'Event' : 'Unlink',
                            'Uniqueid2': (None, uid3),
                            'Uniqueid1': (None, uid2),
                            'Channel2': (None, tech_dest),
                            'Channel1': (None, tech_std)
                            },
                          { 'Event' : 'Masquerade',
                            'Original' : (None, tech_origt),
                            'Clone' : (None, tech_orig),
                            'OriginalState' : (None, 'Up'),
                            'CloneState' : (None, 'Up'),
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, tech_origm),
                            'Oldname' : (None, tech_orig),
                            'Uniqueid' : (None, uid1),
                            'Where' : (None, 'ToMasq'),
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, tech_orig),
                            'Oldname' : (None, tech_origt),
                            'Uniqueid' : ('uid6',),
                            'Where' : (None, 'New'),
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, tech_origtz),
                            'Oldname' : (None, tech_origm),
                            'Uniqueid' : (None, uid1),
                            'Where' : (None, 'Zombie'),
                            },
                          { 'Event' : 'Link',
                            'Uniqueid2' : (None, uid3),
                            'Uniqueid1' : ('uid6bis',),
                            'Channel2' : (None, tech_dest),
                            'Channel1' : (None, tech_orig),
                            },
                          { 'Event' : 'Hangup',
                            'Uniqueid': (None, uid2),
                            # 'Cause': None,
                            'Channel': (None, tech_std)
                            },
                          { 'Event' : 'Hangup',
                            'Uniqueid': (None, uid1),
                            # 'Cause': None,
                            'Channel': (None, tech_origtz)
                            }
                          ]
        return
