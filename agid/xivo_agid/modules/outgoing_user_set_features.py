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

def outgoing_user_set_features(handler, agi, cursor, args):
	srcnum = agi.get_variable('REAL_SRCNUM')
	dstnum = agi.get_variable('REAL_DSTNUM')
	context = agi.get_variable('REAL_CONTEXT')
	exten_pattern = agi.get_variable('REAL_EXTENPATTERN')

	cursor.query("SELECT ${columns} FROM outcall "
                     "WHERE exten = %s "
                     "AND context = %s "
                     "AND commented = 0",
                     ('id', 'externprefix', 'stripnum', 'setcallerid', 'callerid', 'useenum', 'internal', 'hangupringtime'),
                     (exten_pattern, context))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unable to find outgoing call features (extension pattern = '%s', context = '%s')" % (exten_pattern, context))

	outcallid = res['id']
	externprefix = res['externprefix']
	stripnum = int(res['stripnum'])
	setcallerid = res['setcallerid']
	trunk_callerid = res['callerid']
	internal = res['internal']
	hangupringtime = res['hangupringtime']

	if stripnum > 0:
		dstnum = dstnum[stripnum:]

	if externprefix:
		dstnum = externprefix + dstnum

	callerid = ""
	calloptions = ""
	callrecord = False

	if not internal:
		cursor.query("SELECT ${columns} FROM userfeatures "
                             "WHERE number = %s "
                             "AND context = %s "
                             "AND internal = 0 "
                             "AND commented = 0",
                             ('outcallerid', 'enableautomon', 'callrecord'),
                             (srcnum, context))
		res = cursor.fetchone()

		if res:
			if setcallerid:
				callerid = trunk_callerid
			else:
				callerid = res['outcallerid']

			if callerid == "default":
				callerid = ""

			if res['enableautomon'] == 1:
				calloptions += "W"

			if res['callrecord'] == 1:
				callrecord = True

	cursor.query("SELECT ${columns} FROM trunkfeatures "
                     "INNER JOIN outcalltrunk "
                     "ON trunkfeatures.id = outcalltrunk.trunkfeaturesid "
                     "WHERE outcalltrunk.outcallid = %s "
                     "ORDER BY outcalltrunk.priority ASC",
                     ('trunkfeatures.protocol', 'trunkfeatures.protocolid'),
                     (outcallid,))
	res = cursor.fetchall()

	if not res:
		agi.dp_break("The outgoing call is associated with no trunk")

	trunkindex = 0

	for row in res:
		protocol = row['trunkfeatures.protocol']
		protocolid = row['trunkfeatures.protocolid']

		if protocol == "sip":
			protocol = "SIP"
			cursor.query("SELECT ${columns} FROM usersip WHERE id = %s AND commented = 0",
                                     ('name',),
                                     (protocolid,))
			res = cursor.fetchone()

			if not res:
				agi.dp_break("Unable to find SIP peer (ID = '%s')" % protocolid)

			peer = res['name']
			interface = "%s/%s" % (protocol, peer)
		elif protocol == "iax":
			protocol = "IAX2"
			cursor.query("SELECT ${columns} FROM useriax WHERE id = %s AND commented = 0",
                                     ('name',),
                                     (protocolid,))
			res = cursor.fetchone()

			if not res:
				agi.dp_break("Unable to find IAX peer (ID = '%s')" % protocolid)

			peer = res['name']
			interface = "%s/%s" % (protocol, peer)
		elif protocol == "custom":
			protocol = "CUSTOM"
			cursor.query("SELECT ${columns} FROM usercustom WHERE id = %s AND commented = 0",
                                     ('interface', 'intfsuffix'),
                                     (protocolid,))
			res = cursor.fetchone()

			if not res:
				agi.dp_break("Unable to find custom peer (ID = '%s')" % protocolid)

			interface = res['interface']
			intfsuffix = res['intfsuffix']

			if intfsuffix not in (None, ""):
				agi.set_variable('XIVO_TRUNKSUFFIX%d' % trunkindex, "/" + intfsuffix)
		else:
			agi.dp_break("Unknown protocol '%s'" % protocol)

		agi.set_variable('XIVO_INTERFACE%d' % trunkindex, interface)
		agi.set_variable('XIVO_TRUNKEXTEN%d' % trunkindex, dstnum)
		trunkindex += 1

	if callerid:
		agi.set_variable('CALLERID(num)', callerid)

	if callrecord:
		cursor.query("SELECT ${columns} FROM extensions "
                             "WHERE name = 'incallrec' "
                             "AND commented = 0",
                             ('name',))
		res = cursor.fetchone()

		if res:
			agi.set_variable('XIVO_CALLRECORDFILE', "/usr/share/asterisk/sounds/web-interface/monitor/user-%s-%s-%s.wav" % (srcnum, dstnum, int(time.time())))

	if hangupringtime:
		agi.set_variable('XIVO_HANGUPRINGTIME', hangupringtime)

	agi.set_variable('XIVO_CALLOPTIONS', calloptions)

agid.register(outgoing_user_set_features)
