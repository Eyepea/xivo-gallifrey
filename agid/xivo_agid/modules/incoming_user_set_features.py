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
from xivo_agid import objects

def incoming_user_set_features(agi, cursor, args):
	userid = agi.get_variable('XIVO_USERID')
	dstid = int(agi.get_variable('XIVO_DSTID'))
	context = agi.get_variable('XIVO_CONTEXT')
	zone = agi.get_variable('XIVO_CALLORIGIN')
	bypass_filter = agi.get_variable('XIVO_CALLFILTER_BYPASS')

	srcnum = agi.get_variable('XIVO_SRCNUM')
	# FIXME: this is only for the callrecord feature, which is likely to change
	dstnum = agi.get_variable('XIVO_DSTNUM')

	feature_list = objects.FeatureList(agi, cursor)

	try:
		if userid:
			caller = objects.User(agi, cursor, feature_list, xid=int(userid))
		elif zone == "intern":
			# FIXME: lookup ids in a central point at the start of the dialplan, then remove this case
			caller = objects.User(agi, cursor, feature_list, number=srcnum, context=context)
		else:
			caller = None
	except LookupError:
		caller = None

	user = objects.User(agi, cursor, feature_list, xid = dstid)
	ufilter = user.filter

	# Special case. If a boss-secretary filter is set, the code will prematurely
	# exit because the other normally set variables are skipped.
	if not bypass_filter and ufilter and ufilter.active:
		zone_applies = ufilter.check_zone(zone)
		# CHECK does get_secretary_by_id takes an int or a string?
		if caller:
			secretary = ufilter.get_secretary_by_id(caller.id)
		else:
			secretary = None

		if zone_applies and not secretary:
			if ufilter.mode in ("bossfirst-simult", "bossfirst-serial", "all"):
				agi.set_variable('XIVO_CALLFILTER_BOSS_INTERFACE', ufilter.boss.interface)
				agi.set_variable('XIVO_CALLFILTER_BOSS_TIMEOUT', ufilter.boss.ringseconds)

			if ufilter.mode in ("bossfirst-simult", "secretary-simult", "all"):
				interface = '&'.join(secretary.interface for secretary in ufilter.secretaries if secretary.active)
				agi.set_variable('XIVO_CALLFILTER_INTERFACE', interface)
				agi.set_variable('XIVO_CALLFILTER_TIMEOUT', ufilter.ringseconds)
			elif ufilter.mode in ("bossfirst-serial", "secretary-serial"):
				index = 0

				for secretary in ufilter.secretaries:
					if secretary.active:
						agi.set_variable('XIVO_CALLFILTER_SECRETARY%d_INTERFACE' % (index,), secretary.interface)
						agi.set_variable('XIVO_CALLFILTER_SECRETARY%d_TIMEOUT' % (index,), secretary.ringseconds)
						index += 1

			objects.DialAction(agi, cursor, 'noanswer', 'callfilter', ufilter.id).set_variables()

			if ufilter.callerdisplay:
				agi.set_variable('CALLERID(name)', "%s - %s" % (ufilter.callerdisplay, agi.get_variable("CALLERID(name)")))

			agi.set_variable('XIVO_CALLFILTER_MODE', ufilter.mode)
			agi.set_variable('XIVO_CALLFILTER', '1')
			return

	options = ""

	if user.enablexfer:
		options += "t"

	if caller and caller.enablexfer:
		options += "T"

	if user.enableautomon:
		options += "w"

	if caller and caller.enableautomon:
		options += "W"

	if feature_list.incallfilter and user.callfilter:
		options += "p"

	agi.set_variable('XIVO_CALLOPTIONS', options)
	agi.set_variable('XIVO_INTERFACE', user.interface)
	agi.set_variable('XIVO_SIMULTCALLS', user.simultcalls)

	if user.ringseconds > 0:
		agi.set_variable('XIVO_RINGSECONDS', user.ringseconds)

	if feature_list.enablednd:
		agi.set_variable('XIVO_ENABLEDND', user.enablednd)
	else:
		agi.set_variable('XIVO_ENABLEDND', 0)

	if feature_list.enablevm:
		agi.set_variable('XIVO_ENABLEVOICEMAIL', user.enablevoicemail)

		if user.vmbox:
			agi.set_variable('XIVO_MAILBOX', user.vmbox.mailbox)
			agi.set_variable('XIVO_MAILBOX_CONTEXT', user.vmbox.context)

			if user.vmbox.email:
				agi.set_variable('XIVO_USEREMAIL', user.vmbox.email)
	else:
		agi.set_variable('XIVO_ENABLEVOICEMAIL', 0)

	if feature_list.fwdunc:
		agi.set_variable('XIVO_ENABLEUNC', user.enableunc)
		
		if user.enableunc:
			agi.set_variable('XIVO_FWD_USER_UNC_ACTION', 'extension')
			agi.set_variable('XIVO_FWD_USER_UNC_ACTIONARG1', user.destunc)
			agi.set_variable('XIVO_FWD_USER_UNC_ACTIONARG2', context)
	else:
		agi.set_variable('XIVO_ENABLEUNC', 0)

	if feature_list.fwdbusy:
		agi.set_variable('XIVO_ENABLEBUSY', user.enablebusy)

		if user.enablebusy:
			agi.set_variable('XIVO_FWD_USER_BUSY_ACTION', 'extension')
			agi.set_variable('XIVO_FWD_USER_BUSY_ACTIONARG1', user.destbusy)
			agi.set_variable('XIVO_FWD_USER_BUSY_ACTIONARG2', context)
		else:
			objects.DialAction(agi, cursor, 'busy', 'user', user.id).set_variables()
	else:
		agi.set_variable('XIVO_FWD_USER_BUSY_ACTION', 'none')

	if feature_list.fwdrna:
		agi.set_variable('XIVO_ENABLERNA', user.enablerna)

		if user.enablerna:
			agi.set_variable('XIVO_FWD_USER_NOANSWER_ACTION', 'extension')
			agi.set_variable('XIVO_FWD_USER_NOANSWER_ACTIONARG1', user.destrna)
			agi.set_variable('XIVO_FWD_USER_NOANSWER_ACTIONARG2', context)
		else:
			objects.DialAction(agi, cursor, 'noanswer', 'user', user.id).set_variables()
	else:
		agi.set_variable('XIVO_FWD_USER_RNA_ACTION', 'none')

	objects.DialAction(agi, cursor, 'congestion', 'user', user.id).set_variables()
	objects.DialAction(agi, cursor, 'chanunavail', 'user', user.id).set_variables()

	if feature_list.incallrec and user.callrecord:
		# BUGBUG the context is missing in the filename TODO use ids
		agi.set_variable('XIVO_CALLRECORDFILE', "user-%s-%s-%s.wav" % (srcnum, dstnum, int(time.time())))

	if user.musiconhold:
		agi.set_variable('CHANNEL(musicclass)', user.musiconhold)

agid.register(incoming_user_set_features)
