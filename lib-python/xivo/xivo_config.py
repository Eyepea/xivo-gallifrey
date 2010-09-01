"""Common routines and objects for autoprovisioning services in XIVO

Copyright (C) 2007-2010  Proformatique

"""
# Dependencies/highly recommended? : arping curl

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007-2010  Proformatique

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import os
import re
import copy
import yaml
from ConfigParser import ConfigParser
from itertools import chain, count
import logging

from xivo import network
from xivo import ConfigDict
from xivo import interfaces
from xivo import system
from xivo import xys
from xivo import udev
from xivo import shvar
from xivo import MacIpResolver


log = logging.getLogger("xivo.xivo_config") # pylint: disable-msg=C0103


SYSCONF_DIR = "/etc/pf-xivo/sysconf"

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

#                                   path used by system:
INTERFACES_FILE = "interfaces"      # /etc/network/
DHCPD_CONF_FILE = "dhcpd.conf"      # /etc/dhcp3/
DHCP3_SERVER_FILE = "dhcp3-server"  # /etc/default/
IFPLUGD_FILE = "ifplugd"            # /etc/default/


ProvGeneralConf = {
    'listen_ipv4':              "127.0.0.1",
    'listen_port':              8666,
    'connect_ipv4':             "127.0.0.1",
    'connect_port':             8666,

    'use_dhcpd_leases':         1,
    'scan_ifaces_prefix':       "eth,vlan",
    'arping_cmd':               "sudo /usr/sbin/arping",
    'arping_sleep_us':          150000,

    'log_level':                "info",

    'database_uri':             "sqlite:/var/lib/asterisk/astsqlite?timeout_ms=150",
    'http_read_request_to_s':   90,
    'http_request_to_s':        90,

    'excl_del_lock_to_s':       45,

    'tftproot':                 "/tftpboot/",
    'curl_cmd':                 "/usr/bin/curl",
    'curl_to_s':                30,
    'telnet_to_s':              30,
    'templates_dir':            "/usr/share/pf-xivo-provisioning/files/",
    'asterisk_ipv4':            "0.0.0.0",
    'ntp_server_ipv4':          "0.0.0.0",
    'registrar_main':           "0.0.0.0",
    'registrar_backup':         "0.0.0.0",
    'proxy_main':               "0.0.0.0",
    'proxy_backup':             "0.0.0.0",
}
Pgc = ProvGeneralConf
AUTHORIZED_PREFIXES = ("eth", "vlan", "dummy")


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


def netif_managed(ifname):
    """
    Return True iff ifname seems to be a name of a network interface that must
    be managed by this module.
    """
    return True in [ifname.startswith(x) for x in AUTHORIZED_PREFIXES]


def ipv4_from_macaddr(macaddr):
    """
    When using dhcpd.leases; wrapper for MacIpResolver.ipv4_from_macaddr().
    Else; wrapper for network.ipv4_from_macaddr() that sets:
    * ifname_match_func to netif_managed
    * arping_cmd_list to Pgc['arping_cmd'].strip().split()
    * arping_sleep_us to Pgc['arping_sleep_us']
    """
    if Pgc['use_dhcpd_leases']:
        return MacIpResolver.ipv4_from_macaddr(macaddr)
    else:
        return network.ipv4_from_macaddr(macaddr,
                                         ifname_match_func=netif_managed,
                                         arping_cmd_list=Pgc['arping_cmd'].strip().split(),
                                         arping_sleep_us=Pgc['arping_sleep_us'])


def macaddr_from_ipv4(ipv4):
    """
    When using dhcpd.leases; wrapper for MacIpResolver.macaddr_from_ipv4().
    Else; wrapper for network.macaddr_from_ipv4() that sets:
    * ifname_match_func to netif_managed
    * arping_cmd_list to Pgc['arping_cmd'].strip().split()
    * arping_sleep_us to Pgc['arping_sleep_us']
    """
    if Pgc['use_dhcpd_leases']:
        return MacIpResolver.macaddr_from_ipv4(ipv4)
    else:
        return network.macaddr_from_ipv4(ipv4,
                                         ifname_match_func=netif_managed,
                                         arping_cmd_list=Pgc['arping_cmd'].strip().split(),
                                         arping_sleep_us=Pgc['arping_sleep_us'])


# States for linesubst()
NORM = object()
ONE = object()
TWO = object()
LIT = object()
TLIT = object()
TERM = object()

