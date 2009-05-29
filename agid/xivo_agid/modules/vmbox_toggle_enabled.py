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

def vmbox_toggle_enabled(agi, cursor, args):
    vmboxid = agi.get_variable('XIVO_VMBOXID')
    userid  = agi.get_variable('XIVO_USERID')

    xlen = len(args)

    if xlen > 0:
        try:
            if xlen == 1:
                context = objects.User(agi, cursor, xid=int(userid)).context
            else:
                context = args[1]

            vmbox = objects.VMBox(agi, cursor, mailbox=args[0], context=context, commentcond=False)
        except (ValueError, LookupError), e:
            agi.dp_break(str(e))
    else:
        vmbox = None

        if not vmboxid:
            try:
                user = objects.User(agi, cursor, xid=int(userid))
            except (ValueError, LookupError), e:
                agi.dp_break(str(e))

            if user.vmbox:
                vmbox = user.vmbox
            else:
                vmboxid = user.voicemailid

        if not vmbox:
            try:
                vmbox = objects.VMBox(agi, cursor, int(vmboxid), commentcond=False)
            except (ValueError, LookupError), e:
                agi.dp_break(str(e))

    if vmbox.password != '':
        agi.appexec('Authenticate', vmbox.password)

    try:
        enabled = vmbox.toggle_feature()
    except objects.DBUpdateException, e:
        agi.verbose(str(e))

    agi.set_variable('XIVO_VMBOXID', vmbox.id)
    agi.set_variable('XIVO_VMBOX_ENABLED', enabled)

agid.register(vmbox_toggle_enabled)
