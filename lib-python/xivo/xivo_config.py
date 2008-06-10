"""Common routines and objects for autoprovisioning services in XIVO

Copyright (C) 2007, 2008  Proformatique

"""
# Dependencies/highly recommended? : arping curl

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

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
import re
import sys
import yaml
import shutil
import os.path
import subprocess
from ConfigParser import ConfigParser
from itertools import chain

from xivo import network
from xivo import trace_null
from xivo import ConfigDict
from xivo import interfaces
from xivo import except_tb
from xivo import xys
from xivo.easyslog import *


ProvGeneralConf = {
    'database_uri':             "sqlite:/var/lib/asterisk/astsqlite?timeout_ms=150",
    'excl_del_lock_to_s':       45,
    'http_read_request_to_s':   90,
    'http_request_to_s':        90,
    'listen_ipv4':              "127.0.0.1",
    'listen_port':              8666,
    'connect_ipv4':             "127.0.0.1",
    'connect_port':             8666,
    'tftproot':                 "/tftpboot/",
    'scan_ifaces_prefix':       "eth",
    'arping_cmd':               "sudo /usr/sbin/arping",
    'arping_sleep_us':          150000,
    'log_level':                "notice",
    'curl_cmd':                 "/usr/bin/curl",
    'curl_to_s':                30,
    'telnet_to_s':              30,
    'templates_dir':            "/usr/share/pf-xivo-provisioning/files/",
    'asterisk_ipv4':            "192.168.0.254",
    'ntp_server_ipv4':          "192.168.0.254",
}
Pgc = ProvGeneralConf
AUTHORIZED_PREFIXES = ("eth",)


def LoadConfig(filename):
    """
    Load provisioning configuration
    """
    global ProvGeneralConf
    global AUTHORIZED_PREFIXES
    cp = ConfigParser()
    cp.readfp(open(filename))
    ConfigDict.FillDictFromConfigSection(ProvGeneralConf, cp, "general")
    AUTHORIZED_PREFIXES = tuple([
        p.strip()
        for p in Pgc['scan_ifaces_prefix'].split(',')
        if p.strip()
    ])


def any_prefixes(ifname):
    "ifname starts with one of the (few) prefixes in AUTHORIZED_PREFIXES?"
    return True in [ifname.startswith(x) for x in AUTHORIZED_PREFIXES]


def ipv4_from_macaddr(macaddr, logexceptfunc = None):
    """
    Wrapper for network.ipv4_from_macaddr() that sets
    * ifname_filter to any_prefixes
    * arping_cmd_list to Pgc['arping_cmd'].strip().split()
    * arping_sleep_us to Pgc['arping_sleep_us']
    """
    return network.ipv4_from_macaddr(macaddr, logexceptfunc = logexceptfunc,
                                     ifname_filter = any_prefixes,
                                     arping_cmd_list = Pgc['arping_cmd'].strip().split(),
                                     arping_sleep_us = Pgc['arping_sleep_us'])


def macaddr_from_ipv4(macaddr, logexceptfunc = None):
    """
    Wrapper for network.macaddr_from_ipv4() that sets
    * ifname_filter to any_prefixes
    * arping_cmd_list to Pgc['arping_cmd'].strip().split()
    * arping_sleep_us to Pgc['arping_sleep_us']
    """
    return network.macaddr_from_ipv4(macaddr, logexceptfunc = logexceptfunc,
                                     ifname_filter = any_prefixes,
                                     arping_cmd_list = Pgc['arping_cmd'].strip().split(),
                                     arping_sleep_us = Pgc['arping_sleep_us'])


# States for linesubst()
NORM = object()
ONE = object()
TWO = object()
LIT = object()
TLIT = object()
TERM = object()

def linesubst(line, variables):
    """
    In a string, substitute '{{varname}}' occurrences with the
    value of variables['varname'], '\\' being an escaping char...
    If at first you don't understand this function, draw its finite
    state machine and everything will become crystal clear :)
    """
    # trivial no substitution early detection:
    if '{{' not in line and '\\' not in line:
        return line
    st = NORM
    out = ""
    curvar = ""
    for c in line:
        if st is NORM:
            if c == '{':
                st = ONE
            elif c == '\\':
                st = LIT
            else:
                out += c
        elif st is LIT:
            out += c
            st = NORM
        elif st is ONE:
            if c == '{':
                st = TWO
            elif c == '\\':
                out += '{'
                st = LIT
            else:
                out += '{' + c
                st = NORM
        elif st is TWO:
            if c == '\\':
                st = TLIT
            elif c == '}':
                st = TERM
            else:
                curvar += c
        elif st is TLIT:
            curvar += c
            st = TWO
        elif st is TERM:
            if c == '}':
                if curvar not in variables:
                    syslogf(SYSLOG_WARNING, "Unknown variable '%s' detected, will just be replaced by an empty string" % curvar)
                else:
                    syslogf(SYSLOG_DEBUG, "Substitution of {{%s}} by %r" % (curvar, variables[curvar]))
                    out += variables[curvar]
                curvar = ''
                st = NORM
            elif c == '\\':
                curvar += '}'
                st = TLIT
            else:
                curvar += '}' + c
                st = TWO
    if st is not NORM:
        syslogf(SYSLOG_WARNING, "st is not NORM at end of line: " + line)
        syslogf(SYSLOG_WARNING, "returned substitution: " + out)
    return out


def txtsubst(lines, variables, target_file = None):
    """
    Log that target_file is going to be generated, and calculate its
    content by applying the linesubst() transformation with the given
    variables to each given lines.
    """
    if target_file:
        syslogf("In process of generating file %r" % target_file)
    return [linesubst(line, variables) for line in lines]


