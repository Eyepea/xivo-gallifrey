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
from copy import deepcopy

from xivo import tzinform
from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.Cisco") # pylint: disable-msg=C0103


_ZONE_MAP = {
    'Etc/GMT+12': 'Dateline Standard Time',
    'Pacific/Samoa': 'Samoa Standard Time ',
    'US/Hawaii': 'Hawaiian Standard Time ',
    'US/Alaska': 'Alaskan Standard/Daylight Time',
    'US/Pacific': 'Pacific Standard/Daylight Time',
    'US/Mountain': 'Mountain Standard/Daylight Time',
    'Etc/GMT+7': 'US Mountain Standard Time',
    'US/Central': 'Central Standard/Daylight Time',
    'America/Mexico_City': 'Mexico Standard/Daylight Time',
#    '': 'Canada Central Standard Time',
#    '': 'SA Pacific Standard Time',
    'US/Eastern': 'Eastern Standard/Daylight Time',
    'Etc/GMT+5': 'US Eastern Standard Time',
    'Canada/Atlantic': 'Atlantic Standard/Daylight Time',
    'Etc/GMT+4': 'SA Western Standard Time',
    'Canada/Newfoundland': 'Newfoundland Standard/Daylight Time',
    'America/Sao_Paulo': 'South America Standard/Daylight Time',
    'Etc/GMT+3': 'SA Eastern Standard Time',
    'Etc/GMT+2': 'Mid-Atlantic Standard/Daylight Time',
    'Atlantic/Azores': 'Azores Standard/Daylight Time',
    'Europe/London': 'GMT Standard/Daylight Time',
    'Etc/GMT': 'Greenwich Standard Time',
#    'Europe/Belfast': 'W. Europe Standard/Daylight Time',
#    '': 'GTB Standard/Daylight Time',
    'Egypt': 'Egypt Standard/Daylight Time',
    'Europe/Athens': 'E. Europe Standard/Daylight Time',
#    'Europe/Rome': 'Romance Standard/Daylight Time',
    'Europe/Paris': 'Central Europe Standard/Daylight Time',
    'Africa/Johannesburg': 'South Africa Standard Time ',
    'Asia/Jerusalem': 'Jerusalem Standard/Daylight Time',
    'Asia/Riyadh': 'Saudi Arabia Standard Time',
    'Europe/Moscow': 'Russian Standard/Daylight Time', # Russia covers 8 time zones.
    'Iran': 'Iran Standard/Daylight Time',
#    '': 'Caucasus Standard/Daylight Time',
    'Etc/GMT-4': 'Arabian Standard Time',
    'Asia/Kabul': 'Afghanistan Standard Time ',
    'Etc/GMT-5': 'West Asia Standard Time',
#    '': 'Ekaterinburg Standard Time',
    'Asia/Calcutta': 'India Standard Time',
    'Etc/GMT-6': 'Central Asia Standard Time ',
    'Etc/GMT-7': 'SE Asia Standard Time',
#    '': 'China Standard/Daylight Time', # China doesn't observe DST since 1991
    'Asia/Taipei': 'Taipei Standard Time',
    'Asia/Tokyo': 'Tokyo Standard Time',
    'Australia/ACT': 'Cen. Australia Standard/Daylight Time',
    'Australia/Brisbane': 'AUS Central Standard Time',
#    '': 'E. Australia Standard Time',
#    '': 'AUS Eastern Standard/Daylight Time',
    'Etc/GMT-10': 'West Pacific Standard Time',
    'Australia/Tasmania': 'Tasmania Standard/Daylight Time',
    'Etc/GMT-11': 'Central Pacific Standard Time',
    'Etc/GMT-12': 'Fiji Standard Time',
#    '': 'New Zealand Standard/Daylight Time',
}

def _gen_tz_map():
    result = {}
    for tz_name, param_value in _ZONE_MAP.iteritems():
        inform = tzinform.get_timezone_info(tz_name)
        inner_dict = result.setdefault(inform['utcoffset'].as_minutes, {})
        if not inform['dst']:
            inner_dict[None] = param_value
        else:
            inner_dict[inform['dst']['as_string']] = param_value
    return result

