"""Boss-secretary filter classes for XIVO AGI

Copyright (C) 2007, 2008  Proformatique

This module provides a set of mostly unrelated functions that are used by
several AGI scripts in XIVO.

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

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

class bsf_member:
	"""This class represents a boss-secretary filter member (e.g. a boss
	or a secretary).

	"""

	def __init__(self, agi, active, type, userid, number, ringseconds):
		self.agi = agi
		self.active = bool(active)
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

	def agi_str(self):
		s = str(self)

		for line in s.splitlines():
			self.agi.verbose(line)

class bsfilter:
	"""Boss-secretary filter class. Creating a boss-secretary filter
	automatically load everything related to the filter (its properties,
	those of its boss, its secretaries). Creating a filter is also a way
	to check its existence. Trying to construct a filter that doesn't
	exist or has no secretary raises a LookupError.

	"""

	def __init__(self, agi, cursor, boss_number, boss_context):
		self.agi = agi
		self.id = None
		self.active = False
		self.context = None
		self.mode = None
		self.callfrom = None
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
			     "AND callfilter.commented = 0 "
			     "AND callfiltermember.type = 'user' "
			     "AND callfiltermember.bstype = 'boss' "
			     "AND userfeatures.number = %s "
			     "AND userfeatures.context = %s "
			     "AND userfeatures.internal = 0 "
			     "AND userfeatures.bsfilter = 'boss' "
			     "AND userfeatures.commented = 0",
			     ('callfilter.id', 'callfilter.bosssecretary',
			      'callfilter.callfrom', 'callfilter.callerdisplay',
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
		self.callfrom = res['callfilter.callfrom']
		self.callerdisplay = res['callfilter.callerdisplay']
		self.ringseconds = res['callfilter.ringseconds']
		self.boss = bsf_member(True, 'boss', res['userfeatures.id'], boss_number, res['callfiltermember.ringseconds'])
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
				agi.dp_break("Database inconsistency: unable to find custom user (name = '%s', context = '%s')" % (name, context))

			interface = res['interface']
		else:
			agi.dp_break(agi, "Unknown protocol '%s'" % protocol)

		self.boss.interface = interface

		cursor.query("SELECT ${columns} FROM callfiltermember INNER JOIN userfeatures "
			     "ON callfiltermember.typeval = userfeatures.id "
			     "WHERE callfiltermember.callfilterid = %s "
			     "AND callfiltermember.type = 'user' "
			     "AND callfiltermember.bstype = 'secretary' "
			     "AND IFNULL(userfeatures.number,'') != '' "
			     "AND userfeatures.context = %s "
			     "AND userfeatures.internal = 0 "
			     "AND userfeatures.bsfilter = 'secretary' "
			     "AND userfeatures.commented = 0 "
			     "ORDER BY priority ASC",
			     ('callfiltermember.active', 'userfeatures.id', 'userfeatures.protocol',
			      'userfeatures.protocolid', 'userfeatures.name', 'userfeatures.number',
			      'userfeatures.ringseconds'),
			     (self.id, boss_context))
		res = cursor.fetchall()

		if not res:
			raise LookupError, ("Unable to find secretaries for call filter ID %d (context = '%s')" % (self.id, boss_context))

		for row in res:
			protocol = row['userfeatures.protocol']
			protocolid = row['userfeatures.protocolid']
			name = row['userfeatures.name']
			secretary = bsf_member(row['callfiltermember.active'], 'secretary', row['userfeatures.id'],
					       row['userfeatures.number'], row['userfeatures.ringseconds'])

			if secretary.active:
				self.active = True

			if protocol in ("sip", "iax"):
				interface = protocol.upper() + "/" + name
			elif protocol == "custom":
				cursor.query("SELECT ${columns} FROM usercustom "
					     "WHERE id = %s "
					     "AND commented = 0 "
					     "AND category = 'user'",
					     ('interface',),
					     (protocolid,))
				res2 = cursor.fetchone()

				if not res2:
					agi.dp_break("Database inconsistency: unable to find custom user (name = '%s', context = '%s')" % (name, context))

				interface = res2['interface']
			else:
				agi.dp_break("Unknown protocol '%s'" % protocol)

			secretary.interface = interface
			self.secretaries.append(secretary)

	def __str__(self):
		return ("Call filter object :\n"
			"Context:       %s\n"
			"Mode:          %s\n"
			"Callfrom:      %s\n"
			"CallerDisplay: %s\n"
			"RingSeconds:   %s\n"
			"Boss:\n%s\n"
			"Secretaries:\n%s"
			% (self.context, self.mode, self.callfrom, self.callerdisplay,
			   self.ringseconds, self.boss, '\n'.join((str(secretary) for secretary in self.secretaries))))

	def agi_str(self):
		s = str(self)

		for line in s.splitlines():
			self.agi.verbose(line)

	def check_zone(self, zone):
		if self.callfrom == "all":
			return True
		elif self.callfrom == "internal" and zone == "intern":
			return True
		elif self.callfrom == "external" and zone == "extern":
			return True
		else:
			return False

	def get_secretary(self, number, context = None):
		if context and context != self.context:
			return None

		for secretary in self.secretaries:
			if number == secretary.number:
				return secretary

		return None
