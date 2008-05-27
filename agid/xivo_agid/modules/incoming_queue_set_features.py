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

def incoming_queue_set_features(handler, agi, cursor, args):
	dstnum = agi.get_variable('REAL_DSTNUM')
	context = agi.get_variable('REAL_CONTEXT')

	cursor.query("SELECT ${columns} FROM queuefeatures "
		     "INNER JOIN queue "
		     "ON queuefeatures.name = queue.name "
		     "WHERE queuefeatures.number = %s "
		     "AND queuefeatures.context = %s "
		     "AND queue.commented = 0 "
		     "AND queue.category = 'queue'",
		     [('queuefeatures.' + x) for x in ('name', 'data_quality', 'hitting_callee', 'hitting_caller', 'retries', 'ring', 'transfer_user', 'transfer_call', 'write_caller', 'write_calling', 'url', 'announceoverride', 'timeout')],
		     (dstnum, context))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Unknown queue number '%s'" % dstnum)

	options = ''

	if res['queuefeatures.data_quality']:
		options += "d"

	if res['queuefeatures.hitting_callee']:
		options += "h"

	if res['queuefeatures.hitting_caller']:
		options += "H"

	if res['queuefeatures.retries']:
		options += "n"

	if res['queuefeatures.ring']:
		options += "r"

	if res['queuefeatures.transfer_user']:
		options += "t"

	if res['queuefeatures.transfer_call']:
		options += "T"

	if res['queuefeatures.write_caller']:
		options += "w"

	if res['queuefeatures.write_calling']:
		options += "W"

	agi.set_variable('XIVO_QUEUENAME', res['queuefeatures.name'])
	agi.set_variable('XIVO_QUEUEOPTIONS', options)
	agi.set_variable('XIVO_QUEUEURL', res['queuefeatures.url'])
	agi.set_variable('XIVO_QUEUEANNOUNCEOVERRIDE', res['queuefeatures.announceoverride'])

	if res['queuefeatures.timeout']:
		agi.set_variable('XIVO_QUEUETIMEOUT', res['queuefeatures.timeout'])

	handler.ds_set_fwd_vars(id, 'busy', 'queue', 'XIVO_FWD_TYPEBUSY', 'XIVO_FWD_TYPEVAL1BUSY', 'XIVO_FWD_TYPEVAL2BUSY')
	handler.ds_set_fwd_vars(id, 'noanswer', 'queue', 'XIVO_FWD_TYPERNA', 'XIVO_FWD_TYPEVAL1RNA', 'XIVO_FWD_TYPEVAL2RNA')
	handler.ds_set_fwd_vars(id, 'congestion', 'queue', 'XIVO_FWD_TYPECONGESTION', 'XIVO_FWD_TYPEVAL1CONGESTION', 'XIVO_FWD_TYPEVAL2CONGESTION')
	handler.ds_set_fwd_vars(id, 'chanunavail', 'queue', 'XIVO_FWD_TYPEUNAVAIL', 'XIVO_FWD_TYPEVAL1UNAVAIL', 'XIVO_FWD_TYPEVAL2UNAVAIL')

agid.register(incoming_queue_set_features)