def well_formed_provcode(provcode):
    """
    Check whether provcode really is a well formed Xivo provisioning
    code.
    """
    if provcode == '0':
        return True
    for d in provcode:
        if d not in '0123456789':
            return False
    return True


class PhoneVendor:
    """
    Phone vendor base class
    """
    def __init__(self, phone):
        """
        Constructor.

        phone must be a dictionary containing everything needed for
        the one phone provisioning process to take place.  That is the
        following keys:

        'model', 'vendor', 'macaddr', 'actions', 'ipv4' if the value
        for 'actions' is not 'no'
        """
        self.phone = phone
        syslogf("Instantiation of %s" % str(self.phone))
    
    def action_reinit(self):
        """
        This function can be called under some conditions after the
        configuration for this phone has been generated by the
        generate_reinitprov() method.
        """
        if self.phone["actions"] == "no": # possible cause: "distant" provisioning
            syslogf("Skipping REINIT action for phone %s" % self.phone['macaddr'])
            return
        syslogf("Sending REINIT command to phone %s" % self.phone['macaddr'])
        self.do_reinit()
        syslogf(SYSLOG_DEBUG, "Sent REINIT command to phone %s" % self.phone['macaddr'])
    
    def action_reboot(self):
        """
        This function can be called under some conditions after the
        configuration for this phone has been generated by the
        generate_autoprov() method.
        """
        if self.phone["actions"] == "no": # distant provisioning with actions disabled
            syslogf("Skipping REBOOT action for phone %s" % self.phone['macaddr'])
            return
        syslogf("Sending REBOOT command to phone %s" % self.phone['macaddr'])
        self.do_reboot()
        syslogf(SYSLOG_DEBUG, "Sent REBOOT command to phone %s" % self.phone['macaddr'])
    
    def generate_reinitprov(self):
        """
        This function put the configuration for the phone back in
        guest state.
        """
        syslogf("About to GUEST'ify the phone %s" % self.phone['macaddr'])
        self.do_reinitprov()
        syslogf(SYSLOG_DEBUG, "Phone GUEST'ified %s" % self.phone['macaddr'])
    
    def generate_autoprov(self, provinfo):
        """
        This function generate the configuration for the phone with
        provisioning informations provided in the provinfo dictionary,
        which must contain the following keys:

        'name', 'ident', 'number', 'passwd'
        """
        syslogf("About to AUTOPROV the phone %s with infos %s" % (self.phone['macaddr'], str(provinfo)))
        self.do_autoprov(provinfo)
        syslogf(SYSLOG_DEBUG, "Phone AUTOPROV'ed %s" % self.phone['macaddr'])


PhoneClasses = {}


def register_phone_vendor_class(cls):
    """
    Register a new class, derived from PhoneVendor, that implements
    provisioning methods for some phones of a given vendor.
    """
    global PhoneClasses
    key = cls.__name__.lower()
    if key not in PhoneClasses:
        PhoneClasses[key] = cls
    else:
        raise ValueError, "A registration as already occured for %r" % key


def phone_vendor_iter_key_class():
    """
    Iterate over phone classes.
    """
    global PhoneClasses
    return PhoneClasses.iteritems()


def phone_factory(phone):
    """
    Instantiate a PhoneVendor derived class according to the phone
    description.
    """
    global PhoneClasses
    phone_class = PhoneClasses[phone["vendor"]]
    return phone_class(phone)


def default_handler():
    """
    Default exception handler for phone_desc_by_ua()
    """
    except_tb.log_exception(lambda x: syslogf(SYSLOG_ERR, x))


def phone_desc_by_ua(ua, exception_handler = default_handler):
    """
    Return a tuple (vendor_key, model, firmware), or None if no PhoneVendor
    derived class has recognized the user agent string.
    vendor_key, model, and firmware are strings.
    """
    global PhoneClasses
    for phone_class in PhoneClasses.itervalues():
        try:
            r = phone_class.get_vendor_model_fw(ua)
        except:
            r = None
            exception_handler()
            sys.exc_clear()
        if r:
            return r
    return None



### GENERAL CONF


def specific(docstr):
    return docstr not in ('reserved', 'none', 'void')


def network_from_static(static):
    """
    Return the network (4uple of ints) specified in static
    """
    return network.mask_ipv4(network.parse_ipv4(static['netmask']), network.parse_ipv4(static['address']))


def broadcast_from_static(static):
    """
    Return the broadcast address (4uple of ints) specified in static
    """
    if 'broadcast' in static:
        return network.parse_ipv4(static['broadcast'])
    else:
        return network.or_ipv4(network.netmask_invert(network.parse_ipv4(static['netmask'])), network_from_static(static))


def netmask_from_static(static):
    return network.parse_ipv4(static['netmask'])


def ip_in_network(ipv4, net, netmask):
    """
    Return a tuple (innet, other_net) where innet is a boolean that is True
    iff ipv4/netmask is the same as net and other_net is ipv4/netmask.
    """
    other_net = network.mask_ipv4(netmask, ipv4)
    return (net == other_net), other_net


def search_domain(docstr, schema, trace):
    """
    !~search_domain
        Return True if the string in docstr is suitable for use in the
        search line of /etc/resolv.conf, else False
    """
    return network.plausible_search_domain(docstr)


def ipv4_address(docstr, schema, trace):
    """
    !~ipv4_address
        Check that corresponding document strings are IPv4 addresses
    """
    return network.is_ipv4_address_valid(docstr)


