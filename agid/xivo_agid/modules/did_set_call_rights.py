# TODO: see the call_rights module.

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
from xivo_agid import call_rights

def _did_set_call_rights(agi, cursor, args):
	srcnum = agi.get_variable('XIVO_SRCNUM')
	context = agi.get_variable('XIVO_CONTEXT')
	exten_pattern = agi.get_variable('XIVO_EXTENPATTERN')

	cursor.query("SELECT ${columns} FROM rightcallexten",
		     ('rightcallid', 'exten'))
	res = cursor.fetchall()

	if not res:
		call_rights.allow(agi)

	rightcallidset = set((row['rightcallid'] for row in res if call_rights.extension_matches(srcnum, row['exten'])))

	if not rightcallidset:
		call_rights.allow(agi)

	rightcallids = '(' + ','.join((str(el) for el in rightcallidset)) + ')'
	cursor.query("SELECT ${columns} FROM rightcall "
		     "INNER JOIN rightcallmember "
		     "ON rightcall.id = rightcallmember.rightcallid "
		     "INNER JOIN extenumbers "
		     "ON rightcallmember.typeval = extenumbers.typeval "
		     "WHERE rightcall.id IN " + rightcallids + " "
		     "AND rightcallmember.type = 'incall' "
		     "AND extenumbers.exten = %s "
		     "AND extenumbers.context = %s "
		     "AND extenumbers.type = 'incall' "
		     "AND rightcall.commented = 0",
		     (call_rights.RIGHTCALL_AUTHORIZATION_COLNAME, call_rights.RIGHTCALL_PASSWD_COLNAME),
		     (exten_pattern, context))
	res = cursor.fetchall()
	call_rights.apply_rules(agi, res)
	call_rights.allow(agi)

def did_set_call_rights(agi, cursor, args):
	try:
		_did_set_call_rights(agi, cursor, args)
	except call_rights.RuleAppliedException:
		return

agid.register(did_set_call_rights)
