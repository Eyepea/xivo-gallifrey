#!/usr/bin/python
# $Id$
#
# Server program

import os
import sys
import string
import pexpect
import time
from socket import *
from random import randint

# Set global parameters
port_ami = 5038
pidfile = '/tmp/clean_meetme_id_daemon.pid'
time_between_watch_seconds = 5

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

def ami_login(pspawn, loginname):
	pspawn.expect("Asterisk Call Manager/1.0")
	pspawn.sendline("Action: login\rUsername: " + loginname + "\rSecret: " + loginname + "\r")
	pspawn.expect("Message: Authentication accepted")
	return 0

def ami_command(pspawn, command):
	if sys.argv.count('-d') > 0:
                print "#debug# Executing AMI command:", command
	pspawn.sendline("Action: Command\rCommand: " + command + "\r")
	pspawn.expect("--END COMMAND--")
	reply = pspawn.before
	return reply

def ami_quit(pspawn):
	pspawn.sendline("Action: Logoff\r")
	return 0

def spawn_ping(ipaddress):
	if sys.argv.count('-d') > 0:
                print "#debug# Pinging IP address:", ipaddress
	exp_ping = pexpect.spawn("ping -c 1 -W 1 " + ipaddress)
	exp_ping.expect("packet loss")
	reply = exp_ping.before
	exp_ping.close()
	for x in reply.split("\r\n"):
		if(x.find("transmitted") >= 0):
			# 1 packets transmitted, 1 received, 0% packet loss
			y = x.split(None)
			return int(y[3])

def kick_if_needed():
	p = pexpect.spawn('telnet localhost ' + str(port_ami))
	try:
		ami_login(p, "heartbeat")
		ami_reply = ami_command(p, "meetme")
		iid = 0
		firstline_def  = "Conf Num       Parties"
		lastline_def   = "Total number of MeetMe users"
		emptyreply_def = "No active MeetMe conferences."

		if ami_reply.find(emptyreply_def) >= 0:
			if sys.argv.count('-d') > 0:
				print "#debug# empty MeetMe list"
		else:
			if sys.argv.count('-d') > 0:
				print "#debug# MeetMe list not empty"
			for line in ami_reply.split("\r\n"):
				if len(line) > 0:
					if line.find(lastline_def) >= 0:
						# might be useful to get the number of MeetMe users and check it against
						# the number of lines of the reply
						iid = 0
					if iid == 1:
						# 2              0002           N/A        03:24:18  Static
						fields = line.split(None)
						numch1 = int(fields[0])
						numch2 = int(fields[1])
						if(numch2 > 1):
							if sys.argv.count('-d') > 0:
								print "#debug#", str(numch2), "users in the", str(numch1), "conference"
							ami_reply2 = ami_command(p, "meetme list " + str(numch1))
							for mm in ami_reply2.split("\r\n"):
								# for any User that is also SIP
								if (mm.find("User #: ") == 0) and (mm.find("SIP") > 0):
									reject = 0
									# look at its IP address
									sipuser = mm.split(None)[2]
									sipname = mm.split(None)[3]
									ami_reply3 = ami_command(p, "sip show peer " + sipname)
									for l in ami_reply3.split("\r\n"):
										if l.find("Addr->IP") >= 0:
											addip = l.split(None)[2]
											if addip == "(Unspecified)":
												reject = 1
											else:
												if spawn_ping(l.split(None)[2]) == 0:
													reject = 1
									if reject == 1:
										if sys.argv.count('-d') > 0:
											print "#debug# I will kick out", sipname, "from the meeting room number", str(numch1)
										ami_reply4 = ami_command(p, "meetme kick " + str(numch1) + " " + sipuser)
									else:
										if sys.argv.count('-d') > 0:
											print "#debug# I will NOT kick out", sipname, "from the meeting room number", str(numch1)
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

while 1:
	kick_if_needed()
	time.sleep(time_between_watch_seconds)

