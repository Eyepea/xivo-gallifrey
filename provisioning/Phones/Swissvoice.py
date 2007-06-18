"""Support for Swissvoice phones for Xivo Autoprovisioning

Swissvoice IP10S is supported.

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import os, sys, syslog
import provsup
from provsup import BaseProv
from provsup import ProvGeneralConf as pgc

# SWISSVOICE BUGBUG
# It would be possible to make tftp upload a /tftpboot/Swissvoice/swupdate_ip10.inf
# into the phones, however in order for any MAC address to be taken care of,
# the <MACADDR>_ip10.inf files have to be located directly under /tftpboot/
# Nevertheless, we made them refer to the config <MACADDR>_ip10.cfg files
# located under /tftpboot/Swissvoice/

# Swissvoice User-Agent format :
# User-Agent: Swissvoice IP10 SP v1.0.1 (Build 4) 3.0.5.1

SWISSVOICE_SPEC_DIR = pgc['tftproot'] + "Swissvoice/"
SWISSVOICE_SPEC_INF_TEMPLATE = pgc['templates_dir'] + "template_ip10.inf"
SWISSVOICE_SPEC_CFG_TEMPLATE = pgc['templates_dir'] + "template_ip10.cfg"
SWISSVOICE_COMMON_HTTP_USER = "admin"
SWISSVOICE_COMMON_HTTP_PASS = "admin"

class SwissvoiceProv(BaseProv):
	label = "Swissvoice"
	def __init__(self, phone):
		BaseProv.__init__(self, phone)
		# TODO: handle this with a lookup table stored in the DB?
		if self.phone["model"] != "ip10s":
			raise ValueError, "Unknown Swissvoice model '%s'" % self.phone["model"]
	def __action(self, command, user, passwd):
		# -q -- quiet
		# -nv -- non-verbose
		# -O /dev/null -- send result into /dev/null
		# -T 30 -- timeout after 30s
		# -t 1 -- don't retry
		url_rep1 = "Administrator_Settings"
		url_rep2 = "reboot_choice_B.html?WINDWEB_URL=/Administrator_Settings/reboot_choice_B.html"
		url_rep3 = "&EraseFlash=0&Reboot=+Reboot"
		fullcommand = pgc['wget_cmd'] + \
			      " -t 1 -T %s -q -nv -O /dev/null --http-user=%s --http-passwd=%s \"http://%s/%s/%s%s\"" \
			      %(str(pgc['wget_to_s']), user, passwd,
				self.phone['ipv4'], url_rep1, url_rep2, url_rep3)
		os.system(fullcommand)

	def do_reinit(self):
		"""Entry point to send the (possibly post) reinit command to
		the phone.
		
		"""
		self.__action("RESET", SWISSVOICE_COMMON_HTTP_USER, SWISSVOICE_COMMON_HTTP_PASS)
	def do_reboot(self):
		"Entry point to send the reboot command to the phone."
		self.__action("REBOOT", SWISSVOICE_COMMON_HTTP_USER, SWISSVOICE_COMMON_HTTP_PASS)
	def do_reinitprov(self):
		"""Entry point to generate the reinitialized (GUEST)
		configuration for this phone.
		
		"""
		cfg_filename = SWISSVOICE_SPEC_DIR + \
			       self.phone["macaddr"].lower().replace(':','') + '_ip10.cfg'
		inf_filename = SWISSVOICE_SPEC_DIR + "/../" + \
			       self.phone["macaddr"].lower().replace(':','') + '_ip10.inf'
		try:
			os.unlink(cfg_filename)
		except:
			pass
		try:
			os.unlink(inf_filename)
		except:
			pass

	def do_autoprov(self, provinfo):
		"""Entry point to generate the provisioned configuration for
		this phone.
		
		"""
		cfg_template_file = open(SWISSVOICE_SPEC_CFG_TEMPLATE)
		cfg_template_lines = cfg_template_file.readlines()
		cfg_template_file.close()
		inf_template_file = open(SWISSVOICE_SPEC_INF_TEMPLATE)
		inf_template_lines = inf_template_file.readlines()
		inf_template_file.close()
		
		macaddr = self.phone["macaddr"].lower().replace(':','')

		cfg_tmp_filename = SWISSVOICE_SPEC_DIR + \
				   self.phone["macaddr"].lower().replace(':','') + '_ip10.cfg.tmp'
		inf_tmp_filename = SWISSVOICE_SPEC_DIR + "/../" + \
				   self.phone["macaddr"].lower().replace(':','') + '_ip10.inf.tmp'
		cfg_filename = cfg_tmp_filename[:-4]
		inf_filename = inf_tmp_filename[:-4]

		dtmf_swissvoice = "off"
		dtmf_config     = provinfo["dtmfmode"]
		if dtmf_config == "rfc2833":
			dtmf_swissvoice = "on inb"
		elif dtmf_config == "inband":
			dtmf_swissvoice = "off"
		elif dtmf_config == "info":
			dtmf_swissvoice = "on oob"

		txt = provsup.txtsubst(cfg_template_lines, {
			"user_realname1": provinfo["name"],
			"user_name1": provinfo["ident"],
			"user_pname1": provinfo["number"],
			"user_pass1": provinfo["passwd"],
			"http_user": SWISSVOICE_COMMON_HTTP_USER,
			"http_pass": SWISSVOICE_COMMON_HTTP_PASS,
			"phone_name": provinfo["number"],
			"dtmfmode": dtmf_swissvoice,
			"user_idle_text1": provinfo["name"],
			"user_sipusername_as_line1": "on",
			"asterisk_ipv4" : pgc['asterisk_ipv4'],
			"ntp_server_ipv4" : pgc['ntp_server_ipv4']
		}, cfg_filename)
		tmp_file = open(cfg_tmp_filename, 'w')
		tmp_file.writelines(txt)
		tmp_file.close()
		os.rename(cfg_tmp_filename, cfg_filename)

		txt = provsup.txtsubst(inf_template_lines, {
			"macaddr": self.phone["macaddr"].lower().replace(':','')
		}, inf_filename)
		tmp_file = open(inf_tmp_filename, 'w')
		tmp_file.writelines(txt)
		tmp_file.close()
		os.rename(inf_tmp_filename, inf_filename)

	# Introspection entry points

	def get_phones(cls):
		"Report supported phone models for this vendor."
		return (("ip10s", "IP10S"),)
	get_phones = classmethod(get_phones)

	# Entry points for the AGI

	def get_vendor_model_fw(cls, ua):
		"""Extract Vendor / Model / FirmwareRevision from SIP User-Agent
		or return None if we don't deal with this kind of Agent.
		
		"""
		ua_splitted = ua.split(' ')
		if 'swissvoice' != ua_splitted[0].lower():
			return None
		model = ua_splitted[1].lower() + "s"
		fw = ua_splitted[3]
		return ("swissvoice", model, fw)
	get_vendor_model_fw = classmethod(get_vendor_model_fw)

provsup.PhoneClasses["swissvoice"] = SwissvoiceProv
