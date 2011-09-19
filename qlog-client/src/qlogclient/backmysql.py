"""Backend support for MySQL for anysql

Copyright (C) 2007-2010  Proformatique

"""
# NOTE: taken from xivo.BackSQL.backmysql with an adaption to use an SScursor
#       instead of a Cursor to prevent abusively large amount of memory to be
#       taken when doing queries with large result sets

__license__ = """
    Copyright (C) 2007-2011  Proformatique

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version with a Section 7 Additional
    Permission as follows:
      This notice constitutes a grant of such permission as is necessary
      to combine or link this software, or a modified version of it, with
      the MySQL Client Library, as published by Sun Microsystems and/or
      MySQL AB, or a derivative work of the MySQL CLient LIbrary, and to
      copy, modify, and distribute the resulting work. The MySQL Client
      LIbrary is licensed under version 2 of the GNU General Public
      License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import MySQLdb
from MySQLdb.converters import conversions as CST_CONVERSIONS
from MySQLdb.cursors import SSCursor

from xivo import anysql
from xivo import urisup
from xivo.urisup import SCHEME, AUTHORITY, PATH, QUERY, FRAGMENT, uri_help_split

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
    "charset": str,
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

    # The next affectation work around a bug in python-mysqldb which
    # happens when using an unicode charset: the conv parameter is
    # defaulted to the common dictionary MySQLdb.converters.conversions
    # when not explicitly given to the __init__() of
    # MySQLdb.connections.Connection, the _mysql module just store it in
    # the .converter member in the __init__() method of the base class
    # _mysql.connection, and later, back in the __init__() of
    # MySQLdb.connections.Connection, some children of .converter, which
    # are lists, are prepended by dynamically generated functions. The net
    # result is that every times a new Mysql connection is asked for with
    # no individualised conversion dictionary passed to the conv parameter,
    # a bunch of new functions and tuples are created, on which the process
    # will keep references forever, effectively leaking some memory as some
    # won't be used anymore after the termination of any connection.
    # This work around is believed to be effective because the only
    # references to the dynamically created conversion functions generated
    # by MySQLdb.connections will be in this instance-specific copy of
    # MySQLdb.converters.conversions. A unique reference to this copy will
    # be held by the connection instance, so when the latter is garbage
    # collected, the copied conversion dictionary is freed, and eventually
    # the now orphan tuples and generated functions are too.
    params['conv'] = CST_CONVERSIONS.copy()
    params['cursorclass'] = SSCursor

    return MySQLdb.connect(**params)

def escape(s):
    return '.'.join([('`%s`' % comp.replace('`', '``')) for comp in s.split('.')])

anysql.register_uri_backend('mysql', connect_by_uri, MySQLdb, None, escape)
