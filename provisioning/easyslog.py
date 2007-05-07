# -*- coding: iso-8859-15 -*-

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
