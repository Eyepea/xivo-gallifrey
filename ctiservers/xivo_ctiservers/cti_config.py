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

from xivo import anysql
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite
import ConfigParser
from xivo_log import *

def log_debug(level, text):
        log_debug_file(level, text, 'cti_config')

class Config:
        def __init__(self, uri):
                self.uri = uri
                if uri.find(':') >= 0:
                        xconf = uri.split(':')
                        if xconf[0] == 'file':
                                self.kind = 'file'
                                self.xivoconf = ConfigParser.ConfigParser()
                                self.xivoconf.readfp(open(xconf[1]))
                        elif xconf[0] in ['mysql', 'sqlite']:
                                self.kind = 'sql'
                                self.conn = anysql.connect_by_uri(uri)
                                self.cursor = self.conn.cursor()
                else:
                        self.kind = 'file'
                        self.xivoconf = ConfigParser.ConfigParser()
                        self.xivoconf.readfp(open(uri))
                return


        def read_section(self, type, sectionname):
                v = {}
                if self.kind == 'file':
                        try:
                                if sectionname in self.xivoconf.sections():
                                        v = dict(self.xivoconf.items(sectionname))
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- kind=%s section=%s : %s' % (self.kind, sectionname, exc))
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
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- kind=%s type=%s section=%s : %s' % (self.kind, type, sectionname, exc))
                return v
