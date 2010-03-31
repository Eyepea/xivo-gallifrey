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
from xivo.http_json_server import CMD_RW, CMD_R
from xivo_sysconf import helpers, jsoncore

from pf_lib import ocf


class Ha(jsoncore.JsonCore):
    """
    """
    def __init__(self):
        super(Ha, self).__init__()
        self.log = logging.getLogger('xivo_sysconf.modules.ha')

        http_json_server.register(self.generate , CMD_R , name='ha_generate',
            safe_init=self.safe_init)
        http_json_server.register(self.status   , CMD_R , name='ha_status')
        
    def safe_init(self, options):
        super(Ha, self).safe_init(options)
        
        self.file       = options.configuration.get('ha', 'ha_file')
        self.cmd        = options.configuration.get('ha', 'ha_cmd')
   
    SECTIONS  = {
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
    KEYSELECT = 'pf.ha'

    def reload(self, args, options):
        try:
            p = subprocess.Popen([self.cmd])
            ret = p.wait()
        except OSError:
            raise HttpReqError(500, "can't execute '%s'" % self.configexec)

        if ret != 0:
            raise HttpReqError(500, "'%s' process return error %d" % (self.cmd, ret))

        return True
        
    def status(self, args, options):
        master = ocf.ha_master_uname()
        myself = ocf.ha_node_uname(ocf.ha_uuid())
        
        status = 'unknown'
        if master is None:
            status = 'down'
        elif myself == master:
            status = 'master'
        else:
            status = 'slave'
            
        return status
        
        
ha = Ha()
