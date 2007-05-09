#!/usr/bin/python
# $Revision$
# $Date$
#
# Authors : Thomas Bernard, Corentin Le Gall, Benoit Thinot, Guillaume Knispel
#           Proformatique
#           67, rue Voltaire
#           92800 PUTEAUX
#           (+33/0)1.41.38.99.60
#           mailto:technique@proformatique.com
#           (C) 2007 Proformatique
#

## \mainpage
# \section section_1 General description of XIVO Daemon
# The XIVO Daemon aims to monitor all the actions taking place into one or
# more Asterisk servers, in order to provide 2 basic customer facilities :
# - a monitoring switchboard;
# - a customer information popup.
#
# This is achieved thanks to 3 mechanisms :
# - a notification of the Asterisk hints through SIP NOTIFY messages;
# - one or more connections to the Asterisk Manager Interface (AMI), where
# all the events can be watched;
# - Asterisk AGI's that send informations when a call is issued.
#
# This daemon is able to manage any number of Asterisk's one might wish.
#
# \section section_2 Initializations
#
# - Fetch the phone number lists from the SSO addresses.
# - Sending the first SIP REGISTERs and SUBSCRIBEs
#
# \section section_3 Main loop
# The main loop is triggered by a select() on the file descriptors for :
# - the SIP sockets (SIPsocks);
# - the AMI Event sockets (AMIsocks);
# - the UI sockets (UIsock, PHPUIsock);
# - the Caller Information Popup sockets (authentication, keepalive and identrequest).
#
# On reception of a SIP socket, parseSIP is called in order to either read SIP
# informations that might be useful for presence information, either to send
# a reply.
#
# On reception of AMI Events, handle_ami_event() parses the messages to update
# the detailed status of the channels.
#
# For each UI connection, a list of the open UI connections is updated
# (tcpopens_sb or tcpopens_php according to the kind of connection).
# This list is the one used to broadcast the miscellaneous updates
# (it is up to the UI clients to fetch the initial status with the "hints"
# command).
#
# \section section_5 Presence information from SIP/XML
# On startup, a given account (xivosb for instance) is SIP-REGISTERed for
# each Asterisk.
# This account then SIP-SUBSCRIBEs all the SIP phone numbers.
#
# \section section_6 Monitoring with AMI
#
# The AMI events are the basis for a channel-by-channel status of the phones.
# The SIP/XML events do not carry enough information, however they are useful
# for when no channel is open.
#
# Many AMI events are watched for, but not all of them are handled yet.
# The most useful ones are now : Dial, Link, Hangup, Rename.
# The following ones : Newexten, Newchannel, Newcallerid, Newstate are useful when dealing
# complex situations (when there are Local/ channels and Queues for instance).
#
# \section section_8 Caller Information Popup management
#
# The daemon has 3 other listening sockets :
# - Login - TCP - (the clients connect to it to login)
# - KeepAlive - UDP - (the clients send datagram to it to inform
#                      of their current state)
# - IdentRequest - TCP - offer a service to ask for localization and
#                        state of the clients.
# we use the SocketServer "framework" to implement the "services"
# see http://docs.python.org/lib/module-SocketServer.html
#
# \section section_9 Data Structures
#
# The statuses of all the lines/channels are stored in the multidimensional array/dict "plist",
# which is an array of PhoneList.
#
# plist[astn].normal[phonenum].chann[channel] are objects of the class ChannelStatus.
# - astn is the Asterisk id
# - phonenum is the phone id (SIP/<xx>, IAX2/<yy>, ...)
# - channel is the full channel name as known by Asterisk
#
## \file xivo_daemon.py
# \brief XIVO CTI server
#
## \namespace xivo_daemon
# \brief XIVO CTI server
#

import ConfigParser
import getopt
import MySQLdb
import os
import pyPgSQL 
import random
import select
import signal
import socket
import SocketServer
import sqlite
import sys
import threading
import time
import urllib
import xivo_ami
import xivo_sip

dir_to_string = ">"
dir_from_string = "<"
allowed_states = ["available", "away", "outtolunch", "donotdisturb", "berightback"]

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

pidfile = '/var/run/xivo_daemon.pid'
bufsize_large = 8192
bufsize_udp = 2048
bufsize_any = 512

socket.setdefaulttimeout(2)
timeout_between_registers = 60
expires = str(2 * timeout_between_registers) # timeout between subscribes


## \brief Function for Daemonizing
# \return none
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


## \brief Logs actions to a log file, prepending them with a timestamp.
# \param string the string to log
# \return zero
# \sa log_debug
def varlog(string):
	global logfile
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


## \brief Function to load sso.php user file.
# SIP, Zap, mISDN and IAX2 are taken into account there.
# There would remain MGCP, CAPI, h323, ...
# \param url the url where lies the sso, it can be file:/ as well as http://
# \param sipaccount the name of the reserved sip account (typically xivosb)
# \return the new phone numbers list
# \sa update_sipnumlist
def update_userlist_fromurl(astn, url, sipaccount):
	numlist = {}
	try:
		f = urllib.urlopen(url)
	except Exception, e:
		log_debug(configs[astn].astid + " : unable to open URL : " + url + " " + str(e))
		return numlist
	try:
		for line in f:
			# remove leading/tailing whitespaces
			line = line.strip()
			l = line.split('|')
			fullname = ""
			firstname = ""
			lastname = ""
			if len(l) > 7:
				fullname = l[7]
			if len(l) > 8:
				firstname = l[8]
			if len(l) > 9:
				lastname = l[9]
			# line is protocol | username | password | rightflag |
			#         phone number | initialized | disabled(=1) | callerid
                        if l[0] == "sip" and l[5] == "1" and l[6] == "0" and l[1] != sipaccount and l[4] != "":
				numlist["SIP/" + l[4]] = fullname, firstname, lastname
				adduser(astn, l[0]+l[4], l[2])
                        elif l[0] == "iax" and l[5] == "1" and l[6] == "0":
				numlist["IAX2/" + l[4]] = fullname, firstname, lastname
                        elif l[0] == "misdn" and l[5] == "1" and l[6] == "0":
				numlist["mISDN/" + l[4]] = fullname, firstname, lastname
                        elif l[0] == "zap" and l[5] == "1" and l[6] == "0":
				numlist["Zap/" + l[4]] = fullname, firstname, lastname
				adduser(astn, l[0]+l[4], l[2])
	finally:
		f.close()

	return numlist


## \brief Function to load the customer file.
# SIP, Zap, mISDN and IAX2 are taken into account there.
# There would remain MGCP, CAPI, h323, ...
# \param url the url where lies the customer list, it can be file:/ as well as http://
# \return the new phone numbers list
# \sa update_sipnumlist
def update_customers_fromurl(astn, url):
	try:
		f = urllib.urlopen(url)
	except Exception, e:
		log_debug(configs[astn].astid + " : unable to open URL : " + url + " " + str(e))
		return True
	try:
		for line in f:
			# remove leading/tailing whitespaces
			line = line.strip()
			l = line.split('|')
			# line is number | customer name
			customerbase[l[0]] = l[1]
	finally:
		f.close()
	return True


## \brief Function that fetches the call history into a database
# \param astn the asterisk to connect to
# \param sipnum the phone number
# \param nlines the number of lines to fetch for the given phone
def update_history_call(astn, sipnum, nlines):
	results = []
	try:
		conn = MySQLdb.connect(host = configs[astn].remoteaddr,
				       port = 3306,
				       user = "asterisk",
				       passwd = "asterisk",
				       db = "asterisk")
		cursor = conn.cursor()
		table = "cdr"
		sql = "SELECT * FROM %s WHERE ((src = %s) or (dst = %s)) ORDER BY calldate DESC LIMIT %s;" \
		      %(table,sipnum,sipnum,nlines)
		cursor.execute(sql)
		results = cursor.fetchall()
		conn.close()

	except Exception, e:
		log_debug("Connection to MySQL failed")

	return results


## \brief Extracts the main SIP properties from a received packet
# such as CSeq, message type (REGISTER, OPTION, SUBSCRIBE, ...),
# callid, address, number of lines, reurn code (200, 404, 484, ...).
# \param data the SIP buffer to parse
# \return an array containing these above informations
def read_sip_properties(data):
	cseq = 1
	msg = "xxx"
	cid = "no_callid@xivopy"
	ret = -99
	btag = "no_tag"

	lines = data.split("\r\n")
	if lines[0].find("SIP/2.0") == 0: ret = int(lines[0].split(None)[1])

	for x in lines:
		if x.find("CSeq") == 0:
			cseq = int(x.split(None)[1])
			msg = x.split(None)[2]
		elif x.find("From: ") == 0:   address = x.split("<sip:")[1].split("@")[0]
		elif x.find("Call-ID:") == 0: cid = x.split(None)[1]
		elif x.find("branch=") >= 0:  bbranch = x.split("branch=")[1].split(";")[0]
		elif x.find("tag=") >= 0:     btag = x.split("tag=")[1].split(";")[0]
	return [cseq, msg, cid, address, len(lines), ret, bbranch, btag]


## \brief Converts the SIP message to a useful presence information.
# Eventually, it will be done with XML functions.
# \param data the SIP message
# \return the extracted status
def tellpresence(data):
	num, stat = [None, None]
	lines = data.split("\n")

	for x in lines:
		if x.find("<note>") == 0:
			if x.find("Ready") >= 0:          stat = "Ready"
			elif x.find("On the phone") >= 0: stat = "On the phone"
			elif x.find("Ringing") >= 0:      stat = "Ringing"
			elif x.find("Not online") >= 0:   stat = "Not online"
			elif x.find("Unavailable") >= 0:  stat = "Unavailable"
			else:                             stat = "XivoUnknown"
		if x.find("<tuple id") == 0: num = x.split("\"")[1]
	return [num, stat]