class Cisco(PhoneVendorMixin):

    CISCO_MODELS = (('cp7906g', '7906'),
                    ('cp7911g', '7911'),
                    ('cp7912g', '7912'),
                    ('cp7931g', '7931'),
                    ('cp7940g', '7940'),
                    ('cp7941g', '7941GE'),
                    ('cp7942g', '7942'),
                    ('cp7945g', '7945'),
                    ('cp7960g', '7960'),
                    ('cp7961g', '7961GE'),
                    ('cp7962g', '7962'),
                    ('cp7965g', '7965'),
                    ('cp7970g', '7970'),
                    ('cp7971g', '7971GE'),
                    ('cp7975g', '7975'),
                    ('cipc', 'cipc'))

    # Define the capacities of each model with a profile
    CISCO_PROFILE_CAPACITIES = {'default': {'sccp': {'prefix': 'SEP', 'suffix': '.cnf.xml', 'reboot': 'sccp reload', 'compile': False, 'lower': False},
                                            'sip':  {'prefix': 'SIP', 'suffix': '.cnf', 'reboot': False, 'compile': False, 'lower': False}}}

    CISCO_PROFILE_CAPACITIES['xmljava'] = deepcopy(CISCO_PROFILE_CAPACITIES['default'])
    CISCO_PROFILE_CAPACITIES['xmljava']['sip'] = {'prefix': 'SEP', 'suffix': '.cnf.xml', 'reboot': False, 'compile': False, 'lower': False}
    CISCO_PROFILE_CAPACITIES['gk'] = deepcopy(CISCO_PROFILE_CAPACITIES['default'])
    CISCO_PROFILE_CAPACITIES['gk']['sip'] = {'prefix': 'gk', 'suffix': '.txt', 'reboot': 'sip notify check-sync', 'compile': 'cfgfmt', 'lower': True}

    CISCO_CAPACITIES = {'cp7906g': CISCO_PROFILE_CAPACITIES['xmljava'],
                        'cp7911g': CISCO_PROFILE_CAPACITIES['xmljava'],
                        'cp7912g': CISCO_PROFILE_CAPACITIES['gk'],
                        'cp7931g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7940g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7941g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7942g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7945g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7960g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7961g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7962g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7965g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7970g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7971g': CISCO_PROFILE_CAPACITIES['default'],
                        'cp7975g': CISCO_PROFILE_CAPACITIES['default'],
                        'cipc':    CISCO_PROFILE_CAPACITIES['default']}

    CISCO_COMPILER_DIR = "/usr/local/bin/"

    CISCO_COMMON_HTTP_USER = "admin"
    CISCO_COMMON_HTTP_PASS = ""

    CISCO_COMMON_DIR = None
    
    CISCO_LOCALES = {
        'de_DE': {
            'name': 'german_germany',
            'langCode': 'de',
            'networkLocale': 'germany'
        },
        'en_US': {
            'name': 'english_united_states',
            'langCode': 'en',
            'networkLocale': 'united_states'
        },
        'es_ES': {
            'name': 'spanish_spain',
            'langCode': 'es',
            'networkLocale': 'spain'
        },
        'fr_FR': {
            'name': 'french_france',
            'langCode': 'fr',
            'networkLocale': 'france'
        },
        'fr_CA': {
            'name': 'french_france',
            'langCode': 'fr',
            'networkLocale': 'canada'
        }
    }
    
    CISCO_TZ_MAP = _gen_tz_map()

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
        if command == 'REBOOT':
            capacities = self.CISCO_CAPACITIES[self.phone['model']][self.phone['proto']]
            if capacities['reboot'] != False:
                ast_cmd = capacities['reboot']
                if ast_cmd == 'sip notify check-sync':
                    ast_cmd += ' '+self.phone['ipv4']
                print(ast_cmd)
                # a sccp phone is reconfigured by reloading chan_sccp configuration
                # send to commands to asterisk through CTI server remote protocol
                try:
                    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_IP)
                    s.connect(('127.0.0.1', 5004))
                    s.send(ast_cmd)
                    s.close()
                except Exception:
                    log.exception("error when trying to launch asterisk command")

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action("REBOOT", self.CISCO_COMMON_HTTP_USER, self.CISCO_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action("REBOOT", self.CISCO_COMMON_HTTP_USER, self.CISCO_COMMON_HTTP_PASS)

    def __generate(self, provinfo, dry_run):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].replace(":", "").upper()
        capacities = self.CISCO_CAPACITIES[model][provinfo['proto']]
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

        prov_filename = capacities['prefix'] + macaddr + capacities['suffix']
        if capacities['lower'] == True:
            prov_filename = prov_filename.lower()

        tmp_filename = os.path.join(self.CISCO_COMMON_DIR, prov_filename + ".tmp")
        cfg_filename = os.path.join(self.CISCO_COMMON_DIR, prov_filename)

        if provinfo['proto'] == 'sccp':
            exten_pickup_prefix        = ''
            function_keys_config_lines = ''
        else:
            exten_pickup_prefix = \
                clean_extension(provinfo['extensions']['pickupexten']) + "#"

            function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'], model, provinfo)

        if provinfo.get('subscribemwi', 0) != None and bool(int(provinfo.get('subscribemwi', 0))):
            provinfo['vmailaddr'] = "%s@%s" % (provinfo['number'], self.ASTERISK_IPV4)
        else:
            provinfo['vmailaddr'] = ""

        ## sccp:: addons
        addons = ['\n']
        if 'addons' in provinfo and provinfo['addons'] is not None and len(provinfo['addons']) > 0:
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
        if 'language' in provinfo and provinfo['language'] in self.CISCO_LOCALES:
            locale = provinfo['language']
        else:
            locale = self.DEFAULT_LOCALE
        language = """\
 <userLocale>
  <name>Cisco/i18n/%(name)s</name>
  <langCode>%(langCode)s</langCode>
 </userLocale>
 <networkLocale>Cisco/i18n/%(networkLocale)s</networkLocale>\
 """ % self.CISCO_LOCALES[locale]
 
        if 'timezone' in provinfo:
            timezone = provinfo['timezone']
        else:
            timezone = self.DEFAULT_TIMEZONE
        timezone_value = self._timezone_name_to_value(timezone)
        inform = tzinform.get_timezone_info(timezone)
        utcoffset_m = inform['utcoffset'].as_minutes

        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    { 'user_vmail_addr':        provinfo['vmailaddr'],
                      'exten_pickup_prefix':    exten_pickup_prefix,
                      'function_keys':          function_keys_config_lines,
                      'addons':                 addons,
                      'language':               language,
                      'timezone':               timezone_value,
                      'timezone_integer':       str(utcoffset_m),
                    },
                    clean_extension),
                cfg_filename,
                'utf8')

        if fromdhcp:
            if os.path.exists(cfg_filename):
                return

        if dry_run:
            return ''.join(txt)
        else:
            self._write_cfg(tmp_filename, cfg_filename, txt)
            if capacities['compile'] == 'cfgfmt':
                compile_filename = capacities['prefix'] + macaddr
                if capacities['lower'] == True:
                    compile_filename = compile_filename.lower()
                try:
                    subprocess.check_call([self.CISCO_COMPILER_DIR+capacities['compile'],
                                    "-t"+self.CISCO_COMPILER_DIR+'sip_ptag.dat',
                                    cfg_filename,
                                    os.path.join(self.CISCO_COMMON_DIR, compile_filename)],
                                    close_fds = True)
                except OSError:
                    log.exception("error when trying to call "+capacities['compile'])
                except subprocess.CalledProcessError:
                    log.exception("error to compile with: "+capacities['compile']+'. Error code: '+str(CalledProcessError.returncode))

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
    
    @classmethod
    def _timezone_name_to_value(cls, timezone):
        inform = tzinform.get_timezone_info(timezone)
        utcoffset_m = inform['utcoffset'].as_minutes
        if utcoffset_m not in cls.CISCO_TZ_MAP:
            # No UTC offset matching. Let's try finding one relatively close...
            for supp_offset in (30, -30, 60, -60):
                if utcoffset_m + supp_offset in cls.CISCO_TZ_MAP:
                    utcoffset_m += supp_offset
                    break
            else:
                return "Central Europe Standard/Daylight Time"
            
        dst_map = cls.CISCO_TZ_MAP[utcoffset_m]
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
            try:
                fw = fws[1]
                model = ua_splitted[1].lower()
            except IndexError:
                fws = ua_splitted[1].split("/", 1)
                fw = fws[1]
                model = fws[0].lower()+'g'
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

