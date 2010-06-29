# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2010 Proformatique'
__author__    = 'Thomas Bernard'

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
                if match_one(ami_event, event, toexp):
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
        elif toexp.matched_event == 'AtxferAnswered':
            found.update(toexp.event)
            seea[astid].append(post_ami_atxfer_linked(found))
        elif toexp.matched_event == 'AtxferLinked':
            pass
    return False

def match_one(ami_event, event, toexp):
    match = False
    try:
        if toexp.expected[0].get('Event') == ami_event:
            match = True
            for k, v in toexp.expected[0].iteritems():
                if k != 'Event' and v is not None:
                    if isinstance(v, tuple) and len(v) == 2:
                        var_toset = v[0]
                        re_tomatch = v[1]
                        
                        matched_string = None
                        if re_tomatch:
                            vmatch = re_tomatch.match(event.get(k))
                            if not vmatch:
                                match = False
                                break
                            else:
                                matched_string = vmatch.group()
                        else:
                            # this is the case of variables that we just want to get
                            pass
                        
                        if var_toset:
                            if matched_string:
                                toexp.variables[var_toset] = matched_string
                            else:
                                toexp.variables[var_toset] = event.get(k)
                    else:
                        log.warning('match_one : bad compliance %s' % ami_event)
    except Exception:
        log.exception('match_one : %s' % ami_event)
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
                            'Channel2' : (None, re.compile(channel)),
                            'Channel1' : ('tech_orig', None),
                            'Uniqueid2' : (None, re.compile(uid2)),
                            'Uniqueid1' : (None, re.compile(uid1)),
                            },
                          { 'Event' : 'Newchannel',
                            'Channel' : ('local1', re.compile('Local/%s@%s-.*,1' % (exten, context))),
                            'Uniqueid' : ('uid3', None),
                            },
                          { 'Event' : 'Newchannel',
                            'Channel' : ('local2', re.compile('Local/%s@%s-.*,2' % (exten, context))),
                            'Uniqueid' : ('uid4', None),
                            },
                          { 'Event' : 'Newchannel',
                            'CallerIDNum' : (None, re.compile(exten)),
                            'Uniqueid' : ('uid5', None),
                            'Channel' : ('tech_dest', None),
                            },
                          { 'Event' : 'Dial',
                            'Source' : ('local2bis', re.compile('Local/%s@%s-.*,2' % (exten, context))),
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
        
        tech_dest = event.get('tech_dest')
        local1 = event.get('local1')
        local2 = event.get('local2')
        uid3 = event.get('uid3')
        uid4 = event.get('uid4')
        uid5 = event.get('uid5')
        
        uid2 = event.get('uid2')
        tech_std = event.get('tech_std')
        
        tech_destm = tech_dest + '<MASQ>'
        local1z = local1 + '<ZOMBIE>'
        
        self.event = event
        self.expected = [ { 'Event' : 'Link',
                            'Uniqueid2' : (None, re.compile(uid5)),
                            'Uniqueid1' : (None, re.compile(uid4)),
                            'Channel2' : (None, re.compile(tech_dest)),
                            'Channel1' : (None, re.compile(local2))
                            },
                          { 'Event' : 'Atxfer',
                            'SrcChannel' : ('tech_std', None), # (None, re.compile(tech_std)),
                            'SrcUniqueid' : ('uid2', None), # (None, re.compile(uid2)),
                            'DstUniqueid' : (None, re.compile(uid3)),
                            'DstChannel' : (None, re.compile(local1))
                            },
                          { 'Event' : 'Link',
                            'Uniqueid2' : (None, re.compile(uid3)),
                            'Uniqueid1' : ('uid2', None), # (None, re.compile(uid2)),
                            'Channel2' : (None, re.compile(local1)),
                            'Channel1' : ('tech_std', None), # (None, re.compile(tech_std)),
                            },
                          { 'Event' : 'Unlink',
                            'Uniqueid2' : (None, re.compile(uid3)),
                            'Uniqueid1' : ('uid2', None), # (None, re.compile(uid2)),
                            'Channel2' : (None, re.compile(local1)),
                            'Channel1' : ('tech_std', None), # (None, re.compile(tech_std)),
                            },
                          { 'Event' : 'Link',
                            'Uniqueid2' : (None, re.compile(uid3)),
                            'Uniqueid1' : ('uid2', None), # (None, re.compile(uid2)),
                            'Channel2' : (None, re.compile(local1)),
                            'Channel1' : ('tech_std', None), # (None, re.compile(tech_std)),
                            },
                          { 'Event' : 'Masquerade',
                            'Original' : (None, re.compile(local1)),
                            'Clone' : (None, re.compile(tech_dest)),
                            'OriginalState' : (None, re.compile('Up')),
                            'CloneState' : (None, re.compile('Up'))
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, re.compile(tech_destm)),
                            'Oldname' : (None, re.compile(tech_dest)),
                            'Uniqueid' : (None, re.compile(uid5)),
                            'Where' : (None, re.compile('ToMasq'))
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, re.compile(tech_dest)),
                            'Oldname' : (None, re.compile(local1)),
                            'Uniqueid' : (None, re.compile(uid3)),
                            'Where' : (None, re.compile('New'))
                            },
                          { 'Event' : 'HangupRequest',
                            'Channel' : (None, re.compile(local2)),
                            'Uniqueid' : (None, re.compile(uid4))
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, re.compile(local1z)),
                            'Oldname' : (None, re.compile(tech_destm)),
                            'Uniqueid' : (None, re.compile(uid5)),
                            'Where' : (None, re.compile('Zombie'))
                            },
                          { 'Event' : 'Unlink',
                            'Uniqueid2' : (None, re.compile(uid5)),
                            'Uniqueid1' : (None, re.compile(uid4)),
                            'Channel2' : (None, re.compile(local1z)),
                            'Channel1' : (None, re.compile(local2)),
                            'Where' : (None, re.compile('loopisover'))
                            },
                          { 'Event' : 'Hangup',
                            'Uniqueid' : (None, re.compile(uid5)),
                            # 'Cause': None,
                            'Channel' : (None, re.compile(local1z))
                            },
                          { 'Event' : 'Hangup',
                            'Uniqueid' : (None, re.compile(uid4)),
                            # 'Cause': None,
                            'Channel' : (None, re.compile(local2))
                            }
                          ]

