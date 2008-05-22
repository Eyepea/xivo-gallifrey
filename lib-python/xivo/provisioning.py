"""Common routines and objects for autoprovisioning services in XIVO

Copyright (C) 2007, 2008  Proformatique

"""
# Dependencies/highly recommended? : arping curl

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

import os
import sys
import traceback

import syslog
from xivo.easyslog import *
from xivo.ConfigDict import *
from ConfigParser import ConfigParser

from xivo.except_tb import *

ProvGeneralConf = {
	'database_uri':			"sqlite:/var/lib/asterisk/astsqlite?timeout_ms=150",
	'excl_del_lock_to_s':		45,
	'http_read_request_to_s':	90,
	'http_request_to_s':		90,
	'listen_ipv4':			"127.0.0.1",
	'listen_port':			8666,
	'connect_ipv4':			"127.0.0.1",
	'connect_port':			8666,
	'tftproot':			"/tftpboot/",
	'proc_dev_net':			"/proc/net/dev",
	'scan_ifaces_prefix':		"eth",
	'arping_cmd':			"sudo /usr/sbin/arping",
	'arping_sleep_us':		150000,
	'log_level':			"notice",
	'curl_cmd':			"/usr/bin/curl",
	'curl_to_s':			30,
	'telnet_to_s':			30,
	'templates_dir':		"/usr/share/pf-xivo-provisioning/files/",
	'asterisk_ipv4':		"192.168.0.254",
	'ntp_server_ipv4':		"192.168.0.254"
}
pgc = ProvGeneralConf
authorized_prefix = ["eth"]

def LoadConfig(filename):
	global ProvGeneralConf
	global authorized_prefix
	cp = ConfigParser()
	cp.readfp(open(filename))
	FillDictFromConfigSection(ProvGeneralConf, cp, "general")
	authorized_prefix = [
		p.strip()
		for p in pgc['scan_ifaces_prefix'].split(',')
		if p.strip()
	]

def linesubst(line, variables):
	"""
	In a string, substitute '{{varname}}' occurrences with the 
	value of variables['varname'], '\\' being an escaping char...
	If at first you don't understand this function, draw its finite
	state machine and everything will become crystal clear :)
	"""
	NORM=0
	ONE=1
	TWO=2
	LIT=3
	TLIT=4
	TERM=5
	# trivial no substitution early detection:
	if '{{' not in line and '\\' not in line:
		return line
	st = NORM
	out = ""
	curvar = ""
	for c in line:
		if st == NORM:
			if c == '{':
				st = ONE
			elif c == '\\':
				st = LIT
			else:
				out += c
		elif st == LIT:
			out += c
			st = NORM
		elif st == ONE:
			if c == '{':
				st = TWO
			elif c == '\\':
				out += '{'
				st = LIT
			else:
				out += '{' + c
				st = NORM
		elif st == TWO:
			if c == '\\':
				st = TLIT
			elif c == '}':
				st = TERM
			else:
				curvar += c
		elif st == TLIT:
			curvar += c
			st = TWO
		elif st == TERM:
			if c == '}':
				if curvar not in variables:
					syslogf(SYSLOG_WARNING, "Unknown variable '%s' detected, will just be replaced by an empty string" % curvar)
				else:
					syslogf(SYSLOG_DEBUG, "Substitution of {{%s}} by %s" % (curvar, `variables[curvar]`))
					out += variables[curvar]
				curvar = ''
				st = NORM
			elif c == '\\':
				curvar += '}'
				st = TLIT
			else:
				curvar += '}' + c
				st = TWO
	if st != NORM:
		syslogf(SYSLOG_WARNING, "st != NORM at end of line: " + line)
		syslogf(SYSLOG_WARNING, "returned substitution: " + out)
	return out

def txtsubst(lines, variables, target_file = None):
	"""
	Log that target_file is going to be generated, and calculate its
	content by applying the linesubst() transformation with the given
	variables to each given lines.
	"""
	if target_file:
		syslogf("In process of generating file %s" % `target_file`)
	return [linesubst(line, variables) for line in lines]

def get_netdev_list():
	"Get a view of network interfaces as seen by Linux"
	l=[]
	pnd = open(pgc['proc_dev_net'])
	pnd.readline()
	pnd.readline()
	return tuple([line.split(':', 1)[0].strip()
		      for line in pnd.readlines()
		      if ':' in line])

def get_ethdev_list():
	"""
	Get and filter the list of network interfaces, returning only those
	whose names begin with an element of global variable authorized_prefix
	"""
	global authorized_prefix
	return [dev for dev in get_netdev_list()
		if True in [(dev.find(x) == 0)
			    for x in authorized_prefix]]

