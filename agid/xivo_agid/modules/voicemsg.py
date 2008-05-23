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

def voicemsg(handler, agi, cursor, args):
	srcnum = agi.get_variable('REAL_SRCNUM')
	context = agi.get_variable('REAL_CONTEXT')

	cursor.query("SELECT ${columns} FROM userfeatures "
                     "INNER JOIN voicemail "
                     "ON userfeatures.voicemailid = voicemail.uniqueid "
                     "INNER JOIN voicemailfeatures "
                     "ON userfeatures.voicemailid = voicemailfeatures.id "
                     "WHERE userfeatures.number = %s "
                     "AND userfeatures.context = %s "
                     "AND userfeatures.internal = 0 "
                     "AND userfeatures.commented = 0 "
                     "AND voicemail.commented = 0",
                     ('voicemailfeatures.skipcheckpass',),
                     (srcnum, context))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unknown number '%s'" % srcnum)

	if res['voicemailfeatures.skipcheckpass']:
		agi.set_variable('XIVO_VMOPTIONS', "s")

agid.register(voicemsg)
