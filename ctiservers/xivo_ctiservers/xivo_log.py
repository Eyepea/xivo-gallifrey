# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# Alternatively, XIVO Daemon is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XIVO Daemon
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, you will find one at
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.

"""
Simple wrapper for syslogs
"""

import time
from xivo.easyslog import *

## \brief Logs actions to a log file, prepending them with a timestamp.
# \param string the string to log
# \return zero
# \sa log_debug

MAXLOG = SYSLOG_NOTICE
# MAXLOG = SYSLOG_INFO
evtfile = False
# evtfile = open('/var/log/xivo_daemon_ami_events.log', 'a')

def varlog(syslogprio, string):
        if syslogprio <= MAXLOG:
                try:
                        syslogf(syslogprio, 'xivo_daemon : ' + string)
                except Exception, exc:
                        syslogf(syslogprio, '--- exception --- in varlog : xivo_daemon : %s' % str(exc))
        return 0

# reminder :
# LOG_EMERG       0
# LOG_ALERT       1
# LOG_CRIT        2
# LOG_ERR         3
# LOG_WARNING     4
# LOG_NOTICE      5
# LOG_INFO        6
# LOG_DEBUG       7


## \brief Logs all events or status updates to a log file, prepending them with a timestamp.
# \param string the string to log
# \param events log to events file
# \param updatesgui log to gui files
# \return zero
def verboselog(string, events, updatesgui):
        if evtfile and events:
                try:
                        evtfile.write(time.strftime('%b %2d %H:%M:%S ', time.localtime()) + string + '\n')
                except Exception, exc:
                        log_debug_file(SYSLOG_ERR, '--- exception --- (verboselog) : %s' % exc, 'xivo_log')
##        if updatesgui and guifile:
##                guifile.write(time.strftime('%b %2d %H:%M:%S ', time.localtime()) + string + '\n')
##                guifile.flush()
        return 0

## \brief Outputs a string to stdout in no-daemon mode
# and always logs it.
# \param string the string to display and log
# \return the return code of the varlog call
# \sa varlog
def log_debug(syslogprio, string):
        if syslogprio <= SYSLOG_INFO:
                print '#debug# ' + string
        return varlog(syslogprio, string)

def log_debug_file(syslogprio, string, filename):
        if syslogprio <= SYSLOG_INFO:
                print '#debug# (%s) ' % filename + string
        return varlog(syslogprio, '(%s) ' % filename + string)
