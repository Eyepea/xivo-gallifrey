"""Network related routines for XIVO

Copyright (C) 2007-2010  Proformatique

WARNING: Linux specific module, needs /sys/ - also Debian Etch specific module
"""

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

import re
import os
import subprocess
import logging
import socket
import struct
import math

from xivo.UpAllAny import all

from xivo import StreamedLines


log = logging.getLogger("xivo.network") # pylint: disable-msg=C0103


# CONFIG

SYS_CLASS_NET = "/sys/class/net"

# /sys/class/net/<ifname>/carrier tells us if the interface if plugged
CARRIER = "carrier"

IFPLUGD = "/usr/sbin/ifplugd"
IFPLUGD_START = ["/usr/sbin/invoke-rc.d", "ifplugd", "start"]

IFDOWN = "/sbin/ifdown"


# CODE


DECIMAL_SPLIT = re.compile(r'(\d+)').split

def to_int_if_possible(s):
    try:
        return int(s)
    except ValueError:
        return s


def split_alpha_num(s):
    """
    Split the non decimal and the decimal parts of s.
    Don't interpret decimal parts as integers, keep them as string.

    Exemples:
    
    >>> split_alpha_num('wazza42sub10')
    ('wazza', '42', 'sub', '10')
    >>> split_alpha_num('42sub010')
    ('', '42', 'sub', '010')
    >>> split_alpha_num('a42sub')
    ('a', '42', 'sub')
    >>> split_alpha_num('')
    ('',)
    """
    a_n_splitted = DECIMAL_SPLIT(s)
    if len(a_n_splitted) > 1 and a_n_splitted[-1] == '':
        strs = a_n_splitted[:-1]
    else:
        strs = a_n_splitted
    return tuple(strs)


def split_lexdec(lexdec_str):
    """
    Split the non decimal and the decimal parts of lexdec_str
    
    Exemples:
    
    >>> split_lexdec('wazza42sub10')
    ('wazza', 42, 'sub', 10)
    >>> split_lexdec('42sub010')
    ('', 42, 'sub', 10)
    >>> split_lexdec('a42sub')
    ('a', 42, 'sub')
    >>> split_lexdec('')
    ('',)
    """
    return tuple(map(to_int_if_possible, split_alpha_num(lexdec_str)))


def unsplit_lexdec(lexdec_seq):
    """
    Invert of split_lexdec()
    
    WARNING: unsplit_lexdec(split_lexdec("a0001")) == "a1"
    """
    return ''.join(map(str, lexdec_seq))


def cmp_lexdec(x_str, y_str):
    """
    Compare the splitted versions of x_str and y_str
    """
    return cmp(split_lexdec(x_str), split_lexdec(y_str))


def sorted_lst_lexdec(seqof_lexdec_str):
    """
    Sort ifnames according to their split_lexdec() representations
    Return a list.
    NOTES:
    * The sorting is NOT done in place.
    * This function do not strip leading zeros in decimal parts; elements
      are preserved as they are.
    """
    return sorted(seqof_lexdec_str, cmp = cmp_lexdec)


def get_linux_netdev_list():
    """
    Get an unfiltered view of network interfaces as seen by Linux
    """
    return [entry for entry in os.listdir(SYS_CLASS_NET)
            if os.path.isdir(os.path.join(SYS_CLASS_NET, entry))]


def get_filtered_ifnames(ifname_match_func=lambda x:True):
    """
    Return the filtered list of network interfaces
    """
    return filter(ifname_match_func, get_linux_netdev_list())


def is_phy_if(ifname):
    """
    Return True iff ifname seems to be the name of a physical interface
    (not a tagged VLAN).
    """
    return '.' not in ifname


def get_filtered_phys(ifname_match_func=lambda x:True):
    """
    Return the filtered list of network interfaces which are not VLANs
    (the interface name does not contain a '.')
    """
    return [dev for dev in get_filtered_ifnames(ifname_match_func) if is_phy_if(dev)]


def is_interface_plugged(ifname):
    """
    WARNING: Only works on physical interfaces
    """
    return int(file(os.path.join(SYS_CLASS_NET, ifname, CARRIER)).read().strip())


def normalize_ipv4_address(addr):
    """
    Return a canonical string repr of addr (which must be a valid IPv4)
    
    >>> normalize_ipv4_address("1.2.3.077")
    '1.2.3.63'
    >>> normalize_ipv4_address("1.2.3.4")
    '1.2.3.4'
    >>> normalize_ipv4_address("1.2.259")
    '1.2.1.3'
    >>> normalize_ipv4_address("4")
    '0.0.0.4'
    >>> normalize_ipv4_address("1.13")
    '1.0.0.13'
    >>> normalize_ipv4_address("1.16383")
    '1.0.63.255'
    >>> normalize_ipv4_address("0xA.0xa.0x00a.012")
    '10.10.10.10'
    """
    return socket.inet_ntoa(socket.inet_aton(addr))


def is_ipv4_address_valid(addr):
    "True <=> valid"
    try:
        socket.inet_aton(addr)
        return True
    except socket.error:
        return False


