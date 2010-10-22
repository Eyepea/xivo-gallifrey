"""Support for Nortel phones for XIVO Configuration

Nortel IP Phones 1220 and 1230 are supported. Note that these phones have been
rebranded as Avaya 1220 and 1230 IP Deskphones.

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

import logging
import os
import re
import signal
import time
import threading

from xivo import pexpect
from xivo import tzinform
from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.Nortel") # pylint: disable-msg=C0103


class Nortel(PhoneVendorMixin):
    NORTEL_MODELS = ('12x0',)

    NORTEL_COMMON_SSH_USER = "admin"
    NORTEL_COMMON_SSH_PASS = "adminpass"

    NORTEL_COMMON_DIR = None
    
    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.NORTEL_COMMON_DIR = os.path.join(cls.TFTPROOT, 'Nortel')

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in self.NORTEL_MODELS:
            raise ValueError("Unknown Nortel model %r" % self.phone['model'])
        self._cleaned_macaddr = self.phone['macaddr'].replace(":", "").upper()

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self._threaded_factory_reset()

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self._threaded_factory_reset()
        
    def _threaded_factory_reset(self):
        # The phone doesn't seem to always like to be reset while in a
        # call, so we start another thread to do this; this has also
        # some other advantages, like hearing the 'thankyou' prompt.
        t = threading.Thread(target=self._factory_reset)
        t.start()
        
    def _factory_reset(self):
        # The only way to remotely reboot these phones are via a factory reset
        # via SSH. Factory reset has one interesting property: we don't have to
        # increase the version number in the 1220SIP.cfg file to get the phone to
        # reboot next time it will read its config. The bad is that you lose
        # every user setting and info (outbox, inbox, address book, keys, etc).
        #
        #Warning: Permanently added '192.168.32.229' (RSA) to the list of known hosts.
        #1234@192.168.32.229's password: <1234>
        #        
        #
        #Welcome to Avaya problem determination tool.
        #
        #You are connected to IP Phone 1220. 
        #HW version: 313880177D4F1489662A
        #FW version 03.02.16.00MAC Address = 80177D4F1489
        #IP = 192.168.32.229
        #Type "bye" to exit current shell.
        #PDT> reset2factory
        #R2FS->Reset to Default Settings... Are you sure? [Y/N]:  <Y>
        #R2FS->Enter MAC-address:
        #<80177D4F1489>
        #R2FS->Start Reset to Factory Settings...
        #R2FS->The set will be rebooted...
        #Reset to Factory Defaults for Group ID 0xffffffff 
        #reset licensing feature to factory default
        #CTokenManager::tokenRelease: force token release
        #CTokenManager::tokenRelease: token release.
        #licGracePeriodFileRemove: grace period data file is not found
        command = 'ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null %s@%s' % \
                  (self.NORTEL_COMMON_SSH_USER, self.phone['ipv4'])
        child = pexpect.spawn(command, timeout=10)
        try:
            try:
                # XXX add additional logic to manage the case of erroneous input 
                # or when the connection is refused ?
                child.expect('password: ')
                child.sendline(self.NORTEL_COMMON_SSH_PASS)
                child.expect('PDT> ')
                child.sendline('reset2factory')
                child.expect(r'Are you sure?')
                child.sendline('Y')
                child.expect('Enter MAC-address:')
                child.sendline(self._cleaned_macaddr)
                child.expect('Start Reset')
                # The phone won't cleanly reset if you don't wait a small
                # amount of time before closing the ssh connection
                time.sleep(8)
            except pexpect.ExceptionPexpect:
                log.exception("error trying to reset Nortel phone via ssh")
        finally:
            child.close(force=True)

    def do_reinitprov(self, provinfo, dry_run):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        return self._pre_generate(provinfo, dry_run)

    def do_autoprov(self, provinfo, dry_run):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        return self._pre_generate(provinfo, dry_run)
        
    def _pre_generate(self, provinfo, dry_run):
        if self.phone.get('from') == 'dhcp':
            res = self._generate_from_dhcp(provinfo, dry_run)
        else:
            res = self._generate(provinfo, dry_run)
        return res
            
    def _generate_from_dhcp(self, provinfo, dry_run):
        """
        Generate a default device-specific configuration file for the
        phone to autologin in guest if the config file is not already
        there
        """
        cfg_filename = self._get_filenames()[0]
        if not os.path.isfile(cfg_filename):
            return self._generate(provinfo, dry_run)
        
    def _get_filenames(self):
        """
        Return a tuple of (cfg_filename, tmp_filename) for the phone.
        """
        cfg_filename = os.path.join(self.TFTPROOT, "SIP" + self._cleaned_macaddr + ".cfg")
        tmp_filename = cfg_filename + ".tmp"
        return cfg_filename, tmp_filename
        
    def _generate(self, provinfo, dry_run):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self._cleaned_macaddr

        try:
            template_specific_path = os.path.join(self.NORTEL_COMMON_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            template_common_path = os.path.join(self.NORTEL_COMMON_DIR, "templates", "nortel-" + model + ".cfg")

            if not os.access(template_common_path, os.R_OK):
                template_common_path = os.path.join(self.TEMPLATES_DIR, "nortel-" + model + ".cfg")

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      template_common_path)
            template_file = open(template_common_path)

        template_lines = template_file.readlines()
        template_file.close()
        cfg_filename, tmp_filename = self._get_filenames()
            
        if self.PROXY_BACKUP:
            backup_proxy = 'SERVER_IP1_2 %s' % self.PROXY_BACKUP
        else:
            backup_proxy = ''
        
        if 'timezone' in provinfo:
            inform = tzinform.get_timezone_info(provinfo['timezone'])
            timezone = "TIMEZONE_OFFSET %d" % inform['utcoffset'].as_seconds
        else:
            timezone = ''
        
        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'backup_proxy':           backup_proxy,
                      'timezone':               timezone,
                    },
                    format_extension=clean_extension),
                cfg_filename,
                'utf8')

        if dry_run:
            return ''.join(txt)
        else:
            self._write_cfg(tmp_filename, cfg_filename, txt)

    # Introspection entry points

    @classmethod
    def get_phones(cls):
        "Report supported phone models for this vendor."
        return tuple((x, x.upper()) for x in cls.NORTEL_MODELS)

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        # 3.2 1220 UA: Nortel IP Phone 11xx (SIP12x0e.03.02.16.00)
        
        m = re.match(r'^Nortel IP Phone \w+ \(SIP12x0\D*(\d+)\.(\d+)', ua)
        if m:
            return ('nortel', '12x0', '.'.join(m.group(gn).lstrip('0') for gn in [1, 2]))
        return None

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for line in (
            'class "Nortel-unknown" {',
            '    match if substring(option vendor-class-identifier, 0, 6) = "Nortel";',
            '    log(concat("[", binary-to-ascii(16, 8, ":", hardware), "] ", "BOOT Nortel UNKNOWN"));',
            '    option tftp-server-name = config-option VOIP.tftp-server-name;',
            '    if substring(option vendor-class-identifier, 0, 10) = "Nortel-SIP" {',
            '        execute("/usr/share/pf-xivo-provisioning/bin/dhcpconfig",',
            '                "-f",',
            '                "Nortel IP Phone 11xx (SIP12x0e.03.02.16.00)",',
            '                binary-to-ascii(10, 8, ".", leased-address),',
            '                binary-to-ascii(16, 8, ":", suffix(hardware, 6)));',
            '    }',
            '}'):
            yield line + '\n'

    @classmethod
    def get_dhcp_pool_lines(cls):
        yield '        allow members of "Nortel-unknown";\n'

