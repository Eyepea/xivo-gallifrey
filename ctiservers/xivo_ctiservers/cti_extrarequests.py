# vim: set fileencoding=utf-8 :
# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2011 Proformatique'
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
import logging
import time
import urllib
import xmlrpclib

log = logging.getLogger('extrarequests')

def getvariables(fileuri, itemdir):
    myret = {}
    try:
        k = urllib.urlopen(fileuri)
    except IOError, exc:
        log.warning('getvariables %s : unable to open uri (%s)' % (fileuri, exc))
        return myret
    except Exception:
        log.exception('getvariables %s' % fileuri)
        return myret
    json_c = k.read()
    k.close()
    jcs = cjson.decode(json_c)

    t1 = time.time()
    for jc in jcs:
        method = jc.get('method')
        if method == 'xmlrpc':
            accessdefs = jc.get('access')

            nv = []
            for mm in jc.get('inputs'):
                for kk, vv in itemdir.iteritems():
                    if vv is not None:
                        if not isinstance(vv, list):
                            mm = mm.replace('{%s}' % kk, vv)
                nv.append(mm)
            vardest = 'extra-%s' % jc.get('result')

            url = accessdefs.get('url').replace('\/', '/')
            basename = accessdefs.get('basename')
            userid = accessdefs.get('userid') # should be a number
            userpass = accessdefs.get('userpass')
            objectname = accessdefs.get('objectname')
            function = jc.get('function')

            try:
                pp = xmlrpclib.ServerProxy(url)
                myret[vardest] = pp.execute((basename), userid, userpass,
                                            objectname, function, *nv)
            except Exception:
                log.exception('attempt to join %s' % url)
        else:
            log.warning('unknown method %s' % method)
    t2 = time.time()
    log.info('time spent : %f s' % (t2 - t1))
    return myret
