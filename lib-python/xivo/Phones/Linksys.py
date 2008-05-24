"""Support for Linksys phones for XIVO Autoprovisioning

Linksys SPA901, SPA921, SPA922, SPA941, SPA942, SPA962 and PAP2T are supported.

Copyright (C) 2007, 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

import os
import sys
import syslog
import subprocess

from xivo import xivo_config
from xivo.xivo_config import PhoneVendor
from xivo.xivo_config import ProvGeneralConf as pgc
from xivo import except_tb

LINKSYS_COMMON_DIR = pgc['tftproot'] + "Linksys/"
LINKSYS_COMMON_HTTP_USER = "admin"
LINKSYS_COMMON_HTTP_PASS = "adminpass"

class LinksysProv(PhoneVendor):
        
        label = "Linksys"
        
        def __init__(self, phone):
                PhoneVendor.__init__(self, phone)
                # TODO: handle this with a lookup table stored in the DB?
                if self.phone["model"] != "spa901" and \
                   self.phone["model"] != "spa921" and \
                   self.phone["model"] != "spa922" and \
                   self.phone["model"] != "spa941" and \
                   self.phone["model"] != "spa942" and \
                   self.phone["model"] != "spa962" and \
                   self.phone["model"] != "pap2t":
                        raise ValueError, "Unknown Linksys model '%s'" % self.phone["model"]
        
        def __action(self, command, user, passwd):
                try: # XXX: also check return values?
                        
                        ## curl options
                        # -s			-- silent
                        # -o /dev/null		-- dump result
                        # --connect-timeout 30  -- timeout after 30s
                        # -retry 0		-- don't retry
                        subprocess.call([pgc['curl_cmd'],
                                         "--retry", "0",
                                         "--connect-timeout", str(pgc['curl_to_s']),
                                         "-s",
                                         "-o", "/dev/null",
                                         "--digest",
                                         "-u", "%s:%s" % (user, passwd),
                                         "http://%s/admin/%s" % (self.phone['ipv4'], command)],
                                        close_fds = True)
                except OSError:
                        except_tb.syslog_exception()
        
        def do_reinit(self):
                """
                Entry point to send the (possibly post) reinit command to
                the phone.
                """
                self.__action("reboot", LINKSYS_COMMON_HTTP_USER, LINKSYS_COMMON_HTTP_PASS)
        
        def do_reboot(self):
                "Entry point to send the reboot command to the phone."
                self.__action("reboot", LINKSYS_COMMON_HTTP_USER, LINKSYS_COMMON_HTTP_PASS)
        
        def __generate(self, provinfo):
                """
                Entry point to generate the provisioned configuration for
                this phone.
                """
                __model = self.phone["model"]
                __macaddr = self.phone["macaddr"].lower().replace(':','')
                template_file = open(pgc['templates_dir'] + "linksys-" + __model + ".cfg")
                template_lines = template_file.readlines()
                template_file.close()
                tmp_filename = LINKSYS_COMMON_DIR + __model + '-' + __macaddr + ".cfg.tmp"
                cfg_filename = tmp_filename[:-4]
                txt = xivo_config.txtsubst(template_lines,
                        { "user_display_name": provinfo["name"],
                          "user_phone_ident":  provinfo["ident"],
                          "user_phone_number": provinfo["number"],
                          "user_phone_passwd": provinfo["passwd"],
                          "asterisk_ipv4" : pgc['asterisk_ipv4'],
                          "ntp_server_ipv4" : pgc['ntp_server_ipv4'],
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
                        { "name":   "guest",
                          "ident":  "guest",
                          "number": "guest",
                          "passwd": "guest"
                        })
        
        # Introspection entry points
        
        @classmethod
        def get_phones(cls):
                "Report supported phone models for this vendor."
                return (("spa901", "SPA901"),
                        ("spa921", "SPA921"),
                        ("spa922", "SPA922"),
                        ("spa941", "SPA941"),
                        ("spa942", "SPA942"),
                        ("spa962", "SPA962"),
                        ("pap2t", "PAP2T"))
        
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
                
                ua_splitted = ua.split('/', 1)
                if ua_splitted[0] != 'Linksys':
                        return None
                model = 'unknown'
                fw = 'unknown'
                if len(ua_splitted) == 2:
                        modelfw = ua_splitted[1].split('-', 1)
                        model = modelfw[0].lower()
                        if len(modelfw) == 2:
                                fw = modelfw[1]
                return ("linksys", model, fw)

xivo_config.PhoneClasses["linksys"] = LinksysProv
