"""Support for Siemens phones for XIVO Configuration

Siemens Gigaset C470IP and S675IP are supported.

Copyright (C) 2009-2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2009-2010  Proformatique

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
import sha
import logging
import urllib2
import socket
import re

from time import sleep
from distutils import version
from HTMLParser import HTMLParser
from ConfigParser import RawConfigParser
from urllib import urlencode
from cookielib import CookieJar

from xivo import network
from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.Siemens") # pylint: disable-msg=C0103


class SiemensHTTP:

    SECTION_PAGE = (('account0', 'settings_telephony_voip.html', (('id', 0), ('go_profile', 0), ('account_id', 0))),
                    ('account1', 'settings_telephony_voip.html', (('id', 1), ('go_profile', 0), ('account_id', 1))),
                    ('account2', 'settings_telephony_voip.html', (('id', 2), ('go_profile', 0), ('account_id', 2))),
                    ('account3', 'settings_telephony_voip.html', (('id', 3), ('go_profile', 0), ('account_id', 3))),
                    ('account4', 'settings_telephony_voip.html', (('id', 4), ('go_profile', 0), ('account_id', 4))),
                    ('account5', 'settings_telephony_voip.html', (('id', 5), ('go_profile', 0), ('account_id', 5))),
                    ('number_assignment', 'settings_telephony_assignment.html', (('send_6', 0), ('receive_6', 0))),
                    ('network_mailbox', 'settings_telephony_am.html', ()),
                    ('advanced_settings', 'settings_telephony_advanced.html', ()),
                    ('services', 'settings_infoservice.html', ()),
                    ('handsets', 'settings_telephony_tdt.html', ()),
                    ('miscellaneous', 'settings_admin_special.html', ()),
                    ('ip_configuration', 'settings_lan.html', ()),
                    ('audio', 'settings_telephony_audio.html', ()))

    RE_ACC_GIGASET = re.compile('^\s*lines\[6\]\[4\]\s*=\s*2\s*;').match

    def __init__(self, common_dir, common_pin):
        self.common_dir     = common_dir
        self.common_pin     = common_pin
        self.cj             = CookieJar()
        self.opener         = urllib2.build_opener(urllib2.HTTPCookieProcessor(self.cj))

    def request(self, host, page, params=None, timeout=None):
        "Send a HTTP request to phone."

        if params is not None:
            params = urlencode(params)

        url = "http://%s/%s" % (host, page)

        log.info("HTTP Request: %r", url)

        deftimeout = socket.getdefaulttimeout()

        if timeout is not None:
            socket.setdefaulttimeout(timeout)

        res = self.opener.open(urllib2.Request(url, params))

        if timeout is not None:
            socket.setdefaulttimeout(deftimeout)

        return res

    @staticmethod
    def set_ip_configuration_option(rcp, option, value):
        "Set IP configuration for network options."
        value = network.parse_ipv4(value)

        rcp.set('ip_configuration', "%s_1" % option, "%03d" % value[0])
        rcp.set('ip_configuration', "%s_2" % option, "%03d" % value[1])
        rcp.set('ip_configuration', "%s_3" % option, "%03d" % value[2])
        rcp.set('ip_configuration', "%s_4" % option, "%03d" % value[3])

    @staticmethod
    def is_ipv4_netmask_valid(netmask):
        "Verify if it is a valid netmask."
        return network.is_ipv4_address_valid(netmask) \
               and network.plausible_netmask(network.parse_ipv4(netmask))

    @staticmethod
    def _prepare_ip_configuration(rcp):
        "Prepare and verify ip configuration for network options."
        if not rcp.has_option('ip_configuration', 'ip_address_type') \
           or rcp.get('ip_configuration', 'ip_address_type') != '0':
            return False

        options = (('ip_address', False, network.is_ipv4_address_valid),
                   ('subnet_mask', False, SiemensHTTP.is_ipv4_netmask_valid),
                   ('default_gateway', False, network.is_ipv4_address_valid),
                   ('preferred_dns_server', True, network.is_ipv4_address_valid),
                   ('alternate_dns_server', True, network.is_ipv4_address_valid))

        for param, optional, validate in options:
            if not rcp.has_option('ip_configuration', param):
                if not optional:
                    log.error("Missing parameter %s in section ip_configuration.", param)
                    return optional
                else:
                    log.info("Missing parameter %s in section ip_configuration.", param)
                    continue

            value = rcp.get('ip_configuration', param)
            rcp.remove_option('ip_configuration', param)

            if optional and len(value) == 0:
                value = "0.0.0.0"
            elif not validate(value):
                log.error("Invalid parameter %s in section ip_configuration.", param)
                if not optional:
                    return optional
                else:
                    continue

            SiemensHTTP.set_ip_configuration_option(rcp, param, value)

        return True

    def provi(self, host, model, macaddr):
        "Provisioning from web-interface."
        rcp = Siemens.get_config(self.common_dir, model, macaddr)

        if rcp.has_option('miscellaneous', 'user_firmware_url') \
           and rcp.has_option('miscellaneous', 'user_firmware_filename'):
            rcp.set('miscellaneous',
                    'user_firmware_url',
                    "%s/%s" % (rcp.get('miscellaneous', 'user_firmware_url').rstrip('/'),
                               rcp.get('miscellaneous', 'user_firmware_filename').lstrip('/')))

            rcp.remove_option('miscellaneous', 'user_firmware_filename')

        if not self._prepare_ip_configuration(rcp):
            rcp.set('ip_configuration', 'ip_address_type', 1)

        try:
            self.login(host)
 
            # Disable Gigaset account
            request = self.request(host, 'scripts/settings_telephony_voip_multi.js')
 
            for line in request.readlines():
                if self.RE_ACC_GIGASET(line):
                    request.close()
                    request = self.request(host, 'settings_telephony_voip_multi.html', {'account_id': '6'})
                    break
 
            request.close()
 
            for section, page, extend in self.SECTION_PAGE:
                if section not in rcp.sections():
                    continue
 
                args = rcp.items(section)
                args.extend(extend)

                request = self.request(host, page, args)
                request.close()
        finally:
            try:
                self.logout(host)
            except Exception, e: # pylint: disable-msg=W0703
                log.exception(str(e))
                pass

    def login(self, host):
        "Login from web-interface."
        params  = {'language':  1,
                   'password':  self.common_pin}

        request = self.request(host, 'login.html', params)

        if "/login.html" in request.headers.getheaders('ETag'):
            request.close()
            raise LookupError, "Not logged in (ip: %s)" % host

        request.close()

    def logout(self, host):
        "Logout from web-interface."
        request = self.request(host, 'logout.html')
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


class SiemensHTMLParserTD(HTMLParser):
    def __init__(self, searchattrs=None):
        HTMLParser.__init__(self)
        self.searchtag = 'td'
        self.searchattrs = searchattrs
        self.flagdata = 0
        self.result = []

    def handle_starttag(self, tag, attrs):
        if isinstance(self.searchtag, basestring):
            if self.searchtag != tag:
                return

        if isinstance(self.searchattrs, (tuple, list)):
            for k in self.searchattrs:
                if k not in attrs:
                    return

        self.flagdata = 1

    def handle_endtag(self, tag):
        if isinstance(self.searchtag, basestring):
            if self.searchtag != tag:
                return

        self.flagdata = 0

    def handle_data(self, data):
        if self.flagdata:
            self.result.append(data)

    def reset(self):
        HTMLParser.reset(self)
        self.result = []


class Siemens(PhoneVendorMixin):

    SIEMENS_MODELS          = ('C470IP', 'S675IP')
    SIEMENS_MACADDR_PREFIX  = ('1:00:01:e3', '1:00:13:a9', '1:00:21:04')
    SIEMENS_COMMON_PIN      = '0000'
    SIEMENS_FIRMWARE        = '022140000000'

    SIEMENS_COMMON_DIR = None

    RE_FWDL_STATUS      = re.compile('^\s*var\s*status\s*=\s*(\d+)\s*;').match
    RE_DISCOVER_MODEL   = re.compile('^\s*getCurrentNavigationID\s*\(\s*[\'"]\s*([a-zA-Z0-9_\.\-]+)\s*[\'"]\s*\)\s*;').match

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.SIEMENS_COMMON_DIR = os.path.join(cls.TFTPROOT, "Siemens/")

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'].upper() not in self.SIEMENS_MODELS:
            raise ValueError, "Unknown Siemens model %r" % self.phone['model']

    @staticmethod
    def get_config_filename(model, macaddr):
        "Get configuration filename."
        model = model.lower()
        macaddr = macaddr.replace(':', '').lower()

        return ("%s.ini" % model, "%s-%s.ini" % (model, macaddr))

    @staticmethod
    def get_config(common_dir, model, macaddr):
        "Get phone configuration."

        common_file, phone_file = Siemens.get_config_filename(model, macaddr)

        rcp = RawConfigParser()
        rcp.readfp(open(os.path.join(common_dir, common_file)))

        phone_file = os.path.join(common_dir, phone_file)

        if os.access(phone_file, os.R_OK):
            rcp.readfp(open(phone_file))

        return rcp

    def discover_model(self, common_dir, common_pin, host):
        http = SiemensHTTP(common_dir, common_pin)

        try:
            request = http.request(host, 'scripts/navnodes.js')
 
            for line in request.readlines():
                m = self.RE_DISCOVER_MODEL(line)

                if not m:
                    continue

                model = m.group(1).strip().upper()

                if model in self.SIEMENS_MODELS:
                    request.close()
                    return model
 
            request.close()
        except Exception, e: # pylint: disable-msg=W0703
            log.exception(str(e))

    def __action(self, command, common_dir, common_pin):
        http = SiemensHTTP(common_dir, common_pin)
        html = SiemensHTMLParser('input', [('name', 'use_G729_B'), ('checked', 'checked')])

        try:
            http.login(self.phone['ipv4'])
            request = http.request(self.phone['ipv4'], 'settings_telephony_audio.html')
 
            html.feed(request.read())
            request.close()
            html.close()
 
            if len(html.result) != 1:
                raise LookupError, "Unable to %s the phone. (ip: %s)" % (command, self.phone['ipv4'])
 
            for k, v in html.result[0][1]:
                if k == 'value':
                    g729b = (int(v) == 0)
 
            html.reset()
 
            request = http.request(self.phone['ipv4'], 'settings_telephony_audio.html', {'use_G729_B': int(g729b)})
            request.close()
        finally:
            http.logout(self.phone['ipv4'])

    def do_reinit(self, force=False):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        if not self.do_upgradefw(False) and force:
            self.__action('reboot', self.SIEMENS_COMMON_DIR, self.SIEMENS_COMMON_PIN)

    def do_reboot(self, force=False):
        "Entry point to send the reboot command to the phone."
        if not self.do_upgradefw(False) and force:
            self.__action('reboot', self.SIEMENS_COMMON_DIR, self.SIEMENS_COMMON_PIN)

    def do_upgradefw(self, force=True):
        "Entry point to send the firmware upgrade command to the phone."

        if self.phone.get('from') != 'dhcp':
            model = self.phone['model']
        else:
            model = self.discover_model(self.SIEMENS_COMMON_DIR,
                                        self.SIEMENS_COMMON_PIN,
                                        self.phone['ipv4'])
            if not model:
                model = self.phone['model']

        rcp = Siemens.get_config(self.SIEMENS_COMMON_DIR, model, self.phone['macaddr'])

        if not force:
            if self.phone.get('firmware') \
               and self.phone['firmware'] != 'unknown' \
               and version.LooseVersion(self.SIEMENS_FIRMWARE) <= version.LooseVersion(self.phone['firmware']):
                return False
            elif not rcp.has_option('miscellaneous', 'automatic_upgradefw') \
                 or rcp.get('miscellaneous', 'automatic_upgradefw') == '0':
                return False

        params = {'execute_fw_download': '1'}

        if rcp.has_option('miscellaneous', 'user_firmware_url') \
           and rcp.has_option('miscellaneous', 'user_firmware_filename'):
            params['user_firmware_url'] = "%s/%s" % (rcp.get('miscellaneous', 'user_firmware_url').rstrip('/'),
                                                     rcp.get('miscellaneous', 'user_firmware_filename').lstrip('/'))
        elif rcp.has_option('miscellaneous', 'data_server'):
            params['data_server'] = rcp.get('miscellaneous', 'data_server')
        else:
            log.info("Unable to find a firmware update server")
            return False

        http = SiemensHTTP(self.SIEMENS_COMMON_DIR, self.SIEMENS_COMMON_PIN)
        http.login(self.phone['ipv4'])

        request = None
        html = SiemensHTMLParserTD()
        ret = False

        try:
            try:
                request = http.request(self.phone['ipv4'], 'status_device.html')

                html.feed(request.read())
                request.close()
                html.close()

                flagver = 0
                phonefwversion = None

                for k in html.result:
                    if k.lower().find("firmware version") == 0:
                        flagver = 1
                    elif flagver == 1:
                        phonefwversion = k.strip()
                        if version.LooseVersion(self.SIEMENS_FIRMWARE) <= version.LooseVersion(phonefwversion):
                            flagver = 0
                        break
                    else:
                        flagver = 0

                html.reset()

                if flagver != 1:
                    if phonefwversion is None:
                        raise LookupError, "Unable to get phone firmware version"
                    else:
                        raise ValueError, "Unable to upgrade: Phone firmware is greater than " \
                                          "or equal to current firmware. (current: %r, phone: %r)" \
                                          % (self.SIEMENS_FIRMWARE, phonefwversion)

                request = http.request(self.phone['ipv4'], 'settings_admin_special.html', params)
                if "/status.html" not in request.headers.getheaders('ETag'):
                    raise LookupError, "Unable to upgrade: settings_admin_special.html (ip: %s)" % self.phone['ipv4']
                else:
                    request.close()

                loop = True

                while loop:
                    request = http.request(self.phone['ipv4'], 'status.html')
                    for line in request.readlines():
                        match = self.RE_FWDL_STATUS(line) 
                        if not match:
                            continue
                        elif match.group(1) == '0':
                            sleep(8)
                            request.close()
                            break
                        else:
                            loop = False
                            request.close()
                            break
                    else:
                        request.close()
                        break

                if "/status.html" not in request.headers.getheaders('ETag'):
                    raise LookupError, "Unable to upgrade: status.html (ip: %s)" % self.phone['ipv4']
                else:
                    request.close()

                try:
                    # Only accessible when it is possible to upgrade.
                    request = http.request(self.phone['ipv4'], 'executefwdownload.html')
                    request.read()
                except urllib2.HTTPError:
                    raise LookupError, "Unable to upgrade: not permitted. (ip: %s)" % self.phone['ipv4']
            except Exception, e: # pylint: disable-msg=W0703
                log.exception(str(e))
            else:
                ret = True
        finally:
            if request:
                request.close()

            http.logout(self.phone['ipv4'])

        return ret

    def __generate(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        common_file, phone_file = Siemens.get_config_filename(self.phone['model'], self.phone['macaddr'])

        try:
            template_specific_path = os.path.join(self.SIEMENS_COMMON_DIR,
                                                  phone_file.split('-', 1)[1][:-4] + "-template.cfg")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            template_common_path = os.path.join(self.SIEMENS_COMMON_DIR, "templates", "siemens-%s" % common_file)

            if not os.access(template_common_path, os.R_OK):
                template_common_path = os.path.join(self.TEMPLATES_DIR, "siemens-%s" % common_file)

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      template_common_path)
            template_file = open(template_common_path)

        template_lines = template_file.readlines()
        tmp_filename = os.path.join(self.SIEMENS_COMMON_DIR, "%s.tmp" % phone_file)
        cfg_filename = tmp_filename[:-4]

        provinfo['subscribemwi'] = str(int(bool(int(provinfo.get('subscribemwi', 0)))))

        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'config_sha1sum': provinfo['sha1sum'],
                    },
                    format_extension=clean_extension),
                cfg_filename,
                'utf8')

        tmp_file = open(tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(tmp_filename, cfg_filename)

    def __get_config_sha1sum(self):
        "Get sha1sum value from configuration."
        rcp = Siemens.get_config(self.SIEMENS_COMMON_DIR, self.phone['model'], self.phone['macaddr'])

        if rcp.has_option('miscellaneous', 'config_sha1sum'):
            sha1sum = rcp.get('miscellaneous', 'config_sha1sum')
        else:
            sha1sum = '1'

        return sha1sum

    def __verify_need_provi(self, sha1sum):
        "Verify if the configuration changed before do a provisioning."
        if sha1sum == '0':
            return

        rcp = Siemens.get_config(self.SIEMENS_COMMON_DIR, self.phone['model'], self.phone['macaddr'])

        if rcp.has_option('miscellaneous', 'config_sha1sum'):
            rcp.remove_option('miscellaneous', 'config_sha1sum')

        tmp = os.tmpfile()
        rcp.write(tmp)
        tmp.seek(0)
        new_sha1sum = sha.new(tmp.read()).hexdigest()
        tmp.close()

        if sha1sum == new_sha1sum:
            return

        return new_sha1sum

    def __provi(self):
        "Provisioning the phone."
        if self.phone.get('ipv4'):
            http = SiemensHTTP(self.SIEMENS_COMMON_DIR, self.SIEMENS_COMMON_PIN)
            http.provi(self.phone['ipv4'], self.phone['model'], self.phone['macaddr'])

    def __action_prov(self, provinfo):
        if provinfo['sha1sum'] == '0' or not self.phone.get('ipv4'):
            return

        http = SiemensHTTP(self.SIEMENS_COMMON_DIR, self.SIEMENS_COMMON_PIN)
        try:
            request = http.request(self.phone['ipv4'], 'login.html', None, 10)
            request.close()
        except urllib2.URLError, e:
            log.exception("Unable to connect to host. (host: %r, error: %r)", self.phone['ipv4'], str(e))
            return

        self.__generate(provinfo)

        sha1sum = self.__verify_need_provi(provinfo['sha1sum'])

        if sha1sum:
            provinfo['sha1sum'] = sha1sum
            self.__generate(provinfo)
        elif self.phone.get('from') == 'dhcp':
            return

        regenerate = False

        try:
            self.__provi()

            if self.phone.get('from') == 'dhcp' and self.do_upgradefw(False):
                regenerate = True
        except Exception, e: # pylint: disable-msg=W0703
            regenerate = True
            log.exception(str(e))

        if regenerate:
            log.debug("Trying to force provisioning on next reboot.")
            provinfo['sha1sum'] = '1'
            self.__generate(provinfo)

    def do_reinitprov(self, provinfo):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        provinfo['sha1sum'] = self.__get_config_sha1sum()

        self.__action_prov(provinfo)

    def do_autoprov(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        provinfo['sha1sum'] = self.__get_config_sha1sum()

        self.__action_prov(provinfo)

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
        # C470IP021840000000

        for x in cls.SIEMENS_MODELS:
            if ua.startswith(x):
                fw = ua[len(x):].strip()

                if len(fw) == 0:
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
                '            "-w30",\n',
                '            "-f",\n',
                '            "%s",\n' % model,
                '            binary-to-ascii(10, 8, ".", leased-address),\n',
                '            binary-to-ascii(16, 8, ":", suffix(hardware, 6)));\n',
                '}\n',
                '\n'):
                yield line

        for macaddr_prefix in cls.SIEMENS_MACADDR_PREFIX:
            for line in (
                'subclass "phone-mac-address-prefix" %s {\n' % macaddr_prefix,
                '    if not exists vendor-class-identifier {\n',
                '        log("class Siemens prefix %s");\n' % macaddr_prefix,
                '        execute("/usr/share/pf-xivo-provisioning/bin/dhcpconfig",\n',
                '                "-w30",\n',
                '                "-f",\n',
                '                "-u",\n',
                '                "S675IP",\n', # XXX: method do_upgradefw try to discover phone model.
                '                binary-to-ascii(10, 8, ".", leased-address),\n',
                '                binary-to-ascii(16, 8, ":", suffix(hardware, 6)));\n',
                '    }\n',
                '}\n',
                '\n'):
                yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        for x in cls.SIEMENS_MODELS:
            yield '        allow members of "Siemens%s";\n' % x
