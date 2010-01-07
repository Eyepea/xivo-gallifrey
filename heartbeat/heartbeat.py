#!/usr/bin/python
# $Id$
"""
Server program that replies Asterisk channels status on an UDP socket.
Copyright (C) 2007-2010 Proformatique
"""

__version__ = "$Revision$ $Date$"

import os
import select
import string
import sys
import telnetlib
import time
from time import strftime
from socket import *
from random import randint

# Set the socket parameters
host = ""
port_cli = 5049
port_srv = 5050
port_ami = 5038
buf = 1024
addr = (host,port_srv)
#
pidfile = '/var/run/heartbeat/heartbeat_id_daemon.pid'
loopc = 0
timeout_request_ami = 30
lastrequest_time = 0
replystring = chr(0) + chr(0) + chr(0) + chr(0) + chr(0) + chr(0) + chr(0) + chr(0)

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

# converts the numerical zap channels information to a string pattern
def id_to_string(idn, mm_status):
    ch5 = chr((idn & 0x7f000000) >> 24)
    ch6 = chr((idn & 0x00ff0000) >> 16)
    ch7 = chr((idn & 0x0000ff00) >>  8)
    ch8 = chr((idn & 0x000000ff))
    if(mm_status):
        ch1 = chr(1<<7)
    else:
        ch1 = chr(0)
    chnull = chr(0) + chr(0) + chr(0)
    return(ch1 + chnull + ch5 + ch6 + ch7 + ch8)

# looks whether a given daemon is running
def is_alive(process):
    try:
        ppidfile = open("/var/run/heartbeat/" + process + "_id_daemon.pid", 'r')
        ppid = string.strip(ppidfile.readline())
        ppidfile.close()
    except:
        return 0
    try:
        ppidfile = open("/proc/" + ppid + "/cmdline", 'r')
        ppidfile.close()
    except:
        return 0
    return 1

# converts the result of a "show channels concise" Asterisk command to
# a numerical bit-by-bit status
def get_zapchan(blocs):
	id = 0
	for x in blocs:
		#Zap/12-1:from-sip:12:2:Up:MeetMe:12:init::3:263062:(None)
		if (x.find('Zap/') == 0) and (x.find(':Up:') >= 0) :
			xs = x.split(':')
			# xs = Zap/12-1 from-sip 12 2 Up MeetMe 12 init  3 263062 (None)
			xss = xs[0].split('/')
			# xss = Zap 12-1
                        xsss = xss[1].split('-')
                        # xsss[0]=1-15+17-31 => zapchan=0-14+16-30
                        zapchan = int(xsss[0]) - 1
                        if zapchan < 31:
                            id += (1<<zapchan)
	return id

# logs actions to logfile "/var/log/heartbeat.log"
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
	if events == True:
		tnet.write("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + loginname + "\r\nEvents: on\r\n\r\n");
	else:
		tnet.write("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + loginname + "\r\nEvents: off\r\n\r\n");
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

# connects to AMI and returns the numerical ID corresponding to the Zap channels
def request_zap_status():
    tn = telnetlib.Telnet("localhost", port_ami)
    try:
        ami_login(tn, "heartbeat", False)
        ami_reply = ami_command(tn, "show channels concise")
        ami_quit(tn)
        tn.close()
        blocs = ami_reply.split("\n")
        idnum = get_zapchan(blocs)
        return idnum
    except:
        print "a problem occurred when trying to connect to Asterisk AMI (port", port_ami, ")"
        return 0

# returns the string built from clean_meetme status + zap_status
def update_status():
    if sys.argv.count('-t') > 0:
        idn = randint(0, (1 << 30) - 1)
    elif sys.argv.count('-l') > 0:
        idn = 1 << loopc
        if loopc == 14:
            loopc = 16
        else:
            loopc = (loopc+1) % 31
    else :
        idn = request_zap_status()
    log_debug("Up Channels seen by Heartbeat : " + str(idn))
    meetme = is_alive("clean_meetme")
    return id_to_string(idn, meetme)





# ===================================================================
# everything above was Object/function definitions, below
# starts the execution flow...

# daemonize if not in debug mode ;)
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
logfile = open("/var/log/heartbeat.log", 'a')

# Create socket and bind to address
UDPSock = socket(AF_INET,SOCK_DGRAM)
UDPSock.bind(addr)
ins = [UDPSock]

# Receive messages
while True:
	try:
		i,o,e = select.select(ins, [], [], timeout_request_ami)
		if i:
			data,addr = UDPSock.recvfrom(buf)
			log_debug("Sending reply to : " + addr[0])
			# reply on a different socket
			replysocket = socket(AF_INET,SOCK_DGRAM)
			replysocket.bind(('',0))
			replysocket.sendto(replystring,(addr[0],port_cli))
			replysocket.close()
			# reply on the same socket
			# UDPSock.sendto(replystring,(addr[0], addr[1]))

			# updates last heartbeat information for this IP address
			current_time = time.time()
			statusfile = open("/var/run/heartbeat/heartbeat_" + addr[0] + ".log", 'w')
			statusfile.write(str(current_time))
			statusfile.close()
		
			delta_time = current_time - lastrequest_time
			if delta_time > timeout_request_ami:
				# when no timeout has occured for a long time, the status is updated
				lastrequest_time = time.time()
				replystring = update_status()
		else:
			# when timeout occurs, the status is updated
			try:
				lastrequest_time = time.time()
				replystring = update_status()
			except Exception, exc:
				log_debug(str(exc))
	except Exception, exc:
		log_debug(str(exc))


# Close files and sockets
UDPSock.close()
logfile.close()

