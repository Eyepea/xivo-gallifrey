__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>

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

CONFIG_FILE = "/etc/asterisk/xivo_ring.conf"

config = None

def getring(agi, cursor, args):
    dstnum = agi.get_variable('XIVO_REAL_NUMBER')
    context = agi.get_variable('XIVO_REAL_CONTEXT')
    callorigin = agi.get_variable('XIVO_CALLORIGIN')

    try:
        # TODO: maybe replace number@context with user id in conf file ?
        phonetype = config.get('number', "%s@%s" % (dstnum, context))
        ringtype = config.get(phonetype, callorigin)
        agi.set_variable('XIVO_RINGTYPE', ringtype)
        agi.set_variable('XIVO_PHONETYPE', phonetype)
        agi.verbose("Using ring tone %s" % (ringtype,))
    except ConfigParser.NoOptionError:
        ringtype = agi.get_variable('XIVO_RINGTYPE')
        if ringtype:
                agi.set_variable('XIVO_RINGTYPE', "")
        agi.verbose("Using the native phone ring tone")

def setup(cursor):
    global config

    # This module is often called, keep this object alive.
    config = ConfigParser.RawConfigParser()
    config.readfp(open(CONFIG_FILE))

agid.register(getring, setup)
