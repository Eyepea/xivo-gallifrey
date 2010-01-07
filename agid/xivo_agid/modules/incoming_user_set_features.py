__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import time

from xivo_agid import agid
from xivo_agid import objects

def incoming_user_set_features(agi, cursor, args):
    userid = agi.get_variable('XIVO_USERID')
    dstid = agi.get_variable('XIVO_DSTID')
    zone = agi.get_variable('XIVO_CALLORIGIN')
    bypass_filter = agi.get_variable('XIVO_CALLFILTER_BYPASS')

    # FIXME: this is only for the callrecord feature, which is likely to change
    srcnum = agi.get_variable('XIVO_SRCNUM')
    dstnum = agi.get_variable('XIVO_DSTNUM')

    feature_list = objects.ExtenFeatures(agi, cursor)

    if userid:
        try:
            caller = objects.User(agi, cursor, int(userid))
        except (ValueError, LookupError):
            caller = None
    else:
        caller = None

    try:
        user = objects.User(agi, cursor, int(dstid))
    except (ValueError, LookupError), e:
        agi.dp_break(str(e))

    agi.set_variable('XIVO_DST_FIRSTNAME', user.firstname)
    agi.set_variable('XIVO_DST_LASTNAME', user.lastname)

    agi.set_variable('XIVO_REAL_NUMBER', user.number)
    agi.set_variable('XIVO_REAL_CONTEXT', user.context)

    ufilter = user.filter

    # Special case. If a boss-secretary filter is set, the code will prematurely
    # exit because the other normally set variables are skipped.
    if not bypass_filter and ufilter and ufilter.active:
        zone_applies = ufilter.check_zone(zone)

        if caller:
            secretary = ufilter.get_secretary_by_id(caller.id)
        else:
            secretary = None

        if zone_applies and not secretary:
            if ufilter.mode in ("bossfirst-simult", "bossfirst-serial", "all"):
                agi.set_variable('XIVO_CALLFILTER_BOSS_INTERFACE', ufilter.boss.interface)
                agi.set_variable('XIVO_CALLFILTER_BOSS_TIMEOUT', ufilter.boss.ringseconds)

            if ufilter.mode in ("bossfirst-simult", "secretary-simult", "all"):
                interface = '&'.join(secretary.interface for secretary in ufilter.secretaries if secretary.active)
                agi.set_variable('XIVO_CALLFILTER_INTERFACE', interface)
                agi.set_variable('XIVO_CALLFILTER_TIMEOUT', ufilter.ringseconds)
            elif ufilter.mode in ("bossfirst-serial", "secretary-serial"):
                index = 0

                for secretary in ufilter.secretaries:
                    if secretary.active:
                        agi.set_variable('XIVO_CALLFILTER_SECRETARY%d_INTERFACE' % (index,), secretary.interface)
                        agi.set_variable('XIVO_CALLFILTER_SECRETARY%d_TIMEOUT' % (index,), secretary.ringseconds)
                        index += 1

            ufilter.set_dial_actions()
            ufilter.rewrite_cid()

            agi.set_variable('XIVO_CALLFILTER', '1')
            agi.set_variable('XIVO_CALLFILTER_MODE', ufilter.mode)
            return

    options = ""

    if user.enablexfer:
        options += "t"

    if caller and caller.enablexfer:
        options += "T"

    if user.enableautomon:
        options += "w"

    if caller and caller.enableautomon:
        options += "W"

    if feature_list.incallfilter and user.incallfilter: # pylint: disable-msg=E1101
        options += "p"

    agi.set_variable('XIVO_CALLOPTIONS', options)
    agi.set_variable('XIVO_INTERFACE', user.interface)
    agi.set_variable('XIVO_SIMULTCALLS', user.simultcalls)

    if user.ringseconds > 0:
        ringseconds = user.ringseconds
    else:
        ringseconds = ""

    agi.set_variable('XIVO_RINGSECONDS', ringseconds)

    if feature_list.enablednd: # pylint: disable-msg=E1101
        enablednd = user.enablednd
    else:
        enablednd = 0

    agi.set_variable('XIVO_ENABLEDND', enablednd)

    mailbox         = ""
    mailbox_context = ""
    useremail       = ""

    if user.vmbox:
        mailbox         = user.vmbox.mailbox
        mailbox_context = user.vmbox.context

        if user.vmbox.email:
            useremail = user.vmbox.email

    agi.set_variable('XIVO_ENABLEVOICEMAIL', user.enablevoicemail)
    agi.set_variable('XIVO_MAILBOX', mailbox)
    agi.set_variable('XIVO_MAILBOX_CONTEXT', mailbox_context)
    agi.set_variable('XIVO_USEREMAIL', useremail)

    enableunc       = 0
    unc_action      = 'none'
    unc_actionarg1  = ""
    unc_actionarg2  = ""

    if feature_list.fwdunc: # pylint: disable-msg=E1101
        enableunc = user.enableunc

        if enableunc:
            unc_action      = 'extension'
            unc_actionarg1  = user.destunc
            unc_actionarg2  = user.context

    agi.set_variable('XIVO_ENABLEUNC', enableunc)
    objects.DialAction.set_agi_variables(agi,
                                         'unc',
                                         'user',
                                         unc_action,
                                         unc_actionarg1,
                                         unc_actionarg2,
                                         False)

    setbusy         = False
    enablebusy      = 0
    busy_action     = 'none'
    busy_actionarg1 = ""
    busy_actionarg2 = ""

    if feature_list.fwdbusy: # pylint: disable-msg=E1101
        enablebusy = user.enablebusy

        if enablebusy:
            busy_action     = 'extension'
            busy_actionarg1 = user.destbusy
            busy_actionarg2 = user.context
        else:
            setbusy = True
            objects.DialAction(agi, cursor, 'busy', 'user', user.id).set_variables()

    agi.set_variable('XIVO_ENABLEBUSY', enablebusy)

    if not setbusy:
        objects.DialAction.set_agi_variables(agi,
                                             'busy',
                                             'user',
                                             busy_action,
                                             busy_actionarg1,
                                             busy_actionarg2,
                                             False)

    setrna         = False
    enablerna      = 0
    rna_action     = 'none'
    rna_actionarg1 = ""
    rna_actionarg2 = ""

    if feature_list.fwdrna: # pylint: disable-msg=E1101
        enablerna = user.enablerna

        if enablerna:
            rna_action     = 'extension'
            rna_actionarg1 = user.destrna
            rna_actionarg2 = user.context
        else:
            setrna = True
            objects.DialAction(agi, cursor, 'noanswer', 'user', user.id).set_variables()

    agi.set_variable('XIVO_ENABLERNA', enablerna)

    if not setrna:
        objects.DialAction.set_agi_variables(agi,
                                             'noanswer',
                                             'user',
                                             rna_action,
                                             rna_actionarg1,
                                             rna_actionarg2,
                                             False)

    objects.DialAction(agi, cursor, 'congestion', 'user', user.id).set_variables()
    objects.DialAction(agi, cursor, 'chanunavail', 'user', user.id).set_variables()

    if user.musiconhold:
        agi.set_variable('CHANNEL(musicclass)', user.musiconhold)

    if feature_list.callrecord and user.callrecord: # pylint: disable-msg=E1101
        # BUGBUG the context is missing in the filename TODO use ids
        callrecordfile = "user-%s-%s-%s.wav" % (srcnum, dstnum, int(time.time()))
    else:
        callrecordfile = ""

    if user.preprocess_subroutine:
        preprocess_subroutine = user.preprocess_subroutine
    else:
        preprocess_subroutine = ""

    if user.mobilephonenumber:
        mobilephonenumber = user.mobilephonenumber
    else:
        mobilephonenumber = ""

    agi.set_variable('XIVO_CALLRECORDFILE', callrecordfile)
    agi.set_variable('XIVO_USERPREPROCESS_SUBROUTINE', preprocess_subroutine)
    agi.set_variable('XIVO_MOBILEPHONENUMBER', mobilephonenumber)

agid.register(incoming_user_set_features)
