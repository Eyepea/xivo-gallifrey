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

from xivo_agid import agid
from xivo_agid import objects

def incoming_group_set_features(agi, cursor, args):
    groupid = agi.get_variable('XIVO_DSTID')
    referer = agi.get_variable('XIVO_FWD_REFERER')

    try:
        group = objects.Group(agi, cursor, xid=int(groupid))
    except (ValueError, LookupError), e:
        agi.dp_break(str(e))

    options = ""

    if group.transfer_user:
        options += "t"

    if group.transfer_call:
        options += "T"

    if group.write_caller:
        options += "w"

    if group.write_calling:
        options += "W"

    if not group.musiconhold:
        options += "r"

    agi.set_variable('XIVO_REAL_NUMBER', group.number)
    agi.set_variable('XIVO_REAL_CONTEXT', group.context)
    agi.set_variable('XIVO_GROUPNAME', group.name)
    agi.set_variable('XIVO_GROUPOPTIONS', options)

    pickupmark = [] 
    memberlist = agi.get_variable("QUEUE_MEMBER_LIST(%s)" % group.name).split(',')

    for member in memberlist:
        try: 
            protocol, name = member.split('/', 1)
            user = objects.User(agi, cursor, name=name, protocol=protocol)
        except (ValueError, LookupError):
            continue

        if user.number:
                pickupmark.append("%s%%%s" % (user.number, user.context))

    agi.set_variable('__PICKUPMARK', '&'.join(pickupmark))

    if group.preprocess_subroutine:
        agi.set_variable('XIVO_GROUPPREPROCESS_SUBROUTINE', group.preprocess_subroutine)

    if group.timeout:
        agi.set_variable('XIVO_GROUPTIMEOUT', group.timeout)

    group.set_dial_actions()

    if referer == ("group:%s" % group.id) or referer.startswith("voicemenu:"):
        group.set_caller_id()

agid.register(incoming_group_set_features)
