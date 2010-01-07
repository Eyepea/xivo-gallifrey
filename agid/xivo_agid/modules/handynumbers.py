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

from xivo_agid import agid
from xivo_agid import objects

def handynumbers(agi, cursor, args):
    userid = agi.get_variable('XIVO_USERID')
    dstnum = agi.get_variable('XIVO_DSTNUM')
    exten_pattern = agi.get_variable('XIVO_EXTENPATTERN')

    if userid:
        try:
            user = objects.User(agi, cursor, int(userid))
        except (ValueError, LookupError), e:
            user = None
            agi.verbose(str(e))
    else:
        user = None

    try:
        handy_number = objects.HandyNumber(agi, cursor, exten=exten_pattern)
    except LookupError, e:
        agi.dp_break(str(e))

    trunk = handy_number.trunk

    agi.set_variable('XIVO_INTERFACE', trunk.interface)
    agi.set_variable('XIVO_TRUNKEXTEN', dstnum)

    if trunk.intfsuffix:
        intfsuffix = trunk.intfsuffix
    else:
        intfsuffix = ""

    agi.set_variable('XIVO_TRUNKSUFFIX', intfsuffix)

    if user and user.outcallerid and user.outcallerid != 'default':
        objects.CallerID.set(agi, user.outcallerid)

        if user.outcallerid == 'anonymous':
            agi.appexec('SetCallerPres', 'prohib')

agid.register(handynumbers)