def linesubst(line, variables):
    """
    In a string, substitute '{{varname}}' occurrences with the value of
    variables['varname'], '\\' being an escaping char...
    If at first you don't understand this function, draw its finite state
    machine and everything will become crystal clear :)
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
                    log.warning("Unknown variable %r detected, will just be replaced by an empty string", curvar)
                else:
                    log.debug("Substitution of {{%s}} by %r", curvar, variables[curvar])
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
        log.warning("st is not NORM at end of line: " + line)
        log.warning("returned substitution: " + out)
    return out


def txtsubst(lines, variables, target_file=None, charset=None):
    """
    Log that target_file is going to be generated, and calculate its
    content by applying the linesubst() transformation with the given
    variables to each given lines.
    """
    if target_file:
        log.info("In process of generating file %r", target_file)

    if not charset:
        return [linesubst(line, variables) for line in lines]

    ret = []
    for line in lines:
        linesub = linesubst(line, variables)
        if isinstance(line, unicode):
            ret.append(linesub.encode(charset))
        else:
            ret.append(linesub)
    return ret


ID_CHR = ''.join(map(chr, xrange(0, 256)))

def well_formed_provcode(provcode):
    """
    @provcode: string
    Return True <=> provcode is a well formed XIVO provisioning code
    """
    return provcode and not provcode.translate(ID_CHR, "0123456789")


class ProviConfigurability(type):
    """
    Metaclass that manages phone vendor classes
    """

    # Workaround pylint bug:
    # pylint: disable-msg=C0203

    def __init__(cls, name, bases, dct):
        super(ProviConfigurability, cls).__init__(name, bases, dct)
        if hasattr(cls, 'do_reinitprov'):
            register_phone_vendor_class(cls)


class PhoneVendorMixin(object):
    """
    Phone vendor base class
    """

    __metaclass__ = ProviConfigurability

    TELNET_TO_S = None
    CURL_TO_S = None
    CURL_CMD = None
    ASTERISK_IPV4 = None
    REGISTRAR_MAIN = None
    REGISTRAR_BACKUP = None
    PROXY_MAIN = None
    PROXY_BACKUP = None
    TEMPLATES_DIR = None
    NTP_SERVER_IPV4 = None
    TFTPROOT = None
    DEFAULT_LOCALE = 'fr_FR'
    DEFAULT_TIMEZONE = 'Europe/Paris'
    PROVI_VARS = {'config':
                        {'asterisk_ipv4':       None,
                         'ntp_server_ipv4':     None,
                         'registrar_main':      None,
                         'registrar_backup':    None,
                         'proxy_main':          None,
                         'proxy_backup':        None,
                         },
                  'user':
                        {'display_name':    'name',
                         'dtmfmode':        'dtmfmode',
                         'firstname':       'firstname',
                         'lastname':        'lastname',
                         'mailbox':         'mailbox',
                         'phone_ident':     'ident',
                         'phone_passwd':    'passwd',
                         'phone_number':    'number',
                         'simultcalls':     'simultcalls',
                         'subscribe_mwi':   'subscribemwi'},
                  'exten':
                        {'dnd':             'enablednd',
                         'forward_unc':     'fwdunc',
                         'park':            'parkext',
                         'pickup':          'pickup',
                         'pickup_group':    'pickupexten',
                         'voicemail':       'vmusermsg'}
                 }

    @classmethod
    def setup(cls, config):
        """
        Configuration of class attributes
        """
        cls.TELNET_TO_S = config['telnet_to_s']
        cls.CURL_TO_S = config['curl_to_s']
        cls.CURL_CMD = config['curl_cmd']
        cls.ASTERISK_IPV4 = config['asterisk_ipv4']
        cls.TEMPLATES_DIR = config['templates_dir']
        cls.NTP_SERVER_IPV4 = config['ntp_server_ipv4']
        cls.TFTPROOT = config['tftproot']
        def get_value_if_valid(value, default):
            if value and value != '0.0.0.0':
                return value
            else:
                return default
        cls.REGISTRAR_MAIN = config['registrar_main']
        cls.REGISTRAR_BACKUP = get_value_if_valid(config['registrar_backup'], None)
        cls.PROXY_MAIN = config['proxy_main']
        cls.PROXY_BACKUP = get_value_if_valid(config['proxy_backup'], None)

        cls.PROVI_VARS['config']['asterisk_ipv4'] = cls.ASTERISK_IPV4
        cls.PROVI_VARS['config']['ntp_server_ipv4'] = cls.NTP_SERVER_IPV4
        cls.PROVI_VARS['config']['registrar_main'] = cls.REGISTRAR_MAIN
        cls.PROVI_VARS['config']['registrar_backup'] = cls.REGISTRAR_BACKUP
        cls.PROVI_VARS['config']['proxy_main'] = cls.PROXY_MAIN
        cls.PROVI_VARS['config']['proxy_backup'] = cls.PROXY_BACKUP

    @classmethod
    def set_provisioning_variables(cls, provinfo, xvars, format_var=None, format_extension=None):
        for key in cls.PROVI_VARS['config'].keys():
            if xvars.has_key(key):
                continue
            elif not format_var:
                xvars[key] = cls.PROVI_VARS['config'][key]
            else:
                xvars[key] = format_var(cls.PROVI_VARS['config'][key])

        for key, value in cls.PROVI_VARS['user'].iteritems():
            key = "user_%s" % key

            if xvars.has_key(key) \
               or not provinfo.has_key(value) \
               or provinfo[value] is None:
                continue
            elif not format_var:
                xvars[key] = provinfo[value]
            else:
                xvars[key] = format_var(provinfo[value])

        for key, value in cls.PROVI_VARS['exten'].iteritems():
            key = "exten_%s" % key

            if xvars.has_key(key) \
               or not provinfo['extensions'].has_key(value) \
               or provinfo['extensions'][value] is None:
                continue
            elif not format_extension:
                xvars[key] = provinfo['extensions'][value]
            else:
                xvars[key] = format_extension(provinfo['extensions'][value])

        return xvars
    
    def __init__(self, phone):
        """
        Constructor.

        @phone must be a dictionary containing everything needed for the
        one phone provisioning process to take place.  That is the
        following keys:

        'model', 'vendor', 'macaddr', 'actions', 'ipv4' if the value
        for 'actions' is not 'no'
        """
        self.phone = phone
        log.info("Instantiation of %r", self.phone)

    def action_reinit(self):
        """
        This function can be called under some conditions after the
        configuration for this phone has been generated by the
        generate_reinitprov() method.
        """
        if self.phone["actions"] == "no": # possible cause: "distant" provisioning
            log.info("Skipping REINIT action for phone %s", self.phone['macaddr'])
            return
        log.info("Sending REINIT command to phone %s", self.phone['macaddr'])
        self.do_reinit()
        log.debug("Sent REINIT command to phone %s", self.phone['macaddr'])

    def action_reboot(self):
        """
        This function can be called under some conditions after the
        configuration for this phone has been generated by the
        generate_autoprov() method.
        """
        if self.phone["actions"] == "no": # distant provisioning with actions disabled
            log.info("Skipping REBOOT action for phone %s", self.phone['macaddr'])
            return
        log.info("Sending REBOOT command to phone %s", self.phone['macaddr'])
        self.do_reboot()
        log.debug("Sent REBOOT command to phone %s", self.phone['macaddr'])

    def action_upgradefw(self):
        """
        This function upgrade the phone firmware.
        """
        log.info("Sending UPGRADE FIRMWARE command. (phone: %r, vendor: %r)", self.phone['macaddr'], self.phone['vendor'])

        if hasattr(self, 'do_upgradefw'):
            self.do_upgradefw()
        else:
            log.error("Missing UPGRADE FIRMWARE command. (phone: %r, vendor: %r)", self.phone['macaddr'], self.phone['vendor'])
            return
        log.debug("Sent UPGRADE FIRMWARE command. (phone: %r, vendor: %r)", self.phone['macaddr'], self.phone['vendor'])

    def generate_reinitprov(self, provinfo):
        """
        This function put the configuration for the phone back in guest
        state.
        """
        log.info("About to GUEST'ify the phone %s", self.phone['macaddr'])
        self.do_reinitprov(provinfo)
        log.debug("Phone GUEST'ified %s", self.phone['macaddr'])

    def generate_autoprov(self, provinfo):
        """
        This function generate the configuration for the phone with
        provisioning informations provided in the provinfo dictionary,
        which must contain the following keys:

        'name', 'ident', 'number', 'passwd'
        """
        log.info("About to AUTOPROV the phone %s with infos %s", self.phone['macaddr'], str(provinfo))
        self.do_autoprov(provinfo)
        log.debug("Phone AUTOPROV'ed %s", self.phone['macaddr'])


PhoneClasses = {}


def register_phone_vendor_class(cls):
    """
    Register a new class, derived from PhoneVendorMixin, that implements
    provisioning methods for some phones of a given vendor.
    """
    global PhoneClasses
    key = cls.__name__.lower()
    if key not in PhoneClasses:
        PhoneClasses[key] = cls
    else:
        raise ValueError, "A registration as already occurred for %r" % key


def phone_vendor_iter_key_class():
    """
    Iterate over phone classes.
    """
    global PhoneClasses
    return PhoneClasses.iteritems()


def phone_factory(phone):
    """
    Instantiate a PhoneVendorMixin derived class according to the phone
    description.
    """
    global PhoneClasses
    phone_class = PhoneClasses[phone["vendor"]]
    return phone_class(phone)


def phone_desc_by_ua(ua, exc_info=True):
    """
    Return a tuple (vendor_key, model, firmware), or None if no
    PhoneVendorMixin derived class has recognized the user agent string.
    vendor_key, model, and firmware are strings.
    """
    global PhoneClasses
    for phone_class in PhoneClasses.itervalues():
        try:
            r = phone_class.get_vendor_model_fw(ua)
        except Exception:
            r = None
            if exc_info:
                log.exception("in phone_desc_by_ua(ua=%r)", ua)
        if r:
            return r
    return None


def phone_classes_setup():
    """
    For each registered phone class k, call k.setup(Pgc)
    """
    for phone_class in PhoneClasses.itervalues():
        phone_class.setup(Pgc)


### GENERAL CONF


def specific(nstr):
    return nstr not in ('reserved', 'none', 'void', 'removed')


def specific_or_reserved(nstr):
    return nstr not in ('none', 'void', 'removed')


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


def domain_label(nstr, schema):
    """
    !~domain_label
        Return True if the document string is a domain label, else False
    """
    return network.DomainLabelOk(nstr) and len(nstr) <= 63


def search_domain(nstr, schema):
    """
    !~search_domain
        Return True if the document string is suitable for use in the
        search line of /etc/resolv.conf, else False
    """
    return network.plausible_search_domain(nstr)


def macaddr(nstr, schema):
    """
    !~macaddr
        Check that the document string is an ethernet mac addresses
    """
    return network.is_mac_address_valid(nstr)


def ipv4_address(nstr, schema):
    """
    !~ipv4_address
        Check that the document string is an IPv4 addresses
    """
    return network.is_ipv4_address_valid(nstr)


def ipv4_address_or_domain(nstr, schema):
    """
    !~ipv4_address_or_domain
        Return True if the document string is an IPv4 address
        or a domain, else False
    """
    return network.is_ipv4_address_valid(nstr) \
           or network.plausible_search_domain(nstr)


def netmask(nstr, schema):
    """
    !~netmask
        Check that the document string is an IPv4 netmasks
    """
    return network.is_ipv4_address_valid(nstr) \
           and network.plausible_netmask(network.parse_ipv4(nstr))


def specific_prefixDec(fname, prefix):
    """
    Return a XYS validator that checks that corresponding document strings
    are 'reserved', 'none', 'void', 'removed' or valid per !~~prefixDec prefix.
    """
    def validator(nstr, schema):
        """
        !~<validator generated by specific_prefixDec() >
            Checks that corresponding document strings are 'reserved',
            'none', 'void', 'removed' or valid per !~~prefixDec prefix.
        """
        if nstr in ('reserved', 'none', 'void', 'removed'):
            return True
        if not nstr.startswith(prefix):
            return False
        try:
            int(nstr[len(prefix):])
        except ValueError:
            return False
        return True
    validator.__name__ = fname # pylint: disable-msg=W0621
    return validator


def plausible_static(static, schema):
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
        cnt = dict_count.get(symbol, 0)
        if minref <= cnt <= maxref:
            dict_ok[symbol] = cnt
        else:
            dict_out_of_bounds[symbol] = cnt

    return dict_ok, dict_out_of_bounds, dict_undefined


def plausible_configuration(conf, schema):
    """
    !~plausible_configuration
        Validate the general system configuration
    """

    dict_ok, dict_out_of_bounds, dict_undefined = references_relation(conf['ipConfs'], get_referenced_ipConfTags(conf), minref=0, maxref=1)
    if dict_out_of_bounds:
        log.error("duplicated static IP conf references in vlans description: %r", dict_out_of_bounds)
        return False
    if dict_undefined:
        log.error("undefined referenced static IP configurations: %r", dict_undefined)
        return False

    referenced_vsTags = get_referenced_vsTags(conf)
    dict_ok, dict_out_of_bounds, dict_undefined = references_relation(conf['vlans'], referenced_vsTags, minref=0, maxref=1)
    if dict_out_of_bounds:
        log.error("duplicated vlan references in network interfaces description: %r", dict_out_of_bounds)
        return False
    if dict_undefined:
        log.error("undefined vlan configurations: %r", dict_undefined)
        return False

    # TODO: uniqueness concept in schema, default types in schema
    nameservers = conf['resolvConf'].get('nameservers')
    if nameservers:
        nameservers = map(network.normalize_ipv4_address, nameservers)
        unique_nameservers = frozenset(nameservers)
        if len(unique_nameservers) != len(nameservers):
            log.error("duplicated nameservers in %r", tuple(nameservers))
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
        log.error("duplicated active networks: %r", dict((('.'.join(map(str, net)), tuple(names)) for net, names in active_networks.iteritems())))
        return False

    # VOIP service
    ipConfVoip = conf['services']['voip']['ipConf']
    if ipConfVoip not in conf['ipConfs']:
        log.error("the voip service references a static ip configuration that does not exists: %r", ipConfVoip)
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
                log.error("invalid voip service related IP %r: %r", field, addresses[field])
                return False
    # router, if present, must be in the network
    if 'router' in addresses:
        ok, other = ip_in_network(network.parse_ipv4(addresses['router']), net, netmask)
        if not ok:
            log.error("router must be in network %s/%s but seems to be in %s/%s",
                      network.format_ipv4(net), network.format_ipv4(netmask), network.format_ipv4(other), network.format_ipv4(netmask))
            return False
    # check that any range is in the network and with min <= max
    for range_field in 'voipRange', 'alienRange':
        if range_field not in addresses:
            continue
        ip_range = map(network.parse_ipv4, addresses[range_field])
        for ip in ip_range:
            ok, other = ip_in_network(ip, net, netmask)
            if not ok:
                log.error("IP %s is not in network %s/%s", network.format_ipv4(ip), network.format_ipv4(net), network.format_ipv4(netmask))
                return False
        if not (ip_range[0] <= ip_range[1]):
            log.error("Invalid IP range: %r", tuple(addresses[range_field]))
            return False
    # check that there is no overlapping ranges
    parsed_voipRange = map(network.parse_ipv4, addresses['voipRange'])
    all_ranges = [ parsed_voipRange ]
    if 'alienRange' in addresses:
        one = parsed_voipRange
        two = map(network.parse_ipv4, addresses['alienRange'])
        all_ranges.append(two)
        if (one[0] <= two[0] <= one[1]) or (one[0] <= two[1] <= one[1]):
            log.error("overlapping DHCP ranges detected")
            return False
    # check that there is no fixed IP in any DHCP range
    fixed_addresses = [ network.parse_ipv4(ipConfVoip_static[field]) for field in ('address', 'gateway') if field in ipConfVoip_static ]
    fixed_addresses.append(broadcast_from_static(ipConfVoip_static))
    fixed_addresses.extend([ network.parse_ipv4(addresses[field]) for field in voip_fixed if field in addresses ])
    for rang in all_ranges:
        for addr in fixed_addresses:
            if rang[0] <= addr <= rang[1]:
                log.error("fixed address %r detected in DHCP range %r", network.format_ipv4(addr), tuple(rang))
                return False

    return True


xys.add_validator(domain_label, u'!!str')
xys.add_validator(search_domain, u'!!str')
xys.add_validator(ipv4_address, u'!!str')
xys.add_validator(ipv4_address_or_domain, u'!!str')
xys.add_validator(netmask, u'!!str')
xys.add_validator(macaddr, u'!!str')
xys.add_validator(plausible_static, u'!!map')
xys.add_validator(plausible_configuration, u'!!map')
xys.add_validator(specific_prefixDec('vlanIpConf', 'static_'), u'!!str')
xys.add_validator(specific_prefixDec('netIfaceVlans', 'vs_'), u'!!str')


SCHEMA_NETWORK_CONFIG = xys.load("""!~plausible_configuration
resolvConf:
    search?: !~search_domain bla.tld
    nameservers?: !~~seqlen(1,3) [ !~ipv4_address 192.168.0.200 ]
