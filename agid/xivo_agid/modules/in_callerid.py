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

RULES_FILE = '/etc/asterisk/xivo_in_callerid.conf'

import re

from xivo import OrderedConf

from xivo_agid import agid

import ConfigParser

config = None
re_objs = {}

def in_callerid(handler, agi, cursor, args):
	callerid_num = agi.env['agi_callerid']

	for section in config:
		section_name = section.get_name()
		re_obj = re_objs[section_name]

		if not re_obj.match(callerid_num):
			continue

		# We got a match.
		if section.has_option('strip'):
			str_strip = section.get('strip')

			try:
				strip = int(str_strip)
			except ValueError:
				strip = 0

			if strip > 0:
				callerid_num = callerid_num[strip:]
				agi.set_variable('CALLERID(num)', callerid_num)

		if section.has_option('add'):
			add = section.get('add')

			if add:
				callerid_num = add + callerid_num
				agi.set_variable("CALLERID(num)", callerid_num)

		return

def setup(cursor):
	global config

	re_objs.clear()
	config = OrderedConf.OrderedRawConf(filename=RULES_FILE)

	for section in config:
		try:
			regexp = section.get('callerid')
		except ConfigParser.NoOptionError:
			agid.error("option 'callerid' not found in section \"%s\"" % section.get_name())

		try:
			re_obj = re.compile(regexp)
		except re.error:
			agid.error("invalid regexp \"%s\" in section \"%s\"" % (regexp, section.get_name()))

		re_objs[section.get_name()] = re_obj

agid.register(in_callerid, setup)