def is_mac_address_valid(addr):
    "True <=> valid"
    elements = addr.split(":", 6)
    if len(elements) != 6:
        return False
    for elt in elements:
        try:
            i = int(elt, 16)
        except ValueError:
            return False
        if not (0 <= i < 256):
            return False
    return True


def normalize_mac_address(macaddr):
    """
    input: mac address, with bytes in hexa, ':' separated
    ouput: mac address in format %02X:%02X:%02X:%02X:%02X:%02X

    >>> normalize_mac_address("1a:b:c:d:e:f")
    '1A:0B:0C:0D:0E:0F'
    >>> normalize_mac_address("1A:0B:0C:0D:0E:0F")
    '1A:0B:0C:0D:0E:0F'
    """
    macaddr_split = macaddr.upper().split(':', 6)
    if len(macaddr_split) != 6:
        raise ValueError, "Bad format for mac address " + macaddr
    return ':'.join([('%02X' % int(s, 16)) for s in macaddr_split])


def ipv4_from_macaddr(macaddr, exc_info=True, ifname_match_func=lambda x: True, arping_cmd_list=None, arping_sleep_us=150000):
    """
    Given a mac address, get an IPv4 address for an host living on the
    LAN.  This makes use of the tool "arping".  Of course the remote peer
    must respond to ping broadcasts.  Out of the box, some stupid phones
    from well known stupid and expensive brands don't.
    """
    if arping_cmd_list is None:
        arping_cmd_list = ['sudo', 'arping']
    
    # -r : will only display the IP address on stdout, or nothing
    # -c 1 : ping once
    # -w <xxx> : wait for the answer during <xxx> microsec after the ping
    # -I <netiface> : the network interface to use is <netiface>
    #    -I is an undocumented option like -i but it works
    #    with alias interfaces too
    for iface in sorted_lst_lexdec(get_filtered_ifnames(ifname_match_func)):
        result = None
        try:
            child = subprocess.Popen(arping_cmd_list + ["-r", "-c", "1", "-w", str(arping_sleep_us), '-I', iface, macaddr],
                                     bufsize = 0, stdout = subprocess.PIPE, close_fds = True)
            StreamedLines.makeNonBlocking(child.stdout)
            for (result,) in StreamedLines.rxStreamedLines(fobjs=(child.stdout,), timeout=arping_sleep_us * 10. / 1000000.):
                break
        except Exception:
            result = None
            if exc_info:
                log.exception("ipv4 / macaddr lookup failed for %r", macaddr)
        if result:
            return result.strip()
    return None


def macaddr_from_ipv4(ipv4, exc_info=True, ifname_match_func=lambda x: True, arping_cmd_list=None, arping_sleep_us=150000):
    """
    ipv4_from_macaddr() is indeed a symetrical fonction that can be
    used to retrieve an ipv4 address from a given mac address.  This
    function just call the former.
    
    WARNING: this is of course ipv4_from_macaddr() implementation dependent
    """
    return ipv4_from_macaddr(ipv4,
                             exc_info=exc_info,
                             ifname_match_func=ifname_match_func,
                             arping_cmd_list=arping_cmd_list,
                             arping_sleep_us=arping_sleep_us)


def parse_ipv4(straddr):
    """
    Return an IPv4 address as a 4uple of ints
    @straddr: IPv4 address stored as a string

    >>> parse_ipv4("192.168.0.050")
    (192, 168, 0, 40)
    >>> parse_ipv4("192.168.0.0xA")
    (192, 168, 0, 10)
    >>> parse_ipv4("192.168.0.42")
    (192, 168, 0, 42)
    >>> parse_ipv4("192.168.42")
    (192, 168, 0, 42)
    >>> parse_ipv4("192.168.16383")
    (192, 168, 63, 255)
    >>> parse_ipv4("16383")
    (0, 0, 63, 255)
    >>> parse_ipv4("1")
    (0, 0, 0, 1)
    >>> parse_ipv4("1.13")
    (1, 0, 0, 13)
    """
    return struct.unpack("BBBB", socket.inet_aton(straddr))


def format_ipv4(tupaddr):
    """
    Return a string repr of an IPv4 internal repr
    @tupaddr is an IPv4 address stored as a tuple of 4 ints

    >>> format_ipv4((192, 168, 0, 42))
    '192.168.0.42'
    >>> format_ipv4((192, 168, 63, 255))
    '192.168.63.255'
    >>> format_ipv4((0, 0, 63, 255))
    '0.0.63.255'
    >>> format_ipv4((0, 0, 0, 1))
    '0.0.0.1'
    >>> format_ipv4((1, 0, 0, 13))
    '1.0.0.13'
    """
    return '.'.join(map(str, tupaddr))


def bitmask_to_mask_ipv4(bits):
    """
    Return an IPv4 netmask address as a 4uple of ints
    @bits: Bit integer
    """
    return struct.unpack("BBBB", socket.inet_aton(str((0xFFFFFFFF >> (32 - bits)) << (32 - bits))))


