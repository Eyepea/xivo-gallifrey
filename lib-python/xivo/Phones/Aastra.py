"""Support for Aastra phones for XIVO Configuration

Aastra 6730i, 6731i, 6751i, 6753i, 6755i and 6757i are supported.

Copyright (C) 2008-2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008-2010  Proformatique

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

from xivo import tzinform
from xivo import xivo_config
from xivo.xivo_config import PhoneVendorMixin
from xivo.xivo_helpers import clean_extension

log = logging.getLogger("xivo.Phones.Aastra") # pylint: disable-msg=C0103


class Aastra(PhoneVendorMixin):

    AASTRA_MODELS           = (('6730i', '6730i'),
                               ('6731i', '6731i'),
                               ('6739i', '6739i'),
                               ('6751i', '51i'),
                               ('6753i', '53i'),
                               ('6755i', '55i'),
                               ('6757i', '57i'))
    AASTRA_MACADDR_PREFIX   = ('1:00:08:5d',)
    AASTRA_COMMON_HTTP_USER = 'admin'
    AASTRA_COMMON_HTTP_PASS = '22222'

    AASTRA_COMMON_DIR = None

    AASTRA_DP_SYNTAX_FROM_ASTERISK = (('_', ''),
                                      ('X', 'x'),
                                      ('Z', '[1-9]'),
                                      ('N', '[2-9]'),
                                      ('.', '+'),
                                      ('!', '+'))
    
    AASTRA_LOCALES = {
        'de_DE': 'language: 1\nlanguage 1: i18n/lang_de.txt\ntone set: Germany\ninput language: German',
        'en_US': 'language: 0\ntone set: US\ninput language: English',
        'es_ES': 'language: 1\nlanguage 1: i18n/lang_es.txt\ntone set: Europe\ninput language: Spanish',
        'fr_FR': 'language: 1\nlanguage 1: i18n/lang_fr.txt\ntone set: France\ninput language: French',
        'fr_CA': 'language: 1\nlanguage 1: i18n/lang_fr_ca.txt\ntone set: US\ninput language: French',
    }
    
    AASTRA_DICT = {
        'en': {
            'dict_voicemail':  'Voicemail',
            'dict_forward_unc': 'Unconditional forward',
            'dict_dnd': 'D.N.D',
            'dict_local_directory': 'Directory',
            'dict_callers': 'Callers',
            'dict_services': 'Services',
            'dict_pickup': 'Call pickup',
            'dict_remote_directory': 'Directory',
        },
        'fr': {
            'dict_voicemail':  'Messagerie',
            'dict_forward_unc': 'Renvoi inconditionnel',
            'dict_dnd': 'N.P.D',
            'dict_local_directory': 'Repertoire',
            'dict_callers': 'Appels',
            'dict_services': 'Services',
            'dict_pickup': 'Interception',
            'dict_remote_directory': 'Annuaire',
        },
    }

    @classmethod
    def setup(cls, config):
        "Configuration of class attributes"
        PhoneVendorMixin.setup(config)
        cls.AASTRA_COMMON_DIR = os.path.join(cls.TFTPROOT, "Aastra/")

    def __init__(self, phone):
        PhoneVendorMixin.__init__(self, phone)
        if self.phone['model'] not in [x[0] for x in self.AASTRA_MODELS] \
                                    + [x[1] for x in self.AASTRA_MODELS]:
            raise ValueError, "Unknown Aastra model %r" % self.phone['model']

    def __action(self, user, passwd):
        cnx_to = max_to = max(1, self.CURL_TO_S / 2)
        try: # XXX: also check return values?

            ## curl options
            # -s                    -- silent
            # -o /dev/null          -- dump result
            # --connect-timeout 15  -- timeout connection after 15s
            # --max-time 15         -- timeout after 15s when connected
            # -retry 0              -- don't retry
            # -d DATA               -- post DATA

            # We first make two attempts
            # The first one replies: 401 Unauthorized (Authorization failed)
            # The second one does not fail
            for attempts in 1, 2: # pylint: disable-msg=W0612
                subprocess.call([self.CURL_CMD,
                                 "--retry", "0",
                                 "--connect-timeout", str(cnx_to),
                                 "--max-time", str(max_to),
                                 "-s",
                                 "-o", "/dev/null",
                                 "-u", "%s:%s" % (user, passwd),
                                 "http://%s" % self.phone['ipv4']],
                                close_fds = True)

            # once we have been authenticated, we can POST the appropriate commands

            subprocess.call([self.CURL_CMD,
                             "--retry", "0",
                             "--connect-timeout", str(cnx_to),
                             "--max-time", str(max_to),
                             "-s",
                             "-o", "/dev/null",
                             "-u", "%s:%s" % (user, passwd),
                             "http://%s/reset.html" % self.phone['ipv4'],
                             "-d", "resetOption=0"],
                            close_fds = True)
        except OSError:
            log.exception("error when trying to call curl")

    @staticmethod
    def __format_extension(exten):
        if exten is None:
            return ""

        for (key, val) in Aastra.AASTRA_DP_SYNTAX_FROM_ASTERISK:
            exten = exten.replace(key, val)

        if exten.find('#') != -1:
            exten = '"' + exten + '"'

        return exten

    @classmethod
    def __format_expmod(cls, keynum):
        # XXX you get a weird behavior if you have more than 1 M670i expansion module.
        # For example, if you have a 6757i and you want to set the first key of the
        # second module, you'll have to pick, in the xivo web interface, the key number
        # 91 (30 phone softkeys + 60 M675i expansion module keys + 1) instead of 67.
        # That's because the Aastras support more than one type of expansion module, and they
        # don't have the same number of keys. Since we don't know which one the phone is actually
        # using, we pick the one with the most keys, so every expansion module can be fully
        # used, but this leave a weird behavior for multi-expansion setup when smaller
        # expansion module are used....
        if keynum <= 180:
            return "expmod%d key%d" % ((keynum - 1) // 60 + 1, (keynum - 1) % 60 + 1)
        return None
        
    @classmethod
    def __get_keytype_from_model_and_keynum(cls, model, keynum):
        if model in ("6730i", "6731i"):
            if keynum <= 8:
                return "prgkey%d" % keynum
        elif model in ("6753i"):
            if keynum <= 6:
                return "prgkey%d" % keynum
            else:
                return cls.__format_expmod(keynum - 6)
        elif model in ("6755i"):
            if keynum <= 6:
                return "prgkey%d" % keynum
            else:
                keynum -= 6
                if keynum <= 6:
                    return "softkey%d" % keynum
                else:
                    return cls.__format_expmod(keynum - 6)
        elif model in ("6757i"):
            # The 57i has 6 'top keys' and 6 'bottom keys'. 10 functions are programmable for
            # the top keys and 20 are for the bottom keys.
            if keynum <= 10:
                return "topsoftkey%d" % keynum
            else:
                keynum -= 10
                if keynum <= 20:
                    return "softkey%d" % keynum
                else:
                    return cls.__format_expmod(keynum - 20)
        return None
    
    @classmethod
    def __format_function_keys(cls, funckey, model):
        sorted_keys = funckey.keys()
        sorted_keys.sort()
        fk_config_lines = []
        for key in sorted_keys:
            keytype = cls.__get_keytype_from_model_and_keynum(model, int(key))
            if keytype is not None:
                value = funckey[key]
                exten = value['exten']
                if value.get('supervision'):
                    xtype = "blf"
                else:
                    xtype = "speeddial"
                if 'label' in value and value['label'] is not None:
                    label = value['label']
                else:
                    label = exten
                fk_config_lines.append("%s type: %s" % (keytype, xtype))
                fk_config_lines.append("%s label: %s" % (keytype, label))
                fk_config_lines.append("%s value: %s" % (keytype, exten))
                fk_config_lines.append("%s line: 1" % (keytype,))
        return "\n".join(fk_config_lines)

    def do_reinit(self):
        """
        Entry point to send the (possibly post) reinit command to
        the phone.
        """
        self.__action(self.AASTRA_COMMON_HTTP_USER, self.AASTRA_COMMON_HTTP_PASS)

    def do_reboot(self):
        "Entry point to send the reboot command to the phone."
        self.__action(self.AASTRA_COMMON_HTTP_USER, self.AASTRA_COMMON_HTTP_PASS)

    def __generate(self, provinfo):
        """
        Entry point to generate the provisioned configuration for
        this phone.
        """
        model = self.phone['model']
        macaddr = self.phone['macaddr'].replace(":", "").upper()

        try:
            template_specific_path = os.path.join(self.AASTRA_COMMON_DIR, macaddr + "-template.cfg")
            log.debug("Trying phone specific template %r", template_specific_path)
            template_file = open(template_specific_path)
        except IOError, (errno, errstr):
            template_common_path = os.path.join(self.AASTRA_COMMON_DIR, "templates", "aastra-" + model + ".cfg")

            if not os.access(template_common_path, os.R_OK):
                template_common_path = os.path.join(self.TEMPLATES_DIR, "aastra-" + model + ".cfg")

            log.debug("Could not open phone specific template %r (errno: %r, errstr: %r). Using common template %r",
                      template_specific_path,
                      errno,
                      errstr,
                      template_common_path)
            template_file = open(template_common_path)

        template_lines = template_file.readlines()
        template_file.close()
        tmp_filename = os.path.join(self.AASTRA_COMMON_DIR, macaddr + '.cfg.tmp')
        cfg_filename = tmp_filename[:-4]

        provinfo['subscribemwi'] = str(int(bool(int(provinfo.get('subscribemwi', 0)))))

        exten_pickup_prefix = self.__format_extension(
                clean_extension(provinfo['extensions']['pickupexten']))

        function_keys_config_lines = \
                self.__format_function_keys(provinfo['funckey'], model)
        
        if 'language' in provinfo and provinfo['language'] in self.AASTRA_LOCALES:
            locale = provinfo['language']
        else:
            locale = self.DEFAULT_LOCALE
        language = self.AASTRA_LOCALES[locale]
        
        if 'timezone' in provinfo:
            timezone = self.__format_tz_inform(tzinform.get_timezone_info(provinfo['timezone']))
        else:
            timezone = ''
            
        xvars = {
            'exten_pickup_prefix':    exten_pickup_prefix,
            'function_keys':          function_keys_config_lines,
            'language':               language,
            'timezone':               timezone,
        }
        xvars.update(self.__get_lang_dict(locale))

        txt = xivo_config.txtsubst(
                template_lines,
                PhoneVendorMixin.set_provisioning_variables(
                    provinfo,
                    xvars,
                    format_extension=self.__format_extension),
                cfg_filename,
                'utf8')

        tmp_file = open(tmp_filename, 'w')
        tmp_file.writelines(txt)
        tmp_file.close()
        os.rename(tmp_filename, cfg_filename)
        
    @classmethod
    def __get_lang_dict(cls, locale):
        lang_code = cls.__langage_code_from_locale(locale)
        if lang_code not in cls.AASTRA_DICT:
            lang_code = cls.__langage_code_from_locale(cls.DEFAULT_LOCALE)
        return cls.AASTRA_DICT[lang_code]
        
    @classmethod
    def __langage_code_from_locale(cls, locale):
        return locale.split('_', 1)[0]
        
    @classmethod
    def __format_tz_inform(cls, inform):
        lines = []
        lines.append('time zone name: Custom')
        lines.append('time zone minutes: %d' % -(inform['utcoffset'].as_minutes))
        if inform['dst'] is None:
            lines.append('dst config: 0')
        else:
            lines.append('dst config: 3')
            lines.append('dst minutes: %d' % (min(inform['dst']['save'].as_minutes, 60)))
            if inform['dst']['start']['day'].startswith('D'):
                lines.append('dst [start|end] relative date: 0')
            else:
                lines.append('dst [start|end] relative date: 1')
            lines.extend(cls.__format_dst_change('start', inform['dst']['start']))
            lines.extend(cls.__format_dst_change('stop', inform['dst']['end']))
        return '\n'.join(lines)
    
    @classmethod
    def __format_dst_change(cls, suffix, dst_change):
        lines = []
        lines.append('dst %s month: %d' % (suffix, dst_change['month']))
        lines.append('dst %s hour: %d' % (suffix, min(dst_change['time'].as_hours, 23)))
        if dst_change['day'].startswith('D'):
            lines.append('dst %s day: %s' % (suffix, dst_change['day'][1:]))
        else:
            week, weekday = dst_change['day'][1:].split('.')
            if week == '5':
                lines.append('dst %s week: -1' % suffix)
            else:
                lines.append('dst %s week: %s' % (suffix, week))
            lines.append('dst %s day: %s' % (suffix, weekday))
        return lines

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
        return tuple((x[0], x[0]) for x in cls.AASTRA_MODELS)

    # Entry points for the AGI

    @classmethod
    def get_vendor_model_fw(cls, ua):
        """
        Extract Vendor / Model / FirmwareRevision from SIP User-Agent
        or return None if we don't deal with this kind of Agent.
        """
        # Aastra 53i/2.2.0.166
        # Aastra 55i/2.2.0.166

        if 'aastra' != ua[:6].lower():
            return None
        modelfw = ua[6:].strip().split('/', 1)
        model = 'unknown'
        fw = 'unknown'

        if len(modelfw[0]) > 0:
            model = modelfw[0].lower()
            modict = dict([x[1], x[0]] for x in cls.AASTRA_MODELS)
            model = modict.get(model, model)

            if len(modelfw) == 2:
                fw = modelfw[1]
        return ("aastra", model, fw)

    # Entry points for system configuration

    @classmethod
    def get_dhcp_classes_and_sub(cls, addresses):
        for model, oldmodel in cls.AASTRA_MODELS:
            yield 'class "Aastra%s" {\n' % model

            if model != oldmodel:
                yield '    match if (option vendor-class-identifier = "AastraIPPhone%s")\n' % model
                yield '              or (option vendor-class-identifier = "AastraIPPhone%s");\n' % oldmodel
            else:
                yield '    match if option vendor-class-identifier = "AastraIPPhone%s";\n' % model

            for line in (
                '    log("boot Aastra %s");\n' % model,
                '    option tftp-server-name "http://%s/provisioning/Aastra";\n' % addresses['bootServer'],
                '}\n',
                '\n'):
                yield line

        for macaddr_prefix in cls.AASTRA_MACADDR_PREFIX:
            for line in (
                'subclass "phone-mac-address-prefix" %s {\n' % macaddr_prefix,
                '    if not exists vendor-class-identifier {\n',
                '        log("class Aastra prefix %s");\n' % macaddr_prefix,
                '        option tftp-server-name "http://%s/provisioning/Aastra";\n' % addresses['bootServer'],
                '    }\n',
                '}\n',
                '\n'    ):
                yield line

    @classmethod
    def get_dhcp_pool_lines(cls):
        for x in cls.AASTRA_MODELS:
            yield '        allow members of "Aastra%s";\n' % x[0]
