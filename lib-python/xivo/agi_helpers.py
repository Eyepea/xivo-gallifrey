"""Helper functions for XIVO AGI

Copyright (C) 2007, 2008, Proformatique

This module provides a set of mostly unrelated functions that are used by
several AGI scripts in XIVO.

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008, Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

import sys
import anysql
import except_tb

def dp_break(agi, message, show_tb = False):
	"""DialPlan BREAK

	Display a message in the Asterisk CLI, optionally dumping the
	exception trace, hangup the current channel and stop execution.

	If show_tb is True, this function must be called from an except block.

	"""

	agi.verbose(message)

	if show_tb:
		except_tb.log_exception(agi.verbose)

	agi.hangup()
	sys.exit(1)

def agi_verbose_except(agi, message):
	"""Display a message followed by an exception trace. This function
	must be called from an except block.

	"""

	agi.verbose(message)
	except_tb.log_exception(agi.verbose)

def db_connect(agi, db_uri):
	"""DataBase CONNECT

	This function is a simple wrapper to connect to the database with error
	handling. If successful, it returns the connection object; otherwise it
	calls dp_break().

	"""

	try:
		conn = anysql.connect_by_uri(db_uri)
	except:
		dp_break(agi, "Unable to connect to %s" % db_uri, True)

	return conn

def set_fwd_vars(agi, cursor, type, typeval, appval, type_varname, typeval1_varname, typeval2_varname):
	"""The purpose of this function is to set some variables in the
	Asterisk channel related to redirection. It can set up to 3 variables:
	 - the redirection type (e.g. to a user, a group, a queue)
	 - 2 parameters (typeval1 and typeval2)

	type is the redirection type and is used to determine the corresponding
	behaviour (e.g. how many variables to set, where to fetch their value
	from).

	typeval is a value related to the redirection type. For example, if the
	type is 'user', and the type value is '101', this function will look up
	user ID 101 in the user features table and set 2 variables (a number
	and a context) so that the dial plan is able to forward the call to
	that user. In this example, the number and context are the 2 parameters
	(typeval1 and typeval2) that this function sets before returning.

	Forwarding a call to an application sometimes require an extra
	parameter (since type is 'application' and typeval1 is the application
	name). This extra parameter is given in the appval argument.

	Depending on the redirection type and the application (in case the type
	is 'application'), some parameters can be processed so that commas are
	translated to semicolumns. This is an ugly hack imagined because
	Asterisk evaluates variables early, and commas are used as an argument
	separator. In such cases, the dialplan translates semicolumns back
	to commas/pipes before using the variale.

	This function calls dp_break() upon detection of parameter/database
	inconsistency or when the type parameter is invalid.

	XXX This function and its users should be able to handle more
	variables (if possible, there should be no limit).

	"""

	agi.set_variable(type_varname, type)

	if type in ('endcall', 'schedule', 'sound'):
		agi.set_variable(typeval1_varname, typeval)
	elif type == 'application':
		agi.set_variable(typeval1_varname, typeval)

		if typeval in ('disa', 'callback'):
			agi.set_variable(typeval2_varname, appval.replace(",", ";").replace("|", ";"))
		else:
			agi.set_variable(typeval2_varname, appval)
	elif type == 'custom':
		agi.set_variable(typeval1_varname, typeval.replace(",", ";").replace("|", ";"))
	elif type == 'user':
		cursor.query("SELECT ${columns} FROM userfeatures "
                             "WHERE id = %s "
                             "AND internal = 0 "
                             "AND commented = 0",
                             ('number', 'context'),
                             (typeval,))
		res = cursor.fetchone()

		if not res:
			dp_break(agi, "Database inconsistency: unable to find linked destination user '%s'" % typeval)

		agi.set_variable(typeval1_varname, res['number'])
		agi.set_variable(typeval2_varname, res['context'])
	elif type == 'group':
		cursor.query("SELECT ${columns} FROM groupfeatures INNER JOIN queue "
                             "ON groupfeatures.name = queue.name "
                             "WHERE groupfeatures.id = %s "
                             "AND groupfeatures.deleted = 0 "
                             "AND queue.category = 'group' "
                             "AND queue.commented = 0",
                             [('groupfeatures.' + x) for x in ('number', 'context')],
                             (typeval,))
		res = cursor.fetchone()

		if not res:
			dp_break(agi, "Database inconsistency: unable to find linked destination group '%s'" % typeval)

		agi.set_variable(typeval1_varname, res['groupfeatures.number'])
		agi.set_variable(typeval2_varname, res['groupfeatures.context'])
	elif type == 'queue':
		cursor.query("SELECT ${columns} FROM queuefeatures INNER JOIN queue "
                             "ON queuefeatures.name = queue.name "
                             "WHERE queuefeatures.id = %s "
                             "AND queue.category = 'queue' "
                             "AND queue.commented = 0",
                             [('queuefeatures.' + x) for x in ('number', 'context')],
                             (typeval,))
		res = cursor.fetchone()

		if not res:
			dp_break(agi, "Database inconsistency: unable to find linked destination queue '%s'" % typeval)

		agi.set_variable(typeval1_varname, res['queuefeatures.number'])
		agi.set_variable(typeval2_varname, res['queuefeatures.context'])
	elif type == 'meetme':
		cursor.query("SELECT ${columns} FROM meetmefeatures INNER JOIN meetme "
                             "ON meetmefeatures.meetmeid = meetme.id "
                             "WHERE meetmefeatures.id = %s "
                             "AND meetme.commented = 0",
                             [('meetmefeatures.' + x) for x in ('number', 'context')],
                             (typeval,))
		res = cursor.fetchone()

		if not res:
			dp_break(agi, "Database inconsistency: unable to find linked destination conference room '%s'" % typeval)

		agi.set_variable(typeval1_varname, res['meetmefeatures.number'])
		agi.set_variable(typeval2_varname, res['meetmefeatures.context'])
	else:
		dp_break(agi, "Unknown destination type '%s'" % type)

class bsf_member:
	def __init__(self, type, userid, number, ringseconds):
		self.type = type
		self.userid = userid
		self.number = number
		self.interface = None

		if ringseconds == 0:
			self.ringseconds = ""

		else:
			self.ringseconds = ringseconds

	def __str__(self):
		return ("Call filter member object :\n"
                       "Type:        %s\n"
                       "User ID:     %s\n"
                       "Number:      %s\n"
                       "Interface:   %s\n"
                       "RingSeconds: %s"
                       % (self.type, self.userid, self.number, self.interface, self.ringseconds))

	def agi_str(self, agi):
		s = str(self)

		for line in s.splitlines():
			agi.verbose(line)

class bsfilter:
	def __init__(self, agi, cursor, boss_number, boss_context):
		self.id = None
		self.context = None
		self.mode = None
		self.zone = None
		self.callerdisplay = None
		self.ringseconds = None
		self.boss = None
		self.secretaries = None

		cursor.query("SELECT ${columns} FROM callfilter "
                             "INNER JOIN callfiltermember "
                             "ON callfilter.id = callfiltermember.callfilterid "
                             "INNER JOIN userfeatures "
                             "ON callfiltermember.typeval = userfeatures.id "
                             "WHERE callfilter.type = 'bosssecretary' "
                             #"AND callfilter.active = 1 "
                             "AND callfilter.commented = 0 "
                             "AND callfiltermember.type = 'user' "
                             "AND callfiltermember.bstype = 'boss' "
                             #"AND callfiltermember.active = 1 "
                             "AND userfeatures.number = %s "
                             "AND userfeatures.context = %s "
                             "AND userfeatures.internal = 0 "
                             "AND userfeatures.bsfilter = 'boss' "
                             "AND userfeatures.commented = 0",
                             ('callfilter.id', 'callfilter.bosssecretary',
                              'callfilter.zone', 'callfilter.callerdisplay',
                              'callfilter.ringseconds', 'callfiltermember.ringseconds',
                              'userfeatures.id', 'userfeatures.protocol',
                              'userfeatures.protocolid', 'userfeatures.name'),
                             (boss_number, boss_context))
		res = cursor.fetchone()

		if not res:
			raise LookupError, ("Unable to find call filter ID for boss (number = '%s', context = '%s')" % (boss_number, boss_context))

		protocol = res['userfeatures.protocol']
		protocolid = res['userfeatures.protocolid']
		name = res['userfeatures.name']

		self.id = res['callfilter.id']
		self.context = boss_context
		self.mode = res['callfilter.bosssecretary']
		self.zone = res['callfilter.zone']
		self.callerdisplay = res['callfilter.callerdisplay']
		self.ringseconds = res['callfilter.ringseconds']
		self.boss = bsf_member('boss', res['userfeatures.id'], boss_number, res['callfiltermember.ringseconds'])
		self.secretaries = []

		if self.ringseconds == 0:
			self.ringseconds = ""

		if protocol in ("sip", "iax"):
			interface = protocol.upper() + "/" + name
		elif protocol == "custom":
			cursor.query("SELECT ${columns} FROM usercustom "
                                     "WHERE id = %s "
                                     "AND commented = 0 "
                                     "AND category = 'user'",
                                     ('interface',),
                                     (protocolid,))
			res = cursor.fetchone()

			if not res:
				agi_helpers.dp_break(agi, "Database inconsistency: unable to find custom user (name = '%s', context = '%s')" % (name, context))

			interface = res['interface']
		else:
			agi_helpers.dp_break(agi, "Unknown protocol '%s'" % protocol)

		self.boss.interface = interface

		cursor.query("SELECT ${columns} FROM callfiltermember INNER JOIN userfeatures "
                             "ON callfiltermember.typeval = userfeatures.id "
                             "WHERE callfiltermember.callfilterid = %s "
                             "AND callfiltermember.type = 'user' "
                             "AND callfiltermember.bstype = 'secretary' "
                             #"AND callfiltermember.active = 1 "
                             "AND userfeatures.context = %s "
                             "AND userfeatures.internal = 0 "
                             "AND userfeatures.bsfilter = 'secretary' "
                             "AND userfeatures.commented = 0 "
                             "ORDER BY priority ASC",
                             ('userfeatures.id', 'userfeatures.protocol', 'userfeatures.protocolid',
                              'userfeatures.name', 'userfeatures.number', 'userfeatures.ringseconds'),
                             (self.id, boss_context))
		res = cursor.fetchall()

		if not res:
			raise LookupError, ("Unable to find secretaries for call filter ID %d (context = '%s')" % (self.id, boss_context))

		for row in res:
			protocol = row['userfeatures.protocol']
			protocolid = row['userfeatures.protocolid']
			name = row['userfeatures.name']
			secretary = bsf_member('secretary', row['userfeatures.id'],
                                               row['userfeatures.number'], row['userfeatures.ringseconds'])

			if protocol in ("sip", "iax"):
				interface = protocol.upper() + "/" + name
			elif protocol == "custom":
				cursor.query("SELECT ${columns} FROM usercustom "
                                             "WHERE id = %s "
                                             "AND commented = 0 "
                                             "AND category = 'user'",
                                             ('interface',),
                                             (protocolid,))
				res = cursor.fetchone()

				if not res:
					agi_helpers.dp_break(agi, "Database inconsistency: unable to find custom user (name = '%s', context = '%s')" % (name, context))

				interface = res['interface']
			else:
				agi_helpers.dp_break(agi, "Unknown protocol '%s'" % protocol)

			secretary.interface = interface
			self.secretaries.append(secretary)

		self.agi_str(agi)

	def __str__(self):
		return ("Call filter object :\n"
                       "Context:       %s\n"
                       "Mode:          %s\n"
                       "Zone:          %s\n"
                       "CallerDisplay: %s\n"
                       "RingSeconds:   %s\n"
                       "Boss:\n%s\n"
                       "Secretaries:\n%s"
                       % (self.context, self.mode, self.zone, self.callerdisplay,
                          self.ringseconds, self.boss, '\n'.join((str(secretary) for secretary in self.secretaries))))

	def agi_str(self, agi):
		s = str(self)

		for line in s.splitlines():
			agi.verbose(line)

	def check_zone(self, zone):
		if self.zone == "all":
			return True
		elif self.zone == "internal" and zone == "intern":
			return True
		elif self.zone == "external" and zone == "extern":
			return True
		else:
			return False

	def is_secretary(self, number, context = None):
		if not context:
			context = self.context

		secretary_numbers = (secretary.number for secretary in self.secretaries)

		if number in secretary_numbers and context == self.context:
			return True
		else:
			return False
