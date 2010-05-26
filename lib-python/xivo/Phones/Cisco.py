"""Support for Linksys phones for XIVO Configuration

Cisco 79X1 are supported.

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
import socket

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.Cisco") # pylint: disable-msg=C0103


class Cisco(PhoneVendorMixin):

    CISCO_MODELS = (('cp7912g', '7912'),
                    ('cp7940g', '7940'),
                    ('cp7960g', '7960'),
                    ('cp7941g', '7941GE'),
                    ('cp7961g', '7961GE'),
                    ('cpip', 'CPIP'))

    CISCO_COMMON_HTTP_USER = "admin"
    CISCO_COMMON_HTTP_PASS = ""

    CISCO_COMMON_DIR = None
    
    CISCO_LOCALES = {
        'en': {
            # name of locales directory (/tftpboot/Cisco/XXX)
            'name'         : 'English_United_States',
            'langCode'     : 'en',
            # dial tones
            'networkLocale': 'United_States',
        },
        'fr': {
            'name'         : 'French_France',
            'langCode'     : 'fr',
            'networkLocale': 'France',
        },
    }

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.CISCO_COMMON_DIR = "/tftpboot/"

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in [x[0] for x in self.CISCO_MODELS]:
            raise ValueError, "Unknown Cisco model %r" % self.phone['model']

    def __action(self, command, user, passwd):
        if self.phone['proto'] == 'sccp' and command == 'REBOOT':
            # a phone is reconfigured by reloading chan_sccp configuration
            # send to commands to asterisk through CTI server remote protocol

            # WARNING: reloading sccp channel disconnect all SCCP phones 
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_IP)
                s.connect(('127.0.0.1', 5004))
                s.send('module unload chan_sccp.so')
                s.close()

                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_IP)
                s.connect(('127.0.0.1', 5004))
                s.send('module load chan_sccp.so')
                s.close()
            except Exception:
                log.exception("error when trying to reload chan_sccp")
        
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
        self.__action("REBOOT", self.CISCO_COMMON_HTTP_USER, self.CISCO_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action("REBOOT", self.CISCO_COMMON_HTTP_USER, self.CISCO_COMMON_HTTP_PASS)

    def __generate(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].replace(":", "").upper()
        fromdhcp = 0

        if self.phone.get('from') == 'dhcp':
            fromdhcp = 1

        try:
            template_specific_path = os.path.join(self.CISCO_COMMON_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            template_common_path = os.path.join(self.CISCO_COMMON_DIR, "templates", "cisco-" + model + ".cfg")

            if not os.access(template_common_path, os.R_OK):
                prefix = ''
                if provinfo['proto'] != 'sip':
                    prefix = provinfo['proto'] + '-'
                    
                template_common_path = os.path.join(self.TEMPLATES_DIR, prefix + "cisco-" + model + ".cfg")

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      template_common_path)
            template_file = open(template_common_path)

        template_lines = template_file.readlines()
        template_file.close()

        if provinfo['proto'] == 'sccp':
            tmp_filename = os.path.join(self.CISCO_COMMON_DIR, "SEP" + macaddr + ".cnf.xml.tmp")
            cfg_filename = os.path.join(self.CISCO_COMMON_DIR, "SEP" + macaddr + ".cnf.xml")

            exten_pickup_prefix        = ''
            function_keys_config_lines = ''
        else:
            tmp_filename = os.path.join(self.CISCO_COMMON_DIR, "SIP" + macaddr + ".cnf.tmp")
            cfg_filename = os.path.join(self.CISCO_COMMON_DIR, "SIP" + macaddr + ".cnf")

            exten_pickup_prefix = \
                clean_extension(provinfo['extensions']['pickupexten']) + "#"

            function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'], model, provinfo)

        if bool(int(provinfo.get('subscribemwi', 0))):
            provinfo['vmailaddr'] = "%s@%s" % (provinfo['number'], self.ASTERISK_IPV4)
        else:
            provinfo['vmailaddr'] = ""
            
        ## sccp:: addons
        addons = ['\n']
        if 'addons' in provinfo and len(provinfo['addons']) > 0:
            log.debug("addons= %s", provinfo['addons'])
            addons_tpl_path = os.path.join(self.TEMPLATES_DIR, "sccp-cisco-addons.cfg")
            if not os.access(addons_tpl_path, os.R_OK):
                log.debug("Could not open cisco addons template %r (errno: %r, errstr: %r).",
                    addons_tpl_path, errno, errstr)

            addons_tpl_file = open(addons_tpl_path)
            addons_tpl      = addons_tpl_file.readlines()
            addons_tpl_file.close()
            log.debug(addons_tpl)

            _addons = provinfo['addons'].split(',')
            for i in xrange(len(_addons)):
                addons.extend(xivo_config.txtsubst(addons_tpl, 
                    {'index': str(i), 'firmware': ''}, None, 'utf8'))

        addons = ''.join(addons)

        ## sccp:: language
        language = 'en'
        if 'language' in provinfo and provinfo['language'] in self.CISCO_LOCALES:
            language = provinfo['language']
        language = """
 <userLocale>
  <name>%(name)s</name>
  <langCode>%(langCode)s</langCode>
 </userLocale>
 <networkLocale>%(networkLocale)s</networkLocale>
""" % self.CISCO_LOCALES[language]


        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'user_vmail_addr':        provinfo['vmailaddr'],
                      'exten_pickup_prefix':    exten_pickup_prefix,
                      'function_keys':          function_keys_config_lines,
                      'addons':                 addons,
                      'language':               language,
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
        if model not in ('cisco'):
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
        return tuple([(x[0], x[0].upper()) for x in cls.CISCO_MODELS])

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """

	# Cisco-CP7941G-GE/8.0
	# Cisco-CP7961G-GE/8.0
	# Cisco-CP7940G/8.0
	# Cisco-CP7960G/8.0

	ua_splitted = ua.split("-", 2)
	if ua_splitted[0] != 'Cisco':
	    return None
	model = 'unknown'
	fw = 'unknown'
	if len(ua_splitted) == 3:
	    fws = ua_splitted[2].split("/", 1)
	    fw = fws[1]
	    model = ua_splitted[1].lower()
	elif len(ua_splitted) == 2:
	    fws = ua_splitted[1].split("/", 1)
	    fw = fws[1]
	    model = fws[0].lower()
        return ("cisco", model, fw)

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        return

    @classmethod
    def get_dhcp_pool_lines(cls):
        return

    @classmethod
    def get_sccp_devicetype(cls, model):
        """Used to get devicetype (see sccp.conf asterisk configuration file)
           from cisco phone model
        """
        model = filter(lambda x: x[0] == model, cls.CISCO_MODELS)
        if len(model) == 0:
            return None

        return model[0][1]

