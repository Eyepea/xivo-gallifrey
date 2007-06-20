"""Common routines and objects for autoprovisioning services in Xivo

Copyright (C) 2007, Proformatique

"""
# Dependencies/highly recommended? : arping wget

__version__ = "$Revision$ $Date$"

import os, sys, traceback

import syslog
from easyslog import *
from ConfigDict import *
from ConfigParser import ConfigParser

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
	'debug_agi':			0,
	'wget_cmd':			"/usr/bin/wget",
	'wget_to_s':			30,
	'telnet_to_s':			30,
	'templates_dir':	       	"/usr/share/pf-xivo-provisioning/files/",
	'asterisk_ipv4':                "192.168.0.254",
	'ntp_server_ipv4':              "192.168.0.254"
}
pgc = ProvGeneralConf
authorized_prefix = ["eth"]

def LoadConfig(filename):
	global ProvGeneralConf
	cp = ConfigParser()
	cp.readfp(open(filename))
	FillDictFromConfigSection(ProvGeneralConf, cp, "general")
	authorized_prefix = pgc['scan_ifaces_prefix'].split(',')
	authorized_prefix = [	\
		p.strip()	\
		for p in pgc['scan_ifaces_prefix'].split(',')	\
		if p.strip()	\
	]

def elem_or_none(r, el):
	"r is a dictionary or None, and if (e1 in r) then r[el] must exists"
	if (r is None) or (el not in r):
		return None
	return r[el]

def lst_get(lst, idx, dft=None):
	"lst_get(lst, idx, default) -> lst[idx] if idx > 0 and idx < len(lst), else dft."
	if lst is None or (not (idx >= 0 and idx < len(lst))):
		return dft
	return lst[idx]

def linesubst(line, variables):
	"""In a string, substitute '{{varname}}' occurrences with the 
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
					syslogf(SYSLOG_WARNING, 
					    ("Unknown variable '%s' detected, "+
					    "will just be replaced by an "+
					    "empty string") % (curvar,))
				else:
					syslogf(SYSLOG_DEBUG,
					    ("Substitution of {{%s}} by \"%s\""%\
					    	(curvar, variables[curvar])))
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
	"""Log that target_file is going to be generated, and calculate its
	content by applying the linesubst() transformation with the given
	variables to each given lines.
	
	"""
	if target_file:
		syslogf("In process of generating file \"%s\"" % (target_file,))
	return map(lambda line: linesubst(line, variables), lines)

def exception_traceback():
	"""Returns a backtrace of the current exception in a list of strings,
	not terminated by newlines.
	
	"""
	return map(lambda x: x.rstrip(), traceback.format_exception(*sys.exc_info()))

def log_current_exception(loglevel=SYSLOG_ERR, noclear=False):
	"""Log a backtrace of the current exception in the system logs, with
	the desired log level. Clear the current exception at end of command
	if 'noclear' is False.
	
	"""
	for line in exception_traceback():
		syslogf(loglevel, line)
	if not noclear:
		sys.exc_clear()

def get_netdev_list():
	"Get a view of network interfaces as seen by Linux"
	l=[]
	pnd = open(pgc['proc_dev_net'])
	pnd.readline()
	pnd.readline()
	return tuple([line.split(':',1)[0].strip()
	              for line in pnd.readlines()
	              if ':' in line])

def get_ethdev_list():
	"""Get and filter the list of network interfaces, returning only those
	whose names begin with an element of global variable authorized_prefix
	
	"""
	global authorized_prefix
	return filter(lambda dev: True in map(lambda x: dev.find(x) == 0,
                                              authorized_prefix),
	              get_netdev_list())

def normalize_mac_address(macaddr):
	"""input: mac address, with bytes in hexa, ':' separated
	ouput: mac address with format %02X:%02X:%02X:%02X:%02X:%02X
	
	"""
	macaddr_split = macaddr.upper().split(':', 6)
	if len(macaddr_split) != 6:
		raise ValueError, "Bad format for mac address " + macaddr
	return ':'.join(map(lambda s: '%02X' % int(s, 16), macaddr_split))

def ipv4_from_macaddr(macaddr, logexceptfunc = None):
	"""Given a mac address, get an IPv4 address for an host living on the
	LAN. This makes use of the tool "arping". Of course the remote peer
	must respond to ping broadcasts. Out of the box, some stupid phones
	from well known stupid and expensive brands don't.

	"""
	# -r : will only display the IP address on stdout, or nothing
	# -c 1 : ping once
	# -w <xxx> : wait for the answer during <xxx> microsec after the ping
	# -i <netiface> : the network interface to use is <netiface>
	for iface in get_ethdev_list():
		try:
		    ipfd = os.popen(pgc['arping_cmd'] +
			    (" -r -c 1 -w %s -i %s " % (pgc['arping_sleep_us'], iface)) +
			    macaddr)
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
	"""ipv4_from_macaddr() is indeed a symetrical fonction that can be
	used to retrieve an ipv4 address from a given mac address. This
	function just call the former.

	WARNING: this is of course ipv4_from_macaddr() implementation dependent
	"""
	return ipv4_from_macaddr(ipv4, logexceptfunc)

def well_formed_provcode(provcode):
	"""Check whether provcode really is a well formed Xivo provisioning
	code."""
	if provcode == '0':
		return True
	for d in provcode:
		if d not in '0123456789':
			return False
	return True

class BaseProv:
	"""Basic provisioning logic, including syslogs and conditionnal actions
	execution.
	
	"""
	def __init__(self, phone):
		"""Constructor.
		
		phone must be a dictionary containing everything needed for
		the one phone provisioning process to take place. That is the
		following keys:
		
		'model', 'vendor', 'macaddr', 'actions', 'ipv4' if the value
		for 'actions' is not 'no'
		
		"""
		self.phone = phone
		syslogf("Instantiation of %s" % (str(self.phone),))
	def action_reinit(self):
		"""This function can be called under some conditions after the 
		configuration for this phone has been generated by the 
		generate_reinitprov() method.
		
		"""
		if self.phone["actions"].lower() == "no": # possible cause: "distant" provisioning
			syslogf("Skipping REINIT action for phone %s" % self.phone['macaddr'])
			return
		syslogf("Sending REINIT command to phone %s" % self.phone['macaddr'])
		self.do_reinit()
		syslogf(SYSLOG_DEBUG, "Sent REINIT command to phone %s" % self.phone['macaddr'])
	def action_reboot(self):
		"""This function can be called under some conditions after the 
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
		"""This function put the configuration for the phone back in
		guest state.
		
		"""
		syslogf("About to GUEST'ify the phone %s" % self.phone['macaddr'])
		self.do_reinitprov()
		syslogf(SYSLOG_DEBUG, "Phone GUEST'ified %s" % self.phone['macaddr'])
	def generate_autoprov(self, provinfo):
		"""This function generate the configuration for the phone with
		provisioning informations provided in the provinfo dictionary,
		which must contain the following keys:
		
		'name', 'ident', 'number', 'passwd'
		
		"""
		syslogf("About to AUTOPROV the phone %s with infos %s" % (self.phone['macaddr'],str(provinfo)))
		self.do_autoprov(provinfo)
		syslogf(SYSLOG_DEBUG, "Phone AUTOPROV'ed %s" % self.phone['macaddr'])

# Populated by Phone implementation modules
PhoneClasses = {}
