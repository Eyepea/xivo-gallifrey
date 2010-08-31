# vim: set fileencoding=utf-8 :
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
import logging
import urllib
import xmlrpclib

log = logging.getLogger('extrarequests')

def getvariables(fileuri, itemdir):
    k = urllib.urlopen(fileuri)
    json_c = k.read()
    k.close()
    jc = cjson.decode(json_c)
    myret = {}
    method = jc.get('method')
    if method == 'xmlrpc':
        accessdefs = jc.get('access')
        if 'url' in accessdefs and accessdefs.get('url'):
            nv = []
            for mm in jc.get('inputs'):
                for kk, vv in itemdir.iteritems():
                    if vv is not None:
                        if not isinstance(vv, list):
                            mm = mm.replace('{%s}' % kk, vv)
                nv.append(mm)
            vardest = 'extra-%s' % jc.get('result')

            basename = accessdefs.get('basename')
            loginname = accessdefs.get('loginname')
            xname = accessdefs.get('xname')
            function = jc.get('function')

            pp = xmlrpclib.ServerProxy(accessdefs.get('url'))
            if pp:
                myret[vardest] = pp.execute((basename), 1, loginname, xname, function, *nv)
    else:
        log.warning('unknown method %s' % method)
    return myret
