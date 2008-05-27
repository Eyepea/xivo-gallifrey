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

def incoming_did_set_features(handler, agi, cursor, args):
	exten_pattern = agi.get_variable('REAL_EXTENPATTERN')

	cursor.query("SELECT ${columns} FROM incall "
		     "WHERE exten = %s "
		     "AND linked = 1 "
		     "AND commented = 0",
		     ('type', 'typeval', 'applicationval'),
		     (exten_pattern,))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unknown extension '%s'" % exten_pattern)

	handler.set_fwd_vars(res['type'], res['typeval'], res['applicationval'],
			     "XIVO_DIDTYPE", "XIVO_DIDTYPEVAL1", "XIVO_DIDTYPEVAL2")

agid.register(incoming_did_set_features)