def reserved_none_void_prefixDec(fname, prefix):
    """
    Return a XYS validator that checks that corresponding document strings
    are 'reserved', 'none', 'void', or valid per !~~prefixDec prefix.
    """
    def validator(docstr, schema, trace):
        """
        !~<validator generated by reserved_none_void_prefixDec() >
            Checks that corresponding document strings are 'reserved',
            'none', 'void', or valid per !~~prefixDec prefix.
        """
        if docstr in ('reserved', 'none', 'void'):
            return True
        if not docstr.startswith(prefix):
            return False
        try:
            int(docstr[len(prefix):])
        except ValueError:
            return False
        return True
    validator.__name__ = fname
    return validator


def plausible_static(static, schema, trace):
    """
    !~plausible_static (from !!map)
        Check that the netmask is plausible, that every address is in the
        same network, and that there are no duplicated addresses.
    """
    address = network.parse_ipv4(static['address'])
    netmask = network.parse_ipv4(static['netmask'])
    if not network.plausible_netmask(netmask):
        return False
    addr_list = [address]
    net = network.mask_ipv4(netmask, address)
    for other in ('broadcast', 'gateway'):
        other_ip = static.get(other)
        if other_ip:
            parsed_ip = network.parse_ipv4(other_ip)
            addr_list.append(parsed_ip)
            if network.mask_ipv4(netmask, parsed_ip) != net:
                return False
    if 'broadcast' not in static:
        addr_list.append(broadcast_from_static(static))
    if len(addr_list) != len(set(addr_list)):
        return False
    return True


def get_referenced_ipConfTags(conf):
    """
    Get tags of the static IP configurations that are owned by vlans (in
    our relational model vlans include untaggued vlan and physical
    interfaces are never directly related to IP configurations).
    
    Return a list
    """
    return filter(specific, chain(*[elt.itervalues() for elt in conf['vlans'].itervalues()]))


def get_referenced_vsTags(conf):
    """
    Get tags of the VLan sets that are owned by a physical interface.
    
    Return a list
    """
    return filter(specific, conf['netIfaces'].itervalues())


def references_relation(set_defined_symbols, lst_references, minref, maxref):
    """
    Pure function.
    
    For each element of set_defined_symbols, there must be between minref
    and maxref, included, identical elements in references.
    
    This function returns (dict_ok, dict_out_of_bounds, dict_undefined)
    where dictionaries contain entries of symbol: count.
    
    Any symbol of set_defined_symbols appears either in dict_ok or in
    dict_out_of_bounds, even if it is unreferenced: in this case count == 0
    """
    dict_ok = {}
    dict_out_of_bounds = {}
    dict_undefined = {}
    
    dict_count = {}
    
    for symbol in lst_references:
        if symbol in set_defined_symbols:
            dict_count[symbol] = dict_count.get(symbol, 0) + 1
        else:
            dict_undefined[symbol] = dict_undefined.get(symbol, 0) + 1
    
    for symbol in set_defined_symbols:
        count = dict_count.get(symbol, 0)
        if minref <= count <= maxref:
            dict_ok[symbol] = count
        else:
            dict_out_of_bounds[symbol] = count
    
    return dict_ok, dict_out_of_bounds, dict_undefined


def plausible_configuration(conf, schema, trace):
    """
    !~plausible_configuration
        Validate the general system configuration
    """
    
    dict_ok, dict_out_of_bounds, dict_undefined = references_relation(conf['ipConfs'], get_referenced_ipConfTags(conf), minref=0, maxref=1)
    if dict_out_of_bounds:
        trace.err("duplicated static IP conf references in vlans description: %r" % dict_out_of_bounds)
        return False
    if dict_undefined:
        trace.err("undefined referenced static IP configurations: %r" % dict_undefined)
        return False
    
    referenced_vsTags = get_referenced_vsTags(conf)
    dict_ok, dict_out_of_bounds, dict_undefined = references_relation(conf['vlans'], referenced_vsTags, minref=0, maxref=1)
    if dict_out_of_bounds:
        trace.err("duplicated vlan references in network interfaces description: %r" % dict_out_of_bounds)
        return False
    if dict_undefined:
        trace.err("undefined vlan configurations: %r" % dict_undefined)
        return False
    
    # TODO: uniqueness concept in schema, default types in schema
    nameservers = conf['resolvConf'].get('nameservers')
    if nameservers:
        nameservers = map(network.normalize_ipv4_address, nameservers)
        unique_nameservers = frozenset(nameservers)
        if len(unique_nameservers) != len(nameservers):
            trace.err("duplicated nameservers in " + `tuple(nameservers)`)
            return False
    
    # Check that active networks are distinct
    active_networks = {}
    duplicated_networks = False
    for vlanset_name in referenced_vsTags:
        for static_name in conf['vlans'][vlanset_name].itervalues():
            if not specific(static_name):
                continue
            net = network_from_static(conf['ipConfs'][static_name])
            if net in active_networks:
                duplicated_networks = True
                active_networks[net].append(static_name)
            else:
                active_networks[net] = [static_name]
    if duplicated_networks:
        non_duplicated_networks = [net for net, names in active_networks.iteritems() if len(names) <= 1]
        for net in non_duplicated_networks:
            del active_networks[net]
        trace.err("duplicated active networks: %r" % dict((('.'.join(map(str, net)), tuple(names)) for net, names in active_networks.iteritems())))
        return False
    
    # VOIP service
    ipConfVoip = conf['services']['voip']['ipConf']
    if ipConfVoip not in conf['ipConfs']:
        trace.err("the voip service references a static ip configuration that does not exists: %r" % ipConfVoip)
        return False
    ipConfVoip_static = conf['ipConfs'][ipConfVoip]
    netmask = netmask_from_static(ipConfVoip_static)
    net = network_from_static(ipConfVoip_static)
    broadcast = broadcast_from_static(ipConfVoip_static)
    addresses = conf['services']['voip']['addresses']
    voip_fixed = ('voipServer', 'bootServer', 'directory', 'ntp', 'router')
    for field in voip_fixed:
        if field in addresses:
            if network.parse_ipv4(addresses[field]) == broadcast: # TODO: other sanity checks...
                trace.err("invalid voip service related IP %r: %r" % (field, addresses[field]))
                return False
    # router, if present, must be in the network
    if 'router' in addresses:
        ok, other = ip_in_network(network.parse_ipv4(addresses['router']), net, netmask)
        if not ok:
            trace.err("router must be in network %s/%s but seems to be in %s/%s" %
                      (network.unparse_ipv4(net), network.unparse_ipv4(netmask), network.unparse_ipv4(other), network.unparse_ipv4(netmask)))
            return False
    # check that any range is in the network and with min <= max
    for range_field in 'voipRange', 'alienRange':
        if range_field not in addresses:
            continue
        ip_range = map(network.parse_ipv4, addresses[range_field])
        for ip in ip_range:
            ok, other = ip_in_network(ip, net, netmask)
            if not ok:
                trace.err("IP %s is not in network %s/%s" % (network.unparse_ipv4(ip), network.unparse_ipv4(net), network.unparse_ipv4(netmask)))
                return False
        if not (ip_range[0] <= ip_range[1]):
            trace.err("Invalid IP range: " + `tuple(addresses[range_field])`)
            return False
    # check that there is no overlapping ranges
    parsed_voipRange = map(network.parse_ipv4, addresses['voipRange'])
    all_ranges = [ parsed_voipRange ]
    if 'alienRange' in addresses:
        one = parsed_voipRange
        two = map(network.parse_ipv4, addresses['alienRange'])
        all_ranges.append(two)
        if (one[0] <= two[0] <= one[1]) or (one[0] <= two[1] <= one[1]):
            trace.err("overlapping DHCP ranges detected")
            return False
    # check that there is no fixed IP in any DHCP range
    fixed_addresses = [ network.parse_ipv4(ipConfVoip_static[field]) for field in ('address', 'gateway') if field in ipConfVoip_static ]
    fixed_addresses.append(broadcast_from_static(ipConfVoip_static))
    fixed_addresses.extend([ network.parse_ipv4(addresses[field]) for field in voip_fixed if field in addresses ])
    for rang in all_ranges:
        for addr in fixed_addresses:
            if rang[0] <= addr <= rang[1]:
                trace.err("fixed address %r detected in DHCP range %r" % (network.unparse_ipv4(addr), tuple(rang)))
                return False
    
    return True


