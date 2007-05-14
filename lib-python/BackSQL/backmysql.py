# -*- coding: iso-8859-15 -*-
"""Backend support for MySQL for anysql

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import anysql
import MySQLdb
import urisup
from urisup import SCHEME, AUTHORITY, PATH, QUERY, FRAGMENT, uri_help_split

__typemap = {
	"host": str,
	"user": str,
	"passwd": str,
	"db": str,
	"port": int,
	"unix_socket": str,
	"compress": bool,
	"connect_timeout": int,
	"read_default_file": str,
	"read_default_group": str,
	"use_unicode": (lambda x: bool(int(x))),
	"conv": None,
	"quote_conv": None,
	"cursorclass": None,
}

def __apply_types(params, typemap):
	for k in typemap.iterkeys():
		if k in params:
			if typemap[k] is not None:
				params[k] = typemap[k](params[k])
			else:
				del params[k]

def __dict_from_query(query):
	if not query:
		return {}
	return dict(query)

def connect_by_uri(uri):
	"""General URI syntax:
	
	mysql://user:passwd@host:port/db?opt1=val1&opt2=val2&...
	
	where opt_n is in the list of options supported by MySQLdb:
	
	    host,user,passwd,db,compress,connect_timeout,read_default_file,
	    read_default_group,unix_socket,port
	
	NOTE: the authority and the path parts of the URI have precedence
	over the query part, if an argument is given in both.
	
	    conv,quote_conv,cursorclass
	are not (yet?) allowed as complex Python objects are needed, hard to
	transmit within an URI...

	See for description of options:
	    http://dustman.net/andy/python/MySQLdb_obsolete/doc/MySQLdb-3.html#ss3.1
	    http://mysql-python.svn.sourceforge.net/viewvc/mysql-python/trunk/MySQLdb/doc/MySQLdb.txt?revision=438&view=markup&pathrev=438

	"""
	puri = urisup.uri_help_split(uri)
	params = __dict_from_query(puri[QUERY])
	if puri[AUTHORITY]:
		user, passwd, host, port = puri[AUTHORITY]
		if user:
			params['user'] = user
		if passwd:
			params['passwd'] = passwd
		if host:
			params['host'] = host
		if port:
			params['port'] = port
	if puri[PATH]:
		params['db'] = puri[PATH]
		if params['db'] and params['db'][0] == '/':
			params['db'] = params['db'][1:]
	__apply_types(params, __typemap)
	return MySQLdb.connect(**params)

anysql.register_uri_backend('mysql', connect_by_uri, MySQLdb)
