"""Support for Thomson phones for XIVO Configuration

Thomson 2022S and 2030S are supported.

Copyright (C) 2007-2009  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007-2009  Proformatique

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
import time
import logging
import telnetlib

from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin


log = logging.getLogger("xivo.Phones.Thomson") # pylint: disable-msg=C0103


class TelnetExpectationFailed(RuntimeError):
    """
    Exception raised by the new methods introduced by the
    telnetlib.Telnet extending TimeoutingTelnet class.
    """
    pass


class TimeoutingTelnet(telnetlib.Telnet):
    """
    This class extends Telnet so that a global timeout can occur
    during the newly introduced read_until_to().  An exception will
    be raised when that happens.
    """
    def __init__(self, cnx, global_TO):
        if type(cnx) != tuple or len(cnx) < 1:
            raise ValueError, "The cnx argument must be (peer,) or (peer, port) ; %s was given" % str(cnx)
        elif len(cnx) < 2:
            telnetlib.Telnet.__init__(self, cnx[0])
        else:
            # pylint: disable-msg=W0233
            telnetlib.Telnet.__init__(self, cnx[0], cnx[1])
        self.__my_global_to = global_TO
        self.__my_global_to_start = None
        self.__my_cnx = cnx

    def restart_global_to(self):
        "Start / reset the global TO timer for this telnet session"
        self.__my_global_to_start = time.time()

    def stop_global_to(self):
        "Stop the global TO timer for this telnet session"
        self.__my_global_to_start = None

    def read_until_to(self, expected):
        """
        Same as read_until() but if expected has not been received a
        TelnetExpectationFailed will be raised - this will be the case
        when the session global timer is hit.
        """
        if self.__my_global_to_start is not None:
            remaining_time = (self.__my_global_to_start + self.__my_global_to) - time.time()
            if remaining_time <= 0:
                raise TelnetExpectationFailed, "Telnet session already timeouted for peer %s" % str(self.__my_cnx)
            gotstr = self.read_until(expected, remaining_time)
            if expected in gotstr:
                return gotstr
            raise TelnetExpectationFailed, "Telnet session timeouted for peer %s - the expected string '%s' has not yet been received" % (str(self.__my_cnx), expected)
        else:
            gotstr = self.read_until(expected)
            if expected in gotstr:
                return gotstr
            raise TelnetExpectationFailed, "Expected string '%s' has not been received before termination of the telnet session with peer %s" % (expected, str(self.__my_cnx))


# NOTES:
# ~/etc/dhcpd3/dhcpd.conf -> /tftpboot/Thomson/ST2030S_v1.53.inf
#                               -> /tftpboot/Thomson/ST2030S_common_v1.53.txt
#                               -> /tftpboot/Thomson/binary/v2030SG.070309.1.53.zz
#                               -> ...also firmware and other global
#                                       config files like ringing tones...


class Thomson(PhoneVendorMixin):

    THOMSON_MODELS = (('2022s', 'ST2022S'),
                      ('2030s', 'ST2030S'))

    THOMSON_USER = "admin"          # XXX
    THOMSON_PASSWD = "superpass"    # XXX

    THOMSON_COMMON_DIR = None
    THOMSON_COMMON_INF = None
    THOMSON_SPEC_TXT_TEMPLATE = None
    THOMSON_SPEC_TXT_BASENAME = None

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.THOMSON_COMMON_DIR = os.path.join(cls.TFTPROOT, "Thomson/")
        cls.THOMSON_COMMON_INF = os.path.join(cls.THOMSON_COMMON_DIR, "ST") # + "2030S_common"
        cls.THOMSON_SPEC_TXT_TEMPLATE = os.path.join(cls.TEMPLATES_DIR, "ST") # + "2030S_template.txt"
        cls.THOMSON_SPEC_TXT_BASENAME = os.path.join(cls.TFTPROOT, "ST") # + "2030S_template.txt"

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in [x[0] for x in self.THOMSON_MODELS]:
            raise ValueError, "Unknown Thomson model %r" % self.phone['model']

    @staticmethod
    def __generate_timestamp():
        tuple_time = time.localtime()
        seximin = tuple_time[3] * 360 + tuple_time[4] * 6 + int(tuple_time[5] / 10)
        return "%04d%02d%02d%04d" % (tuple_time[0], tuple_time[1], tuple_time[2], seximin)

    def __configurate_telnet(self, user, passwd):
        self.user = user
        self.passwd = passwd

    def __action(self, commands):
        ip = self.phone['ipv4']
        tn = TimeoutingTelnet((ip,), self.TELNET_TO_S)
        tn.restart_global_to()
        try:
            tn.read_until_to("Login: ")
            tn.write(self.user + "\r\n")
            if self.passwd:
                tn.read_until_to("Password: ")
                tn.write(self.passwd + "\r\n")
            for cmd in commands:
                tn.read_until_to("[" + self.user + "]#")
                log.debug("sending telnet command (%s): %s", self.phone['macaddr'], cmd)
                tn.write("%s\n" % cmd)
                if cmd == 'reboot':
                    break
        finally:
            tn.close()

    @staticmethod
    def __format_function_keys(funckey):
        sorted_keys = funckey.keys()
        sorted_keys.sort()
        fk_config_lines = []
        for key in sorted_keys:
            exten, supervise = funckey[key]
            fk_config_lines.append("FeatureKeyExt%02d=%s/<sip:%s>" % (int(key), 'LS'[supervise], exten))
        return "\n".join(fk_config_lines)

    def __generate(self, provinfo):
        model = self.phone['model'].upper()
        macaddr = self.phone['macaddr'].replace(":", "").upper()

        try:
            txt_template_specific_path = os.path.join(self.THOMSON_COMMON_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", txt_template_specific_path)
            txt_template_file = open(txt_template_specific_path)
        except IOError, (errno, errstr):
            txt_template_common_path = os.path.join(self.THOMSON_COMMON_DIR, "templates", "ST" + model + "_template.txt")

            if not os.access(txt_template_common_path, os.R_OK):
                txt_template_common_path = self.THOMSON_SPEC_TXT_TEMPLATE + model + "_template.txt"

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      txt_template_specific_path,
                      errno,
                      errstr,
                      txt_template_common_path)
            txt_template_file = open(txt_template_common_path)

        txt_template_lines = txt_template_file.readlines()
        txt_template_file.close()
        tmp_filename = self.THOMSON_SPEC_TXT_BASENAME + model + "_" + macaddr + ".txt.tmp"
        txt_filename = tmp_filename[:-4]

        multilines = str(provinfo['simultcalls'])
        function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'])

        txt = xivo_config.txtsubst(txt_template_lines,
                { 'user_display_name':  provinfo['name'],
# THOMSON BUG #1
# provinfo['number'] is volontarily not set in "TEL1Number" because Thomson
# phones authentify with their telnumber.. :/
                  'user_phone_ident':   provinfo['ident'],
                  'user_phone_number':  provinfo['number'],
                  'user_phone_passwd':  provinfo['passwd'],
                  'user_vmail_addr':    provinfo['vmailaddr'],
                  'asterisk_ipv4':      self.ASTERISK_IPV4,
                  'ntp_server_ipv4':    self.NTP_SERVER_IPV4,
                  'simultcalls':        multilines,
                  # <WARNING: THIS FIELD MUST STAY IN LOWER CASE IN THE TEMPLATE AND MAC SPECIFIC FILE>
                  'config_sn':          self.__generate_timestamp(),
                  # </WARNING>
                  'function_keys':      function_keys_config_lines,
                },
                txt_filename,
                'utf8')

        tmp_file = open(tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(tmp_filename, txt_filename) # atomically update the file
        inf_filename = self.THOMSON_SPEC_TXT_BASENAME + model + "_" + macaddr + ".inf"
        try:
            os.lstat(inf_filename)
            os.unlink(inf_filename)
        except Exception: # XXX: OsError?
            pass
        os.symlink(self.THOMSON_COMMON_INF + model, inf_filename)

    # Daemon entry points for configuration generation and issuing commands

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__configurate_telnet(self.THOMSON_USER, self.THOMSON_PASSWD)
        self.__action(('reboot',))

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
# THOMSON BUG #2
# We are waiting for Thomson to correctly reload their common .txt file after a
# reinit; until then we don't really reinit the phone but just put it in the
# guest state, by its mac specific configuration
        self.__configurate_telnet(self.THOMSON_USER, self.THOMSON_PASSWD)
#               self.__action(('sys set rel 0', 'ffs format', 'ffs commit', 'ffs commit', 'reboot', 'quit'))
        self.__action(('reboot',))

    def do_reinitprov(self):
        """
        Entry point to generate the reinitialized (GUEST)
        configuration for this phone.
        """
        self.__generate(
                { 'name':           "guest",
                  'ident':          "guest",
                  'number':         "guest",
                  'passwd':         "guest",
                  'simultcalls':    "10",
                  'vmailaddr':      "",
                  'funckey':        {},
                })

    def do_autoprov(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        if bool(int(provinfo.get('vmenable', 0))):
            provinfo['vmailaddr'] = self.ASTERISK_IPV4
        else:
            provinfo['vmailaddr'] = ""

        self.__generate(provinfo)

    # Introspection entry points

    @classmethod
    def get_phones(cls):
        "Report supported phone models for this vendor."
        return tuple([(x[0], x[0].upper()) for x in cls.THOMSON_MODELS])

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        if "THOMSON" != ua[:7].upper():
            return None
        splitted_ua = ua.split()
        fw = "unknown"
        if len(splitted_ua) < 2:
            return None
        if splitted_ua[1][:2] == "ST":
            model = splitted_ua[1][2:].lower() + "s"
        else:
            model = splitted_ua[1].lower() + "s"
        if len(splitted_ua) >= 4:
            fw = splitted_ua[3]
        return ("thomson", model, fw)

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for x in cls.THOMSON_MODELS:
            for line in (
                'class "Thomson%s" {\n' % x[1],
                '    match if (option user-class = "Thomson %s"\n' % x[1],
                '              or binary-to-ascii(16,\n',
                '                   8,\n',
                '                   "",\n',
                '                   substring(option vendor-class-identifier, 2, 15)) = "%s");\n' %
                                        '54686f6d736f6e26'.join(["%x" % ord(c) for c in x[1][:-1]]),
                '    log("boot Thomson%s");\n' % x[1],
                '    option bootfile-name "Thomson/%s";\n' % x[1],
                '    option tftp-server-name "%s";\n' % addresses['bootServer'],
                '}\n',
                '\n'):
                yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        for x in cls.THOMSON_MODELS:
            yield '        allow members of "Thomson%s";\n' % x[1]
