"""Support for Swissvoice phones for XIVO Configuration

Swissvoice IP10S is supported.

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

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin


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

    def do_reinitprov(self):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        macaddr = self.phone['macaddr'].lower().replace(":", "")
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

    def do_autoprov(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        cfg_template_file = open(self.SWISSVOICE_SPEC_CFG_TEMPLATE)
        cfg_template_lines = cfg_template_file.readlines()
        cfg_template_file.close()
        inf_template_file = open(self.SWISSVOICE_SPEC_INF_TEMPLATE)
        inf_template_lines = inf_template_file.readlines()
        inf_template_file.close()

        macaddr = self.phone['macaddr'].lower().replace(":", "")

        cfg_tmp_filename = os.path.join(self.SWISSVOICE_SPEC_DIR, macaddr + "_ip10.cfg.tmp")
        inf_tmp_filename = os.path.join(self.SWISSVOICE_SPEC_DIR, "..", macaddr + "_ip10.inf.tmp")
        cfg_filename = cfg_tmp_filename[:-4]
        inf_filename = inf_tmp_filename[:-4]

        dtmf_swissvoice = "off"
        dtmf_config     = provinfo["dtmfmode"]
        if dtmf_config == "rfc2833":
            dtmf_swissvoice = "on inb"
        elif dtmf_config == "inband":
            dtmf_swissvoice = "off"
        elif dtmf_config == "info":
            dtmf_swissvoice = "on oob"

        txt = xivo_config.txtsubst(cfg_template_lines,
                { 'user_display_name':  provinfo['name'],
                  'user_phone_ident':   provinfo['ident'],
                  'user_phone_number':  provinfo['number'],
                  'user_phone_passwd':  provinfo['passwd'],
                  'http_user':          self.SWISSVOICE_COMMON_HTTP_USER,
                  'http_pass':          self.SWISSVOICE_COMMON_HTTP_PASS,
                  'dtmfmode':           dtmf_swissvoice,
                  'asterisk_ipv4' :     self.ASTERISK_IPV4,
                  'ntp_server_ipv4' :   self.NTP_SERVER_IPV4,
                },
                cfg_filename)
        tmp_file = open(cfg_tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(cfg_tmp_filename, cfg_filename)

        txt = xivo_config.txtsubst(inf_template_lines,
                { 'macaddr': macaddr },
                inf_filename)
        tmp_file = open(inf_tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(inf_tmp_filename, inf_filename)

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
