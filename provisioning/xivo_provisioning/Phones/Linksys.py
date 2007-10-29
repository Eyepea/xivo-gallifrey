"""Support for Linksys phones for XIVO Autoprovisioning

Linksys SPA901, SPA942, SPA962 and PAP2T are supported.

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import os, sys, syslog
import provsup
from provsup import BaseProv
from provsup import ProvGeneralConf as pgc

LINKSYS_COMMON_DIR = pgc['tftproot'] + "Linksys/"

class LinksysProv(BaseProv):
	label = "Linksys"
	def __init__(self, phone):
		BaseProv.__init__(self, phone)
		# TODO: handle this with a lookup table stored in the DB?
		if self.phone["model"] != "spa901" and \
                       self.phone["model"] != "spa942" and \
                       self.phone["model"] != "spa962" and \
                       self.phone["model"] != "pap2t":
			raise ValueError, "Unknown Linksys model '%s'" % self.phone["model"]

	def __action(self, command):
		# -q -- quiet
		# -nv -- non-verbose
		# -O /dev/null -- send result into /dev/null
		# -T 30 -- timeout after 30s
		# -t 1 -- don't retry
		os.system(pgc['wget_cmd'] + ' -t 1 -T %s -q -nv -O /dev/null http://%s/admin/%s'
                          % (str(pgc['wget_to_s']), self.phone['ipv4'], command))

	def do_reinit(self):
		"""Entry point to send the (possibly post) reinit command to
		the phone.
		
		"""
		self.__action("reboot")

	def do_reboot(self):
		"Entry point to send the reboot command to the phone."
		self.__action("reboot")


	def __generate(self, myprovinfo):
		"""Entry point to generate the provisioned configuration for
		this phone.
		
		"""
		template_file = open(pgc['templates_dir'] + "linksys-" + self.phone["model"] + ".cfg")
		template_lines = template_file.readlines()
		template_file.close()
                __macaddr = self.phone["macaddr"].lower()
		tmp_filename = LINKSYS_COMMON_DIR + self.phone["model"] + '-' + __macaddr + ".cfg.tmp"
		cfg_filename = tmp_filename[:-4]
		txt = provsup.txtsubst(template_lines,
                                       { "user_realname1": myprovinfo["name"],
                                         "user_name1": myprovinfo["ident"],
                                         "user_pname1": myprovinfo["number"],
                                         "user_pass1": myprovinfo["passwd"],
                                         "phone_name": myprovinfo["number"],
                                         "user_idle_text1": myprovinfo["name"],
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

	def get_phones(cls):
		"Report supported phone models for this vendor."
		return (("spa901", "SPA901"), ("spa942", "SPA942"), ("spa962", "SPA962"), ("pap2t", "PAP2T"))
	get_phones = classmethod(get_phones)

	# Entry points for the AGI

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
                        model = modelfw[0]
                        if len(modelfw) == 2:
                                fw = modelfw[1]
		return ("linksys", model, fw)
	get_vendor_model_fw = classmethod(get_vendor_model_fw)

provsup.PhoneClasses["linksys"] = LinksysProv
