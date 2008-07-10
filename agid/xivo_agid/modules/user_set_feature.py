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

def user_set_feature(agi, cursor, args):
	userid = agi.get_variable('XIVO_USERID')
	srcnum = agi.get_variable('XIVO_SRCNUM')
	context = agi.get_variable('XIVO_CONTEXT')
	
	try:
		user = objects.User(agi, cursor, int(userid))
	except LookupError, e:
		agi.dp_break(str(e))

	feature = args[0]

	if feature in ("unc", "rna", "busy"):
		enabled = int(args[1])

		try:
			arg = args[2]
		except IndexError:
			arg = None

		user.set_feature(feature, enabled, arg)
	elif feature in ("vm", "dnd", "callrecord", "callfilter"):
		enabled = user.toggle_feature(feature)

		if feature == "vm":
			agi.set_variable('XIVO_VMENABLED', user.enablevoicemail)
		elif feature == "dnd":
			agi.set_variable('XIVO_DNDENABLED', user.enablednd)
		elif feature == "callrecord":
			agi.set_variable('XIVO_CALLRECORDENABLED', user.callrecord)
		elif feature == "callfilter":
			agi.set_variable('XIVO_CALLFILTERENABLED', user.callfilter)

	# TODO: rewrite.
	elif feature == "bsfilter":
		try:
			num1, num2 = args[1].split('*')
		except ValueError:
			agi.dp_break("Invalid number")

		secretary = None

		# Both the boss and secretary numbers are passed, so select the one
		# we don't already know.
		if srcnum == num1:
			number = num2
		elif srcnum == num2:
			number = num1
		else:
			agi.dp_break("Invalid number")

		try:
			# First, suppose the caller is a secretary and the number is
			# one of its bosses number.
			try:
				bsf = objects.BossSecretaryFilter(agi, cursor, number, context)
				caller_type = "secretary"
				secretary_number = srcnum

			# If it fails, suppose the caller is the boss and the number is
			# one of its secretaries number.
			except LookupError:
				bsf = objects.BossSecretaryFilter(agi, cursor, srcnum, context)
				caller_type = "boss"
				secretary_number = number

			bsf.set_dial_actions();
			secretary = bsf.get_secretary_by_number(secretary_number)

		# If all tries fail, give up.
		except LookupError:
			pass

		if secretary:
			agi.verbose("Filter exists ! Caller is %s, secretary number is %s" % (caller_type, secretary_number))
			cursor.query("SELECT ${columns} FROM callfiltermember "
				     "WHERE callfilterid = %s "
				     "AND type = %s "
				     "AND typeval = %s "
				     "AND bstype = %s",
				     ('active',),
				     (bsf.id, "user", secretary.id, "secretary"))
			res = cursor.fetchone()

			if not res:
				agi.dp_break("Unable to find secretary, secretary id = %d" % secretary.id)

			new_state = int(not res['active'])
			cursor.query("UPDATE callfiltermember "
				     "SET active = %s "
				     "WHERE callfilterid = %s "
				     "AND type = %s "
				     "AND typeval = %s "
				     "AND bstype = %s",
				     parameters = (new_state, bsf.id, "user", secretary.id, "secretary"))

			if cursor.rowcount != 1:
				agi.dp_break("Unable to perform the requested update")

			agi.set_variable('XIVO_BSFILTERENABLED', new_state)
		else:
			agi.dp_break("Unable to find boss-secretary filter")
	else:
		agi.dp_break("Unknown feature '%s'" % feature)

agid.register(user_set_feature)
