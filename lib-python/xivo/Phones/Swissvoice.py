"""Support for Swissvoice phones for XIVO Configuration

Swissvoice IP10S is supported.

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

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.Swissvoice") # pylint: disable-msg=C0103


# SWISSVOICE BUG
# It would be possible to make tftp upload a /tftpboot/Swissvoice/swupdate_ip10.inf
# into the phones, however in order for any MAC address to be taken care of,
# the <MACADDR>_ip10.inf files have to be located directly under /tftpboot/
# Nevertheless, we made them refer to the config <MACADDR>_ip10.cfg files
# located under /tftpboot/Swissvoice/

# Swissvoice User-Agent format :
# User-Agent: Swissvoice IP10 SP v1.0.1 (Build 4) 3.0.5.1


class Swissvoice(PhoneVendorMixin):

    SWISSVOICE_COMMON_HTTP_USER = "admin"
    SWISSVOICE_COMMON_HTTP_PASS = "admin"

    SWISSVOICE_SPEC_DIR = None
    SWISSVOICE_SPEC_INF_TEMPLATE = None
    SWISSVOICE_SPEC_CFG_TEMPLATE = None

    SWISSVOICE_DTMF = {
        'rfc2833':  "on inb",
        'inband':   "off",
        'info':     "on oob"
    }

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.SWISSVOICE_SPEC_DIR = os.path.join(cls.TFTPROOT, "Swissvoice/")
        cls.SWISSVOICE_SPEC_INF_TEMPLATE = os.path.join(cls.TEMPLATES_DIR, "template_ip10.inf")
        cls.SWISSVOICE_SPEC_CFG_TEMPLATE = os.path.join(cls.TEMPLATES_DIR, "template_ip10.cfg")

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] != "ip10s":
            raise ValueError, "Unknown Swissvoice model %r" % self.phone['model']

    def __action(self, user, passwd):
        cnx_to = max_to = max(1, self.CURL_TO_S / 2)
        try: # XXX: also check return values?

            ## curl options
            # -s                    -- silent
            # -o /dev/null          -- dump result
            # --connect-timeout 30  -- timeout after 30s
            # --max-time 15         -- timeout after 15s when connected
            # -retry 0              -- don't retry
            url_rep1 = "Administrator_Settings"
            url_rep2 = "reboot_choice_B.html?WINDWEB_URL=/Administrator_Settings/reboot_choice_B.html"
            url_rep3 = "&EraseFlash=0&Reboot=+Reboot"

            subprocess.call([self.CURL_CMD,
                             "--retry", "0",
                             "--connect-timeout", str(cnx_to),
                             "--max-time", str(max_to),
                             "-s",
                             "-o", "/dev/null",
                             "-u", "%s:%s" % (user, passwd),
                             "http://%s/%s/%s%s" % (self.phone['ipv4'], url_rep1, url_rep2, url_rep3)],
                            close_fds = True)
        except OSError:
            log.exception("error when trying to call curl")

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action(self.SWISSVOICE_COMMON_HTTP_USER, self.SWISSVOICE_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action(self.SWISSVOICE_COMMON_HTTP_USER, self.SWISSVOICE_COMMON_HTTP_PASS)

    def do_reinitprov(self, provinfo, dry_run):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        if dry_run:
            return
        else:
            macaddr = self.phone['macaddr'].replace(":", "").lower()
            cfg_filename = os.path.join(self.SWISSVOICE_SPEC_DIR, macaddr + "_ip10.cfg")
            inf_filename = os.path.join(self.SWISSVOICE_SPEC_DIR, "..", macaddr + "_ip10.inf")
            try:
                os.unlink(cfg_filename)
            except OSError:
                pass
            try:
                os.unlink(inf_filename)
            except OSError:
                pass

    def do_autoprov(self, provinfo, dry_run):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        if dry_run:
            return
        macaddr = self.phone['macaddr'].replace(":", "").lower()

        try:
            cfg_template_specific_path = os.path.join(self.SWISSVOICE_SPEC_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", cfg_template_specific_path)
            cfg_template_file = open(cfg_template_specific_path)
        except IOError, (errno, errstr):
            cfg_template_common_path = os.path.join(self.SWISSVOICE_SPEC_DIR, "templates", "template_ip10.cfg")

            if not os.access(cfg_template_common_path, os.R_OK):
                cfg_template_common_path = self.SWISSVOICE_SPEC_CFG_TEMPLATE

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      cfg_template_specific_path,
                      errno,
                      errstr,
                      cfg_template_common_path)
            cfg_template_file = open(cfg_template_common_path)

        cfg_template_lines = cfg_template_file.readlines()
        cfg_template_file.close()
        inf_template_file = open(self.SWISSVOICE_SPEC_INF_TEMPLATE)
        inf_template_lines = inf_template_file.readlines()
        inf_template_file.close()

        cfg_tmp_filename = os.path.join(self.SWISSVOICE_SPEC_DIR, macaddr + "_ip10.cfg.tmp")
        inf_tmp_filename = os.path.join(self.SWISSVOICE_SPEC_DIR, "..", macaddr + "_ip10.inf.tmp")
        cfg_filename = cfg_tmp_filename[:-4]
        inf_filename = inf_tmp_filename[:-4]



        txt = xivo_config.txtsubst(
                cfg_template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'http_user':          self.SWISSVOICE_COMMON_HTTP_USER,
                      'http_pass':          self.SWISSVOICE_COMMON_HTTP_PASS,
                      'user_dtmfmode':      self.SWISSVOICE_DTMF.get(provinfo['dtmfmode'], "off"),
                    },
                    format_extension=clean_extension),
                cfg_filename,
                'utf8')

        self._write_cfg(cfg_tmp_filename, cfg_filename, txt)

        txt = xivo_config.txtsubst(
                inf_template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'macaddr':    macaddr
                    },
                    format_extension=clean_extension),
                inf_filename,
                'utf8')

        self._write_cfg(inf_tmp_filename, inf_filename, txt)

    # Introspection entry points

    @classmethod
    def get_phones(cls):
        "Report supported phone models for this vendor."
        return (('ip10s', 'IP10S'),)

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        ua_splitted = ua.split(' ')
        if 'swissvoice' != ua_splitted[0].lower():
            return None
        model = ua_splitted[1].lower() + "s"
        fw = ua_splitted[3]
        return ("swissvoice", model, fw)

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for line in (
            'subclass "phone-mac-address-prefix" 1:00:05:90 {\n',
            '    log("class Swissvoice prefix 1:00:05:90");\n',
            '    option tftp-server-name "%s";\n' % addresses['bootServer'],
            '    option bootfile-name "swupdate_ip10.inf";\n',
            '}\n',
            '\n'):
            yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        return ()
