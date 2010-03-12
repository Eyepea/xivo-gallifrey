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


    def test_commonconf_get(self):
        (resp, data) = self.client.request('POST', '/commonconf_get', {'key': '*'})
        pprint.pprint(cjson.decode(data))
        self.assertEqual(resp.status, 200)

        (resp, data) = self.client.request('POST', '/commonconf_get', {'key': 'xivo.*'})
        pprint.pprint(cjson.decode(data))
        self.assertEqual(resp.status, 200)

        (resp, data) = self.client.request('POST', '/commonconf_get', 
            {'key': ('xivo.maintenance', 'alert_emails')})
        pprint.pprint(cjson.decode(data))
        self.assertEqual(resp.status, 200)

    def test_commonconf_set(self):
        (resp, data) = self.client.request('POST', '/commonconf_set', {})
        self.assertEqual(resp.status, 415)

        (resp, data) = self.client.request('POST', '/commonconf_set', 
                {'key': 'sysconfd.unittest.kv0', 'value': 'value0'})
        self.assertEqual(resp.status, 200)

        (resp, data) = self.client.request('POST', '/commonconf_set', 
                {'keyvalues': {'sysconfd.unittest.kv1': 'value1', 'sysconfd.unittest.kv2': 'value2'}})
        self.assertEqual(resp.status, 200)

        (resp, data) = self.client.request('GET', '/commonconf_genconfig', ())
        self.assertEqual(resp.status, 200)

if __name__ == '__main__':
    unittest.main()
