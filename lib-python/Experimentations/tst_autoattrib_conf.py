import yaml
from xivo import xivo_config

conf = xivo_config.load_configuration("""
resolvConf: {}
ipConfs:
    static_001:
        address:     192.168.0.200
        netmask:     255.255.255.0
        broadcast:   192.168.0.255
        gateway:     192.168.0.254
    static_002:
        address:     192.168.0.200
        netmask:     255.255.255.0
    static_003:
        address:     192.168.1.200
        netmask:     255.255.255.0
vlans:
    vs_0001:
        0: void
netIfaces:
    eth0: void
    eth1: void
services:
    voip:
        ipConf: static_001
        addresses:
            voipServer: 192.168.0.200
            bootServer: 192.168.0.200
            voipRange:
            - 192.168.0.100
            - 192.168.0.199
""")

#import tracer
#import time
#tracer.enable_tofp(open("traces.%s" % time.time(), 'w'))

print xivo_config.autoattrib_conf(conf)
print
print yaml.dump(conf, default_flow_style=False, indent=4)
