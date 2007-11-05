"""Support for Polycom phones for XIVO Autoprovisioning

Polycom SoundPoint IP 430 SIP and SoundPoint IP 650 SIP are supported.

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import os, sys, syslog
import provsup
from provsup import BaseProv
from provsup import ProvGeneralConf as pgc

POLYCOM_COMMON_DIR = pgc['tftproot'] + "Polycom/"
POLYCOM_COMMON_HTTP_USER = "Polycom"
POLYCOM_COMMON_HTTP_PASS = "456"

class PolycomProv(BaseProv):
	label = "Polycom"
	def __init__(self, phone):
		BaseProv.__init__(self, phone)
		# TODO: handle this with a lookup table stored in the DB?
		if self.phone["model"] != "spip_430" and \
		   self.phone["model"] != "spip_650":
			raise ValueError, "Unknown Polycom model '%s'" % self.phone["model"]

	def __action(self, command, user, passwd):
		# -q -- quiet
		# -nv -- non-verbose
		# -O /dev/null -- send result into /dev/null
		# -T 30 -- timeout after 30s
		# -t 1 -- don't retry
		os.system(pgc['wget_cmd'] + " -t 1 -T %s -q -nv -O /dev/null --http-user=%s --http-passwd=%s http://%s/form-submit --post-data=%s"
                          % (str(pgc['wget_to_s']), user, passwd, self.phone['ipv4'], command))

	def do_reboot(self):
		"Entry point to send the reboot command to the phone."
		self.__action("up.welcomeSoundEnabled=0", POLYCOM_COMMON_HTTP_USER, POLYCOM_COMMON_HTTP_PASS)

	def do_reinit(self):
		"""Entry point to send the (possibly post) reinit command to
		the phone.
		
		"""
                self.__action("up.welcomeSoundEnabled=0", POLYCOM_COMMON_HTTP_USER, POLYCOM_COMMON_HTTP_PASS)


	def __generate(self, myprovinfo):
                template_main_file = open(pgc['templates_dir'] + "polycom-%s.cfg" % self.phone["model"])
		template_main_lines = template_main_file.readlines()
		template_main_file.close()
                template_phone_file = open(pgc['templates_dir'] + "polycom-phone.cfg")
		template_phone_lines = template_phone_file.readlines()
		template_phone_file.close()

                __macaddr = self.phone["macaddr"].replace(':','').lower()
		tmp_main_filename = POLYCOM_COMMON_DIR + __macaddr + ".cfg.tmp"
		cfg_main_filename = tmp_main_filename[:-4]
		tmp_phone_filename = POLYCOM_COMMON_DIR + __macaddr + "-phone.cfg.tmp"
		cfg_phone_filename = tmp_phone_filename[:-4]

                txt_main = provsup.txtsubst(template_main_lines,
                                            { "phone.cfg": __macaddr + "-phone.cfg" },
                                            cfg_main_filename)
		txt_phone = provsup.txtsubst(template_phone_lines,
                                            { "user_display_name": myprovinfo["name"],
                                              "user_phone_ident":  myprovinfo["ident"],
                                              "user_phone_number": myprovinfo["number"],
                                              "user_phone_passwd": myprovinfo["passwd"],
                                              "asterisk_ipv4" : pgc['asterisk_ipv4']
                                              },
                                            cfg_phone_filename)

		tmp_main_file = open(tmp_main_filename, 'w')
		tmp_main_file.writelines(txt_main)
		tmp_main_file.close()
		tmp_phone_file = open(tmp_phone_filename, 'w')
		tmp_phone_file.writelines(txt_phone)
		tmp_phone_file.close()

		os.rename(tmp_main_filename, cfg_main_filename)
		os.rename(tmp_phone_filename, cfg_phone_filename)


	def do_reinitprov(self):
		"""Entry point to generate the reinitialized (GUEST)
		configuration for this phone.
		
		"""
                self.__generate({ "name":   "guest",
                                  "ident":  "guest",
                                  "number": "guest",
                                  "passwd": "guest"
                                  })

	def do_autoprov(self, provinfo):
		"""Entry point to generate the provisioned configuration for
		this phone.
		
		"""
		self.__generate(provinfo)


	# Introspection entry points

	def get_phones(cls):
		"Report supported phone models for this vendor."
		return (("spip_430", "SoundPoint IP 430 SIP"), ("spip_650", "SoundPoint IP 650 SIP"))
	get_phones = classmethod(get_phones)

	# Entry points for the AGI

	def get_vendor_model_fw(cls, ua):
		"""Extract Vendor / Model / FirmwareRevision from SIP User-Agent
		or return None if we don't deal with this kind of Agent.
		
		"""
                # PolycomSoundPointIP-SPIP_430-UA/2.2.0.0047
                # PolycomSoundPointIP-SPIP_650-UA/2.2.0.0047

                if ua[:7] != 'Polycom':
                        return None
                model = 'unknown'
                fw = 'unknown'
                ua_splitted = ua.split('/', 1)
                if len(ua_splitted) == 2:
                        fw = ua_splitted[1]
                        model = ua_splitted[0].split('-')[1].lower()
		return ("polycom", model, fw)
	get_vendor_model_fw = classmethod(get_vendor_model_fw)

provsup.PhoneClasses["polycom"] = PolycomProv
