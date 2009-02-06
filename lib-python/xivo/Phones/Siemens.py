"""Support for Siemens phones for XIVO Configuration

Siemens Gigaset S675IP are supported.

Copyright (C) 2009  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2009  Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

import os
import logging
import urllib2

from HTMLParser import HTMLParser
from ConfigParser import RawConfigParser
from urllib import urlencode
from cookielib import CookieJar

from xivo import network
from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin


log = logging.getLogger("xivo.Phones.Siemens") # pylint: disable-msg=C0103

class SiemensHTTP:
    
    SECTION_PAGE = (('account0', 'settings_telephony_voip.html', (('id', 0), ('go_profile', 0), ('account_id', 0))),
                    ('account1', 'settings_telephony_voip.html', (('id', 1), ('go_profile', 0), ('account_id', 1))),
                    ('account2', 'settings_telephony_voip.html', (('id', 2), ('go_profile', 0), ('account_id', 2))),
                    ('account3', 'settings_telephony_voip.html', (('id', 3), ('go_profile', 0), ('account_id', 3))),
                    ('account4', 'settings_telephony_voip.html', (('id', 4), ('go_profile', 0), ('account_id', 4))),
                    ('account5', 'settings_telephony_voip.html', (('id', 5), ('go_profile', 0), ('account_id', 5))),
                    ('number_assignment', 'settings_telephony_assignment.html', (('send_6', 0), ('receive_6', 0))),
                    ('network_mailbox', 'settings_telephony_am.html'),
                    ('advanced_settings', 'settings_telephony_advanced.html'),
                    ('services', 'settings_infoservice.html'),
                    ('handsets', 'settings_telephony_tdt.html'),
                    ('miscellaneous', 'settings_admin_special.html'),
                    ('ip_configuration', 'settings_lan.html'),
                    ('audio', 'settings_telephony_audio.html'),)

    def __init__(self, common_dir, common_pin):
        self.common_dir     = common_dir
        self.common_pin     = common_pin
        self.cj             = CookieJar()
        self.opener         = urllib2.build_opener(urllib2.HTTPCookieProcessor(self.cj))

    def request(self, ipv4, page, params=None):
        if not isinstance(page, basestring):
            raise ValueError, "Invalid argument page: '%r'" % page

        if isinstance(params, (dict, list)):
            params = urlencode(params)
        else:
            params = None

        url = "http://%s/%s" % (ipv4, page)
        request = self.opener.open(urllib2.Request(url, params))

        return request

    @staticmethod
    def set_ip_configuration_option(rcp, option, value):
        value = network.parse_ipv4(value)

        rcp.set('ip_configuration', "%s_1" % option, "%03d" % value[0])
        rcp.set('ip_configuration', "%s_2" % option, "%03d" % value[1])
        rcp.set('ip_configuration', "%s_3" % option, "%03d" % value[2])
        rcp.set('ip_configuration', "%s_4" % option, "%03d" % value[3])

    @staticmethod
    def is_ipv4_netmask_valid(netmask):
        return network.is_ipv4_address_valid(netmask) \
        and network.plausible_netmask(network.parse_ipv4(netmask))

    @staticmethod
    def _prepare_ip_configuration(rcp):
        if not rcp.has_option('ip_configuration', 'ip_address_type') \
        or rcp.getint('ip_configuration', 'ip_address_type') != 0:
            return

        options = (('ip_address', False, network.is_ipv4_address_valid),
                   ('subnet_mask', False, SiemensHTTP.is_ipv4_netmask_valid),
                   ('default_gateway', False, network.is_ipv4_address_valid),
                   ('preferred_dns_server', True, network.is_ipv4_address_valid),
                   ('alternate_dns_server', True, network.is_ipv4_address_valid))

        for opt in options:
            if not rcp.has_option('ip_configuration', opt[0]):
                if not opt[1]:
                    log.error("Missing parameter %s in section ip_configuration." % opt[0])
                    return opt[1]
                else:
                    log.info("Missing parameter %s in section ip_configuration." % opt[0])
                    continue

            value = rcp.get('ip_configuration', opt[0])
            rcp.remove_option('ip_configuration', opt[0])

            if opt[1] and len(value) == 0:
                value = "0.0.0.0"
            elif not opt[2](value):
                log.error("Invalid parameter %s in section ip_configuration." % opt[0])
                if not opt[1]:
                    return opt[1]
                else:
                    continue

            SiemensHTTP.set_ip_configuration_option(rcp, opt[0], value)

        return True

    def provi(self, ipv4, model, macaddr):
        model = model.lower()
        macaddr = macaddr.lower().replace(":", "")

        common_file = os.path.join(self.common_dir, model + ".ini")
        phone_file = os.path.join(self.common_dir, model + "-" + macaddr + ".ini")

        if not os.access(common_file, os.R_OK):
            raise IOError, "'%s' isn't a file or isn't readable" % common_file

        self.login(ipv4)

        rcp = RawConfigParser()
        rcp.readfp(open(common_file))

        if os.access(phone_file, os.R_OK):
            rcp.readfp(open(phone_file))

        if rcp.has_option('miscellaneous', 'user_firmware_url') \
        and rcp.has_option('miscellaneous', 'user_firmware_filename'):
            rcp.set('miscellaneous',
                    'user_firmware_url',
                    "%s/%s" % (rcp.get('miscellaneous', 'user_firmware_url').rstrip('/'),
                               rcp.get('miscellaneous', 'user_firmware_filename').lstrip('/')))

            rcp.remove_option('miscellaneous', 'user_firmware_filename')

        if not self._prepare_ip_configuration(rcp):
            rcp.set('ip_configuration', 'ip_address_type', 1)

        for value in self.SECTION_PAGE:
            xlen = len(value)

            if xlen < 2 or value[0] not in rcp.sections():
                continue

            args = rcp.items(value[0])

            if xlen > 2:
                args.extend(value[2])

            request = self.request(ipv4, value[1], args)
            request.close()

        self.logout(ipv4)

    def login(self, ipv4):
        params  = { 'language': 1,
                    'password': self.common_pin}
        request = self.request(ipv4, 'login.html', params)

        if "/login.html" in request.headers.getheaders('ETag'):
            request.close()
            raise LookupError("Not logged in (ip: '%s')" % ipv4)

        request.close()
 
    def logout(self, ipv4):
        request = self.request(ipv4, 'logout.html')
        request.close()
        self.cj.clear_session_cookies()


class SiemensHTMLParser(HTMLParser):
    def __init__(self, searchtag=None, searchattrs=None):
        HTMLParser.__init__(self)
        self.searchtag = searchtag
        self.searchattrs = searchattrs
        self.result = []

    def handle_starttag(self, tag, attrs):
        if self.searchtag is None and self.searchattrs is None:
            self.result.append([tag, attrs])
        elif isinstance(self.searchtag, basestring):
            if self.searchtag != tag:
                return

        if isinstance(self.searchattrs, (tuple, list)):
            for k in self.searchattrs:
                if k not in attrs:
                    return

        self.result.append([tag, attrs])

    def reset(self):
        HTMLParser.reset(self)
        self.result = []


class Siemens(PhoneVendorMixin):

    SIEMENS_MODELS = ('S675IP',)

    SIEMENS_MACADDR_PREFIX = ('1:00:01:e3', '1:00:13:a9')

    SIEMENS_COMMON_PIN = '0000'

    SIEMENS_COMMON_DIR = None

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.SIEMENS_COMMON_DIR = os.path.join(cls.TFTPROOT, "Siemens/")

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'].upper() not in self.SIEMENS_MODELS:
            raise ValueError, "Unknown Siemens model %r" % self.phone['model']

    def __action(self, command, common_dir, common_pin):
        http = SiemensHTTP(common_dir, common_pin)
        html = SiemensHTMLParser('input', [('name', 'use_G729_B'), ('checked', 'checked')])

        http.login(self.phone['ipv4'])
        request = http.request(self.phone['ipv4'], 'settings_telephony_audio.html')

        html.feed(request.read())
        request.close()
        html.close()

        http.logout(self.phone['ipv4'])

        if len(html.result) != 1:
            raise LookupError("Unable to %s the phone. (ip: %s)" % (command, self.phone['ipv4']))

        for k, v in html.result[0][1]:
            if k == 'value':
                g729b = bool(int(v)) is False

        html.reset()

        http.login(self.phone['ipv4'])
        request = http.request(self.phone['ipv4'], 'settings_telephony_audio.html', {'use_G729_B': int(g729b)})
        request.close()
        http.logout(self.phone['ipv4'])

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action("reboot", self.SIEMENS_COMMON_DIR, self.SIEMENS_COMMON_PIN)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action("reboot", self.SIEMENS_COMMON_DIR, self.SIEMENS_COMMON_PIN)

    def __generate(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model'].lower()
        macaddr = self.phone['macaddr'].lower().replace(":", "")
        template_file = open(os.path.join(self.TEMPLATES_DIR, "siemens-" + model + ".ini"))
        template_lines = template_file.readlines()
        tmp_filename = os.path.join(self.SIEMENS_COMMON_DIR, model + "-" + macaddr + ".ini.tmp")
        cfg_filename = tmp_filename[:-4]

        txt = xivo_config.txtsubst(template_lines,
                { 'user_display_name':  provinfo['name'],
                  'user_phone_ident':   provinfo['ident'],
                  'user_phone_number':  provinfo['number'],
                  'user_phone_passwd':  provinfo['passwd'],
                  'asterisk_ipv4':      self.ASTERISK_IPV4,
                  'ntp_server_ipv4':    self.NTP_SERVER_IPV4,
                },
                cfg_filename)
        tmp_file = open(tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(tmp_filename, cfg_filename)

    def __provi(self):
        http = SiemensHTTP(self.SIEMENS_COMMON_DIR, self.SIEMENS_COMMON_PIN)
        http.provi(self.phone['ipv4'], self.phone['model'], self.phone['macaddr'])

    def do_reinitprov(self):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        self.__generate(
                { 'name':   "guest",
                  'ident':  "guest",
                  'number': "guest",
                  'passwd': "guest",
                })
        self.__provi()

    def do_autoprov(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        self.__generate(provinfo)
        self.__provi()

    # Introspection entry points

    @classmethod
    def get_phones(cls):
        "Report supported phone models for this vendor."
        return tuple([(x.lower(), x) for x in cls.SIEMENS_MODELS])

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        # S675IP  021400000000

        ua_splitted = ua.split("  ", 1)

        for x in cls.SIEMENS_MODELS:
            if x == ua_splitted[0]:
                if len(ua_splitted) == 2:
                    fw = ua_splitted[1].strip()
                else:
                    fw = 'unknown'
                return ('siemens', x.lower(), fw)

        return None

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for model in cls.SIEMENS_MODELS:
            for line in (
                'class "Siemens%s" {\n' % model,
                '    match if substring(option vendor-class-identifier, 0, %d) = "%s";\n' % (len(model), model),
                '    log("boot Siemens %s");\n' % model,
                '    execute("/usr/share/pf-xivo-provisioning/bin/dhcpconfig",\n',
                '            "%s",\n' % model,
                '            binary-to-ascii(10, 8, ".", leased-address),\n',
                '            binary-to-ascii(16, 8, ":", suffix(hardware, 6)),\n',
                '            20);\n',
                '}\n',
                '\n'):
                yield line

        for macaddr_prefix in cls.SIEMENS_MACADDR_PREFIX:
            for line in (
                'subclass "phone-mac-address-prefix" %s {\n' % macaddr_prefix,
                '    if not exists vendor-class-identifier {\n',
                '        log("class Siemens prefix %s");\n' % macaddr_prefix,
                '    }\n',
                '}\n',
                '\n'):
                yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        for x in cls.SIEMENS_MODELS:
            yield '        allow members of "Siemens%s";\n' % x
