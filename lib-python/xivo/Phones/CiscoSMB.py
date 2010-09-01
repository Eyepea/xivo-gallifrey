"""Support for CiscoSMB SPA50XG phones for XIVO Configuration

CiscoSMB SPA501G, SPA502G, SPA504G, SPA508G, SPA509G and SPA525G are supported.

Copyright (C) 2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2010  Proformatique

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
import subprocess
import math

from xml.sax.saxutils import escape

from xivo import tzinform
from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.CiscoSMB") # pylint: disable-msg=C0103


class CiscoSMB(PhoneVendorMixin):

    CISCOSMB_MODELS = ('spa501g',
                       'spa502g',
                       'spa504g',
                       'spa508g',
                       'spa509g',
                       'spa525g')

    CISCOSMB_MACADDR_PREFIX = ('1:00:26:99',)

    CISCOSMB_COMMON_HTTP_USER = "admin"
    CISCOSMB_COMMON_HTTP_PASS = "adminpass"

    CISCOSMB_COMMON_DIR = None
    
    CISCOSMB_LOCALES = {
        'de_DE': 'German',
        'en_US': 'English',
        'es_ES': 'Spanish',
        'fr_FR': 'French',
        'fr_CA': 'French',
    }

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.CISCOSMB_COMMON_DIR = os.path.join(cls.TFTPROOT, 'CiscoSMB', '')

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in self.CISCOSMB_MODELS:
            raise ValueError, "Unknown Cisco SMB model %r" % self.phone['model']

    @staticmethod
    def xml_escape(data):
        if data is None:
            return ""
        elif not isinstance(data, basestring):
            return str(data)

        return escape(data, {"'": '&apos;', '"': '&quot;'})

    def __action(self, command, user, passwd):
        cnx_to = max_to = max(1, self.CURL_TO_S / 2)
        try: # XXX: also check return values?

            ## curl options
            # -s                    -- silent
            # -o /dev/null          -- dump result
            # --connect-timeout 30  -- timeout after 30s
            # --max-time 15         -- timeout after 15s when connected
            # -retry 0              -- don't retry
            subprocess.call([self.CURL_CMD,
                             "--retry", "0",
                             "--connect-timeout", str(cnx_to),
                             "--max-time", str(max_to),
                             "-s",
                             "-o", "/dev/null",
                             "--digest",
                             "-u", "%s:%s" % (user, passwd),
                             "http://%s/admin/%s" % (self.phone['ipv4'], command)],
                            close_fds = True)
        except OSError:
            log.exception("error when trying to call curl")

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action("reboot", self.CISCOSMB_COMMON_HTTP_USER, self.CISCOSMB_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action("reboot", self.CISCOSMB_COMMON_HTTP_USER, self.CISCOSMB_COMMON_HTTP_PASS)

    def __generate(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].replace(":", "").lower()

        try:
            template_specific_path = os.path.join(self.CISCOSMB_COMMON_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            template_common_path = os.path.join(self.CISCOSMB_COMMON_DIR, "templates", "ciscosmb-" + model + ".cfg")

            if not os.access(template_common_path, os.R_OK):
                template_common_path = os.path.join(self.TEMPLATES_DIR, "ciscosmb-" + model + ".cfg")

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      template_common_path)
            template_file = open(template_common_path)

        template_lines = template_file.readlines()
        template_file.close()
        tmp_filename = os.path.join(self.CISCOSMB_COMMON_DIR, model[:-1] + model[-1].upper() + "-" + macaddr + ".cfg.tmp")
        cfg_filename = tmp_filename[:-4]

        if bool(int(provinfo.get('subscribemwi', 0))):
            provinfo['vmailaddr'] = "%s@%s" % (provinfo['number'], self.ASTERISK_IPV4)
        else:
            provinfo['vmailaddr'] = ""

        exten_pickup_prefix = \
                clean_extension(provinfo['extensions']['pickupexten']) + "#"

        function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'], model)

        if 'language' in provinfo and provinfo['language'] in self.CISCOSMB_LOCALES:
            locale = provinfo['language']
        else:
            locale = self.DEFAULT_LOCALE
        language = self.CISCOSMB_LOCALES[locale]
        
        if 'timezone' in provinfo:
            timezone = self.__format_tz_inform(tzinform.get_timezone_info(provinfo['timezone']))
        else:
            timezone = ''
            
        if self.PROXY_BACKUP:
            backup_proxy = 'xivo_proxies:SRV=%s:5060:p=0|%s:5060:p=1' % (self.PROXY_MAIN, self.PROXY_BACKUP)
        else:
            backup_proxy = ''
        
        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'user_vmail_addr':        self.xml_escape(provinfo['vmailaddr']),
                      'exten_pickup_prefix':    exten_pickup_prefix,
                      'function_keys':          function_keys_config_lines,
                      'language':               language,
                      'timezone':               timezone,
                      'backup_proxy':           backup_proxy,
                    },
                    self.xml_escape,
                    clean_extension),
                cfg_filename,
                'utf8')

        tmp_file = open(tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(tmp_filename, cfg_filename)
    
    @classmethod
    def __format_tz_inform(cls, inform):
        lines = []
        hours, minutes = inform['utcoffset'].as_hms[:2]
        lines.append('<Time_Zone ua="rw">GMT%+03d:%02d</Time_Zone>' % (hours, minutes))
        # We need to substract 1 from the computed hour (bug in the SPA firmware?)
        lines.append('<Time_Offset__HH_mm_ ua="rw">%d/%d</Time_Offset__HH_mm_>' % (hours - 1, minutes))
        if inform['dst'] is None:
            lines.append('<Daylight_Saving_Time_Enable ua="rw">no</Daylight_Saving_Time_Enable>')
        else:
            lines.append('<Daylight_Saving_Time_Enable ua="rw">yes</Daylight_Saving_Time_Enable>')
            h, m, s = inform['dst']['save'].as_hms
            lines.append('<Daylight_Saving_Time_Rule ua="rw">start=%s;end=%s;save=%d:%d:%s</Daylight_Saving_Time_Rule>' %
                         (cls.__format_dst_change(inform['dst']['start']),
                          cls.__format_dst_change(inform['dst']['end']),
                          h, m, s,
                          ))
        return '\n'.join(lines)
    
    @classmethod
    def __format_dst_change(cls, dst_change):
        _day = dst_change['day']
        if _day.startswith('D'):
            day = _day[1:]
            weekday = '0'
        else:
            week, weekday = _day[1:].split('.')
            if week == '5':
                day = '-1'
            else:
                day = (int(week) - 1) * 7 + 1
        
        h, m, s = dst_change['time'].as_hms
        return ('%s/%s/%s/%s:%s:%s' %
                (dst_change['month'], day, weekday, h, m, s))

    @classmethod
    def __format_function_keys(cls, funckey, model):
        sorted_keys = funckey.keys()
        sorted_keys.sort()
        fk_config_lines = []
        for key in sorted_keys:
            value   = funckey[key]
            exten   = value['exten']
            key     = int(key)
            if 'label' in value and value['label'] is not None:
                label = value['label']
            else:
                label = exten
            label = CiscoSMB.xml_escape(label)

            if value.get('supervision'):
                blf = "+blf"
            else:
                blf = ""

            func = "fnc=sd+cp%s;sub=%s@%s;nme=%s" % (blf, exten, cls.ASTERISK_IPV4, label)

            if key == 1:
                continue
            elif model in ('spa501g', 'spa508g'):
                if key > 8:
                    key -= 8
                    fk_config_lines.append(cls.__format_function_keys_unit(key, func))
                    continue
            elif model == 'spa502g':
                if key > 2:
                    key -= 2
                    fk_config_lines.append(cls.__format_function_keys_unit(key, func))
                    continue
            elif model == 'spa504g':
                if key > 4:
                    key -= 4
                    fk_config_lines.append(cls.__format_function_keys_unit(key, func))
                    continue
            elif model == 'spa509g':
                if key > 12:
                    key -= 12
                    fk_config_lines.append(cls.__format_function_keys_unit(key, func))
                    continue
            elif model == 'spa525g':
                if key > 5:
                    key -= 5
                    fk_config_lines.append(cls.__format_function_keys_unit(key, func))
                    continue
            elif key > 12:
                continue

            fk_config_lines.append('<Extension_%d_ ua="na">Disabled</Extension_%d_>' % (key, key))
            fk_config_lines.append('<Short_Name_%d_ ua="na">%s</Short_Name_%d_>' % (key, label, key))
            fk_config_lines.append('<Extended_Function_%d_ ua="na">%s</Extended_Function_%d_>' % (key, func, key))

        return "\n".join(fk_config_lines)

    @classmethod
    def __format_function_keys_unit(cls, key, func):
        unit = int(math.ceil(float(key) / 32))
        key %= 32

        if key == 0:
            key = 32

        return '<Unit_%d_Key_%d ua="na">%s</Unit_%d_Key_%d>' % (unit, key, func, unit, key)

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
        return tuple([(x, x.upper()) for x in cls.CISCOSMB_MODELS])

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        # Cisco/SPA504G-7.1.3a

        ua_splitted = ua.split("/", 1)
        if len(ua_splitted) != 2 or ua_splitted[0] != 'Cisco':
            return None

        modelfw = ua_splitted[1].split("-", 1)
        model   = modelfw[0].lower()
        fw      = 'unknown'

        if model not in cls.CISCOSMB_MODELS:
            return None
        elif len(modelfw) == 2:
            fw = modelfw[1]

        return ("ciscosmb", model, fw)

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for model in cls.CISCOSMB_MODELS:
            for line in (
                'class "CiscoSMB%s" {\n' % model.upper(),
                '    match if (option vendor-class-identifier = "Cisco Systems, Inc. IP Phone %s";\n' % model.upper(),
                '              or option vendor-class-identifier = "Cisco %s");\n' % model.upper(),
                '    log("boot CiscoSMB %s");\n' % model.upper(),
                '    option tftp-server-name "%s";\n' % addresses['bootServer'],
                '}\n',
                '\n'):
                yield line

        for macaddr_prefix in cls.CISCOSMB_MACADDR_PREFIX:
            for line in (
                'subclass "phone-mac-address-prefix" %s {\n' % macaddr_prefix,
                '    if not exists vendor-class-identifier {\n',
                '        log("class CiscoSMB prefix %s");\n' % macaddr_prefix,
                '        option tftp-server-name "%s";\n' % addresses['bootServer'],
                '    }\n',
                '}\n',
                '\n'):
                yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        for model in cls.CISCOSMB_MODELS:
            yield '        allow members of "CiscoSMB%s";\n' % model.upper()
