"""Support for Linksys phones for XIVO Configuration

Linksys SPA901, SPA921, SPA922, SPA941, SPA942, SPA962, SPA3102 and PAP2T are supported.

Copyright (C) 2007-2009  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007-2009  Proformatique

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
import subprocess
import math

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin


log = logging.getLogger("xivo.Phones.Linksys") # pylint: disable-msg=C0103


class Linksys(PhoneVendorMixin):

    LINKSYS_MODELS = (('spa901','SPA-901'),
                      ('spa921','SPA-921'),
                      ('spa922','SPA-922'),
                      ('spa941','SPA-941'),
                      ('spa942','SPA-942'),
                      ('spa962','SPA-962'),
                      ('spa3102','SPA-3102'),
                      ('pap2t','PAP2T'))

    LINKSYS_MACADDR_PREFIX = ('1:00:0e:08', '1:00:18:f8', '1:00:1c:10', '1:00:1e:e5', '1:00:1d:7e')

    LINKSYS_COMMON_HTTP_USER = "admin"
    LINKSYS_COMMON_HTTP_PASS = "adminpass"

    LINKSYS_COMMON_DIR = None

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.LINKSYS_COMMON_DIR = os.path.join(cls.TFTPROOT, "Linksys/")

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in [x[0] for x in self.LINKSYS_MODELS]:
            raise ValueError, "Unknown Linksys model %r" % self.phone['model']

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
        self.__action("reboot", self.LINKSYS_COMMON_HTTP_USER, self.LINKSYS_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action("reboot", self.LINKSYS_COMMON_HTTP_USER, self.LINKSYS_COMMON_HTTP_PASS)

    def __generate(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].lower().replace(":", "")
        template_file = open(os.path.join(self.TEMPLATES_DIR, "linksys-" + model + ".cfg"))
        template_lines = template_file.readlines()
        template_file.close()
        tmp_filename = os.path.join(self.LINKSYS_COMMON_DIR, model + "-" + macaddr + ".cfg.tmp")
        cfg_filename = tmp_filename[:-4]

        function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'])

        txt = xivo_config.txtsubst(template_lines,
                { 'user_display_name':  provinfo['name'],
                  'user_phone_ident':   provinfo['ident'],
                  'user_phone_number':  provinfo['number'],
                  'user_phone_passwd':  provinfo['passwd'],
                  'user_vmail_addr':    provinfo['vmailaddr'],
                  'asterisk_ipv4' :     self.ASTERISK_IPV4,
                  'ntp_server_ipv4' :   self.NTP_SERVER_IPV4,
                  'function_keys':      function_keys_config_lines,
                },
                cfg_filename)
        tmp_file = open(tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(tmp_filename, cfg_filename)

    @classmethod
    def __format_function_keys(cls, funckey):
        sorted_keys = funckey.keys()
        sorted_keys.sort()
        fk_config_lines = []
        for key in sorted_keys:
            exten, supervise = funckey[key]

            key = int(key)
            unit = int(math.ceil(math.modf(key)[1] / 32))
            key = key % 32

            key = (int(key) % 32)

            if key == 0:
                key = 32

            if supervise:
                blf = "+blf"
            else:
                blf = ""

            fk_config_lines.append('<Unit_%d_Key_%d ua="na">fnc=sd+cp%s;sub=%s@%s:nme=%s</Unit_%d_Key_%d>'
                                   % (unit, key, blf, exten, cls.ASTERISK_IPV4, exten, unit, key))
        return "\n".join(fk_config_lines)

    def do_reinitprov(self):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        self.__generate(
                { 'name':       "guest",
                  'ident':      "guest",
                  'number':     "guest",
                  'passwd':     "guest",
                  'vmailaddr':  "",
                  'funckey':    {},
                })

    def do_autoprov(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        if bool(int(provinfo.get('vmenable', 0))):
            provinfo['vmailaddr'] = "%s@%s" % (provinfo['number'], self.ASTERISK_IPV4)
        else:
            provinfo['vmailaddr'] = ""

        self.__generate(provinfo)

    # Introspection entry points

    @classmethod
    def get_phones(cls):
        "Report supported phone models for this vendor."
        return tuple([(x[0], x[0].upper()) for x in cls.LINKSYS_MODELS])

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        # Linksys/SPA901-4.1.11(c)
        # Linksys/SPA942-5.1.10
        # Linksys/SPA962-5.1.7
        # Linksys/PAP2T-5.1.5(LS)

        ua_splitted = ua.split("/", 1)
        if ua_splitted[0] != 'Linksys':
            return None
        model = 'unknown'
        fw = 'unknown'
        if len(ua_splitted) == 2:
            modelfw = ua_splitted[1].split("-", 1)
            model = modelfw[0].lower()
            if len(modelfw) == 2:
                fw = modelfw[1]
        return ("linksys", model, fw)

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for model, identifier in cls.LINKSYS_MODELS:
            for line in (
                'class "Linksys%s" {\n' % model.upper(),
                '    match if option vendor-class-identifier = "LINKSYS %s";\n' % identifier,
                '    log("boot Linksys %s");\n' % identifier,
                '    option tftp-server-name "%s";\n' % addresses['bootServer'],
                '    option bootfile-name "Linksys/%s.cfg";\n' % model,
                '    next-server %s;\n' % addresses['bootServer'],
                '}\n',
                '\n'):
                yield line

        for macaddr_prefix in cls.LINKSYS_MACADDR_PREFIX:
            for line in (
                'subclass "phone-mac-address-prefix" %s {\n' % macaddr_prefix,
                '    log("class Linksys prefix %s");\n' % macaddr_prefix,
                '    option tftp-server-name "%s";\n' % addresses['bootServer'],
                '}\n',
                '\n'):
                yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        for x in cls.LINKSYS_MODELS:
            yield '        allow members of "Linksys%s";\n' % x[0].upper()
