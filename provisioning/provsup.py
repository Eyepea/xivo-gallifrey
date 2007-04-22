# -*- coding: iso-8859-15 -*-
# Dependencies : arping

import os, sys, string, syslog, traceback

LISTEN_IPV4 = ""
LISTEN_PORT = 8666

TFTPROOT     = "/tftpboot/"
PROC_NET_DEV = "/proc/net/dev"
AUTHORIZED_PREFIX = ("eth",)

# wait time after arping in µs
ARPING	    = 'sudo /usr/sbin/arping'
SLEEP_PB    = 150000	# 150ms should be enough
			# XXX maybe some phones are really slow...

# r is a dictionary or None, and if e1 in r then r[el] must exists
def elem_or_none(r, el):
	if (r is None) or (el not in r):
		return None
	return r[el]

# In a string, substitute "{{varname}}" occurrences with value
# of variables["varname"], '\\' being an escaping char...
# This function works and has no bug ;)
# If you don't believe it draw the graph :}
def linesubst(line, variables):
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
					syslog.syslog(syslog.LOG_WARNING, 
					    ("Unknown variable '%s' detected, "+
					    "will just be replaced by an "+
					    "empty string") % (curvar,))
				else:
					syslog.syslog(syslog.LOG_DEBUG,
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
		syslog.syslog(syslog.LOG_WARNING, "st != NORM at end of line: " + line)
		syslog.syslog(syslog.LOG_WARNING, "returned substitution: " + out)
	return out

def txtsubst(lines, variables, target_file = None):
	if target_file:
		syslog.syslog("In process of generating file \"%s\"" % (target_file,))
	result = []
	for line in lines:
		result.append(linesubst(line, variables))
	return result

def exception_traceback():
	return map(lambda x: x.rstrip(), traceback.format_exception(*sys.exc_info()))

def log_debug_current_exception():
	for line in exception_traceback():
		syslog.syslog(syslog.LOG_DEBUG, line)

# Get Linux view of the list of network interfaces
def get_netdev_list():
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

# Get and filter the list of network interfaces, returning only those
# whose names begin with an element of AUTHORIZED_PREFIX
def get_ethdev_list():
	return tuple([e for e in get_netdev_list()
		      if True in map(lambda x: e.find(x) == 0,
		      		     AUTHORIZED_PREFIX)])

# input: mac address, with bytes in hexa, ':' separated
# ouput: mac address with format %02X:%02X:%02X:%02X:%02X:%02X
def normalize_mac_address(macaddr):
	macaddr_split = macaddr.upper().split(':', 6)
	if len(macaddr_split) != 6:
		raise "Bad format for mac address " + macaddr
	return string.join(map(lambda s: '%02X' % int(s, 16), macaddr_split), ':')

# macaddr must be normalized
# WARNING: macaddr_from_ipv4() makes implementation
# dependent use of this function
def ipv4_from_macaddr(macaddr, logexceptfunc = None):
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
	# ipv4_from_macaddr is indeed a symetrical fonction that can be
	# used to retrieve an ipv4 address from a given mac address
	# WARNING: this is of course implementation dependent
	return ipv4_from_macaddr(ipv4, logexceptfunc)

def well_formed_provcode(provcode):
	if provcode == '0':
		return True
	for d in provcode:
		if d not in '0123456789':
			return False
	return True

# Basic provisioning logic, including syslogs
class BaseProv:
	def __init__(self, phone):
		self.phone = phone
		syslog.syslog("Instantiation of %s" % (str(self.phone),))
	def action_reinit(self):
		if self.phone["actions"].lower() == "no": # possible cause: "distant" provisioning
			syslog.syslog("Skipping REINIT action for phone %s" % self.phone['macaddr'])
			return
		syslog.syslog("Sending REINIT command to phone %s" % self.phone['macaddr'])
		self.do_reinit()
		syslog.syslog("Sent REINIT command to phone %s" % self.phone['macaddr'])
	def action_reboot(self):
		if self.phone["actions"] == "no": # distant provisioning with actions disabled
			syslog.syslog("Skipping REBOOT action for phone %s" % self.phone['macaddr'])
			return
		syslog.syslog("Sending REBOOT command to phone %s" % self.phone['macaddr'])
		self.do_reboot()
		syslog.syslog("Sent REBOOT command to phone %s" % self.phone['macaddr'])
	def reinitprov(self):
		syslog.syslog("About to GUEST'ify the phone %s" % self.phone['macaddr'])
		self.do_reinitprov()
		syslog.syslog("Phone GUEST'ified %s" % self.phone['macaddr'])
		self.action_reinit()
	def autoprov(self, provinfo):
		syslog.syslog("About to AUTOPROV the phone %s" % self.phone['macaddr'])
		self.do_autoprov(provinfo)
		syslog.syslog("Phone AUTOPROV'ed %s" % self.phone['macaddr'])
		self.action_reboot()

# Populated by Phone implementation modules
PhoneClasses = {}