## \class AMIClass
# AMI definition in order to interact with the Asterisk AMI.
class AMIClass:
	class AMIError(Exception):
		def __init__(self, msg):
                    self.msg = msg
		def __str__(self):
                    return msg

	# \brief Class initialization.
	def __init__(self, address, loginname, password):
		self.address   = address
		self.loginname = loginname
		self.password  = password
		self.i = 1
	# \brief Connection to a socket.
	def connect(self):
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect(self.address)
		self.f = s.makefile()
		s.close()
		str = self.f.readline()
		#print str,
	# \brief Sending any AMI command.
	def sendcommand(self, action, args):
		ret = False
		try:
			self.f.write('Action: ' + action + '\r\n')
			for (name, value) in args:
				self.f.write(name + ': ' + value + '\r\n')
			self.f.write('\r\n')
			self.f.flush()
			ret = True
		except:
			ret = False
		if ret == False:
			try:
				self.connect()
				self.login()
				if self:
					log_debug("retrying AMI command " + action)
					self.sendcommand(action, args)
			except Exception, e:
				log_debug("AMI not connected : " + str(e))
	# \brief For debug.
	def printresponse_forever(self):
		while True:
			str = self.f.readline()
			self.i = self.i + 1
	# \brief Reads a part of a reply.
	def readresponsechunk(self):
		start = True
		list = []
		while True:
			str = self.f.readline()
			#print "--------------", self.i, len(str), str,
			self.i = self.i + 1
			if start and str == '\r\n': continue
			start = False
			if str == '\r\n' or str == '': break
			l = [ x.strip() for x in str.split(': ') ]
			if len(l) == 2:
				list.append((l[0], l[1]))
		return dict(list)
	# \brief Reads the reply.
	def readresponse(self, check):
		first = self.readresponsechunk()
		if first=={}: return []
		if first['Response'] != 'Success':
			#and first['Response'] != 'Follows':
			if first.has_key('Message'):
				raise self.AMIError(first['Message'])
			else:
				raise self.AMIError('')
		if check == '':
			return []
		resp = []
		while True:
			chunk = self.readresponsechunk()
			#print "chunk", chunk
			if chunk=={}:
				#print 'empty chunk'
				resp.append(first)
				break
			resp.append(chunk)
			if not chunk.has_key('Event'):
				continue
				#break
			if chunk['Event'] == check:
				break
		return resp
	# \brief Logins to the AMI.
	def login(self):
		try:
			self.sendcommand('login',
					 [('Username', self.loginname),
					  ('Secret', self.password),
					  ('Events', 'off')])
			self.readresponse('')
			return True
		except self.AMIError, e:
			return False

	# \brief Executes a CLI command.
	def execclicommand(self, command):
		# special procession for cli commands.
		self.sendcommand('Command',
				 [('Command', command)])
		resp = []
		for i in (1, 2): str = self.f.readline()
		while True:
			str = self.f.readline()
			#print self.i, len(str), str,
			self.i = self.i + 1
			if str == '\r\n' or str == '' or str == '--END COMMAND--\r\n':
				break
			resp.append(str)
		return resp

	# \brief Hangs up a Channel.
	def hangup(self, channel, channel_peer):
		try:
			self.sendcommand('Hangup',
					 [('Channel', channel)])
			self.readresponse('')
			self.sendcommand('Hangup',
					 [('Channel', channel_peer)])
			self.readresponse('')
			return True
		except self.AMIError, e:
			return False

	# \brief Originates a call from a phone towards another.
	def originate(self, phonesrc, phonedst, locext):
		# originate a call btw src and dst
		# src will ring first, and dst will ring when src responds
		try:
			self.sendcommand('Originate', [('Channel', 'SIP/' + phonesrc),
						       ('Exten', phonedst),
						       ('Context', locext),
						       ('Priority', '1'),
						       # ('CallerID', phonesrc + " calls " + phonedst),
						       ('CallerID', phonesrc),
						       ('Async', 'true')])
			self.readresponse('')
			return True
		except self.AMIError, e:
			return False

	# \brief Transfers a channel towards a new extension.
	def transfer(self, channel, extension, context):
		try:
			self.sendcommand('Redirect', [('Channel', channel),
						      ('Exten', extension),
						      ('Context', context),
						      ('Priority', '1')])
			self.readresponse('')
			return True
		except self.AMIError, e:
			return False


## \brief Builds the full list of customers in order to send them to the requesting client.
# This should be done after a command called "customers".
# \return a string containing the full customers list
# \sa manage_tcp_connection
def build_customers():
	fullstat = "directory-response=2;Numero;Nom"
	for num in customerbase:
		fullstat += ";%s;%s" %(num, customerbase[num])
	fullstat += "\n"
	return fullstat


## \brief Builds the full list of callerIDs in order to send them to the requesting client.
# This should be done after a command called "callerid".
# \return a string containing the full callerIDs list
# \sa manage_tcp_connection
def build_callerids():
	global plist
	fullstat = "callerids="
	for n in items_asterisks:
		sskeys = plist[n].normal.keys()
		sskeys.sort()
		for phonenum in sskeys:
			phoneinfo = "cid:" + plist[n].astid + ":" \
				    + plist[n].normal[phonenum].tech + ":" \
				    + plist[n].normal[phonenum].phonenum + ":" \
				    + plist[n].normal[phonenum].calleridfull + ":" \
				    + plist[n].normal[phonenum].calleridfirst + ":" \
				    + plist[n].normal[phonenum].calleridlast
			fullstat += phoneinfo + ";"
	fullstat += "\n"
	return fullstat


## \brief Builds the base status (no channel information) for one phone identifier
# \param phoneid the "pointer" to the Asterisk phone statuses
# \return the string containing the base status of the phone
def build_basestatus(phoneid):
	basestatus = phoneid.tech + ":" \
		     + phoneid.phonenum  + ":" \
		     + phoneid.imstat + phoneid.voicemail + phoneid.queueavail  + ":" \
		     + phoneid.sipstatus
	return basestatus


## \brief Builds the channel-by-channel part for the hints/update replies.
# \param phoneid the "pointer" to the Asterisk phone statuses
# \return the string containing the statuses for each channel of the given phone
def build_fullstatlist(phoneid):
	nchans = len(phoneid.chann)
	fstat = str(nchans)
	for chan in phoneid.chann.keys():
		fstat += ":" + chan + ":" + \
			 phoneid.chann[chan].getStatus() + ":" + \
			 str(phoneid.chann[chan].getDeltaTime()) + ":" + \
			 phoneid.chann[chan].getDirection() + ":" + \
			 phoneid.chann[chan].getChannelPeer() + ":" + \
			 phoneid.chann[chan].getChannelNum()
	return fstat


## \brief Builds the full list of phone statuses in order to send them to the requesting client.
# \return a string containing the full list of statuses
def build_statuses():
	global plist
	fullstat = "hints="
	for n in items_asterisks:
		plist_normal_keys = plist[n].normal.keys()
		plist_normal_keys.sort()
		for phonenum in plist_normal_keys:
			plist[n].normal[phonenum].update_time()
			phoneinfo = "hnt:" + plist[n].astid + ":" + build_basestatus(plist[n].normal[phonenum])
			fullstat += phoneinfo + ":" + build_fullstatlist(plist[n].normal[phonenum]) + ";"
	fullstat += "\n"
	return fullstat


## \brief Sends a status update to all the connected xivo-switchboard(-like) clients.
# \param astnum the asterisk numerical identifier
# \param phonenum the phone identifier
# \param fromwhom a string that tells who has requested such an update
# \return none
def update_GUI_clients(astnum, phonenum, fromwhom):
	global tcpopens_sb, plist
	phoneinfo = fromwhom + ":" + plist[astnum].astid + ":" + build_basestatus(plist[astnum].normal[phonenum])
	fstatlist = build_fullstatlist(plist[astnum].normal[phonenum])
	for tcpclient in tcpopens_sb:
		try:
			tcpclient[0].send("update=" + phoneinfo + ":" + fstatlist + "\n")
		except Exception, e:
			log_debug("send has failed on " + str(tcpclient[0])+ " " + str(e))


## \brief Handles the SIP messages according to their meaning (reply to a formerly sent message).
# \param astnum the asterisk numerical identifier
# \param data   the data read from the socket
# \param l_sipsock the socket identifier in order to reply
# \param l_addrsip the SIP address in order to reply
# \return True if it is an OPTIONS packet
# \sa read_sip_properties
def parseSIP(astnum, data, l_sipsock, l_addrsip):
    global tcpopens_sb, plist, configs
    spret = False
    [icseq, imsg, icid, iaddr, ilength, iret, ibranch, itag] = read_sip_properties(data)
    # if ilength != 11:
    #print "###", astnum, ilength, icseq, icid, iaddr, imsg, iret, ibranch, itag
##    if imsg == "REGISTER" and iret == 200 and icid == "reg_cid@xivopy":
##        for k in tcpopens_sb:
##            k[0].send("asterisk=registered_" + configs[astnum].astid + "\n")
    if imsg == "SUBSCRIBE":
	    sipphone = "SIP/" + icid.split("@")[0].split("subscribexivo_")[1]
	    if sipphone in plist[astnum].normal.keys(): # else : send sth anyway ?
		    plist[astnum].normal[sipphone].set_lasttime(time.time())
		    if iret != 200:
			    if plist[astnum].normal[sipphone].sipstatus != "Fail" + str(iret):
				    plist[astnum].normal[sipphone].set_sipstatus("Fail" + str(iret))
				    update_GUI_clients(astnum, sipphone, "sip___1")
    if imsg == "OPTIONS" or imsg == "NOTIFY":
	    command = xivo_sip.sip_ok(configs[astnum], "sip:" + configs[astnum].mysipname,
				      icseq, icid, iaddr, imsg, ibranch, itag)
	    l_sipsock.sendto(command,(configs[astnum].remoteaddr, l_addrsip[1]))
	    if imsg == "NOTIFY":
		    sipnum, sippresence = tellpresence(data)
		    if [sipnum, sippresence] != [None, None]:
			    sipphone = "SIP/" + sipnum
			    if sipphone in plist[astnum].normal:
				    plist[astnum].normal[sipphone].set_lasttime(time.time())
				    if plist[astnum].normal[sipphone].sipstatus != sippresence:
					    plist[astnum].normal[sipphone].set_sipstatus(sippresence)
					    update_GUI_clients(astnum, sipphone, "sip___2")
				    else:
					    pass
			    else:
				    pass
	    else:
		    spret = True
    return spret


## \brief Extracts the phone number and the channel name from the asterisk/SIP/num-08abcf
# UI syntax for hangups or transfers
# \param fullname full string sent by the UI
# \return the phone number and the channel name, without the asterisk id
def split_from_ui(fullname):
	phone = ""
	channel = ""
	s1 = fullname.split("/")
	if len(s1) == 3:
		phone = s1[1] + "/" + s1[2].split("-")[0]
		channel = s1[1] + "/" + s1[2]
	return [phone, channel]


