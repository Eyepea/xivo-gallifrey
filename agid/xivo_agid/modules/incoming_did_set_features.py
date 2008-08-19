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

from xivo_agid import agid
from xivo_agid import objects

def incoming_did_set_features(agi, cursor, args):
	exten_pattern = agi.get_variable('XIVO_EXTENPATTERN')
	context = agi.get_variable('XIVO_CONTEXT')

	did = objects.DID(agi, cursor, exten = exten_pattern, context = context)

	agi.set_variable('XIVO_FAXDETECT_ENABLE', did.faxdetectenable)
	agi.set_variable('XIVO_FAXDETECT_TIMEOUT', did.faxdetecttimeout)
	agi.set_variable('XIVO_FAXDETECT_EMAIL', did.faxdetectemail)

	did.set_dial_actions()

agid.register(incoming_did_set_features)
