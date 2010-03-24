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

import httplib, urllib, base64
import cjson as json

class JSONClient(object):
    objects = {
        'entity'        : ['xivo/configuration'      , 'manage'],
        'users'         : ['service/ipbx'            , 'pbx_settings'],
        'incall'        : ['service/ipbx'            , 'call_management'],
    }

    def __init__(self, ip='localhost', port=80, ssl=False, username=None, password=None):
        self.headers = {
            "Content-type": "application/json",
            "Accept": "text/plain"
        }
        
        if username is not None:
            self.headers['Authorization'] = 'Basic ' + \
                base64.encodestring('%s:%s' % (username, password))[:-1]
        self.baseuri  = '/%s/json.php/restricted/%s/%s/?act=%s'
        
        if ssl:
            self.conn = httplib.HTTPSConnection(ip, port)
        else:
            self.conn = httplib.HTTPConnection(ip, port)

        
    def request(self, method, uri, params=None):
        if method == 'POST':
            params = json.encode(params)
        elif params:
            mark   = '&' if '?' in uri else '?'
            uri    = "%s%s%s" % (uri, mark, urllib.urlencode(params))
            params = None

#        print 'request= ', uri
        self.conn.request(method, uri, params, self.headers)
        response = self.conn.getresponse()
        data     = response.read()
        
        return (response, data)
        
    def list(self, obj):
        if obj not in self.objects:
            raise 'Unknown %s object' % obj
         
        params = self.objects[obj]
        return self.request('GET', 
            self.baseuri % (params[0], params[1], obj, 'list')
        )
        
    def add(self, obj, content):
        if obj not in self.objects:
            raise 'Unknown %s object' % obj
         
        params = self.objects[obj]
        return self.request('POST', 
            self.baseuri % (params[0], params[1], obj, 'add'), 
            content
        )
    def view(self, obj, id):
        if obj not in self.objects:
            raise 'Unknown %s object' % obj
         
        params = self.objects[obj]
        return self.request('GET', 
            self.baseuri % (params[0], params[1], obj, 'view'), 
            {'id': id}
        )
        
    def delete(self, obj, id):
        if obj not in self.objects:
            raise 'Unknown %s object' % obj
         
        params = self.objects[obj]
        return self.request('GET', 
            self.baseuri % (params[0], params[1], obj, 'delete'), 
            {'id': id}
        )



