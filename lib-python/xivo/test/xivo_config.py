#!/usr/bin/python

"""Tests for xivo_config

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

import unittest

from xivo import xivo_config
from xivo import xys

VALID_CONFIGS = [
('base', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans: {}
netIfaces: {}
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

('one_unused_vs', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
    static_002:
        address:     192.168.0.20
        netmask:     255.255.255.0
vlans:
    vs_001:
        0: static_001
    vs_002:
        0: static_002
netIfaces:
    eth0: vs_001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

('vlan_high', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        4094: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

]

INVALID_CONFIGS = [

('i_miss_you', """
"""),

('hollywood', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.256
        netmask:     255.255.255.0
vlans: {}
netIfaces: {}
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.199
            - 192.168.0.100
"""),

('outer_space', """
resolvConf: {}
ipConfs:
  static_001:
    address:     192.168.0.200
    netmask:     255.255.255.0
vlans: {}
netIfaces: {}
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
outerSpace:
"""),

('duplicated_nameserver', """
resolvConf:
    nameservers:
    - 192.168.0.50
    - 192.168.0.50
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
        broadcast:   192.168.0.255
        gateway:     192.168.0.254
        mtu:         1500
vlans: {}
netIfaces: {}
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

('duplicated_referenced_network', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
        broadcast:   192.168.0.255
        gateway:     192.168.0.254
    static_002:
        address:     192.168.0.12
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
        1: static_002
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

('services_referenced_net_does_not_exist', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_002
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

('vlans_referenced_net_does_not_exist', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_002
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

('netIfaces_referenced_vs_does_not_exist', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0002
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

('service_voipServer_is_bcast', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.255
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

('service_router_is_bcast', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
            router: 192.168.0.255
"""),

('service_router_out_of_network', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
            router: 192.168.1.12
"""),

('service_voip_inverted_range', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.199
            - 192.168.0.100
"""),

('service_overlapping_ranges_1', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
            alienRange:
            - 192.168.0.199
            - 192.168.0.210
"""),

('service_overlapping_ranges_2', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
            alienRange:
            - 192.168.0.50
            - 192.168.0.100
"""),

('service_overlapping_ranges_3', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
            alienRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

('service_overlapping_ranges_4', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
            alienRange:
            - 192.168.0.90
            - 192.168.0.210
"""),

('implicit_bcast_in_voip_range', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.250
            - 192.168.0.255
"""),

('explicit_bcast_in_voip_range', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
        broadcast:   192.168.0.12
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.12
            - 192.168.0.20
"""),

('implicit_bcast_in_alien_range', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
            alienRange:
            - 192.168.0.250
            - 192.168.0.255
"""),

('explicit_bcast_in_alien_range', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
        broadcast:   192.168.0.12
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
            alienRange:
            - 192.168.0.12
            - 192.168.0.20
"""),

('voipServer_in_voip_range', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.210
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.210
            voipRange:
            - 192.168.0.100
            - 192.168.0.200
"""),

('address_in_voip_range', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.210
            bootServer: 192.168.0.210
            voipRange:
            - 192.168.0.100
            - 192.168.0.200
"""),

('vlan_too_high', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        4095: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

('vlan_too_low', """
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        -1: static_001
netIfaces:
    eth0: vs_0001
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
"""),

]


# REM: schema follow...
"""!~plausible_configuration
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
"""

import logging


logging.basicConfig(level=logging.CRITICAL)


class TestXysXivoConfigValidation(unittest.TestCase):

    for p, (test_title, src_conf) in enumerate(VALID_CONFIGS):
        exec \
         """def test_valid_%s(self):
                conf = xivo_config.load_configuration(VALID_CONFIGS[%d][1])
                self.assertEqual(isinstance(conf, dict), True)""" % (test_title, p)

    for p, (test_title, src_conf) in enumerate(INVALID_CONFIGS):
        exec \
         """def test_invalid_%s(self):
                self.assertRaises(xivo_config.InvalidConfigurationError, xivo_config.load_configuration, INVALID_CONFIGS[%d][1])""" % (test_title, p)


unittest.main()
