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

import ConfigParser

from xivo_agid import agid

CONFIG_FILE     = "/etc/asterisk/xivo_ring.conf"
CONFIG_PARSER   = None

def getring(agi, cursor, args):
    dstnum              = agi.get_variable('XIVO_REAL_NUMBER')
    context             = agi.get_variable('XIVO_REAL_CONTEXT')
    origin              = agi.get_variable('XIVO_CALLORIGIN')
    referer             = agi.get_variable('XIVO_FWD_REFERER').split(':', 1)[0]
    forwarded           = agi.get_variable('XIVO_CALLFORWARDED')
    # TODO: maybe replace number@context with user id in conf file ?
    dstnum_context      = "%s@%s" % (dstnum, context)
    referer_origin      = "%s@%s" % (referer, origin)
    origin_fwd          = "%s&forwarded" % origin
    referer_origin_fwd  = "%s&forwarded" % referer_origin
    section             = None

    if CONFIG_PARSER.has_option('number', "!%s" % dstnum_context):
        agi.set_variable('XIVO_RINGTYPE', "")
        return

    if len(dstnum) > 0 and CONFIG_PARSER.has_option('number', dstnum_context):
        section = CONFIG_PARSER.get('number', dstnum_context)

    try:
        if section is None:
            section = CONFIG_PARSER.get('number', "@%s" % context)

        if section == 'number':
            raise ValueError("Invalid section name")

        if forwarded == '1' and CONFIG_PARSER.has_option(section, referer_origin_fwd):
            ringtype = CONFIG_PARSER.get(section, referer_origin_fwd)
        elif CONFIG_PARSER.has_option(section, referer_origin):
            ringtype = CONFIG_PARSER.get(section, referer_origin)
        elif forwarded == '1' and CONFIG_PARSER.has_option(section, origin_fwd):
            ringtype = CONFIG_PARSER.get(section, origin_fwd)
        elif forwarded == '1' and CONFIG_PARSER.has_option(section, 'forward'):
            ringtype = CONFIG_PARSER.get(section, 'forward')
        else:
            ringtype = CONFIG_PARSER.get(section, origin)

        phonetype = CONFIG_PARSER.get(section, 'phonetype')
    except (ConfigParser.NoOptionError, ValueError):
        agi.set_variable('XIVO_RINGTYPE', "")
        agi.verbose("Using the native phone ring tone")
    else:
        agi.set_variable('XIVO_RINGTYPE', ringtype)
        agi.set_variable('XIVO_PHONETYPE', phonetype)
        agi.verbose("Using ring tone %s" % (ringtype,))

def setup(cursor):
    global CONFIG_PARSER

    # This module is often called, keep this object alive.
    CONFIG_PARSER = ConfigParser.RawConfigParser()
    CONFIG_PARSER.readfp(open(CONFIG_FILE))

agid.register(getring, setup)
