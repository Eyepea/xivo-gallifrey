"""Object classes for XIVO AGI

Copyright (C) 2007, 2008  Proformatique

This module provides a set of objects that are used by several AGI scripts
in XIVO.

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

import re


class DBUpdateException(Exception):
    pass


class FeatureList:
    def __init__(self, agi, cursor):
        cursor.query("SELECT ${columns} FROM extensions "
                     'WHERE name IN ("fwdunc", "fwdrna", "fwdbusy", "enablevm", "incallfilter", "incallrec", "enablednd") '
                     "AND commented = 0",
                     ('name',))
        res = cursor.fetchall()

        if not res:
            raise LookupError("Unable to find features in table extensions")

        enabled_features = set([row['name'] for row in res])

        self.fwdunc = 'fwdunc' in enabled_features
        self.fwdrna = 'fwdrna' in enabled_features
        self.fwdbusy = 'fwdbusy' in enabled_features
        self.enablevm = 'enablevm' in enabled_features
        self.incallfilter = 'incallfilter' in enabled_features
        self.incallrec = 'incallrec' in enabled_features
        self.enablednd = 'enablednd' in enabled_features


class BossSecretaryFilterMember:
    """This class represents a boss-secretary filter member (e.g. a boss
    or a secretary).

    """

    def __init__(self, agi, active, xtype, xid, number, ringseconds):
        self.agi = agi
        self.active = bool(active)
        self.type = xtype
        self.id = xid
        self.number = number
        self.interface = None

        if ringseconds == 0:
            self.ringseconds = ""
        else:
            self.ringseconds = ringseconds

    def __str__(self):
        return ("Call filter member object :\n"
                "Type:        %s\n"
                "User ID:     %s\n"
                "Number:      %s\n"
                "Interface:   %s\n"
                "RingSeconds: %s"
                % (self.type, self.id, self.number, self.interface, self.ringseconds))

    def agi_str(self):
        s = str(self)

        for line in s.splitlines():
            self.agi.verbose(line)


# TODO: refactor this.
class BossSecretaryFilter:
    """Boss-secretary filter class. Creating a boss-secretary filter
    automatically load everything related to the filter (its properties,
    those of its boss, its secretaries). Creating a filter is also a way
    to check its existence. Trying to construct a filter that doesn't
    exist or has no secretary raises a LookupError.

    """

    def __init__(self, agi, cursor, boss_number, boss_context):
        self.agi = agi
        self.cursor = cursor
        self.id = None
        self.active = False
        self.context = None
        self.mode = None
        self.callfrom = None
        self.ringseconds = None
        self.boss = None
        self.secretaries = None

        cursor.query("SELECT ${columns} FROM callfilter "
                     "INNER JOIN callfiltermember "
                     "ON callfilter.id = callfiltermember.callfilterid "
                     "INNER JOIN userfeatures "
                     "ON callfiltermember.typeval = userfeatures.id "
                     "WHERE callfilter.type = 'bosssecretary' "
                     "AND callfilter.commented = 0 "
                     "AND callfiltermember.type = 'user' "
                     "AND callfiltermember.bstype = 'boss' "
                     "AND userfeatures.number = %s "
                     "AND userfeatures.context = %s "
                     "AND userfeatures.internal = 0 "
                     "AND userfeatures.bsfilter = 'boss' "
                     "AND userfeatures.commented = 0",
                     ('callfilter.id', 'callfilter.bosssecretary',
                      'callfilter.callfrom', 'callfilter.ringseconds',
                      'callfiltermember.ringseconds',
                      'userfeatures.id', 'userfeatures.protocol',
                      'userfeatures.protocolid', 'userfeatures.name'),
                     (boss_number, boss_context))
        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find call filter ID for boss (number = '%s', context = '%s')" % (boss_number, boss_context))

        protocol = res['userfeatures.protocol']
        protocolid = res['userfeatures.protocolid']
        name = res['userfeatures.name']

        self.id = res['callfilter.id']
        self.context = boss_context
        self.mode = res['callfilter.bosssecretary']
        self.callfrom = res['callfilter.callfrom']
        self.ringseconds = res['callfilter.ringseconds']
        self.boss = BossSecretaryFilterMember(self.agi, True, 'boss', res['userfeatures.id'],
                                              boss_number, res['callfiltermember.ringseconds'])
        self.secretaries = []

        if self.ringseconds == 0:
            self.ringseconds = ""

        try:
            self.boss.interface = protocol_intf_and_suffix(cursor, protocol, 'user', protocolid)[0]
        except (ValueError, LookupError), e:
            self.agi.dp_break(str(e))

        cursor.query("SELECT ${columns} FROM callfiltermember INNER JOIN userfeatures "
                     "ON callfiltermember.typeval = userfeatures.id "
                     "WHERE callfiltermember.callfilterid = %s "
                     "AND callfiltermember.type = 'user' "
                     "AND callfiltermember.bstype = 'secretary' "
                     "AND IFNULL(userfeatures.number,'') != '' "
                     "AND userfeatures.context = %s "
                     "AND userfeatures.internal = 0 "
                     "AND userfeatures.bsfilter = 'secretary' "
                     "AND userfeatures.commented = 0 "
                     "ORDER BY priority ASC",
                     ('callfiltermember.active', 'userfeatures.id', 'userfeatures.protocol',
                      'userfeatures.protocolid', 'userfeatures.name', 'userfeatures.number',
                      'userfeatures.ringseconds'),
                     (self.id, boss_context))
        res = cursor.fetchall()

        if not res:
            raise LookupError("Unable to find secretaries for call filter ID %d (context = %r)" % (self.id, boss_context))

        for row in res:
            protocol = row['userfeatures.protocol']
            protocolid = row['userfeatures.protocolid']
            name = row['userfeatures.name']
            secretary = BossSecretaryFilterMember(self.agi,
                                                  row['callfiltermember.active'],
                                                  'secretary',
                                                  row['userfeatures.id'],
                                                  row['userfeatures.number'],
                                                  row['userfeatures.ringseconds'])

            if secretary.active:
                self.active = True

            try:
                secretary.interface = protocol_intf_and_suffix(cursor, protocol, 'user', protocolid)[0]
            except (ValueError, LookupError), e:
                self.agi.dp_break(str(e))

            self.secretaries.append(secretary)

    def __str__(self):
        return ("Call filter object :\n"
                "Context:       %s\n"
                "Mode:          %s\n"
                "Callfrom:      %s\n"
                "RingSeconds:   %s\n"
                "Boss:\n%s\n"
                "Secretaries:\n%s"
                % (self.context, self.mode, self.callfrom,
                   self.ringseconds, self.boss, '\n'.join((str(secretary) for secretary in self.secretaries))))

    def agi_str(self):
        s = str(self)

        for line in s.splitlines():
            self.agi.verbose(line)

    def check_zone(self, zone):
        if self.callfrom == "all":
            return True
        elif self.callfrom == "internal" and zone == "intern":
            return True
        elif self.callfrom == "external" and zone == "extern":
            return True
        else:
            return False

    def get_secretary_by_number(self, number, context=None):
        if context and context != self.context:
            return None

        for secretary in self.secretaries:
            if number == secretary.number:
                return secretary
        else:
            return None

    def get_secretary_by_id(self, xid):
        for secretary in self.secretaries:
            if xid == secretary.id:
                return secretary
        else:
            return None

    def set_dial_actions(self):
        DialAction(self.agi, self.cursor, "noanswer", "callfilter", self.id).set_variables()

    def set_caller_id(self):
        CallerID(self.agi, self.cursor, "callfilter", self.id).set_caller_id(force_rewrite=True)


class VMBox:
    def __init__(self, agi, cursor, xid):
        self.agi = agi
        self.cursor = cursor

        vm_columns = ('mailbox', 'context', 'email')
        vmf_columns = ('skipcheckpass',)
        columns = ["voicemail." + c for c in vm_columns] + ["voicemailfeatures." + c for c in vmf_columns]

        cursor.query("SELECT ${columns} FROM voicemail "
                     "INNER JOIN voicemailfeatures "
                     "ON voicemail.uniqueid = voicemailfeatures.id "
                     "WHERE voicemail.uniqueid = %s "
                     "AND voicemail.commented = 0",
                     columns,
                     (xid,))
        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find voicemail box (id: %d)" % xid)

        self.id = xid
        self.mailbox = res['voicemail.mailbox']
        self.context = res['voicemail.context']
        self.email = res['voicemail.email']
        self.skipcheckpass = res['voicemailfeatures.skipcheckpass']


class User:
    def __init__(self, agi, cursor, xid, feature_list=None):
        self.agi = agi
        self.cursor = cursor

        cursor.query("SELECT ${columns} FROM userfeatures "
                     "WHERE id = %s "
                     "AND internal = 0 "
                     "AND commented = 0",
                     ('id', 'number', 'context', 'protocol', 'protocolid',
                      'name', 'ringseconds', 'simultcalls',
                      'enablevoicemail', 'voicemailid', 'enablexfer',
                      'enableautomon', 'callrecord', 'callfilter',
                      'enablednd', 'enableunc', 'destunc', 'enablerna',
                      'destrna', 'enablebusy', 'destbusy', 'musiconhold',
                      'outcallerid', 'bsfilter'),
                     (xid,))

        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find user (id: %d)" % xid)

        self.id = res['id']
        self.number = res['number']
        self.context = res['context']
        self.protocol = res['protocol']
        self.protocolid = res['protocolid']
        self.name = res['name']
        self.ringseconds = int(res['ringseconds'])
        self.simultcalls = res['simultcalls']
        self.enablevoicemail = res['enablevoicemail']
        self.voicemailid = res['voicemailid']
        self.enablexfer = res['enablexfer']
        self.enableautomon = res['enableautomon']
        self.callrecord = res['callrecord']
        self.callfilter = res['callfilter']
        self.enablednd = res['enablednd']
        self.enableunc = res['enableunc']
        self.destunc = res['destunc']
        self.enablerna = res['enablerna']
        self.destrna = res['destrna']
        self.enablebusy = res['enablebusy']
        self.destbusy = res['destbusy']
        self.musiconhold = res['musiconhold']
        self.outcallerid = res['outcallerid']
        bsfilter = res['bsfilter']

        if self.destunc == '':
            self.enableunc = 0

        if self.destrna == '':
            self.enablerna = 0

        if self.destbusy == '':
            self.enablebusy = 0

        self.interface = protocol_intf_and_suffix(cursor, self.protocol, 'user', self.protocolid)[0]

        if bsfilter == "boss":
            try:
                self.filter = BossSecretaryFilter(agi, cursor, self.number, self.context)
            except LookupError:
                self.filter = None
        else:
            self.filter = None

        if not feature_list:
            feature_list = FeatureList(agi, cursor)

        if feature_list.enablevm and self.enablevoicemail and self.voicemailid:
            try:
                self.vmbox = VMBox(agi, cursor, self.voicemailid)
            except LookupError:
                self.vmbox = None
        else:
            self.vmbox = None

    # XXX Database and object not updated consistently.
    def reset(self):
        self.enableunc = False
        self.destunc = ""
        self.enablerna = False
        self.destrna = ""
        self.enablebusy = False
        self.destbusy = ""

        self.cursor.query("UPDATE userfeatures "
                          "SET enableunc = 0, "
                          "    destunc = '', "
                          "    enablerna = 0, "
                          "    destrna = '', "
                          "    enablebusy = 0, "
                          "    destbusy = '' "
                          "WHERE id = %s",
                          parameters = (self.id,))

        if self.cursor.rowcount != 1:
            raise DBUpdateException("Unable to perform the requested update")

    # XXX Database and object not updated consistently.
    def set_feature(self, feature, enabled, arg):
        enabled = int(bool(enabled))

        if enabled:
            dest = arg
        else:
            dest = ""

        if feature == "unc":
            self.enableunc = enabled
            self.destunc = dest
        elif feature == "rna":
            self.enablerna = enabled
            self.destrna = dest
        elif feature == "busy":
            self.enablebusy = enabled
            self.destbusy = dest
        else:
            raise ValueError("invalid feature")

        self.cursor.query("UPDATE userfeatures "
                          "SET enable%s = %%d, "
                          "    dest%s = %%s "
                          "WHERE id = %%d" % (feature, feature),
                          parameters = (enabled, dest, self.id))

        if self.cursor.rowcount != 1:
            raise DBUpdateException("Unable to perform the requested update")

    def toggle_feature(self, feature):
        toggle = lambda x: int(not x)

        if feature == "vm":
            self.enablevoicemail = toggle(self.enablevoicemail)
            enabled = self.enablevoicemail
            feature = "enablevoicemail"
        elif feature == "dnd":
            self.enablednd = toggle(self.enablednd)
            enabled = self.enablednd
            feature = "enablednd"
        elif feature == "callrecord":
            self.callrecord = toggle(self.callrecord)
            enabled = self.callrecord
        elif feature == "callfilter":
            self.callfilter = toggle(self.callfilter)
            enabled = self.callfilter
        else:
            raise ValueError("invalid feature")

        self.cursor.query("UPDATE userfeatures "
                          "SET %s = %%d "
                          "WHERE id = %%d" % feature,
                          parameters = (enabled, self.id))

        if self.cursor.rowcount != 1:
            raise DBUpdateException("Unable to perform the requested update")

        return enabled


class Group:
    def __init__(self, agi, cursor, xid=None, number=None, context=None):
        self.agi = agi
        self.cursor = cursor

        groupfeatures_columns = ('id', 'number', 'context', 'name',
                                 'timeout', 'transfer_user',
                                 'transfer_call', 'write_caller', 'write_calling')
        queue_columns = ('musiconhold',)
        columns = ["groupfeatures." + c for c in groupfeatures_columns] + ["queue." + c for c in queue_columns]

        if xid:
            cursor.query("SELECT ${columns} FROM groupfeatures "
                         "INNER JOIN queue "
                         "ON groupfeatures.name = queue.name "
                         "WHERE groupfeatures.id = %s "
                         "AND groupfeatures.deleted = 0 "
                         "AND queue.category = 'group' "
                         "AND queue.commented = 0",
                         columns,
                         (xid,))
        elif number and context:
            cursor.query("SELECT ${columns} FROM groupfeatures "
                         "INNER JOIN queue "
                         "ON groupfeatures.name = queue.name "
                         "WHERE groupfeatures.number = %s "
                         "AND groupfeatures.context = %s "
                         "AND groupfeatures.deleted = 0 "
                         "AND queue.category = 'group' "
                         "AND queue.commented = 0",
                         columns,
                         (number, context))
        else:
            raise LookupError("id or number@context must be provided to look up a group")

        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find group (id: %s, number: %s, context: %s)" % (xid, number, context))

        self.id = res['groupfeatures.id']
        self.number = res['groupfeatures.number']
        self.context = res['groupfeatures.context']
        self.name = res['groupfeatures.name']
        self.timeout = res['groupfeatures.timeout']
        self.transfer_user = res['groupfeatures.transfer_user']
        self.transfer_call = res['groupfeatures.transfer_call']
        self.write_caller = res['groupfeatures.write_caller']
        self.write_calling = res['groupfeatures.write_calling']
        self.musiconhold = res['queue.musiconhold']

    def set_dial_actions(self):
        for event in ('noanswer', 'congestion', 'busy', 'chanunavail'):
            DialAction(self.agi, self.cursor, event, "group", self.id).set_variables()

    def set_caller_id(self):
        CallerID(self.agi, self.cursor, "group", self.id).set_caller_id(force_rewrite=False)


class MeetMe:
    def __init__(self, agi, cursor, xid=None, number=None, context=None):
        self.agi = agi
        self.cursor = cursor

        columns = ('id', 'number', 'context', 'mode', 'musiconhold',
                   'poundexit', 'quiet', 'record', 'adminmode',
                   'announceusercount', 'announcejoinleave',
                   'alwayspromptpin', 'starmenu', 'enableexitcontext',
                   'exitcontext')
        columns = ["meetmefeatures." + c for c in columns]

        if xid:
            cursor.query("SELECT ${columns} FROM meetmefeatures "
                         "INNER JOIN staticmeetme "
                         "ON meetmefeatures.meetmeid = staticmeetme.id "
                         "WHERE meetmefeatures.id = %s "
                         "AND staticmeetme.commented = 0",
                         columns,
                         (xid,))
        elif number and context:
            cursor.query("SELECT ${columns} FROM meetmefeatures "
                         "INNER JOIN staticmeetme "
                         "ON meetmefeatures.meetmeid = staticmeetme.id "
                         "WHERE meetmefeatures.number = %s "
                         "AND meetmefeatures.context = %s "
                         "AND staticmeetme.commented = 0",
                         columns,
                         (number, context))
        else:
            raise LookupError("id or number@context must be provided to look up a conference room")

        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find conference room (id: %s, number: %s, context: %s)" % (xid, number, context))

        self.id = res['meetmefeatures.id']
        self.number = res['meetmefeatures.number']
        self.context = res['meetmefeatures.context']
        self.mode = res['meetmefeatures.mode']

        if self.mode == "talk":
            self.mode_talk = True
            self.mode_listen = False
        elif self.mode == "listen":
            self.mode_talk = False
            self.mode_listen = True
        elif self.mode == "all":
            self.mode_talk = True
            self.mode_listen = True
        else:
            raise ValueError("Invalid mode for conference room (id: %d, number: %s, context: %s)" % (xid, number, context))

        self.musiconhold = res['meetmefeatures.musiconhold']
        self.poundexit = res['meetmefeatures.poundexit']
        self.quiet = res['meetmefeatures.quiet']
        self.record = res['meetmefeatures.record']
        self.adminmode = res['meetmefeatures.adminmode']
        self.announceusercount = res['meetmefeatures.announceusercount']
        self.announcejoinleave = res['meetmefeatures.announcejoinleave']
        self.alwayspromptpin = res['meetmefeatures.alwayspromptpin']
        self.starmenu = res['meetmefeatures.starmenu']
        self.enableexitcontext = res['meetmefeatures.enableexitcontext']
        self.exitcontext = res['meetmefeatures.exitcontext']


class Queue:
    def __init__(self, agi, cursor, xid=None, number=None, context=None):
        self.agi = agi
        self.cursor = cursor

        columns = ('id', 'number', 'context', 'name', 'data_quality',
                   'hitting_callee', 'hitting_caller',
                   'retries', 'ring',
                   'transfer_user', 'transfer_call',
                   'write_caller', 'write_calling',
                   'url', 'announceoverride', 'timeout')
        columns = ["queuefeatures." + c for c in columns]

        if xid:
            cursor.query("SELECT ${columns} FROM queuefeatures "
                         "INNER JOIN queue "
                         "ON queuefeatures.name = queue.name "
                         "WHERE queuefeatures.id = %s "
                         "AND queue.commented = 0 "
                         "AND queue.category = 'queue'",
                         columns,
                         (xid,))
        elif number and context:
            cursor.query("SELECT ${columns} FROM queuefeatures "
                         "INNER JOIN queue "
                         "ON queuefeatures.name = queue.name "
                         "WHERE queuefeatures.number = %s "
                         "AND queuefeatures.context = %s "
                         "AND queue.commented = 0 "
                         "AND queue.category = 'queue'",
                         columns,
                         (number, context))
        else:
            raise LookupError("id or number@context must be provided to look up a queue")

        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find queue (id: %s, number: %s, context: %s)" % (xid, number, context))

        self.id = res['queuefeatures.id']
        self.number = res['queuefeatures.number']
        self.context = res['queuefeatures.context']
        self.name = res['queuefeatures.name']
        self.data_quality = res['queuefeatures.data_quality']
        self.hitting_callee = res['queuefeatures.hitting_callee']
        self.hitting_caller = res['queuefeatures.hitting_caller']
        self.retries = res['queuefeatures.retries']
        self.ring = res['queuefeatures.ring']
        self.transfer_user = res['queuefeatures.transfer_user']
        self.transfer_call = res['queuefeatures.transfer_call']
        self.write_caller = res['queuefeatures.write_caller']
        self.write_calling = res['queuefeatures.write_calling']
        self.url = res['queuefeatures.url']
        self.announceoverride = res['queuefeatures.announceoverride']
        self.timeout = res['queuefeatures.timeout']

    def set_dial_actions(self):
        for event in ('noanswer', 'congestion', 'busy', 'chanunavail'):
            DialAction(self.agi, self.cursor, event, "queue", self.id).set_variables()

    def set_caller_id(self):
        CallerID(self.agi, self.cursor, "queue", self.id).set_caller_id(force_rewrite=False)


class Agent:
    def __init__(self, agi, cursor, xid=None, number=None):
        self.agi = agi
        self.cursor = cursor

        columns = ('id', 'number', 'passwd', 'firstname', 'lastname', 'language', 'silent')

        if xid:
            cursor.query("SELECT ${columns} FROM agentfeatures "
                         "WHERE id = %s "
                         "AND commented = 0",
                         columns,
                         (xid,))
        elif number:
            cursor.query("SELECT ${columns} FROM agentfeatures "
                         "WHERE number = %s "
                         "AND commented = 0",
                         columns,
                         (number,))
        else:
            raise LookupError("id or number must be provided to look up an agent")

        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find agent (id: %s, number: %s)" % (xid, number))

        self.id = res['id']
        self.number = res['number']
        self.passwd = res['passwd']
        self.firstname = res['firstname']
        self.lastname = res['lastname']
        self.language = res['language']
        self.silent = res['silent']


class DialAction:
    def __init__(self, agi, cursor, event, category, categoryval):
        self.agi = agi
        self.cursor = cursor
        self.event = event
        self.category = category

        cursor.query("SELECT ${columns} FROM dialaction "
                     "WHERE event = %s "
                     "AND category = %s "
                     "AND categoryval = %s "
                     "AND linked = 1",
                     ('action', 'actionarg1', 'actionarg2'),
                     (event, category, categoryval))
        res = cursor.fetchone()

        if not res:
            self.action = "none"
            self.actionarg1 = None
            self.actionarg2 = None
        else:
            self.action = res['action']
            self.actionarg1 = res['actionarg1']
            self.actionarg2 = res['actionarg2']

    def set_variables(self):
        xtype = ("%s_%s" % (self.category, self.event)).upper()
        self.agi.set_variable('XIVO_FWD_%s_ACTION' % xtype, self.action)

        # Sometimes, it's useful to know whether these variables were
        # set manually, or by this object.
        self.agi.set_variable('XIVO_FWD_%s_ISDA' % xtype, "1")

        if self.actionarg1:
            self.agi.set_variable('XIVO_FWD_%s_ACTIONARG1' % xtype,
                             self.actionarg1.replace('|', ';'))

        if self.actionarg2:
            self.agi.set_variable('XIVO_FWD_%s_ACTIONARG2' % xtype,
                             self.actionarg2)


class Trunk:
    def __init__(self, agi, cursor, xid):
        self.agi = agi
        self.cursor = cursor

        columns = ('protocol', 'protocolid')

        cursor.query("SELECT ${columns} FROM trunkfeatures "
                     "WHERE id = %s",
                     columns,
                     (xid,))
        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find trunk (id: %d)" % xid)

        self.id = xid
        self.protocol = res['protocol']
        self.protocolid = res['protocolid']

        (self.interface, self.intfsuffix) = protocol_intf_and_suffix(cursor,
                                                                     self.protocol,
                                                                     'trunk',
                                                                     self.protocolid)


class HandyNumber:
    def __init__(self, agi, cursor, xid=None, exten=None):
        self.agi = agi
        self.cursor = cursor

        columns = ('id', 'exten', 'trunkfeaturesid', 'type')

        if xid:
            cursor.query("SELECT ${columns} FROM handynumbers "
                         "WHERE id = %s "
                         "AND commented = 0",
                         columns,
                         (xid,))
        elif exten:
            cursor.query("SELECT ${columns} FROM handynumbers "
                         "WHERE exten = %s "
                         "AND commented = 0",
                         columns,
                         (exten,))
        else:
            raise LookupError("id or exten must be provided to look up a handy number")

        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find handy number (id: %s, exten: %s)" % (xid, exten))

        self.id = res['id']
        self.exten = res['exten']
        self.trunkfeaturesid = int(res['trunkfeaturesid'])
        self.type = res['type']

        self.trunk = Trunk(agi, cursor, self.trunkfeaturesid)


class DID:
    def __init__(self, agi, cursor, xid=None, exten=None, context=None):
        self.agi = agi
        self.cursor = cursor

        columns = ('id', 'exten', 'context',
                   'faxdetectenable', 'faxdetecttimeout', 'faxdetectemail')

        if xid:
            cursor.query("SELECT ${columns} FROM incall "
                         "WHERE id = %s "
                         "AND commented = 0",
                         columns,
                         (xid,))
        elif exten and context:
            cursor.query("SELECT ${columns} FROM incall "
                         "WHERE exten = %s "
                         "AND context = %s "
                         "AND commented = 0",
                         columns,
                         (exten, context))
        else:
            raise LookupError("id or exten@context must be provided to look up a DID entry")

        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find DID entry (id: %s, exten: %s, context: %s)" % (xid, exten, context))

        self.id = res['id']
        self.exten = res['exten']
        self.context = res['context']
        self.faxdetectenable = int(bool(res['faxdetectenable']))
        self.faxdetecttimeout = res['faxdetecttimeout']
        self.faxdetectemail = res['faxdetectemail']

    def set_dial_actions(self):
        DialAction(self.agi, self.cursor, "answer", "incall", self.id).set_variables()

    def set_caller_id(self):
        CallerID(self.agi, self.cursor, "incall", self.id).set_caller_id(force_rewrite=True)


class Outcall:
    def __init__(self, agi, cursor, feature_list=None, xid=None, exten=None, context=None):
        self.agi = agi
        self.cursor = cursor

        columns = ('id', 'exten', 'context', 'externprefix', 'stripnum',
                   'setcallerid', 'callerid', 'useenum', 'internal',
                   'hangupringtime')

        if xid:
            cursor.query("SELECT ${columns} FROM outcall "
                         "WHERE id = %s "
                         "AND commented = 0",
                         ('exten', 'context'),
                         columns,
                         (xid,))
        elif exten and context:
            cursor.query("SELECT ${columns} FROM outcall "
                         "WHERE exten = %s "
                         "AND context = %s "
                         "AND commented = 0",
                         columns,
                         (exten, context))
        else:
            raise LookupError("id or exten@context must be provided to look up an outcall entry")

        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find outcall entry (id: %s, exten: %s, context: %s)" % (xid, exten, context))

        self.id = res['id']
        self.exten = res['exten']
        self.context = res['context']
        self.externprefix = res['externprefix']
        self.stripnum = res['stripnum']
        self.setcallerid = res['setcallerid']
        self.callerid = res['callerid']
        self.useenum = res['useenum']
        self.internal = res['internal']
        self.hangupringtime = res['hangupringtime']

        if not feature_list:
            feature_list = FeatureList(agi, cursor)

        cursor.query("SELECT ${columns} FROM outcalltrunk "
                     "WHERE outcallid = %s "
                     "ORDER BY priority ASC",
                     ('trunkfeaturesid',),
                     (self.id,))
        res = cursor.fetchall()

        if not res:
            raise ValueError("No trunk associated with outcall (id: %d)" % (xid,))

        self.trunks = []

        for row in res:
            try:
                trunk = Trunk(agi, cursor, row['trunkfeaturesid'])
            except LookupError:
                continue

            self.trunks.append(trunk)


class Schedule:
    def __init__(self, agi, cursor, xid):
        self.agi = agi
        self.cursor = cursor

        columns = ('timebeg', 'timeend', 'daynamebeg', 'daynameend',
                   'daynumbeg', 'daynumend', 'monthbeg', 'monthend')

        cursor.query("SELECT ${columns} FROM schedule "
                     "WHERE id = %s "
                     "AND commented = 0",
                     columns,
                     (xid,))
        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find schedule entry (id: %d)" % (xid,))

        self.id = xid
        self.timerange = '|'.join((
                self.forgetimefield(res['timebeg'], res['timeend']),
                self.forgetimefield(res['daynamebeg'], res['daynameend']),
                self.forgetimefield(res['daynumbeg'], res['daynumend']),
                self.forgetimefield(res['monthbeg'], res['monthend'])
        ))

    @staticmethod
    def forgetimefield(start, end):
        if start == '*':
            return '*'
        else:
            if end in (None, ''):
                return '%s' % (start,)
            else:
                return '%s-%s' % (start, end)

    def set_dial_actions(self):
        DialAction(self.agi, self.cursor, "inschedule", "schedule", self.id).set_variables()
        DialAction(self.agi, self.cursor, "outschedule", "schedule", self.id).set_variables()


class VoiceMenu:
    def __init__(self, agi, cursor, xid):
        self.agi = agi
        self.cursor = cursor

        columns = ('name', 'context')

        cursor.query("SELECT ${columns} FROM voicemenu "
                     "WHERE id = %s "
                     "AND commented = 0",
                     columns,
                     (xid,))
        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find voicemenu entry (id: %d)" % (xid,))

        self.id = xid
        self.name = res['name']
        self.context = res['context']


CALLERID_MATCHER = re.compile('^(?:"(.+)"|([a-zA-Z0-9\-\.\!%\*_\+`\'\~]+)) ?(?:<([0-9\*#]+)>)?$').match

class CallerID:
    def __init__(self, agi, cursor, xtype, typeval):
        self.agi = agi
        self.cursor = cursor
        self.type = xtype
        self.typeval = typeval

        cursor.query("SELECT ${columns} FROM callerid "
                     "WHERE type = %s "
                     "AND typeval = %s "
                     "AND mode IS NOT NULL",
                     ('mode', 'callerdisplay'),
                     (xtype, typeval))
        res = cursor.fetchone()

        self.mode = None
        self.callerdisplay = ''
        self.calleridname = None
        self.calleridnum = None

        if res:
            m = CALLERID_MATCHER(res['callerdisplay'])

            if m:
                self.mode = res['mode']
                self.callerdisplay = res['callerdisplay']
                self.calleridnum = m.group(3)

                self.calleridname = m.group(1)

                if self.calleridname is None:
                    self.calleridname = m.group(2)

    def set_caller_id(self, force_rewrite):
        """
        Set/Modify the caller ID if needed and allowed and create
        the XIVO_CID_REWRITTEN channel variable in some cases.

        @force_rewrite:
            True <=> CID modification is always allowed in this case.
                XIVO_CID_REWRITTEN is neither taken into account nor
                written.
            False <=> CID modification is only allowed if the channel
                variable XIVO_CID_REWRITTEN is not set prior to the
                call to this method.  If the CID modification really
                took place, XIVO_CID_REWRITTEN is created.
        """
        if not self.mode:
            return

        cidrewritten = self.agi.get_variable('XIVO_CID_REWRITTEN')

        if force_rewrite or not cidrewritten:

            calleridname = self.agi.get_variable('CALLERID(name)')
            calleridnum = self.agi.get_variable('CALLERID(num)')

            if calleridname in (None, '', '""'):
                calleridname = 'unknown'
            elif calleridname[0] == '"' and calleridname[-1] == '"':
                calleridname = calleridname[1:-1]

            if self.mode == 'prepend':
                name = "%s - %s" % (self.calleridname, calleridname)
            elif self.mode == 'overwrite':
                name = self.calleridname
            elif self.mode == 'append':
                name = "%s - %s" % (calleridname, self.calleridname)
            else:
                raise RuntimeError("Unknown callerid mode: %r" % self.mode)

            if self.calleridnum is not None:
                calleridnum = self.calleridnum
            elif calleridnum in (None, ''):
                calleridnum = 'unknown'

            self.agi.appexec('SetCallerPres', 'allowed')
            self.agi.set_variable('CALLERID(all)', '"%s" <%s>' % (name, calleridnum))

            if not force_rewrite:
                self.agi.set_variable('XIVO_CID_REWRITTEN', 1)


class ChanSIP:
    @staticmethod
    def get_intf_and_suffix(cursor, category, xid):

        cursor.query("SELECT ${columns} FROM usersip "
                     "WHERE id = %s "
                     "AND category = %s "
                     "AND commented = 0",
                     ('name',),
                     (xid, category))
        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find usersip entry (category: %s, id: %s)" % (category, xid))

        return ("SIP/%s" % res['name'], None)


class ChanIAX2:
    @staticmethod
    def get_intf_and_suffix(cursor, category, xid):

        cursor.query("SELECT ${columns} FROM useriax "
                     "WHERE id = %s "
                     "AND category = %s "
                     "AND commented = 0",
                     ('name',),
                     (xid, category))
        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find useriax entry (category: %s, id: %s)" % (category, xid))

        return ("IAX2/%s" % res['name'], None)


class ChanCustom:
    @staticmethod
    def get_intf_and_suffix(cursor, category, xid):

        cursor.query("SELECT ${columns} FROM usercustom "
                     "WHERE id = %s "
                     "AND category = %s "
                     "AND commented = 0",
                     ('interface', 'intfsuffix'),
                     (xid, category))
        res = cursor.fetchone()

        if not res:
            raise LookupError("Unable to find usercustom entry (category: %s, id: %s)" % (category, xid))

        # In case the suffix is the integer 0, bool(intfsuffix)
        # returns False though there is a suffix. Casting it to
        # a string prevents such an error.

        return (res['interface'], str(res['intfsuffix']))


CHAN_PROTOCOL = {
    'sip': ChanSIP,
    'iax': ChanIAX2,
    'custom': ChanCustom,
}

def protocol_intf_and_suffix(cursor, protocol, category, xid):
    """
    Lookup by protocol, category, xid and return the interface and interface suffix.
    On error, raise LookupError, ValueError, or an exception coming from the SQL backend.
    """
    if protocol in CHAN_PROTOCOL:
        return CHAN_PROTOCOL[protocol].get_intf_and_suffix(cursor, category, xid)
    else:
        raise ValueError("Unknown protocol %r" % protocol)
