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
import netifaces
import subprocess

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


class InetxParser:
    MATCH_SINGLEARG     = re.compile('^\s*([^\s]+)\s+([^\s#]+)').match
    MATCH_MULTIARGS     = re.compile('^\s*([^\s]+)\s+([^#]+)').match
    MATCH_HWADDRESS     = re.compile('^\s*hwaddress\s+(\w+)\s+([a-fA-F0-9:]+)').match
    OPTIONS             = {}

    @staticmethod
    def parse_multiargs(line):
        match = InetxParser.MATCH_MULTIARGS(line)

        if match:
            args = match.group(2).strip()
            if args:
                return (match.group(1), args)

        return (None, None)

    @staticmethod
    def parse_hwaddress(hwaddress):
        match = InetxParser.MATCH_HWADDRESS(hwaddress)

        if match:
            return ' '.join(match.groups())

    def __init__(self, filename):
        self.OPTIONS    = {'hwaddress': self.parse_hwaddress}

        self.filename   = filename
        self.fp         = file(filename)
        self.interfaces = None

    def reloadfile(self):
        if self.fp and not self.fp.closed:
            self.fp.close()

        self.fp         = file(self.filename)
        self.interfaces = {}

    def ifaces(self, reloadfile=False):
        if reloadfile:
            self.reloadfile()
        elif self.interfaces is None:
            self.interfaces = {}
        elif self.interfaces:
            return self.interfaces

        eni = interfaces.parse(self.fp)

        for block in eni:
            if not isinstance(block, interfaces.EniBlockIface):
                continue

            ifname = block.ifname
            self.interfaces[ifname] = [('name', ifname)]

            if isinstance(block, (interfaces.EniBlockFamilyInet, interfaces.EniBlockFamilyInet6)):
                self.interfaces[ifname].append(('family', block.family))
                self.interfaces[ifname].append(('method', block.method))

            for line in block.cooked_lines:
                if line.split(None, 1)[0] in ('name', 'iface', 'family', 'method'):
                    continue

                option, args = self.parse_multiargs(line)
                if not option:
                    continue
                elif not self.OPTIONS.has_key(option):
                    self.interfaces[ifname].append((option, args))
                    continue

                parsed = self.OPTIONS[option](line)
                if parsed:
                    self.interfaces[ifname].append((option, parsed))

        return self.interfaces

    def get(self, iface, reloadfile=False):
        if self.interfaces is None:
            self.ifaces()
        else:
            self.ifaces(reloadfile)

        return self.interfaces.get(iface)

    def close(self):
        if self.fp and not self.fp.closed:
            self.fp.close()


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

        if network.ipv4_in_network(network.parse_ipv4(defgwstr),
                                   netmask,
                                   network.mask_ipv4(netmask, address)):
            return defgwstr

    def _intf_repr(self, ifent):
        """
        Return the configuration for a network interface as a dict like interfaces(5).
        """
        if not ifent \
           or ifent['type'] not in (dumbnet.INTF_TYPE_LOOPBACK, dumbnet.INTF_TYPE_ETH):
            return ifent

        ret = {'name': ifent['name']}

        if not ifent.has_key('addr'):
            ret['mtu']          = ifent['mtu']
            ret['flags']        = ifent['flags']
            ret['type']         = self.INTF_TYPES[ifent['type']]
            ret['typeid']       = ifent['type']

            if ret['type'] == 'eth':
                ret['family'] = 'inet'
            else:
                ret['family'] = 'unknown'

            if ifent.has_key('link_addr'):
                ret['hwaddress'] = str(ifent['link_addr'])

            return ret

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
            ret['typeid']       = ifent['type']
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
                # Set a new default gateway
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

    LOCK    = RWLock()
    CONFIG  = {'interfaces_file':       os.path.join(os.path.sep, 'etc', 'network', 'interfaces'),
               'interfaces_tpl_file':   os.path.join('network', 'interfaces'),
               'netiface_up_cmd':       "sudo /sbin/ifup",
               'netiface_down_cmd':     "sudo /sbin/ifdown",
               'lock_timeout':          60}

    def __init__(self):
        self.netcfg         = NetworkConfig()
        self.inetxparser    = InetxParser(self.CONFIG['interfaces_file'])

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
            raise HttpReqError(415, "invalid option 'address'")
        else:
            try:
                addresses = [dumbnet.addr(x) for x in addresses]
            except ValueError, e:
                raise HttpReqError(415, "%s: %s" % (e, x))

        if not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        try:
            return dict((str(address), function(address)) for address in addresses)
        finally:
            self.LOCK.release()

    def get_netiface_info(self, iface):
        try:
            info = self.netcfg.get(iface)
        except OSError:
            return False

        if not info or not info.has_key('family'):
            return False

        info['carrier']             = False
        info['physicalif']          = False
        info['vlanif']              = False
        info['vlan-id']             = None
        info['vlan-raw-device']     = None
        info['aliasif']             = False
        info['alias-raw-device']    = None
        info['hwtypeid']            = None
        info['options']             = None

        if network.is_linux_netdev_if(iface):
            info['carrier']     = network.is_interface_plugged(iface)
            info['flags']       = network.get_interface_flags(iface)
            info['physicalif']  = network.is_linux_phy_if(iface)
            info['vlanif']      = network.is_linux_vlan_if(iface)
            info['hwtypeid']    = network.get_interface_hwtypeid(iface)

            if not info['physicalif'] and info.has_key('gateway'):
                del info['gateway']

            if not info.has_key('hwaddress'):
                info['hwaddress'] = network.get_interface_hwaddress(iface)

            if not info.has_key('mtu'):
                info['mtu'] = network.get_interface_mtu(iface)
        else:
            if network.is_alias_if(iface):
                info['aliasif'] = True
                phyifname = network.phy_name_from_alias_if(iface)
                info['alias-raw-device'] = phyifname

                if network.is_linux_netdev_if(phyifname):
                    if info.has_key('gateway'):
                        try:
                            phyinfo = self.netcfg.get(phyifname)
                            if phyinfo.get('gateway') == info['gateway']:
                                del info['gateway']
                        except OSError:
                            pass

                    info['carrier']     = network.is_interface_plugged(phyifname)
                    info['hwtypeid']    = network.get_interface_hwtypeid(phyifname)

            if not info.has_key('flags'):
                info['flags'] = None

            if not info.has_key('hwaddress'):
                info['hwaddress'] = None

            if not info.has_key('mtu'):
                info['mtu'] = None

        if info['family'] in ('inet', 'inet6'):
            inetxparsed = self.inetxparser.get(iface)
            if inetxparsed:
                info['options'] = xivo_config.unreserved_interfaces_options(inetxparsed)

                xdict                   = dict(inetxparsed)
                info['method']          = xdict.get('method')
                info['vlan-id']         = xdict.get('vlan-id')
                info['vlan-raw-device'] = xdict.get('vlan-raw-device')

                if 'address' not in info \
                    and 'address' in xdict \
                    and 'netmask' in xdict:
                    info['address'] = xdict.get('address')
                    info['netmask'] = xdict['netmask']

                    if 'broadcast' in xdict:
                        info['broadcast'] = xdict['broadcast']

                    if 'network' in xdict:
                        info['network'] = xdict['network']

                if info['family'] == 'inet' \
                   and not info.has_key('gateway') \
                   and info.has_key('netmask') \
                   and info.has_key('network') \
                   and xdict.has_key('gateway') \
                   and network.ipv4_in_network(network.parse_ipv4(xdict['gateway']),
                                               network.parse_ipv4(info['netmask']),
                                               network.parse_ipv4(info['network'])):
                    info['gateway'] = xdict['gateway']
        else:
            info['method'] = None

        if info['vlanif']:
            vconfig = network.get_vlan_info(iface)
            info['vlan-id']         = vconfig.get('vlan-id', info['vlan-id'])
            info['vlan-raw-device'] = vconfig.get('vlan-raw-device', info['vlan-raw-device'])

        return info

    def safe_init(self, options):
        """Load parameters, etc"""
        cfg = options.configuration

        tpl_path        = cfg.get('general', 'templates_path')
        custom_tpl_path = cfg.get('general', 'custom_templates_path')
        backup_path     = cfg.get('general', 'backup_path')

        if cfg.has_section('network'):
            for x in self.CONFIG.iterkeys():
                if cfg.has_option('network', x):
                    self.CONFIG[x] = cfg.get('network', x)

        self.CONFIG['lock_timeout'] = float(self.CONFIG['lock_timeout'])

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
            raise HttpReqError(503, "unable to take LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        rs = {}

        try:
            self.inetxparser.reloadfile()
            for iface in netifaces.interfaces():
                info = self.get_netiface_info(iface)
                if info:
                    rs[iface] = info
                    
            return rs
        finally:
            self.LOCK.release()

    def netiface(self, args, options):
        """
        GET /netiface

        >>> netiface({}, {})
        >>> netiface({}, {'ifname': 'eth0'})
        >>> netiface({}, {'ifname': {0: 'eth0', 1: 'eth1'}})
        >>> netiface({}, {'ifname': ['eth0', 'eth1']})
        """
        self.args       = args
        self.options    = options

        if 'ifname' in self.options:
            ifaces = helpers.extract_scalar(self.options['ifname'])
            if not ifaces:
                raise HttpReqError(415, "invalid option 'ifname'")
        else:
            return self.discover_netifaces({}, {})

        if not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        try:
            self.inetxparser.reloadfile()
            if len(ifaces) == 1:
                return self.get_netiface_info(ifaces[0])

            return dict((iface, self.get_netiface_info(iface)) for iface in ifaces)
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
        if 'ifname' in self.options:
            if not isinstance(self.options['ifname'], basestring) \
               or not xivo_config.netif_managed(self.options['ifname']):
                raise HttpReqError(415, "invalid interface name, ifname: %r" % self.options['ifname'])

            try:
                eth = self.get_netiface_info(self.options['ifname'])
            except (OSError, TypeError), e:
                raise HttpReqError(415, "%s: %r", (e, self.options['ifname']))

            if not eth:
                raise HttpReqError(404, "interface not found")
            elif eth.get('type') != 'eth':
                raise HttpReqError(415, "invalid interface type")
            elif eth.get('family') != 'inet':
                raise HttpReqError(415, "invalid address family")
        else:
            raise HttpReqError(415, "missing option 'ifname'")

        return eth

    def normalize_inet_options(self):
        if 'method' in self.args:
            if self.args['method'] == 'static':
                if not xivo_config.plausible_static(self.args, None):
                    raise HttpReqError(415, "invalid static arguments for command")
            elif self.args['method'] == 'dhcp':
                for x in ('address', 'netmask', 'broadcast', 'gateway', 'mtu'):
                    if x in self.args:
                        del self.args[x]
        else:
            raise HttpReqError(415, "missing argument 'method'")

    def get_interface_filecontent(self, conf):
        backupfilepath = None

        if not os.path.isdir(self.CONFIG['interfaces_backup_path']):
            os.makedirs(self.CONFIG['interfaces_backup_path'])

        if os.access(self.CONFIG['interfaces_file'], os.R_OK):
            backupfilepath = "%s.%d" % (self.CONFIG['interfaces_backup_file'], time())
            copy2(self.CONFIG['interfaces_file'], backupfilepath)
            old_lines = file(self.CONFIG['interfaces_file'])
        else:
            old_lines = ()

        if os.access(self.CONFIG['interfaces_custom_tpl_file'], (os.F_OK | os.R_OK)):
            filename = self.CONFIG['interfaces_custom_tpl_file']
        else:
            filename = self.CONFIG['interfaces_tpl_file']

        template_file = open(filename)
        template_lines = template_file.readlines()
        template_file.close()

        filecontent = xivo_config.txtsubst(template_lines,
                                           {'_XIVO_NETWORK_INTERFACES':
                                                ''.join(xivo_config.generate_interfaces(old_lines, conf))},
                                           self.CONFIG['interfaces_file'],
                                           'utf8')

        if old_lines:
            old_lines.close()

        return (filecontent, backupfilepath)


    MODIFY_PHYSICAL_ETH_IPV4_SCHEMA = xys.load("""
    method:     !~~enum(static,dhcp)
    address?:   !~ipv4_address 192.168.0.1
    netmask?:   !~netmask 255.255.255.0
    broadcast?: !~ipv4_address 192.168.0.255
    gateway?:   !~ipv4_address 192.168.0.254
    mtu?:       !~~between(68,1500) 1500
    auto?:      !!bool True
    up?:        !!bool True
    options?:   !~~seqlen(0,64) [ !~~seqlen(2,2) ['dns-search', 'toto.tld tutu.tld'],
                                  !~~seqlen(2,2) ['dns-nameservers', '127.0.0.1 192.168.0.254'] ]
    """)

    def modify_physical_eth_ipv4(self, args, options):
        """
        POST /modify_physical_eth_ipv4

        >>> modify_physical_eth_ipv4({'method': 'dhcp',
                                      'auto':   True},
                                     {'ifname': 'eth0'})
        """
        self.args       = args
        self.options    = options

        import pprint
        pprint.pprint(args)
        pprint.pprint(options)
        if not xys.validate(self.args, self.MODIFY_PHYSICAL_ETH_IPV4_SCHEMA):
            raise HttpReqError(415, "invalid arguments for command")

        eth = self._get_valid_eth_ipv4()

        if not eth['physicalif']:
            raise HttpReqError(415, "invalid interface, it is not a physical interface")

        self.normalize_inet_options()

        if not os.access(self.CONFIG['interfaces_path'], (os.X_OK | os.W_OK)):
            raise HttpReqError(415, "path not found or not writable or not executable: %r" % self.CONFIG['interfaces_path'])
        elif not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        self.args['auto']   = self.args.get('auto', True)
        self.args['family'] = 'inet'

        conf = {'netIfaces':        {},
                'vlans':            {},
                'customipConfs':    {}}

        netifacesbakfile = None

        try:
            if self.CONFIG['netiface_down_cmd']:
                subprocess.call(self.CONFIG['netiface_down_cmd'].strip().split() + [eth['name']])

            for iface in netifaces.interfaces():
                conf['netIfaces'][iface] = 'reserved'

            conf['netIfaces'][eth['name']]      = eth['name']
            conf['vlans'][eth['name']]          = {0: eth['name']}
            conf['customipConfs'][eth['name']]  = self.args

            filecontent, netifacesbakfile = self.get_interface_filecontent(conf)

            try:
                system.file_writelines_flush_sync(self.CONFIG['interfaces_file'], filecontent)

                if self.args.get('up', True) and self.CONFIG['netiface_up_cmd']:
                    subprocess.call(self.CONFIG['netiface_up_cmd'].strip().split() + [eth['name']])
            except Exception, e:
                if netifacesbakfile:
                    copy2(netifacesbakfile, self.CONFIG['interfaces_file'])
                raise e.__class__(str(e))
            return True
        finally:
            self.LOCK.release()


    REPLACE_VIRTUAL_ETH_IPV4_SCHEMA = xys.load("""
    ifname:         !!str vlan42
    method:         !~~enum(static,dhcp)
    vlanid?:        !~~between(0,65535) 42
    vlanrawdevice?: !!str eth0
    address?:       !~ipv4_address 172.16.42.1
    netmask?:       !~netmask 255.255.255.0
    broadcast?:     !~ipv4_address 172.16.42.255
    gateway?:       !~ipv4_address 172.16.42.254
    mtu?:           !~~between(68,1500) 1500
    auto?:          !!bool True
    up?:            !!bool True
    options?:       !~~seqlen(0,64) [ !~~seqlen(2,2) ['dns-search', 'toto.tld tutu.tld'],
                                      !~~seqlen(2,2) ['dns-nameservers', '127.0.0.1 192.168.0.254'] ]
    """)

    def replace_virtual_eth_ipv4(self, args, options):
        """
        POST /replace_virtual_eth_ipv4

        >>> replace_virtual_eth_ipv4({'ifname': 'eth0:0',
                                      'method': 'dhcp',
                                      'auto':   True},
                                     {'ifname': 'eth0:0'})
        """
        self.args       = args
        self.options    = options
        phyifname       = None
        phyinfo         = None

        if 'ifname' not in self.options \
           or not isinstance(self.options['ifname'], basestring) \
           or not xivo_config.netif_managed(self.options['ifname']):
            raise HttpReqError(415, "invalid interface name, ifname: %r" % self.options['ifname'])
        elif not xys.validate(self.args, self.REPLACE_VIRTUAL_ETH_IPV4_SCHEMA):
            raise HttpReqError(415, "invalid arguments for command")

        info = self.get_netiface_info(self.options['ifname'])

        if info and info['physicalif']:
            raise HttpReqError(415, "invalid interface, it is a physical interface")
        elif network.is_alias_if(self.args['ifname']):
            phyifname   = network.phy_name_from_alias_if(self.args['ifname'])
            phyinfo     = self.get_netiface_info(phyifname)
            if not phyinfo or True not in (phyinfo['physicalif'], phyinfo['vlanif']):
                raise HttpReqError(415, "invalid interface, it is not an alias interface")
            elif self.args['method'] != 'static':
                raise HttpReqError(415, "invalid method, must be static")

            if 'vlanrawdevice' in self.args:
                del self.args['vlanrawdevice']
            if 'vlanid' in self.args:
                del self.args['vlanid']
        elif network.is_vlan_if(self.args['ifname']):
            if not 'vlanrawdevice' in self.args:
                raise HttpReqError(415, "invalid arguments for command, missing vlanrawdevice")
            if not 'vlanid' in self.args:
                raise HttpReqError(415, "invalid arguments for command, missing vlanid")

            phyifname   = self.args['vlanrawdevice']
            phyinfo     = self.get_netiface_info(phyifname)
            if not phyinfo or not phyinfo['physicalif']:
                raise HttpReqError(415, "invalid vlanrawdevice, it is not a physical interface")

            vconfig = network.get_vlan_info_from_ifname(self.args['ifname'])

            if 'vlan-id' not in vconfig:
                raise HttpReqError(415, "invalid vlan interface name")
            elif vconfig['vlan-id'] != int(self.args['vlanid']):
                raise HttpReqError(415, "invalid vlanid")
            elif vconfig.get('vlan-raw-device', self.args['vlanrawdevice']) != self.args['vlanrawdevice']:
                raise HttpReqError(415, "invalid vlanrawdevice")

            self.args['vlan-id']            = self.args.pop('vlanid')
            self.args['vlan-raw-device']    = self.args.pop('vlanrawdevice')
        else:
            raise HttpReqError(415, "invalid ifname argument for command")

        if phyinfo.get('type') != 'eth':
            raise HttpReqError(415, "invalid interface type")
        elif phyinfo.get('family') != 'inet':
            raise HttpReqError(415, "invalid address family")

        self.normalize_inet_options()

        if not os.access(self.CONFIG['interfaces_path'], (os.X_OK | os.W_OK)):
            raise HttpReqError(415, "path not found or not writable or not executable: %r" % self.CONFIG['interfaces_path'])
        elif not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        self.args['auto']   = self.args.get('auto', True)
        self.args['family'] = 'inet'

        conf = {'netIfaces':        {},
                'vlans':            {},
                'customipConfs':    {}}

        netifacesbakfile = None

        try:
            if self.CONFIG['netiface_down_cmd']:
                subprocess.call(self.CONFIG['netiface_down_cmd'].strip().split() + [self.options['ifname']])

            for iface in netifaces.interfaces():
                if self.options['ifname'] != iface:
                    conf['netIfaces'][iface] = 'reserved'

            conf['netIfaces'][self.args['ifname']]      = self.args['ifname']
            conf['vlans'][self.args['ifname']]          = {self.args.get('vlan-id', 0): self.args['ifname']}
            conf['customipConfs'][self.args['ifname']]  = self.args

            filecontent, netifacesbakfile = self.get_interface_filecontent(conf)

            try:
                system.file_writelines_flush_sync(self.CONFIG['interfaces_file'], filecontent)

                if self.args.get('up', True) and self.CONFIG['netiface_up_cmd']:
                    subprocess.call(self.CONFIG['netiface_up_cmd'].strip().split() + [self.args['ifname']])
            except Exception, e:
                if netifacesbakfile:
                    copy2(netifacesbakfile, self.CONFIG['interfaces_file'])
                raise e.__class__(str(e))
            return True
        finally:
            self.LOCK.release()


    MODIFY_ETH_IPV4_SCHEMA = xys.load("""
    address:    !~ipv4_address 192.168.0.1
    netmask:    !~netmask 255.255.255.0
    broadcast?: !~ipv4_address 192.168.0.255
    gateway?:   !~ipv4_address 192.168.0.254
    mtu?:       !~~between(68,1500) 1500
    auto?:      !!bool True
    up?:        !!bool True
    options?:   !~~seqlen(0,64) [ !~~seqlen(2,2) ['dns-search', 'toto.tld tutu.tld'],
                                  !~~seqlen(2,2) ['dns-nameservers', '127.0.0.1 192.168.0.254'] ]
    """)

    def modify_eth_ipv4(self, args, options):
        """
        POST /modify_eth_ipv4

        >>> modify_eth_ipv4({'address':     '192.168.0.1',
                             'netmask':     '255.255.255.0',
                             'broadcast':   '192.168.0.255',
                             'gateway':     '192.168.0.254',
                             'mtu':         1500,
                             'auto':        True,
                             'up':          True,
                             'options':     [['dns-search', 'toto.tld tutu.tld'],
                                             ['dns-nameservers', '127.0.0.1 192.168.0.254']]},
                            {'ifname':  'eth0'})
        """
        self.args       = args
        self.options    = options

        eth = self._get_valid_eth_ipv4()

        if not xys.validate(self.args, self.MODIFY_ETH_IPV4_SCHEMA):
            raise HttpReqError(415, "invalid arguments for command")

        if self.args.has_key('up'):
            if self.args['up']:
                eth['flags'] |= dumbnet.INTF_FLAG_UP
            else:
                eth['flags'] &= ~dumbnet.INTF_FLAG_UP
            del self.args['up']

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
        elif not os.access(self.CONFIG['interfaces_path'], (os.X_OK | os.W_OK)):
            raise HttpReqError(415, "path not found or not writable or not executable: %r" % self.CONFIG['interfaces_path'])
        elif not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        conf = {'netIfaces':    {},
                'vlans':        {},
                'ipConfs':      {}}

        ret = False
        netifacesbakfile = None

        try:
            if self.CONFIG['netiface_down_cmd'] \
               and subprocess.call(self.CONFIG['netiface_down_cmd'].strip().split() + [eth['name']]) == 0 \
               and not (eth['flags'] & dumbnet.INTF_FLAG_UP):
                ret = True

            for iface in netifaces.interfaces():
                conf['netIfaces'][iface] = 'reserved'

            eth['ifname']                   = eth['name']
            conf['netIfaces'][eth['name']]  = eth['name']
            conf['vlans'][eth['name']]      = {eth.get('vlan-id', 0): eth['name']}
            conf['ipConfs'][eth['name']]    = eth

            filecontent, netifacesbakfile = self.get_interface_filecontent(conf)

            try:
                system.file_writelines_flush_sync(self.CONFIG['interfaces_file'], filecontent)

                if self.CONFIG['netiface_up_cmd'] \
                   and (eth['flags'] & dumbnet.INTF_FLAG_UP) \
                   and subprocess.call(self.CONFIG['netiface_up_cmd'].strip().split() + [eth['name']]) == 0:
                    ret = True

                if not ret:
                    if eth.has_key('gateway') and not (eth['flags'] & dumbnet.INTF_FLAG_UP):
                        del eth['gateway']
                    self.netcfg.set(eth)
            except Exception, e:
                if netifacesbakfile:
                    copy2(netifacesbakfile, self.CONFIG['interfaces_file'])
                raise e.__class__(str(e))
            return True
        finally:
            self.LOCK.release()


    CHANGE_STATE_ETH_SCHEMA = xys.load("""
    state:  !!bool True
    """)

    def change_state_eth_ipv4(self, args, options):
        """
        POST /change_state_eth_ipv4

        >>> change_state_eth_ipv4({'state': True},
                                  {'ifname':    'eth0'})
        """
        self.args       = args
        self.options    = options

        eth = self._get_valid_eth_ipv4()

        if not xys.validate(self.args, self.CHANGE_STATE_ETH_SCHEMA):
            raise HttpReqError(415, "invalid arguments for command")
        elif not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        conf = {'netIfaces':        {},
                'vlans':            {},
                'customipConfs':    {}}

        ret = False
        netifacesbakfile = None

        try:
            for iface in netifaces.interfaces():
                conf['netIfaces'][iface] = 'reserved'

            if self.args['state']:
                eth['auto'] = True
                eth['flags'] |= dumbnet.INTF_FLAG_UP

                if self.CONFIG['netiface_up_cmd'] \
                   and subprocess.call(self.CONFIG['netiface_up_cmd'].strip().split() + [eth['name']]) == 0:
                    ret = True
            else:
                eth['auto'] = False
                eth['flags'] &= ~dumbnet.INTF_FLAG_UP

                if self.CONFIG['netiface_down_cmd'] \
                   and subprocess.call(self.CONFIG['netiface_down_cmd'].strip().split() + [eth['name']]) == 0:
                    ret = True

            eth['ifname']                       = eth['name']
            conf['netIfaces'][eth['name']]      = eth['name']
            conf['vlans'][eth['name']]          = {eth.get('vlan-id', 0): eth['name']}
            conf['customipConfs'][eth['name']]  = eth

            filecontent, netifacesbakfile = self.get_interface_filecontent(conf)

            try:
                system.file_writelines_flush_sync(self.CONFIG['interfaces_file'], filecontent)

                if not ret:
                    if not self.args['state'] and eth.has_key('gateway'):
                        del eth['gateway']
                    self.netcfg.set(eth)
            except Exception, e:
                if netifacesbakfile:
                    copy2(netifacesbakfile, self.CONFIG['interfaces_file'])
                raise e.__class__(str(e))
            return True
        finally:
            self.LOCK.release()

    def delete_eth_ipv4(self, args, options):
        """
        GET /delete_eth_ipv4

        >>> delete_eth_ipv4({},
                            {'ifname':  'eth0'})
        """
        self.args       = args
        self.options    = options

        eth = None

        try:
            eth = self._get_valid_eth_ipv4()
        except HttpReqError, e:
            if e.code == 404:
                pass

        if not self.LOCK.acquire_read(self.CONFIG['lock_timeout']):
            raise HttpReqError(503, "unable to take LOCK for reading after %s seconds" % self.CONFIG['lock_timeout'])

        conf = {'netIfaces':    {}}

        ret                 = False
        netifacesbakfile    = None
        ifname              = self.options['ifname']

        try:
            for iface in netifaces.interfaces():
                conf['netIfaces'][iface] = 'reserved'

            conf['netIfaces'][ifname] = 'removed'

            if self.CONFIG['netiface_down_cmd'] \
               and subprocess.call(self.CONFIG['netiface_down_cmd'].strip().split() + [ifname]) == 0:
                ret = True

            filecontent, netifacesbakfile = self.get_interface_filecontent(conf)

            try:
                system.file_writelines_flush_sync(self.CONFIG['interfaces_file'], filecontent)

                if ret:
                    return True
                elif not eth:
                    raise HttpReqError(404, "interface not found")

                eth['flags'] &= ~dumbnet.INTF_FLAG_UP
                if eth.has_key('gateway'):
                    del eth['gateway']
                self.netcfg.set(eth)
            except HttpReqError, e:
                raise e.__class__(e.code, e.text)
            except Exception, e:
                if netifacesbakfile:
                    copy2(netifacesbakfile, self.CONFIG['interfaces_file'])
                raise e.__class__(str(e))
            return True
        finally:
            self.LOCK.release()


dnetintf = DNETIntf()

http_json_server.register(dnetintf.discover_netifaces, CMD_R, safe_init=dnetintf.safe_init)
http_json_server.register(dnetintf.netiface, CMD_R)
http_json_server.register(dnetintf.netiface_from_dst_address, CMD_R)
http_json_server.register(dnetintf.netiface_from_src_address, CMD_R)
http_json_server.register(dnetintf.modify_physical_eth_ipv4, CMD_RW)
http_json_server.register(dnetintf.replace_virtual_eth_ipv4, CMD_RW)
http_json_server.register(dnetintf.modify_eth_ipv4, CMD_RW)
http_json_server.register(dnetintf.change_state_eth_ipv4, CMD_RW)
http_json_server.register(dnetintf.delete_eth_ipv4, CMD_R)