## \brief Deals with requests from the UI clients.
# \param connid connection identifier
# \param allow_events tells if this connection belongs to events-allowed ones
# (for switchboard) or to events-disallowed ones (for php CLI commands)
# \return none
def manage_tcp_connection(connid, allow_events):
    global AMIclasssock, AMIcomms, ins
    try:
	    msg = connid[0].recv(bufsize_large)
    except Exception, e:
	    msg = ""
	    log_debug("UI connection : a problem occured when recv from " + str(connid[0]) + " " + str(e))
    if len(msg) == 0:
        connid[0].close()
        ins.remove(connid[0])
	if allow_events == True:
		tcpopens_sb.remove(connid)
		log_debug("TCP (SB)  socket closed from " + connid[1] + " " + str(connid[2]))
	else:
		tcpopens_php.remove(connid)
		log_debug("TCP (PHP) socket closed from " + connid[1] + " " + str(connid[2]))
    else:
        # what if more lines ???
        usefulmsg = msg.split("\r\n")[0].split("\n")[0]
        if usefulmsg == "hints":
		try:
			connid[0].send(build_statuses())
		except Exception, e:
			log_debug("UI connection : a problem occured when sending to " + str(connid[0]) + " " + str(e))
        elif usefulmsg == "callerids":
		try:
			connid[0].send(build_callerids())
		except Exception, e:
			log_debug("UI connection : a problem occured when sending to " + str(connid[0]) + " " + str(e))
	elif usefulmsg == "keepalive":
		try:
			connid[0].send("keepalive=\n")
		except Exception, e:
			log_debug("UI connection : a problem occured when sending to " + str(connid[0]) + " " + str(e))
	elif usefulmsg != "":
		l = usefulmsg.split()
		if len(l) == 2 and l[0] == 'hangup':
			idassrc = -1
			assrc = l[1].split("/")[0]
			if assrc in asteriskr: idassrc = asteriskr[assrc]
			if idassrc == -1:
				connid[0].send("asterisk=hangup KO : no such asterisk id\n")
			else:
				log_debug("attempting a HANGUP : " + str(l))
				phone, channel = split_from_ui(l[1])
				if phone in plist[idassrc].normal:
					if channel in plist[idassrc].normal[phone].chann:
						channel_peer = plist[idassrc].normal[phone].chann[channel].getChannelPeer()
						log_debug("UI action : " + configs[idassrc].astid + \
							  " : hanging up <" + channel + "> and <" + \
							  channel_peer + ">")
						if not AMIclasssock[idassrc]:
							log_debug("AMI was not connected - attempting to connect again")
							AMIclasssock[idassrc] = connect_to_AMI((configs[idassrc].remoteaddr,
												configs[idassrc].ami_port),
											       configs[idassrc].ami_login,
											       configs[idassrc].ami_pass)
						if AMIclasssock[idassrc]:
							ret = AMIclasssock[idassrc].hangup(channel, channel_peer)
							if ret:
								connid[0].send("asterisk=hangup successful\n")
							else:
								connid[0].send("asterisk=hangup KO : socket request failed\n")
						else:
							connid[0].send("asterisk=hangup KO : no socket available\n")
					else:
						connid[0].send("asterisk=hangup KO : no such channel\n")
				else:
					connid[0].send("asterisk=hangup KO : no such phone\n")
		elif len(l) == 2 and l[0] == 'directory-search':
			try:
				connid[0].send(build_customers())
			except Exception, e:
				log_debug("UI connection : a problem occured when sending to " + str(connid[0]) + " " + str(e))
		elif len(l) == 3 and (l[0] == 'originate' or l[0] == 'transfer'):
			idassrc = -1
			assrc = l[1].split("/")[0]
			if assrc in asteriskr: idassrc = asteriskr[assrc]
			idasdst = -1
			asdst = l[2].split("/")[0]
			if asdst in asteriskr: idasdst = asteriskr[asdst]
			if idassrc != -1 and idassrc == idasdst:
				if not AMIclasssock[idassrc]:
					"AMI was not connected - attempting to connect again"
					AMIclasssock[idassrc] = connect_to_AMI((configs[idassrc].remoteaddr,
										configs[idassrc].ami_port),
									       configs[idassrc].ami_login,
									       configs[idassrc].ami_pass)
				if AMIclasssock[idassrc]:
					if l[0] == 'originate':
						log_debug("attempting a ORIGINATE : " + str(l))
						if l[2].split("/")[1] == "SIP":
							ret = AMIclasssock[idassrc].originate(l[1].split("/")[2],
											      l[2].split("/")[2],
											      "local-extensions")
						else:
							if l[2].split("/")[1] != "":
								ret = AMIclasssock[idassrc].originate(l[1].split("/")[2],
												      l[2].split("/")[1],
												      "extern-extensions")
							else:
								ret = False
						if ret:
							connid[0].send("asterisk=originate %s %s successful\n" %(l[1], l[2]))
						else:
							connid[0].send("asterisk=originate %s %s KO\n" %(l[1], l[2]))
					elif l[0] == 'transfer':
						log_debug("attempting a TRANSFER : " + str(l))
						phonesrc, phonesrcchan = split_from_ui(l[1])
						if phonesrc == phonesrcchan:
							connid[0].send("asterisk=transfer KO : %s not a channel\n" %phonesrcchan)
						else:
							phonedst = l[2].split("/")[2]
							if phonesrc in plist[idassrc].normal.keys():
								channellist = plist[idassrc].normal[phonesrc].chann
								nopens = len(channellist)
								if nopens == 0:
									log_debug("no channel currently open in the phone " + phonesrc)
								else:
									ret = AMIclasssock[idassrc].transfer(channellist[phonesrcchan].getChannelPeer(), phonedst, "local-extensions")
									if ret:
										connid[0].send("asterisk=transfer successful " + str(idassrc) + "\n")
									else:
										connid[0].send("asterisk=transfer KO\n")
			else:
				connid[0].send("asterisk=originate or transfer KO : asterisk id mismatch\n")
		elif len(l) == 3 and l[0] == 'history':
			idassrc = -1
			assrc = l[1].split("/")[0]
			if assrc in asteriskr: idassrc = asteriskr[assrc]
			if idassrc == -1:
				connid[0].send("asterisk=hangup KO : no such asterisk id\n")
			else:
				phone, channel = split_from_ui(l[1])
				hist = update_history_call(idassrc, phone.split("/")[1], l[2])
				repstr = "history="
				separ = ";"
				for x in hist:
					repstr = repstr + x[0].isoformat() + separ + x[1] \
						 + separ + str(x[10]) + separ + x[11]
					if phone.split("/")[1] == x[2]:
						repstr = repstr + separ + x[3] + separ + "OUT"
					elif phone.split("/")[1] == x[3]:
						repstr = repstr + separ + x[2] + separ + "IN"
					else:
						repstr = repstr + separ + separ + "UNKNOWN"
					repstr = repstr + ";"
				connid[0].send(repstr + "\n")
		elif allow_events == False: # i.e. if PHP-style connection
			n = -1
			if connid[1] in ip_reverse_php: n = ip_reverse_php[connid[1]]
			if n == -1:
				connid[0].send("XIVO CLI:CLIENT NOT ALLOWED\n")
			else:
				connid[0].send("XIVO CLI:" + configs[n].astid + "\n")
				try:
					s = AMIclasssock[n].execclicommand(usefulmsg.strip())
					try:
						for x in s: connid[0].send(x)
						connid[0].send("XIVO CLI:OK\n")
					except Exception, e:
						log_debug(configs[n].astid + " : could not send reply : " + str(e))
				except Exception, e:
					connid[0].send("XIVO CLI:KO Exception : " + str(e) + "\n")
		else:
			connid[0].send("XIVO CLI:NOT ALLOWED from Switchboard\n")


## \brief Tells whether a channel is a "normal" one, i.e. SIP, IAX2, mISDN, Zap
# or not (like Local, Agent, ... anything else).
# \param chan the channel-like string (that should be like "proto/phone-id")
# \return True or False according to the above description
def is_normal_channel(chan):
	if chan.find("SIP/") == 0 or chan.find("IAX2/") == 0 or \
	   chan.find("mISDN/") == 0 or chan.find("Zap/") == 0: return True
	else: return False


## \brief Updates some channels according to the Dial events occuring in the AMI.
# \param listkeys the list of allowed phones
# \param astnum the Asterisk numerical identifier
# \param src the source channel
# \param dst the dest channel
# \param clid the callerid
# \param clidn the calleridname
# \return none
def handle_ami_event_dial(listkeys, astnum, src, dst, clid, clidn):
	global plist
	if is_normal_channel(src):
		phonenum = src.split("-")[0]
		if phonenum in listkeys:
			plist[astnum].normal[phonenum].set_chan(src, "Calling", 0, dir_to_string, dst, "")
			update_GUI_clients(astnum, phonenum, "ami-ed1")
		else: plist[astnum].others[src] = "d"
	elif src.find("Local/") == 0:
		if src in plist[astnum].locals:
			plist[astnum].locals[src].set_chan("Dial", dst)
			log_debug("[watch] Local/ Dial : " + src + " " + \
				  plist[astnum].locals[src].state + " " + \
				  plist[astnum].locals[src].callerid + " " + \
				  plist[astnum].locals[src].peer)
		else: plist[astnum].others[src] = "d"
	else: plist[astnum].others[src] = "d"

	if is_normal_channel(dst):
		phonenum = dst.split("-")[0]
		if phonenum in listkeys:
			if clid == "<unknown>" and is_normal_channel(src):
				clid = src.split("-")[0].split("/")[1]
			plist[astnum].normal[phonenum].set_chan(dst, "Ringing", 0, dir_from_string, src, clid)
			update_GUI_clients(astnum, phonenum, "ami-ed2")
		else: plist[astnum].others[dst] = "D"
	elif dst.find("Local/") == 0:
		log_debug("[watch] Dial to Local/ : " + src + " " + dst)
		plist[astnum].others[dst] = "D"
	else: plist[astnum].others[dst] = "D"


