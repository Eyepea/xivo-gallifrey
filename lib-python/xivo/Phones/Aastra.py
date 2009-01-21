"""Support for Aastra phones for XIVO Configuration

Aastra 51i, 53i, 55i and 57i are supported.

Copyright (C) 2008-2009  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008-2009  Proformatique

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

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin


log = logging.getLogger("xivo.Phones.Aastra") # pylint: disable-msg=C0103


class Aastra(PhoneVendorMixin):

    AASTRA_MODELS = ('51i', '53i', '55i', '57i')
    AASTRA_COMMON_HTTP_USER = 'admin'
    AASTRA_COMMON_HTTP_PASS = '22222'

    AASTRA_COMMON_DIR = None

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.AASTRA_COMMON_DIR = os.path.join(cls.TFTPROOT, "Aastra/")

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in self.AASTRA_MODELS:
            raise ValueError, "Unknown Aastra model %r" % self.phone['model']

    def __action(self, user, passwd):
        cnx_to = max_to = max(1, self.CURL_TO_S / 2)
        try: # XXX: also check return values?

            ## curl options
            # -s                    -- silent
            # -o /dev/null          -- dump result
            # --connect-timeout 15  -- timeout connection after 15s
            # --max-time 15         -- timeout after 15s when connected
            # -retry 0              -- don't retry
            # -d DATA               -- post DATA

            # We first make two attempts
            # The first one replies: 401 Unauthorized (Authorization failed)
            # The second one does not fail
            for attempts in 1, 2: # pylint: disable-msg=W0612
                subprocess.call([self.CURL_CMD,
                                 "--retry", "0",
                                 "--connect-timeout", str(cnx_to),
                                 "--max-time", str(max_to),
                                 "-s",
                                 "-o", "/dev/null",
                                 "-u", "%s:%s" % (user, passwd),
                                 "http://%s" % self.phone['ipv4']],
                                close_fds = True)

            # once we have been authenticated, we can POST the appropriate commands

            # first : upgrade
            # this seems to be compulsory when taking the phones out of their box, since the tftpboot parameters
            # can not handle the Aastra/ subdirectory with the out-of-the-box version
            subprocess.call([self.CURL_CMD,
                             "--retry", "0",
                             "--connect-timeout", str(cnx_to),
                             "--max-time", str(max_to),
                             "-s",
                             "-o", "/dev/null",
                             "-u", "%s:%s" % (user, passwd),
                             "http://%s/upgrade.html" % self.phone['ipv4'],
                             "-d", "tftp=%s&file=Aastra/%s.st" % (self.ASTERISK_IPV4, self.phone['model'])],
                            close_fds = True)

            # then reset
            subprocess.call([self.CURL_CMD,
                             "--retry", "0",
                             "--connect-timeout", str(cnx_to),
                             "--max-time", str(max_to),
                             "-s",
                             "-o", "/dev/null",
                             "-u", "%s:%s" % (user, passwd),
                             "http://%s/reset.html" % self.phone['ipv4'],
                             "-d", "resetOption=0"],
                            close_fds = True)
        except OSError:
            log.exception("error when trying to call curl")

    @staticmethod
    def __format_function_keys(funckey, model):
        sorted_keys = funckey.keys()
        sorted_keys.sort()
        fk_config_lines = []
        unit = 1
        for key in sorted_keys:
            exten, supervise = funckey[key] # pylint: disable-msg=W0612

            key = int(key)

            if key <= 6:
                if model == '57i':
                    keytype = "topsoft"
                else:
                    keytype = "prg"
            elif key > 12:
                key = (key - 12) % 60

                if key == 0:
                    key = 60
                    unit += 1

                keytype = "expmod%d " % unit
            else:
                keytype = "soft"

            if supervise:
                type = "blf"
            else:
                type = "speeddial"

            fk_config_lines.append("%skey%d type: %s" % (keytype, key, type))
            fk_config_lines.append("%skey%d label: %s" % (keytype, key, exten))
            fk_config_lines.append("%skey%d value: %s" % (keytype, key, exten))
            fk_config_lines.append("%skey%d line: 1" % (keytype, key))
        return "\n".join(fk_config_lines)

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action(self.AASTRA_COMMON_HTTP_USER, self.AASTRA_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action(self.AASTRA_COMMON_HTTP_USER, self.AASTRA_COMMON_HTTP_PASS)

    def __generate(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].upper().replace(":", "")
        template_file = open(os.path.join(self.TEMPLATES_DIR, "aastra-" + model + ".cfg"))
        template_lines = template_file.readlines()
        template_file.close()
        tmp_filename = os.path.join(self.AASTRA_COMMON_DIR, macaddr + '.cfg.tmp')
        cfg_filename = tmp_filename[:-4]

        function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'], model)

        txt = xivo_config.txtsubst(template_lines,
                { 'user_display_name': provinfo['name'],
                  'user_phone_ident':  provinfo['ident'],
                  'user_phone_number': provinfo['number'],
                  'user_phone_passwd': provinfo['passwd'],
                  'function_keys': function_keys_config_lines,
                },
                cfg_filename)
        tmp_file = open(tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(tmp_filename, cfg_filename)

    def do_autoprov(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        self.__generate(provinfo)

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
                  'funckey': {},
                })

    # Introspection entry points

    @classmethod
    def get_phones(cls):
        "Report supported phone models for this vendor."
        return tuple([(x, x) for x in cls.AASTRA_MODELS])

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        # Aastra 53i/2.2.0.166
        # Aastra 55i/2.2.0.166

        if 'aastra' != ua[:6].lower():
            return None
        modelfw = ua[6:].strip().split('/', 1)
        model = 'unknown'
        fw = 'unknown'

        if len(modelfw[0]) > 0:
            model = modelfw[0].lower()
            if len(modelfw) == 2:
                fw = modelfw[1]
        return ("aastra", model, fw)

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for model in cls.AASTRA_MODELS:
            for line in (
                'class "Aastra%s" {\n' % model,
                '    match if option vendor-class-identifier = "AastraIPPhone%s";\n' % model,
                '    log("boot Aastra %s");\n' % model,
                '    option tftp-server-name "%s";\n' % addresses['bootServer'],
                '    option bootfile-name "aastra.cfg";\n',
                '    next-server %s;\n' % addresses['bootServer'],
                '}\n',
                '\n'):
                yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        for model in cls.AASTRA_MODELS:
            yield '        allow members of "Aastra%s";\n' % model