ipConfs:
    !~~prefixedDec static_: !~plausible_static
        address:    !~ipv4_address 192.168.0.100
        netmask:    !~netmask 255.255.255.0
        broadcast?: !~ipv4_address 192.168.0.255
        gateway?:   !~ipv4_address 192.168.0.254
        mtu?:       !~~between(68,1500) 1500
vlans:
    !~~prefixedDec vs_:
        !~~between(0,4094) 0: !~vlanIpConf static_0001
netIfaces:
    !~~prefixedDec eth: !~netIfaceVlans vs_0001
services:
    voip:
        ipConf: !~~prefixedDec static_
        addresses:
            voipServer:  !~ipv4_address 192.168.1.200
            bootServer:  !~ipv4_address 192.168.1.200
            voipRange:   !~~seqlen(2,2) [ !~ipv4_address 192.168.1.200 ]
            alienRange?: !~~seqlen(2,2) [ !~ipv4_address 192.168.1.200 ]
            directory?:  !~ipv4_address 192.168.1.200
            ntp?:        !~ipv4_address 192.168.1.200
            router?:     !~ipv4_address 192.168.1.254
""")

# TODO:
# ipConfs:
#	static_xxxx:
#		comment:
# dont le contenu sera injecte dans /e/n/i

# TODO creer un program qui fill les truc par defaut non remplis de network.yaml

# TODO interface de lookup inverse: envoyer la liste des static_xxxx => mac addr + VLan ID


def reserved_netIfaces(conf):
    """
    Return the set of reserved physical network interfaces
    """
    return frozenset([ifname for ifname, ifacevlan in conf['netIfaces'].iteritems() if ifacevlan == 'reserved'])


def removed_netIfaces(conf):
    """
    Return the set of removed interfaces
    """
    return frozenset([ifname for ifname, ifacevlan in conf['netIfaces'].iteritems() if ifacevlan == 'removed'])


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


def unreserved_interfaces_options(options):
    """
    Return the set of unreserved interfaces options
    """
    reserved = interfaces.EniBlockAllow.ALLOWUP_CLASSES + \
               ('name',
                'iface',
                'mapping',
                'ifname',
                'family',
                'method',
                'address',
                'netmask',
                'broadcast',
                'gateway',
                'vlan-id',
                'vlan-raw-device',
                'mtu')

    return [(optname, optvalue) for optname, optvalue in options if optname not in reserved]


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


def load_configuration(conf_source):
    """
    Parse the first YAML document in a stream and produce the corresponding
    normalized internal representation of the configuration.

    Raise a xivo_config.InvalidConfigurationError if the configuration is
    invalid.
    """
    conf = yaml.safe_load(conf_source)
    if not xys.validate(conf, SCHEMA_NETWORK_CONFIG):
        raise InvalidConfigurationError("Invalid configuration")
    # TODO: do that thanks to a schema based mapping ("mapping" in functional programming meaning)
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


def save_configuration(conf, file_obj):
    """
    Serialize the internal representation of the configuration to @file_obj
    in YAML.

    This can only be done if the configuration is valid.
    InvalidConfigurationError will be raised if the configuration is not
    valid.
    """
    if not xys.validate(conf, SCHEMA_NETWORK_CONFIG):
        raise InvalidConfigurationError("Invalid configuration")
    result = yaml.safe_dump(conf, stream=file_obj, default_flow_style=False, indent=4)
    system.flush_sync_file_object(file_obj)
    return result


def load_current_configuration():
    """
    Load the configuration from the standard current path.
    """
    return load_configuration(file(os.path.join(STORE_BASE, STORE_CURRENT, NETWORK_CONFIG_FILE)))


def save_configuration_for_transaction(conf):
    """
    Serialize the internal representation of the XIVO configuration in a
    file that will be used during the system configuration generation
    transaction.
    """
    return save_configuration(conf, system.file_w_create_directories(os.path.join(STORE_BASE, STORE_TMP, NETWORK_CONFIG_FILE)))


def natural_vlan_name(phy, vlanId):
    """
    @phy: string
    @vlanId: integer
    """
    if not vlanId:
        return phy
    else:
        return "%s.%d" % (phy, vlanId)


def generate_interfaces(old_lines, conf):
    """
    Yield the new lines of interfaces(5) according to the old ones and the
    current configuration
    """
    log.info("ENTERING generate_interfaces()")

    eni = interfaces.parse(old_lines)

    rsvd_base = reserved_netIfaces(conf)
    rmvd_full = removed_netIfaces(conf)
    rsvd_full = reserved_vlans(conf)
    rsvd_mapping_dest = interfaces.get_mapping_dests(eni, rsvd_base, rsvd_full)

    def unhandled_or_reserved(ifname):
        """
        Is ifname not handled either because it does not start with a
        known prefix, or because it is explicitely reserved
        """
        if ifname in rmvd_full:
            return False
        return (not netif_managed(ifname)) or \
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
                log.debug("keeping unhandled or reserved %s block %r", block.__class__.__name__, block.ifname)
                down = KEPT
            else:
                log.info("removing handled and not reserved %s block %r", block.__class__.__name__, block.ifname)
                down = REMOVED
        elif isinstance(block, interfaces.EniBlockAllow):
            assert len(block.cooked_lines) == 1, "a EniBlockAllow contains more than one cooked line"
            line_recipe = interfaces.EniCookLineRecipe(block.raw_lines)
            for ifname in block.allow_list[:]:
                if unhandled_or_reserved(ifname):
                    log.debug("keeping unhandled or reserved %r in %r stanza", ifname, block.allow_kw)
                else:
                    log.info("removing handled and not reserved %r in %r stanza", ifname, block.allow_kw)
                    mo = re.search(re.escape(ifname) + r'(\s)*', line_recipe.cooked_line)
                    if mo:
                        line_recipe.remove_part(mo.start(), mo.end())
                    else:
                        log.warning("%r has not been found in %r", ifname, line_recipe.cooked_line)
                    block.allow_list.remove(ifname)
            line_recipe.update_block(block)
            if block.allow_list:
                down = KEPT
            else:
                log.info("removing empty %r stanza", block.allow_kw)
                down = REMOVED
        else: # interfaces.EniBlockUnknown
            log.info("removing invalid block")
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
            elif 'ipConfs' in conf and ipConfs_tag in conf['ipConfs']:
                family      = 'inet'
                method      = 'static'
                currentconf = conf['ipConfs'][ipConfs_tag]
            else:
                currentconf = conf['customipConfs'][ipConfs_tag]
                family      = currentconf.get('family', 'inet')
                method      = currentconf.get('method', 'static')

            if currentconf.has_key('ifname'):
                ifname = currentconf['ifname']
            else:
                ifname = natural_vlan_name(phy, vlanId)

            log.info("generating configuration for %r", ifname)

            for allow in interfaces.EniBlockAllow.ALLOWUP_CLASSES:
                if currentconf.get(allow):
                    yield "%s %s\n" % (allow, ifname)

            yield "iface %s %s %s\n" % (ifname, family, method)

            if vlanId:
                yield "\tvlan-id %d\n" % vlanId
                yield "\tvlan-raw-device %s\n" % currentconf.get('vlan-raw-device', phy)

            if method == 'static' and family in ('inet', 'inet6'):
                yield "\taddress %s\n" % currentconf['address']
                yield "\tnetmask %s\n" % currentconf['netmask']

                if family == 'inet' and 'broadcast' in currentconf:
                    yield "\tbroadcast %s\n" % currentconf['broadcast']

                if 'gateway' in currentconf:
                    yield "\tgateway %s\n" % currentconf['gateway']

                if 'mtu' in currentconf:
                    yield "\tmtu %d\n" % currentconf['mtu']

            if 'options' in currentconf:
                for optname, optvalue in unreserved_interfaces_options(currentconf['options']):
                    yield "\t%s %s\n" % (optname, optvalue)

            yield "\n"

    log.info("LEAVING generate_interfaces.")


# XXX traces
def generate_dhcpd_conf(conf):
    """
    Yield each line of the generated dhcpd.conf
    """
    log.info("ENTERING generate_dhcpd_conf()")

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
    yield 'subnet %s netmask %s {\n' % (network.format_ipv4(network_from_static(ipConfVoip)), ipConfVoip['netmask'])
    yield '\n'
    yield '    one-lease-per-client on;\n'
    yield '\n'
    yield '    option subnet-mask %s;\n' % ipConfVoip['netmask']
    yield '    option broadcast-address %s;\n' % network.format_ipv4(broadcast_from_static(ipConfVoip))
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

    log.info("LEAVING generate_dhcpd_conf.")


# note: we use the header below because it is the header also used by the
# ifplugd package
DEFAULT_IFPLUGD = \
"""# This file may be changed either manually or by running dpkg-reconfigure.
#
# N.B.: dpkg-reconfigure deletes everything from this file except for
# the assignments to variables INTERFACES, HOTPLUG_INTERFACES, ARGS and
# SUSPEND_ACTION.  When run it uses the current values of those variables
# as their default values, thus preserving the administrator's changes.
#
# This file is sourced by both the init script /etc/init.d/ifplugd and
# the hotplug script /etc/hotplug.d/net/ifplugd.hotplug to give default
# values. The init script starts ifplugd for all interfaces listed in
# INTERFACES, and the hotplug script starts ifplugd for all interfaces
# listed in HOTPLUG_INTERFACES. The special value all starts one
# ifplugd for all interfaces being present.
INTERFACES=""
HOTPLUG_INTERFACES=""
ARGS="-q -f -u0 -d10 -w -I"
SUSPEND_ACTION="stop"
"""


def generate_default_ifplugd(old_lines, conf):
    """
    Yield the new lines of /etc/default/ifplugd according to the old ones and
    the current configuration
    """
    log.info("ENTERING generate_default_ifplugd()")

    if not old_lines:
        old_lines = DEFAULT_IFPLUGD.split("\n")

    reslst, resdct = shvar.load(old_lines)
    shvar.strip_overridden_assignments(reslst)
    iflist = [phy for phy, vsTag in conf['netIfaces'] if specific_or_reserved(vsTag)]
    shvar.slow_set_assign(reslst, "INTERFACES", " ".join(iflist))

    for line in shvar.format(reslst):
        yield line

    log.info("LEAVING generate_default_ifplugd.")


class TransactionError(Exception):
    "Error raised on transaction cancellation."
    def __init__(self, msg, original_exception=None):
        self.__reprmsg = "<%s %r>" % (self.__class__.__name__, msg)
        self.__strmsg = str(msg)
        self.original_exception = original_exception
        Exception.__init__(self, msg)
    def __repr__(self):
        return self.__reprmsg
    def __str__(self):
        return self.__strmsg


def rotate_entries(previous, current, new):
    """
    If current exists:
      1) Delete previous.
      2) Move current to previous.
    3) Finally, move new to current.

    OSError exceptions raised during 1) are ignored (but traced).
    OSError exceptions during 2) are handled by trying to delete current
    instead.
    Other exceptions are not catched.
    """
    log.info("Entering rotate_entries(previous=%r, current=%r, new=%r)", previous, current, new)

    if os.path.exists(current):
        log.info("%r to %r", current, previous)

        log.debug("about to remove %r", previous)
        try:
            system.rm_rf(previous)
        except OSError:
            log.warning("rotate_entries: failed to delete %r", previous, exc_info=True)

        log.debug("about to rename %r to %r", current, previous)
        try:
            os.rename(current, previous)
        except OSError:
            log.warning("rotate_entries: failed to rename %r to %r", current, previous, exc_info=True)
            system.rm_rf(current)

        system.sync_no_oserror()

    log.debug("about to rename %r to %r", new, current)
    os.rename(new, current)

    system.sync_no_oserror()

    log.info("Leaving rotate_entries.")


def transactional_generation(store_base, store_subs, gen_base, gen_subs, generation_func):
    """
    This function first removes some temporary directories.  Then it
    completes a three staged transaction if it has been started, or does
    nothing otherwise.  The purpose of the transaction is to generate some
    configuration files from others.  The source files are all stored along
    in a common subdirectory, and the destination files are in an other
    subdirectory.  The transaction guaranty as much as possible (we depends
    on the filesystem) that in the stable state the source and the
    destination configurations are in sync.  The transaction must be
    externally initiated by the creation of the new store entry in the
    filesystem.

    This function returns True if a transaction has been processed
    successfully or None if no transaction was in progress.  A transaction
    fails and is cancelled iff generation_func() raises an exception; in
    this case a TransactionError is raised.  Uncatched exceptions raised by
    transactional_generation() must be considered as fatal errors requiring
    human intervention or at least restoration of a known stable state.

    If this function returns with no exception, the resulting state is
    stable and clean; there are no remaining temporary directories.

    @store_base: base directory of source configurations.
    @store_subs: sub-paths from store_base to previous, current, new, tmp,
                 and failed.
    @gen_base: base directory of all generated configurations.
    @gen_subs: sub-paths from gen_base to previous, current, new, and tmp.
    @generation_func(to_gen, prev_gen, current_src):
        where:
        @to_gen: path where the configuration must be generated.
        @prev_gen: path where the previously generated configuration
            stands.  If no configuration has been previously generated,
            prev_gen contains None instead.
        @current_src: path where the source configuration stands.

    NOTE: Because of our requirements some state that is only stored in the
    generated files must be preserved (configuration of reserved interfaces
    and of unhandled interfaces).  That is why generation_func takes its
    second parameter.
    """
    log.info("ENTERING transactional_generation()")
    log.debug("  store_base = %r", store_base)
    log.debug("  store_subs = %r", store_subs)
    log.debug("  gen_base = %r", gen_base)
    log.debug("  gen_subs = %r", gen_subs)

    store_previous, store_current, store_new, store_tmp, store_failed = [os.path.join(store_base, x) for x in store_subs]
    gen_previous, gen_current, gen_new, gen_tmp = [os.path.join(gen_base, x) for x in gen_subs]

    for entry in (store_tmp, gen_tmp):
        if os.path.exists(entry):
            log.warning("transactional_generation: removing stalled entry %r", entry)
            system.rm_rf(entry)

    if os.path.exists(store_new) and not os.path.exists(gen_new):
        log.info("BEGIN PHASE 1")

        log.debug("about to remove %r", gen_tmp)
        system.rm_rf(gen_tmp)

        if os.path.exists(gen_current):
            log.info("%r exists", gen_current)
            previously_generated = gen_current
        else:
            log.info("%r does not exist", gen_current)
            previously_generated = None

        log.debug("about to call %r", generation_func)
        log.debug("  gen_tmp = %r", gen_tmp)
        log.debug("  previously_generated = %r", previously_generated)
        log.debug("  store_new = %r", store_new)

        try:
            generation_func(gen_tmp, previously_generated, store_new)

        except Exception, ex:
            log.exception("Error during generation - cancelling transaction")

            system.rm_rf(store_failed)

            log.debug("about to %r rename to %r", store_new, store_failed)
            try:
                os.rename(store_new, store_failed)
            except OSError:
                log.warning("transactional_generation: failed to rename %r to %r - destroying %r",
                            store_new, store_failed, store_new, exc_info=True)
                system.rm_rf(store_new)

            log.info("about to remove incompletely generated directory %r", gen_tmp)
            system.rm_rf(gen_tmp)

            system.sync_no_oserror()

            log.info("END PHASE 1 - transaction cancelled - raising TransactionError")

            raise TransactionError("generation failure", ex)

        else:
            log.info("Successful generation")

            system.sync_no_oserror()

            log.debug("about to rename %r to %r", gen_tmp, gen_new)
            os.rename(gen_tmp, gen_new)

            system.sync_no_oserror()

            log.info("END PHASE 1 - generation performed")

    if os.path.exists(store_new) and os.path.exists(gen_new):
        log.info("BEGIN PHASE 2")

        rotate_entries(store_previous, store_current, store_new)

        log.info("END PHASE 2")

    if (not os.path.exists(store_new)) and os.path.exists(gen_new):
        log.info("BEGIN PHASE 3")

        rotate_entries(gen_previous, gen_current, gen_new)

        log.info("END PHASE 3")

        log.info("LEAVING transactional_generation - success")
        return True

    log.info("LEAVING transactional_generation - no transaction to complete")


def generate_system_configuration(to_gen, prev_gen, current_xivo_conf):
    """
    Generate system configuration from our own configuration model.
    """
    os.mkdir(to_gen)

    config = load_configuration(file(os.path.join(current_xivo_conf, NETWORK_CONFIG_FILE)))

    def gen_from_old(filename, genfunc):
        if prev_gen:
            old_lines = file(os.path.join(prev_gen, filename))
        else:
            old_lines = ()
        system.file_writelines_flush_sync(os.path.join(to_gen, filename),
                                          genfunc(old_lines, config))
        if old_lines:
            old_lines.close()

    gen_from_old(INTERFACES_FILE, generate_interfaces)

    gen_from_old(IFPLUGD_FILE, generate_default_ifplugd)

    system.file_writelines_flush_sync(os.path.join(to_gen, DHCPD_CONF_FILE),
                                      generate_dhcpd_conf(config))


def transaction_system_configuration():
    """
    Transactionally generate system configuration from our own
    configuration model.
    """
    transactional_generation(STORE_BASE, (STORE_PREVIOUS, STORE_CURRENT, STORE_NEW, STORE_TMP, STORE_FAILED),
                             GENERATED_BASE, (GENERATED_PREVIOUS, GENERATED_CURRENT, GENERATED_NEW, GENERATED_TMP),
                             generate_system_configuration)


def gen_plugged_by_phy(phys):
    """
    Construct a cache of carrier status of interfaces in the sequence phys.
    The cache is a mapping where keys are interfaces and values a boolean
    representing the carrier status:
        (False => disconnected, True => connected)
    """
    return dict(((phy, network.is_interface_plugged(phy)) for phy in phys))


def cmp_bool_lexdec(x, y):
    """
    Let X = (x[0], x[1]) and Y = (y[0], y[1]) where
      x[0] and y[0] are boolean and
      x[1] and y[1] are lexico-decimal strings
        Lexico-decimal strings are totally ordered by the
        network.cmp_lexdec function.

    This function defines a total order on the set of
    boolean times lexico-decimal strings, which X and Y belong to.
    """
    return cmp((x[0], network.split_lexdec(x[1])), (y[0], network.split_lexdec(y[1])))


def aa_lst_npst_phy(conf, plugged_by_phy):
    """
    Return a list of (npst, phy) in cmp_bool_lexdec order.
    @npst: not plugged status
    @phy: physical interfaces name

    Only phys that are in the "void" will be enumerated.
    """
    # TODO: detect the default path of get() and: trace it?
    def phy_handled_relate_void(phy):
        """
        Is phy both handled (its name prefix is known) and in the "void"
        """
        return netif_managed(phy) and conf['netIfaces'].get(phy, 'void') == 'void'

    phys = network.get_filtered_phys(phy_handled_relate_void)
    return sorted(((not plugged_by_phy.get(phy, False), phy) for phy in phys), cmp=cmp_bool_lexdec)


def aa_lst_npst_fifn_vsTag_vlanId(conf, plugged_by_phy):
    """
    Return a list of (npst, fifn, vsTag, vlanId) in cmp_bool_lexdec order
    for (npst, fifn) where:
    @npst: True if the supporting interface is not connected (boolean)
    @fifn: "full" interface name - that is the linux vlan interface name
        or ethXX.0 for untagged vlans (string)
    @vsTag: vlan set tag (string)
    @vlanId: vlan Id (integer)

    Only interfaces that are in the "void" will be enumerated.
    """
    res = []
    for phy, vsTag in conf['netIfaces'].iteritems():
        if specific(vsTag):
            for vlanId, ipConfs_tag in conf['vlans'][vsTag].iteritems():
                if ipConfs_tag == 'void':
                    fifn = "%s.%d" % (phy, vlanId)
                    res.append((not plugged_by_phy.get(phy, False), fifn, vsTag, vlanId))
    res.sort(cmp=cmp_bool_lexdec)
    return res


def aa_lst_vsTag(conf):
    """
    Return a list of vlan set names that are not owned by a physical
    interface and for which related IP configurations will not be in
    conflict with IP configurations already used or previously selected IP
    configurations.  The list is sorted by network.sorted_lst_lexdec().
    """
    referenced_networks = frozenset([network_from_static(conf['ipConfs'][ipConfTag]) for ipConfTag in get_referenced_ipConfTags(conf)])
    owned = frozenset(get_referenced_vsTags(conf))
    eligible_networks = set()
    unsorted_eligible_vsTag = []
    for vsTag in conf['vlans'].iterkeys():
        if vsTag in owned:
            continue
        new_nets = [network_from_static(conf['ipConfs'][ipConfTag]) for ipConfTag in conf['vlans'][vsTag].itervalues() if specific(ipConfTag)]
        set_new_nets = set(new_nets)
        # conflict within the very references of this vset?
        if len(new_nets) != len(set_new_nets):
            continue
        # conflict with networks already referenced or
        if referenced_networks.intersection(set_new_nets) or eligible_networks.intersection(set_new_nets):
            continue
        eligible_networks.update(set_new_nets)
        unsorted_eligible_vsTag.append(vsTag)
    return network.sorted_lst_lexdec(unsorted_eligible_vsTag)


def aa_lst_ipConfTag(conf):
    """
    Return a list of ipconf tags that are not owned by a vlan (in our
    terminology vlans include untaggued vlan) for which corresponding IP
    configurations will not be in conflict with IP configurations already
    used or previously selected IP configurations.
    The list is sorted by network.sorted_lst_lexdec().
    """
    owned = frozenset(get_referenced_ipConfTags(conf))
    referenced_networks = frozenset([network_from_static(conf['ipConfs'][ipConfTag]) for ipConfTag in owned])
    eligible_networks = set()
    unsorted_eligible_ipConfTag = []
    for ipConfTag in conf['ipConfs'].iterkeys():
        if ipConfTag in owned:
            continue
        new_net = network_from_static(conf['ipConfs'][ipConfTag])
        if new_net in referenced_networks or new_net in eligible_networks:
            continue
        eligible_networks.add(new_net)
        unsorted_eligible_ipConfTag.append(ipConfTag)
    return network.sorted_lst_lexdec(unsorted_eligible_ipConfTag)


def iter_new_vsTag(conf):
    """
    Yield vsTags that are not yet used.
    """
    return ("vs_%04d" % cnt for cnt in count(max(network.split_lexdec(vsTag)[1] for vsTag in conf['vlans']) + 1))


def autoattrib_conf(conf):
    """
    Auto attribute orphan vlan set to 'void' physical interfaces, in
    priority to plugged interfaces then to unplugged interfaces.
    Once done auto attribute vlan interfaces (including untagged vlans) to
    orphan ip configurations.  Finally auto attribute remaining IP
    configuration directly to remaining interfaces, creating trivial vlan
    sets so that the end to end relationship is made possible.
    """
    conf = copy.deepcopy(conf)

    plugged_by_phy = gen_plugged_by_phy(network.get_filtered_phys(netif_managed))

    # auto assign vlan set to physical interface
    iter_vsTag = iter(aa_lst_vsTag(conf))
    for npst, phy in aa_lst_npst_phy(conf, plugged_by_phy):
        try:
            vsTag = iter_vsTag.next()
        except StopIteration:
            break
        conf['netIfaces'][phy] = vsTag

    # auto assign IP configuration to VLAN
    ipConfTag_iter = iter(aa_lst_ipConfTag(conf))
    for npst, fifn, vsTag, vlanId in aa_lst_npst_fifn_vsTag_vlanId(conf, plugged_by_phy):
        try:
            ipConfTag = ipConfTag_iter.next()
        except StopIteration:
            break
        conf['vlans'][vsTag][vlanId] = ipConfTag

    # Auto assign IP configuration to physical interface, generating trivial
    # vlan set as needed.
    iter_npst_phy = iter(aa_lst_npst_phy(conf, plugged_by_phy))
    iter_vsTag = iter_new_vsTag(conf)
    for ipConfTag in ipConfTag_iter:
        try:
            npst, phy = iter_npst_phy.next()
        except StopIteration:
            break
        vsTag = iter_vsTag.next()
        conf['vlans'][vsTag] = { 0: ipConfTag }
        conf['netIfaces'][phy] = vsTag

    return conf


class AlreadyExist(Exception):
    """
    Raised if one tries to create something that already exists.
    """
    pass


def add_vlan(conf, vsTag, vlanId):
    """
    Add a VLan in conf, in-place.
    """
    vs = conf['vlans'][vsTag]
    if vlanId in vs:
        raise AlreadyExist, "VLan %d already exists in %s" % (vlanId, vsTag)
    vs[vlanId] = "void"


def save_configuration_initiate_transaction(conf):
    """
    Save XIVO configuration in a place suitable for the system
    configuration generation transaction, then initiate the transaction but
    do *not* run it.

    The transaction will be completed during the next call of
    transaction_system_configuration() - note that there is such a call at
    system startup.
    """
    save_configuration_for_transaction(conf)
    system.sync_no_oserror()
    os.rename(os.path.join(STORE_BASE, STORE_TMP), os.path.join(STORE_BASE, STORE_NEW))


def transaction_just_initiatiated():
    """
    Return True if the new store directory exists, or False.
    """
    return os.path.exists(os.path.join(STORE_BASE, STORE_NEW))


def undo_transaction_initiation():
    """
    Cancel a transaction that has just been initiated but for which
    absolutely no work of transactional_generation() has been performed
    yet.

    WARNING: the temporary store directory must not exists - and when this
    function completes it still won't exist.
    """
    # make the new store directory disappear atomically
    os.rename(os.path.join(STORE_BASE, STORE_NEW), os.path.join(STORE_BASE, STORE_TMP))
    system.rm_rf(os.path.join(STORE_BASE, STORE_TMP))


def save_configuration_perform_generation_transaction(conf):
    """
    Save XIVO configuration in a place suitable for the system
    configuration generation transaction, then initiate and perform the
    transaction.
    """
    save_configuration_initiate_transaction(conf)
    transaction_system_configuration()


def autoattrib():
    """
    Auto VLAN Set and IP Configuration attributions.
    Should be called at server startup, after any potential transaction is
    completed by transactional_generation()
    """
    config = load_current_configuration()
    aaconf = autoattrib_conf(config)
    if aaconf == config:
        return
    save_configuration_perform_generation_transaction(aaconf)


def netif_source_name(ifname):
    """
    Return True iff ifname can be a source name for a network interface.
    """
    return netif_managed(ifname) and network.is_phy_if(ifname)


def netif_target_name(ifname):
    """
    Return True iff ifname can be a target name for a network interface.
    """
    if not netif_source_name(ifname):
        return False
    parts = network.split_alpha_num(ifname)
    if len(parts) != 2:
        return False
    if str(int(parts[1])) != parts[1]:
        return False
    return True


def phy_free_in_conf(conf, ifname):
    """
    Test if @ifname is either absent from @conf, or in the "void".
    """
    return conf['netIfaces'].get(ifname, 'void') == 'void'


class EthernetRenamer(object):
    """
    Class of operations passed to udev.rename_persistent_net_rules()
    Implements non udev procedures needed to complete an udev based
    renaming of Ethernet interfaces.
    """
    def __init__(self, src_dst_lst, pure_dst_set):
        self.config = load_current_configuration()
        for pure_dst in pure_dst_set:
            if not phy_free_in_conf(self.config, pure_dst):
                raise ValueError, "Target interface name busy in XIVO configuration: %r" % pure_dst
        for src, dst in src_dst_lst:
            if src not in self.config['netIfaces']:
                raise ValueError, "Source interface name does not exist in XIVO configuration: %r" % src
            if not netif_source_name(src):
                raise ValueError, "Invalid source interface name %r" % src
            if not netif_target_name(dst):
                raise ValueError, "Invalid target interface name %r" % dst
        self.src_dst_lst = src_dst_lst
        self.pure_dst_set = pure_dst_set

    def edit(self):
        """
        Do the change, initiate a transaction but do not complete it.
        """
        orig_netIfaces = dict(self.config['netIfaces'])
        for src, dst in self.src_dst_lst:
            self.config['netIfaces'][dst] = orig_netIfaces[src]
            del self.config['netIfaces'][src]
        save_configuration_initiate_transaction(self.config)

    @staticmethod
    def preup():
        """
        Run the transaction to completion.
        """
        transaction_system_configuration()

    @staticmethod
    def rollback():
        """
        Rollback the transaction (but do nothing if no transaction has
        been just initiated).
        """
        if transaction_just_initiatiated():
            undo_transaction_initiation()


def rename_ethernet_interface(old_name, new_name):
    """
    Rename the @old_name physical interface to @new_name.
    On internal failure, the operation is undone.

    XXX: On external failure (kill -9, power outage), a small time window
    remains where the configuration could be leaved in an inconsistent state.
    """
    # TODO: detect if a previous renaming operation has been interrupted the
    # hard way (kill -9, power failure) and rollback if possible.
    # This will be better placed in an other function.

    udev.rename_persistent_net_rules([(old_name, new_name)], EthernetRenamer)


def swap_ethernet_interfaces(name1, name2):
    """
    Swap ethernet interfaces @name1 and @name2.
    """
    # NOTE: see also rename_ethernet_interface() for various generic comments

    udev.rename_persistent_net_rules([(name1, name2), (name2, name1)], EthernetRenamer)