def bitmask_to_mask_ipv6(bits):
    """
    Return an IPv6 netmask address as a 8uple of binary strings
    @bits: Bit integer
    """
    bits = int(bits)

    if not 0 <= bits <= 128:
        raise ValueError("Invalid bitmask: %r" % bits)

    ret = []

    nb = int(math.floor(float(bits) / 16))

    if nb > 0:
        ret.extend(["\xff\xff"] * nb)
        bits -= (16 * nb)

    if bits > 0:
        ret.append(struct.pack("!H", (0xFFFF >> (16 - bits)) << (16 - bits)))

    xlen = len(ret)
    if xlen < 8:
        ret.extend(["\x00\x00"] * (8 - xlen))

    return tuple(ret)


def mask_ipv4(mask, addr):
    """
    Binary AND of IPv4 mask and IPv4 addr
    (mask and addr are 4uple of ints)
    """
    return tuple([m & a for m, a in zip(mask, addr)])


def or_ipv4(mask, addr):
    """
    Binary OR of IPv4 mask and IPv4 addr
    (mask and addr are 4uple of ints)
    """
    return tuple([m | a for m, a in zip(mask, addr)])


def netmask_invert(mask):
    """
    Invert bits in mask
    (mask is 4uple of ints)
    """
    return tuple([m ^ 0xFF for m in mask])


_valid_netmask = frozenset([
    struct.unpack("BBBB", struct.pack(">L", 0xFFFFFFFF ^ ((1 << _m) - 1)))
    for _m in xrange(0, 33)
])
del _m

def plausible_netmask(addr):
    """
    Check that addr (4uple of ints) makes a plausible netmask
    (set bits first, reset bits last)

    >>> plausible_netmask((255, 255, 255, 255))
    True
    >>> plausible_netmask((0, 0, 0, 0))
    True
    >>> plausible_netmask((255, 255, 128, 0))
    True
    >>> plausible_netmask((255, 255, 64, 0))
    False
    """
    return addr in _valid_netmask


# WARNING: the following function does not test the length which must be <= 63
DomainLabelOk = re.compile(r'[a-zA-Z]([-a-zA-Z0-9]*[a-zA-Z0-9])?$').match

def plausible_search_domain(search_domain):
    """
    Return True if the search_domain is suitable for use in the search
    line of /etc/resolv.conf, else False.
    """
    # NOTE: 251 comes from FQDN 255 maxi including label length bytes, we
    # do not want to validate search domain beginning or ending with '.',
    # 255 seems to include the final '\0' length byte, so a FQDN is 253
    # char max.  We remove 2 char so that a one letter label requested and
    # prepended to the search domain results in a FQDN that is not too long
    return search_domain and len(search_domain) <= 251 and \
           all((((len(label) <= 63) and DomainLabelOk(label))
                for label in search_domain.split('.')))


class NetworkOpError(Exception):
    "Error raised on network related operation failures."
    pass


def force_shutdown(phy):
    """
    Remove all VLAN on the network interface @phy, then shutdown it.
    First "ifplugd" is stopped for this interface, then both VLAN removal and
    interface shutdown are done by calling "ifdown".
    
    Unlike /etc/init.d/networking stop, it won't test for mounted network
    filesystems or other resources.  It just shutdown the given interface,
    right when called.
    
    WARNING: This function won't work properly if "ifplugd" or "ifdown" (and
    "ifup") are not used on the system.
    """
    # NOTE: The order in which ifplugd and ifdown are called is very important.
    # If you invert the order, the interface will probably not be completely
    # down (from the p.o.v. of Linux) when the function returns.
    
    try:
        status = subprocess.call([IFPLUGD, "-i", phy, "-k"], close_fds=True)
    except OSError:
        errmsg = "could not invoke ifplugd to kill its %r instance" % phy
        log.exception(errmsg)
        raise NetworkOpError(errmsg)
    if status:
        if status == 6:
            log.warning("%r ifplugd instance seems to have already been stopped", phy)
        else:
            raise NetworkOpError("ifplugd miserably failed while trying to kill instance %r" % phy)
    
    vlans_phy = [vlan for vlan in get_linux_netdev_list() if vlan.startswith(phy + ".")]
    vlans_phy.append(phy)
    
    for vlan in vlans_phy:
        try:
            status = subprocess.call([IFDOWN, vlan], close_fds=True)
        except OSError:
            errmsg = "could not invoke ifdown to shutdown interface %r" % vlan
            log.exception(errmsg)
            raise NetworkOpError(errmsg)
        if status:
            raise NetworkOpError("ifdown miserably failed to shutdown the %r network interface" % vlan)


def ifplugd_start():
    """
    /etc/init.d/ifplugd start
    """
    try:
        status = subprocess.call(IFPLUGD_START, close_fds=True)
    except OSError:
        errmsg = "could not invoke " + ' '.join(IFPLUGD_START)
        log.exception(errmsg)
        raise NetworkOpError(errmsg)
    if status:
        raise NetworkOpError("failure of: " + ' '.join(IFPLUGD_START))


if __name__ == "__main__":
    def _test():
        import doctest
        doctest.testmod()
    _test()
