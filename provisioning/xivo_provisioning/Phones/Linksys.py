"""Support for Linksys phones for XIVO Autoprovisioning

Linksys SPA901, SPA921, SPA922, SPA941, SPA942, SPA962 and PAP2T are supported.

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import os, sys, syslog
import provsup
from provsup import BaseProv
from provsup import ProvGeneralConf as pgc

LINKSYS_COMMON_DIR = pgc['tftproot'] + "Linksys/"
LINKSYS_COMMON_HTTP_USER = "admin"
LINKSYS_COMMON_HTTP_PASS = "adminpass"

class LinksysProv(BaseProv):
	label = "Linksys"
	def __init__(self, phone):
		BaseProv.__init__(self, phone)
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
		## curl options
		# -s			-- silent
		# -o /dev/null		-- dump result
		# --connect-timeout 30  -- timeout after 30s
		# -retry 0		-- don't retry
		os.system(pgc['curl_cmd'] + ' --retry 0 --connect-timeout %s -s -o /dev/null --digest -u %s:%s http://%s/admin/%s'
                          % (str(pgc['curl_to_s']), user, passwd, self.phone['ipv4'], command))

	def do_reinit(self):
		"""Entry point to send the (possibly post) reinit command to
		the phone.
		
		"""
		self.__action("reboot", LINKSYS_COMMON_HTTP_USER, LINKSYS_COMMON_HTTP_PASS)

	def do_reboot(self):
		"Entry point to send the reboot command to the phone."
		self.__action("reboot", LINKSYS_COMMON_HTTP_USER, LINKSYS_COMMON_HTTP_PASS)


	def __generate(self, myprovinfo):
		"""Entry point to generate the provisioned configuration for
		this phone.
		
		"""
                __model = self.phone["model"]
                __macaddr = self.phone["macaddr"].lower().replace(':','')
		template_file = open(pgc['templates_dir'] + "linksys-" + __model + ".cfg")
		template_lines = template_file.readlines()
		template_file.close()
		tmp_filename = LINKSYS_COMMON_DIR + __model + '-' + __macaddr + ".cfg.tmp"
		cfg_filename = tmp_filename[:-4]
		txt = provsup.txtsubst(template_lines,
                                       { "user_display_name": myprovinfo["name"],
                                         "user_phone_ident":  myprovinfo["ident"],
                                         "user_phone_number": myprovinfo["number"],
                                         "user_phone_passwd": myprovinfo["passwd"],
                                         "asterisk_ipv4" : pgc['asterisk_ipv4'],
                                         "ntp_server_ipv4" : pgc['ntp_server_ipv4']
                                         },
                                       cfg_filename)
		tmp_file = open(tmp_filename, 'w')
		tmp_file.writelines(txt)
		tmp_file.close()
		os.rename(tmp_filename, cfg_filename)

	def do_autoprov(self, provinfo):
		"""Entry point to generate the provisioned configuration for
		this phone.
		
		"""
		self.__generate(provinfo)

	def do_reinitprov(self):
		"""Entry point to generate the reinitialized (GUEST)
		configuration for this phone.
		
		"""
		self.__generate({ "name":   "guest",
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
		"""Extract Vendor / Model / FirmwareRevision from SIP User-Agent
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

provsup.PhoneClasses["linksys"] = LinksysProv
