"""Backend support for PostgreSQL for anysql

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

import anysql
from pyPgSQL import PgSQL
import urisup
from urisup import SCHEME, AUTHORITY, PATH, QUERY, FRAGMENT, uri_help_split

__typemap = {
	"host": str,
	"user": str,
	"password": str,
	"database": str,
	"port": int,
	"client_encoding": str,
	"unicode_results": (lambda x: bool(int(x))),
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
	
	postgres://user:password@host:port/database?opt1=val1&opt2=val2...
	
	where opt_n is in the list of options supported by PostGreSQL:
	
	    host,user,password,port,database,client_encoding,unicode_results
	
	NOTE: the authority and the path parts of the URI have precedence
	over the query part, if an argument is given in both.
	
	Descriptions of options:
	    file:///usr/lib/python?.?/site-packages/pyPgSQL/PgSQL.py
	
	"""
	puri = urisup.uri_help_split(uri)
	params = __dict_from_query(puri[QUERY])
	if puri[AUTHORITY]:
		user, password, host, port = puri[AUTHORITY]
		if user:
			params['user'] = user
		if password:
			params['password'] = password
		if host:
			params['host'] = host
		if port:
			params['port'] = port
	if puri[PATH]:
		params['database'] = puri[PATH]
		if params['database'] and params['database'][0] == '/':
			params['database'] = params['database'][1:]
	__apply_types(params, __typemap)
	return PgSQL.connect(**params)

anysql.register_uri_backend('postgres', connect_by_uri, PgSQL)
