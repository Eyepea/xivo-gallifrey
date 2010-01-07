"""Mac Address <-> IPv4 Resolver using the dhcpd.leases file.

Copyright (C) 2008-2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008-2010  Proformatique

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

import time

from xivo.network import is_ipv4_address_valid, normalize_ipv4_address, \
                         is_mac_address_valid, normalize_mac_address


DHCPD_LEASES_FILENAME = "/var/lib/dhcp3/dhcpd.leases"


def match_remain_and_strip_semicolon(s, start):
    """
    If @s startswith @start, returns @s with @start (and any additionnal
    spaces) left stripped and a potential semicolon right stripped.
    Else returns None.
    """
    if s.startswith(start):
        remain = s[len(start):].strip()
        if len(remain) and remain[-1] == ";":
            return remain[:-1]
        else:
            return remain
    else:
        return None


NOWHERE = object()
UNKNOWN = object()
LEASE = object()


class LeaseEntry(object):
    """
    This class defines the internal representation of an entry of the
    dhcpd.leases(5) file.
    """
    
    __slots__ = ('ipv4', 'starts', 'ends', 'tstp', 'binding_state', 'next_binding_state', 'macaddr')
    
    def __init__(self, ipv4):
        self.ipv4 = normalize_ipv4_address(ipv4)
    
    def _set_attr_time(self, attr, stime):
        """
        Common code for .set_starts(), .set_ends(), .set_tstp()
        """
        splitted_time = stime.split(None, 3)
        if len(splitted_time) != 3:
            # TODO log / count / etc
            return
        Ymd_HMS = " ".join(splitted_time[1:3])
        try:
            tup_utc_time = time.strptime(Ymd_HMS, "%Y/%m/%d %H:%M:%S")[0:6]
            setattr(self, attr, tup_utc_time)
        except ValueError:
            pass # TODO log / count / etc
        
    def set_starts(self, s):
        """
        Called when a 'starts' statement is read from the leases file.
        """
        self._set_attr_time('starts', s)
    
    def set_ends(self, s):
        """
        Called when an 'ends' statement is read from the leases file.
        """
        self._set_attr_time('ends', s)
    
    def set_tstp(self, s):
        """
        Called when a 'tstp' statement is read from the leases file.
        """
        self._set_attr_time('tstp', s)
    
    def set_binding_state(self, s):
        """
        Called when a 'binding state' statement is read from the leases file.
        """
        self.binding_state = s # pylint: disable-msg=W0201
    
    def set_next_binding_state(self, s):
        """
        Called when a 'next binding state' statement is read from the leases file.
        """
        self.next_binding_state = s # pylint: disable-msg=W0201
    
    def set_hardware_ethernet(self, s):
        """
        Called when a 'hardware ethernet' statement is read from the leases file.
        """
        self.macaddr = normalize_mac_address(s).lower() # pylint: disable-msg=W0201
    
    def __repr__(self):
        return "<LeaseEntry %s >" % " ".join(("%s:%r" % (slot, getattr(self, slot)) for slot in self.__slots__ if hasattr(self, slot)))


# TODO validation here?
LEASE_ATTRS = [
    ("starts ", LeaseEntry.set_starts),
    ("ends ", LeaseEntry.set_ends),
    ("tstp ", LeaseEntry.set_tstp),
    ("binding state ", LeaseEntry.set_binding_state),
    ("next binding state ", LeaseEntry.set_next_binding_state),
    ("hardware ethernet ", LeaseEntry.set_hardware_ethernet),
]


def load(filename):
    """
    Load a dhcpd.leases file.
    
    WARNING: The parsing is _NOT_ done the same way dhcpd does it, for
    simplicity reasons.  Anyway it should work.  Maybe.
    
    Returns (by_ipv4, by_macaddr) where both are dictionaries in which values
    are LeaseEntry instances and keys are IPv4 in @by_ipv4 and mac address in
    @by_macaddr.
    
    TODO: check validity periods
    """
    by_ipv4 = {}
    by_macaddr = {}
    
    state = NOWHERE
    
    for line in file(filename):
        
        line = line.rstrip()
        fully_stripped_line = line.lstrip()
        
        if (not fully_stripped_line) or fully_stripped_line[0] == '#':
            continue
        
        if state is NOWHERE:
            if line.startswith("lease "):
                lease_header = line.split()
                if len(lease_header) == 3 and is_ipv4_address_valid(lease_header[1]) and lease_header[2] == "{":
                    current = LeaseEntry(lease_header[1])
                    state = LEASE
                else:
                    # XXX log that our parser is too dumb to correctly parse the file
                    state = UNKNOWN
        elif state is UNKNOWN:
            if fully_stripped_line == '}':
                state = NOWHERE
            else:
                pass # XXX log
        elif state is LEASE:
            for start, method in LEASE_ATTRS:
                remain = match_remain_and_strip_semicolon(fully_stripped_line, start)
                if remain is not None:
                    method(current, remain)
                    break
            else:
                if fully_stripped_line == '}':
                    if current.ipv4 in by_ipv4:
                        macaddr_to_rm = by_ipv4[current.ipv4].macaddr
                        del by_ipv4[current.ipv4]
                        del by_macaddr[macaddr_to_rm]
                    if is_mac_address_valid(getattr(current, 'macaddr', "")) \
                    and getattr(current, 'binding_state', None) == 'active':
                        if current.macaddr in by_macaddr:
                            ipv4_to_rm = by_macaddr[current.macaddr].ipv4
                            del by_ipv4[ipv4_to_rm]
                            del by_macaddr[current.macaddr]
                        by_ipv4[current.ipv4] = current
                        by_macaddr[current.macaddr] = current
                    else:
                        # log that this lease entry is useless for us if without a mac address or binding state isn't active
                        pass
                    del current
                    state = NOWHERE
                else:
                    # XXX count number of unknown lease lines
                    pass
    
    return by_ipv4, by_macaddr


def macaddr_from_ipv4(ipv4):
    """
    Given @ipv4, returns the @macaddr or None if unknown / lookup failed, etc.
    """
    if not is_ipv4_address_valid(ipv4):
        return None
    ipv4 = normalize_ipv4_address(ipv4)
    by_ipv4 = load(DHCPD_LEASES_FILENAME)[0]
    if ipv4 not in by_ipv4:
        return None
    else:
        return by_ipv4[ipv4].macaddr


def ipv4_from_macaddr(macaddr):
    """
    Given @macaddr, returns the @ipv4 or None if unknown / lookup failed, etc.
    """
    if not is_mac_address_valid(macaddr):
        return None
    macaddr = normalize_mac_address(macaddr).lower()
    by_macaddr = load(DHCPD_LEASES_FILENAME)[1]
    if macaddr not in by_macaddr:
        return None
    else:
        return by_macaddr[macaddr].ipv4
