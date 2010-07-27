# XiVO CTI Server
# vim: set fileencoding=utf-8

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

from xivo import anysql
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite
import ConfigParser
import logging
import urllib2
import sys
import cjson

log = logging.getLogger('cti_config')

def debug_add_section(where, section_name):
    where.add_section(section_name)
    #print "LALALA ADDING SECTION %s" % section_name


class Config:
    def __init__(self, urilist):
        if urilist.find('json') >= 0:
            if urilist.find(':') >= 0:
                xconf = urilist.split(':')
                if xconf[0] in ['http', 'https', 'file']:
                    self.kind = 'file'
                    response = urllib2.urlopen(urilist)
                    self.json_config = response.read()
                    self.xivoconf_json = cjson.decode(self.json_config)
                    self.xivoconf = ConfigParser.ConfigParser()

                    for profile, profdef in self.xivoconf_json['xivocti']['profiles'].iteritems():
                        if profdef['xlets']:
                            for xlet_attr in profdef['xlets']:
                                if 'N/A' in xlet_attr:
                                    xlet_attr.remove('N/A')
                                if ('tab' or 'tabber') in xlet_attr:
                                    del xlet_attr[2]
                                if xlet_attr[1] == "grid" :
                                    del xlet_attr[2]

                    def add_sheet_action_section(self, section_name, rest):
                        debug_add_section(self.xivoconf, section_name)
                        for key, v in rest.iteritems():
                            if isinstance(v, list):
                                self.xivoconf.set(section_name, key, ','.join(v))
                            else:
                                self.xivoconf.set(section_name, key, v)

                    debug_add_section(self.xivoconf, 'sheet_events')
                    for k, eventdef in self.xivoconf_json["sheets"]["events"].iteritems():
                        if k == "custom":
                            for csheet, customeventdef in eventdef.iteritems():
                                self.xivoconf.set('sheet_events', csheet, customeventdef)
                                if self.xivoconf_json["sheets"]["actions"][customeventdef]:
                                    add_sheet_action_section(self,
                                                             customeventdef,
                                                             self.xivoconf_json["sheets"]["actions"][customeventdef])
                        else:
                            self.xivoconf.set('sheet_events', k, eventdef)
                            if len(eventdef) and self.xivoconf_json["sheets"]["actions"][eventdef]:
                                add_sheet_action_section(self,
                                                         eventdef,
                                                         self.xivoconf_json["sheets"]["actions"][eventdef])

                    def add_sheet_display_section(self, section_name, rest):
                        debug_add_section(self.xivoconf, section_name)
                        for key, v in rest.iteritems():
                            if isinstance(v, list):
                                self.xivoconf.set(section_name, key, '|'.join(v))
                            else:
                                self.xivoconf.set(section_name, key, v)

                    for sheet_type, sheetdef in self.xivoconf_json["sheets"]["displays"].iteritems():
                        for sheet_name in sheetdef:
                            if sheetdef[sheet_name]:
                                add_sheet_display_section(self,
                                                          'sheets.displays.%s.%s' % (sheet_type, sheet_name),
                                                          sheetdef[sheet_name])

        elif urilist.find(':') >= 0:
            xconf = urilist.split(':')
            if xconf[0] == 'file':
                self.kind = 'file'
                self.xivoconf = ConfigParser.ConfigParser()
                self.xivoconf.readfp(open(xconf[1]))
        else:
            if len(urilist) > 0:
                uris = urilist.split(',')
                self.kind = 'file'
                self.xivoconf = ConfigParser.ConfigParser()
                for uri in uris:
                    self.xivoconf.readfp(open(uri))
        return
    
    def read_section(self, type, sectionname):
        v = {}
        if self.kind == 'file':
            try:
                if sectionname in self.xivoconf.sections():
                    for tk, tv in dict(self.xivoconf.items(sectionname)).iteritems():
                        v[tk] = tv # tv.decode('utf8')
            except Exception, e:
                log.exception('kind=%s section=%s' % (self.kind, sectionname))
                print e
        return v
