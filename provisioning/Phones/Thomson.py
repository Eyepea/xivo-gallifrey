# -*- coding: iso-8859-15 -*-
"""Support for Thomson phones for Xivo Autoprovisioning

Thomson 2030S is supported.

Copyright (C) 2007, Proformatique

"""
# Dependencies : apache wget

import os, sys, syslog, time, telnetlib
import provsup
from provsup import BaseProv

# === THOMSON ===

# TODO handle 2022S?

# NOTES:
# ~/etc/dhcpd3/dhcpd.conf -> /tftpboot/Thomson/ST2030S_v1.53.inf
#				-> /tftpboot/Thomson/ST2030S_common_v1.53.txt
#				-> /tftpboot/Thomson/binary/v2030SG.070309.1.53.zz
# (ainsi que sur le firmware et d'autres fichiers de config globaux...)
# (ex: sonneries)

THOMSON_COMMON_DIR = provsup.TFTPROOT + "Thomson/"
THOMSON_COMMON_INF = THOMSON_COMMON_DIR + "ST2030S_v1.53.inf"
THOMSON_COMMON_TXT = THOMSON_COMMON_DIR + "ST2030S_common_v1.53.txt"
THOMSON_SPEC_DIR = provsup.TFTPROOT # MUST BE *TFTPROOT* BECAUSE OF THE PHONE BEHAVIOR
THOMSON_USER = "admin"		# XXX
THOMSON_PASSWD = "superpass"	# XXX
THOMSON_SPEC_TXT_TEMPLATE = "files/ST2030S_template.txt"
# THOMSON_SPEC_TXT_BASENAME = "files/ST2030S_"
THOMSON_SPEC_TXT_BASENAME = "/tftpboot/ST2030S_"
# warning drop number use ident instead
class ThomsonProv(BaseProv):
	label = "Thomson"
	def __init__(self, phone):
		BaseProv.__init__(self, phone)
	# private function:
	def pv_generate_timestamp(self):
		tuple_time = time.localtime()
		seximin = tuple_time[3] * 360 + tuple_time[4] * 6 + int(tuple_time[5] / 10)
		return "%04d%02d%02d%04d" % (tuple_time[0], tuple_time[1], tuple_time[2], seximin)
	# private function:
	def pv_configurate_telnet(self, user, passwd):
		self.user = user
		self.passwd = passwd
	# private function:
	def pv_action(self, commands):
		ip = self.phone["ipv4"]
		tn = telnetlib.Telnet(ip)
		try:
			tn.read_until("Login: ")
			tn.write(self.user + "\r\n")
			if self.passwd:
				tn.read_until("Password: ")
				tn.write(self.passwd + "\r\n")
			for cmd in commands:
				tn.read_until("["+self.user+"]#")
				syslog.syslog(syslog.LOG_DEBUG,
					"sending telnet command (%s): %s" \
					% (str(self.phone),cmd))
				tn.write("%s\n" % cmd)
				if cmd == 'reboot':
					break
		finally:
			tn.close()
	# private function:
	def pv_generate(self, myprovinfo):
		txt_template_file = open(THOMSON_SPEC_TXT_TEMPLATE)
		txt_template_lines = txt_template_file.readlines()
		txt_template_file.close()
		tmp_filename = THOMSON_SPEC_TXT_BASENAME + self.phone["macaddr"].replace(':','') + '.txt.tmp'
		txt_filename = tmp_filename[:-4]
		txt = provsup.txtsubst(txt_template_lines, {
			"DisplayName1": myprovinfo["name"],
			# myprovinfo["number"] is volontarily not set in
			# "TEL1Number" because of a Thomson bug:
			# Thomson phones authentify with their telnumber.. :/
			"TEL1Number": myprovinfo["ident"],
			"regid1": myprovinfo["ident"],
			"regpwd1": myprovinfo["passwd"],
			"config_sn": self.pv_generate_timestamp()
		}, txt_filename)
		tmp_file = open(tmp_filename, 'w')
		tmp_file.writelines(txt)
		tmp_file.close()
		os.rename(tmp_filename, txt_filename) # atomically update the file
		inf_filename = THOMSON_SPEC_TXT_BASENAME + self.phone["macaddr"].replace(':','') + '.inf'
		try:
			os.lstat(inf_filename)
			os.unlink(inf_filename)
		except:
			pass
		os.symlink(THOMSON_COMMON_INF, inf_filename)
	def do_reboot(self):
		self.pv_configurate_telnet(THOMSON_USER, THOMSON_PASSWD)
		self.pv_action(('reboot',))
	def do_reinit(self):
		self.pv_configurate_telnet(THOMSON_USER, THOMSON_PASSWD)
#		self.pv_action(('sys set rel 0', 'ffs format', 'ffs commit', 'ffs commit', 'reboot', 'quit'))
#		NOTE: We are waiting for Thomson to correctly reload their common .txt file after a reinit;
#			until then we don't really reinit the phone but just put it in the guest state, by
#			its mac specific configuration
		self.pv_action(('reboot',))
	def do_reinitprov(self):
		self.pv_generate({
			"name": "guest",
			"ident": "guest",
			"number": "guest",
			"passwd": "guest",
			# WARNING: THIS FIELD MUST STAY IN LOWER CASE IN THE TEMPLATE AND MAC SPECIFIC FILE
			"config_sn": self.pv_generate_timestamp()
			# WARNING: THIS FIELD MUST STAY IN LOWER CASE IN THE TEMPLATE AND MAC SPECIFIC FILE
		})
	def do_autoprov(self, provinfo):
		self.pv_generate(provinfo)

	# Report supported Phones to the WebInterface (or anybody...)
	def get_phones(cls):
		return (("2022s", "2022S"), ("2030s", "2030S"))
	get_phones = classmethod(get_phones)

	# Extract Vendor / Model / FirmwareRevision from SIP User-Agent
	# or return None if we don't deal with this kind of Agent
	def get_vendor_model_fw(cls, ua):
		if "THOMSON" != ua[:7].upper():
			return None
		splitted_ua = ua.split()
		fw = "unknown"
		if len(splitted_ua) < 2:
			return None
		if splitted_ua[1][:2] == 'ST':
			model = splitted_ua[1][2:].lower() + 's'
		else:
			model = splitted_ua[1].lower() + 's'
		if len(splitted_ua) >= 4:
			fw = splitted_ua[3]
		return ("thomson", model, fw)
	get_vendor_model_fw = classmethod(get_vendor_model_fw)

provsup.PhoneClasses["thomson"] = ThomsonProv
