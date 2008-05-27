__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008  Proformatique

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

def agent_set_features(handler, agi, cursor, args):
	number = args[0]

	cursor.query("SELECT ${columns} FROM agentfeatures "
		     "WHERE number = %s "
		     "AND commented = 0",
		     ('silent',),
		     (number,))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unknown agent number '%s'" % number)

	options = ''

	if res['silent']:
		options += 's'

	agi.set_variable('XIVO_AGENTOPTIONS', options)

agid.register(agent_set_features)
