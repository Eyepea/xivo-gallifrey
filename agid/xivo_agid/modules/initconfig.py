"""Provisioning AGI for XIVO

Copyright (C) 2007-2010  Proformatique <technique@proformatique.com>

"""
# TODO WARNING: can be used only if the caller is of a SIP tech

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007-2010  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""


CONFIG_FILE = '/etc/pf-xivo/provisioning.conf'

import logging
import socket
import cjson
import httplib
import sys

from xivo import xivo_config
from xivo.xivo_config import ProvGeneralConf as Pgc
from xivo import all_phones # pylint: disable-msg=W0611
from xivo import agitb

from xivo_agid import agid

log = logging.getLogger('xivo_agid.modules.initconfig') # pylint: disable-msg=C0103


def send_error(agi, error, playback=None):
    "agi.verbose and if playback given, run app PLAYBACK"
    agi.verbose(error)
    if playback:
        agi.appexec("PLAYBACK", playback)


# TODO: use an RFC compliant regexp instead of
# this stupid way of parsing things
def user_ipv4_from_sip_uri(sip_addr):
    "parse the sip uri, return (sip_user, ip)"
    splitted_sip = sip_addr.split(':')
    if len(splitted_sip) < 2:
        return None
    splitted_sip = splitted_sip[1].split('@')
    if len(splitted_sip) < 2:
        return None
    sip_user = splitted_sip[0]
    ip = splitted_sip[1].split('>')[0]
    return (sip_user, ip)


def initconfig(agi, cursor, args): # pylint: disable-msg=W0613
    """
    Provisioning by code on keypad.
    """
    sip_uri = args[0]
    code = args[1]
    ua = args[2]
    isinalan = 1

    # Get Sip User, IPv4 and Mac Address
    user_ipv4 = user_ipv4_from_sip_uri(sip_uri)
    if not user_ipv4:
        send_error(agi, "Could not parse Sip URI \"%s\"" % sip_uri)
        return
    sip_user, ipv4 = user_ipv4 # pylint: disable-msg=W0612
    macaddr = xivo_config.macaddr_from_ipv4(ipv4) # XXX, agi_session.verbose)
    if not macaddr:
        send_error(agi, "Could not find Mac Address from IPv4 \"%s\"" % ipv4)
        return

    # Get Phone description (if we are able to handle this vendor...)
    phone_desc = xivo_config.phone_desc_by_ua(ua) # XXX, agitb.handler)
    if not phone_desc:
        send_error(agi, "Unknown UA %r" % (ua,))
        return
    phone_vendor = phone_desc[0]
    phone_model = phone_desc[1]

    if code == 'init':
        code = '0'
    if not xivo_config.well_formed_provcode(code):
        send_error(agi, "Badly formed provisioning code", "privacy-incorrect")
        return
    code = int(code)

    command = {
        'mode':     'authoritative',
        'vendor':   phone_vendor,
        'model':    phone_model,
        'macaddr':  macaddr,
        'ipv4':     ipv4,
        'provcode': code,
        'actions':  'yes',
        'proto':    'sip',
        'isinalan': isinalan,
    }

    try:
        socket.setdefaulttimeout(float(Pgc['http_request_to_s']))
        conn = httplib.HTTPConnection(Pgc['connect_ipv4'] + ':' + str(Pgc['connect_port']))
        conn.request("POST", "/provisioning", cjson.encode(command), {"Content-Type": "application/json"})
        response = conn.getresponse()
        response.read() # eat every data sent by the provisioning server
        conn.close()
        reason = response.reason
        status = response.status
    except Exception, xcept:
        reason = str(xcept)
        status = 500
        agitb.handler()
        del xcept
        sys.exc_clear()

    if status != 200:
        send_error(agi, "Provisioning failure; %s" % reason, "prov-error")
        return


def setup(cursor): # pylint: disable-msg=W0613
    """
    (re)load config
    """
    xivo_config.LoadConfig(CONFIG_FILE)
    xivo_config.phone_classes_setup()


agid.register(initconfig, setup)
