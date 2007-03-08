#!/usr/bin/python
# $Id$
#
# Server program

import os
import sys
import string
import pexpect
import time
from time import strftime
from socket import *
from random import randint

# Set global parameters
port_ami = 5038
pidfile = '/tmp/clean_meetme_id_daemon.pid'
time_between_watch_fast_seconds = 3
time_between_watch_slow_seconds = 12
factor = time_between_watch_slow_seconds / time_between_watch_fast_seconds
idfactor = 0

# daemonize function
def daemonize():
	try:
		pid = os.fork()
		if pid > 0: sys.exit(0)
	except OSError, e: sys.exit(1)
	os.setsid()
	os.umask(0)
	try:
		pid = os.fork()
		if pid > 0: sys.exit(0)
	except OSError, e: sys.exit(1)
	dev_null = file('/dev/null', 'r+')
	os.dup2(dev_null.fileno(), sys.stdin.fileno())
	os.dup2(dev_null.fileno(), sys.stdout.fileno())
	os.dup2(dev_null.fileno(), sys.stderr.fileno())

# logs actions to logfile "/var/log/clean_meetme.log"
def varlog(string):
	logfile.write(strftime("%b %2d %H:%M:%S ", time.localtime()) + string + "\n")
	logfile.flush()
	return 0

# outputs a string to stdout in no-daemon mode
def debugs(string):
	if sys.argv.count('-d') > 0:
		print "#debug# " + string
	logfile.write(strftime("%b %2d %H:%M:%S ", time.localtime()) + "#debug# " + string + "\n")
	logfile.flush()
	return 0

# logins into the Asterisk MI
def ami_login(pspawn, loginname):
	pspawn.expect("Asterisk Call Manager/1.0")
	pspawn.sendline("Action: login\rUsername: " + loginname + "\rSecret: " + loginname + "\r")
	pspawn.expect("Message: Authentication accepted")
	return 0

# sends any command to the Asterisk MI
def ami_command(pspawn, command):
	debugs("Executing AMI command: " + command)
	pspawn.sendline("Action: Command\rCommand: " + command + "\r")
	pspawn.expect("--END COMMAND--")
	reply = pspawn.before
	return reply

# sends the logoff command to the Asterisk MI
def ami_quit(pspawn):
	pspawn.sendline("Action: Logoff\r")
	return 0

# pings a given address, with a timeout of 1s, and returns 1 if a
# pong has been received, 0 otherwise
def spawn_ping(ipaddress):
	debugs("Pinging IP address: " + ipaddress)
	exp_ping = pexpect.spawn("ping -c 1 -W 1 " + ipaddress)
	exp_ping.expect("packet loss")
	reply = exp_ping.before
	exp_ping.close()
	for x in reply.split("\r\n"):
		if(x.find("transmitted") >= 0):
			# 1 packets transmitted, 1 received, 0% packet loss
			y = x.split(None)
			return int(y[3])

# pings the IP address of the SIP channel according to "sip show peer"
def analyze_sipshowpeer(ami_reply):
	rej = 0
	for l in ami_reply.split("\r\n"):
		if l.find("Addr->IP") >= 0:
			addip = l.split(None)[2]
			if addip == "(Unspecified)":
				rej = 1
			else:
				if spawn_ping(l.split(None)[2]) == 0:
					rej = 1
	return rej

# kicks the unavailable users from the meetme rooms
def kick_if_needed(idf):
	p = pexpect.spawn('telnet localhost ' + str(port_ami))
	try:
		ami_login(p, "heartbeat")
		ami_reply1 = ami_command(p, "meetme")
		iid = 0
		firstline_def  = "Conf Num       Parties"
		lastline_def   = "Total number of MeetMe users"
		emptyreply_def = "No active MeetMe conferences."

		if ami_reply1.find(emptyreply_def) >= 0:
			debugs("empty MeetMe list")
		else:
			debugs("MeetMe list not empty")
			for line in ami_reply1.split("\r\n"):
				if len(line) > 0:
					if line.find(lastline_def) >= 0:
						# might be useful to get the number of MeetMe users and check it against
						# the number of lines of the reply
						iid = 0
					if iid == 1:
						# 2              0002           N/A        03:24:18  Static
						fields = line.split(None)
						roomnumber = int(fields[0])
						number = int(fields[1])
						docheck = 0
						if number > 2:
							docheck = 1
						elif number == 2 and idf == 0:
							docheck = 1
						if docheck:
							debugs(str(number) + " users in the " + str(roomnumber) + " conference")
							ami_reply2 = ami_command(p, "meetme list " + str(roomnumber))
							for mm in ami_reply2.split("\r\n"):
								# for any User that is of the SIP kind
								if (mm.find("User #: ") == 0) and (mm.find("SIP") > 0):
									sipuser = mm.split(None)[2]
									sipname = mm.split(None)[3]
									if(number > 2):
										reject = 1
									else:
										# look at its IP address
										ami_reply3 = ami_command(p, "sip show peer " + sipname)
										reject = analyze_sipshowpeer(ami_reply3)

									if reject == 1:
										debugs("I will kick out " + sipname +
										       " from the meeting room number " + str(roomnumber))
										ami_reply4 = ami_command(p, "meetme kick " + str(roomnumber) +
													 " " + sipuser)
									else:
										debugs("I will NOT kick out " + sipname +
										       " from the meeting room number " + str(roomnumber))
					if line.find(firstline_def) >= 0:
						iid = 1
		ami_quit(p)
		p.close()
	except:
		print "a problem occurred when trying to connect to Asterisk AMI (port", port_ami, ")"
		p.close()

# ===================================================================
# everything above was Object/function definitions, below
# starts the execution flow...

# daemonize if not in debug mode
if sys.argv.count('-d') == 0:
	daemonize()
try:
	f = open(pidfile, "w")
	try:
		f.write("%d\n"%os.getpid())
	finally:
		f.close()
except Exception, e:
	print e

logfile = open("/var/log/clean_meetme.log", 'a')

while 1:
	kick_if_needed(idfactor)
	idfactor = idfactor + 1
	if idfactor == factor:
		idfactor = 0
	time.sleep(time_between_watch_fast_seconds)