def normalize_mac_address(macaddr):
	"""
	input: mac address, with bytes in hexa, ':' separated
	ouput: mac address with format %02X:%02X:%02X:%02X:%02X:%02X
	"""
	macaddr_split = macaddr.upper().split(':', 6)
	if len(macaddr_split) != 6:
		raise ValueError, "Bad format for mac address " + macaddr
	return ':'.join([('%02X' % int(s, 16)) for s in macaddr_split])

def ipv4_from_macaddr(macaddr, logexceptfunc = None):
	"""
	Given a mac address, get an IPv4 address for an host living on the
	LAN. This makes use of the tool "arping". Of course the remote peer
	must respond to ping broadcasts. Out of the box, some stupid phones
	from well known stupid and expensive brands don't.
	"""
	# -r : will only display the IP address on stdout, or nothing
	# -c 1 : ping once
	# -w <xxx> : wait for the answer during <xxx> microsec after the ping
	# -I <netiface> : the network interface to use is <netiface>
	#    -I is an undocumented option like -i but it works
	#    with alias interfaces too
	for iface in get_ethdev_list():
		try:
		    ipfd = os.popen("%s -r -c 1 -w %s -I %s %s" % (pgc['arping_cmd'], pgc['arping_sleep_us'], iface, macaddr))
		except:
		    if logexceptfunc:
			for line in exception_traceback():
			    logexceptfunc(line)
		    continue
		try:
		    try:
			result = ipfd.readline()
		    finally:
			ipfd.close()
		except:
		    if logexceptfunc:
			for line in exception_traceback():
			    logexceptfunc(line)
		    result = None
		if result:
			return result.strip()
	return None

def macaddr_from_ipv4(ipv4, logexceptfunc = None):
	"""
	ipv4_from_macaddr() is indeed a symetrical fonction that can be
	used to retrieve an ipv4 address from a given mac address. This
	function just call the former.
	
	WARNING: this is of course ipv4_from_macaddr() implementation dependent
	"""
	return ipv4_from_macaddr(ipv4, logexceptfunc)

def well_formed_provcode(provcode):
	"""
	Check whether provcode really is a well formed Xivo provisioning
	code.
	"""
	if provcode == '0':
		return True
	for d in provcode:
		if d not in '0123456789':
			return False
	return True

class BaseProv:
	"""
	Basic provisioning logic, including syslogs and conditionnal actions
	execution.
	"""
	def __init__(self, phone):
		"""
		Constructor.
		
		phone must be a dictionary containing everything needed for
		the one phone provisioning process to take place. That is the
		following keys:
		
		'model', 'vendor', 'macaddr', 'actions', 'ipv4' if the value
		for 'actions' is not 'no'
		"""
		self.phone = phone
		syslogf("Instantiation of %s" % str(self.phone))
	def action_reinit(self):
		"""
		This function can be called under some conditions after the 
		configuration for this phone has been generated by the 
		generate_reinitprov() method.
		"""
		if self.phone["actions"] == "no": # possible cause: "distant" provisioning
			syslogf("Skipping REINIT action for phone %s" % self.phone['macaddr'])
			return
		syslogf("Sending REINIT command to phone %s" % self.phone['macaddr'])
		self.do_reinit()
		syslogf(SYSLOG_DEBUG, "Sent REINIT command to phone %s" % self.phone['macaddr'])
	def action_reboot(self):
		"""
		This function can be called under some conditions after the 
		configuration for this phone has been generated by the 
		generate_autoprov() method.
		"""
		if self.phone["actions"] == "no": # distant provisioning with actions disabled
			syslogf("Skipping REBOOT action for phone %s" % self.phone['macaddr'])
			return
		syslogf("Sending REBOOT command to phone %s" % self.phone['macaddr'])
		self.do_reboot()
		syslogf(SYSLOG_DEBUG, "Sent REBOOT command to phone %s" % self.phone['macaddr'])
	def generate_reinitprov(self):
		"""
		This function put the configuration for the phone back in
		guest state.
		"""
		syslogf("About to GUEST'ify the phone %s" % self.phone['macaddr'])
		self.do_reinitprov()
		syslogf(SYSLOG_DEBUG, "Phone GUEST'ified %s" % self.phone['macaddr'])
	def generate_autoprov(self, provinfo):
		"""
		This function generate the configuration for the phone with
		provisioning informations provided in the provinfo dictionary,
		which must contain the following keys:
		
		'name', 'ident', 'number', 'passwd'
		"""
		syslogf("About to AUTOPROV the phone %s with infos %s" % (self.phone['macaddr'], str(provinfo)))
		self.do_autoprov(provinfo)
		syslogf(SYSLOG_DEBUG, "Phone AUTOPROV'ed %s" % self.phone['macaddr'])

# Populated by Phone implementation modules
PhoneClasses = {}
