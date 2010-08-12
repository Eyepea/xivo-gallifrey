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

import os, re, logging, subprocess, cjson, traceback, cStringIO
from datetime import datetime

from xivo import http_json_server
from xivo.http_json_server import HttpReqError
from xivo.http_json_server import CMD_RW, CMD_R
from xivo_sysconf import helpers, jsoncore

from pf_lib import ocf


class Ha(jsoncore.JsonCore):
    """
    """
    def __init__(self):
        super(Ha, self).__init__()
        self.log = logging.getLogger('xivo_sysconf.modules.ha')

        http_json_server.register(self.generate , CMD_RW, name='ha_generate',
            safe_init=self.safe_init)
        http_json_server.register(self.status   , CMD_R , name='ha_status')
        http_json_server.register(self.apply    , CMD_R , name='ha_apply')
        http_json_server.register(self.stop     , CMD_R , name='ha_stop')

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

    def _exec(self, title, command, output):
        output.append("* %s" % title)
        p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        ret = p.wait()
        output.append(p.stdout.read())
        
        self.log.debug("  . %s: %d" % (command, ret))
        if ret != 0:
            raise HttpReqError(500, "\n".join(output))
            
    def apply(self, args, options):
        # We follow the steps described at https://wiki.xivo.fr/index.php/Install_XiVO_HA
        self.log.debug('** apply HA changes **')
        out = ["** apply HA changes **"]
        
        """
            case #1: HA not starter (ha_master_uname == '')
              we cannot stand if we have another HA node
              : HA configuration is applied

            case #2: we are not master node
              : configuration not applied

            case #3: slave nodes are not stopped
              : configuration not applied
        """

        master = ocf.ha_node_uname(ocf.ha_uuid())
        if ocf.ha_master_uname() is not None and master != ocf.ha_master_uname():
            out.append('Changes not applied: current node MUST be master node')
            raise HttpReqError(500, '\n'.join(out))

        slaves = ocf.ha_nodes_uname_except(master)
        if slaves is not None:
            for slave in slaves:
                if ocf.ha_node_status(slave) == 'active':
                    out.append('Changes not applied: slave node \'%s\' is active' % slave)
                    raise HttpReqError(500, '\n'.join(out))

        try:
            #1. stop heartbeat
            self._exec('stopping heartbeat', ['/etc/init.d/heartbeat', 'stop'], out)
                
            #2. start mysql
            self._exec('start mysql', ['/etc/init.d/mysql', 'start'], out)
            
            #3.  execute update-pf-ha
            self._exec('executing update-pf-ha', ['/usr/sbin/update-pf-ha'], out)
                
            match = re.match(".*CIB file written:\s+(/tmp/cib.xml.[0-9]+).*", out[-1], re.S|re.M)
            if match is None:
                out.append("ERROR: no CIB file")
                raise HttpReqError(500, "\n".join(out))
                
            cibfile = match.group(1)
            if not os.path.isfile(cibfile):
                out.append("ERROR: CIB file %s not found" % cibfile)
                raise HttpReqError(500, "\n".join(out))
                
            #4. stopping mysql
            self._exec('stop mysql', ['/etc/init.d/mysql', 'stop'], out)
            
            #5. verify CIB file
            self._exec("verify %s CIB file" % cibfile, 
                ['/usr/sbin/crm_verify', '-V', "--xml-file=%s" % cibfile], 
                out
            )

            #6. clean heartbeat state
            self._exec('clean heartbeat CRM state', 
                ['/bin/rm', '-Rf', '/var/lib/heartbeat/crm/*'], 
                out
            )

            #7. install cib file
            self._exec("install CIB file", 
                ['cp', '-a', cibfile, '/var/lib/heartbeat/crm/cib.xml'], 
                out
            )
            
            #8. start heartbeat
            self._exec("start heartbeat", ['/etc/init.d/heartbeat', 'start'], out)
            
        except OSError, e:
            traceback.print_exc()
            raise HttpReqError(500, '\n'.join(out))

        return "\n".join(out)

    def stop(self, args, options):
        out = ["** stopping HA **"]
        self.log.debug(out[0])
        
        try:
            #1. stop heartbeat
            self._exec('stopping heartbeat', ['/etc/init.d/heartbeat', 'stop'], out)
        except OSError, e:
            traceback.print_exc()
            raise HttpReqError(500, "Can't stop ha")

        return "\n".join(out)

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
