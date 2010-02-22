# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2010 Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
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

def debug_add_section(where,section_name):
    where.add_section(section_name)
    print "LALALA ADDING SECTION %s" % section_name


class Config:
    def __init__(self, urilist):
        if urilist.find('json') >= 0:
            if urilist.find(':') >= 0:
                xconf = urilist.split(':')
                if xconf[0] in ['http', 'https']:
                    self.kind = 'file'
                    response = urllib2.urlopen(urilist)
                    self.json_config = response.read()
                
                    json = cjson.decode(self.json_config)
                
                    self.xivoconf = ConfigParser.ConfigParser()
                    debug_add_section(self.xivoconf,'general')
                    self.xivoconf.set('general','incoming_tcp_fagi',' : '.join(json['main']['incoming_tcp_fagi']))
                    self.xivoconf.set('general','incoming_tcp_cti',' : '.join(json['main']['incoming_tcp_cti']))
                    self.xivoconf.set('general','incoming_tcp_webi',' : '.join(json['main']['incoming_tcp_webi']))
                    self.xivoconf.set('general','incoming_tcp_info',' : '.join(json['main']['incoming_tcp_info']))
                    self.xivoconf.set('general','incoming_udp_announce',' : '.join(json['main']['incoming_udp_announce']))
                
                    self.xivoconf.set('general','asterisklist',','.join(json['main']['asterisklist']))
                    self.xivoconf.set('general','sockettimeout',json['main']['sockettimeout'])
                    self.xivoconf.set('general','contextlist',','.join(json['main']['contextlist']))
                    self.xivoconf.set('general','logintimeout',json['main']['logintimeout'])
                    self.xivoconf.set('general','commandset',json['main']['commandset'])
                    self.xivoconf.set('general','userlists',','.join(json['main']['userlists']))


                    for asterisk_server in json['main']['asterisklist']:
                        debug_add_section(self.xivoconf,asterisk_server)
                        for asterisk_config in json[asterisk_server]:
                            if asterisk_config in ["urllist_phonebook", "urllist_voicemail", "urllist_trunks",
                                                   "urllist_agents", "urllist_agents", "urllist_queues",
                                                   "urllist_phones", "urllist_incomingcalls", "urllist_groups",
                                                   "urllist_meetme"]:
                                self.xivoconf.set(asterisk_server,
                                                  asterisk_config,
                                                  ','.join(json[asterisk_server][asterisk_config]).replace("\\/","/"))
                            else:
                                self.xivoconf.set(asterisk_server,
                                                  asterisk_config,
                                                  json[asterisk_server][asterisk_config].replace("\\/","/"))

                    debug_add_section(self.xivoconf,'xivocti')
                    self.xivoconf.set('xivocti','allowedxlets', "file:///etc/pf-xivo/ctiservers/allowedxlets.json")
                    for profil in json['xivocti']['profils']:
                        self.xivoconf.set('xivocti', profil + "-funcs", ','.join(json['xivocti']['profils'][profil]['funcs']))
                        xletlist = ""
                        for xlet_nu in json['xivocti']['profils'][profil]['xlets']:
                            xletlist += '-'.join(xlet_nu) + ","
                        xletlist = xletlist[:-1]
                        self.xivoconf.set('xivocti', profil + "-xlets", xletlist)
                        self.xivoconf.set('xivocti', profil + "-funcs", ','.join(json['xivocti']['profils'][profil]['funcs']))
                        self.xivoconf.set('xivocti', profil + "-appliname",json['xivocti']['profils'][profil]['appliname'] )
                        self.xivoconf.set('xivocti', profil + "-maxgui",json['xivocti']['profils'][profil]['maxgui'] )

                    for presence_list_name in json["presences"]:
                        debug_add_section(self.xivoconf,'presence-presence.' + presence_list_name)
                        for presence_name in json["presences"][presence_list_name]:
                            presence = json["presences"][presence_list_name][presence_name]
                            formatted_action = ""
                            for action_name, action_value in presence['actions'].iteritems():
                                formatted_action += '|'.join([str(action_name), str(action_value)]) + ":"
                            formatted_action = formatted_action[:-1]
                            presence_value = "%s,%s,%s,%s" % ( presence['display'], ':'.join(presence['status']), formatted_action,presence['color']) 
                            self.xivoconf.set('presence-presence.' + presence_list_name, presence_name, presence_value)

                    debug_add_section(self.xivoconf, 'phonehints')
                    for phonehint in json["phonehints"]:
                        self.xivoconf.set('phonehints', phonehint, ','.join(json["phonehints"][phonehint]))

                            




                
        elif urilist.find(':') >= 0:
            xconf = urilist.split(':')
            if xconf[0] == 'file':
                self.kind = 'file'
                self.xivoconf = ConfigParser.ConfigParser()
                self.xivoconf.readfp(open(xconf[1]))
            elif xconf[0] in ['mysql', 'sqlite']:
                self.kind = 'sql'
                self.conn = anysql.connect_by_uri(urilist)
                self.cursor = self.conn.cursor()

        else:
            if len(urilist) > 0:
                uris = urilist.split(',')
                self.kind = 'file'
                self.xivoconf = ConfigParser.ConfigParser()
                for uri in uris:
                    self.xivoconf.readfp(open(uri))
                
        return


    def read_section(self, type, sectionname):
        print "LALALA TRYING TO READ %s (%s)" % (type, sectionname)
        v = {}
        if self.kind == 'file':
            print self.kind, sectionname
            try:
                if sectionname in self.xivoconf.sections():
                    for tk, tv in dict(self.xivoconf.items(sectionname)).iteritems():
                        print tk,"mooo",tv
                        v[tk] = tv.decode('utf8')
            except Exception, e:
                log.exception('kind=%s section=%s' % (self.kind, sectionname))
                print e
        elif self.kind == 'sql':
            try:
                if type == 'commandset':
                    self.cursor.query('SELECT * FROM metaxlet_props')
                    z = self.cursor.fetchall()
                    for zz in z:
                        [name, displayname, maxgui] = zz
                        v['%s-xlets' % name] = ''
                        v['%s-funcs' % name] = ''
                        v['%s-maxgui' % name] = maxgui
                        v['%s-appliname' % name] = displayname
                    self.cursor.query('SELECT * FROM metaxlet_defs')
                    z = self.cursor.fetchall()
                    for zz in z:
                        [name, xtype, display, option] = zz
                        if display != 'func':
                            v['%s-xlets' % xtype] = '%s-%s-%s' % (name, display, option)
                        else:
                            v['%s-funcs' % xtype] = name
                elif type == 'ipbx':
                    self.cursor.query('SELECT * FROM asterisk_defs')
                    z = self.cursor.fetchall()
                    for zz in z:
                        if zz[0] == sectionname:
                            [xivoname, localaddr, ipaddress, ipaddress_webi,
                             urllist_phones, urllist_queues, urllist_agents,
                             ami_port, ami_login, ami_pass,
                             cdr_db_uri, userfeatures_db_uri] = zz
                else:
                    self.cursor.query('SELECT ${columns} FROM xivodaemonconf WHERE sectionname = %s',
                                      ('sectionname', 'var_name', 'var_val'),
                                      sectionname)
                    z = self.cursor.fetchall()
                    for zz in z:
                        [catname, var_name, var_val] = zz
                        v[var_name] = var_val
            except Exception:
                log.exception('kind=%s type=%s section=%s' % (self.kind, type, sectionname))
        return v
