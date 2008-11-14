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

import cjson
import csv
import logging
import md5
import urllib

log = logging.getLogger('urllist')

class UrlList:
        def __init__(self, url):
                self.list = {}
                self.url = url
                self.trueurl = url.split('?')
                self.urlmd5 = ''
                return

        def getlist(self, index, length, header = False):
                self.list = {}
                contenttype = None
                try:
                        if self.url is not None:
                                kind = self.url.split(':')[0]
                                if kind == 'file' or kind == self.url:
                                        f = urllib.urlopen(self.trueurl[0])
                                        contenttype = f.headers.getheaders('Content-Type')
                                        contenttype = ['application/json']
                                elif kind in ['mysql', 'sqlite', 'ldap']:
                                        log.warning('URL kind %s not supported yet' % kind)
                                elif kind in ['http', 'https']:
                                        request = '%s?sum=%s' % (self.url, self.urlmd5)
                                        f = urllib.urlopen(request)
                                        contenttype = f.headers.getheaders('Content-Type')
                                else:
                                        log.warning('URL kind %s not supported' % kind)
                                        return -1
                        else:
                                log.warning('No URL has been defined')
                                return -1
                except Exception, exc:
                        log.error("--- exception --- (UrlList) unable to open URL %s : %s" %(self.url, str(exc)))
                        return -1

                ret = -1
                try:
                        keys = []
                        mytab = []
                        for line in f:
                                mytab.append(line)
                        f.close()
                        fulltable = ''.join(mytab)
                        savemd5 = self.urlmd5
                        self.listmd5 = md5.md5(fulltable).hexdigest()
                        if contenttype == ['application/json']:
                                self.jsonreply = cjson.decode(fulltable)
                                try:
                                        if len(self.trueurl) > 1:
                                                [var, val] = self.trueurl[1].split('=')
                                                for k in self.jsonreply:
                                                        k[var] = val
                                        self.urlmd5 = self.listmd5
                                except Exception, exc:
                                        log.error('--- exception --- (UrlList) trying to enforce setting %s %s' % (self.url, exc))
                                ret = 2
                        elif contenttype == ['text/html; charset=UTF-8']:
                                if fulltable == 'XIVO-WEBI: Error/403':
                                        log.warning('unauthorized connection (403) to %s' % self.url)
                                elif fulltable == 'XIVO-WEBI: no-update':
                                        self.urlmd5 = savemd5
                                        log.info('%s : received no-update from WEBI' % self.url)
                                        ret = 2
                        else:
                                csvreader = csv.reader(mytab, delimiter = '|')
                                # builds the users list
                                for line in csvreader:
                                        if len(line) == length:
                                                if header and len(keys) == 0:
                                                        keys = line
                                                else:
                                                        if line[index] != '':
                                                                self.list[line[index]] = line
                                        elif len(line) == 1:
                                                if line[0] == 'XIVO-WEBI: no-data':
                                                        log.info('received no-data from WEBI')
                                                elif line[0] == 'XIVO-WEBI: no-update':
                                                        log.info('received no-update from WEBI')
                                                        self.urlmd5 = savemd5
                                        else:
                                                # log.warning('unallowed line length %d' % len(line))
                                                pass
                                ret = 1
                except Exception, exc:
                        log.error('--- exception --- (UrlList) problem occured when retrieving list for %s : %s' % (self.url, exc))
                        ret = -1
                return ret
