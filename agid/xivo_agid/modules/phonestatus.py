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

def phonestatus(handler, agi, cursor, args):
	srcnum = agi.get_variable('REAL_SRCNUM')
	context = agi.get_variable('REAL_CONTEXT')

	cursor.query("SELECT ${columns} FROM userfeatures "
                     "WHERE number = %s "
                     "AND context = %s "
                     "AND internal = 0 "
                     "AND commented = 0",
                     ('enableunc', 'destunc', 'enablebusy', 'destbusy', 'enablerna', 'destrna', 'enablevoicemail', 'callfilter', 'callrecord', 'enablednd'),
                     (srcnum, context))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unknown number '%s'" % srcnum)

	enableunc = res['enableunc']
	destunc = res['destunc']
	enablebusy = res['enablebusy']
	destbusy = res['destbusy']
	enablerna = res['enablerna']
	destrna = res['destrna']
	enablevoicemail = res['enablevoicemail']
	callfilter = res['callfilter']
	callrecord = res['callrecord']
	enablednd = res['enablednd']

	cursor.query("SELECT ${columns} FROM extensions "
                     "WHERE name IN ('fwdunc', 'fwdrna', 'fwdbusy', 'enablevm', 'incallfilter', 'incallrec', 'enablednd') "
                     "AND commented = 0",
                     ('name',))
	res = cursor.fetchall()

	if not res:
		agi.verbose("All features disabled")
		return

	features_list = [row['name'] for row in res]

	if 'fwdunc' in features_list:
		agi.set_variable('XIVO_ENABLEUNC', enableunc)

		if enableunc:
			agi.set_variable('XIVO_DESTUNC', destunc)

	if 'fwdbusy' in features_list:
		agi.set_variable('XIVO_ENABLEBUSY', enablebusy)

		if enablebusy:
			agi.set_variable('XIVO_DESTBUSY', destbusy)

	if 'fwdrna' in features_list:
		agi.set_variable('XIVO_ENABLERNA', enablerna)

		if enablerna:
			agi.set_variable('XIVO_DESTRNA', destrna)

	if 'enablevm' in features_list:
		agi.set_variable('XIVO_ENABLEVOICEMAIL', enablevoicemail)

	if 'incallfilter' in features_list:
		agi.set_variable('XIVO_CALLFILTER', callfilter)

	if 'incallrec' in features_list:
		agi.set_variable('XIVO_CALLRECORD', callrecord)

	if 'enablednd' in features_list:
		agi.set_variable('XIVO_ENABLEDND', enablednd)

agid.register(phonestatus)
