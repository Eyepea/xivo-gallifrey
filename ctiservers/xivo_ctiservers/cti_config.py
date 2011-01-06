# XiVO CTI Server
# vim: set fileencoding=utf-8

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

from xivo import anysql
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite
import logging
import urllib2
import sys
import cjson

log = logging.getLogger('cti_config')

class Config:
    def __init__(self, urilist):
        if urilist.find('json') >= 0:
            if urilist.find(':') >= 0:
                xconf = urilist.split(':')
                if xconf[0] in ['http', 'https', 'file']:
                    self.kind = 'file'
                    response = urllib2.urlopen(urilist)
                    self.json_config = response.read()
                    self.xc_json = cjson.decode(self.json_config)

                    for profile, profdef in self.xc_json['xivocti']['profiles'].iteritems():
                        if profdef['xlets']:
                            for xlet_attr in profdef['xlets']:
                                if 'N/A' in xlet_attr:
                                    xlet_attr.remove('N/A')
                                if ('tab' or 'tabber') in xlet_attr:
                                    del xlet_attr[2]
                                if xlet_attr[1] == 'grid':
                                    del xlet_attr[2]
        return
