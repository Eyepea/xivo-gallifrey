"""Support for Snom phones for XIVO Configuration

Snom 300 320 and 360 are supported.

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
import os.path
import subprocess

from xivo import xivo_config
from xivo.xivo_config import PhoneVendor
from xivo.xivo_config import ProvGeneralConf as pgc
from xivo import except_tb

# SNOM BUGBUG #1
# Snom doesn't support something else than files at root of tftproot when using
# tftp... :/ (It just internaly replaces the first '/' with a '\0' :/// )

# SNOM BUGBUG #2
# Because it seems much technically impossible to detect the phone model by
# dhcp request (model not in the request.... :///), we'll need to also support
# HTTP based xivo_config

SNOM_SPEC_DIR = os.path.join(pgc['tftproot'], "Snom/")
SNOM_SPEC_TEMPLATE = os.path.join(pgc['templates_dir'], "snom-template.htm")
SNOM_COMMON_HTTP_USER = "guest"
SNOM_COMMON_HTTP_PASS = "guest"

class Snom(PhoneVendor):
        
        SNOM_MODELS = ('300', '320', '360')
        
        def __init__(self, phone):
                PhoneVendor.__init__(self, phone)
                # TODO: handle this with a lookup table stored in the DB?
                if self.phone['model'] not in self.SNOM_MODELS:
                        raise ValueError, "Unknown Snom model %r" % self.phone['model']
        
        def __action(self, command, user, passwd):
                try: # XXX: also check return values?
                        
                        ## curl options
                        # -s                    -- silent
                        # -o /dev/null          -- dump result
                        # --connect-timeout 30  -- timeout after 30s
                        # -retry 0              -- don't retry
                        subprocess.call([pgc['curl_cmd'],
                                         "--retry", "0",
                                         "--connect-timeout", str(pgc['curl_to_s']),
                                         "-s",
                                         "-o", "/dev/null",
                                         "-u", "%s:%s" % (user, passwd),
                                         "http://%s/confirm.html?%s=yes" % (self.phone['ipv4'], command)],
                                        close_fds = True)
                except OSError:
                        except_tb.syslog_exception()
        
        def do_reinit(self):
                """
                Entry point to send the (possibly post) reinit command to
                the phone.
                """
                self.__action("RESET", SNOM_COMMON_HTTP_USER, SNOM_COMMON_HTTP_PASS)
        
        def do_reboot(self):
                "Entry point to send the reboot command to the phone."
                self.__action("REBOOT", SNOM_COMMON_HTTP_USER, SNOM_COMMON_HTTP_PASS)
        
        def do_reinitprov(self):
                """
                Entry point to generate the reinitialized (GUEST)
                configuration for this phone.
                """
                htm_filename = os.path.join(SNOM_SPEC_DIR, "snom" + self.phone['model'] + "-" + self.phone['macaddr'].replace(":", "") + ".htm")
                try:
                        os.unlink(htm_filename)
                except OSError:
                        pass
        
        def do_autoprov(self, provinfo):
                """
                Entry point to generate the provisioned configuration for
                this phone.
                """
                template_file = open(SNOM_SPEC_TEMPLATE)
                template_lines = template_file.readlines()
                template_file.close()
                tmp_filename = os.path.join(SNOM_SPEC_DIR, "snom" + self.phone['model'] + "-" + self.phone['macaddr'].replace(":", "") + ".htm.tmp")
                htm_filename = tmp_filename[:-4]
                txt = xivo_config.txtsubst(template_lines,
                        { 'user_display_name': provinfo['name'],
                          'user_phone_ident':  provinfo['ident'],
                          'user_phone_number': provinfo['number'],
                          'user_phone_passwd': provinfo['passwd'],
                          'http_user': SNOM_COMMON_HTTP_USER,
                          'http_pass': SNOM_COMMON_HTTP_PASS,
                        },
                        htm_filename)
                tmp_file = open(tmp_filename, 'w')
                tmp_file.writelines(txt)
                tmp_file.close()
                os.rename(tmp_filename, htm_filename)
        
        # Introspection entry points
        
        @classmethod
        def get_phones(cls):
                "Report supported phone models for this vendor."
                return tuple(map(lambda x: (x, x), cls.SNOM_MODELS))
        
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
                yield 'subclass "phone-mac-address-prefix" 1:00:04:13 {\n'
                yield '    log("class Snom prefix 1:00:04:13");\n'
                yield '    option tftp-server-name "http://%s:8667/";\n' % addresses['bootServer']
                yield '    option bootfile-name "snom.php?mac={mac}";\n'
                yield '    next-server %s;\n' % addresses['bootServer']
                yield '}\n'
                yield '\n'
        
        @classmethod
        def get_dhcp_pool_lines(cls):
                return ()

xivo_config.register_phone_vendor_class(Snom)
