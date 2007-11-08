#!/usr/bin/python
# $Date$
"""
LDAP class.
Copyright (C) 2007, Proformatique
"""

import csv
import ldap
import syslog
import sys
import urllib

__version__ = "$Revision$ $Date$"

def varlog(syslogprio, string):
        if syslogprio <= syslog.LOG_NOTICE:
                syslog.syslog(syslogprio, "xivo_ldap : requested URI=<%s>" % string)
        return 0

def log_debug(syslogprio, string):
        #        if debug_mode:
        if syslogprio <= syslog.LOG_INFO:
                print "#debug# (xivo_ldap) " + string
        return varlog(syslogprio, string)

## \class xivo_ldap
class xivo_ldap:
        def __init__(self, iuri):
                try:
                        log_debug(syslog.LOG_DEBUG, iuri)
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
			log_debug(syslog.LOG_ERR, '__init__ : exception ldap.LDAPError : %s' % str(exc))
                        # sys.exit()

        def getldap(self, filter, attrib):
                try:
                        resultat = self.l.search_s(self.dbname,
                                                   ldap.SCOPE_SUBTREE,
                                                   filter,
                                                   attrib)
                        return resultat
                except ldap.LDAPError, exc:
                        log_debug(syslog.LOG_ERR, 'getldap : exception ldap.LDAPError : %s' % str(exc))


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