## \brief Updates some channels according to the Link events occuring in the AMI.
# \param listkeys the list of allowed phones
# \param astnum the Asterisk numerical identifier
# \param src the source channel
# \param dst the dest channel
# \param clid1 the src callerid
# \param clid2 the dest callerid
# \return none
def handle_ami_event_link(listkeys, astnum, src, dst, clid1, clid2):
	global plist
	if is_normal_channel(src):
		phonenum = src.split("-")[0]
		if phonenum in listkeys:
			plist[astnum].normal[phonenum].set_chan(src, "On the phone", 0, dir_to_string, dst, clid2)
			update_GUI_clients(astnum, phonenum, "ami-el1")
		else: plist[astnum].others[src] = "l"
	elif src.find("Local/") == 0:
		if src in plist[astnum].locals:
			plist[astnum].locals[src].set_chan("Link", dst)
			log_debug("[watch] Local/ Link : " + src + " " + \
				  plist[astnum].locals[src].state + " " + \
				  plist[astnum].locals[src].callerid + " " + \
				  plist[astnum].locals[src].peer)
		else: plist[astnum].others[src] = "l"
	else: plist[astnum].others[src] = "l"

	if is_normal_channel(dst):
		phonenum = dst.split("-")[0]
		if phonenum in listkeys:
			if clid1 == "(null)" and is_normal_channel(src):
				clid1 = src.split("-")[0].split("/")[1]
			plist[astnum].normal[phonenum].set_chan(dst, "On the phone", 0, dir_from_string, src, clid1)
			update_GUI_clients(astnum, phonenum, "ami-el2")
		else: plist[astnum].others[dst] = "L"
	elif dst.find("Local/") == 0: # occurs when someone picks up the phone
		if dst in plist[astnum].others.keys(): del plist[astnum].others[dst]
		# here dst ends with ",1" => binding with the same with ",2"
		newdst = dst.replace(",1", ",2")
		log_debug("[watch] Link to Local/ : " + src + " " + dst + " => " + newdst)
		#plist[astnum].others[newdst] = "L"
		if newdst in plist[astnum].locals:
			phonenuma = src.split("-")[0]
			phonenumb = plist[astnum].locals[newdst].peer.split("-")[0]

			if phonenuma in plist[astnum].normal:
				plist[astnum].normal[phonenuma].set_chan(src, "On the phone", 0,
									 dir_to_string,
									 plist[astnum].locals[newdst].peer,
									 plist[astnum].locals[newdst].peer)
				update_GUI_clients(astnum, phonenuma, "ami-eq1")
			else:
				pass
			if phonenumb in plist[astnum].normal:
				plist[astnum].normal[phonenumb].set_chan(plist[astnum].locals[newdst].peer,
									 "On the phone", 0,
									 dir_from_string, src,
									 plist[astnum].locals[newdst].callerid)
				update_GUI_clients(astnum, phonenumb, "ami-eq2")
			else:
				pass
	else: plist[astnum].others[dst] = "L"


## \brief Updates some channels according to the Hangup events occuring in the AMI.
# \param listkeys the list of allowed phones
# \param astnum the Asterisk numerical identifier
# \param chan the channel
# \param cause the reason why there has been a hangup (not used)
# \return
def handle_ami_event_hangup(listkeys, astnum, chan, cause):
	global plist
	if is_normal_channel(chan):
		phonenum = chan.split("-")[0]
		if phonenum in listkeys:
			plist[astnum].normal[phonenum].set_chan_hangup(chan)
			update_GUI_clients(astnum, phonenum, "ami-eh1")
			plist[astnum].normal[phonenum].del_chan(chan)
			update_GUI_clients(astnum, phonenum, "ami-eh2")
		else:
			if chan in plist[astnum].others.keys(): del plist[astnum].others[chan]
	elif chan.find("Local/") == 0:
		if chan in plist[astnum].locals:
			plist[astnum].locals[chan].set_chan("Hup", "")
			log_debug("[watch] Local/ Hangup : " + chan + " " + \
				  plist[astnum].locals[chan].state + " " + \
				  plist[astnum].locals[chan].callerid + " " + \
				  plist[astnum].locals[chan].peer)
		else:
			if chan in plist[astnum].others.keys(): del plist[astnum].others[chan]
	else:
		if chan in plist[astnum].others.keys(): del plist[astnum].others[chan]


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
	global plist, save_for_next_packet_events
	listkeys = plist[astnum].normal.keys()

	full_idata = save_for_next_packet_events[astnum] + idata
	evlist = full_idata.split("\r\n\r\n")
	save_for_next_packet_events[astnum] = evlist.pop()

	for z in evlist:
		# we assume no ";" character is present in AMI events fields
		x = z.replace("\r\n", ";")
		if x.find("Dial;") == 7:
			src   = getvalue(x, "Source")
			dst   = getvalue(x, "Destination")
			clid  = getvalue(x, "CallerID")
			clidn = getvalue(x, "CallerIDName")
			handle_ami_event_dial(listkeys, astnum, src, dst, clid, clidn)
		elif x.find("Link;") == 7:
			src   = getvalue(x, "Channel1")
			dst   = getvalue(x, "Channel2")
			clid1 = getvalue(x, "CallerID1")
			clid2 = getvalue(x, "CallerID2")
			handle_ami_event_link(listkeys, astnum, src, dst, clid1, clid2)
		elif x.find("Unlink;") == 7:
			# might be something to parse here
			src   = getvalue(x, "Channel1")
			dst   = getvalue(x, "Channel2")
			clid1 = getvalue(x, "CallerID1")
			clid2 = getvalue(x, "CallerID2")
		elif x.find("Hangup;") == 7:
			chan  = getvalue(x, "Channel")
			cause = getvalue(x, "Cause-txt")
			handle_ami_event_hangup(listkeys, astnum, chan, cause)
		elif x.find("Reload;") == 7:
			# warning : "reload" as well as "reload manager" can appear here
			log_debug("AMI:Reload: " + plist[astnum].astid)
		elif x.find("Shutdown;") == 7:
			log_debug("AMI:Shutdown: " + plist[astnum].astid)
		elif x.find("Join;") == 7:
			clid  = getvalue(x, "CallerID")
			qname = getvalue(x, "Queue")
			if len(clid) > 0:
				for k in tcpopens_sb:
					k[0].send("asterisk=<%s> is calling the Queue <%s>\n" %(clid, qname))
		elif x.find("PeerStatus;") == 7:
			# <-> register's ? notify's ?
			pass
		elif x.find("Agentlogin;") == 7:
			log_debug("//AMI:Agentlogin// %s : %s" %(plist[astnum].astid, x))
		elif x.find("Agentlogoff;") == 7:
			log_debug("//AMI:Agentlogoff// %s : %s" %(plist[astnum].astid, x))
		elif x.find("Agentcallbacklogin;") == 7:
			log_debug("//AMI:Agentcallbacklogin// %s : %s" %(plist[astnum].astid, x))
		elif x.find("Agentcallbacklogoff;") == 7:
			log_debug("//AMI:Agentcallbacklogoff// %s : %s" %(plist[astnum].astid, x))
		elif x.find("AgentCalled;") == 7:
			log_debug("//AMI:AgentCalled// %s : %s" %(plist[astnum].astid, x))
		elif x.find("ParkedCallsComplete;") == 7:
			log_debug("//AMI:ParkedCallsComplete// %s : %s" %(plist[astnum].astid, x))
		elif x.find("ParkedCalled;") == 7:
			log_debug("//AMI:ParkedCalled// %s : %s" %(plist[astnum].astid, x))
		elif x.find("Cdr;") == 7:
			log_debug("//AMI:Cdr// %s : %s" %(plist[astnum].astid, x))
		elif x.find("Alarm;") == 7:
			log_debug("//AMI:Alarm// %s : %s" %(plist[astnum].astid, x))
		elif x.find("AlarmClear;") == 7:
			log_debug("//AMI:AlarmClear// %s : %s" %(plist[astnum].astid, x))
		elif x.find("MeetmeJoin;") == 7:
			channel = getvalue(x, "Channel")
			meetme = getvalue(x, "Meetme")
			usernum = getvalue(x, "Usernum")
			log_debug("AMI:MeetmeJoin %s : %s %s %s"
				  %(plist[astnum].astid, channel, meetme, usernum))
		elif x.find("MeetmeLeave;") == 7:
			channel = getvalue(x, "Channel")
			meetme = getvalue(x, "Meetme")
			usernum = getvalue(x, "Usernum")
			log_debug("AMI:MeetmeLeave %s : %s %s %s"
				  %(plist[astnum].astid, channel, meetme, usernum))
		elif x.find("ExtensionStatus;") == 7:
			exten   = getvalue(x, "Exten")
			context = getvalue(x, "Context")
			status  = getvalue(x, "Status")
			log_debug("AMI:ExtensionStatus: %s : %s %s %s"
				  %(plist[astnum].astid, exten, context, status))
			# QueueMemberStatus ExtensionStatus
			#                 0                  AST_DEVICE_UNKNOWN
			#                 1               0  AST_DEVICE_NOT_INUSE  /  libre
			#                 2               1  AST_DEVICE IN USE     / en ligne
			#                 3                  AST_DEVICE_BUSY
			#                                 4  AST_EXTENSION_UNAVAILABLE ?
			#                 5                  AST_DEVICE_UNAVAILABLE
			#                 6 AST_EXTENSION_RINGING = 8  appele
		elif x.find("OriginateSuccess;") == 7: pass
		elif x.find("OriginateFailure;") == 7:
			log_debug("AMI:OriginateFailure: " + plist[astnum].astid + \
				  " - reason=" + getvalue(x, "Reason"))
			#define AST_CONTROL_HANGUP              1
			#define AST_CONTROL_RING                2
			#define AST_CONTROL_RINGING             3
			#define AST_CONTROL_ANSWER              4
			#define AST_CONTROL_BUSY                5
			#define AST_CONTROL_TAKEOFFHOOK         6
			#define AST_CONTROL_OFFHOOK             7
			#define AST_CONTROL_CONGESTION          8
			#define AST_CONTROL_FLASH               9
			#define AST_CONTROL_WINK                10
		elif x.find("Rename;") == 7:
			# appears when there is a transfer
			channel_old = getvalue(x, "Oldname")
			channel_new = getvalue(x, "Newname")
			if channel_old.find("<MASQ>") < 0 and channel_new.find("<MASQ>") < 0 and \
			       is_normal_channel(channel_old) and is_normal_channel(channel_new):
				log_debug("AMI:Rename:N: %s : old=%s new=%s"
					  %(plist[astnum].astid, channel_old, channel_new))
				phone_old = channel_old.split("-")[0]
				phone_new = channel_new.split("-")[0]

				channel_p1 = plist[astnum].normal[phone_old].chann[channel_old].getChannelPeer()
				channel_p2 = plist[astnum].normal[phone_new].chann[channel_new].getChannelPeer()
				phone_p1 = channel_p1.split("-")[0]

				if channel_p2 == "": # occurs when 72 (interception) is called
					# A is calling B, intercepted by C
					# in this case old = B and new = C
					if phone_new in listkeys:
						plist[astnum].normal[phone_new].chann[channel_new].setChannelPeer(channel_p1)
						update_GUI_clients(astnum, phone_new, "ami-er1")
					else:
						pass
					if phone_p1 in listkeys:
						plist[astnum].normal[phone_p1].chann[channel_p1].setChannelPeer(channel_new)
						update_GUI_clients(astnum, phone_p1, "ami-er2")
					else:
						pass
				else:
					# A -> B  then B' transfers to C
					# in this case old = B' and new = A
					# => channel_p1 = peer(old) = C
					# => channel_p2 = peer(new) = B

					phone_p2 = channel_p2.split("-")[0]
					# the new peer of A is C / the new peer of C is A
					if phone_new in listkeys and phone_old in listkeys:
						plist[astnum].normal[phone_new].chann[channel_new].setChannelPeer(channel_p1)
						plist[astnum].normal[phone_new].chann[channel_new].setChannelNum(plist[astnum].normal[phone_old].chann[channel_old].getChannelNum())
						update_GUI_clients(astnum, phone_new, "ami-er3")
					if phone_p1 in listkeys and phone_p2 in listkeys:
						plist[astnum].normal[phone_p1].chann[channel_p1].setChannelPeer(channel_new)
						plist[astnum].normal[phone_p1].chann[channel_p1].setChannelNum(plist[astnum].normal[phone_p2].chann[channel_p2].getChannelNum())
						update_GUI_clients(astnum, phone_p1, "ami-er4")
			else:
				log_debug("AMI:Rename:A: %s : old=%s new=%s"
					  %(plist[astnum].astid, channel_old, channel_new))
		elif x.find("Newstate;") == 7:
			chan    = getvalue(x, "Channel")
			clid    = getvalue(x, "CallerID")
			state   = getvalue(x, "State")
			phonenum = chan.split("-")[0]
			if phonenum in listkeys:
				plist[astnum].normal[phonenum].set_chan(chan, state, 0, "", "", "")
				update_GUI_clients(astnum, phonenum, "ami-ns0")
			else:
				pass
		elif x.find("Newcallerid;") == 7:
			# for tricky queues' management
			chan    = getvalue(x, "Channel")
			clid    = getvalue(x, "CallerID")
			clidn   = getvalue(x, "CallerIDName")
			log_debug("AMI:Newcallerid: " + plist[astnum].astid + \
				  " channel=" + chan + " callerid=" + clid + " calleridname=" + clidn)
			if chan.find("Local/") == 0:
				plist[astnum].locals[chan] = LocalChannel("Init", clid)
			else:
				pass
		elif x.find("Newchannel;") == 7:
			chan    = getvalue(x, "Channel")
			clid    = getvalue(x, "CallerID")
			phonenum = chan.split("-")[0]
			if phonenum in listkeys:
				plist[astnum].normal[phonenum].set_chan(chan, "", 0, "", "", "")
				# update_GUI_clients(astnum, phonenum, "ami-nc0")
			else:
				pass
			if not (clid == "" or (clid == "<unknown>" and is_normal_channel(chan))):
				for k in tcpopens_sb:
					k[0].send("asterisk=<" + clid + "> is entering the Asterisk <" + plist[astnum].astid + "> through " + chan + "\n")
			else:
				pass
		elif x.find("Newexten;") == 7: # in order to handle outgoing calls ?
			chan    = getvalue(x, "Channel")
			exten   = getvalue(x, "Extension")
			if exten != "s" and exten != "h" and exten != "t":
				#print "--- exten :", chan, exten
				if is_normal_channel(chan):
					phonenum = chan.split("-")[0]
					if phonenum in listkeys:
						plist[astnum].normal[phonenum].set_chan(chan, "Calling", 0, dir_to_string, "", exten)
						update_GUI_clients(astnum, phonenum, "ami-en0")
					else:
						log_debug("AMI:Newexten: " + plist[astnum].astid + " warning : " + phonenum + " does not belong to our phone list")
				else:
					pass
			else:
				pass
		elif x.find("MessageWaiting;") == 7:
			mwi_string = getvalue(x,"Mailbox") + " waiting=" + getvalue(x,"Waiting") \
				     + "; new=" + getvalue(x, "New") + "; old=" + getvalue(x, "Old")
			log_debug("AMI:MessageWaiting: " + plist[astnum].astid + " : " + mwi_string)
		elif x.find("QueueParams;") == 7:
			log_debug("//AMI:QueueParams// %s : %s" %(plist[astnum].astid, x))
		elif x.find("QueueMember;") == 7:
			log_debug("//AMI:QueueMember// %s : %s" %(plist[astnum].astid, x))
		elif x.find("QueueMemberStatus;") == 7:
			queuenameq = getvalue(x, "Queue")
			location   = getvalue(x, "Location")
			status     = getvalue(x, "Status")
			log_debug("AMI:QueueMemberStatus: " + plist[astnum].astid + " " + queuenameq + " " + location + " " + status)
		elif x.find("Leave;") == 7:
			queuenameq = getvalue(x, "Queue")
			log_debug("AMI:Leave: " + plist[astnum].astid + " " + queuenameq)
		else:
			if len(x) > 0:
				log_debug("AMI:XXX: " + plist[astnum].astid + " <" + x + ">")


