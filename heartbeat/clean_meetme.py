#!/usr/bin/python
# $Id$
#
# Server program that cleans meeting rooms

import os
import socket
import string
import sys
import telnetlib
import time
from time import strftime
from random import randint

# Set global parameters
port_ami = 5038
pidfile = '/tmp/clean_meetme_id_daemon.pid'
time_global_sweep = 15
deltatime_last_heartbeat = 10

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
def log_debug(string):
	if sys.argv.count('-d') > 0:
		print "#debug# " + string
	varlog(string)
	return 0

# logins into the Asterisk MI
def ami_login(tnet, loginname, events):
	tnet.read_until("Asterisk Call Manager/1.0")
	if events == 0:
		tnet.write("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + loginname + "\r\nEvents: off\r\n\r\n");
	else:
		tnet.write("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + loginname + "\r\n\r\n");
	tnet.read_until("Message: Authentication accepted")
	return 0

# sends any command to the Asterisk MI
def ami_command(tnet, command):
	log_debug("Executing AMI command: <" + command + ">")
	tnet.write("Action: Command\r\nCommand: " + command + "\r\n\r\n")
	reply = tnet.read_until("--END COMMAND--")
	return reply

# sends the logoff command to the Asterisk MI
def ami_quit(tnet):
	tnet.write("Action: Logoff\r\n\r\n")
	return 0

# pings the IP address of the SIP channel according to "sip show peer"
def analyze_sipshowpeer(reply):
	rej = 0
	for l in reply.split("\n"):
		if l.find("Addr->IP") >= 0:
			addip = l.split(None)[2]
			if addip == "(Unspecified)":
				return 1

			# check the time of last update of the heartbeat info at a given IP address
			#				        current_time = time.time()
			current_time = time.time()
			try:
				statusfile = open("/tmp/heartbeat_" + addip + ".log", 'r')
				ttime = string.strip(statusfile.readline())
				statusfile.close()
			except:
				ttime = 0
			gtime = current_time - float(ttime)
			if gtime > deltatime_last_heartbeat:
				log_debug(addip + " not registered by heartbeat for " + str(gtime) + " seconds")
				rej = 1
	return rej

# kicks the unavailable users from the meetme rooms
def kick_if_needed():
        tn = telnetlib.Telnet("localhost", port_ami)
	try:
		ami_login(tn, "heartbeat", 0)
		ami_reply = ami_command(tn, "meetme")

		firstline_def  = "Conf Num       Parties"
		lastline_def   = "Total number of MeetMe users"
		emptyreply_def = "No active MeetMe conferences."

		busy_rooms = []
		if ami_reply.find(emptyreply_def) >= 0:
			log_debug("  MeetMe list is EMPTY")
		else:
			nrooms = 0
			nusers = 0
			iid = 0
			for line in ami_reply.split("\n"):
				if len(line) > 0:
					if line.find(lastline_def) >= 0:
						# might be useful to get the number of MeetMe users and check it against
						# the number of lines of the reply
						iid = 0
					if iid == 1:
						# 2              0002           N/A        03:24:18  Static
						nrooms = nrooms + 1
						fields = line.split(None)
						roomnumber = int(fields[0])
						number = int(fields[1])
						nusers = nusers + number
						if number > 1:
							busy_rooms.append(roomnumber)
					if line.find(firstline_def) >= 0:
						iid = 1
			log_debug("  MeetMe list is NOT empty : " +
				  str(nusers) + " user(s) ; " +
				  str(nrooms) + " room(s) ; " +
				  str(len(busy_rooms)) + " room(s) with >=2 users")

		# looping over the busy rooms
		for busy_room in busy_rooms:
			ami_reply = ami_command(tn, "meetme list " + str(busy_room))
			sip_th = str(busy_room + 100)
			not_rejected = []
			for mm in ami_reply.split("\n"):
				# for any User that is of the SIP kind
				if (mm.find("User #: ") == 0) and (mm.find("SIP") > 0):
					sipuser = mm.split(None)[2]
					sipname = mm.split(None)[3]
					if sipname != sip_th:
						reject = 1
					else:
						# look at its IP address
						ami_reply3 = ami_command(tn, "sip show peer " + sipname)
						reject = analyze_sipshowpeer(ami_reply3)

					if reject == 1:
						log_debug("I'm kicking OUT SIP/" + sipname + " (user # " + sipuser +
							  ") from the meeting room number " + str(busy_room) +
							  " right now")
						ami_command(tn, "meetme kick " + str(busy_room) + " " + sipuser)
					else:
						not_rejected.append(sipuser)
						log_debug("I let SIP/" + sipname +
							  " inside the meeting room number " + str(busy_room))

			num_not_rejected = len(not_rejected)
			log_debug("Number of SIP channels still in the meeting room #" + str(busy_room) + " : " + str(num_not_rejected))
			if num_not_rejected > 1:
				for i in range(0, num_not_rejected - 1):
					ami_command(tn, "meetme kick " + str(busy_room) + " " + not_rejected[i])
		ami_quit(tn)
		tn.close()
	except:
		print "a problem occurred when trying to connect to Asterisk AMI (port", port_ami, ")"
		tn.close()

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

# opens the logfile for output
logfile = open("/var/log/clean_meetme.log", 'a')

string_expected = ["Uniqueid: "]

# opens the AMI in order to watch for Call actions
try:
	tnwatch = telnetlib.Telnet("localhost", port_ami)
except:
	log_debug("Asterisk is probably not running : exiting")
	sys.exit(1)
ami_login(tnwatch, "heartbeat", 1)

# main loop
while 1:
	try:
		x,y,ami_reply = tnwatch.expect(string_expected, time_global_sweep)
	except:
		log_debug("Asterisk has probably been closed : exiting")
		sys.exit(1)

	if x == 0:
		id = 0
		sipnames = []
		for line in ami_reply.split("\n"):
			if line.find("Event: Newchannel") == 0:
				id = 1
			elif line.find("Channel: ") == 0 and (id == 1):
				sip_fullname = line.split(None)[1]
				sipnames.append(sip_fullname)
				id = 0

		for sip_fullname in sipnames:
			sip_noaddr   = sip_fullname.split('-')[0]
			sip_number   = sip_noaddr.split('/')[1]
			room = str(int(sip_number)-100)
			log_debug(str(len(sipnames)) + " - Incoming call from : " + sip_fullname +
				  " - I will check the meeting room number : " + room)

			tn = telnetlib.Telnet("localhost", port_ami)
			ami_login(tn, "heartbeat", 0)
			ami_reply = ami_command(tn, "meetme list " + room)
			for line_users in ami_reply.split("\n"):
				if (line_users.find("User #: ") == 0) and (line_users.find("SIP") >= 0) and (line_users.find(sip_fullname) < 0):
					log_debug("  An unwanted SIP channel has been found in the room " + room +
						  " - I'm kicking it out right now")
					log_debug("  " + line_users)
					ami_command(tn, "meetme kick " + room + " " + line_users.split(None)[2])
			ami_quit(tn)
			tn.close()
	else:
		log_debug("Timeout occured (" + str(time_global_sweep) + " s) : global sweeping is launched.")
		kick_if_needed()

# Close files and sockets
logfile.close()

