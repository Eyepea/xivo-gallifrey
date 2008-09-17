__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006, 2007, 2008  Proformatique

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

import time

from xivo_agid import agid
from xivo_agid import objects

def incoming_meetme_set_features(agi, cursor, args):
    meetmeid = agi.get_variable('XIVO_DSTID')

    try:
        meetme = objects.MeetMe(agi, cursor, xid=int(meetmeid))
    except (ValueError, LookupError), e:
        agi.dp_break(str(e))

    options = ""

    if meetme.mode == "talk":
        options += "t"
    elif meetme.mode == "listen":
        options += "l"

    if meetme.musiconhold:
        agi.set_variable('CHANNEL(musicclass)', meetme.musiconhold)
        options += "M"

    if meetme.poundexit:
        options += "p"

    if meetme.quiet:
        options += "q"

    if meetme.record:
        options += "r"

    if meetme.adminmode:
        options += "a"

    if meetme.announceusercount:
        options += "c"

    if meetme.announcejoinleave:
        options += "i"

    if meetme.announcejoinleave:
        options += "P"

    if meetme.starmenu:
        options += "s"

    if meetme.enableexitcontext and meetme.exitcontext:
        options += "X"
        agi.set_variable('MEETME_EXIT_CONTEXT', meetme.exitcontext)

    agi.set_variable('MEETME_RECORDINGFILE', "meetme-%s-%s" % (meetme.number, int(time.time())))
    agi.set_variable('XIVO_MEETMENUMBER', meetme.number)
    agi.set_variable('XIVO_MEETMEOPTIONS', options)

agid.register(incoming_meetme_set_features)