xys.add_validator(search_domain, u'!!str')
xys.add_validator(ipv4_address, u'!!str')
xys.add_validator(plausible_static, u'!!map')
xys.add_validator(plausible_configuration, u'!!map')
xys.add_validator(reserved_none_void_prefixDec('vlanIpConf', 'static_'), u'!!str')
xys.add_validator(reserved_none_void_prefixDec('netIfaceVlans', 'vs_'), u'!!str')


SCHEMA_NETWORK_CONFIG = xys.load("""!~plausible_configuration
resolvConf:
    search?: !~search_domain bla.tld
    nameservers?: !~~seqlen(1,3) [ !~ipv4_address 192.168.0.200 ]
ipConfs:
    !~~prefixedDec static_: !~plausible_static
        address:     !~ipv4_address 192.168.0.100
        netmask:     !~ipv4_address 255.255.255.0
        broadcast?:  !~ipv4_address 192.168.0.255
        gateway?:    !~ipv4_address 192.168.0.254
        mtu?:        !~~between(68,1500) 1500
vlans:
    !~~prefixedDec vs_:
        !~~between(0,4094) 0: !~vlanIpConf static_0001
netIfaces:
    !~~prefixedDec eth: !~netIfaceVlans vs_0001
services:
    voip:
        ipConf: !~~prefixedDec static_
        addresses:
            voipServer: !~ipv4_address 192.168.1.200
            bootServer: !~ipv4_address 192.168.1.200
            voipRange: !~~seqlen(2,2) [ !~ipv4_address 192.168.1.200 ]
            alienRange?: !~~seqlen(2,2) [ !~ipv4_address 192.168.1.200 ]
            directory?: !~ipv4_address 192.168.1.200
            ntp?: !~ipv4_address 192.168.1.200
            router?: !~ipv4_address 192.168.1.254
""")


def reserved_netIfaces(conf):
    """
    Return the set of reserved physical network interfaces
    """
    return frozenset([ifname for ifname, ifacevlan in conf['netIfaces'].iteritems() if ifacevlan == 'reserved'])


def reserved_vlans(conf):
    """
    Return the set of reserved vlan interfaces
    """
    reserved_vlan_list = []
    for phy, vsTag in conf['netIfaces'].iteritems():
        if not specific(vsTag):
            continue
        reserved_vlan_list.extend(["%s.%d" % (phy, vlanId)
                                   for vlanId, ipConfs_tag in conf['vlans'][vsTag].iteritems()
                                   if ipConfs_tag == 'reserved'])
    return frozenset(reserved_vlan_list)


def normalize_static(static):
    """
    Normalize IPv4 addresses in static
    """
    # TODO: check before normalization, or better schema for not only formatting but also typing
    for key in ('address', 'netmask', 'broadcast', 'gateway'):
        if key in static:
            static[key] = network.normalize_ipv4_address(static[key])


class InvalidConfigurationError(Exception):
    "Error raised when a configuration is detected as semantically invalid."
    def __init__(self, msg):
        self.__reprmsg = "<%s %r>" % (self.__class__.__name__, msg)
        self.__strmsg = str(msg)
        Exception.__init__(self, msg)
    def __repr__(self):
        return self.__reprmsg
    def __str__(self):
        return self.__strmsg


