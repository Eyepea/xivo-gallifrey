"""Support for Thomson phones for Xivo Autoprovisioning

Thomson 2030S is supported.

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import os, sys, syslog, time, telnetlib
import provsup
from provsup import BaseProv
from provsup import ProvGeneralConf as pgc

import time # for TimeoutingTelnet

# NOTES:
# ~/etc/dhcpd3/dhcpd.conf -> /tftpboot/Thomson/ST2030S_v1.53.inf
#				-> /tftpboot/Thomson/ST2030S_common_v1.53.txt
#				-> /tftpboot/Thomson/binary/v2030SG.070309.1.53.zz
#				-> ...also firmware and other global
#					config files like ringing tones...

THOMSON_COMMON_DIR = pgc['tftproot'] + "Thomson/"
THOMSON_COMMON_INF = THOMSON_COMMON_DIR + "ST" # + "2030S_common"

# THOMSON BUGBUG #3
# THOMSON_SPEC_DIR must be *TFTPROOT* because the phone will only download its
# mac specific configuration from this directory.
THOMSON_SPEC_DIR = pgc['tftproot']

THOMSON_USER = "admin"		# XXX
THOMSON_PASSWD = "superpass"	# XXX
THOMSON_SPEC_TXT_TEMPLATE = pgc['templates_dir'] + "ST" # + "2030S_template.txt"

# for some tests: THOMSON_SPEC_TXT_BASENAME = pgc['templates_dir'] + "ST2030S_"
THOMSON_SPEC_TXT_BASENAME = "/tftpboot/ST" # + "2030S_"

class TelnetExpectationFailed(RuntimeError):
	"""Exception raised by the new methods introduced by the
	telnetlib.Telnet extending TimeoutingTelnet class.
	
	"""
	pass

class TimeoutingTelnet(telnetlib.Telnet):
	"""This class extends Telnet so that a global timeout can trigger
	during the newly introduced read_until_to(), and this will result
	in the raising of an exception.
	
	"""
	def __init__(self, cnx, global_TO = pgc['telnet_to_s']):
		if type(cnx) != tuple or len(cnx) < 1:
			raise ValueError, "The cnx argument must be (peer,) or (peer,port) ; %s was given" % str(cnx)
		elif len(cnx) < 2:
			telnetlib.Telnet.__init__(self, cnx[0])
		else:
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
		"""Same as read_until() but if expected has not been received a
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

class ThomsonProv(BaseProv):
	label = "Thomson"
	def __init__(self, phone):
		BaseProv.__init__(self, phone)
		# TODO: handle this with a lookup table stored in the DB?
		if self.phone["model"] != "2022s" and \
		   self.phone["model"] != "2030s":
			raise ValueError, "Unknown Thomson model '%s'" % self.phone["model"]
	def __generate_timestamp(self):
		tuple_time = time.localtime()
		seximin = tuple_time[3] * 360 + tuple_time[4] * 6 + int(tuple_time[5] / 10)
		return "%04d%02d%02d%04d" % (tuple_time[0], tuple_time[1], tuple_time[2], seximin)
	def __configurate_telnet(self, user, passwd):
		self.user = user
		self.passwd = passwd
	def __action(self, commands):
		ip = self.phone["ipv4"]
		tn = TimeoutingTelnet((ip,))
		tn.restart_global_to()
		try:
			tn.read_until_to("Login: ")
			tn.write(self.user + "\r\n")
			if self.passwd:
				tn.read_until_to("Password: ")
				tn.write(self.passwd + "\r\n")
			for cmd in commands:
				tn.read_until_to("["+self.user+"]#")
				syslog.syslog(syslog.LOG_DEBUG,
					"sending telnet command (%s): %s" \
					% (self.phone["macaddr"],cmd))
				tn.write("%s\n" % cmd)
				if cmd == 'reboot':
					break
		finally:
			tn.close()
	def __generate(self, myprovinfo):
		txt_template_file = open(THOMSON_SPEC_TXT_TEMPLATE + self.phone["model"].upper() + "_template.txt")
		txt_template_lines = txt_template_file.readlines()
		txt_template_file.close()
		tmp_filename = THOMSON_SPEC_TXT_BASENAME + self.phone["model"].upper() + "_" + self.phone["macaddr"].replace(':','') + '.txt.tmp'
		txt_filename = tmp_filename[:-4]
		txt = provsup.txtsubst(txt_template_lines, {
			"DisplayName1": myprovinfo["name"],
# THOMSON BUGBUG #1
# myprovinfo["number"] is volontarily not set in "TEL1Number" because Thomson
# phones authentify with their telnumber.. :/
			"TEL1Number": myprovinfo["ident"],
			"regid1": myprovinfo["ident"],
			"regpwd1": myprovinfo["passwd"],
			"simultcalls": myprovinfo["simultcalls"],
			# <WARNING: THIS FIELD MUST STAY IN LOWER CASE IN THE TEMPLATE AND MAC SPECIFIC FILE>
			"config_sn": self.__generate_timestamp()
			# </WARNING>
		}, txt_filename)
		tmp_file = open(tmp_filename, 'w')
		tmp_file.writelines(txt)
		tmp_file.close()
		os.rename(tmp_filename, txt_filename) # atomically update the file
		inf_filename = THOMSON_SPEC_TXT_BASENAME + self.phone["model"].upper() + "_" + self.phone["macaddr"].replace(':','') + '.inf'
		try:
			os.lstat(inf_filename)
			os.unlink(inf_filename)
		except:
			pass
		os.symlink(THOMSON_COMMON_INF + self.phone["model"].upper() + "_common", inf_filename)

	# Daemon entry points for configuration generation and issuing commands

	def do_reboot(self):
		"Entry point to send the reboot command to the phone."
		self.__configurate_telnet(THOMSON_USER, THOMSON_PASSWD)
		self.__action(('reboot',))
	def do_reinit(self):
		"""Entry point to send the (possibly post) reinit command to
		the phone.
		
		"""
# THOMSON BUGBUG #2
# We are waiting for Thomson to correctly reload their common .txt file after a
# reinit; until then we don't really reinit the phone but just put it in the
# guest state, by its mac specific configuration
		self.__configurate_telnet(THOMSON_USER, THOMSON_PASSWD)
#		self.__action(('sys set rel 0', 'ffs format', 'ffs commit', 'ffs commit', 'reboot', 'quit'))
		self.__action(('reboot',))
	def do_reinitprov(self):
		"""Entry point to generate the reinitialized (GUEST)
		configuration for this phone.
		
		"""
		self.__generate({
			"name": "guest",
			"ident": "guest",
			"number": "guest",
			"passwd": "guest",
		})
	def do_autoprov(self, provinfo):
		"""Entry point to generate the provisioned configuration for
		this phone.
		
		"""
		self.__generate(provinfo)

	# Introspection entry points

	def get_phones(cls):
		"Report supported phone models for this vendor."
		return (("2022s", "2022S"), ("2030s", "2030S"))
	get_phones = classmethod(get_phones)

	# Entry points for the AGI

	def get_vendor_model_fw(cls, ua):
		"""Extract Vendor / Model / FirmwareRevision from SIP User-Agent
		or return None if we don't deal with this kind of Agent.
		
		"""
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
