#!/usr/bin/python
# $Date$
"""
LDAP class.
Copyright (C) 2007, Proformatique
"""

import ldap
import syslog
import sys

__version__ = "$Revision$ $Date$"

def varlog(string):
        syslog.syslog(syslog.LOG_NOTICE, "xivo_ldap : requested URI=<%s>" % string)
        return 0

def log_debug(string):
#        if debug_mode:
        print "#debug# (xivo_ldap) " + string
        return varlog(string)

## \class xivo_ldap
class xivo_ldap:
        def __init__(self, iuri):
                try:
                        log_debug(iuri)
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
			log_debug('__init__ : exception ldap.LDAPError : %s' % str(exc))
                        # sys.exit()

        def getldap(self, filter, attrib):
                try:
                        resultat = self.l.search_s(self.dbname,
                                                   ldap.SCOPE_SUBTREE,
                                                   filter,
                                                   attrib)
                        return resultat
                except ldap.LDAPError, exc:
                        log_debug('getldap : exception ldap.LDAPError : %s' % str(exc))
