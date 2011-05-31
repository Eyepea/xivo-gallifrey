# vim: set fileencoding=utf-8 :
# XiVO CTI Server

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2011 Proformatique'
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
Record Features.
"""

import logging
import time

from xivo import anysql
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite

log = logging.getLogger('records_db')

class Records:
    def __init__(self, uri, table):
        self.uri = uri
        self.tablename = table

        self.dbconn = anysql.connect_by_uri(uri)

    # insert methods

    def new_call(self, calldata):
        log.info('new call entered into DB : %s' % calldata)
        columns = tuple(calldata.keys())
        params = tuple(calldata.values())
        args = ', '.join(['%s'] * len(columns))
        query = 'INSERT INTO %s (${columns}) VALUES (' % self.tablename + args + ')'

        cursor = self.dbconn.cursor()
        cursor.query(query,
                     columns,
                     params)
        self.dbconn.commit()
        return

    # update methods

    def update_call(self, idv, calldata):
        log.info('updating call %s in DB with %s' % (idv, calldata))
        columns = tuple(calldata.keys())
        params = tuple(calldata.values())
        args = []
        for c in columns:
            args.append('%s = %%s' % c)
        query = 'UPDATE %s SET ' % self.tablename + ', '.join(args) + ' WHERE id = %s' % idv

        cursor = self.dbconn.cursor()
        cursor.query(query,
                     (),
                     params)
        self.dbconn.commit()

    # select methods

    def get_one_record(self, calldata, columns):
        log.info('DB id requested for call : %s' % calldata)
        args = []
        for c in tuple(calldata.keys()):
            args.append('%s = %%s' % c)
        query = 'SELECT ${columns} FROM %s WHERE ' % self.tablename + ' AND '.join(args)
        params = tuple(calldata.values())

        cursor = self.dbconn.cursor()
        cursor.query(query,
                     columns,
                     params)
        result = cursor.fetchone()
        return dict(zip(columns, result))

    def get_before_date(self, requested, tomatch, callstartdate):
        log.info('DB date requested for records : %s' % callstartdate)
        columns = tuple(requested)
        args = []
        for c in tuple(tomatch.keys()):
            args.append('%s = %%s' % c)
        query = 'SELECT ${columns} FROM %s WHERE callstart < %f' % (self.tablename, callstartdate)
        if args:
            query += ' AND ' + ' AND '.join(args)
        params = tuple(tomatch.values())

        cursor = self.dbconn.cursor()
        cursor.query(query,
                     columns,
                     params)
        results = cursor.fetchall()
        dresults = []
        for r in results:
            dresults.append(dict(zip(requested, r)))
        return dresults

    def get(self, requested):
        log.info('DB data requested for call : %s' % (requested,))
        columns = tuple(requested)
        query = 'SELECT ${columns} FROM %s' % self.tablename

        cursor = self.dbconn.cursor()
        cursor.query(query,
                     columns,
                     list())
        results = cursor.fetchall()
        dresults = []
        for r in results:
            dresults.append(dict(zip(requested, r)))
        return dresults

    def get_all(self):
        query = 'SELECT * FROM %s' % self.tablename
        cursor = self.dbconn.cursor()
        cursor.query(query)
        results = cursor.fetchall()
        return results
