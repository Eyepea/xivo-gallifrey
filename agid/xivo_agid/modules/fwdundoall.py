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

def fwdundoall(handler, agi, cursor, args):
	srcnum = agi.get_variable('REAL_SRCNUM')
	context = agi.get_variable('REAL_CONTEXT')

	cursor.query("UPDATE userfeatures "
                     "  SET enableunc = 0, "
                     "      destunc = '', "
                     "      enablerna = 0, "
                     "      destrna = '', "
                     "      enablebusy = 0, "
                     "      destbusy = '' "
                     "  WHERE number = %s "
                     "    AND context = %s",
                     parameters = (srcnum, context))

	if cursor.rowcount != 1:
		agi.dp_break("Unable to perform the requested update")

agid.register(fwdundoall)
