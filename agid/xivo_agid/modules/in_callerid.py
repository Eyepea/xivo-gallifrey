__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

RULES_FILE = '/etc/asterisk/xivo_in_callerid.conf'

import re
import sys
import logging
import ConfigParser

from xivo import OrderedConf

from xivo_agid import agid

log = logging.getLogger('xivo_agid.modules.in_callerid')

config = None
re_objs = {}

def in_callerid(agi, cursor, args):
    callerid_num = agi.env['agi_callerid']

    for section in config:
        section_name = section.get_name()
        re_obj = re_objs[section_name]

        if not re_obj.match(callerid_num):
            continue

        # We got a match.
        if section.has_option('strip'):
            str_strip = section.get('strip')

            if str_strip.isdigit():
                strip = int(str_strip)

                if strip > 0:
                    callerid_num = callerid_num[strip:]

        if section.has_option('add'):
            add = section.get('add')

            if add:
                callerid_num = add + callerid_num

        agi.set_variable('CALLERID(num)', "%s" % callerid_num)

        return

def setup(cursor):
    global config

    re_objs.clear()
    config = OrderedConf.OrderedRawConf(filename=RULES_FILE)

    for section in config:
        try:
            regexp = section.get('callerid')
        except ConfigParser.NoOptionError:
            log.error("option 'callerid' not found in section %r", section.get_name())
            sys.exit(1)

        try:
            re_obj = re.compile(regexp)
        except re.error:
            log.error("invalid regexp %r in section %r", regexp, section.get_name())
            sys.exit(1)

        re_objs[section.get_name()] = re_obj

agid.register(in_callerid, setup)