class post_ami_atxfer_linked(post_events):
    matched_event = 'AtxferLinked'
    def __init__(self, event):
        post_events.__init__(self)
        
        uid1 = event.get('uid1')
        uid2 = event.get('uid2')
        uid3 = event.get('uid3')
        uid6 = event.get('uid6')
        
        tech_dest = event.get('tech_dest')
        tech_std = event.get('tech_std')
        tech_orig = event.get('tech_orig')
        tech_origm = tech_orig + '<MASQ>'
        tech_origt = 'Transfered/' + tech_orig
        tech_origtz = tech_origt + '<ZOMBIE>'
        
        self.event = event
        self.expected = [ { 'Event' : 'Unlink',
                            'Uniqueid2': (None, re.compile(uid3)),
                            'Uniqueid1': (None, re.compile(uid2)),
                            'Channel2': (None, re.compile(tech_dest)),
                            'Channel1': (None, re.compile(tech_std))
                            },
                          { 'Event' : 'Masquerade',
                            'Original' : (None, re.compile(tech_origt)),
                            'Clone' : (None, re.compile(tech_orig)),
                            'OriginalState' : (None, re.compile('Up')),
                            'CloneState' : (None, re.compile('Up')),
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, re.compile(tech_origm)),
                            'Oldname' : (None, re.compile(tech_orig)),
                            'Uniqueid' : (None, re.compile(uid1)),
                            'Where' : (None, re.compile('ToMasq')),
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, re.compile(tech_orig)),
                            'Oldname' : (None, re.compile(tech_origt)),
                            'Uniqueid' : (None, re.compile(uid6)),
                            'Where' : (None, re.compile('New')),
                            },
                          { 'Event' : 'Rename',
                            'Newname' : (None, re.compile(tech_origtz)),
                            'Oldname' : (None, re.compile(tech_origm)),
                            'Uniqueid' : (None, re.compile(uid1)),
                            'Where' : (None, re.compile('Zombie')),
                            },
                          { 'Event' : 'Link',
                            'Uniqueid2' : (None, re.compile(uid3)),
                            'Uniqueid1' : (None, re.compile(uid6)),
                            'Channel2' : (None, re.compile(tech_dest)),
                            'Channel1' : (None, re.compile(tech_orig)),
                            },
                          { 'Event' : 'Hangup',
                            'Uniqueid': (None, re.compile(uid2)),
                            # 'Cause': None,
                            'Channel': (None, re.compile(tech_std))
                            },
                          { 'Event' : 'Hangup',
                            'Uniqueid': (None, re.compile(uid1)),
                            # 'Cause': None,
                            'Channel': (None, re.compile(tech_origtz))
                            }
                          ]
