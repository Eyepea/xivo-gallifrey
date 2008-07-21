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

def get_ast_dir(agi, cursor, args):
	name = str(args[0]).lower()
	ast_dir = agi.env.get("ast_%s_dir" % name)

	if ast_dir is None:
		ast_dir = ""
		agi.verbose("Unable to fetch an Asterisk's directory path from: %s", name)

	agi.set_variable('XIVO_AST_DIR', ast_dir)

agid.register(get_ast_dir)
