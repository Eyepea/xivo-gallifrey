"""Support for Snom phones for XIVO Configuration

Snom 300 320 and 360 are supported.

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
import subprocess

from xivo import tzinform
from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.Snom") # pylint: disable-msg=C0103

# SNOM BUG #2
# Because it seems much technically impossible to detect the phone model by
# dhcp request (model not in the request.... :///), we'll need to also support
# HTTP based xivo_config
# update (2010-07-07): vendor-class-identifier option is sent starting since
# firmware 7.3.15


class Snom(PhoneVendorMixin):

    SNOM_MODELS = ('300', '320', '360', '370', '820', '821', '870')

    SNOM_COMMON_HTTP_USER = "guest"
    SNOM_COMMON_HTTP_PASS = "guest"

    SNOM_SPEC_DIR = None
    SNOM_SPEC_TEMPLATE = None
    
    SNOM_LOCALES = {
        'de_DE': ('Deutsch', 'GER'),
        'en_US': ('English', 'USA'),
        'es_ES': ('Espanol', 'ESP'),
        'fr_FR': ('Francais', 'FRA'),
        'fr_CA': ('Francais', 'USA'),
    }

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.SNOM_SPEC_DIR = os.path.join(cls.TFTPROOT, "Snom")
        cls.SNOM_SPEC_TEMPLATE = os.path.join(cls.TEMPLATES_DIR, "snom-template.htm")

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in self.SNOM_MODELS:
            raise ValueError, "Unknown Snom model %r" % self.phone['model']

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
                             "-u", "%s:%s" % (user, passwd),
                             "http://%s/confirm.html?%s=yes" % (self.phone['ipv4'], command)],
                            close_fds = True)
        except OSError:
            log.exception("error when trying to call curl")

    @classmethod
    def __format_function_keys(cls, funckey):
        sorted_keys = funckey.keys()
        sorted_keys.sort()
        fk_config_lines = []
        for key in sorted_keys:
            value = funckey[key]
            exten = value['exten']

            if value.get('supervision'):
                xtype = "dest"
            else:
                xtype = "speed"
            fk_config_lines.append('<fkey idx="%d" context="active" perm="R">%s &lt;sip:%s@%s&gt;</fkey>' % (int(key)-1, xtype, exten, cls.ASTERISK_IPV4))
        return "\n".join(fk_config_lines)

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action("RESET", self.SNOM_COMMON_HTTP_USER, self.SNOM_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action("REBOOT", self.SNOM_COMMON_HTTP_USER, self.SNOM_COMMON_HTTP_PASS)
    
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

    def __generate(self, provinfo, dry_run):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].replace(":", "").upper()

        try:
            template_specific_path = os.path.join(self.SNOM_SPEC_DIR, macaddr + "-template.htm")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            template_common_path = os.path.join(self.SNOM_SPEC_DIR, "templates", "snom-template.htm")

            if not os.access(template_common_path, os.R_OK):
                template_common_path = self.SNOM_SPEC_TEMPLATE

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      template_common_path)
            template_file = open(template_common_path)

        template_lines = template_file.readlines()
        template_file.close()
        tmp_filename = os.path.join(self.SNOM_SPEC_DIR, "snom" + model + "-" + macaddr + ".xml.tmp")
        xml_filename = tmp_filename[:-4]

        function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'])
        
        if 'language' in provinfo and provinfo['language'] in self.SNOM_LOCALES:
            locale = provinfo['language']
        else:
            locale = self.DEFAULT_LOCALE
        language = """\
    <language perm="R">%s</language>
    <web_language perm="R">%s</language>
    <tone_scheme perm="R">%s</tone_scheme>""" % (self.SNOM_LOCALES[locale][0],
                                                 self.SNOM_LOCALES[locale][0],
                                                 self.SNOM_LOCALES[locale][1])

        if 'timezone' in provinfo:
            timezone = self.__format_tz_inform(tzinform.get_timezone_info(provinfo['timezone']))
        else:
            timezone = ''
        
        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'http_user':          self.SNOM_COMMON_HTTP_USER,
                      'http_pass':          self.SNOM_COMMON_HTTP_PASS,
                      'function_keys':      function_keys_config_lines,
                      'language':           language,
                      'timezone':           timezone,
                    },
                    format_extension=clean_extension),
                xml_filename,
                'utf8')

        if dry_run:
            return ''.join(txt)
        else:
            # We need to create a file which contains only a link to another file if we want the
            # configuration parameters to be applied with the correct priority... (i.e. we want
            # the per-phone parameters to override the generic parameters)
            redirect_file = open(os.path.join(self.SNOM_SPEC_DIR, "snom" + model + "-" + macaddr + ".htm"), 'w')
            redirect_file.write(
"""\
<?xml version="1.0" encoding="UTF-8" ?>
<setting-files>
  <file url="http://%s:8667/Snom/snom%s-%s.xml"/>
</setting-files>
""" % (self.ASTERISK_IPV4, model, macaddr))
            redirect_file.close()
            self._write_cfg(tmp_filename, xml_filename, txt)
        
    @classmethod
    def __format_tz_inform(cls, inform):
        lines = []
        lines.append('<timezone perm="R"></timezone>')
        lines.append('<utc_offset perm="R">%+d</utc_offset>' % inform['utcoffset'].as_seconds)
        if inform['dst'] is None:
            lines.append('<dst perm="R"></dst>')
        else:
            lines.append('<dst perm="R">%d %s %s</dst>' % 
                         (inform['dst']['save'].as_seconds,
                          cls.__format_dst_change(inform['dst']['start']),
                          cls.__format_dst_change(inform['dst']['end'])))
        return '\n'.join(lines)

    @classmethod
    def __format_dst_change(cls, dst_change):
        fmted_time = '%02d:%02d:%02d' % tuple(dst_change['time'].as_hms)
        day = dst_change['day']
        if day.startswith('D'):
            return '%02d.%02d %s' % (int(day[1:]), dst_change['month'], fmted_time)
        else:
            week, weekday = map(int, day[1:].split('.'))
            weekday = tzinform.week_start_on_monday(weekday)
            return '%02d.%02d.%02d %s' % (dst_change['month'], week, weekday, fmted_time)
    
    # Introspection entry points

    @classmethod
    def get_phones(cls):
        "Report supported phone models for this vendor."
        return tuple([(x, x) for x in cls.SNOM_MODELS])

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        if 'snom' != ua[:4].lower():
            return None
        fw = 'unknown'
        ua_splitted = ua.split('/')
        model = ua_splitted[0][4:].lower()
        if len(ua_splitted) > 1:
            fw = ua_splitted[1]
        return ("snom", model, fw)

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for model in cls.SNOM_MODELS:
            for line in (
                'class "Snom%s" {\n' % model,
                '    match if option vendor-class-identifier = "snom%s";\n' % model,
                '    log("boot Snom %s");\n' % model,
                '    option tftp-server-name "http://%s:8667/";\n' % addresses['bootServer'],
                '    option bootfile-name "Snom/snom%s.htm";\n' % model,
                '}\n',
                '\n'):
                yield line

        for line in (
            'subclass "phone-mac-address-prefix" 1:00:04:13 {\n',
            '    if not exists vendor-class-identifier {\n',
            '        log("class Snom prefix 1:00:04:13");\n',
            '        option tftp-server-name "http://%s:8667/";\n' % addresses['bootServer'],
            '        option bootfile-name "snom.php?mac={mac}";\n',
            '    }\n',
            '}\n',
            '\n'):
            yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        for model in cls.SNOM_MODELS:
            yield '        allow members of "Snom%s";\n' % model
