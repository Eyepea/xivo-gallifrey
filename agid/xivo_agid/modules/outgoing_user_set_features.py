__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>

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

def outgoing_user_set_features(agi, cursor, args):
    userid = agi.get_variable('XIVO_USERID')
    dstid = agi.get_variable('XIVO_DSTID')
    dstnum = agi.get_variable('XIVO_DSTNUM')

    # FIXME: this is only for the callrecord feature, which is likely to change
    srcnum = agi.get_variable('XIVO_SRCNUM')

    try:
        outcall = objects.Outcall(agi, cursor, int(dstid))
    except (ValueError, LookupError), e:
        agi.dp_break(str(e))

    orig_dstnum = dstnum
    callerid = None
    callrecord = False
    options = ""

    if outcall.stripnum > 0:
        dstnum = dstnum[outcall.stripnum:]

    if outcall.externprefix:
        dstnum = outcall.externprefix + dstnum

    if not outcall.internal:
        try:
            user = objects.User(agi, cursor, int(userid))

            callerid = user.outcallerid

            if user.enableautomon:
                options += "W"

            callrecord = user.callrecord
        except (ValueError, LookupError):
            pass

    if callerid in (None, '', 'default') and outcall.setcallerid:
        callerid = outcall.callerid

    for i, trunk in enumerate(outcall.trunks):
        agi.set_variable('XIVO_INTERFACE%d' % i, trunk.interface)

        # XXX numbers of stripped digits and prefix should be set on
        # per-trunk basis instead of per-outcall.
        agi.set_variable('XIVO_TRUNKEXTEN%d' % i, dstnum)

        if trunk.intfsuffix:
            intfsuffix = trunk.intfsuffix
        else:
            intfsuffix = ""

        agi.set_variable('XIVO_TRUNKSUFFIX%d' % i, intfsuffix)

    if callerid and callerid != 'default':
        objects.CallerID.set(agi, callerid)

        if callerid == 'anonymous':
            agi.appexec('SetCallerPres', 'prohib')

    if callrecord and objects.FeatureList(agi, cursor).callrecord:
        # BUGBUG the context is missing in the filename TODO use ids
        callrecordfile = "user-%s-%s-%s.wav" % (srcnum, orig_dstnum, int(time.time()))
    else:
        callrecordfile = ""

    if outcall.preprocess_subroutine:
        preprocess_subroutine = outcall.preprocess_subroutine
    else:
        preprocess_subroutine = ""

    if outcall.hangupringtime:
        hangupringtime = outcall.hangupringtime
    else:
        hangupringtime = ""

    agi.set_variable('XIVO_OUTCALLID', outcall.id)
    agi.set_variable('XIVO_CALLOPTIONS', options)
    agi.set_variable('XIVO_CALLRECORDFILE', callrecordfile)
    agi.set_variable('XIVO_OUTCALLPREPROCESS_SUBROUTINE', preprocess_subroutine)
    agi.set_variable('XIVO_HANGUPRINGTIME', hangupringtime)

agid.register(outgoing_user_set_features)
