#!/usr/bin/python

__version__ = "$Revision$ $Date: 2007-06-15 18:42:49 +0200 (ven, 15 jun 2007) $"

import ConfigParser
import encodings.utf_8
import getopt
import md5
import os
import random
import re
import select
import signal
import socket
import SocketServer
import sys
import syslog
import threading
import time
import urllib
import _sre

dir_to_string = ">"
dir_from_string = "<"
allowed_states = ["available", "away", "outtolunch", "donotdisturb", "berightback"]

dummy_dir = ""
dummy_rchan = ""
dummy_exten = ""
dummy_mynum = ""

laststates = {}
willsoonrestart = 0
willrestart = False
time_restart = 2100 * 1000000
# global : userlist
# liste des champs :
#  user :             user name
#  passwd :           password
#  sessionid :        session id generated at connection
#  sessiontimestamp : last time when the client proved itself to be ALIVE :)
#  ip :               ip address of the client (current session)
#  port :             port here the client is listening.
#  state :            cf. allowed_states
# The user identifier will likely be its phone number

pidfile = '/var/run/e1alarmon.pid'
bufsize_large = 8192
bufsize_udp = 2048
bufsize_any = 512

socket.setdefaulttimeout(2)
timeout_between_registers = 5

## \brief Logins into the Asterisk Manager Interface.
def ami_socket_login(raddr, amiport, loginname, passname, events):
	try:
		sockid = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sockid.connect((raddr, amiport))
		sockid.recv(bufsize_large)
		# check against "Asterisk Call Manager/1.0\r\n"
		if events == True:
			sockid.send("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + passname + "\r\nEvents: on\r\n\r\n")
		else:
			sockid.send("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + passname + "\r\nEvents: off\r\n\r\n")
		# check against "Message: Authentication accepted\r\n"
	except:
		sockid = -1
	return sockid


## \brief Sends a Status request to the AMI.
def ami_socket_status(sockid):
	"""Sends a Status command to the socket sockid"""
	try:
		sockid.send("Action: Status\r\n\r\n")
		return True
	except:
		return False

## \brief Sends a Command to the AMI.
def ami_socket_command(sockid, command):
	"""Sends a <command> command to the socket sockid"""
	try:
		sockid.send("Action: Command\r\nCommand: " + command + "\r\n\r\n")
		return True
	except:
		return False


## \class myLDAP
class myLDAP:
        def __init__(self, ihost, iport, iuser, ipass):
                try:
                        self.l = ldap.initialize("ldap://%s:%s" %(ihost, iport))
                        self.l.protocol_version = ldap.VERSION3
                        self.l.simple_bind_s(iuser, ipass)
                        
                except ldap.LDAPError, exc:
                        print exc
                        sys.exit()

        def getldap(self, ibase, filter, attrib):
                try:
                        resultat = self.l.search_s(ibase,
                                                   ldap.SCOPE_SUBTREE,
                                                   filter,
                                        	   attrib)
			return resultat
		except ldap.LDAPError, exc:
			print exc


## \brief Function for Daemonizing
# \return none
def daemonize():
	try:
		pid = os.fork()
		if pid > 0: sys.exit(0)
	except OSError, exc:
		sys.exit(1)
	os.setsid()
	os.umask(0)
	try:
		pid = os.fork()
		if pid > 0: sys.exit(0)
	except OSError, exc:
		sys.exit(1)
	dev_null = file('/dev/null', 'r+')
	os.dup2(dev_null.fileno(), sys.stdin.fileno())
	os.dup2(dev_null.fileno(), sys.stdout.fileno())
	os.dup2(dev_null.fileno(), sys.stderr.fileno())


## \brief Logs actions to a log file, prepending them with a timestamp.
# \param string the string to log
# \return zero
# \sa log_debug
def varlog(string):
	global logfile
	syslog.syslog(syslog.LOG_NOTICE, "e1alarm_mon : " + string)
	if logfile:
		logfile.write(time.strftime("%b %2d %H:%M:%S ", time.localtime()) + string + "\n")
		logfile.flush()
	return 0


## \brief Outputs a string to stdout in no-daemon mode
# and always logs it.
# \param string the string to display and log
# \return the return code of the varlog call
# \sa varlog
def log_debug(string):
	if sys.argv.count('-d') > 0: print "#debug# " + string
	return varlog(string)


## \brief Returns a given field from an AMI line.
# \param lineami the line extracted from AMI
# \param field the field whose value one is interested in
# \return the value of the field
def getvalue(lineami, field):
	ret = "<NOFIELD>"
	s1 = lineami.split(";" + field + ": ")
	if len(s1) == 2:
		s2 = s1[1].split(";")[0]
		ret = s2
	return ret


## \brief Handling of AMI events occuring in Events=on mode.
# \param astnum the asterisk numerical identifier
# \param idata the data read from the AMI we want to parse
# \return none
# \sa handle_ami_event_dial, handle_ami_event_link, handle_ami_event_hangup
def handle_ami_event(astnum, idata):
	global plist, save_for_next_packet_events, laststates, willsoonrestart, willrestart, time_restart
	listkeys = plist[astnum].normal.keys()

	full_idata = save_for_next_packet_events[astnum] + idata
	evlist = full_idata.split("\r\n\r\n")
	save_for_next_packet_events[astnum] = evlist.pop()

	for z in evlist:
		# we assume no ";" character is present in AMI events fields
		x = z.replace("\r\n", ";")
		#print getvalue(x, "Context")
		if x.find("Reload;") == 7:
			# warning : "reload" as well as "reload manager" can appear here
			log_debug("AMI:Reload")
		elif x.find("Shutdown;") == 7:
			log_debug("AMI:Shutdown")
		elif x.find("AlarmClear;") == 7:
			log_debug("AMI:AlarmClear: %s" %getvalue(x, "Channel"))
			if willsoonrestart > 0:
				log_debug("will restart asterisk because AlarmClear after Red Alarm (%d received)" %willsoonrestart)
				willsoonrestart = 0
				willrestart = True
				time_restart = time.time()
		elif x.find("Alarm;") == 7:
			chan = getvalue(x, "Channel")
			alarmname = getvalue(x, "Alarm")
			if alarmname == "Red Alarm" and willsoonrestart == False:
				willsoonrestart = True
				log_debug("AMI:Alarm: flag set in order to restart Asterisk at next AlarmClear notifications")
			log_debug("AMI:Alarm: %s %s" %(alarmname, chan))
		elif x.find("PeerStatus;") == 7:
			log_debug("AMI:PeerStatus: %s %s" %(getvalue(x, "Peer"), getvalue(x, "PeerStatus")))
		elif x.find("Newchannel;") == 7:
			chan = getvalue(x, "Channel")
			if chan.find("SIP/") == 0:
				laststates[chan] = "Newchannel"
			log_debug("AMI:Newchannel: %s %s %s" %(getvalue(x, "Channel"), getvalue(x, "State"), getvalue(x, "CallerID")))
		elif x.find("Dial;") == 7:
			chan = getvalue(x, "Source")
			if chan.find("SIP/") == 0:
				laststates[chan] = "Dial"
			log_debug("AMI:Dial: %s %s" %(getvalue(x, "Source"), getvalue(x, "Destination")))
		elif x.find("Link;") == 7:
			chan = getvalue(x, "Channel1")
			if chan.find("SIP/") == 0:
				laststates[chan] = "Link"
			log_debug("AMI:Link: %s %s" %(getvalue(x, "Channel1"), getvalue(x, "Channel2")))
		elif x.find("Unlink;") == 7:
			chan = getvalue(x, "Channel1")
			if chan.find("SIP/") == 0:
				laststates[chan] = "Unlink"
			log_debug("AMI:Unlink: %s %s" %(getvalue(x, "Channel1"), getvalue(x, "Channel2")))
		elif x.find("Hangup;") == 7:
			chan = getvalue(x, "Channel")
			if chan.find("SIP/") == 0:
				log_debug("AMI:Hangup: %s" %chan)
				if chan in laststates:
##					if laststates[chan] == "Newchannel":
##						log_debug("restarting asterisk : " + str(laststates))
##						os.system("/etc/init.d/asterisk restart")
					del laststates[chan]
		elif x.find("Newstate;") == 7:
			pass
		elif x.find("Newexten;") == 7:
			pass
		elif x.find("Newcallerid;") == 7:
			pass
		else:
			if len(x) > 0:
				log_debug("AMI:XXX: " + plist[astnum].astid + " <" + x + ">")




## \brief Connects to the AMI if not yet.
# \param astnum Asterisk id to (re)connect
# \return none
def update_amisocks(astnum):
	if AMIsocks[astnum] == -1:
		log_debug(plist[astnum].astid + " : AMI (events = on)  : attempting to connect")
		als1 = ami_socket_login(configs[astnum].remoteaddr,
						 configs[astnum].ami_port,
						 configs[astnum].ami_login,
						 configs[astnum].ami_pass, True)
		AMIsocks[astnum] = als1
		if AMIsocks[astnum] != -1:
			ins.append(als1)
			log_debug(configs[astnum].astid + " : AMI (events = on)  : connected")
			"""Clears the channels before requesting a new status"""
			for x in plist[astnum].normal.keys():
				plist[astnum].normal[x].clear_channels()
		else:
			log_debug(configs[astnum].astid + " : AMI (events = on)  : could NOT connect")




## \class PhoneList
# \brief Properties of the lines of a given Asterisk
class PhoneList:
	## \var astid
	# \brief Asterisk id, the same as the one given in the configs

	## \var normal
	# \brief "Normal" phone lines, like SIP, IAX, Zap, ...

	## \var locals
	# \brief Local channels occuring on the Asterisk.

	## \var others
	# \brief Unmonitored channels, reserved for future use.

	##  \brief Class initialization.
	def __init__(self, iastid):
		self.astid = iastid
		self.normal = {}
		self.locals = {}
		self.others = {}




## \class AsteriskRemote
# \brief Properties of an Asterisk server
class AsteriskRemote:
	## \var astid
	# \brief Asterisk String ID
	
	## \var userlisturl
	# \brief Asterisk's URL
	
	## \var extrachannels
	# \brief Comma-separated List of the Channels not present in the SSO

	## \var localaddr
	# \brief Local IP address

	## \var remoteaddr
	# \brief Address of the Asterisk server

	## \var ipaddress_php
	# \brief IP address allowed to send CLI commands

	## \var portsipclt
	# \brief Local SIP port for the monitored Asterisk

	## \var portsipsrv
	# \brief SIP port of the monitored Asterisk

	## \var mysipaccounts
	# \brief SIP identifier as registered on the monitored Asterisk

	## \var ami_port
	# \brief AMI port of the monitored Asterisk

	## \var ami_login
	# \brief AMI login of the monitored Asterisk

	## \var ami_pass
	# \brief AMI password of the monitored Asterisk
	
	##  \brief Class initialization.
	def __init__(self,
		     astid,
		     userlisturl,
		     extrachannels,
		     localaddr = "127.0.0.1",
		     remoteaddr = "127.0.0.1",
		     ipaddress_php = "127.0.0.1",
		     ami_port = 5038,
		     ami_login = "xivouser",
		     ami_pass = "xivouser",
		     portsipclt = 5005,
		     portsipsrv = 5060,
		     mysipaccounts = []):

		self.astid = astid
		self.userlisturl = userlisturl
		self.extrachannels = extrachannels
		self.localaddr = localaddr
		self.remoteaddr = remoteaddr
		self.ipaddress_php = ipaddress_php
		self.portsipclt = portsipclt
		self.portsipsrv = portsipsrv
		self.ami_port = ami_port
		self.ami_login = ami_login
		self.ami_pass = ami_pass
		self.mysipaccounts = []
		sipaccs = mysipaccounts.split(";")
		for sipacc in sipaccs:
			[ctx, acc, zpass] = sipacc.split(",")
			self.mysipaccounts.append([ctx, acc, zpass])


# ==============================================================================
# Main Code starts here
# ==============================================================================

# daemonize if not in debug mode
if sys.argv.count('-d') == 0:
	daemonize()
try:
	f = open(pidfile, "w")
	try:
		f.write("%d\n"%os.getpid())
	finally:
		f.close()
except Exception, exc:
	print exc

xivoconffile = "/etc/asterisk/e1alarmon.conf"

opts, args = getopt.getopt(sys.argv[1:], "dc:", ["daemon", "config="])
for opt, arg in opts:
        if opt == "-c":
		xivoconffile = arg

xivoconf = ConfigParser.ConfigParser()
xivoconf.readfp(open(xivoconffile))

port_switchboard_base_sip = 5005
session_expiration_time = 60
log_filename = "/var/log/e1alarmon.log"
xivoconf_general = dict(xivoconf.items("general"))
capabilities = ""
asterisklist = ""

if "port_switchboard_base_sip" in xivoconf_general:
	port_switchboard_base_sip = int(xivoconf_general["port_switchboard_base_sip"])
if "expiration_session" in xivoconf_general:
	session_expiration_time = int(xivoconf_general["expiration_session"])
if "logfile" in xivoconf_general:
	log_filename = xivoconf_general["logfile"]
if "capabilities" in xivoconf_general:
	capabilities = xivoconf_general["capabilities"]
if "asterisklist" in xivoconf_general:
	asterisklist = xivoconf_general["asterisklist"].split(",")


with_ami = True
if "noami" in xivoconf_general: with_ami = False

configs = []
save_for_next_packet_events = []
save_for_next_packet_status = []
n = 0
ip_reverse_php = {}
ip_reverse_sht = {}

for i in xivoconf.sections():
	if i != "general" and i in asterisklist:
		xivoconf_local = dict(xivoconf.items(i))

		localaddr = "127.0.0.1"
		userlisturl = "sso.php"
		ipaddress = "127.0.0.1"
		ipaddress_php = "127.0.0.1"
		extrachannels = ""
		ami_port = 5038
		ami_login = "xivouser"
		ami_pass = "xivouser"
		sip_port = 5060
		sip_presence = ""

		if "localaddr" in xivoconf_local:
			localaddr = xivoconf_local["localaddr"]
		if "userlisturl" in xivoconf_local:
			userlisturl = xivoconf_local["userlisturl"]
		if "ipaddress" in xivoconf_local:
			ipaddress = xivoconf_local["ipaddress"]
		if "ipaddress_php" in xivoconf_local:
			ipaddress_php = xivoconf_local["ipaddress_php"]
		if "extrachannels" in xivoconf_local:
			extrachannels = xivoconf_local["extrachannels"]
		if "ami_port" in xivoconf_local:
			ami_port = int(xivoconf_local["ami_port"])
		if "ami_login" in xivoconf_local:
			ami_login = xivoconf_local["ami_login"]
		if "ami_pass" in xivoconf_local:
			ami_pass = xivoconf_local["ami_pass"]
		if "sip_port" in xivoconf_local:
			sip_port = int(xivoconf_local["sip_port"])
		if "sip_presence" in xivoconf_local:
			sip_presence = xivoconf_local["sip_presence"]

		configs.append(AsteriskRemote(i,
					      userlisturl,
					      extrachannels,
					      localaddr,
					      ipaddress,
					      ipaddress_php,
					      ami_port,
					      ami_login,
					      ami_pass,
					      port_switchboard_base_sip + n,
					      sip_port,
					      sip_presence))
		ip_reverse_sht[ipaddress] = n
		ip_reverse_php[ipaddress_php] = n
		save_for_next_packet_events.append("")
		save_for_next_packet_status.append("")
		n += 1



# opens the logfile for output in append mode
try:
	logfile = open(log_filename, 'a')
except Exception, exc:
	print "Could not open %s in append mode : %s" %(log_filename,exc)
	logfile = False

# user list initialized empty
userlist = []

plist = []
SIPsocks = []
AMIsocks = []
asteriskr = {}

ins = []

items_asterisks = xrange(len(configs))

log_debug("the monitored asterisk's is/are : " + str(asterisklist))
log_debug("# STARTING XIVO Daemon # (1/3) AMI socket connections")

for n in items_asterisks:
	plist.append(PhoneList(configs[n].astid))
	userlist.append({})
	asteriskr[configs[n].astid] = n
	AMIsocks.append(-1)

log_debug("# STARTING XIVO Daemon # (2/3) listening UI sockets")

lastrequest_time = []

askedtoquit = False

log_debug("# STARTING XIVO Daemon # (3/3) fetch SSO, SIP register and subscribe")
for n in items_asterisks:
	if with_ami: update_amisocks(n)
	lastrequest_time.append(time.time())


## \brief Handler for catching signals (in the main thread)
# \param signum signal number
# \param frame frame
# \return none
def sighandler(signum, frame):
	global askedtoquit
	print "--- signal", signum, "received : quits"
	askedtoquit = True


## \brief Handler for catching signals (in the main thread)
# \param signum signal number
# \param frame frame
# \return none
def sighandler_reload(signum, frame):
	global askedtoquit
	print "--- signal", signum, "received : quits (but will reload later on)"
	askedtoquit = False


signal.signal(signal.SIGINT, sighandler)
signal.signal(signal.SIGTERM, sighandler)
signal.signal(signal.SIGHUP, sighandler_reload)


# Receive messages
while not askedtoquit:
    try:
	    [i, o, e] = select.select(ins, [], [], timeout_between_registers)
    except Exception, exc:
	    if askedtoquit: sys.exit(5)
	    # TBD : if not askedtoquit => reload the config
	    else: sys.exit(6)
    if i:
	# these AMI connections are used in order to manage AMI commands with incoming events
	if filter(lambda j: j in AMIsocks, i):
		res = filter(lambda j: j in AMIsocks, i)[0]
		for n in items_asterisks:
			if AMIsocks[n] is res: break
		try:
			a = AMIsocks[n].recv(bufsize_any)
			if len(a) == 0: # end of connection from server side : closing socket
				log_debug(configs[n].astid + " : AMI (events = on)  : CLOSING")
				AMIsocks[n].close()
				ins.remove(AMIsocks[n])
				AMIsocks[n] = -1
			else:
				handle_ami_event(n, a)
		except Exception, exc:
			pass
	else:
		pass
		#log_debug("unknown socket " + str(i))

	if willrestart:
		print "will restart : ", time.time() - time_restart
		if (time.time() - time_restart) > 10:
			willrestart = False
			time_restart = 2100 * 1000000
			log_debug("Restarting Asterisk now (A)")
			os.system("/etc/init.d/asterisk restart")
	for n in items_asterisks:
		if (time.time() - lastrequest_time[n]) > timeout_between_registers:
			lastrequest_time[n] = time.time()
			if with_ami: update_amisocks(n)
    else:
	    if willrestart:
		    print "will restart : ", time.time() - time_restart
		    if (time.time() - time_restart) > 10:
			    willrestart = False
			    time_restart = 2100 * 1000000
			    log_debug("Restarting Asterisk now (B)")
			    os.system("/etc/init.d/asterisk restart")
	    for n in items_asterisks:
		    lastrequest_time[n] = time.time()
		    if with_ami: update_amisocks(n)


try:
	os.unlink(pidfile)
except Exception, exc:
	print exc

print 'end of the execution flow...'
sys.exit(0)

# Close files and sockets
logfile.close()

