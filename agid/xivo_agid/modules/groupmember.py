__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2009  Proformatique <technique@proformatique.com>

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

def groupmember(agi, cursor, args):
    userid  = agi.get_variable('XIVO_USERID')
    xlen    = len(args)

    if xlen < 3:
        agi.dp_break("Invalid number of arguments (args: %r)" % args)
    elif args[0] not in ('group', 'queue'):
        agi.dp_break("Invalid type (args: %r, excepted: 'group' or 'queue')" % args[0])
    elif args[1] not in ('add', 'remove', 'toggle'):
        agi.dp_break("Invalid action (args: %r, excepted: 'add', 'remove' or 'toggle')" % args[1])

    if xlen > 3 and args[3] != '':
        try:
            if xlen == 4:
                context = objects.User(agi, cursor, xid=int(userid)).context
            else:
                context = args[4]

            user = objects.User(agi, cursor, exten=args[3], context=context)
        except (ValueError, LookupError), e:
            agi.dp_break(str(e))
    else:
        try:
            user = objects.User(agi, cursor, int(userid))
        except (ValueError, LookupError), e:
            agi.dp_break(str(e))

    args[2] = str(args[2])

    if args[2].startswith('*'):
        xid     = args[2][1:]
        number  = None
        context = None
    else:
        xid     = None
        number  = args[2]
        context = user.context

    try:
        if args[0] == 'group':
            name = objects.Group(agi, cursor, xid, number, context).name
        else:
            name = objects.Queue(agi, cursor, xid, number, context).name
    except LookupError, e:
        agi.dp_break(str(e))

    agi.set_variable('XIVO_GROUPMEMBER_NAME', name)
    agi.set_variable('XIVO_GROUPMEMBER_ACTION', args[1])
    agi.set_variable('XIVO_GROUPMEMBER_USER_INTERFACE', user.interface)

agid.register(groupmember)