def load_configuration(conf_source, trace=trace_null):
    """
    Parse the first YAML document in a stream and produce the corresponding
    normalized internal representation of the configuration.
    
    Raise a xivo_config.InvalidConfigurationError if the configuration is
    invalid.
    """
    conf = yaml.load(conf_source)
    if not xys.validate(conf, SCHEMA_NETWORK_CONFIG, trace):
        raise InvalidConfigurationError("Invalid configuration")
    # TODO: do that thanks to schema based mapping ("mapping" in functional programming meaning)
    nameservers = conf['resolvConf'].get('nameservers')
    if nameservers:
        conf['resolvConf']['nameservers'] = map(network.normalize_ipv4_address, nameservers)
    for static in conf['ipConfs'].itervalues():
        normalize_static(static)
    voip_addresses = conf['services']['voip']['addresses']
    for field in 'voipServer', 'bootServer', 'directory', 'ntp', 'router':
        if field in voip_addresses:
            voip_addresses[field] = network.normalize_ipv4_address(voip_addresses[field])
    for range_name in 'voipRange', 'alienRange':
        if range_name in voip_addresses:
            voip_addresses[range_name][:] = map(network.normalize_ipv4_address, voip_addresses[range_name])
    return conf


def natural_vlan_name(phy, vlanId):
    """
    * phy: string
    * vlanId: integer
    """
    if not vlanId:
        return phy
    else:
        return "%s.%d" % (phy, vlanId)


def generate_interfaces(old_interfaces_lines, conf, trace=trace_null):
    """
    Yield the new lines of interfaces(5) according to the old ones and the
    current configuration
    """
    trace.notice("ENTERING generate_interfaces()")
    
    eni = interfaces.parse(old_interfaces_lines)
    
    rsvd_base = reserved_netIfaces(conf)
    rsvd_full = reserved_vlans(conf)
    rsvd_mapping_dest = interfaces.get_mapping_dests(eni, rsvd_base, rsvd_full)
    
    def unhandled_or_reserved(ifname):
        """
        Is ifname not handled either because it does not start with a
        known prefix, or because it is explicitely reserved
        """
        return (not any_prefixes(ifname)) or \
               interfaces.ifname_in_base_full_mapsymbs(ifname, rsvd_base, rsvd_full, rsvd_mapping_dest)
    
    KEPT = object()
    REMOVED = object()
    SPACE = object()
    
    new_eni = []
    space_blocks = []
    up = REMOVED
    down = REMOVED
    
    def flush_space_blocks():
        """
        Some space blocks are only preserved if they are not
        surrounded by other non preserved blocks.
        This function flush or dismiss the space blocks according to
        this rule, and must be called as soon as the KEPT / REMOVED of
        the lower non space block is known.
        """
        if down is KEPT:
            new_eni.extend(space_blocks)
        elif up is KEPT:
            if space_blocks:
                new_eni.append(space_blocks[0])
        else: # dismiss space blocks for which adjacent non space blocks are both removed
            pass
        del space_blocks[:]
    
    # The following transformation is performed on 'eni' and the actions are traced
    #
    # EniBlockSpace         keep according to wizard rules of attraction of other kept blocks
    # EniBlockWithIfName    keep iff unhandled/reserved
    #   EniBlockMapping
    #   EniBlockIface
    # EniBlockAllow         suppress handled and non reserved inside, keep only if result is non empty
    # EniBlockUnknown       suppress
    #
    for block in eni:
        if isinstance(block, interfaces.EniBlockSpace):
            space_blocks.append(block)
            down = SPACE
        elif isinstance(block, interfaces.EniBlockWithIfName):
            if unhandled_or_reserved(block.ifname):
                trace.debug("keeping unhandled or reserved %s block %r" % (block.__class__.__name__, block.ifname))
                down = KEPT
            else:
                trace.info("removing handled and not reserved %s block %r" % (block.__class__.__name__, block.ifname))
                down = REMOVED
        elif isinstance(block, interfaces.EniBlockAllow):
            assert len(block.cooked_lines) == 1, "a EniBlockAllow contains more than one cooked line"
            line_recipe = interfaces.EniCookLineRecipe(block.raw_lines)
            for ifname in block.allow_list[:]:
                if unhandled_or_reserved(ifname):
                    trace.debug("keeping unhandled or reserved %r in %r stanza" % (ifname, block.allow_kw))
                else:
                    trace.info("removing handled and not reserved %r in %r stanza" % (ifname, block.allow_kw))
                    mo = re.search(re.escape(ifname) + r'(\s)*', line_recipe.cooked_line)
                    if mo:
                        line_recipe.remove_part(mo.start(), mo.end())
                    else:
                        trace.warning("%r has not been found in %r" % (ifname, line_recipe.cooked_line))
                    block.allow_list.remove(ifname)
            line_recipe.update_block(block)
            if block.allow_list:
                down = KEPT
            else:
                trace.info("removing empty %r stanza" % block.allow_kw)
                down = REMOVED
        else: # interfaces.EniBlockUnknown
            trace.info("removing invalid block")
            down = REMOVED
        
        if down is not SPACE:
            flush_space_blocks()
            if down is KEPT:
                new_eni.append(block)
            up = down
    
    if down is SPACE:
        down = REMOVED
        flush_space_blocks()
    
    eni = new_eni
    
    # yield initial comments
    #
    yield "# XIVO: FILE AUTOMATICALLY GENERATED BY THE XIVO CONFIGURATION SUBSYSTEM\n"
    yield "# XIVO: ONLY RESERVED STANZAS WILL BE PRESERVED WHEN IT IS REGENERATED\n"
    yield "# XIVO: \n"
    
    # yield remaining lines
    #
    for block in eni:
        for raw_line in block.raw_lines:
            if not raw_line.startswith("# XIVO: "):
                yield raw_line
    
    # generate new config for handled interfaces
    #
    for phy, vsTag in conf['netIfaces'].iteritems():
        if not specific(vsTag):
            continue
        for vlanId, ipConfs_tag in conf['vlans'][vsTag].iteritems():
            if not specific(ipConfs_tag):
                continue
            ifname = natural_vlan_name(phy, vlanId)
            trace.info("generating configuration for %r" % ifname)
            static = conf['ipConfs'][ipConfs_tag]
            yield "iface %s inet static\n" % ifname
            yield "\taddress %s\n" % static['address']
            yield "\tnetmask %s\n" % static['netmask']
            for optional in ('broadcast', 'gateway'):
                if optional in static:
                    yield "\t%s %s\n" % (optional, static[optional])
            if 'mtu' in static:
                yield "\tmtu %d\n" % static['mtu']
            yield "\n"
    
    trace.notice("LEAVING generate_interfaces.")


