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

def incoming_group_set_features(handler, agi, cursor, args):
	dstnum = agi.get_variable('REAL_DSTNUM')
	context = agi.get_variable('REAL_CONTEXT')

	cursor.query("SELECT ${columns} FROM groupfeatures "
                     "INNER JOIN queue "
                     "ON groupfeatures.name = queue.name "
                     "WHERE groupfeatures.number = %s "
                     "AND groupfeatures.context = %s "
                     "AND groupfeatures.deleted = 0 "
                     "AND queue.commented = 0 "
                     "AND queue.category = 'group'",
                     ('groupfeatures.id', 'groupfeatures.name', 'groupfeatures.timeout', 'groupfeatures.transfer_user', 'groupfeatures.transfer_call', 'groupfeatures.write_caller', 'groupfeatures.write_calling', 'queue.musiconhold'),
                     (dstnum, context))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unknown group number '%s'" % dstnum)

	id = res['groupfeatures.id']
	name = res['groupfeatures.name']
	timeout = res['groupfeatures.timeout']
	options = ""

	if res['groupfeatures.transfer_user']:
		options += "t"

	if res['groupfeatures.transfer_call']:
		options += "T"

	if res['groupfeatures.write_caller']:
		options += "w"

	if res['groupfeatures.write_calling']:
		options += "W"

	if not res['queue.musiconhold']:
		options += "r"

	agi.set_variable('XIVO_GROUPNAME', name)
	agi.set_variable('XIVO_GROUPOPTIONS', options)

	if timeout:
		agi.set_variable('XIVO_GROUPTIMEOUT', timeout)

	handler.ds_set_fwd_vars(id, 'busy', 'group', 'XIVO_FWD_TYPEBUSY', 'XIVO_FWD_TYPEVAL1BUSY', 'XIVO_FWD_TYPEVAL2BUSY')
	handler.ds_set_fwd_vars(id, 'noanswer', 'group' 'XIVO_FWD_TYPERNA', 'XIVO_FWD_TYPEVAL1RNA', 'XIVO_FWD_TYPEVAL2RNA')
	handler.ds_set_fwd_vars(id, 'congestion', 'group', 'XIVO_FWD_TYPECONGESTION', 'XIVO_FWD_TYPEVAL1CONGESTION', 'XIVO_FWD_TYPEVAL2CONGESTION')
	handler.ds_set_fwd_vars(id, 'chanunavail', 'group', 'XIVO_FWD_TYPEUNAVAIL', 'XIVO_FWD_TYPEVAL1UNAVAIL', 'XIVO_FWD_TYPEVAL2UNAVAIL')

agid.register(incoming_group_set_features)
