# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2009 Proformatique'
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

"""
LDAP class.
"""

import ldap
import logging
import sys
import urllib

log = logging.getLogger('ldap')

## \class xivo_ldap
class xivo_ldap:
        def __init__(self, iuri):
                self.l = None
                self.iuri = iuri
                try:
                        log.info('requested uri = %s' % iuri)
                        ldapf = iuri.split('://')
                        ldapkind = ldapf[0]
                        postldap = ldapf[1]
                        if postldap.find('@') > 0:
                                userpass = postldap.split('@')[0]
                                [addport, self.dbname] = postldap.split('@')[1].split('/')
                        else:
                                userpass = None
                                [addport, self.dbname] = postldap.split('/')
                        self.uri  = 'ldap://%s' % addport
                        self.l = ldap.initialize(self.uri)
                        self.l.protocol_version = ldap.VERSION3
                        if ldapkind == 'ldaps':
                                # find a better way to define when TLS has to be used ? (ldaps:// doesn't seem proper)
                                self.l.start_tls_s()
                        if userpass is not None:
                                self.user = userpass.split(':', 1)[0]
                                self.passwd = userpass.split(':', 1)[1]
                                self.l.simple_bind_s(self.user, self.passwd)
                        else:
                                self.l.simple_bind_s()
                                
                except ldap.LDAPError:
                        log.exception('__init__ : ldap.LDAPError (%s %s)' % (self.l, iuri))
                        self.l = None
                        
        def getldap(self, filter, attrib):
                try:
                        resultat = self.l.search_s(self.dbname,
                                                   ldap.SCOPE_SUBTREE,
                                                   filter,
                                                   attrib)
                        return resultat
                except ldap.LDAPError:
                        log.exception('getldap : ldap.LDAPError (%s %s) retrying to connect' % (self.l, self.uri))
                        self.__init__(self.iuri)
                        try:
                                resultat = self.l.search_s(self.dbname,
                                                           ldap.SCOPE_SUBTREE,
                                                           filter,
                                                           attrib)
                                return resultat
                        except ldap.LDAPError:
                                log.exception('getldap : ldap.LDAPError (%s %s) could not reconnect' % (self.l, self.uri))
