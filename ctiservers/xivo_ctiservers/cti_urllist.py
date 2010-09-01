# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2010 Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, XiVO CTI Server is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XiVO CTI Server
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import cjson
import csv
import logging
import md5
import urllib2

log = logging.getLogger('urllist')

class DefaultErrorHandler(urllib2.HTTPDefaultErrorHandler):
    def http_error_default(self, req, fp, code, msg, headers):
        result = urllib2.HTTPError(req.get_full_url(), code, msg, headers, fp)
        result.status = code
        return result

class UrlList:
    def __init__(self, url):
        self.list = {}
        self.url = url.replace('\\/', '/')
        self.trueurl = self.url.split('?')
        self.urlmd5 = ''
        return

    def getlist(self, index, length, header = False):
        self.list = {}
        http_contenttype = None
        http_code = 200
        try:
            if self.url is not None:
                kind = self.url.split(':')[0]
                if kind == 'file' or kind == self.url:
                    f = urllib2.urlopen(self.trueurl[0])
                    http_contenttype = ['application/json']
                elif kind in ['mysql', 'sqlite', 'ldap']:
                    log.warning('URL kind %s not supported yet' % kind)
                elif kind in ['http', 'https']:
                    request = '%s?sum=%s' % (self.trueurl[0], self.urlmd5)
                    urequest = urllib2.Request(request)
                    opener = urllib2.build_opener(DefaultErrorHandler)
                    f = opener.open(urequest)
                    http_contenttype = f.headers.getheaders('Content-Type')
                    http_code = f.code
                else:
                    log.warning('URL kind %s not supported' % kind)
                    return -1
            else:
                log.warning('No URL has been defined')
                return -1
        except urllib2.URLError, uerr:
            errnum = uerr.reason[0]
            if errnum == 113: # No route to host
                log.error('(UrlList) %s %s : %s' % (self.url, uerr.args[0].__class__, uerr.reason))
            elif errnum == 111: # Connection refused
                log.error('(UrlList) %s %s : %s' % (self.url, uerr.args[0].__class__, uerr.reason))
            else:
                # The connect operation timed out
                # The read operation timed out
                log.exception('(UrlList) %s %s : (untracked) %s' % (self.url, uerr.args[0].__class__, uerr.reason))
            return -1
        except Exception:
            log.exception('(UrlList) unable to open URL %s' % self.url)
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
            if http_code == 200 and http_contenttype == ['application/json']:
                self.jsonreply = cjson.decode(fulltable)
                try:
                    if len(self.trueurl) > 1:
                        [var, val] = self.trueurl[1].split('=')
                        for k in self.jsonreply:
                            k[var] = val
                    self.urlmd5 = self.listmd5
                except Exception:
                    log.exception('(UrlList) trying to enforce setting %s' % self.url)
                ret = 2
            elif http_contenttype == [] or http_contenttype == ['text/html; charset=UTF-8']:
                if http_code == 403:
                    log.warning('%s : Forbidden (403)' % self.url)
                elif http_code == 401:
                    log.warning('%s : Unauthorized (401)' % self.url)
                elif http_code == 404:
                    log.warning('%s : Not Found (404)' % self.url)
                elif http_code == 500:
                    log.warning('%s : Internal Error (500)' % self.url)
                elif http_code == 304:
                    self.urlmd5 = savemd5
                    log.info('%s : received no-update (304) from WEBI' % self.url)
                    ret = 2
                elif http_code == 204:
                    log.info('%s : received no-data (204) from WEBI' % self.url)
                elif http_code == 200: # XXX temporary compatibility
                    if fulltable == 'XIVO-WEBI: no-update':
                        self.urlmd5 = savemd5
                        log.info('%s : received no-update from WEBI' % self.url)
                        ret = 2
                else:
                    log.warning('%s : unknown code (%d)' % (self.url, http_code))
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
        except Exception:
            log.exception('(UrlList) problem occured when retrieving list for %s' % self.url)
            ret = -1
        return ret