## \brief Handling of AMI events for the initial Status Command.
# These are AMI events received as a reply to a command.
# \param astnum the asterisk numerical identifier
# \param idata the data read from the AMI we want to parse
# \return
def handle_ami_status(astnum, idata):
	global plist, save_for_next_packet_status
	listkeys = plist[astnum].normal.keys()

	full_idata = save_for_next_packet_status[astnum] + idata
	evlist = full_idata.split("\r\n\r\n")
	save_for_next_packet_status[astnum] = evlist.pop()

	for z in evlist:
		# we assume no ";" character is present in AMI events fields
		x = z.replace("\r\n", ";")
		#if len(x) > 0:
		#print "statuses --FULL--", x
		if x.find("Status;") == 7:
			if x.find(";State: Up;") >= 0:
				if x.find(";Seconds: ") >= 0:
					chan    = getvalue(x, "Channel")
					clid    = getvalue(x, "CallerID")
					exten   = getvalue(x, "Extension")
					seconds = getvalue(x, "Seconds")
					if x.find(";Link: ") >= 0:
						link = getvalue(x, "Link")
						#print "statuses up --------", chan, clid, exten, seconds, link
						if is_normal_channel(link):
							phonenum = link.split("-")[0]
							if phonenum in listkeys:
								plist[astnum].normal[phonenum].set_chan(link, "On the phone", int(seconds), dir_from_string, chan, clid)
								update_GUI_clients(astnum, phonenum, "ami-st1")
							else:
								pass
						else:
							pass
						if is_normal_channel(chan):
							phonenum = chan.split("-")[0]
							if phonenum in listkeys:
								plist[astnum].normal[phonenum].set_chan(chan, "On the phone", int(seconds), dir_to_string, link, exten)
								update_GUI_clients(astnum, phonenum, "ami-st2")
							else:
								pass
						else:
							pass
					else:
						log_debug("AMI::Status UP: " + chan + " " + clid + " " + exten + " " + seconds)
				else:
					pass
			elif x.find(";State: Ring;") >= 0:
				log_debug("AMI::Status TO: " + getvalue(x, "Channel") + \
					  " " + getvalue(x, "Extension") + " " + getvalue(x, "Seconds"))
			elif x.find(";State: Ringing;") >= 0:
				log_debug("AMI::Status FROM: " + getvalue(x, "Channel"))
			else:
				log_debug("AMI::Status : " + x)
		elif x.find("Response: Follows;Privilege: Command;") == 0:
			for y in x.split("Response: Follows;Privilege: Command;")[1].split("\n"):
				log_debug("AMI:Response: " + plist[astnum].astid + " : " + y)
		else:
			log_debug("AMI:_status_: " + plist[astnum].astid + " : " + x)


## \brief Sends a SIP register + n x SIP subscribe messages.
# \param astnum the asterisk numerical identifier
# \param l_sipsock the SIP socket where to reply
# \return
def do_sip_register_subscribe(astnum, l_sipsock):
	global tcpopens_sb, plist, configs
	rdc = chr(65 + 32 * random.randrange(2) + random.randrange(26))
	command = xivo_sip.sip_register(configs[n], "sip:" + configs[n].mysipname, 1, "reg_cid@xivopy", expires)
	l_sipsock.sendto(command, (configs[n].remoteaddr, configs[n].portsipsrv))
	#command = xivo_sip.sip_options(configs[n], "sip:" + configs[n].mysipname, "testoptions@xivopy", "107")
	#l_sipsock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
	#l_sipsock.sendto(command, ("192.168.0.255", 5060))

	for sipnum in plist[astnum].normal.keys():
		if sipnum.find("SIP/") == 0:
			dtnow = time.time() - plist[astnum].normal[sipnum].lasttime
			if dtnow > (2 * timeout_between_registers):
				if plist[astnum].normal[sipnum].sipstatus != "Timeout":
					plist[astnum].normal[sipnum].set_sipstatus("Timeout")
					update_GUI_clients(astnum, sipnum, "sip___3")
			cid = rdc + "subscribexivo_" + sipnum.split("/")[1] + "@" + configs[n].localaddr
			command = xivo_sip.sip_subscribe(configs[n], "sip:" + configs[n].mysipname, 1,
							 cid,
							 sipnum.split("/")[1], expires)
			l_sipsock.sendto(command, (configs[n].remoteaddr, configs[n].portsipsrv))
			#command = xivo_sip.sip_options(configs[n], "sip:" + configs[n].mysipname,
			#cid, sipnum)
			#l_sipsock.sendto(command, (configs[n].remoteaddr, configs[n].portsipsrv))


