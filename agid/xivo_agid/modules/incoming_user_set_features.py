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

import time

from xivo_agid import agid
from xivo_agid import bsfilter

def incoming_user_set_features(handler, agi, cursor, args):
	srcnum = agi.get_variable('REAL_SRCNUM')
	dstnum = agi.get_variable('REAL_DSTNUM')
	context = agi.get_variable('REAL_CONTEXT')
	zone = agi.get_variable('REAL_CALLTYPE')
	bypass = agi.get_variable('XIVO_BSFILTER_BYPASS')

	# Modify below if columns is modified and contains any vars not of the form
	# [a-zA-Z_][a-zA-Z0-9_]*
	columns = ('id', 'protocol' , 'protocolid', 'name', 'ringseconds', 'simultcalls', 'enablevoicemail', 'voicemailid', 'enablexfer', 'enableautomon', 'callrecord', 'callfilter', 'enablednd', 'enableunc', 'destunc', 'enablerna', 'destrna', 'enablebusy', 'destbusy', 'musiconhold', 'bsfilter')
	cursor.query("SELECT ${columns} FROM userfeatures "
		     "WHERE number = %s "
		     "AND context = %s "
		     "AND internal = 0 "
		     "AND commented = 0",
		     columns,
		     (dstnum, context))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unknown number '%s'" % dstnum)

	# Populate locals()
	# works if columns contains vars of the form [a-zA-Z_][a-zA-Z0-9_]*
	for var in columns:
		exec("%s = res['%s']" % (var, var))

	# Special case. If a boss-secretary filter is set, the code will prematurely
	# exit because the other normally set variables become useless.
	if not bypass and bsfilter == 'boss':
		apply_filter = False

		try:
			filter = bsfilter.bsfilter(agi, cursor, dstnum, context)

			if not filter.active:
				raise LookupError

			zone_applies = filter.check_zone(zone)
			secretary = filter.get_secretary(srcnum)

			if zone_applies and not secretary:
				apply_filter = True
		except LookupError:
			pass

		if apply_filter:
			if filter.mode in ("bossfirst-simult", "secretary-simult", "all"):
				if filter.mode in ("bossfirst-simult", "all"):
					agi.set_variable('XIVO_BSFILTER_BOSS_INTERFACE', filter.boss.interface)
					agi.set_variable('XIVO_BSFILTER_BOSS_TIMEOUT', filter.boss.ringseconds)

				interface = '&'.join(secretary.interface for secretary in filter.secretaries if secretary.active)
				agi.set_variable('XIVO_BSFILTER_INTERFACE', interface)
				agi.set_variable('XIVO_BSFILTER_TIMEOUT', filter.ringseconds)

			elif filter.mode in ("bossfirst-serial", "secretary-serial"):
				if filter.mode == "bossfirst-serial":
					agi.set_variable('XIVO_BSFILTER_BOSS_INTERFACE', filter.boss.interface)
					agi.set_variable('XIVO_BSFILTER_BOSS_TIMEOUT', filter.boss.ringseconds)

				index = 0

				for secretary in filter.secretaries:
					if secretary.active:
						agi.set_variable('XIVO_BSFILTER_SECRETARY%d_INTERFACE' % index, secretary.interface)
						agi.set_variable('XIVO_BSFILTER_SECRETARY%d_TIMEOUT' % index, secretary.ringseconds)
						index += 1

			handler.ds_set_fwd_vars(filter.id, 'noanswer', 'callfilter', 'XIVO_BSFILTER_FWD_TYPERNA', 'XIVO_BSFILTER_FWD_TYPEVAL1RNA', 'XIVO_BSFILTER_FWD_TYPEVAL2RNA')

			if filter.callerdisplay:
				agi.set_variable('CALLERID(name)', "%s - %s" % (filter.callerdisplay, agi.get_variable("CALLERID(name)")))

			agi.set_variable('XIVO_BSFILTER_MODE', filter.mode)
			agi.set_variable('XIVO_BSFILTER', '1')
			return

	if protocol == "iax":
		interface = "IAX2/" + name
	elif protocol == "sip":
		interface = "SIP/" + name
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
		agi.dp_break("Unknown protocol '%s'" % protocol)

	# The extension table contains some rows which are used to activate some
	# services like redirections or voicemail.
	cursor.query("SELECT ${columns} FROM extensions "
		     "WHERE name IN ('fwdunc', 'fwdrna', 'fwdbusy', 'enablevm', 'incallfilter', 'incallrec', 'enablednd') "
		     "AND commented = 0",
		     ('name',))
	res = cursor.fetchall()

	features_list = [row['name'] for row in res]

	calloptions = ''

	if enablexfer == 1:
		calloptions += "t"

	if enableautomon == 1:
		calloptions += "w"

	if 'incallfilter' in features_list and callfilter == 1:
		calloptions += "p"

	agi.set_variable('XIVO_INTERFACE', interface)
	agi.set_variable('XIVO_SIMULTCALLS', simultcalls)

	if ringseconds > 0:
		agi.set_variable('XIVO_RINGSECONDS', ringseconds)

	if 'enablednd' in features_list:
		agi.set_variable('XIVO_ENABLEDND', enablednd)
	else:
		agi.set_variable('XIVO_ENABLEDND', 0)

	if 'enablevm' in features_list:
		agi.set_variable('XIVO_ENABLEVOICEMAIL', enablevoicemail)

		if enablevoicemail:
			cursor.query("SELECT ${columns} FROM voicemail "
				     "WHERE uniqueid = %s "
				     "AND context = %s "
				     "AND commented = 0",
				     ('email',),
				     (voicemailid, context))
			res = cursor.fetchone()

			if res and res['email']:
				agi.set_variable('XIVO_USEREMAIL', res['email'])
	else:
		agi.set_variable('XIVO_ENABLEVOICEMAIL', 0)

	agi.set_variable('XIVO_CALLOPTIONS', calloptions)

	if 'fwdunc' in features_list:
		agi.set_variable('XIVO_ENABLEUNC', enableunc)

		if enableunc == 1:
			# The redirection isn't actually meant to target a user, but
			# the current implementation allows the use of this type for
			# any kind of number.
			agi.set_variable('XIVO_FWD_TYPEUNC', 'user')
			agi.set_variable('XIVO_FWD_TYPEVAL1UNC', destunc)
			agi.set_variable('XIVO_FWD_TYPEVAL2UNC', context)
	else:
		agi.set_variable('XIVO_ENABLEUNC', 0)

	if 'fwdbusy' in features_list:
		agi.set_variable('XIVO_ENABLEBUSY', enablebusy)

		if enablebusy == 1:
			# See the comment concerning the fwdunc feature.
			agi.set_variable('XIVO_FWD_TYPEBUSY', 'user')
			agi.set_variable('XIVO_FWD_TYPEVAL1BUSY', destbusy)
			agi.set_variable('XIVO_FWD_TYPEVAL2BUSY', context)
		else:
			handler.ds_set_fwd_vars(id, 'busy', 'user', 'XIVO_FWD_TYPEBUSY', 'XIVO_FWD_TYPEVAL1BUSY', 'XIVO_FWD_TYPEVAL2BUSY')
	else:
		agi.set_variable('XIVO_FWD_TYPEBUSY', 'endcall')
		agi.set_variable('XIVO_FWD_TYPEVAL1BUSY', 'none')

	if 'fwdrna' in features_list:
		if enablerna:
			# See the comment concerning the fwdunc feature.
			agi.set_variable('XIVO_FWD_TYPERNA', 'user')
			agi.set_variable('XIVO_FWD_TYPEVAL1RNA', destrna)
			agi.set_variable('XIVO_FWD_TYPEVAL2RNA', context)
		else:
			handler.ds_set_fwd_vars(id, 'noanswer', 'user', 'XIVO_FWD_TYPERNA', 'XIVO_FWD_TYPEVAL1RNA', 'XIVO_FWD_TYPEVAL2RNA')
	else:
		agi.set_variable('XIVO_FWD_TYPERNA', 'endcall')
		agi.set_variable('XIVO_FWD_TYPEVAL1RNA', 'none')

	handler.ds_set_fwd_vars(id, 'congestion', 'user', 'XIVO_FWD_TYPECONGESTION', 'XIVO_FWD_TYPEVAL1CONGESTION', 'XIVO_FWD_TYPEVAL2CONGESTION')
	handler.ds_set_fwd_vars(id, 'chanunavail', 'user', 'XIVO_FWD_TYPEUNAVAIL', 'XIVO_FWD_TYPEVAL1UNAVAIL', 'XIVO_FWD_TYPEVAL2UNAVAIL')

	if 'incallrec' in features_list and callrecord:
		agi.set_variable('XIVO_CALLRECORDFILE', "/usr/share/asterisk/sounds/web-interface/monitor/user-%s-%s-%s.wav" % (srcnum, dstnum, int(time.time())))

	if musiconhold:
		agi.set_variable('MUSICCLASS()', musiconhold)

agid.register(incoming_user_set_features)