# XXX traces
def generate_dhcpd_conf(conf, trace=trace_null):
    """
    Yield each line of the generated dhcpd.conf
    """
    trace.notice("ENTERING generate_dhcpd_conf()")
    
    addresses = conf['services']['voip']['addresses']
    ipConfVoip_key = conf['services']['voip']['ipConf']
    ipConfVoip = conf['ipConfs'][ipConfVoip_key]
    
    yield '# XIVO: FILE AUTOMATICALLY GENERATED BY THE XIVO CONFIGURATION SUBSYSTEM\n'
    yield '# XIVO: DO NOT EDIT\n'
    yield '\n'
    yield 'class "phone-mac-address-prefix" {\n'
    yield '    match substring(hardware, 0, 4);\n'
    yield '}\n'
    yield '\n'
    for phone_class in PhoneClasses.itervalues():
        yield '# %s\n' % phone_class.__name__
        for line in phone_class.get_dhcp_classes_and_sub(addresses):
            yield line
    yield '\n'
    yield 'subnet %s netmask %s {\n' % (network.unparse_ipv4(network_from_static(ipConfVoip)), ipConfVoip['netmask'])
    yield '\n'
    yield '    one-lease-per-client on;\n'
    yield '\n'
    yield '    option subnet-mask %s;\n' % ipConfVoip['netmask']
    yield '    option broadcast-address %s;\n' % network.unparse_ipv4(broadcast_from_static(ipConfVoip))
    yield '    option ip-forwarding off;\n'
    if 'router' in addresses:
        yield '    option routers %s;\n' % addresses['router']
    yield '\n'
    yield '    log(binary-to-ascii(16,8,":",hardware));\n'
    yield '    log(option user-class);\n'
    yield '    log(option vendor-class-identifier);\n'
    yield '\n'
    yield '    pool {\n'
    yield '        range dynamic-bootp %s %s;\n' % tuple(addresses['voipRange'])
    yield '        default-lease-time 14400;\n'
    yield '        max-lease-time 28800;\n'
    yield '\n'
    yield '        log("VoIP pool");\n'
    yield '\n'
    yield '        allow members of "phone-mac-address-prefix";\n'
    for phone_class in PhoneClasses.itervalues():
        first_line = True
        for line in phone_class.get_dhcp_pool_lines():
            if first_line:
                yield '        # %s\n' % phone_class.__name__
                first_line = False
            yield line
    yield '    }\n'
    if 'alienRange' in addresses:
        yield '\n'
        yield '    pool {\n'
        yield '        range dynamic-bootp %s %s;\n' % tuple(addresses['alienRange'])
        yield '        default-lease-time 7200;\n'
        yield '        max-lease-time 14400;\n'
        yield '\n'
        yield '        log("non VoIP pool");\n'
        yield '    }\n'
    yield '}\n'
    
    trace.notice("LEAVING generate_dhcpd_conf.")


# SYSCONF_DIR = "/etc/pf-xivo/sysconf"
SYSCONF_DIR = "/home/xilun/xivo/people/xilun"

STORE_BASE = os.path.join(SYSCONF_DIR, "store")

STORE_DEFAULT = "default"
STORE_CURRENT = "current"
STORE_PREVIOUS = "previous"
STORE_TMP = "tmp"
STORE_NEW = "new"
STORE_FAILED = "failed"
STORE_RESERVED = (STORE_DEFAULT, STORE_CURRENT, STORE_PREVIOUS, STORE_TMP, STORE_NEW, STORE_FAILED)

GENERATED_BASE = os.path.join(SYSCONF_DIR, "generated")

GENERATED_CURRENT = "current"
GENERATED_PREVIOUS = "previous"
GENERATED_TMP = "tmp"
GENERATED_NEW = "new"

NETWORK_CONFIG_FILE = "network.yaml"

INTERFACES_FILE = "interfaces"
DHCPD_CONF_FILE = "dhcpd.conf"


def sync():
    """
    Call /bin/sync
    """
    # is there a wrapper for the sync syscall in Python?
    subprocess.call("/bin/sync", close_fds = True)


