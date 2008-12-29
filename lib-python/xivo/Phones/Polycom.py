"""Support for Polycom phones for XIVO Configuration

Polycom SoundPoint IP 430 SIP and SoundPoint IP 650 SIP are supported.

Copyright (C) 2007, 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

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
import urllib

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin

class Polycom(PhoneVendorMixin):

    POLYCOM_MODELS = (('spip_301','SPIP301'),
                      ('spip_320','SPIP320'),
                      ('spip_330','SPIP330'),
                      ('spip_430','SPIP430'),
                      ('spip_450','SPIP450'),
                      ('spip_500','SPIP500'),
                      ('spip_501','SPIP501'),
                      ('spip_550','SPIP550'),
                      ('spip_560','SPIP560'),
                      ('spip_600','SPIP600'),
                      ('spip_601','SPIP601'),
                      ('spip_650','SPIP650'),
                      ('spip_670','SPIP670'),
                      ('spip_4000','SSIP4000'),
                      ('spip_6000','SSIP6000'),
                      ('spip_7000','SSIP7000'))

    POLYCOM_SSIP_MODELS = ('ssip_4000', 'ssip_6000', 'ssip_7000')

    POLYCOM_COMMON_HTTP_USER = "Polycom"
    POLYCOM_COMMON_HTTP_PASS = "456"

    POLYCOM_COMMON_DIR = None

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.POLYCOM_COMMON_DIR = os.path.join(cls.TFTPROOT, "Polycom/")

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in self.POLYCOM_SSIP_MODELS \
        and self.phone['model'] not in [x[0] for x in self.POLYCOM_MODELS]:
            raise ValueError, "Unknown Polycom model %r" % self.phone['model']

    def __action(self, command, user, passwd):

        params = urllib.urlencode({'reg.1.server.1.address': "%s " % self.ASTERISK_IPV4})

        try: # XXX: also check return values?
            request = urllib.urlopen("http://%s:%s@%s/form-submit" % (user, passwd, self.phone['ipv4']), params)
            request.close()
        except (IOError, AttributeError), e:
            log.exception(e)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action('reboot', self.POLYCOM_COMMON_HTTP_USER, self.POLYCOM_COMMON_HTTP_PASS)

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action('reboot', self.POLYCOM_COMMON_HTTP_USER, self.POLYCOM_COMMON_HTTP_PASS)

    def __generate(self, provinfo):
        template_file = open(os.path.join(self.TEMPLATES_DIR, "polycom-phone.cfg"))
        template_lines = template_file.readlines()
        template_file.close()

        macaddr = self.phone['macaddr'].replace(":", "").lower()
        tmp_filename = os.path.join(self.POLYCOM_COMMON_DIR, macaddr + "-phone.cfg.tmp")
        cfg_filename = tmp_filename[:-4]

        txt_phone = xivo_config.txtsubst(template_lines,
                { 'user_display_name':  provinfo['name'],
                  'user_phone_ident':   provinfo['ident'],
                  'user_phone_number':  provinfo['number'],
                  'user_phone_passwd':  provinfo['passwd'],
                  'asterisk_ipv4':      self.ASTERISK_IPV4,
                  'ntp_server_ipv4':    self.NTP_SERVER_IPV4,
                },
                cfg_filename)

        tmp_file = open(tmp_filename, "w")
        tmp_file.writelines(txt_phone)
        tmp_file.close()
        os.rename(tmp_filename, cfg_filename)

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

    def do_autoprov(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        self.__generate(provinfo)

    # Introspection entry points

    @classmethod
    def get_phones(cls):
        "Report supported phone models for this vendor."
        return cls.POLYCOM_MODELS

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        # PolycomSoundPointIP-SPIP_430-UA/2.2.0.0047
        # PolycomSoundPointIP-SPIP_650-UA/2.2.0.0047

        if ua[:7] != 'Polycom':
            return None
        model = 'unknown'
        fw = 'unknown'
        ua_splitted = ua.split('/', 1)
        if len(ua_splitted) == 2:
            fw = ua_splitted[1]
            model = ua_splitted[0].split('-')[1].lower()
        return ("polycom", model, fw)

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for line in (
            'subclass "phone-mac-address-prefix" 1:00:04:f2 {\n',
            '    log("class Polycom prefix 1:00:04:f2");\n',
            '    option tftp-server-name "tftp://%s/Polycom";\n' % addresses['bootServer'],
            '}\n',
            '\n'):
            yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        return ()
