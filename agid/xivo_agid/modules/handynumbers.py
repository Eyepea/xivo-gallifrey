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

from xivo_agid import agid
from xivo_agid import objects

def handynumbers(agi, cursor, args):
	userid = int(agi.get_variable('XIVO_USERID'))
	dstnum = agi.get_variable('XIVO_DSTNUM')
	exten_pattern = agi.get_variable('XIVO_EXTENPATTERN')

	user = objects.User(agi, cursor, xid = userid)
	handy_number = objects.HandyNumber(agi, cursor, exten=exten_pattern)
	trunk = handy_number.trunk

	agi.set_variable('XIVO_INTERFACE', trunk.interface)
	agi.set_variable('XIVO_TRUNKEXTEN', dstnum)

	if user.outcallerid:
		agi.set_variable('CALLERID(num)', user.outcallerid)

agid.register(handynumbers)
