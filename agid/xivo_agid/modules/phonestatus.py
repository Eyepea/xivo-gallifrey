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

def phonestatus(agi, cursor, args):
	srcnum = agi.get_variable('REAL_SRCNUM')
	context = agi.get_variable('REAL_CONTEXT')

	feature_list = objects.FeatureList(agi, cursor)
	user = objects.User(agi, cursor, feature_list, number = srcnum, context = context)

	if feature_list.fwdunc:
		agi.set_variable('XIVO_ENABLEUNC', user.enableunc)

		if user.enableunc:
			agi.set_variable('XIVO_DESTUNC', user.destunc)

	if feature_list.fwdbusy:
		agi.set_variable('XIVO_ENABLEBUSY', user.enablebusy)

		if user.enablebusy:
			agi.set_variable('XIVO_DESTBUSY', user.destbusy)

	if feature_list.fwdrna:
		agi.set_variable('XIVO_ENABLERNA', user.enablerna)

		if user.enablerna:
			agi.set_variable('XIVO_DESTRNA', user.destrna)

	if feature_list.enablevm:
		agi.set_variable('XIVO_ENABLEVOICEMAIL', user.enablevoicemail)

	if feature_list.incallfilter:
		agi.set_variable('XIVO_CALLFILTER', user.callfilter)

	if feature_list.incallrec:
		agi.set_variable('XIVO_CALLRECORD', user.callrecord)

	if feature_list.enablednd:
		agi.set_variable('XIVO_ENABLEDND', user.enablednd)

agid.register(phonestatus)
