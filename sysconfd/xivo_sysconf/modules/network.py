"""Network configuration module

Copyright (C) 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008  Proformatique

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


from xivo import http_json_server
from xivo.http_json_server import HttpReqError
from xivo.http_json_server import CMD_R, CMD_RW
from xivo.moresynchro import RWLock
from xivo import xivo_config
from xivo import yaml_json
from xivo import xys
from xivo import json_ops


NET_LOCK_TIMEOUT = 60 # XXX
NETLOCK = RWLock()


def network_config(args): # pylint: disable-msg=W0613
    """
    GET /network_config
    
    Just returns the network configuration
    """
    if not NETLOCK.acquire_read(NET_LOCK_TIMEOUT):
        raise HttpReqError(503, "unable to take NETLOCK for reading after %s seconds" % NET_LOCK_TIMEOUT)
    try:
        netconf = xivo_config.load_current_configuration()
        return yaml_json.stringify_keys(netconf)
    finally:
        NETLOCK.release()


REN_ETH_SCHEMA = xys.load("""
old_name: !~~prefixedDec eth
new_name: !~~prefixedDec eth
""")

def rename_ethernet_interface(args):
    """
    POST /rename_ethernet_interface
    
    args ex:
    {'old_name': "eth42",
     'new_name': "eth1"}
    """
    if not xys.validate(args, REN_ETH_SCHEMA):
        raise HttpReqError(415, "invalid arguments for command")
    if not NETLOCK.acquire_write(NET_LOCK_TIMEOUT):
        raise HttpReqError(503, "unable to take NETLOCK for writing after %s seconds" % NET_LOCK_TIMEOUT)
    try:
        xivo_config.rename_ethernet_interface(args['old_name'], args['new_name'])
        return True
    finally:
        NETLOCK.release()


SWAP_ETH_SCHEMA = xys.load("""
name1: !~~prefixedDec eth
name2: !~~prefixedDec eth
""")

def swap_ethernet_interfaces(args):
    """
    POST /swap_ethernet_interfaces
    
    args ex:
    {'name1': "eth0",
     'name2': "eth1"}
    """
    if not xys.validate(args, SWAP_ETH_SCHEMA):
        raise HttpReqError(415, "invalid arguments for command")
    if not NETLOCK.acquire_write(NET_LOCK_TIMEOUT):
        raise HttpReqError(503, "unable to take NETLOCK for writing after %s seconds" % NET_LOCK_TIMEOUT)
    try:
        xivo_config.swap_ethernet_interfaces(args['name1'], args['name2'])
        return True
    finally:
        NETLOCK.release()


def _val_modify_network_config(args):
    """
    ad hoc validation function for modify_network_config command
    """
    if set(args) != set(['rel', 'old', 'chg']):
        return False
    if not isinstance(args['rel'], list):
        return False
    for elt in args['rel']:
        if not isinstance(elt, basestring):
            return False
    return True


def modify_network_config(args):
    """
    POST /modify_network_config
    
    XXX
    """
    if not _val_modify_network_config(args):
        raise HttpReqError(415, "invalid arguments for command")
    try:
        check_conf = json_ops.compile_conj(args['rel'])
    except ValueError:
        raise HttpReqError(415, "invalid relation")
    
    if not NETLOCK.acquire_write(NET_LOCK_TIMEOUT):
        raise HttpReqError(503, "unable to take NETLOCK for writing after %s seconds" % NET_LOCK_TIMEOUT)
    try:
        current_config = xivo_config.load_current_configuration()
        if not check_conf(args['old'], current_config):
            raise HttpReqError(409, "Conflict between state wanted by client and current state")
        
    finally:
        NETLOCK.release()


http_json_server.register(network_config, CMD_R)
http_json_server.register(rename_ethernet_interface, CMD_RW)
http_json_server.register(swap_ethernet_interfaces, CMD_RW)
