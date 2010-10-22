"""Support for Zenitel stations/phones for XIVO Configuration

All IP stations are supported.

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

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin

log = logging.getLogger("xivo.Phones.Zenitel") # pylint: disable-msg=C0103


class Zenitel(PhoneVendorMixin):

    ZENITEL_MODELS = ('ipstation',)

    ZENITEL_MACADDR_PREFIX = ('1:00:13:CB',)

    ZENITEL_COMMON_HTTP_USER = "admin"
    ZENITEL_COMMON_HTTP_PASS = "alphaadmin"

    ZENITEL_COMMON_DIR = None
    
    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.ZENITEL_COMMON_DIR = os.path.join(cls.TFTPROOT, 'Zenitel', '')

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in self.ZENITEL_MODELS:
            raise ValueError, "Unknown Zenitel model %r" % self.phone['model']

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
                             '--form-string', 'message=%s' % command,
                             "http://%s/goform/zForm_send_cmd" % self.phone['ipv4']],
                            close_fds = True)
        except OSError:
            log.exception("error when trying to call curl")

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action("Reboot", self.ZENITEL_COMMON_HTTP_USER, self.ZENITEL_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action("Reboot", self.ZENITEL_COMMON_HTTP_USER, self.ZENITEL_COMMON_HTTP_PASS)

    def __generate(self, provinfo, dry_run):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].replace(":", "").lower()

        try:
            template_specific_path = os.path.join(self.ZENITEL_COMMON_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            template_common_path = os.path.join(self.ZENITEL_COMMON_DIR, "templates", "zenitel-" + model + ".cfg")

            if not os.access(template_common_path, os.R_OK):
                template_common_path = os.path.join(self.TEMPLATES_DIR, "zenitel-" + model + ".cfg")

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      template_common_path)
            template_file = open(template_common_path)

        template_lines = template_file.readlines()
        template_file.close()
        tmp_filename = os.path.join(self.TFTPROOT, model[:4] + "_config_" + self.phone['macaddr'].replace(":", "_").lower() + ".cfg.tmp")
        cfg_filename = tmp_filename[:-4]
        
        speeddial_keys = self.__format_speeddial_keys(provinfo['funckey'])

        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    {  'speeddial_keys':    speeddial_keys
                    }),
                cfg_filename,
                'utf8')

        if dry_run:
            return ''.join(txt)
        else:
            self._write_cfg(tmp_filename, cfg_filename, txt)
        
    @classmethod
    def __format_speeddial_keys(cls, funckeys):
        fk_config_lines = []
        for keynum in sorted(funckeys.keys()):
            if 1 <= keynum <= 3:
                value = funckeys[keynum]
                exten = value['exten']
                fk_config_lines.append('speeddial_%s_max_ring_time=0' % keynum)
                fk_config_lines.append('speeddial_%s_max_conv_time=0' % keynum)
                fk_config_lines.append('speeddial_%s_loop=0' % keynum)
                fk_config_lines.append('speeddial_%s_c1=%s' % (keynum, exten))
        return '\n'.join(' ' + s for s in fk_config_lines)

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
        return tuple([(x, x.upper()) for x in cls.ZENITEL_MODELS])

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        # Zenitel IPSTATION v2.0

        ua_splitted = ua.lower().split(' ')
        if len(ua_splitted) == 3:
            vendor, model, fw_version = ua_splitted
            if vendor == 'zenitel' and model == 'ipstation':
                return (vendor, model, fw_version) 
        return None

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for macaddr_prefix in cls.ZENITEL_MACADDR_PREFIX:
            for line in (
                'subclass "phone-mac-address-prefix" %s {\n' % macaddr_prefix,
                '    log("class Zenitel prefix %s");\n' % macaddr_prefix,
                '    option tftp-server-name "%s";\n' % addresses['bootServer'],
                '}\n',
                '\n'):
                yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        return ()
