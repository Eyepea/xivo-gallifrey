"""Support for Snom phones for XIVO Configuration

Snom 300 320 and 360 are supported.

Copyright (C) 2007-2009  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007-2009  Proformatique

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

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin


log = logging.getLogger("xivo.Phones.Snom") # pylint: disable-msg=C0103


# SNOM BUG #1
# Snom doesn't support something else than files at root of tftproot when using
# tftp... :/ (It just internaly replaces the first '/' with a '\0' :/// )
# TODO: check if still true!

# SNOM BUG #2
# Because it seems much technically impossible to detect the phone model by
# dhcp request (model not in the request.... :///), we'll need to also support
# HTTP based xivo_config
# TODO: check if still true!


class Snom(PhoneVendorMixin):

    SNOM_MODELS = ('300', '320', '360')

    SNOM_COMMON_HTTP_USER = "guest"
    SNOM_COMMON_HTTP_PASS = "guest"

    SNOM_SPEC_DIR = None
    SNOM_SPEC_TEMPLATE = None

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.SNOM_SPEC_DIR = os.path.join(cls.TFTPROOT, "Snom/")
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
            exten, supervise = funckey[key] # pylint: disable-msg=W0612
            fk_config_lines.append("fkey%01d: blf <sip:%s@%s;phone=user>" % (int(key), exten, cls.ASTERISK_IPV4))
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

    def do_reinitprov(self):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].replace(":", "").upper()

        htm_filename = os.path.join(self.SNOM_SPEC_DIR, "snom" + model + "-" + macaddr + ".htm")
        try:
            os.unlink(htm_filename)
        except OSError:
            pass

    def do_autoprov(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].replace(":", "").upper()

        try:
            template_specific_path = os.path.join(self.SNOM_SPEC_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      self.SNOM_SPEC_TEMPLATE)
            template_file = open(self.SNOM_SPEC_TEMPLATE)

        template_lines = template_file.readlines()
        template_file.close()
        tmp_filename = os.path.join(self.SNOM_SPEC_DIR, "snom" + model + "-" + macaddr + ".htm.tmp")
        htm_filename = tmp_filename[:-4]

        function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'])

        txt = xivo_config.txtsubst(template_lines,
                { 'user_display_name':  provinfo['name'],
                  'user_phone_ident':   provinfo['ident'],
                  'user_phone_number':  provinfo['number'],
                  'user_phone_passwd':  provinfo['passwd'],
                  'asterisk_ipv4':      self.ASTERISK_IPV4,
                  'ntp_server_ipv4':    self.NTP_SERVER_IPV4,
                  'http_user':          self.SNOM_COMMON_HTTP_USER,
                  'http_pass':          self.SNOM_COMMON_HTTP_PASS,
                  'function_keys':      function_keys_config_lines,
                },
                htm_filename,
                'utf8')

        tmp_file = open(tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(tmp_filename, htm_filename)

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
        for line in (
            'subclass "phone-mac-address-prefix" 1:00:04:13 {\n',
            '    log("class Snom prefix 1:00:04:13");\n',
            '    option tftp-server-name "http://%s:8667/";\n' % addresses['bootServer'],
            '    option bootfile-name "snom.php?mac={mac}";\n',
            '    next-server %s;\n' % addresses['bootServer'],
            '}\n',
            '\n'):
            yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        return ()
