from __future__ import with_statement
"""ha module
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

import os, re, logging, subprocess, cjson, traceback
from datetime import datetime

from xivo import http_json_server
from xivo.http_json_server import HttpReqError
from xivo.http_json_server import CMD_RW, CMD_R
from xivo.xivo_config import txtsubst
from xivo import xys
from xivo import system

from xivo_sysconf import helpers

log = logging.getLogger('xivo_sysconf.modules.ha') # pylint: disable-msg=C0103

class Ha:
    """
    """
    def __init__(self):
        http_json_server.register(self.get      , CMD_RW, name='ha_get', safe_init=self.safe_init)
        http_json_server.register(self.set      , CMD_RW, name='ha_set')
        http_json_server.register(self.generate , CMD_R , name='ha_generate')
        
    def safe_init(self, options):
        self.db         = options.database
        self.file       = options.configuration.get('ha', 'ha_file')
        self.cmd        = options.configuration.get('ha', 'ha_cmd')
   
    GET_SCHEMA = [
        xys.load("""key: !!str   pf.ha.*"""),
        xys.load("""key: !~~seqlen(0,64) [!!str pf.ha.serial]""")
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

    def generate(self, args, options):
        """
        GET /generate
        """
        sections = {
            '1. Services'           : ['pf.ha.apache2', 'pf.ha.asterisk', 'pf.ha.dhcp', 
                'pf.ha.monit', 'pf.ha.mysql', 'pf.ha.ntp', 'pf.ha.rsync', 
                'pf.ha.smokeping', 'pf.ha.mailto'],

            '2. Heartbeat Settings' : ['pf.ha.alert_emails', 'pf.ha.serial', 
                'pf.ha.authkeys', 'pf.ha.com_mode'],
            '3. Replication'        : ['pf.ha.user', 'pf.ha.password', 
                'pf.ha.dest_user', 'pf.ha.dest_password'],
            '4. Monitoring of network connection': ['pf.ha.ping_ipaddr'],
            '5. Uname of Heartbeat nodes': ['pf.ha.uname_node'],
            '6. Virtual network information': ['pf.ha'],
            '7. Network interface and host to communicate with the other nodes.':
                ['pf.ha.dest'],
        }
                
        with open(self.file, 'w') as f:
            def write_keyval(key):
                def keyval__dispatch(key, value):
                    try:
                        eval('write_keyval__%s(key, value)' % type(value).__name__, {
                            'write_keyval__bool'     : write_keyval__bool,
                            'write_keyval__str'      : write_keyval__str,
                            'write_keyval__dict'     : write_keyval__dict,
                            'write_keyval__list'     : write_keyval__list,
                            'write_keyval__NoneType' : write_keyval__NoneType,
                                                
                            'key'               : key,
                            'value'             : value
                        })
                    except:
                        traceback.print_exc()
                        log.error("no callback defined for *%s* value type" % \
                            type(value).__name__)
                        
                def write_keyval__bool(key, value):
                    value = 'yes' if value else 'no'
                    f.write("%s=\"%s\"\n" % (key, value))
                            
                def write_keyval__str(key, value):
                    value = re.sub(r'\r\n', r'\\r\\n', value)
                    f.write("%s=\"%s\"\n" % (key, value))
                            
                def write_keyval__NoneType(key, value):
                    f.write("#%s=\"\"\n" % key)
                            
                def write_keyval__dict(key, value):
                    key = key.replace('[', '_%s[') if '[' in key else ''.join(key, '_%s')

                    for k, v in value.iteritems():
                        keyval__dispatch(key % k.upper(), v)
                            
                def write_keyval__list(key, value):
                    if len(value) == 0:
                        f.write("#%s[0]=\"\"\n" % key)
                    else:
                        for i in xrange(len(value)):
                            keyval__dispatch("%s[%d]" % (key, i), value[i])

                if dbvalues.has_key(key):
                    value = cjson.decode(dbvalues[key])
                    keyval__dispatch(key.upper().replace('.', '_'), value)
                else:
                    log.error("undefined key '%s' in sysconfd items database table" % key)
            # END write_keyval()

            def write_section(name):
                f.write("\n# %s\n" % name)
                for dbkey in sections[name]:
                    write_keyval(dbkey)

            f.write("### AUTOMATICALLY GENERATED BY sysconfd. DO NOT EDIT ###\n")
            f.write(datetime.now().strftime("# $%Y/%m/%d %H:%M:%S$\n\n"))
                
            cursor = self.db.cursor()
            cursor.query("SELECT ${columns} FROM items WHERE key LIKE 'pf.ha%'", ('key', 'value'))
            dbvalues = dict(cursor.fetchall())
            cursor.close()

            f.write("### Configuration ###")
            for key in sorted(sections.keys()):
                write_section(key)

        return True
        
    def reload(self, args, options):
        try:
            p = subprocess.Popen([self.cmd])
            ret = p.wait()
        except OSError:
            raise HttpReqError(500, "can't execute '%s'" % self.configexec)

        if ret != 0:
            raise HttpReqError(500, "'%s' process return error %d" % (self.cmd, ret))

        return True
        
        
ha = Ha()
