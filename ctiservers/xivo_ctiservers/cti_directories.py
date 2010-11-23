# vim: set fileencoding=utf-8 :
# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2010 Proformatique'
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

import logging
import urllib
from xivo import anysql
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite
from xivo_ctiservers import xivo_ldap
from xivo_ctiservers import cti_directories_csv

log = logging.getLogger('directories')

# - '*' (wildcard) support
# - more than 1 result
# - message for error/empty
# - encodings
# - avoid reconnections to already opened links (if applicable)
# - better reverse vs. direct
# - extend to 'internal'

def findpattern(xivocti, dirname, searchpattern, z, reversedir):
        fullstatlist = []

        if searchpattern == '':
            return []

        if z.dbkind in ['ldap', 'ldaps']:
            ldap_filter = []
            ldap_attributes = []
            for fname in z.match_direct:
                if searchpattern == '*':
                    ldap_filter.append("(%s=*)" % fname)
                else:
                    ldap_filter.append("(%s=*%s*)" %(fname, searchpattern))

            for listvalue in z.fkeys.itervalues():
                for attrib in listvalue:
                    if isinstance(attrib, unicode):
                        ldap_attributes.append(attrib.encode('utf8'))
                    else:
                        ldap_attributes.append(attrib)

            try:
                results = None
                if z.uri not in xivocti.ldapids:
                    # first connection to ldap, or after failure
                    ldapid = xivo_ldap.xivo_ldap(z.uri)
                    if ldapid.ldapobj is not None:
                        xivocti.ldapids[z.uri] = ldapid
                else:
                    # retrieve the connection already setup, if not yet deleted
                    ldapid = xivocti.ldapids[z.uri]
                    if ldapid.ldapobj is None:
                        del xivocti.ldapids[z.uri]

                # at this point, either we have a successful ldapid.ldapobj value, with xivocti.ldapids[z.uri] = ldapid
                #                either ldapid.ldapobj is None, and z.uri not in xivocti.ldapids

                if ldapid.ldapobj is not None:
                    # if the ldapid had already been defined and setup, the failure would occur here
                    try:
                        results = ldapid.getldap('(|%s)' % ''.join(ldap_filter),
                                                 ldap_attributes,
                                                 searchpattern)
                    except Exception:
                        ldapid.ldapobj = None
                        del xivocti.ldapids[z.uri]

                if results is not None:
                    for result in results:
                        futureline = {'xivo-directory' : z.name}
                        for keyw, dbkeys in z.fkeys.iteritems():
                            for dbkey in dbkeys:
                                if futureline.get(keyw, '') != '':
                                    break
                                elif dbkey in result[1]:
                                    futureline[keyw] = result[1][dbkey][0]
                                elif keyw not in futureline:
                                    futureline[keyw] = ''
                        fullstatlist.append(futureline)
            except Exception:
                log.exception('ldaprequest (directory)')

        elif z.dbkind == 'phonebook':
            if reversedir:
                matchkeywords = z.match_reverse
            else:
                matchkeywords = z.match_direct
            for iastid in xivocti.weblist['phonebook'].keys():
                for k, v in xivocti.weblist['phonebook'][iastid].keeplist.iteritems():
                    matchme = False
                    for tmatch in matchkeywords:
                        if v.has_key(tmatch):
                            if reversedir:
                                if v[tmatch].lstrip('0') == searchpattern.lstrip('0'):
                                    matchme = True
                            else:
                                if searchpattern == '*' or v[tmatch].lower().find(searchpattern.lower()) >= 0:
                                    matchme = True
                    if matchme:
                        futureline = {'xivo-directory' : z.name}
                        for keyw, dbkeys in z.fkeys.iteritems():
                            for dbkey in dbkeys:
                                if dbkey in v.keys():
                                    futureline[keyw] = v[dbkey]
                        fullstatlist.append(futureline)

        elif z.dbkind == 'file':
            if reversedir:
                matchkeywords = z.match_reverse
            else:
                matchkeywords = z.match_direct
            fullstatlist = cti_directories_csv.lookup(searchpattern.encode('utf8'),
                                                      z.uri,
                                                      matchkeywords,
                                                      z.fkeys,
                                                      z.delimiter,
                                                      z.name)

        elif z.dbkind == 'http':
            if not reversedir:
                fulluri = z.uri
                # add an ending slash if needed
                if fulluri[8:].find('/') == -1:
                    fulluri += '/'
                fulluri += '?' + '&'.join([key + '=' + urllib.quote(searchpattern.encode('utf-8')) for key in z.match_direct])
                n = 0
                try:
                    f = urllib.urlopen(fulluri)
                    # use f.info() to detect charset
                    charset = 'utf-8'
                    s = f.info().getheader('Content-Type')
                    k = s.lower().find('charset=')
                    if k >= 0:
                        charset = s[k:].split(' ')[0].split('=')[1]                                    
                    for line in f:
                        if n == 0:
                            header = line
                            headerfields = header.strip().split(z.delimiter)
                        else:
                            ll = line.strip()
                            if isinstance(ll, str): # dont try to decode unicode string.
                                ll = ll.decode(charset)
                            t = ll.split(z.delimiter)
                            futureline = {'xivo-directory' : z.name}
                            # XXX problem when badly set delimiter + index()
                            for keyw, dbkeys in z.fkeys.iteritems():
                                for dbkey in dbkeys:
                                    idx = headerfields.index(dbkey)
                                    futureline[keyw] = t[idx]
                            fullstatlist.append(futureline)
                        n += 1
                    f.close()
                except Exception:
                    log.exception('__build_customers_bydirdef__ (http) %s' % fulluri)
                if n == 0:
                    log.warning('WARNING : %s is empty' % z.uri)
                # we don't warn about "only one line" here since the filter has probably already been applied
            else:
                fulluri = z.uri
                # add an ending slash if needed
                if fulluri[8:].find('/') == -1:
                    fulluri += '/'
                fulluri += '?' + '&'.join([key + '=' + urllib.quote(searchpattern) for key in z.match_reverse])
                f = urllib.urlopen(fulluri)
                # TODO : use f.info() to detect charset
                fsl = f.read().strip()
                if fsl:
                    fullstatlist = [ {'xivo-directory' : z.name,
                                      'db-fullname' : fsl}]
                else:
                    fullstatlist = []

        elif z.dbkind in ['sqlite', 'mysql']:
            if searchpattern == '*':
                whereline = ''
            else:
                # prevent SQL injection and make use of '*' wildcard possible
                esc_searchpattern = searchpattern.replace("'", "\\'").replace('%', '\\%').replace('*', '%')
                wl = ["%s LIKE '%%%s%%'" % (fname, esc_searchpattern) for fname in z.match_direct]
                whereline = 'WHERE ' + ' OR '.join(wl)

            results = []
            try:
                conn = anysql.connect_by_uri(str(z.uri))
                cursor = conn.cursor()
                sqlrequest = 'SELECT ${columns} FROM %s %s' % (z.sqltable, whereline)
                cursor.query(sqlrequest,
                             tuple(z.match_direct),
                             None)
                results = cursor.fetchall()
                conn.close()
            except Exception:
                log.exception('sqlrequest for %s' % z.uri)

            for result in results:
                futureline = {'xivo-directory' : z.name}
                for keyw, dbkeys in z.fkeys.iteritems():
                    for dbkey in dbkeys:
                        if dbkey in z.match_direct:
                            n = z.match_direct.index(dbkey)
                            futureline[keyw] = result[n]
                fullstatlist.append(futureline)

        elif z.dbkind in ['internal']:
            pass

        elif z.dbkind in ['mssql']:
            pass

        else:
            log.warning('wrong or no database method defined (%s) - please fill the uri field of the directory <%s> definition'
                        % (z.dbkind, dirname))



        if reversedir:
            display_reverse = z.display_reverse
            if fullstatlist:
                for k, v in fullstatlist[0].iteritems():
                    if isinstance(v, unicode):
                        display_reverse = display_reverse.replace('{%s}' % k, v)
                    elif isinstance(v, str):
                        # decoding utf8 data as we know the DB is storing utf8 so some bug may lead this data to come here still utf8 encoded
                        # in the future, this code could be removed, once we are sure encoding is properly handled "up there" (in sqlite client)
                        display_reverse = display_reverse.replace('{%s}' % k, v.decode('utf8'))
                    else:
                        log.warning('__build_customers_bydirdef__ %s is neither unicode nor str' % k)
                e = fullstatlist[0]
                e.update({'dbr-display' : display_reverse})
        return fullstatlist
