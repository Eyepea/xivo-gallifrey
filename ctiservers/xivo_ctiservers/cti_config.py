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
                    debug_add_section(self.xivoconf, 'general')
                    
                    for k, v in self.xivoconf_json["main"].iteritems():
                        if type(v) == type(list()):
                            if k in ['incoming_tcp_fagi', 'incoming_tcp_cti',
                                     'incoming_tcp_info', 'incoming_tcp_webi',
                                     'incoming_udp_announce']:
                                self.xivoconf.set('general', k, ' : '.join(v))
                            else:
                                self.xivoconf.set('general', k, ','.join(v))
                        elif v is not None:
                            self.xivoconf.set('general', k, v)
                            
                    urllist_params_list = ['urllist_phonebook', 'urllist_voicemail', 'urllist_trunks',
                                           'urllist_agents', 'urllist_agents', 'urllist_queues',
                                           'urllist_phones', 'urllist_incomingcalls', 'urllist_groups',
                                           'urllist_meetme']
                    for asterisk_server in self.xivoconf_json['main']['asterisklist']:
                        debug_add_section(self.xivoconf, asterisk_server)
                        for asterisk_config in self.xivoconf_json[asterisk_server]:
                            if self.xivoconf_json[asterisk_server][asterisk_config]:
                                if asterisk_config in urllist_params_list:
                                    self.xivoconf.set(asterisk_server,
                                                      asterisk_config,
                                                      ','.join(self.xivoconf_json[asterisk_server][asterisk_config]).replace("\\/","/"))
                                else:
                                    self.xivoconf.set(asterisk_server,
                                                      asterisk_config,
                                                      self.xivoconf_json[asterisk_server][asterisk_config].replace("\\/","/"))
                                    
                    debug_add_section(self.xivoconf, 'xivocti')
                    self.xivoconf.set('xivocti','allowedxlets', "file:///etc/pf-xivo/ctiservers/allowedxlets.json")
                    for profile in self.xivoconf_json['xivocti']['profiles']:
                        self.xivoconf.set('xivocti', profile + "-funcs", ','.join(self.xivoconf_json['xivocti']['profiles'][profile]['funcs']))
                        xletlist = ""
                        if self.xivoconf_json['xivocti']['profiles'][profile]['xlets']:
                            for xlet_attr in self.xivoconf_json['xivocti']['profiles'][profile]['xlets']:
                                if 'N/A' in xlet_attr:
                                    xlet_attr.remove('N/A')
                                if ('tab' or 'tabber') in xlet_attr:
                                    del xlet_attr[2]
                                if xlet_attr[1] == "grid" :
                                    del xlet_attr[2]
                                xletlist += '-'.join(xlet_attr) + ","
                            xletlist = xletlist[:-1]
                        self.xivoconf.set('xivocti', profile + "-xlets", xletlist)
                        self.xivoconf.set('xivocti', profile + "-funcs", ','.join(self.xivoconf_json['xivocti']['profiles'][profile]['funcs']))
                        self.xivoconf.set('xivocti', profile + "-appliname",self.xivoconf_json['xivocti']['profiles'][profile]['appliname'] )
                        self.xivoconf.set('xivocti', profile + "-maxgui",self.xivoconf_json['xivocti']['profiles'][profile]['maxgui'] )
                        self.xivoconf.set('xivocti', profile + "-presence",self.xivoconf_json['xivocti']['profiles'][profile]['presence'] )
                        self.xivoconf.set('xivocti', profile + "-services",self.xivoconf_json['xivocti']['profiles'][profile]['services'] )
                        
                    for presence_list_name in self.xivoconf_json["presences"]:
                        debug_add_section(self.xivoconf, 'presences.' + presence_list_name)
                        for presence_name in self.xivoconf_json["presences"][presence_list_name]:
                            presence = self.xivoconf_json["presences"][presence_list_name][presence_name]
                            formatted_action = ""
                            for action_name, action_value in presence['actions'].iteritems():
                                formatted_action += '|'.join([str(action_name), str(action_value)]) + ":"
                            formatted_action = formatted_action[:-1]
                            presence_value = "%s,%s,%s,%s" % ( presence['display'], ':'.join(presence['status']), formatted_action,presence['color']) 
                            self.xivoconf.set('presences.' + presence_list_name, presence_name, presence_value)

                    debug_add_section(self.xivoconf, 'phonehints')
                    for phonehint in self.xivoconf_json["phonehints"]:
                        self.xivoconf.set('phonehints', phonehint, ','.join(self.xivoconf_json["phonehints"][phonehint]))

                    for context_name in self.xivoconf_json["contexts"]:
                        debug_add_section(self.xivoconf, 'contexts.' + context_name)
                        self.xivoconf.set('contexts.' + context_name, "display", self.xivoconf_json["contexts"][context_name]['display'])
                        self.xivoconf.set('contexts.' + context_name, "directories", ','.join(self.xivoconf_json["contexts"][context_name]['directories']))
                        self.xivoconf.set('contexts.' + context_name, "contextname", context_name)

                    for display_name in self.xivoconf_json["displays"]:
                        kname = 'displays.' + display_name
                        debug_add_section(self.xivoconf, kname)
                        for k, v in self.xivoconf_json["displays"][display_name].iteritems():
                            self.xivoconf.set(kname, k, '|'.join(v))

                    debug_add_section(self.xivoconf, 'reversedid')
                    for exten, directories in self.xivoconf_json["reversedid"].iteritems():
                        self.xivoconf.set('reversedid', exten, ','.join(directories))

                    for directory_name in self.xivoconf_json["directories"]:
                        kname = 'directories.' + directory_name
                        debug_add_section(self.xivoconf, kname)
                        for k, v in self.xivoconf_json["directories"][directory_name].iteritems():
                            if type(v) == type(list()):
                                self.xivoconf.set(kname, k, ','.join(v))
                            else:
                                self.xivoconf.set(kname, k, unicode(v))
                    
                    def add_sheet_action_section(self, section_name, rest):
                        debug_add_section(self.xivoconf, section_name)
                        for key in rest:
                            if type(rest[key]) == type(list()):
                                self.xivoconf.set(section_name, key, ','.join(rest[key]))
                            else:
                                self.xivoconf.set(section_name, key, rest[key])

                    debug_add_section(self.xivoconf, 'sheet_events')
                    for k in self.xivoconf_json["sheets"]["events"]:
                        if k == "custom":
                            for csheet in self.xivoconf_json["sheets"]["events"]["custom"]:
                                self.xivoconf.set('sheet_events', csheet, self.xivoconf_json["sheets"]["events"][k][csheet])
                                add_sheet_action_section(self,
                                                  self.xivoconf_json["sheets"]["events"][k][csheet],
                                                  self.xivoconf_json["sheets"]["actions"][self.xivoconf_json["sheets"]["events"][k][csheet]])
                        else:
                            self.xivoconf.set('sheet_events', k, self.xivoconf_json["sheets"]["events"][k])
                            if len(self.xivoconf_json["sheets"]["events"][k]):
                                add_sheet_action_section(self,
                                                  self.xivoconf_json["sheets"]["events"][k],
                                                  self.xivoconf_json["sheets"]["actions"][self.xivoconf_json["sheets"]["events"][k]])

                    def add_sheet_display_section(self, section_name, rest):
                        debug_add_section(self.xivoconf, section_name)
                        for key in rest:
                            if type(rest[key]) == type(list()):
                                self.xivoconf.set(section_name, key, '|'.join(rest[key]))
                            else:
                                self.xivoconf.set(section_name, key, rest[key])

                    for sheet_type in self.xivoconf_json["sheets"]["displays"]:
                        for sheet_name in self.xivoconf_json["sheets"]["displays"][sheet_type]:
                            add_sheet_display_section(self,
                                                      "sheets.displays." + sheet_type + "." + sheet_name,
                                                      self.xivoconf_json["sheets"]["displays"][sheet_type][sheet_name])

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
    
    def set_queuelogger_path(self, loggeruri):
        try:
            response = urllib2.urlopen(loggeruri)
            json_config = response.read()
            json = cjson.decode(json_config)
            ql_uri = json.get('db_uri')
            if ql_uri:
                self.xivoconf.set('general', 'asterisk_queuestat_db', ql_uri)
            else:
                log.warning('loggeruri : empty db_uri value from %s' % loggeruri)
        except Exception:
            log.exception('set_queuelogger_path : %s' % loggeruri)
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
