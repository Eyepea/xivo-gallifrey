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

import os
import time
import ConfigParser

from xivo_agid import agid

CONFIG_FILE = '/etc/asterisk/xivo_callback_on_congestion.conf'

max_retries = None
retry_time = None
wait_time = None

def callback_on_congestion(handler, agi, cursor, args):
	srcnum = agi.get_variable('REAL_SRCNUM')
	dstnum = agi.get_variable('REAL_DSTNUM')
	context = agi.get_variable('REAL_CONTEXT')

	mtime = time.time()
	filename = "%s-to-%s-%s.call" % (srcnum, dstnum, int(mtime))

	# TODO fetch path from configuration file.
	tmpfile = "/var/spool/asterisk/tmp/" + filename
	realfile = "/var/spool/asterisk/outgoing/" + filename

	f = open(tmpfile, 'w')
	f.write("Channel: Local/%s\n"
		"MaxRetries: %d\n"
		"RetryTime: %d\n"
		"WaitTime: %d\n"
		"CallerID: %s\n"
		"Context: %s\n"
		"Extension: %s\n"
		"Priority: 1\n" % (srcnum, max_retries, retry_time, wait_time, srcnum, context, dstnum))
	f.close()

	os.utime(tmpfile, (mtime, mtime))
	os.rename(tmpfile, realfile)

def setup(cursor):
	global max_retries
	global retry_time
	global wait_time

	config = ConfigParser.RawConfigParser()
	config.readfp(open(CONFIG_FILE))
	max_retries = config.getint('general', 'max_retries')
	retry_time = config.getint('general', 'retry_time')
	wait_time = config.getint('general', 'wait_time')

agid.register(callback_on_congestion, setup)
