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

#    def test_01_munin(self):
#        (resp, data) = self.client.request('GET', '/munin_update', {})
#        self.assertEqual(resp.status, 200)


#    def test_10_ha_status(self):
#        (resp, data) = self.client.request('GET', '/ha_status', {})
#        pprint.pprint(cjson.decode(data))
#        self.assertEqual(resp.status, 200)
#        
#    def test_11_ha_set(self):
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
#        
#    def test_19_ha_apply(self):
#        (resp, data) = self.client.request('GET', '/ha_apply', {})
#        if resp.status != 200:
#            print data
#        self.assertEqual(resp.status, 200)
#        print cjson.decode(data)

#    def test_29_commonconf_apply(self):
#        (resp, data) = self.client.request('GET', '/commonconf_apply', {})
#        if resp.status != 200:
#            print data
#        self.assertEqual(resp.status, 200)
#        print cjson.decode(data)

    def test_30_getdns(self):
        (resp, data) = self.client.request('GET', '/dns', {})
        if resp.status != 200:
            print data
        self.assertEqual(resp.status, 200)
        print cjson.decode(data)

    def test_40_services(self):
        (resp, data) = self.client.request('POST', '/services', {'networking': 'stop'})
        self.assertEqual(resp.status, 200)
        
        import time; time.sleep(5)
        
        (resp, data) = self.client.request('POST', '/services', {'networking': 'start'})
        self.assertEqual(resp.status, 200)

        
if __name__ == '__main__':
    unittest.main()
