from __future__ import with_statement
"""
    Abstract JSON sysconfd database management:
        - get key/values
        - set key/values
        - generate flat sh file from DB key/values
        
    Currently used by commonconf & ha modules
"""

__version__ = "$Revision$ $Date$"
__author__  = "Guillaume Bour <gbour@proformatique.com>"
__license__ = """
    Copyright (C) 2010  Proformatique

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

import os, re, cjson, traceback, logging
from datetime import datetime

from xivo import http_json_server
from xivo.http_json_server import HttpReqError
from xivo.http_json_server import CMD_RW, CMD_R
from xivo import xys


class JsonDB(object):
    def __init__(self):
        pass
        
    def safe_init(self, options):
        self.db         = options.database
        self.file       = None
   
    GET_SCHEMA = [
        xys.load("""key: !!str   key"""),
        xys.load("""key: !~~seqlen(0,64) [!!str key]""")
    ]

    def get(self, args, options):
        """
        GET /get

        >>> get({'key': 'pf.ha.*'})
        >>> get({'key': ['pf.ha.serial', 'pf.ha.com_mode']})
        """
        
        #TODO: protect key string agains SQL injection
        if xys.validate(args, self.GET_SCHEMA[0]):
            query = "SELECT ${columns} FROM items WHERE key LIKE '%s'" % \
                args['key'].replace('*', '%');
            
        elif xys.validate(args, self.GET_SCHEMA[1]):
            query = "SELECT ${columns} FROM items WHERE key IN (%s)" % \
                ','.join(["'%s'" % x for x in args['key']])
        else:
            raise HttpReqError(415, "invalid arguments for command")

        cursor = self.db.cursor()
        cursor.query(query, ('key', 'value'))
        res = dict(cursor.fetchall())
        cursor.close()

        for key in res.iterkeys():
            print "%s :: %s" % (key, res[key])
            res[key] = cjson.decode(res[key])
            
        return res

    def set(self, args, options):
        """
        POST /set

        Receive a dictionary of key/value where value is an object 
            (string, int, bool, list, ...).
        Data is save as a JSON string into the database.

        >>> set({'pf.ha.serial': 'ttyS0', 'pf.ha.apache2': True})
        """
        if len(args) == 0:
            raise HttpReqError(415, "invalid arguments for command")

        items = [(key, cjson.encode(args[key])) for key in args]

        try:
            cursor = self.db.cursor()
            cursor.querymany("INSERT OR REPLACE INTO items VALUES(%s, %s, '')", None, items)
        finally:
            cursor.close()
            self.db.commit()

        return True


    SECTIONS   = {}
    KEYSELECT  = ''

    ## type generators
    def _generators_dispatch(self, f, key, value):
        try:
            eval('self._gen_%s(f, key, value)' % type(value).__name__, {
                'self'  : self,
                'f'     : f,
                'key'   : key,
                'value' : value,
            })
        except:
            traceback.print_exc()
            self.log.error("no callback defined for *%s* value type" % \
                type(value).__name__)

    def _gen_bool(self, f, key, value):
        value = 'yes' if value else 'no'
        f.write("%s=\"%s\"\n" % (key, value))
        
    def _gen_str(self, f, key, value):
        value = re.sub(r'\r\n', r'\\r\\n', value)
        f.write("%s=\"%s\"\n" % (key, value))
        
    def _gen_NoneType(self, f, key, value):
        f.write("#%s=\"\"\n" % key)
        
    def _gen_dict(self, f, key, value):
        key = key.replace('[', '_%s[') if '[' in key else ''.join(key, '_%s')

        for k, v in value.iteritems():
            self._generators_dispatch(f, key % k.upper(), v)
        f.write('\n')
        
    def _gen_list(self, f, key, value):
        if len(value) == 0:
            # empty list
            f.write("#%s[0]=\"\"\n" % key)
        else:
            for i in xrange(len(value)):
                self._generators_dispatch(f, "%s[%d]" % (key, i), value[i])
    ## /
    
    def generate(self, args, options):
        """
        GET /generate
        """
        
        with open(self.file, 'w') as f:
            def write_keyval(key):
                if dbvalues.has_key(key):
                    value = cjson.decode(dbvalues[key])
                    self._generators_dispatch(f, key.upper().replace('.', '_'), value)
                else:
                    self.log.error("undefined key '%s' in sysconfd items database table" % key)

            def write_section(name):
                f.write("\n# %s\n" % name)
                for dbkey in self.SECTIONS[name]:
                    write_keyval(dbkey)

            f.write("### AUTOMATICALLY GENERATED BY sysconfd. DO NOT EDIT ###\n")
            f.write(datetime.now().strftime("# $%Y/%m/%d %H:%M:%S$\n\n"))
                
            cursor = self.db.cursor()
            cursor.query("SELECT ${columns} FROM items WHERE key LIKE '%s%%'" % \
                self.KEYSELECT, ('key', 'value'))
            dbvalues = dict(cursor.fetchall())
            cursor.close()

            f.write("### Configuration ###")
            for key in sorted(self.SECTIONS.keys()):
                write_section(key)

        return True
        
    def reload(self, args, options):
        raise HttpReqError(500, "not implemented")

