"""Trace to syslog backend

Copyright (C) 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008  Proformatique

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

from xivo.easyslog import *

def err(message):
	syslogf(SYSLOG_ERR, message)

def warning(message):
	syslogf(SYSLOG_WARNING, message)

def notice(message):
	syslogf(SYSLOG_NOTICE, message)

def info(message):
	syslogf(SYSLOG_INFO, message)

def debug(message):
	sysglof(SYSLOG_DEBUG, message)

__all__ = ('err', 'warning', 'notice', 'info', 'debug')