def transactional_generation(store_base, store_subs, gen_base, gen_subs, generation_func, trace=trace_null):
    """
    This function completes a three staged transaction if it has been
    started, or does nothing otherwise.  The purpose of the transaction is
    to generate some configuration files from others, where the source
    files are all stored along in a common subdirectory, and the
    destination files are in an other subdirectory, while guarantying as
    much as possible that in the stable state, the source and the
    destination directories are in sync.  The transaction must be
    externally initiated by the creation of the directory 'store_new'.
    
    This function returns True if a transaction has been processed
    successfully, False if it has been cancelled, None if no transaction
    was in progress.  A transaction fails and is cancelled iff
    generation_func() raises an exception.  Uncatched exceptions raised by
    transactional_generation() must be considered as fatal errors requiring
    human intervention or at least restoration of a known stable state.
    
    NOTE: Because of our requirements some state that is only stored in the
    generated files must be preserved (configuration of reserved interfaces
    and of unhandled interfaces).  That is why generation_func takes its
    second parameter.
    """
    success = None
    trace.notice("ENTERING transactional_generation()")
    trace.debug("store_base = %r" % store_base)
    trace.debug("store_subs = %r" % (store_subs,))
    trace.debug("gen_base = %r" % gen_base)
    trace.debug("gen_subs = %r" % (gen_subs,))
    store_previous, store_current, store_new, store_failed = [os.path.join(store_base, x) for x in store_subs]
    gen_previous, gen_current, gen_new, gen_tmp = [os.path.join(gen_base, x) for x in gen_subs]
    if os.path.isdir(store_new) and not os.path.isdir(gen_new):
        success = True
        trace.notice("BEGIN PHASE 1")
        trace.debug("about to recursively remove %r" % gen_tmp)
        shutil.rmtree(gen_tmp, ignore_errors = True)
        if os.path.isdir(gen_current):
            trace.info("%r exists" % gen_current)
            previously_generated = gen_current
        else:
            trace.info("%r does not exist" % gen_current)
            previously_generated = None
        trace.debug("about to call %r" % generation_func)
        trace.debug("  gen_tmp = %r" % gen_tmp)
        trace.debug("  previously_generated = %r" % previously_generated)
        trace.debug("  store_new = %r" % store_new)
        try:
            generation_func(gen_tmp, previously_generated, store_new, trace)
        except:
            except_tb.log_exception(trace.err)
            success = False
        sync()
        if success:
            trace.info("Successful generation")
            trace.debug("about to rename %r to %r" % (gen_tmp, gen_new))
            os.rename(gen_tmp, gen_new)
            sync()
            trace.notice("END PHASE 1 - generation performed")
        else:
            trace.info("Error during generation - cancelling transaction")
            shutil.rmtree(store_failed, ignore_errors = True)
            try:
                os.rename(store_new, store_failed)
                trace.notice("%r renamed to %r" % (store_new, store_failed))
            except:
                trace.warning("transactional_generation: failed to rename %r to %r - destroying %r"
                              % (store_new, store_failed, store_new))
                except_tb.log_exception(trace.warning)
                shutil.rmtree(store_new, ignore_errors = True)
            sync()
            trace.notice("about to remove incomplete generated directory %r" % gen_tmp)
            shutil.rmtree(gen_tmp, ignore_errors = True)
            sync()
            trace.notice("END PHASE 1 - transaction cancelled")
    if os.path.isdir(store_new) and os.path.isdir(gen_new):
        success = True
        trace.notice("BEGIN PHASE 2")
        if os.path.isdir(store_current):
            trace.info("%r to %r" % (store_current, store_previous))
            trace.debug("about to recursively remove %r" % store_previous)
            shutil.rmtree(store_previous, ignore_errors = True)
            trace.debug("about to rename %r to %r" % (store_current, store_previous))
            try:
                os.rename(store_current, store_previous)
            except:
                trace.warning("transactional_generation: failed to rename %r to %r" % (store_current, store_previous))
                except_tb.log_exception(trace.warning)
                shutil.rmtree(store_current)
            sync()
        trace.debug("about to rename %r to %r" % (store_new, store_current))
        os.rename(store_new, store_current)
        sync()
        trace.notice("END PHASE 2")
    if (not os.path.isdir(store_new)) and os.path.isdir(gen_new):
        success = True
        trace.notice("BEGIN PHASE 3")
        if os.path.isdir(gen_current):
            trace.info("%r to %r" % (gen_current, gen_previous))
            trace.debug("about to recursively remove %r" % gen_previous)
            shutil.rmtree(gen_previous, ignore_errors = True)
            trace.debug("about to rename %r to %r" % (gen_current, gen_previous))
            try:
                os.rename(gen_current, gen_previous)
            except:
                trace.warning("transactional_generation: failed to rename %r to %r" % (gen_current, gen_previous))
                except_tb.log_exception(trace.warning)
                shutil.rmtree(gen_current)
            sync()
        trace.debug("about to rename %r to %r" % (gen_new, gen_current))
        os.rename(gen_new, gen_current)
        sync()
        trace.notice("END PHASE 3")
    trace.notice("LEAVING transactional_generation.")
    return success


def file_writelines_flush_sync(path, lines):
    """
    Fill file at @path with @lines then flush all buffers
    (Python and system buffers)
    """
    fp = file(path, "w")
    fp.writelines(lines)
    fp.flush()
    os.fsync(fp.fileno())
    fp.close()


def generate_system_configuration(to_gen, prev_gen, current_xivo_conf, trace=trace_null):
    """
    Generate system configuration from our own configuration model.
    """
    os.mkdir(to_gen)

    config = load_configuration(file(os.path.join(current_xivo_conf, NETWORK_CONFIG_FILE)), trace)

    if prev_gen:
        old_interfaces_lines = file(os.path.join(prev_gen, INTERFACES_FILE))
    else:
        old_interfaces_lines = ()
    file_writelines_flush_sync(os.path.join(to_gen, INTERFACES_FILE),
                               generate_interfaces(old_interfaces_lines, config, trace))
    if old_interfaces_lines:
        old_interfaces_lines.close()
        old_interfaces_lines = None

    file_writelines_flush_sync(os.path.join(to_gen, DHCPD_CONF_FILE),
                               generate_dhcpd_conf(config, trace))