## \brief Connects to the AMI if not yet.
# \param astnum Asterisk id to (re)connect
# \return none
def update_amisocks(astnum):
	if AMIcomms[astnum] == -1:
		log_debug(plist[astnum].astid + " : AMI (events = off) : attempting to connect")
		als0 = xivo_ami.ami_socket_login(configs[astnum].remoteaddr,
						 configs[astnum].ami_port,
						 configs[astnum].ami_login,
						 configs[astnum].ami_pass, False)
		AMIcomms[astnum] = als0
		if AMIcomms[astnum] != -1:
			ins.append(als0)
			log_debug(configs[astnum].astid + " : AMI (events = off) : connected")
			"""Clears the channels before requesting a new status"""
			for x in plist[astnum].normal.keys():
				plist[astnum].normal[x].clear_channels()
			ret = xivo_ami.ami_socket_status(AMIcomms[astnum])
			if not ret:
				log_debug(configs[astnum].astid + " : could not send status command")
		else:
			log_debug(configs[astnum].astid + " : AMI (events = off) : could NOT connect")

	if AMIsocks[astnum] == -1:
		log_debug(plist[astnum].astid + " : AMI (events = on)  : attempting to connect")
		als1 = xivo_ami.ami_socket_login(configs[astnum].remoteaddr,
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
			ret = xivo_ami.ami_socket_status(AMIcomms[astnum])
			#xivo_ami.ami_socket_command(AMIcomms[astnum], "show channeltypes")
			#xivo_ami.ami_socket_command(AMIcomms[astnum], "show uptime")
			#xivo_ami.ami_socket_command(AMIcomms[astnum], "show version")
			#xivo_ami.ami_socket_command(AMIcomms[astnum], "meetme")
			if not ret:
				log_debug(configs[astnum].astid + " : could not send status command")
		else:
			log_debug(configs[astnum].astid + " : AMI (events = on)  : could NOT connect")


## \brief Updates the list of sip numbers according to the SSO then sends old and new peers to the UIs.
# The reconnection to the AMI is also done here when it has been broken.
# If the AMI sockets are dead, a reconnection is also attempted here.
# \param astnum the asterisk numerical identifier
# \return none
# \sa update_userlist_fromurl
def update_sipnumlist(astnum):
	global plist, configs

	userlist_lock.acquire()
	for user in userlist[astnum].keys():
		if "sessiontimestamp" in userlist[astnum][user].keys():
			if time.time() - userlist[astnum][user]["sessiontimestamp"] > session_expiration_time:
				del userlist[astnum][user]["sessionid"]
				del userlist[astnum][user]["sessiontimestamp"]
				del userlist[astnum][user]["ip"]
				del userlist[astnum][user]["port"]
				userlist[astnum][user]["state"] = "unknown"
				sipnumber = "SIP/" + user.split("sip")[1]
				if sipnumber in plist[astnum].normal:
					plist[astnum].normal[sipnumber].set_imstat("unknown")
					plist[astnum].normal[sipnumber].update_time()
					update_GUI_clients(astnum, sipnumber, "kfc-dsc")
				log_debug(plist[astnum].astid + " : timeout reached for " + sipnumber)
	userlist_lock.release()

	if len(plist[astnum].others) > 0:
		log_debug("WARNING : unmonitored list : " + str(plist[astnum].others))

	sipnumlistold = plist[astnum].normal.keys()
	sipnumlistold.sort()
	sipnuml = update_userlist_fromurl(astnum, configs[astnum].userlisturl, configs[astnum].mysipname)
	update_customers_fromurl(astnum, "file:/home/corentin/customers.txt")
	for x in configs[astnum].extrachannels.split(","):
		if x != "": sipnuml[x] = [x, "", ""]
	sipnumlistnew = sipnuml.keys()
	sipnumlistnew.sort()
	if sipnumlistnew != sipnumlistold:
		lstdel = ""
		lstadd = ""
		for snl in sipnumlistold:
			if snl not in sipnumlistnew:
				del plist[astnum].normal[snl] # or = "Absent"/0 ?
				lstdel += "del:" + configs[astnum].astid + ":SIP:" + snl + ";"
		for snl in sipnumlistnew:
			if snl not in sipnumlistold:
				if snl.find("SIP") == 0:
					plist[astnum].normal[snl] = LineProp("SIP", snl.split("/")[1], "BefSubs")
				elif snl.find("IAX2") == 0:
					plist[astnum].normal[snl] = LineProp("IAX2", snl.split("/")[1], "Ready")
				elif snl.find("mISDN") == 0:
					plist[astnum].normal[snl] = LineProp("mISDN", snl.split("/")[1], "Ready")
				elif snl.find("Zap") == 0:
					plist[astnum].normal[snl] = LineProp("Zap", snl.split("/")[1], "Ready")
				else:
					log_debug(snl + " format not supported")

				if snl in plist[astnum].normal:
					plist[astnum].normal[snl].set_callerid(sipnuml[snl])

				lstadd += "add:" + configs[astnum].astid + ":" + \
					  plist[astnum].normal[snl].tech + ":" + snl.split("/")[1] + ":unknown:unknown:0;"
		for k in tcpopens_sb:
			if lstdel != "": k[0].send("peerremove=" + lstdel + "\n")
			if lstadd != "": k[0].send("peeradd=" + lstadd + "\n")


## \brief Connects to the AMI through AMIClass.
# \param address IP address
# \param loginname loginname
# \param password password
# \return the socket
def connect_to_AMI(address, loginname, password):
	lAMIsock = AMIClass(address, loginname, password)
	try:
		lAMIsock.connect()
		lAMIsock.login()
	except socket.timeout: pass
	except socket:         pass
	except:
		del lAMIsock
		lAMIsock = False
	return lAMIsock


## \class LocalChannel
# \brief Properties of a temporary "Local" channel.
class LocalChannel:
	# \brief Class initialization.
	def __init__(self, istate, icallerid):
		self.state = istate
		self.callerid = icallerid
		self.peer = ""
	# \brief Sets the state and the peer channel name.
	def set_chan(self, istate, ipeer):
		self.state = istate
		if ipeer != "":
			self.peer = ipeer
	def set_callerid(self, icallerid):
		self.callerid = icallerid


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

## \class ChannelStatus
# \brief Properties of a Channel, as given by the AMI.
class ChannelStatus:
	## \var status
	# \brief Channel status

	## \var deltatime
	# \brief Elapsed time spent by the channel with the current status

	## \var time
	# \brief Absolute time

	## \var direction
	# \brief "To" or "From"

	## \var channel_peer
	# \brief Channel name of the peer with whom it is in relation

	## \var channel_num
	# \brief Phone number of the peer with whom it is in relation

	##  \brief Class initialization.
	def __init__(self, istatus, dtime, idir, ipeerch, ipeernum, itime):
		self.status = istatus
		self.deltatime = dtime
		self.time = itime
		self.direction = idir
		self.channel_peer = ipeerch
		self.channel_num = ipeernum
	def updateDeltaTime(self, dtime):
		self.deltatime = dtime
	def setChannelPeer(self, ipeerch):
		self.channel_peer = ipeerch
	def setChannelNum(self, ipeernum):
		self.channel_num = ipeernum
	def getChannelPeer(self):
		return self.channel_peer
	def getChannelNum(self):
		return self.channel_num
	def getDirection(self):
		return self.direction
	def getTime(self):
		return self.time
	def getDeltaTime(self):
		return self.deltatime
	def getStatus(self):
		return self.status


## \class LineProp
# \brief Properties of a phone line. It might contain many channels.
class LineProp:
	## \var tech
	# \brief Protocol of the phone (SIP, IAX2, ...)
	
	## \var lasttime
	# \brief Last time the phone has received a reply from a SUBSCRIBE
	
	## \var chann
	# \brief List of Channels, with their properties as ChannelStatus
	
	## \var sipstatus
	# \brief Status given through SIP presence detection
	
	## \var imstat
	# \brief Instant Messaging status, as given by Xivo Clients
	
	## \var voicemail
	# \brief Voicemail status (only the member name for now)
	
	## \var phonenum
	# \brief Phone number
	
	## \var queueavail
	# \brief Queue availability (only the member name for now)
	
	## \var callerid
	# \brief Caller ID
	
	##  \brief Class initialization.
	def __init__(self, itech, iphonenum, isipstatus):
		self.tech = itech
		self.phonenum = iphonenum
		self.lasttime = 0
		self.chann = {}
		self.sipstatus = isipstatus # Asterisk "hints" status
		self.imstat = "unknown"  # XMPP / Instant Messaging status
		self.voicemail = ""  # Voicemail status
		self.queueavail = "" # Availability as a queue member
		self.calleridfull = "nobody"
		self.calleridfirst = "nobody"
		self.calleridlast = "nobody"
	def set_tech(self, itech):
		self.tech = itech
	def set_phonenum(self, iphonenum):
		self.tech = iphonenum
	def set_sipstatus(self, isipstatus):
		self.sipstatus = isipstatus
	def set_imstat(self, istatus):
		self.imstat = istatus
	def set_lasttime(self, ilasttime):
		self.lasttime = ilasttime
	def set_callerid(self, icallerid):
		self.calleridfull  = icallerid[0]
		self.calleridfirst = icallerid[1]
		self.calleridlast  = icallerid[2]
	##  \brief Updates the time elapsed on a channel according to current time.
	def update_time(self):
		nowtime = time.time()
		for ic in self.chann:
			dtime = int(nowtime - self.chann[ic].getTime())
			self.chann[ic].updateDeltaTime(dtime)
	##  \brief Removes all channels.
	def clear_channels(self):
		self.chann = {}
	##  \brief Adds a channel or changes its properties.
	# If the values of status, itime, peerch and/or peernum are empty,
	# they are not updated : the previous value is kept.
	# \param ichan the Channel to hangup.
	# \param status the status to set
	# \param itime the elapsed time to set
	def set_chan(self, ichan, status, itime, idir, peerch, peernum):
		# does not update peerch and peernum if the new values are empty
		newstatus = status
		newdir = idir
		newpeerch = peerch
		newpeernum = peernum
		if ichan in self.chann:
			if status  == "": newstatus = self.chann[ichan].getStatus()
			if idir    == "": newdir = self.chann[ichan].getDirection()
			if peerch  == "": newpeerch = self.chann[ichan].getChannelPeer()
			if peernum == "": newpeernum = self.chann[ichan].getChannelNum()
		firsttime = time.time()
		self.chann[ichan] = ChannelStatus(newstatus, itime, newdir,
						  newpeerch, newpeernum, firsttime - itime)
		for ic in self.chann:
			self.chann[ic].updateDeltaTime(int(firsttime - self.chann[ic].getTime()))

	##  \brief Hangs up a Channel.
	# \param ichan the Channel to hangup.
	def set_chan_hangup(self, ichan):
		nichan = ichan
		if ichan.find("<ZOMBIE>") >= 0:
		        log_debug("sch channel contains a <ZOMBIE> part : " + ichan + " : sending hup to " + nichan + "anyway")
			nichan = ichan.split("<ZOMBIE>")[0]
		firsttime = time.time()
		self.chann[nichan] = ChannelStatus("Hangup", 0, "", "", "", firsttime)
		for ic in self.chann:
			self.chann[ic].updateDeltaTime(int(firsttime - self.chann[ic].getTime()))

	##  \brief Removes a Channel.
	# \param ichan the Channel to remove.
	def del_chan(self, ichan):
		nichan = ichan
		if ichan.find("<ZOMBIE>") >= 0:
		        log_debug("dch channel contains a <ZOMBIE> part : " + ichan + " : deleting " + nichan + "anyway")
			nichan = ichan.split("<ZOMBIE>")[0]
		if nichan in self.chann: del self.chann[nichan]

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

	## \var mysipname
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
		     ami_login = "sylvain",
		     ami_pass = "sylvain",
		     portsipclt = 5080,
		     portsipsrv = 5060,
		     mysipname = "xivoaccount"):

		self.astid = astid
		self.userlisturl = userlisturl
		self.extrachannels = extrachannels
		self.localaddr = localaddr
		self.remoteaddr = remoteaddr
		self.ipaddress_php = ipaddress_php
		self.portsipclt = portsipclt
		self.portsipsrv = portsipsrv
		self.mysipname = mysipname
		self.ami_port = ami_port
		self.ami_login = ami_login
		self.ami_pass = ami_pass

