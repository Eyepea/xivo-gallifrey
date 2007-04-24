# -*- coding: iso-8859-15 -*-
"""Support for Snom phones for Xivo Autoprovisioning

Snom 300 320 and 360 are supported.

Copyright (C) 2007, Proformatique

"""
# Dependencies : apache wget

import os, sys, syslog
import provsup
from provsup import BaseProv

WGET        = "/usr/bin/wget"

# === SNOM ===

# NOTE: Snom doesn't support something else than files 
# at root of tftproot when using tftp... :/ (It just
# internaly replace the first '/' with a '\0' :/// )

# NOTE: Because it seems much technically impossible to detect the phone
# modelby dhcp request (model not in the request.... :///), we'll need to
# also support HTTP based provisioning

SNOM_SPEC_DIR = provsup.TFTPROOT + "Snom/"
SNOM_SPEC_TEMPLATE = "files/snom-template.htm"
SNOM_COMMON_HTTP_USER = "guest"
SNOM_COMMON_HTTP_PASS = "guest"
class SnomProv(BaseProv):
	label = "Snom"
	def __init__(self, phone):
		BaseProv.__init__(self, phone)
		# TODO: handle this with a ?translation table stored in the DB
		if self.phone["model"] != "300" and \
		   self.phone["model"] != "320" and \
		   self.phone["model"] != "360":
			raise "Unknown Snom model '%s'" % self.phone["model"]
	def pv_action(self, command, user, passwd):
		os.system(WGET + " -q -nv -O /dev/null --http-user=%s --http-passwd=%s http://%s/confirm.html?%s=yes" % (user, passwd, self.phone['ipv4'], command))
	def do_reinit(self):
		self.pv_action("RESET", SNOM_COMMON_HTTP_USER, SNOM_COMMON_HTTP_PASS)
	def do_reboot(self):
		self.pv_action("REBOOT", SNOM_COMMON_HTTP_USER, SNOM_COMMON_HTTP_PASS)
	def do_reinitprov(self):
		htm_filename = SNOM_SPEC_DIR + "snom" + self.phone["model"] + '-' + self.phone["macaddr"].replace(':','') + '.htm'
		try:
			os.unlink(htm_filename)
		except:
			pass
	def do_autoprov(self, provinfo):
		template_file = open(SNOM_SPEC_TEMPLATE)
		template_lines = template_file.readlines()
		template_file.close()
		tmp_filename = SNOM_SPEC_DIR + "snom" + self.phone["model"] + '-' + self.phone["macaddr"].replace(':','') + '.htm.tmp'
		htm_filename = tmp_filename[:-4]
		txt = provsup.txtsubst(template_lines, {
			"user_realname1": provinfo["name"],
			"user_name1": provinfo["ident"],
			"user_pname1": provinfo["number"],
			"user_pass1": provinfo["passwd"],
			"http_user": SNOM_COMMON_HTTP_USER,
			"http_pass": SNOM_COMMON_HTTP_PASS,
			"phone_name": provinfo["number"],
			"user_idle_text1": provinfo["name"],
			"user_sipusername_as_line1": "on"
		}, htm_filename)
		tmp_file = open(tmp_filename, 'w')
		tmp_file.writelines(txt)
		tmp_file.close()
		os.rename(tmp_filename, htm_filename)

	# Report supported Phones to the WebInterface (or anybody...)
	def get_phones(cls):
		return (("300", "300"), ("320", "320"), ("360","360"))
	get_phones = classmethod(get_phones)

	# Extract Vendor / Model / FirmwareRevision from SIP User-Agent
	# or return None if we don't deal with this kind of Agent
	def get_vendor_model_fw(cls, ua):
		if 'snom' != ua[:4].lower():
			return None
		fw = 'unknown'
		ua_splitted = ua.split('/')
		model = ua_splitted[0][4:].lower()
		if len(ua_splitted) > 1:
			fw = ua_splitted[1]
		return ("snom", model, fw)
	get_vendor_model_fw = classmethod(get_vendor_model_fw)

provsup.PhoneClasses["snom"] = SnomProv
