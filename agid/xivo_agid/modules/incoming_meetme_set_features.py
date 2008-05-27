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

def incoming_meetme_set_features(handler, agi, cursor, args):
	dstnum = agi.get_variable('REAL_DSTNUM')
	context = agi.get_variable('REAL_CONTEXT')

	cursor.query("SELECT ${columns} FROM meetmefeatures "
		     "INNER JOIN staticmeetme "
		     "ON meetmefeatures.meetmeid = staticmeetme.id "
		     "WHERE meetmefeatures.number = %s "
		     "AND meetmefeatures.context = %s "
		     "AND staticmeetme.commented = 0",
		     [('meetmefeatures.' + x) for x in ('mode', 'musiconhold', 'poundexit', 'quiet', 'record', 'adminmode', 'announceusercount', 'announcejoinleave', 'alwayspromptpin', 'starmenu', 'enableexitcontext', 'exitcontext')],
		     (dstnum, context))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unknown conference room number '%s'" % dstnum)

	options = ''

	if res['meetmefeatures.mode'] == 'talk':
		options += "t"
	elif res['meetmefeatures.mode'] == 'listen':
		options += "m"
	elif res['meetmefeatures.mode'] != 'all':
		agi.dp_break("Bogus value for conference mode '%s'" % res['meetmefeatures.mode'])

	if res['meetmefeatures.musiconhold']:
		agi.set_variable('MUSICCLASS()', res['meetmefeatures.musiconhold'])
		options += "M"

	if res['meetmefeatures.poundexit']:
		options += "p"

	if res['meetmefeatures.quiet']:
		options += "q"

	if res['meetmefeatures.record']:
		options += "r"

	if res['meetmefeatures.adminmode']:
		options += "a"

	if res['meetmefeatures.announceusercount']:
		options += "c"

	if res['meetmefeatures.announcejoinleave']:
		options += "i"

	if res['meetmefeatures.alwayspromptpin']:
		options += "P"

	if res['meetmefeatures.starmenu']:
		options += "s"

	if res['meetmefeatures.enableexitcontext'] and res['meetmefeatures.exitcontext']:
		options += "X"
		agi.set_variable('MEETME_EXIT_CONTEXT', res['meetmefeatures.exitcontext'])

	agi.set_variable('MEETME_RECORDINGFILE', "/usr/share/asterisk/sounds/web-interface/monitor/meetme-%s-%s" % (dstnum, int(time.time())))
	agi.set_variable('XIVO_MEETMEOPTIONS', options)

agid.register(incoming_meetme_set_features)
