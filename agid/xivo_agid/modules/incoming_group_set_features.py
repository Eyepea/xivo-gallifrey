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

def incoming_group_set_features(agi, cursor, args):
	dstnum = agi.get_variable('XIVO_DSTNUM')
	dstid = int(agi.get_variable('XIVO_DSTID'))
	context = agi.get_variable('XIVO_CONTEXT')

	if dstnum and context:
		group = objects.Group(agi, cursor, number = dstnum, context = context)
		agi.set_variable('XIVO_DSTID', group.id)
	elif dstid:
		group = objects.Group(agi, cursor, xid = dstid)
	else:
		agi.dp_break("No dstnum@context or groupid given, unable to lookup group")

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

	agi.set_variable('XIVO_GROUPNAME', group.name)
	agi.set_variable('XIVO_GROUPOPTIONS', options)

	if group.timeout:
		agi.set_variable('XIVO_GROUPTIMEOUT', group.timeout)

	group.set_dial_actions()

agid.register(incoming_group_set_features)
