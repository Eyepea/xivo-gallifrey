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

import unittest
import sysconfd_client

class TestGeneric(unittest.TestCase):
    def setUp(self):
        self.client = sysconfd_client.SysconfdClient()

    def test_mail(self):
        params = {
            'origin'            : 'TEST_ORIGIN', 
            'relayhost'         : 'TEST_RELAYHOST', 
            'fallback_relayhost': 'TEST_FB_RELAYHOST', 
            'canonical'         : 'TEST_CANONICAL',
            'mydomain'          : 'TEST_MYDOMAIN',
        }
        (response, data) = self.client.request("POST", "/mailconfig_set", params)
        
        self.assertEqual(response.status, 200)


if __name__ == '__main__':
    unittest.main()
