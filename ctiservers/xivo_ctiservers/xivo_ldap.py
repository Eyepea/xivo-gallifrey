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

"""
LDAP class.
"""

import csv
import ldap
import syslog
import sys
import urllib
from xivo_log import *

def log_debug(a, b):
        log_debug_file(a, b, 'ldap')

## \class xivo_ldap
class xivo_ldap:
        def __init__(self, iuri):
                try:
                        log_debug(SYSLOG_INFO, 'requested uri = %s' % iuri)
                        addport = iuri.split("@")[1].split("/")[0]
                        userpass = iuri.split("@")[0].split("://")[1]
                        self.dbname = iuri.split("@")[1].split("/")[1]
                        
                        self.user = userpass.split(":", 1)[0]
                        self.passwd = userpass.split(":", 1)[1]
                        self.uri  = "ldap://" + addport
                        self.l = ldap.initialize(self.uri)
                        self.l.protocol_version = ldap.VERSION3
                        self.l.simple_bind_s(self.user, self.passwd)
                        
                except ldap.LDAPError, exc:
			log_debug(SYSLOG_ERR, '__init__ : exception ldap.LDAPError : %s' % str(exc))
                        # sys.exit()

        def getldap(self, filter, attrib):
                try:
                        resultat = self.l.search_s(self.dbname,
                                                   ldap.SCOPE_SUBTREE,
                                                   filter,
                                                   attrib)
                        return resultat
                except ldap.LDAPError, exc:
                        log_debug(SYSLOG_ERR, 'getldap : exception ldap.LDAPError : %s' % str(exc))


class xivo_csv:
        def __init__(self, uri):
                self.uri = uri
                self.opened = False

        def open(self):
                if self.uri.find('file:') == 0:
                        self.path = self.uri[5:]
                if self.uri.find('file:') == 0 or self.uri.find('http:') == 0:
                        self.items = []
                        f = urllib.urlopen(self.uri)
                        csvreader = csv.reader(f, delimiter = ';')
                        self.keys = csvreader.next()
                        for line in csvreader:
                                if len(line) > 0:
                                        self.items.append(line)
                        f.close()
                        self.opened = True
                return self.opened

        def index(self, key):
                return self.keys.index(key)

        def add(self, listitems):
                if self.opened:
                        if listitems not in self.items:
                                self.items.append(listitems)
                                linetoadd = ';'.join(listitems)
                                toadd = open(self.path, 'a')
                                toadd.write('%s\n' % linetoadd)
                                toadd.close()
