# -*- coding: iso-8859-15 -*-
"""Backend support for SQLite for anysql

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import anysql
import sqlite
import urisup
from urisup import SCHEME, AUTHORITY, PATH, QUERY, FRAGMENT, uri_help_split

def __dict_from_query(query):
	if not query:
		return {}
	return dict(query)

def connect_by_uri(uri):
	puri = urisup.uri_help_split(uri)
	opts = __dict_from_query(puri[QUERY])
	con = sqlite.connect(puri[PATH])
	if "timeout_ms" in opts:
		con.db.sqlite_busy_timeout(int(opts["timeout_ms"]))
	return con

anysql.register_uri_backend('sqlite', connect_by_uri, sqlite)
