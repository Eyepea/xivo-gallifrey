"""Backend support for SQLite for anysql

Copyright (C) 2007-2009  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007-2009  Proformatique

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import sqlite
import os.path

from xivo import anysql
from xivo import urisup
from xivo.urisup import SCHEME, AUTHORITY, PATH, QUERY, FRAGMENT, uri_help_split, uri_help_unsplit

def __dict_from_query(query):
    if not query:
        return {}
    return dict(query)

def connect_by_uri(uri):
    puri = urisup.uri_help_split(uri)
    opts = __dict_from_query(puri[QUERY])
    con = sqlite.connect(puri[PATH], client_encoding='utf8')
    if "timeout_ms" in opts:
        con.db.sqlite_busy_timeout(int(opts["timeout_ms"]))
    return con

def c14n_uri(uri):
    puri = list(urisup.uri_help_split(uri))
    puri[PATH] = os.path.abspath(puri[PATH])
    return uri_help_unsplit(tuple(puri))

def escape(s):
    return '.'.join([('"%s"' % comp.replace('"', '""')) for comp in s.split('.')])

anysql.register_uri_backend('sqlite', connect_by_uri, sqlite, c14n_uri, escape)
