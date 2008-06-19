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

import ConfigParser

from xivo_agid import agid

CONFIG_FILE = "/etc/asterisk/xivo_ring.conf"

config = None

def getring(agi, cursor, args):
	dstnum = agi.get_variable('REAL_DSTNUM')
	context = agi.get_variable('REAL_CONTEXT')
	calltype = agi.get_variable('REAL_CALLTYPE')

	try:
		phonetype = config.get('number', "%s@%s" % (dstnum, context))
		ringtype = config.get(phonetype, calltype)
		agi.set_variable('XIVO_RINGTYPE', ringtype)
		agi.verbose("Using ring tone %s" % ringtype)

		# XXX Should special cases such as this one be handled here ?
		# XXX Looks like the dial plan is broken and commented out
		# with regard to the use of this variable, consider this
		# useless, it will probably be removed soon.
		if phonetype == 'thomson':
			agi.set_variable("XIVO_ISTHOMSON", "1")
	except ConfigParser.NoOptionError:
		agi.verbose("Using the phone ring tone")

def setup(cursor):
	global config

	# This module is often called, keep this object alive.
	config = ConfigParser.RawConfigParser()
	config.readfp(open(CONFIG_FILE))

agid.register(getring, setup)
