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
from xivo_agid import bsfilter

def user_set_feature(handler, agi, cursor, args):
	srcnum = agi.get_variable('REAL_SRCNUM')
	context = agi.get_variable('REAL_CONTEXT')

	custom_query = False
	query = "UPDATE userfeatures SET"
	params = []

	type = args[0]

	if type == "unc":
		enableunc = int(args[1])
		query += " enableunc = %s"
		params.append(enableunc)

		if enableunc:
			destunc = args[2]
		else:
			destunc = ""

		query += ", destunc = %s"
		params.append(destunc)
	elif type == "rna":
		enablerna = int(args[1])
		query += " enablerna = %s"
		params.append(enablerna)

		if enablerna:
			destrna = args[2]
		else:
			destrna = ""

		query += ", destrna = %s"
		params.append(destrna)
	elif type == "busy":
		enablebusy = int(args[1])
		query += " enablebusy = %s"
		params.append(enablebusy)

		if enablebusy:
			destbusy = args[2]
		else:
			destbusy = ""

		query += ", destbusy = %s"
		params.append(destbusy)
	elif type == "vm":
		cursor.query("SELECT ${columns} FROM userfeatures "
			     "WHERE number = %s "
			     "AND context = %s "
			     "AND internal = 0 "
			     "AND commented = 0",
			     ('enablevoicemail',),
			     (srcnum, context))
		res = cursor.fetchone()

		if not res:
			agi.dp_break("Unknown number '%s'" % srcnum)

		if res['enablevoicemail']:
			enablevoicemail = 0
		else:
			enablevoicemail = 1

		query += " enablevoicemail = %s"
		params.append(enablevoicemail)
		agi.set_variable('XIVO_VMENABLED', enablevoicemail)
	elif type == "dnd":
		cursor.query("SELECT ${columns} FROM userfeatures "
			     "WHERE number = %s "
			     "AND context = %s "
			     "AND internal = 0 "
			     "AND commented = 0",
			     ('enablednd',),
			     (srcnum, context))
		res = cursor.fetchone()

		if not res:
			agi.dp_break("Unknown number '%s'" % srcnum)

		if res['enablednd']:
			enablednd = 0
		else:
			enablednd = 1

		query += " enablednd = %s"
		params.append(enablednd)
		agi.set_variable('XIVO_DNDENABLED', enablednd)
	elif type == "callrecord":
		cursor.query("SELECT ${columns} FROM userfeatures "
			     "WHERE number = %s "
			     "AND context = %s "
			     "AND internal = 0 "
			     "AND commented = 0",
			     ('callrecord',),
			     (srcnum, context))
		res = cursor.fetchone()

		if not res:
			agi.dp_break("Unknown number '%s'" % srcnum)

		if res['callrecord']:
			callrecord = 0
		else:
			callrecord = 1

		query += " callrecord = %s"
		params.append(callrecord)
		agi.set_variable('XIVO_CALLRECORDENABLED', callrecord)
	elif type == "callfilter":
		cursor.query("SELECT ${columns} FROM userfeatures "
			     "WHERE number = %s "
			     "AND context = %s "
			     "AND internal = 0 "
			     "AND commented = 0",
			     ('callfilter',),
			     (srcnum, context))
		res = cursor.fetchone()

		if not res:
			agi.dp_break("Unknown number '%s'" % srcnum)

		if res['callfilter']:
			callfilter = 0
		else:
			callfilter = 1

		query += " callfilter = %s"
		params.append(callfilter)
		agi.set_variable('XIVO_CALLFILTERENABLED', callfilter)
	elif type == "bsfilter":
		custom_query = True

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
				filter = bsfilter.bsfilter(agi, cursor, number, context)
				caller_type = "secretary"
				secretary_number = srcnum

			# If it fails, suppose the caller is the boss and the number is
			# one of its secretaries number.
			except LookupError:
				filter = bsfilter.bsfilter(agi, cursor, srcnum, context)
				caller_type = "boss"
				secretary_number = number

			secretary = filter.get_secretary(secretary_number)

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
				     (filter.id, "user", secretary.userid, "secretary"))
			res = cursor.fetchone()

			if not res:
				agi.dp_break("Unable to find secretary, userid = %d" % secretary.userid)

			new_state = int(not res['active'])
			cursor.query("UPDATE callfiltermember "
				     "SET active = %s "
				     "WHERE callfilterid = %s "
				     "AND type = %s "
				     "AND typeval = %s "
				     "AND bstype = %s",
				     parameters = (new_state, filter.id, "user", secretary.userid, "secretary"))

			if cursor.rowcount != 1:
				agi.dp_break("Unable to perform the requested update")

			agi.set_variable('XIVO_BSFILTERENABLED', new_state)
		else:
			agi.dp_break("Unable to find boss-secretary filter")
	else:
		agi.dp_break("Unknown forwarding type '%s'" % type)

	if not custom_query:
		query += " WHERE number = %s AND context = %s"
		params.append(srcnum)
		params.append(context)
		cursor.query(query, parameters = params)

		if cursor.rowcount != 1:
			agi.dp_break("Unable to perform the requested update")

agid.register(user_set_feature)
