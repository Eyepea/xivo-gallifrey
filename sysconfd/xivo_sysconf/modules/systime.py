from __future__ import with_statement
"""systime module

Manage system time informations
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

import os.path, logging

from xivo import http_json_server
from xivo.http_json_server import HttpReqError
from xivo.http_json_server import CMD_RW, CMD_R
from xivo_sysconf import helpers

logger = logging.getLogger('xivo_sysconf.modules.services')

def timezone(args, options):
    """Return system timezone. * is Debian specific (and probably ubuntu too) *
    GET /timezone

    >>> return: 'Europe/Paris'
    """
    tz = None
    if os.path.exists('/etc/timezone'):
        with open('/etc/timezone') as f:
            tz = f.readline()[:-1]

    return tz


http_json_server.register(timezone, CMD_R, name='timezone')

