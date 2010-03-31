"""munin module
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

import os, logging, subprocess
from datetime import datetime

from xivo import http_json_server
from xivo.http_json_server import CMD_RW, CMD_R

from xivo_sysconf import helpers


class Munin(object):
    """
    """
    def __init__(self):
        super(Munin, self).__init__()
        self.log = logging.getLogger('xivo_sysconf.modules.munin')

        http_json_server.register(self.update , CMD_RW, 
            safe_init=self.safe_init, 
            name='munin_update')
            
        self.cmd = '/usr/sbin/update-pf-stats-munin'
        
    def safe_init(self, options):
        pass
   
    def update(self, args, options):
        try:
            p = subprocess.Popen([self.cmd])
            ret = p.wait()
        except OSError:
            raise HttpReqError(500, "can't execute '%s'" % self.configexec)

        if ret != 0:
            raise HttpReqError(500, "'%s' process return error %d" % (self.cmd, ret))

        return True
        
munin = Munin()
