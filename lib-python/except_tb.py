"""Exception logging

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

import sys, traceback

from easyslog import *

def LOGLINE_STDERR(x):
	print >> sys.stderr, x

def exception_traceback():
	"""Returns a backtrace of the current exception in a list of strings,
	not terminated by newlines.
	
	"""
	return map(lambda x: x.rstrip(), traceback.format_exception(*sys.exc_info()))

def log_exception(logline_func = LOGLINE_STDERR, noclear = False):
	"""Log the current exception using the passed 'logline_func' function.
	'logline_func' will be called one line at a time, without any trailing
	\\n (or \\r). Clear the current exception at end of command if
	'noclear' is False. """
	for x in exception_traceback():
		logline_func(x)
	if not noclear:
		sys.exc_clear()

def syslog_exception(loglevel=SYSLOG_ERR, noclear=False):
	"""Log a backtrace of the current exception in the system logs, with
	the desired log level. Clear the current exception at end of command
	if 'noclear' is False.
	
	"""
	log_exception(lambda x:syslogf(loglevel,x), noclear)

__all__ = [ 'exception_traceback', 'log_exception', 'syslog_exception',
            'LOGLINE_STDERR' ]
