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

"""
LDAP class.
"""

# http://www.ietf.org/rfc/rfc4516.txt # obsoletes 2255

import ldap
import logging
from xivo import urisup

log = logging.getLogger('ldap')

## \class xivo_ldap
class xivo_ldap:
    def __init__(self, iuri):
        self.iuri    = iuri
        self.ldapopj = None
        self.uri     = None
        self.dbname  = None

        try:
            log.info('LDAP URI requested: %r', iuri)

            if isinstance(iuri, unicode):
                ldapuri = urisup.uri_help_split(iuri.encode('utf8'))
            else:
                ldapuri = urisup.uri_help_split(iuri)

            uri_scheme = ldapuri[0]
            if uri_scheme not in ('ldap', 'ldaps'):
                raise NotImplementedError, 'Unknown URI scheme: %r' % uri_scheme

            # user, pass, host, port
            if ldapuri[1][0] is None:
                ldapuser = ''
            else:
                ldapuser = ldapuri[1][0]

            if ldapuri[1][1] is None:
                ldappass = ''
            else:
                ldappass = ldapuri[1][1]

            if ldapuri[1][2] is None:
                ldaphost = 'localhost'
            else:
                ldaphost = ldapuri[1][2]

            if ldapuri[1][3] is None:
                if uri_scheme == 'ldaps':
                    ldapport = '636'
                else:
                    ldapport = '389'
            else:
                ldapport = ldapuri[1][3]

            # dbname
            if ldapuri[2] is not None:
                if ldapuri[2].startswith('/'):
                    self.dbname = ldapuri[2][1:]
                else:
                    self.dbname = ldapuri[2]

            # bypassing the result made by urisup, since it gives bad results, i.e. for :
            # ldap://user:pass@host:333/dbname???(&(mail=*.com)(telephoneNumber=*)(displayName=*%Q*))
            # the tuple result is :
            # (('??(', None), ('(mail', '*.com)(telephoneNumber=*)(displayName=*%Q*))'))
            # while we should be able to extract :
            # - attributes = ''
            # - scope = ''
            # - filter = '(&(mail=*.com)(telephoneNumber=*)(displayName=*%Q*))'
            # - extensions = ''

            dbpos = iuri.find(self.dbname)
            # ?attributes?scope?filter?extensions = 'asfe'
            asfe = iuri[dbpos + len(self.dbname):].split('?', 4)
            while len(asfe) < 5:
                asfe.append('')
            (self.base_attributes, self.base_scope,
             self.base_filter, self.base_extensions) = asfe[1:]

            self.uri = "%s://%s:%s" % (uri_scheme, ldaphost, ldapport)
            self.ldapobj = ldap.initialize(self.uri)

            log.info('LDAP URI understood: %r / filter: %r', self.uri, self.base_filter)

            # actual cases where these options are useful and used would be
            # to be investigated further, since it does not look like the proper
            # way to add such options to the LDAP URI :
            # ldapquery = dict(ldapuri[3])
            # if ldapquery.has_key('protocol_version'):
            #   self.ldapobj.set_option(ldap.OPT_PROTOCOL_VERSION,
            #   int(ldapquery.get('protocol_version')))
            # if ldapquery.has_key('network_timeout'):
            #   self.ldapobj.set_option(ldap.OPT_NETWORK_TIMEOUT,
            #   float(ldapquery.get('network_timeout')))

            # XXX how should we handle the starttls option ?
            # if uri_scheme == 'ldap' and int(ldapquery.get('tls', 0)):
            # self.ldapobj.start_tls_s()

            self.ldapobj.simple_bind_s(ldapuser, ldappass)
        except ldap.LDAPError, exc:
            log.exception('__init__: ldap.LDAPError (%r, %r, %r)', self.ldapobj, iuri, exc)
            self.ldapobj = None

    def getldap(self, search_filter, search_attributes, searchpattern):
        if self.ldapobj is None:
            self.__init__(self.iuri)

        if self.base_filter:
            actual_filter = '(&(%s)(%s))' % (self.base_filter.replace('%Q', searchpattern),
                                             search_filter)
        else:
            actual_filter = search_filter

        if self.base_scope:
            try:
                actual_scope = getattr(ldap, 'SCOPE_%s' % self.base_scope.upper())
            except AttributeError:
                actual_scope = ldap.SCOPE_SUBTREE
        else:
            actual_scope = ldap.SCOPE_SUBTREE

        log.info('getldap : performing a request with scope %s, filter %s and attribute %s'
                 % (actual_scope, actual_filter, search_attributes))

        try:
            result = self.ldapobj.search_s(self.dbname,
                                           actual_scope,
                                           actual_filter,
                                           search_attributes)
            return result
        except (AttributeError, ldap.LDAPError), exc1:
            # display exc1 since sometimes the error stack looks too long for the logfile
            log.exception('getldap: ldap.LDAPError (%r, %r, %r) retrying to connect',
                          self.ldapobj, self.uri, exc1)
            self.__init__(self.iuri)
            try:
                result = self.ldapobj.search_s(self.dbname,
                                               ldap.SCOPE_SUBTREE,
                                               actual_filter,
                                               search_attributes)
                return result
            except ldap.LDAPError, exc2:
                log.exception('getldap: ldap.LDAPError (%r, %r, %r) could not reconnect',
                              self.ldapobj, self.uri, exc2)
