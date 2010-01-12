"""lshw module

Copyright (C) 2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
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

import logging

from xivo import http_json_server
from xivo.http_json_server import HttpReqError
from xivo.http_json_server import CMD_R
from xivo.moresynchro import RWLock
from xivo.xml2dict import XML2Dict

from xivo_sysconf import helpers

import re
import subprocess

log = logging.getLogger('xivo_sysconf.modules.lshw') # pylint: disable-msg=C0103

LSHW_LOCK_TIMEOUT       = 60 # XXX
LSHWLOCK                = RWLock()

LSHW_BIN                = "/usr/bin/lshw"

LSHW_RE_XML_DECLARATION = re.compile(r'^\s*(<\?xml\s+[^>]*>)((?:\s*.*)*)', re.M).match

LSHW_CLASS_LIST = ('system',
                   'brigde',
                   'memory',
                   'processor',
                   'address',
                   'storage',
                   'disk',
                   'tape',
                   'bus',
                   'network',
                   'display',
                   'input',
                   'printer',
                   'multimedia',
                   'communication',
                   'power',
                   'volume',
                   'generic')

LSHW_TEST_LIST  = ('dmi',
                   'device-tree',
                   'spd',
                   'memory',
                   'cpuinfo',
                   'cpuid',
                   'pci',
                   'sapnp',
                   'pcmcia',
                   'ide',
                   'usb',
                   'scsi',
                   'network')


class LshwExecutionError(Exception):
    pass

def Lshw(args, options):    # pylint: disable-msg=W0613
    """
    GET /lshw
    
    Just returns the list hardware

    >>> lshw({}, {'class':  'network'})
    >>> lshw({}, {'class':  {0: 'network', 1: 'system'}})
    >>> lshw({}, {'class':  ['network', 'system']})
    """

    opts = {}

    if 'class' in options:
        options['class'] = helpers.extract_scalar(options['class'])

        if helpers.exists_in_list(options['class'], LSHW_CLASS_LIST):
            opts['class'] = options['class']
        else:
            raise HttpReqError(415, "invalid option 'class'")

    if 'disable' in options:
        options['disable'] = helpers.extract_scalar(options['disable'])

        if helpers.exists_in_list(options['disable'], LSHW_TEST_LIST):
            opts['disable'] = options['disable']
        else:
            raise HttpReqError(415, "invalid option 'disable'")

    if 'enable' in options:
        options['enable'] = helpers.extract_scalar(options['enable'])

        if helpers.exists_in_list(options['enable'], LSHW_TEST_LIST):
            opts['enable'] = options['enable']
        else:
            raise HttpReqError(415, "invalid option 'enable'")

    if 'sanitize' in options:
        opts['sanitize'] = None

    if 'numeric' in options:
        opts['numeric'] = None

    if not LSHWLOCK.acquire_read(LSHW_LOCK_TIMEOUT):
        raise HttpReqError(503, "unable to take LSHWLOCK for reading after %s seconds" % LSHW_LOCK_TIMEOUT)

    lshw_cmd = [LSHW_BIN, '-xml']

    for key, value in opts.iteritems():
        if isinstance(value, (tuple, list)):
            for x in value:
                lshw_cmd.append("-%s" % key)
                lshw_cmd.append(x)
        else:
            lshw_cmd.append("-%s" % key)

            if value is not None:
                lshw_cmd.append(value)

    try:
        lshw = subprocess.Popen(lshw_cmd,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE,
                                close_fds=True)

        if lshw.wait():
            log.error(lshw.stderr.read())
            raise LshwExecutionError("A problem occurred while executing command. (command: %r, error: %r)"
                                     % (lshw_cmd, lshw.stderr.read()))

        data = lshw.stdout.read()
        match = LSHW_RE_XML_DECLARATION(data)

        # XXX Workaround when missing XML top-level
        if match:
            xml = "%s<lshw>%s</lshw>" % (match.group(1), match.group(2))
        else:
            xml = "<lshw>%s</lshw>" % data

        return XML2Dict().Parse(xml)
    finally:
        LSHWLOCK.release()


http_json_server.register(Lshw, CMD_R, name='lshw')
