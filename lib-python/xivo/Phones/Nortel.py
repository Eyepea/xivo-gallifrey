"""Support for Linksys phones for XIVO Configuration

Nortel 1220 are supported.

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

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.Nortel") # pylint: disable-msg=C0103


class Nortel(PhoneVendorMixin):

    NORTEL_MODELS = (('1220', '1220'),)

    NORTEL_COMMON_HTTP_USER = "admin"
    NORTEL_COMMON_HTTP_PASS = "1234"

    NORTEL_COMMON_DIR = None

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.NORTEL_COMMON_DIR = os.path.join(cls.TFTPROOT, "Nortel/")

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in [x[0] for x in self.NORTEL_MODELS]:
            raise ValueError, "Unknown Nortel model %r" % self.phone['model']

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
			     "-d", "CMD=%s" % command,
                             "http://%s/reboot.html" % self.phone['ipv4']],
                            close_fds = True)
        except OSError:
            log.exception("error when trying to call curl")

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action("REBOOT", self.NORTEL_COMMON_HTTP_USER, self.NORTEL_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action("REBOOT", self.NORTEL_COMMON_HTTP_USER, self.NORTEL_COMMON_HTTP_PASS)

    def __generate(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].replace(":", "").lower()
        fromdhcp = 0

        if self.phone.get('from') == 'dhcp':
            fromdhcp = 1

        try:
            template_specific_path = os.path.join(self.NORTEL_COMMON_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            template_common_path = os.path.join(self.NORTEL_COMMON_DIR, "templates", "cisco-" + model + ".cfg")

            if not os.access(template_common_path, os.R_OK):
                template_common_path = os.path.join(self.TEMPLATES_DIR, "cisco-" + model + ".cfg")

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      template_common_path)
            template_file = open(template_common_path)

        template_lines = template_file.readlines()
        template_file.close()
        tmp_filename = os.path.join(self.NORTEL_COMMON_DIR, "SIP" + macaddr + ".xml.tmp")
        cfg_filename = os.path.join(self.NORTEL_COMMON_DIR, "SIP" + macaddr + ".xml")

        if bool(int(provinfo.get('subscribemwi', 0))):
            provinfo['vmailaddr'] = "%s@%s" % (provinfo['number'], self.ASTERISK_IPV4)
        else:
            provinfo['vmailaddr'] = ""

        exten_pickup_prefix = \
                clean_extension(provinfo['extensions']['pickupexten']) + "#"

        function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'], model, provinfo)

        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'user_vmail_addr':        provinfo['vmailaddr'],
                      'exten_pickup_prefix':    exten_pickup_prefix,
                      'function_keys':          function_keys_config_lines
                    },
                    clean_extension),
                cfg_filename,
                'utf8')

        if fromdhcp:
            if os.path.exists(cfg_filename):
                return

        tmp_file = open(tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(tmp_filename, cfg_filename)

    @classmethod
    def __format_function_keys(cls, funckey, model, provinfo):
        if model not in ('nortel'):
            return ""

        sorted_keys = funckey.keys()
        sorted_keys.sort()
        fk_config_lines = []

        exten_pickup_prefix = \
                clean_extension(provinfo['extensions']['pickupexten'])

        for key in sorted_keys:
            value   = funckey[key]
            exten   = value['exten']
            key     = int(key)
            label   = value.get('label', exten)

            #fk_config_lines.append('No supported')

        return "\n".join(fk_config_lines)

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
        return tuple([(x[0], x[0].upper()) for x in cls.NORTEL_MODELS])

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """

	# Nortel-CP7941G-GE/8.0

        ua_splitted = ua.split("-", 2)
        if ua_splitted[0] != 'Nortel':
            return None
        model = 'unknown'
        fw = 'unknown'
        if len(ua_splitted) == 3:
            fws = ua_splitted[2].split("/", 1)
	    fw = fws[1]
            model = ua_splitted[1].lower()
        return ("cisco", model, fw)

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        return

    @classmethod
    def get_dhcp_pool_lines(cls):
        return