## \brief Adds (or updates) a user in the userlist.
# \param user the user to add
# \param passwd the user's passwd
# \return none
def adduser(astn, user, passwd):
	global userlist
	if userlist[astn].has_key(user):
		userlist[astn][user]['passwd'] = passwd
	else:
		userlist[astn][user] = {'user':user, 'passwd':passwd}

## \brief Deletes a user from the userlist.
# \param user the user to delete
# \return none
def deluser(astn, user):
	global userlist
	if userlist[astn].has_key(user):
		userlist[astn].pop(user)

## \brief Returns the user from the list.
# \param user searched for
# \return user found, otherwise None
def finduser(astn, user):
	u = userlist[astn].get(user)
	return u


## \class LoginHandler
# \brief The clients connect to this in order to obtain a valid session id.
# This could be enhanced to support a more complete protocol
# supporting commands coming from the client in order to pilot asterisk.
class LoginHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		#print '  request :', self.request
		log_debug("LoginHandler : client connected = " + str(self.client_address))
		list0 = self.rfile.readline()    # list0 should be "LOGIN <asteriskname>/sip<nnn>"
		list1 = list0.strip().split(' ') # list1 should be "[LOGIN <asteriskname>/sip<nnn>]"
		if len(list1) != 2 or list1[0] != 'LOGIN':
			self.wfile.write('ERROR : wrong number of arguments\r\n')
			return
		if list1[1].find("/") >= 0:
			astname_xivoc = list1[1].split("/")[0]
			user = list1[1].split("/")[1]
		else:
			self.wfile.write('ERROR : wrong ID format\r\n')
			return


		self.wfile.write('Send PASS for authentification\r\n')
		list1 = self.rfile.readline().strip().split(' ')
		if len(list1) != 2 or list1[0] != 'PASS':
			self.wfile.write('ERROR : wrong format for PASS reply\r\n')
			return
		passwd = list1[1]
		#print 'user/pass : ' + user + '/' + passwd

		userlist_lock.acquire()
		astnum = asteriskr[astname_xivoc]
		e = finduser(astnum, user)
		goodpass = (e != None) and (e.get('passwd') == passwd)
		userlist_lock.release()
		if not goodpass:
			self.wfile.write('ERROR : WRONG LOGIN PASSWD\r\n')
			return

		self.wfile.write('Send PORT command\r\n')
		list1 = self.rfile.readline().strip().split(' ')
		if len(list1) != 2 or list1[0] != 'PORT':
			self.wfile.write('ERROR PORT\r\n')
			return
		port = list1[1]

		self.wfile.write('Send STATE command\r\n')
		list1 = self.rfile.readline().strip().split(' ')
		if len(list1) != 2 or list1[0] != 'STATE':
			self.wfile.write('ERROR STATE\r\n')
			return
		state = list1[1]

		# TODO : random pas au top, faire generation de session id plus luxe
		sessionid = '%u' % random.randint(0,999999999)
		userlist_lock.acquire()
		e['sessionid'] = sessionid
		e['sessiontimestamp'] = time.time()
		e['ip'] = self.client_address[0]
		e['port'] = port
		if state in allowed_states:
			e['state'] = state
		else:
			e['state'] = "undefinedstate"
		userlist_lock.release()
		retline = 'OK SESSIONID ' + sessionid + '\r\n'
		self.wfile.write(retline)

		if astnum >= 0:
			sipnumber = "SIP/" + user.split("sip")[1]
			plist[astnum].normal[sipnumber].set_imstat(state)
			plist[astnum].normal[sipnumber].update_time()
			update_GUI_clients(astnum, sipnumber, "kfc-lin")

		#print userlist


## \class IdentRequestHandler
# \brief Gives client identification to the profile pusher.
# The connection is kept alive so several requests can be made on the same open TCP connection.
class IdentRequestHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		list0 = self.rfile.readline().strip().split(' ')
		log_debug("IdentRequestHandler : client = " + str(self.client_address) + " / " + str(list0))
		retline = 'ERROR\r\n'
		if list0[0] == 'QUERY' and len(list0) == 2:
			user = list0[1]
			userlist_lock.acquire()
			try:
				astnum = ip_reverse_sht[self.client_address[0]]
				e = finduser(astnum, user)
				if e == None:
					retline = 'ERROR USER <' + user + '> NOT FOUND\r\n'
				else:
					if e.has_key('ip') and e.has_key('port') \
					       and e.has_key('state') and e.has_key('sessionid') \
					       and e.has_key('sessiontimestamp'):
						if time.time() - e.get('sessiontimestamp') > session_expiration_time:
							retline = 'ERROR USER SESSION EXPIRED for <' + user + '>\r\n'
						else:
							retline = 'USER ' + user
							retline += ' SESSIONID ' + e.get('sessionid')
							retline += ' IP ' + e.get('ip')
							retline += ' PORT ' + e.get('port')
							retline += ' STATE ' + e.get('state')
							retline += '\r\n'
					else:
						retline = 'ERROR USER SESSION NOT DEFINED for <' + user + '>\r\n'
			except Exception, e:
				retline = 'ERROR (exception) : ' + str(e) + '\r\n'
			userlist_lock.release()
		try:
			self.wfile.write(retline)
		except Exception, e:
			# something bad happened.
			log_debug("IdentRequestHandler/Exception: " + str(e))
			return


## \class KeepAliveHandler
# \brief It receives UDP datagrams and sends back a datagram containing whether
# "OK" or "ERROR <error-text>".
# It could be a good thing to give a numerical code to each error.
class KeepAliveHandler(SocketServer.DatagramRequestHandler):
	def handle(self):
		log_debug("KeepAliveHandler : client = " + str(self.client_address))
		astname_xivoc = ""
		userlist_lock.acquire()
		try:
			ip = self.client_address[0]
			list = self.request[0].strip().split(' ')
			timestamp = time.time()
			# ALIVE user SESSIONID sessionid
			if len(list) == 2 and list[0] == 'STOP':
				response = 'DISC\r\n'
				astname_xivoc = list[1].split("/")[0]
				astnum = asteriskr[astname_xivoc]
				user = list[1].split("/")[1]

				userlist_lock.acquire()
				if "sessiontimestamp" in userlist[astnum][user].keys():
					del userlist[astnum][user]["sessionid"]
					del userlist[astnum][user]["sessiontimestamp"]
					del userlist[astnum][user]["ip"]
					del userlist[astnum][user]["port"]
					userlist[astnum][user]["state"] = "unknown"
					sipnumber = "SIP/" + user.split("sip")[1]
					if sipnumber in plist[astnum].normal:
						plist[astnum].normal[sipnumber].set_imstat("unkown")
						plist[astnum].normal[sipnumber].update_time()
						update_GUI_clients(astnum, sipnumber, "kfc-dcc")
				userlist_lock.release()
			elif len(list) == 3 and list[0] == 'MESSAGE':
				response = 'SENT\r\n'
				#astname_xivoc = list[1].split("/")[0]
				#astnum = asteriskr[astname_xivoc]
				#user = list[1].split("/")[1]
				for k in tcpopens_sb:
					k[0].send("asterisk=<%s> said : <%s>\n" %(list[1], list[2]))
			elif len(list) < 4 or list[0] != 'ALIVE' or list[2] != 'SESSIONID':
				response = 'ERROR unknown\r\n'
			else:
				astname_xivoc = list[1].split("/")[0]
				astnum = asteriskr[astname_xivoc]
				user = list[1].split("/")[1]
				sessionid = list[3]
				state = "undefinedstate"
				if len(list) >= 6:
					state = list[5]
				e = finduser(astnum, user)
				if e == None:
					response = 'ERROR user unknown\r\n'
				else:
					#print user, e['user']
					#print sessionid, e['sessionid']
					#print ip, e['ip']
					#print timestamp, e['sessiontimestamp']
					#print timestamp - e['sessiontimestamp']
					if sessionid == e['sessionid'] and ip == e['ip'] and \
					       e['sessiontimestamp'] + session_expiration_time > timestamp:
						if state in allowed_states:
							e['state'] = state
						else:
							e['state'] = "undefinedstate"
						e['sessiontimestamp'] = timestamp
						response = 'OK\r\n'
					else:
						response = 'ERROR SESSION EXPIRED OR INVALID\r\n'
		except Exception, e:
			response = 'ERROR (exception) : ' + str(e) + '\r\n'
		userlist_lock.release()
		self.request[1].sendto(response, self.client_address)
		if response == 'OK\r\n':
			astnum = asteriskr[astname_xivoc]
			if astnum >= 0:
				sipnumber = "SIP/" + user.split("sip")[1]
				if sipnumber in plist[astnum].normal:
					plist[astnum].normal[sipnumber].set_imstat(state)
					plist[astnum].normal[sipnumber].update_time()
					update_GUI_clients(astnum, sipnumber, "kfc-kah")


## \class MyTCPServer
# \brief TCPServer with the reuse address on.
class MyTCPServer(SocketServer.ThreadingTCPServer):
	allow_reuse_address = True

