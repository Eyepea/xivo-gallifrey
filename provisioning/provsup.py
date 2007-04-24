# -*- coding: iso-8859-15 -*-
"""Common routines and objects daemon for autoprovisioning services in Xivo

Copyright (C) 2007, Proformatique

"""
# Dependencies : arping

import os, sys, traceback

import syslog
from syslog import syslog      as syslogf
from syslog import LOG_EMERG   as SYSLOG_EMERG
from syslog import LOG_ALERT   as SYSLOG_ALERT
from syslog import LOG_CRIT    as SYSLOG_CRIT
from syslog import LOG_ERR     as SYSLOG_ERR
from syslog import LOG_WARNING as SYSLOG_WARNING
from syslog import LOG_NOTICE  as SYSLOG_NOTICE
from syslog import LOG_INFO    as SYSLOG_INFO
from syslog import LOG_DEBUG   as SYSLOG_DEBUG

LISTEN_IPV4 = ""
LISTEN_PORT = 8666

TFTPROOT     = "/tftpboot/"
PROC_NET_DEV = "/proc/net/dev"
AUTHORIZED_PREFIX = ("eth",)

# wait time after arping in µs
ARPING	    = 'sudo /usr/sbin/arping'
SLEEP_PB    = 150000	# 150ms should be enough
			# XXX maybe some phones are really slow...

def elem_or_none(r, el):
	"r is a dictionary or None, and if (e1 in r) then r[el] must exists"
	if (r is None) or (el not in r):
		return None
	return r[el]

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

def log_current_exception(loglevel=SYSLOG_ERR):
	"""Log a backtrace of the current exception in the system logs, with
	the desired log level.
	
	"""
	for line in exception_traceback():
		syslogf(loglevel, line)

def get_netdev_list():
	"Get a view of network interfaces as seen by Linux"
	l=[]
	pnd = open(PROC_NET_DEV)
	pnd.readline()
	pnd.readline()
	line = pnd.readline()
	while line:
		if ':' in line:
			l.append(line.split(':',1)[0].strip())
		line = pnd.readline()
	return tuple(l)

def get_ethdev_list():
	"""Get and filter the list of network interfaces, returning only those
	whose names begin with an element of AUTHORIZED_PREFIX
	
	"""
	return tuple([e for e in get_netdev_list()
		      if True in map(lambda x: e.find(x) == 0,
		      		     AUTHORIZED_PREFIX)])

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
	must respond to ping broadcasts. Some stupid phones from well known
	stupid and expensive brands don't.

	"""
	# -r : will only display the IP address on stdout, or nothing
	# -c 1 : ping once
	# -w <xxx> : wait for the answer during <xxx> µs after the ping
	# -i <netiface> : the network interface to use is <netiface>
	for iface in get_ethdev_list():
		try:
		    ipfd = os.popen(ARPING +
			    (" -r -c 1 -w %s -i %s " % (SLEEP_PB, iface)) +
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
		self.phone = phone
		syslogf("Instantiation of %s" % (str(self.phone),))
	def action_reinit(self):
		if self.phone["actions"].lower() == "no": # possible cause: "distant" provisioning
			syslogf("Skipping REINIT action for phone %s" % self.phone['macaddr'])
			return
		syslogf("Sending REINIT command to phone %s" % self.phone['macaddr'])
		self.do_reinit()
		syslogf("Sent REINIT command to phone %s" % self.phone['macaddr'])
	def action_reboot(self):
		if self.phone["actions"] == "no": # distant provisioning with actions disabled
			syslogf("Skipping REBOOT action for phone %s" % self.phone['macaddr'])
			return
		syslogf("Sending REBOOT command to phone %s" % self.phone['macaddr'])
		self.do_reboot()
		syslogf("Sent REBOOT command to phone %s" % self.phone['macaddr'])
	def reinitprov(self):
		syslogf("About to GUEST'ify the phone %s" % self.phone['macaddr'])
		self.do_reinitprov()
		syslogf("Phone GUEST'ified %s" % self.phone['macaddr'])
		self.action_reinit()
	def autoprov(self, provinfo):
		syslogf("About to AUTOPROV the phone %s" % self.phone['macaddr'])
		self.do_autoprov(provinfo)
		syslogf("Phone AUTOPROV'ed %s" % self.phone['macaddr'])
		self.action_reboot()

# Populated by Phone implementation modules
PhoneClasses = {}
