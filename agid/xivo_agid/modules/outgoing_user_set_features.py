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

def outgoing_user_set_features(agi, cursor, args):
	userid = int(agi.get_variable('XIVO_USERID'))
	srcnum = agi.get_variable('XIVO_SRCNUM')
	dstnum = agi.get_variable('XIVO_DSTNUM')
	context = agi.get_variable('XIVO_CONTEXT')
	exten_pattern = agi.get_variable('XIVO_EXTENPATTERN')

	feature_list = objects.FeatureList(agi, cursor)
	outcall = objects.Outcall(agi, cursor, feature_list, exten = exten_pattern, context = context)

	orig_dstnum = dstnum
	callerid = ""
	callrecord = False
	options = ""

	if outcall.stripnum > 0:
		dstnum = dstnum[outcall.stripnum:]

	if outcall.externprefix:
		dstnum = outcall.externprefix + dstnum

	if not outcall.internal:
		try:
			user = objects.User(agi, cursor, feature_list, xid = userid)

			# TODO: Rethink all the caller id stuff.
			if outcall.setcallerid:
				callerid = outcall.callerid
			else:
				callerid = user.callerid

			if callerid == "default":
				callerid = ""

			if user.enableautomon:
				options += "W"

			if user.callrecord:
				callrecord = True

	for i, trunk in enumerate(outcall.trunks):
		agi.set_variable('XIVO_INTERFACE%d' % (i,), trunk.interface)

		# XXX numbers of stripped digits and prefix should be set on
		# per-trunk basis instead of per-outcall.
		agi.set_variable('XIVO_TRUNKEXTEN%d' % (i,), dstnum)

		if trunk.intfsuffix:
			agi.set_variable('XIVO_TRUNKSUFFIX%d' % (i,), "/" + trunk.intfsuffix)

	if callerid:
		agi.set_variable('CALLERID(num)', callerid)

	if callrecord:
		if feature_list.incallrec:
			agi.set_variable('XIVO_CALLRECORDFILE', "/usr/share/asterisk/sounds/web-interface/monitor/user-%s-%s-%s.wav" % (srcnum, orig_dstnum, int(time.time())))

	if outcall.hangupringtime:
		agi.set_variable('XIVO_HANGUPRINGTIME', outcall.hangupringtime)

	agi.set_variable('XIVO_CALLOPTIONS', options)

agid.register(outgoing_user_set_features)
