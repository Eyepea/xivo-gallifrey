# -*- coding: iso-8859-15 -*-

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

import syslog
from syslog import syslog      as syslogf
from syslog import LOG_EMERG   as SYSLOG_EMERG
from syslog import LOG_ALERT   as SYSLOG_ALERT
from syslog import LOG_CRIT    as SYSLOG_CRIT
from syslog import LOG_ERR     as SYSLOG_ERR
from syslog import LOG_WARNING as SYSLOG_WARNING
from syslog import LOG_NOTICE  as SYSLOG_NOTICE
from syslog import LOG_INFO    as SYSLOG_INFO
from syslog import LOG_DEBUG   as SYSLOG_DEBUG

syslogmap = {
	'emerg':   SYSLOG_EMERG,
	'alert':   SYSLOG_ALERT,
	'crit':    SYSLOG_CRIT,
	'err':     SYSLOG_ERR,
	'warning': SYSLOG_WARNING,
	'notice':  SYSLOG_NOTICE,
	'info':    SYSLOG_INFO,
	'debug':   SYSLOG_DEBUG
}

def sysloglevel_from_str(v):
	if v.lower() not in syslogmap:
		raise ValueError, "Unknown log filter '%s'" % v
	return syslogmap[v.lower()]

__all__ = [
	'syslogf', 'SYSLOG_EMERG', 'SYSLOG_ALERT', 'SYSLOG_CRIT',
	'SYSLOG_ERR', 'SYSLOG_WARNING', 'SYSLOG_NOTICE', 'SYSLOG_INFO',
	'SYSLOG_DEBUG', 'syslogmap', 'sysloglevel_from_str'
]
