"""Support for Polycom phones for XIVO Configuration

Polycom SoundPoint IP 430 SIP and SoundPoint IP 650 SIP are supported.

Copyright (C) 2007-2010  Proformatique

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

import os
import logging
import urllib
import time

from xml.sax.saxutils import escape

from xivo import tzinform
from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.Polycom") # pylint: disable-msg=C0103


class Polycom(PhoneVendorMixin):

    POLYCOM_MODELS = (('spip_301', 'SPIP301'),
                      ('spip_320', 'SPIP320'),
                      ('spip_321', 'SPIP321'),
                      ('spip_330', 'SPIP330'),
                      ('spip_331', 'SPIP331'),
                      ('spip_335', 'SPIP335'),
                      ('spip_430', 'SPIP430'),
                      ('spip_450', 'SPIP450'),
                      ('spip_501', 'SPIP501'),
                      ('spip_550', 'SPIP550'),
                      ('spip_560', 'SPIP560'),
                      ('spip_600', 'SPIP600'),
                      ('spip_601', 'SPIP601'),
                      ('spip_650', 'SPIP650'),
                      ('spip_670', 'SPIP670'),
                      ('spip_4000', 'SSIP4000'),
                      ('spip_5000', 'SSIP5000'),
                      ('spip_6000', 'SSIP6000'),
                      ('spip_7000', 'SSIP7000'))

    POLYCOM_SSIP_MODELS = ('ssip_4000', 'spip_5000', 'ssip_6000', 'ssip_7000')

    POLYCOM_COMMON_HTTP_USER = "Polycom"
    POLYCOM_COMMON_HTTP_PASS = "456"

    POLYCOM_COMMON_DIR = None
    
    POLYCOM_LOCALES = {
        'de_DE': 'German_Germany',
        'en_US': 'English_United_States',
        'es_ES': 'Spanish_Spain',
        'fr_FR': 'French_France',
        'fr_CA': 'French_France',
    }

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

    @staticmethod
    def xml_escape(data):
        if data is None:
            return ""
        elif not isinstance(data, basestring):
            return str(data)

        return escape(data)

    def __action(self, command, user, passwd):
        # For the phone to reboot, its configuration must change, so we set a
        # different value for the inexistent foobar attribute every time
        params = urllib.urlencode({'foobar': time.time()})

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
        macaddr = self.phone['macaddr'].replace(":", "").lower()

        try:
            template_specific_path = os.path.join(self.POLYCOM_COMMON_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            template_common_path = os.path.join(self.POLYCOM_COMMON_DIR, "templates", "polycom-user.cfg")

            if not os.access(template_common_path, os.R_OK):
                template_common_path = os.path.join(self.TEMPLATES_DIR, "polycom-user.cfg")

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      template_common_path)
            template_file = open(template_common_path)

        template_lines = template_file.readlines()
        template_file.close()

        tmp_filename = os.path.join(self.POLYCOM_COMMON_DIR, macaddr + "-user.cfg.tmp")
        cfg_filename = tmp_filename[:-4]

        if bool(int(provinfo.get('subscribemwi', 0))):
            provinfo['vmailaddr'] = "%s@%s" % (provinfo['number'], self.ASTERISK_IPV4)
        else:
            provinfo['vmailaddr'] = ""
        
        if 'language' in provinfo and provinfo['language'] in self.POLYCOM_LOCALES:
            locale = provinfo['language']
        else:
            locale = self.DEFAULT_LOCALE
        language = self.POLYCOM_LOCALES[locale]
        
        if 'timezone' in provinfo:
            timezone = self.__format_tz_inform(tzinform.get_timezone_info(provinfo['timezone']))
        else:
            timezone = ''
        
        if self.PROXY_BACKUP:
            lines = []
            lines.append('reg.1.server.2.address="%s"' % self.PROXY_BACKUP)
            lines.append('reg.1.server.2.port="5060"')
            lines.append('reg.1.server.2.transport="UDPOnly"')
            lines.append('reg.1.server.2.expires="3600"')
            if self.REGISTRAR_BACKUP and self.REGISTRAR_BACKUP == self.PROXY_BACKUP:
                register = 1
            else:
                register = 0
            lines.append('reg.1.server.2.register="%s"' % register)
            backup_pbx = '\n'.join(lines)
        else:
            backup_pbx = ''

        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'user_vmail_addr':        self.xml_escape(provinfo['vmailaddr']),
                      'language':               language,
                      'timezone':               timezone,
                      'backup_pbx':             backup_pbx,
                    },
                    self.xml_escape,
                    clean_extension),
                cfg_filename,
                'utf8')

        tmp_file = open(tmp_filename, "w")
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(tmp_filename, cfg_filename)
        
    @classmethod
    def __format_tz_inform(cls, inform):
        lines = []
        lines.append('tcpIpApp.sntp.gmtOffset="%d"' % inform['utcoffset'].as_seconds)
        if inform['dst'] is None:
            lines.append('tcpIpApp.sntp.daylightSavings.enable="0"')
        else:
            lines.append('tcpIpApp.sntp.daylightSavings.enable="1"')
            if inform['dst']['start']['day'].startswith('D'):
                lines.append('tcpIpApp.sntp.daylightSavings.fixedDayEnable="1"')
            else:
                lines.append('tcpIpApp.sntp.daylightSavings.fixedDayEnable="0"')
            lines.extend(cls.__format_dst_change('start', inform['dst']['start']))
            lines.extend(cls.__format_dst_change('stop', inform['dst']['end']))
        return '\n'.join(lines)
            
    @classmethod
    def __format_dst_change(cls, suffix, dst_change):
        lines = []
        lines.append('tcpIpApp.sntp.daylightSavings.%s.month="%d"' % (suffix, dst_change['month']))
        lines.append('tcpIpApp.sntp.daylightSavings.%s.time="%d"' % (suffix, dst_change['time'].as_hours))
        if dst_change['day'].startswith('D'):
            lines.append('tcpIpApp.sntp.daylightSavings.%s.date="%s"' % (suffix, dst_change['day'][1:]))
        else:
            week, weekday = dst_change['day'][1:].split('.')
            lines.append('tcpIpApp.sntp.daylightSavings.%s.dayOfWeek="%s"' % (suffix, weekday))
            if week == '5':
                lines.append('tcpIpApp.sntp.daylightSavings.%s.dayOfWeek.lastInMonth="1"' % suffix)
            else:
                lines.append('tcpIpApp.sntp.daylightSavings.%s.dayOfWeek.lastInMonth="0"' % suffix)
                lines.append('tcpIpApp.sntp.daylightSavings.%s.date="%d"' % (suffix, (int(week) - 1) * 7 + 1))
        return lines
        

    def do_reinitprov(self, provinfo):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        self.__generate(provinfo)

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
