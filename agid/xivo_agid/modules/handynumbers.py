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

def handynumbers(handler, agi, cursor, args):
	srcnum = agi.get_variable('REAL_SRCNUM')
	dstnum = agi.get_variable('REAL_DSTNUM')
	context = agi.get_variable('REAL_CONTEXT')
	exten_pattern = agi.get_variable('REAL_EXTENPATTERN')

	cursor.query("SELECT ${columns} FROM userfeatures "
		     "WHERE number = %s "
		     "AND context = %s "
		     "AND internal = 0 "
		     "AND commented = 0",
		     ('outcallerid',),
		     (srcnum, context))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unknown number '%s'" % srcnum)

	callerid = res['outcallerid']

	cursor.query("SELECT ${columns} FROM trunkfeatures "
		     "INNER JOIN handynumbers "
		     "ON trunkfeatures.id = handynumbers.trunkfeaturesid "
		     "WHERE handynumbers.exten = %s "
		     "AND handynumbers.commented = 0",
		     ('protocol', 'protocolid'),
		     (exten_pattern,))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unable to find trunk features (handy number pattern = '%s')" % exten_pattern)

	protocol = res['protocol']
	protocolid = res['protocolid']

	if protocol == "sip":
		protocol = "SIP"
		cursor.query("SELECT ${columns} FROM usersip "
			     "WHERE id = %s "
			     "AND commented = 0",
			     ('name',),
			     (protocolid,))
		res = cursor.fetchone()

		if not res:
			agi.dp_break("Unable to find SIP peer (ID = '%s')" % protocolid)

		peer = res['name']
		interface = protocol + "/" + peer

	elif protocol == "iax":
		protocol = "IAX2"
		cursor.query("SELECT ${columns} FROM useriax "
			     "WHERE id = %s "
			     "AND commented = 0",
			     ('name',),
			     (protocolid,))
		res = cursor.fetchone()

		if not res:
			agi.dp_break("Unable to find IAX peer (ID = '%s')" % protocolid)

		peer = res['name']
		interface = protocol + "/" + peer

	elif protocol == "custom":
		protocol = "CUSTOM"
		cursor.query("SELECT ${columns} FROM usercustom "
			     "WHERE id = %s "
			     "AND commented = 0",
			     ('name', 'interface',),
			     (protocolid,))
		res = cursor.fetchone()

		if not res:
			agi.dp_break("Unable to find custom peer (ID = '%s')" % protocolid)

		peer = res['name']
		interface = res['interface']
	else:
		agi.dp_break("Unknown protocol '%s'" % protocol)

	agi.set_variable('XIVO_INTERFACE', interface)
	agi.set_variable('XIVO_TRUNKEXTEN', dstnum)

	if callerid:
		agi.set_variable('CALLERID(num)', callerid)

agid.register(handynumbers)
