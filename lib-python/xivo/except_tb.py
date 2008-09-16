"""Exception logging

Copyright (C) 2007, 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

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

import sys
import traceback

from xivo.easyslog import *

def LOGLINE_STDERR(x):
	print >> sys.stderr, x

def SYSLOG_EXCEPT(loglevel):
	return lambda x: syslogf(loglevel, x)

def exception_raw_traceback():
	"""
	Return a backtrace of the current exception in a list of strings,
	terminated by newlines.
	"""
	return traceback.format_exception(*sys.exc_info())

def exception_traceback():
	"""
	Return a backtrace of the current exception in a list of strings,
	not terminated by newlines.
	"""
	return [ x.rstrip() for x in exception_raw_traceback() ]

def log_full_exception(logfull_func = None, logline_func = None, noclear = False):
	"""
	Log the current exception using the 'logline_func' for each right
	stripped line first, then the whole traceback in a single multiline
	string using the 'logfull_func'
	The current exception is cleared before the function returns if
	'noclear' is False.
	"""
	tb_line_list = exception_raw_traceback()
	if logline_func:
		for line in tb_line_list:
			logline_func(line.rstrip())
	if logfull_func:
		logfull_func(''.join(tb_line_list))
	if not noclear:
		sys.exc_clear()

def log_exception(logline_func = LOGLINE_STDERR, noclear = False):
	"""
	Log the current exception using the passed 'logline_func' function.
	'logline_func' will be called one line at a time, without any trailing
	\\n (or \\r)
	The current exception is cleared before the function returns if
	'noclear' is False.
	"""
	for x in exception_traceback():
		logline_func(x)
	if not noclear:
		sys.exc_clear()

def syslog_exception(loglevel=SYSLOG_ERR, noclear = False):
	"""
	Log a backtrace of the current exception in the system logs, with
	the desired log level.
	The current exception is cleared before the function returns if
	'noclear' is False.
	"""
	log_exception(SYSLOG_EXCEPT(loglevel), noclear)

__all__ = ( 'exception_traceback', 'log_exception', 'syslog_exception', 'log_full_exception', 'LOGLINE_STDERR', 'SYSLOG_EXCEPT' )
