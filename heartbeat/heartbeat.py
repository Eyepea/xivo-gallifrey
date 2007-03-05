#!/usr/bin/python
# $Id$
#
# Server program

import os, sys, posix, string, pexpect
from socket import *
from random import randint

# Set the socket parameters
host = ""
portc = 5049
ports = 5050
port_ami = 5038
buf = 1024
addr = (host,ports)
pidfile = '/tmp/heartbeat_id_daemon.pid'

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

def id_to_string(idn):
    ch1 = chr((idn & 0x3f000000) >> 24)
    ch2 = chr((idn & 0x00ff0000) >> 16)
    ch3 = chr((idn & 0x0000ff00) >>  8)
    ch4 = chr((idn & 0x000000ff))
    chnull = chr(0) + chr(0) + chr(0) + chr(0)
    return(chnull + ch1 + ch2 + ch3 + ch4)

def get_zapchan(blocs):
    id = 0
    for x in blocs:
        #Zap/12-1:from-sip:12:2:Up:MeetMe:12:init::3:263062:(None)
        if (x.find(':') >= 0) and (x.find('/') >= 0) :
            xs = x.split(':')
            # xs = Zap/12-1 from-sip 12 2 Up MeetMe 12 init  3 263062 (None)
            xss = xs[0].split('/')
            # xss = Zap 12-1
            if (xss[0] == "Zap") and (xss[1].find('-') >= 0) :
                xsss = xss[1].split('-')
                if xs[4] == "Up":
                    zapchan = int(xsss[0]) - 1
                    if zapchan < 32:
                        id += (1<<zapchan)
    return id

def ami_command(pspawn, command):
    p.logfile = sys.stdout
    p.expect("Asterisk Call Manager/1.0")
    p.sendline("Action: login\rUsername: heartbeat\rSecret: heartbeat\r")
    p.expect("Message: Authentication accepted")
    p.sendline("Action: Command\rCommand: " + command + "\r")
    p.expect("--END COMMAND--")
    reply = p.before
    p.sendline("Action: Logoff\r")
    return reply

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
        else:
            p = pexpect.spawn('telnet localhost ' + str(port_ami))
            try:
                ami_reply = ami_command(p, "show channels concise")
                p.close()
            except:
                print "a problem occurred when trying to connect to Asterisk AMI (port", port_ami, ")"
                p.close()

            if sys.argv.count('-d') > 0:
                print "# Up Channels seen by Heartbeat :"
            blocs = ami_reply.split("\r\n")
            idnum = get_zapchan(blocs)

        replystring = id_to_string(idnum)

        replysocket = socket(AF_INET,SOCK_DGRAM)
        replysocket.bind(('',0))
        replysocket.sendto(replystring,(addr[0],portc))
        replysocket.close()
            
# Close socket
UDPSock.close()

