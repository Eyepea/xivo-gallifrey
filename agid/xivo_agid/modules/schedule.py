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

def forgetimefield(start, end):
	if start == '*':
		return '*'
	else:
		if end in (None, ''):
			return '%s' % start
		else:
			return '%s-%s' % (start, end)

def forgetime(res):
	return ','.join((
		forgetimefield(res['timebeg'], res['timeend']),
		forgetimefield(res['daynamebeg'], res['daynameend']),
		forgetimefield(res['daynumbeg'], res['daynumend']),
		forgetimefield(res['monthbeg'], res['monthend']),
	))

def schedule(handler, agi, cursor, args):
	id = args[0]

	cursor.query("SELECT ${columns} FROM schedule "
		     "WHERE id = %s "
		     "AND linked = 1 "
		     "AND commented = 0",
		     ('timebeg', 'timeend', 'daynamebeg', 'daynameend', 'daynumbeg', 'daynumend', 'monthbeg', 'monthend', 'typetrue', 'typevaltrue', 'applicationvaltrue', 'typefalse', 'typevalfalse', 'applicationvalfalse'),
		     (id,))
	res = cursor.fetchone()

	if not res:
		agi.dp_break("Invalid schedule ID '%s'" % id)

	agi.set_variable("XIVO_SCHEDULE_TIMERANGE", forgetime(res))
	handler.set_fwd_vars(res['typetrue'], res['typevaltrue'], res['applicationvaltrue'], 'XIVO_SCHEDULE_TYPETRUE', 'XIVO_SCHEDULE_TYPEVAL1TRUE', 'XIVO_SCHEDULE_TYPEVAL2TRUE')
	handler.set_fwd_vars(res['typefalse'], res['typevalfalse'], res['applicationvalfalse'], 'XIVO_SCHEDULE_TYPEFALSE', 'XIVO_SCHEDULE_TYPEVAL1FALSE', 'XIVO_SCHEDULE_TYPEVAL2FALSE')

agid.register(schedule)