# ==============================================================================
# ==============================================================================


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
except Exception, e:
	print e

xivoconffile = "/etc/asterisk/xivo_daemon.conf"

opts, args = getopt.getopt(sys.argv[1:], "dc:", ["daemon", "config="])
for opt, arg in opts:
        if opt == "-c":
		xivoconffile = arg

xivoconf = ConfigParser.ConfigParser()
xivoconf.readfp(open(xivoconffile))

port_login = 5000
port_keepalive = 5001
port_request = 5002
port_ui_srv = 5003
port_phpui_srv = 5004
port_switchboard_base_sip = 5005
session_expiration_time = 60
log_filename = "/var/log/xivo_daemon.log"
xivoconf_general = xivoconf.items("general")

if "port_fiche_login" in xivoconf_general:
	port_login = int(xivoconf_general["port_fiche_login"])
if "port_fiche_keepalive" in xivoconf_general:
	port_keepalive = int(xivoconf_general["port_fiche_keepalive"])
if "port_fiche_agi" in xivoconf_general:
	port_request = int(xivoconf_general["port_fiche_agi"])
if "port_switchboard" in xivoconf_general:
	port_ui_srv = int(xivoconf_general["port_switchboard"])
if "port_php" in xivoconf_general:
	port_phpui_srv = int(xivoconf_general["port_php"])
if "port_switchboard_base_sip" in xivoconf_general:
	port_switchboard_base_sip = int(xivoconf_general["port_switchboard_base_sip"])
if "expiration_session" in xivoconf_general:
	session_expiration_time = int(xivoconf_general["expiration_session"])
if "logfile" in xivoconf_general:
	log_filename = xivoconf_general["logfile"]

with_ami = True
with_sip = True
if "noami" in xivoconf_general: with_ami = False
if "nosip" in xivoconf_general: with_sip = False

configs = []
save_for_next_packet_events = []
save_for_next_packet_status = []
n = 0
ip_reverse_php = {}
ip_reverse_sht = {}

for i in xivoconf.sections():
	if i != "general":
		configs.append(AsteriskRemote(i,
					      xivoconf.get(i, "userlisturl"),
					      xivoconf.get(i, "extrachannels"),
					      xivoconf.get(i, "localaddr"),
					      xivoconf.get(i, "ipaddress"),
					      xivoconf.get(i, "ipaddress_php"),
					      int(xivoconf.get(i, "ami_port")),
					      xivoconf.get(i, "ami_login"),
					      xivoconf.get(i, "ami_pass"),
					      port_switchboard_base_sip + n,
					      int(xivoconf.get(i, "sip_port")),
					      xivoconf.get(i, "sip_presence_account")))
		ip_reverse_sht[xivoconf.get(i, "ipaddress")] = n
		ip_reverse_php[xivoconf.get(i, "ipaddress_php")] = n
		save_for_next_packet_events.append("")
		save_for_next_packet_status.append("")
		n += 1

# Instantiate the SocketServer Objects.
loginserver = MyTCPServer(('', port_login), LoginHandler)
# TODO: maybe we should listen on only one interface (localhost ?)
requestserver = MyTCPServer(('', port_request), IdentRequestHandler)
# Do we need a Threading server for the keep alive ? I dont think so,
# packets processing is non blocking so thead creation/start/stop/delete
# overhead is not worth it.
#keepaliveserver = SocketServer.ThreadingUDPServer(('', port_keepalive), KeepAliveHandler)
keepaliveserver = SocketServer.UDPServer(('', port_keepalive), KeepAliveHandler)


# opens the logfile for output in append mode
try:
	logfile = open(log_filename, 'a')
except Exception, e:
	print "Could not open", log_filename, "in append mode : ", e
	logfile = False

# user list initialized empty
userlist = []
userlist_lock = threading.Condition()

plist = []
SIPsocks = []
AMIsocks = []
AMIcomms = []
AMIclasssock = []
asteriskr = {}
customerbase = {}

# We have three sockets to listen to so we cannot use the
# very easy to use SocketServer.serve_forever()
# So select() is what we need. The SocketServer.handle_request() calls
# won't block the execution. In case of the TCP servers, they will
# spawn a new thread, in case of the UDP server, the request handling
# process should be fast. If it isnt, use a threading UDP server ;)
ins = [loginserver.socket, requestserver.socket, keepaliveserver.socket]

items_asterisks = xrange(len(configs))

log_debug("# STARTING XIVO Daemon # (1/3) AMI socket connections")

for n in items_asterisks:
	plist.append(PhoneList(configs[n].astid))
	userlist.append({})
	asteriskr[configs[n].astid] = n
	AMIcomms.append(-1)
	AMIsocks.append(-1)

	SIPsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	SIPsock.bind(("", configs[n].portsipclt))
	SIPsocks.append(SIPsock)
	ins.append(SIPsock)

	AMIclasssock.append(connect_to_AMI((configs[n].remoteaddr,
					    configs[n].ami_port),
					   configs[n].ami_login,
					   configs[n].ami_pass))

log_debug("# STARTING XIVO Daemon # (2/3) listening UI sockets")

# opens the listening socket for UI connections
UIsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
UIsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
UIsock.bind(("", port_ui_srv))
UIsock.listen(10)
ins.append(UIsock)

# opens the listening socket for PHP/CLI connections
PHPUIsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
PHPUIsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
PHPUIsock.bind(("", port_phpui_srv))
PHPUIsock.listen(10)
ins.append(PHPUIsock)

tcpopens_sb = []
tcpopens_php = []
lastrequest_time = []

askedtoquit = False

log_debug("# STARTING XIVO Daemon # (3/3) fetch SSO, SIP register and subscribe")
for n in items_asterisks:
	update_sipnumlist(n)
	if with_ami: update_amisocks(n)
	if with_sip: do_sip_register_subscribe(n, SIPsocks[n])
	lastrequest_time.append(time.time())

## \brief Handler for catching signals (in the main thread)
# \param signum signal number
# \param frame frame
# \return none
def sighandler(signum, frame):
	global askedtoquit
	print "signal", signum, "received : quitting"
	askedtoquit = True

signal.signal(signal.SIGINT, sighandler)
signal.signal(signal.SIGTERM, sighandler)
signal.signal(signal.SIGHUP, sighandler)

# Receive messages
while not askedtoquit:
    [i, o, e] = select.select(ins, [], [], timeout_between_registers)
    if i:
        if loginserver.socket in i:
		loginserver.handle_request()
	elif requestserver.socket in i:
		requestserver.handle_request()
	elif keepaliveserver.socket in i:
		keepaliveserver.handle_request()
	# SIP incoming packets are catched here
	elif filter(lambda j: j in SIPsocks, i):
		res = filter(lambda j: j in SIPsocks, i)[0]
		for n in items_asterisks:
			if SIPsocks[n] is res: break
		[data, addrsip] = SIPsocks[n].recvfrom(bufsize_udp)
		is_an_options_packet = parseSIP(n, data, SIPsocks[n], addrsip)
		# if the packet is an OPTIONS one (sent for instance when * is restarted)
		if is_an_options_packet:
			log_debug(configs[n].astid + " : do_sip_register_subscribe (parse SIP)")
			update_sipnumlist(n)
			if with_ami: update_amisocks(n)
			if with_sip: do_sip_register_subscribe(n, SIPsocks[n])
			lastrequest_time[n] = time.time()
	# these AMI connections are used in order to manage AMI commands with incoming events
	elif filter(lambda j: j in AMIsocks, i):
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
		except Exception, e:
			pass
	# these AMI connections are used in order to manage AMI commands without events
	elif filter(lambda j: j in AMIcomms, i):
		res = filter(lambda j: j in AMIcomms, i)[0]
		for n in items_asterisks:
			if AMIcomms[n] is res: break
		try:
			a = AMIcomms[n].recv(bufsize_any)
			if len(a) == 0: # end of connection from server side : closing socket
				log_debug(configs[n].astid + " : AMI (events = off) : CLOSING")
				AMIcomms[n].close()
				ins.remove(AMIcomms[n])
				AMIcomms[n] = -1
			else:
				handle_ami_status(n, a)
		except Exception, e:
			pass
	# the new UI (SB) connections are catched here
        elif UIsock in i:
		[conn, UIsockparams] = UIsock.accept()
		log_debug("TCP (SB)  socket opened on   " + UIsockparams[0] + " " + str(UIsockparams[1]))
		# appending the opened socket to the ones watched
		ins.append(conn)
		conn.setblocking(0)
		tcpopens_sb.append([conn, UIsockparams[0], UIsockparams[1]])
	# the new UI (PHP) connections are catched here
        elif PHPUIsock in i:
		[conn, PHPUIsockparams] = PHPUIsock.accept()
		log_debug("TCP (PHP) socket opened on   " + PHPUIsockparams[0] + " " + str(PHPUIsockparams[1]))
		# appending the opened socket to the ones watched
		ins.append(conn)
		conn.setblocking(0)
		tcpopens_php.append([conn, PHPUIsockparams[0], PHPUIsockparams[1]])
	# open UI (SB) connections
        elif filter(lambda j: j[0] in i, tcpopens_sb):
		conn = filter(lambda j: j[0] in i, tcpopens_sb)[0]
		manage_tcp_connection(conn, True)
	# open UI (PHP) connections
        elif filter(lambda j: j[0] in i, tcpopens_php):
		conn = filter(lambda j: j[0] in i, tcpopens_php)[0]
		manage_tcp_connection(conn, False)
	else:
		log_debug("unknown socket " + str(i))

	for n in items_asterisks:
		if (time.time() - lastrequest_time[n]) > timeout_between_registers:
			lastrequest_time[n] = time.time()
			log_debug(configs[n].astid + " : do_sip_register_subscribe (computed timeout)")
			update_sipnumlist(n)
			if with_ami: update_amisocks(n)
			if with_sip: do_sip_register_subscribe(n, SIPsocks[n])
    else:
	    log_debug("do_sip_register_subscribe (select's timeout)")
	    for n in items_asterisks:
		    lastrequest_time[n] = time.time()
		    update_sipnumlist(n)
		    if with_ami: update_amisocks(n)
		    if with_sip: do_sip_register_subscribe(n, SIPsocks[n])


try:
	os.unlink(pidfile)
except Exception, e:
	print e

print 'end of the execution flow...'
sys.exit(0)

# Close files and sockets
logfile.close()

