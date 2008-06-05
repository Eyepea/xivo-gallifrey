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

import csv
import md5
import urllib
from xivo_log import *

def log_debug(level, text):
        log_debug_file(level, text, 'urllist')

class UrlList:
        def __init__(self, url):
                self.list = {}
                self.url = url
                self.urlmd5 = ''
                return

        def getlist(self, index, length):
                try:
                        if self.url is not None:
                                kind = self.url.split(':')[0]
                                if kind == 'file' or kind == self.url:
                                        f = urllib.urlopen(self.url)
                                elif kind in ['mysql', 'sqlite', 'ldap']:
                                        log_debug(SYSLOG_WARNING, 'URL kind %s not supported yet' % kind)
                                elif kind in ['http', 'https']:
                                        f = urllib.urlopen(self.url + "?sum=%s" % self.urlmd5)
                                else:
                                        log_debug(SYSLOG_WARNING, 'URL kind %s not supported' % kind)
                                        return -1
                        else:
                                log_debug(SYSLOG_WARNING, 'No URL has been defined')
                                return -1
                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- (UrlList) unable to open URL %s : %s" %(self.url, str(exc)))
                        return -1

                try:
                        mytab = []
                        for line in f:
                                mytab.append(line)
                        f.close()
                        fulltable = ''.join(mytab)
                        savemd5 = self.urlmd5
                        self.listmd5 = md5.md5(fulltable).hexdigest()
                        csvreader = csv.reader(mytab, delimiter = '|')
                        # builds the users list
                        for line in csvreader:
                                if len(line) == length:
                                        self.list[line[index]] = line
                                elif len(line) == 1:
                                        if line[0] == 'XIVO-WEBI: no-data':
                                                log_debug(SYSLOG_INFO, 'received no-data from WEBI')
                                        elif line[0] == 'XIVO-WEBI: no-update':
                                                log_debug(SYSLOG_INFO, 'received no-update from WEBI')
                                                self.urlmd5 = savemd5
                                else:
                                        # log_debug(SYSLOG_WARNING, 'unallowed line length %d' % len(line))
                                        pass
                        f.close()
                except Exception, exc:
                        log_debug(SYSLOG_ERR, "--- exception --- (UrlList) problem occured when retrieving list : %s" % str(exc))
                        return -1

                return 0