def transaction_system_configuration(trace=trace_null):
    """
    Transactionally generate system configuration from our own
    configuration model.
    """
    transactional_generation(STORE_BASE, (STORE_PREVIOUS, STORE_CURRENT, STORE_NEW, STORE_FAILED),
                             GENERATED_BASE, (GENERATED_PREVIOUS, GENERATED_CURRENT, GENERATED_NEW, GENERATED_TMP),
                             generate_system_configuration, trace)


def gen_plugged_by_phy(phys):
    """
    Construct a cache of carrier status of interfaces in the sequence phys.
    The cache is a mapping where keys are interfaces and values a boolean
    representing the carrier status (False => disconnected,
    True => connected)
    """
    return dict(((phy, network.is_interface_plugged(phy)) for phy in phys))


def cmp_bool_lexdec(x, y):
    """
    Let X = (x[0], x[1]) and Y = (y[0], y[1]) where
      x[0] and y[0] are boolean and
      x[1] and y[1] are lexico-decimal strings
        where a lexico-decimal string is a string representing a
        lexico-decimal tuple by concatenating alternating strings and
        decimal represenations of integers in the same order as they appear
        in the tuple.  Lexico-decimal strings are totally ordered by the
        network.cmp_lexdec function.
    
    This function defines a total order on the set of
    boolean x lexico-decimal strings, which X and Y belong to.
    """
    return cmp((x[0], network.split_lexdec(x[1])), (y[0], network.split_lexdec(y[1])))


def aaLst_npst_phy(conf, plugged_by_phy):
    """
    Return a list of (npst, phy) in cmp_bool_lexdec order.
    * npst: not plugged status
    * phy: physical interfaces name
    
    Only phys that are in the "void" will be enumerated.
    """
    # TODO: detect the default path of get() and: trace it?
    def phy_handled_relate_void(phy):
        """
        Is phy both handled (its name prefix is known) and in the "void"
        """
        return any_prefixes(phy) and conf['netIfaces'].get(phy, 'void') == 'void'
    
    phys = network.get_filtered_phys(phy_handled_relate_void)
    return sorted(((not plugged_by_phy.get(phy, False), phy) for phy in phys), cmp = cmp_bool_lexdec)


def aaLst_npst_fifn_vsTag_vlanId(conf, plugged_by_phy):
    """
    Return a list of (npst, fifn, vsTag, vlanId) in cmp_bool_lexdec order
    for (npst, fifn) where:
    * npst: True if the supporting interface is not connected (boolean)
    * fifn: "full" interface name - that is the linux vlan interface name
      or ethXX.0 for untagged vlans (string)
    * vsTag: vlan set tag (string)
    * vlanId: vlan Id (integer)

    Only interfaces that are in the "void" will be enumerated.
    """
    res = []
    for phy, vsTag in conf['netIfaces'].iteritems():
        if specific(vsTag):
            for vlanId, ipConfs_tag in conf['vlans'][vsTag].iteritems():
                if ipConfs_tag == 'void':
                    fifn = "%s.%d" % (phy, vlanId)
                    res.append((not plugged_by_phy.get(phy, False), fifn, vsTag, vlanId))
    res.sort(cmp = cmp_bool_lexdec)
    return res


def aaLst_vsTag(conf):
    """
    Return a list of vlan set names that are not owned by a physical
    interface, in network.cmp_lexdec order.
    """
    owned = frozenset(get_referenced_vsTags(conf))
    return network.sorted_lst_lexdec((vsTag for vsTag in conf['vlans'].iterkeys() if vsTag not in owned))


def aaLst_ipConfTag(conf):
    """
    Return a list of ipconf tags that are not owned by a vlan (in our
    terminology vlans include untaggued vlan) in network.cmp_lexdec order.
    """
    owned = frozenset(get_referenced_ipConfTags(conf))
    return network.sorted_lst_lexdec((ipConfTag for ipConfTag in conf['ipConfs'].iterkeys() if ipConfTag not in owned))


def autoattrib_conf(conf):
    """
    Auto attribute orphan vlan set to 'void' physical interfaces, in
    priority to plugged interfaces then to unplugged interfaces.
    Once done auto attribute vlan interfaces (including untagged vlans)
    to orphan ip configurations.
    """
    plugged_by_phy = gen_plugged_by_phy(network.get_filtered_phys(any_prefixes))
    
    vsTag_iter = iter(aaLst_vsTag(conf))
    for npst, phy in aaLst_npst_phy(conf, plugged_by_phy):
        try:
            vsTag = vsTag_iter.next()
        except StopIteration:
            break
        conf['netIfaces'][phy] = vsTag
    
    ipConfTag_iter = iter(aaLst_ipConfTag(conf))
    for npst, fifn, vsTag, vlanId in aaLst_npst_fifn_vsTag_vlanId(conf, plugged_by_phy):
        try:
            ipConfTag = ipConfTag_iter.next()
        except StopIteration:
            break
        conf['vlans'][vsTag][vlanId] = ipConfTag
    
    return conf


def autoattrib(trace=trace_null):
    """
    Auto VLAN Set and IP Configuration attributions at server startup
    """
    config = load_configuration(file(os.path.join(STORE_BASE, STORE_CURRENT, NETWORK_CONFIG_FILE)), trace)
    return autoattrib_conf(config)


__all__ = (
    'ProvGeneralConf', 'LoadConfig', 'txtsubst', 'well_formed_provcode',
    'ipv4_from_macaddr', 'macaddr_from_ipv4',
    'PhoneVendor', 'register_phone_vendor_class', 'phone_factory',
    'phone_vendor_iter_key_class', 'phone_desc_by_ua',
    'transaction_system_configuration',
)

# TODO: les attributions automatiques de demarrage ne checkent pas toute la semantique:
# en particulier si rien n'empeche qu'une attr au demarrage entraine une network collision
# BUGFIXer ca...
