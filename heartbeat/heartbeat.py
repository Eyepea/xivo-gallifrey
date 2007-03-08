#!/usr/bin/python
# $Id$
#
# Server program

import os
import sys
import string
import pexpect
from socket import *
from random import randint

# Set the socket parameters
host = ""
port_cli = 5049
port_srv = 5050
port_ami = 5038
buf = 1024
addr = (host,port_srv)
pidfile = '/tmp/heartbeat_id_daemon.pid'
loopc = 0

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
        ppidfile = open("/tmp/" + process + "_id_daemon.pid", 'r')
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

# logins into the Asterisk MI
def ami_login(pspawn, loginname):
    pspawn.expect("Asterisk Call Manager/1.0")
    pspawn.sendline("Action: login\rUsername: " + loginname + "\rSecret: " + loginname + "\r")
    pspawn.expect("Message: Authentication accepted")
    return 0

# sends any command to the Asterisk MI
def ami_command(pspawn, command):
    pspawn.sendline("Action: Command\rCommand: " + command + "\r")
    pspawn.expect("--END COMMAND--")
    reply = pspawn.before
    return reply

# sends the logoff command to the Asterisk MI
def ami_quit(pspawn):
    pspawn.sendline("Action: Logoff\r")
    return 0

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

# Create socket and bind to address
UDPSock = socket(AF_INET,SOCK_DGRAM)
UDPSock.bind(addr)

# Receive messages
while 1:
    data,addr = UDPSock.recvfrom(buf)
    if not data:
        print "Client has exited!"
        break
    else:
        ami_reply=""
        if sys.argv.count('-t') > 0:
		idnum = randint(0, (1 << 30) - 1)
	elif sys.argv.count('-l') > 0:
		idnum = 1 << loopc
		if loopc == 14:
			loopc = 16
		else:
			loopc = (loopc+1) % 31
	else :
            p = pexpect.spawn('telnet localhost ' + str(port_ami))
            try:
                ami_login(p, "heartbeat")
                ami_reply = ami_command(p, "show channels concise")
                ami_quit(p)
                p.close()
            except:
                print "a problem occurred when trying to connect to Asterisk AMI (port", port_ami, ")"
                p.close()

            if sys.argv.count('-d') > 0:
                print "# Up Channels seen by Heartbeat :"
            blocs = ami_reply.split("\r\n")
            idnum = get_zapchan(blocs)

        meetme = is_alive("clean_meetme")
        replystring = id_to_string(idnum, meetme)

	# reply on a different socket
        replysocket = socket(AF_INET,SOCK_DGRAM)
        replysocket.bind(('',0))
        replysocket.sendto(replystring,(addr[0],port_cli))
        replysocket.close()
	# reply on the same socket
	# UDPSock.sendto(replystring,(addr[0], addr[1]))
    
# Close socket
UDPSock.close()

