"""dnetintf module

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

import os
import logging
import re
import dumbnet

from time import time
from shutil import copy2

from xivo import http_json_server
from xivo.http_json_server import HttpReqError
from xivo.http_json_server import CMD_R, CMD_RW
from xivo.moresynchro import RWLock
from xivo import xys
from xivo import network
from xivo import interfaces
from xivo import xivo_config
from xivo import system

from xivo_sysconf import helpers


log = logging.getLogger('xivo_sysconf.modules.dnetintf') # pylint: disable-msg=C0103


class NetworkConfig(dumbnet.intf):
    """
    Network configuration class.
    """

    INTF_TYPES  = dict((getattr(dumbnet, x), x[10:].lower())
                       for x in dir(dumbnet) if x.startswith("INTF_TYPE_"))

    def __init__(self):
        dumbnet.intf.__init__(self)
        self.route              = dumbnet.route()
        self.default_dst_ipv4   = dumbnet.addr('0.0.0.0/0')

    def __realloc__(self):
        dumbnet.intf.__init__(self)

    def _intf_ipv4_gateway(self, ifname):
        """
        Return the default gateway if it belongs to the interface
        """
        defgw   = self.route.get(self.default_dst_ipv4)
        xaddr   = dumbnet.intf.get(self, ifname)['addr']
        route   = self.route.get(xaddr)

        if not defgw \
           or (route and defgw != route):
            return None

        defgwstr = str(defgw)

        address = network.parse_ipv4(dumbnet.ip_ntoa(xaddr.ip))
        netmask = network.bitmask_to_mask_ipv4(xaddr.bits)
        net     = network.mask_ipv4(netmask, address)
        netgw   = network.mask_ipv4(netmask, network.parse_ipv4(defgwstr))

        if net == netgw:
            return defgwstr

    def _intf_repr(self, ifent):
        """
        Return the configuration for a network interface as a dict like interfaces(5).
        """
        if not ifent or not ifent.has_key('addr'):
            return ifent

        ret = {'name': ifent['name']}

        xaddr = ifent['addr']
        if xaddr.addrtype == dumbnet.ADDR_TYPE_IP:
            ret['address']      = dumbnet.ip_ntoa(xaddr.ip)
            ret['netmask']      = network.format_ipv4(
                                        network.bitmask_to_mask_ipv4(xaddr.bits))
            ret['broadcast']    = str(xaddr.bcast())
            ret['network']      = str(xaddr.net())
            ret['mtu']          = ifent['mtu']
            ret['flags']        = ifent['flags']
            ret['type']         = self.INTF_TYPES[ifent['type']]
            ret['family']       = 'inet'

            gw = self._intf_ipv4_gateway(ifent['name'])

            if gw:
                ret['gateway'] = gw

            if ifent.has_key('dst_addr'):
                ret['pointopoint'] = str(ifent['dst_addr'])

            if ifent.has_key('link_addr'):
                ret['hwaddress'] = str(ifent['link_addr'])
        elif xaddr.addrtype == dumbnet.ADDR_TYPE_IP6:
            ret['address']      = dumbnet.ip6_ntoa(xaddr.ip6)
            ret['netmask']      = xaddr.bits
            ret['broadcast']    = str(xaddr.bcast())
            ret['mtu']          = ifent['mtu']
            ret['flags']        = ifent['flags']
            ret['type']         = self.INTF_TYPES[ifent['type']]
            ret['family']       = 'inet6'

            if ifent.has_key('link_addr'):
                ret['hwaddress'] = str(ifent['link_addr'])

        return ret

    def _iter_append(self, entry, l):
        l.append(self._intf_repr(entry))

    def get(self, name):
        """
        Return the configuration for a network interface as a dict.
        """
        return self._intf_repr(dumbnet.intf.get(self, name))

    def get_dst(self, dst):
        """
        Return the configuration for the best interface with which to
        reach the specified dst address.
        """
        self.__realloc__() # XXX: Workarround
        return self._intf_repr(dumbnet.intf.get_dst(self, dst))

    def get_src(self, src):
        """
        Return the configuration for the interface whose primary address
        matches the specified source address.
        """
        return self._intf_repr(dumbnet.intf.get_src(self, src))

    def set(self, d, name=None):
        """
        Set the configuration for an interface from a dict like interfaces(5).
        """
        if name is not None:
            d['name'] = name

        iface = self.get(d['name'])

        address = d.get('address', iface['address'])
        netmask = d.get('netmask', iface['netmask'])

        d['addr'] = dumbnet.addr("%s/%s" % (address, netmask))

        newgateway = None
        delgateway = None

        if d.has_key('gateway'):
            if iface['type'] != 'eth' or iface['family'] != 'inet':
                raise NotImplementedError("This method only supports modify IPv4 gateway for ethernet interface")

            gwstr = network.format_ipv4(network.parse_ipv4("%s" % d['gateway']))

            log.info("######## toto: %r:%r ########", iface.get('gateway'), gwstr)
            if iface.get('gateway', None) != gwstr:
                newgateway = dumbnet.addr("%s" % d['gateway'])
        # If d hasn't gateway but iface has a gateway, remove previous gateway.
        elif iface.has_key('gateway'):
            if iface['type'] != 'eth' or iface['family'] != 'inet':
                raise NotImplementedError("This method only supports modify IPv4 gateway for ethernet interface")

            delgateway = d['addr']

        if d.has_key('pointopoint'):
            d['dst_addr'] = dumbnet.addr("%s" % d['pointopoint'])

        if d.has_key('hwaddress'):
            d['link_addr'] = dumbnet.addr("%s" % d['hwaddress'], dumbnet.ADDR_TYPE_ETH)

        dumbnet.intf.set(self, d)

        # If iface has previously a default gateway
        if delgateway:
            try:
                self.route.delete(self.default_dst_ipv4)
            except OSError, e:
                # If an error has occurred, rollback
                if iface.has_key('gateway'):
                    del iface['gateway']

                self.set(iface)
                raise OSError(str(e))
        elif newgateway:
            prevdefgw = self.route.get(self.default_dst_ipv4)
            try:
                self.route.delete(self.default_dst_ipv4)
            except OSError:
                prevdefgw = None

            try:
                # Set a new default gateway.
                log.info("Set a new default gateway: %r:%r", self.default_dst_ipv4, newgateway)
                self.route.add(self.default_dst_ipv4, newgateway)
            except OSError, e:
                # If an error has occurred, rollback
                if iface.has_key('gateway'):
                    del iface['gateway']

                self.set(iface)

                # If there is a previous gateway
                if prevdefgw:
                    self.route.add(self.default_dst_ipv4, prevdefgw)
                raise OSError(str(e))

    def loop(self, callback, arg=None):
        """
        Iterate over the system interface table, invoking a user callback
        with each entry, returning the status of the callback routine.
        """
        l = []
        dumbnet.intf.loop(self, self._iter_append, l)

        for x in l:
            callback(x, arg)


class DNETIntf:
    """
    Network Interfaces class.
    """

    RE_MATCH_GW_IP  = re.compile('^\s*gateway\s*([a-fA-F0-9\.:]+)\s*$').match
    LOCK            = RWLock()
    CONFIG          = {'interfaces_file':       os.path.join(os.path.sep, 'etc', 'network', 'interfaces'),
                       'interfaces_tpl_file':   os.path.join('network', 'interfaces'),
                       'lock_timeout':          60}

    def __init__(self):
        self.netcfg     = NetworkConfig()

        self.args       = {}
        self.options    = {}

    def _netiface_from_address(self, function):
        """
        Find best interface from destination or source address.
        """
        if 'address' in self.options:
            addresses = helpers.extract_scalar(self.options['address'])
        else:
            addresses = None

        if not addresses:
            HttpReqError(415, "invalid option 'address'")
        else:
            try:
                addresses = [dumbnet.addr(x) for x in addresses]
            except ValueError, e:
                raise HttpReqError(415, "%s: %s" % (e, x))

        if not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take self.LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        try:
            return dict((address, function(address)) for address in addresses)
        finally:
            self.LOCK.release()

    def safe_init(self, options):
        """Load parameters, etc"""
        cfg = options.configuration

        tpl_path        = cfg.get('general', 'templates_path')
        custom_tpl_path = cfg.get('general', 'custom_templates_path')
        backup_path     = cfg.get('general', 'backup_path')

        if cfg.has_section('network'):
            if cfg.has_option('network', 'lock_timeout'):
                self.CONFIG['lock_timeout'] = cfg.getfloat('network', 'lock_timeout')

            if cfg.has_option('network', 'interfaces_file'):
                self.CONFIG['interfaces_file'] = cfg.get('network', 'interfaces_file')

        self.CONFIG['interfaces_tpl_file'] = os.path.join(tpl_path,
                                                          self.CONFIG['interfaces_tpl_file'])

        self.CONFIG['interfaces_custom_tpl_file'] = os.path.join(custom_tpl_path,
                                                                 self.CONFIG['interfaces_tpl_file'])

        self.CONFIG['interfaces_path'] = os.path.dirname(self.CONFIG['interfaces_file'])
        self.CONFIG['interfaces_backup_file'] = os.path.join(backup_path,
                                                             self.CONFIG['interfaces_file'].lstrip(os.path.sep))
        self.CONFIG['interfaces_backup_path'] = os.path.join(backup_path,
                                                             self.CONFIG['interfaces_path'].lstrip(os.path.sep))

    def discover_netifaces(self, args, options):
        """
        GET /discover_netifaces
        """
        self.args       = args
        self.options    = options

        if not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take self.LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        rs = {}

        try:
            self.netcfg.loop(lambda iface, rs: rs.update({iface['name']: iface}), rs)
            return rs
        finally:
            self.LOCK.release()

    def netiface(self, args, options):
        """
        GET /netiface

        >>> netiface({}, {})
        >>> netiface({}, {'iface':   'eth0'})
        >>> netiface({}, {'iface':   {0: 'eth0', 1: 'eth1'}})
        >>> netiface({}, {'iface':   ['eth0', 'eth1']})
        """
        self.args       = args
        self.options    = options

        if 'iface' in self.options:
            ifaces = helpers.extract_scalar(self.options['iface'])
            if not ifaces:
                HttpReqError(415, "invalid option 'iface'")
        else:
            return self.discover_netifaces({}, {})

        if not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take self.LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        try:
            return dict((iface, self.netcfg.get(iface)) for iface in ifaces)
        finally:
            self.LOCK.release()

    def netiface_from_dst_address(self, args, options):
        """
        GET /netiface_from_dst_address

        >>> netiface_from_dst_address({}, {'address':   '192.168.0.1'})
        >>> netiface_from_dst_address({}, {'address':   {0: '192.168.0.1', 1: '172.16.1.1'}})
        >>> netiface_from_dst_address({}, {'address':   ['192.168.0.1', '172.16.1.1']})
        """
        self.args       = args
        self.options    = options

        return self._netiface_from_address(self.netcfg.get_dst)

    def netiface_from_src_address(self, args, options):
        """
        GET /netiface_from_src_address

        >>> netiface_from_src_address({}, {'address':   '192.168.0.1'})
        >>> netiface_from_src_address({}, {'address':   {0: '192.168.0.1', 1: '172.16.1.1'}})
        >>> netiface_from_src_address({}, {'address':   ['192.168.0.1', '172.16.1.1']})
        """
        self.args       = args
        self.options    = options

        return self._netiface_from_address(self.netcfg.get_src)

    def _get_valid_eth_ipv4(self):
        if 'name' in self.options:
            if not xivo_config.netif_managed(self.options['name']):
                raise HttpReqError(415, "invalid interface name, name: %r" % self.options['name'])

            try:
                eth = self.netcfg.get(self.options['name'])
            except (OSError, TypeError), e:
                raise HttpReqError(415, "%s: %r", (e, self.options['name']))

            if eth['type'] != 'eth':
                raise HttpReqError(415, "invalid interface type")
            elif eth['family'] != 'inet':
                raise HttpReqError(415, "invalid address family")
        else:
            raise HttpReqError(415, "missing option 'name'")

        return eth

    def static_eth_gateway(self, ifname):
        """
        Return the static gateway if it exists.
        """

        if not os.access(self.CONFIG['interfaces_file'], os.R_OK):
            return False

        iflines = file(self.CONFIG['interfaces_file'])

        eni = interfaces.parse(iflines)

        for block in eni:
            if not isinstance(block, interfaces.EniBlockWithIfName) \
               or block.ifname != ifname:
                continue

            for line in block.cooked_lines:
                match = self.RE_MATCH_GW_IP(line)
                if match:
                    iflines.close()
                    return "%s" % dumbnet.addr(match.group(1))

        iflines.close()
        return None

    UPDATE_ETH_SCHEMA  = xys.load("""
    address:    !~ipv4_address 192.168.0.1
    netmask:    !~netmask 255.255.255.0
    broadcast?: !~ipv4_address 192.168.0.255
    gateway?:   !~ipv4_address 192.168.0.254
    mtu?:       !~~between(68,1500) 1500
    auto?:      !!bool True
    up?:        !!bool True
    """)

    def update_eth_ipv4(self, args, options):
        """
        POST /update_eth_ipv4

        >>> update_eth_ipv4({'address':     '192.168.0.1',
                             'netmask':     '255.255.255.0',
                             'broadcast':   '192.168.0.255',
                             'gateway':     '192.168.0.254',
                             'mtu':         1500,
                             'auto':        True,
                             'up':          True},
                            {'name': 'eth0'})
        """
        self.args       = args
        self.options    = options

        eth = self._get_valid_eth_ipv4()

        if not xys.validate(self.args, self.UPDATE_ETH_SCHEMA):
            raise HttpReqError(415, "invalid arguments for command")

        if self.args.has_key('up'):
            if self.args['up']:
                eth['flags'] |= dumbnet.INTF_FLAG_UP
            else:
                eth['flags'] &= ~dumbnet.INTF_FLAG_UP
            del args['up']

        if not self.args.has_key('broadcast') and eth.has_key('broadcast'):
            del eth['broadcast']

        if not self.args.has_key('gateway') and eth.has_key('gateway'):
            del eth['gateway']

        if not self.args.has_key('mtu') and eth.has_key('mtu'):
            del eth['mtu']

        eth.update(self.args)

        eth['auto'] = self.args.get('auto', True)

        if not xivo_config.plausible_static(eth, None):
            raise HttpReqError(415, "invalid arguments for command")

        if not os.access(self.CONFIG['interfaces_path'], (os.X_OK | os.W_OK)):
            raise HttpReqError(415, "path not found or not writable or not executable: %r" % self.CONFIG['interfaces_path'])

        if not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take self.LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        conf = {'netIfaces':    {},
                'vlans':        {},
                'ipConfs':      {}}

        netifacesbakfile    = None

        try:
            if not os.path.isdir(self.CONFIG['interfaces_backup_path']):
                os.makedirs(self.CONFIG['interfaces_backup_path'])

            if os.access(self.CONFIG['interfaces_file'], os.R_OK):
                netifacesbakfile = "%s.%d" % (self.CONFIG['interfaces_backup_file'], time())
                copy2(self.CONFIG['interfaces_file'], netifacesbakfile)
                old_lines = file(self.CONFIG['interfaces_file'])
            else:
                old_lines = ()

            self.netcfg.loop(lambda iface, rs: rs.update({iface['name']: 'reserved'}), conf['netIfaces'])
            conf['netIfaces'][eth['name']]  = eth['name']
            conf['vlans'][eth['name']]      = {0: eth['name']}
            conf['ipConfs'][eth['name']]    = eth

            if os.access(self.CONFIG['interfaces_custom_tpl_file'], (os.F_OK | os.R_OK)):
                filename = self.CONFIG['interfaces_custom_tpl_file']
            else:
                filename = self.CONFIG['interfaces_tpl_file']

            template_file = open(filename)
            template_lines = template_file.readlines()
            template_file.close()

            txt = xivo_config.txtsubst(template_lines,
                                       {'_XIVO_NETWORK_INTERFACES':
                                            ''.join(xivo_config.generate_interfaces(old_lines, conf))},
                                        self.CONFIG['interfaces_file'],
                                        'utf8')

            if old_lines:
                old_lines.close()

            system.file_writelines_flush_sync(self.CONFIG['interfaces_file'], txt)

            if eth.has_key('gateway') and not (eth['flags'] & dumbnet.INTF_FLAG_UP):
                del eth['gateway']

            try:
                self.netcfg.set(eth)
            except Exception, e:
                if netifacesbakfile:
                    copy2(netifacesbakfile, self.CONFIG['interfaces_file'])
                raise e.__class__(str(e))
            return True
        finally:
            self.LOCK.release()

    CHANGE_STATE_ETH_SCHEMA = xys.load("""
    state:      !!bool True
    """)

    def change_state_eth_ipv4(self, args, options):
        """
        POST /change_state_eth_ipv4

        >>> change_state_eth_ipv4({'state':   True},
                                  {'name': 'eth0'})
        """
        self.args       = args
        self.options    = options

        eth = self._get_valid_eth_ipv4()

        if not xys.validate(self.args, self.CHANGE_STATE_ETH_SCHEMA):
            raise HttpReqError(415, "invalid arguments for command")

        if self.args['state']:
            eth['flags'] |= dumbnet.INTF_FLAG_UP

            gateway = self.static_eth_gateway(eth['name'])
            if gateway:
                eth['gateway'] = gateway
        else:
            eth['flags'] &= ~dumbnet.INTF_FLAG_UP

            if eth.has_key('gateway'):
                del eth['gateway']

        if not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take self.LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        try:
            self.netcfg.set(eth)
            return True
        finally:
            self.LOCK.release()


dnetintf = DNETIntf()

http_json_server.register(dnetintf.discover_netifaces, CMD_R, safe_init=dnetintf.safe_init)
http_json_server.register(dnetintf.netiface, CMD_R)
http_json_server.register(dnetintf.netiface_from_dst_address, CMD_R)
http_json_server.register(dnetintf.netiface_from_src_address, CMD_R)
http_json_server.register(dnetintf.update_eth_ipv4, CMD_RW)
http_json_server.register(dnetintf.change_state_eth_ipv4, CMD_RW)
