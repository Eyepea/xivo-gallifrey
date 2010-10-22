"""Support for Yealink phones for XIVO Configuration

Yealink T20P, T22P, T26P and T28P are supported.

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
import md5
from time import time

from xivo import tzinform
from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.Yealink") # pylint: disable-msg=C0103


class Yealink(PhoneVendorMixin):

    YEALINK_MODELS = ('T20P',
                      'T22P',
                      'T26P',
                      'T28P')

    YEALINK_MACADDR_PREFIX = ('1:00:15:65',)

    YEALINK_COMMON_HTTP_USER = "admin"
    YEALINK_COMMON_HTTP_PASS = "admin"

    YEALINK_COMMON_DIR = None

    YEALINK_DTMF = {
        'inband':   '0',
        'rfc2833':  '1',
        'info':     '2'
    }
    
    YEALINK_LOCALES = {
        'de_DE': ('German', 'Germany'),
        'en_US': ('English', 'United States'),
        'es_ES': ('Spanish', 'Spain'),
        'fr_FR': ('French', 'France'),
        'fr_CA': ('French', 'United States'),
    }

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.YEALINK_COMMON_DIR = os.path.join(cls.TFTPROOT, "Yealink/")

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'].upper() not in self.YEALINK_MODELS:
            raise ValueError, "Unknown Yealink model %r" % self.phone['model']

    @staticmethod
    def __generate_fake_md5(model, macaddr):
        """
        Generate a md5 to force update.
        """
        return md5.new("%s-%s-%s" % (time(), model, macaddr)).hexdigest()

    @staticmethod
    def escape_quote(xstr):
        """
        Escape simple quote
        """
        return str(xstr).replace("'", "'\\''")

    def __action(self, command, user, passwd):
        cnx_to = max_to = max(1, self.CURL_TO_S / 2)

        try: # XXX: also check return values?

            ## curl options
            # -s                    -- silent
            # -o /dev/null          -- dump result
            # --connect-timeout 30  -- timeout after 30s
            # --max-time 15         -- timeout after 15s when connected
            # -retry 0              -- don't retry

            # XXX: If Yealink phone has a call in progress, it couldn't reboot.
            #      So we fork the CURL command.
            # TODO: Rewrite this:
            #       - Modify shell sleep to python sleep
            #       - Remove " ".join()
            #       - Remove shell=True
            subprocess.Popen(" ".join(["sleep %s;" % cnx_to,
                                       self.CURL_CMD,
                                       "--retry 0",
                                       "--connect-timeout %s" % cnx_to,
                                       "--max-time %s" % max_to,
                                       "-s",
                                       "-o /dev/null",
                                       "-u '%s:%s'" % (self.escape_quote(user), self.escape_quote(passwd)),
                                       "-d 'PAGEID=7'",
                                       "-d 'CONFIG_DATA=%s'" % self.escape_quote(command),
                                       "'http://%s/cgi-bin/ConfigManApp.com'" % self.phone['ipv4']]),
                             shell=True,
                             close_fds=True)
        except OSError:
            log.exception("error when trying to call curl")

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action("REBOOT", self.YEALINK_COMMON_HTTP_USER, self.YEALINK_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action("REBOOT", self.YEALINK_COMMON_HTTP_USER, self.YEALINK_COMMON_HTTP_PASS)

    def __generate(self, provinfo, dry_run):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].replace(":", "").lower()

        try:
            template_specific_path = os.path.join(self.YEALINK_COMMON_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            template_common_path = os.path.join(self.YEALINK_COMMON_DIR, "templates", "yealink-" + model + ".cfg")

            if not os.access(template_common_path, os.R_OK):
                template_common_path = os.path.join(self.TEMPLATES_DIR, "yealink-" + model + ".cfg")

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      template_common_path)
            template_file = open(template_common_path)

        template_lines = template_file.readlines()
        template_file.close()
        tmp_filename = os.path.join(self.YEALINK_COMMON_DIR, model + "-" + macaddr + ".cfg.tmp")
        cfg_filename = os.path.join(self.YEALINK_COMMON_DIR, macaddr + ".cfg")

        provinfo['subscribemwi'] = str(int(bool(int(provinfo.get('subscribemwi', 0)))))

        exten_pickup_prefix = \
                clean_extension(provinfo['extensions']['pickupexten'])

        function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'], model, exten_pickup_prefix)
                
        if 'language' in provinfo and provinfo['language'] in self.YEALINK_LOCALES:
            locale = provinfo['language']
        else:
            locale = self.DEFAULT_LOCALE
        language = self.__format_language(self.YEALINK_LOCALES[locale][0])
        country = "Country = %s" % self.YEALINK_LOCALES[locale][0]
        
        if 'timezone' in provinfo:
            timezone = self.__format_tz_inform(tzinform.get_timezone_info(provinfo['timezone']))
        else:
            timezone = ''

        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'user_dtmfmode':  self.YEALINK_DTMF.get(provinfo['dtmfmode'], '2'),
                      'mac_fake_md5':   self.__generate_fake_md5(model, macaddr),
                      'function_keys':  function_keys_config_lines,
                      'language':       language,
                      'country':        country,
                      'timezone':       timezone,
                    },
                    clean_extension),
                cfg_filename,
                'utf8')

        if dry_run:
            return ''.join(txt)
        else:
            self._write_cfg(tmp_filename, cfg_filename, txt)
        
    @classmethod
    def __format_language(cls, lang):
        return "WebLanguage = %s\nActiveWebLanguage = %s" % (lang, lang)
        
    @classmethod
    def __format_tz_inform(cls, inform):
        lines = []
        lines.append('TimeZone = %+d' % min(max(inform['utcoffset'].as_hours, -11), 12))
        if inform['dst'] is None:
            lines.append('SummerTime = 0')
        else:
            lines.append('SummerTime = 2')
            if inform['dst']['start']['day'].startswith('D'):
                lines.append('DSTTimeType = 0')
            else:
                lines.append('DSTTimeType = 1')
            lines.append('StartTime = %s' % cls.__format_dst_change(inform['dst']['start']))
            lines.append('EndTime = %s' % cls.__format_dst_change(inform['dst']['end']))
            lines.append('OffsetTime = %s' % inform['dst']['save'].as_minutes)
        return '\n'.join(lines)

    @classmethod
    def __format_dst_change(cls, dst_change):
        if dst_change['day'].startswith('D'):
            return '%02d/%02d/%02d' % (dst_change['month'], dst_change['day'][1:], dst_change['time'].as_hour)
        else:
            week, weekday = map(int, dst_change['day'][1:].split('.'))
            weekday = tzinform.week_start_on_monday(weekday)
            return '%d/%d/%d/%d' % (dst_change['month'], week, weekday, dst_change['time'].as_hours)

    @classmethod
    def __format_function_keys(cls, funckey, model, exten_pickup_prefix):
        if model not in ('t26p', 't28p'):
            return ""

        sorted_keys = funckey.keys()
        sorted_keys.sort()
        fk_config_lines = []

        unused_keys = set(n for n in xrange(1, 11))
        for key in sorted_keys:
            # XXX keys between 11 and 16 for the T28 and between 11 and 13 for the T26
            # are not handled really well. Those keys correspond to the line keys. I'm
            # leaving this as is right now, hoping that Yealink will eventually makes
            # the provisioning more robust and more documented (and up to date)... 
            value   = funckey[key]
            exten   = value['exten']
            if value.get('supervision'):
                type = "blf"
                dktype = "16"
            else:
                type = ""
                dktype = "13"

            unused_keys.discard(int(key))
            fk_config_lines.append("[ memory%s ]" % key)
            fk_config_lines.append("path = /config/vpPhone/vpPhone.ini")
            fk_config_lines.append("type = %s" % type)
            fk_config_lines.append("Line = 0")
            fk_config_lines.append("Value = %s" % exten)
            fk_config_lines.append("DKtype = %s" % dktype)
            fk_config_lines.append("PickupValue = %s%s\n" % (exten_pickup_prefix, exten))
        for key in unused_keys:
            fk_config_lines.append("[ memory%s ]" % key)
            fk_config_lines.append("path = /config/vpPhone/vpPhone.ini")
            fk_config_lines.append("Line = 0")
            fk_config_lines.append("Value = ")
            fk_config_lines.append("DKtype = 0")

        return "\n".join(fk_config_lines)

    def do_reinitprov(self, provinfo, dry_run):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        return self.__generate(provinfo, dry_run)

    def do_autoprov(self, provinfo, dry_run):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        return self.__generate(provinfo, dry_run)

    # Introspection entry points

    @classmethod
    def get_phones(cls):
        "Report supported phone models for this vendor."
        return tuple([(x.lower(), x) for x in cls.YEALINK_MODELS])

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """

        ua_splitted = ua.strip().split(" ", 3)
        fw          = 'unknown'

        # Yealink SIP-T28P 2.50.0.50
        if ua_splitted[0] == "Yealink" and len(ua_splitted) > 1:
            model_splitted = ua_splitted[1].split("-", 1)
            if len(model_splitted) != 2 or model_splitted[1] not in cls.YEALINK_MODELS:
                return None
            model = model_splitted[1].lower()
            if len(ua_splitted) > 2:
                fw = ua_splitted[2]
        # T28 2.50.0.50
        elif ("%sP" % ua_splitted[0]) in cls.YEALINK_MODELS:
            model = "%sp" % ua_splitted[0].lower()

            if len(ua_splitted) > 1:
                fw = ua_splitted[1]
        else:
            return None

        return ("yealink", model, fw)

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for macaddr_prefix in cls.YEALINK_MACADDR_PREFIX:
            for line in (
                'subclass "phone-mac-address-prefix" %s {\n' % macaddr_prefix,
                '    log("class Yealink prefix %s");\n' % macaddr_prefix,
                '    option tftp-server-name "tftp://%s/Yealink";\n' % addresses['bootServer'],
                '}\n',
                '\n'):
                yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        return ()
