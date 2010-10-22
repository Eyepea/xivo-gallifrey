"""Support for Thomson phones for XIVO Configuration

Thomson 2022S and 2030S are supported.

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
import time
import logging
import telnetlib

from xivo import tzinform
from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

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


_ZONE_LIST = [
    'Pacific/Kwajalein',    # Eniwetok, Kwajalein
    'Pacific/Midway',       # Midway Island, Samoa
    'US/Hawaii',            # Hawaii
    'US/Alaska',            # Alaska
    'US/Pacific',           # Pacific Time(US & Canada); Tijuana
    'US/Arizona',           # Arizona
    'US/Mountain',          # Mountain Time(US & Canada)
    'US/Central',           # Central Time(US & Canada)
    'America/Tegucigalpa',  # Mexico City, Tegucigalpa (!)
    'Canada/Saskatchewan',  # Central America, Mexico City,Saskatchewan (!)
    'America/Bogota',       # Bogota, Lima, Quito
    'US/Eastern',           # Eastern Time(US & Canada)
    'US/East-Indiana',      # Indiana(East)
    'Canada/Atlantic',      # Atlantic Time (Canada)
    'America/La_Paz',       # Caracas, La Paz
    'Canada/Newfoundland',  # Newfoundland
    'America/Sao_Paulo',    # Brasilia
    'America/Argentina/Buenos_Aires',   # Buenos Aires, Georgetown
    'Atlantic/South_Georgia',           # Mid-Atlantic
    'Atlantic/Azores',      # Azores, Cape Verde Is
    'Africa/Casablanca',    # Casablanca, Monrovia    (!)
    'Europe/London',        # Greenwich Mean Time: Dublin, Edinburgh, Lisbon, London
    'Europe/Paris',         # Amsterdam, Copenhagen, Madrid, Paris, Vilnius
    'Europe/Belgrade',      # Central Europe Time(Belgrade, Sarajevo, Skopje, Sofija, Zagreb) (?)
    'Europe/Bratislava',    # Bratislava, Budapest, Ljubljana, Prague, Warsaw
    'Europe/Brussels',      # Brussels, Berlin, Bern, Rome, Stockholm, Vienna
    'Europe/Athens',        # Athens, Istanbul, Minsk
    'Europe/Bucharest',     # Bucharest
    'Africa/Cairo',         # Cairo
    'Africa/Harare',        # Harare, Pretoria
    'Europe/Helsinki',      # Helsinki, Riga, Tallinn
    'Israel',               # Israel
    'Asia/Baghdad',         # Baghdad, Kuwait, Riyadh
    'Europe/Moscow',        # Moscow, St. Petersburg, Volgograd
    'Africa/Nairobi',       # Nairobi
    'Asia/Tehran',          # Tehran
    'Asia/Muscat',          # Abu Dhabi, Muscat
    'Asia/Baku',            # Baku, Tbilisi (!)
    'Asia/Kabul',           # Kabul
    'Asia/Yekaterinburg',   # Ekaterinburg
    'Asia/Karachi',         # Islamabad, Karachi, Tashkent
    'Asia/Calcutta',        # Bombay, Calcutta, Madras, New Delhi
    'Asia/Kathmandu',       # Kathmandu
    'Asia/Almaty',          # Almaty, Dhaka
    'Asia/Colombo',         # Colombo
    'Asia/Rangoon',         # Rangoon
    'Asia/Bangkok',         # Bangkok, Hanoi, Jakarta
    'Asia/Hong_Kong',       # Beijin, Chongqing, Hong Kong, Urumqi
    'Australia/Perth',      # Perth
    'Asia/Urumqi',          # Urumqi,Taipei, Kuala Lumpur, Sinapore
    'Asia/Tokyo',           # Osaka, Sappora, Tokyo
    'Asia/Seoul',           # Seoul
    'Asia/Yakutsk',         # Yakutsk
    'Australia/Adelaide',   # Adelaide
    'Australia/Darwin',     # Darwin
    'Australia/Brisbane',   # Brisbane
    'Australia/Canberra',   # Canberra, Melbourne, Sydney
    'Pacific/Guam',         # Guam, Port Moresby
    'Australia/Hobart',     # Hobart
    'Asia/Vladivostok',     # Vladivostok
    'Asia/Magadan',         # Magadan, Solomon Is., New Caledonia
    'Pacific/Auckland',     # Auckland, Wellington
    'Pacific/Fiji',         # Fiji, Kamchatka, Marshall Is. (!)
    'Pacific/Tongatapu',    # Nuku'alofa
]

def _gen_tz_map():
    result = {}
    for i, tz_name in enumerate(_ZONE_LIST):
        inform = tzinform.get_timezone_info(tz_name)
        inner_dict = result.setdefault(inform['utcoffset'].as_minutes, {})
        if not inform['dst']:
            inner_dict[None] = i
        else:
            inner_dict[inform['dst']['as_string']] = i
    return result

# NOTES:
# ~/etc/dhcpd3/dhcpd.conf -> /tftpboot/Thomson/ST2030S_v1.53.inf
#                               -> /tftpboot/Thomson/ST2030S_common_v1.53.txt
#                               -> /tftpboot/Thomson/binary/v2030SG.070309.1.53.zz
#                               -> ...also firmware and other global
#                                       config files like ringing tones...


class Thomson(PhoneVendorMixin):

    THOMSON_MODELS = (('2022s', 'ST2022S'),
                      ('2030s', 'ST2030S'),
                      ('tb30s', 'TB30S'))

    THOMSON_USER = "admin"          # XXX
    THOMSON_PASSWD = "superpass"    # XXX

    THOMSON_COMMON_DIR = None
    THOMSON_COMMON_INF = None
    THOMSON_SPEC_TXT_TEMPLATE = None
    THOMSON_SPEC_TXT_BASENAME = None
    
    THOMSON_LOCALES = {
        'de_DE': ('3', 'DE'),
        'en_US': ('0', 'US'),
        'es_ES': ('2', 'ES'),
        'fr_FR': ('1', 'FR'),
        'fr_CA': ('1', 'US'),
    }
    
    THOMSON_TZ_MAP = _gen_tz_map()
    
    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.THOMSON_COMMON_DIR = os.path.join(cls.TFTPROOT, "Thomson/")
        cls.THOMSON_COMMON_INF = cls.THOMSON_COMMON_DIR # + "2030S_common"
        cls.THOMSON_SPEC_TXT_TEMPLATE = cls.TEMPLATES_DIR # + "2030S_template.txt"
        cls.THOMSON_SPEC_TXT_BASENAME = cls.TFTPROOT # + "2030S_template.txt"

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
            value = funckey[key]
            exten = value['exten']
            supervise = int(value.get('supervision', 0))
            fk_config_lines.append("FeatureKeyExt%02d=%s/<sip:%s>" % (int(key), 'LS'[supervise], exten))
        return "\n".join(fk_config_lines)

    def __generate(self, provinfo, dry_run):
        model = self.phone['model'].upper()
        macaddr = self.phone['macaddr'].replace(":", "").upper()
        phonetype = "ST"
        if self.phone['model'] == "tb30s":
            phonetype = ""
        
        try:
            txt_template_specific_path = os.path.join(self.THOMSON_COMMON_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", txt_template_specific_path)
            txt_template_file = open(txt_template_specific_path)
        except IOError, (errno, errstr):
            txt_template_common_path = os.path.join(self.THOMSON_COMMON_DIR, "templates", phonetype + model + "_template.txt")

            if not os.access(txt_template_common_path, os.R_OK):
                txt_template_common_path = self.THOMSON_SPEC_TXT_TEMPLATE + phonetype + model + "_template.txt"

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      txt_template_specific_path,
                      errno,
                      errstr,
                      txt_template_common_path)
            txt_template_file = open(txt_template_common_path)

        txt_template_lines = txt_template_file.readlines()
        txt_template_file.close()
        tmp_filename = self.THOMSON_SPEC_TXT_BASENAME + phonetype + model + "_" + macaddr + ".txt.tmp"
        txt_filename = tmp_filename[:-4]

        provinfo['subscribemwi'] = str(int(bool(int(provinfo.get('subscribemwi', 0)))))
        provinfo['simultcalls'] = str(provinfo['simultcalls'])

        function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'])
                
        if 'language' in provinfo and provinfo['language'] in self.THOMSON_LOCALES:
            locale = provinfo['language']
        else:
            locale = self.DEFAULT_LOCALE
        language = self.THOMSON_LOCALES[locale][0]
        country = self.THOMSON_LOCALES[locale][1]
        
        if 'timezone' in provinfo:
            timezone = provinfo['timezone']
        else:
            timezone = self.DEFAULT_TIMEZONE
        zonenum = str(self.__tz_name_to_num(timezone))

# THOMSON BUG #1
# provinfo['number'] is volontarily not set in "TEL1Number" because Thomson
# phones authentify with their telnumber.. :/
        txt = xivo_config.txtsubst(
                txt_template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { # <WARNING: THIS FIELD MUST STAY IN LOWER CASE IN THE TEMPLATE AND MAC SPECIFIC FILE>
                      'config_sn':          self.__generate_timestamp(),
                      # </WARNING>
                      'function_keys':      function_keys_config_lines,
                      'language':           language,
                      'country':            country,
                      'zonenum':            zonenum,
                    },
                    format_extension=clean_extension),
                txt_filename,
                'utf8')

        if dry_run:
            return ''.join(txt)
        else:
            self._write_cfg(tmp_filename, txt_filename, txt)
            inf_filename = self.THOMSON_SPEC_TXT_BASENAME + phonetype + model + "_" + macaddr + ".inf"
            try:
                os.lstat(inf_filename)
                os.unlink(inf_filename)
            except Exception: # XXX: OsError?
                pass
            os.symlink(self.THOMSON_COMMON_INF + phonetype + model, inf_filename)

    @classmethod
    def __tz_name_to_num(cls, timezone):
        inform = tzinform.get_timezone_info(timezone)
        utcoffset_m = inform['utcoffset'].as_minutes
        if utcoffset_m not in cls.THOMSON_TZ_MAP:
            # No UTC offset matching. Let's try finding one relatively close...
            for supp_offset in (30, -30, 60, -60):
                if utcoffset_m + supp_offset in cls.THOMSON_TZ_MAP:
                    utcoffset_m += supp_offset
                    break
            else:
                return 22
            
        dst_map = cls.THOMSON_TZ_MAP[utcoffset_m]
        if inform['dst']:
            dst_key = inform['dst']['as_string']
        else:
            dst_key = None
        if dst_key not in dst_map:
            # No DST rules matching. Fallback on all-standard time or random
            # DST rule in last resort...
            if None in dst_map:
                dst_key = None
            else:
                dst_key = dst_map.keys[0]
        return dst_map[dst_key]
    
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
        if bool(int(provinfo.get('subscribemwi', 0))):
            provinfo['subscribemwi'] = '1'
        else:
            provinfo['subscribemwi'] = '0'

        return self.__generate(provinfo, dry_run)

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
