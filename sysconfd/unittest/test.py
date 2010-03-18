#!/usr/bin/python
# -*- coding: utf-8 -*-

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

import unittest, pprint, cjson
import sysconfd_client

class TestGeneric(unittest.TestCase):
    def setUp(self):
        self.client = sysconfd_client.SysconfdClient()

#    def test_commonconf_get(self):
#        (resp, data) = self.client.request('POST', '/commonconf_get', {'key': '*'})
#        pprint.pprint(cjson.decode(data))
#        self.assertEqual(resp.status, 200)

#        (resp, data) = self.client.request('POST', '/commonconf_get', {'key': 'xivo.*'})
#        pprint.pprint(cjson.decode(data))
#        self.assertEqual(resp.status, 200)

#        (resp, data) = self.client.request('POST', '/commonconf_get', 
#            {'key': ('xivo.maintenance', 'alert_emails')})
#        pprint.pprint(cjson.decode(data))
#        self.assertEqual(resp.status, 200)

#    def test_commonconf_set(self):
#        (resp, data) = self.client.request('POST', '/commonconf_set', {})
#        self.assertEqual(resp.status, 415)

#        (resp, data) = self.client.request('POST', '/commonconf_set', {
#            ''
#        })
#        self.assertEqual(resp.status, 200)

#        (resp, data) = self.client.request('POST', '/commonconf_set', 
#                {'keyvalues': {'sysconfd.unittest.kv1': 'value1', 'sysconfd.unittest.kv2': 'value2'}})
#        self.assertEqual(resp.status, 200)

#        (resp, data) = self.client.request('GET', '/commonconf_genconfig', ())
#        self.assertEqual(resp.status, 200)

    def test_commonconf_generate(self):
        (resp, data) = self.client.request('GET', '/commonconf_generate', {})
        self.assertEqual(resp.status, 200)


    def test_ha_generate(self):
        (resp, data) = self.client.request('GET', '/ha_generate', {})
        self.assertEqual(resp.status, 200)

    def test_ha_get(self):
        (resp, data) = self.client.request('POST', '/ha_get', {'key': 'pf.ha.*'})
        pprint.pprint(cjson.decode(data))
        self.assertEqual(resp.status, 200)

#    def test_ha_set(self):
#        (resp, data) = self.client.request('POST', '/ha_set', {
#            'pf.ha.apache2'     : True,
#            'pf.ha.monit'       : True,
#            
#            'pf.ha.serial'      : 'ttyS0', 
#            
#            'pf.ha.uname_node'  : ['xivo-1', 'xivo-2'],
#            'pf.ha.dest'        : [
#                {'iface': 'eth0', 'host': '192.168.0.253', 'transfer': False},
#                {'iface': 'vboxnet0', 'host': '172.16.1.253' , 'transfer': True}
#            ],
#            'pf.ha'             : [
#                {'ipaddr': '192.168.0.254', 'netmask': '255.255.255.0', 'broadcast': '192.168.0.255'}
#            ],
#            'pf.ha.ping_ipaddr' : ['192.168.0.1'],
#        })
#        self.assertEqual(resp.status, 200)
        
if __name__ == '__main__':
    unittest.main()
