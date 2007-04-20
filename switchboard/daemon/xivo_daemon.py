#!/usr/bin/python
# $Revision$
# $Date$
#
# Authors : Thomas Bernard, Corentin Le Gall
#           Proformatique
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
# The statuses of all the lines are stored in the multidimensional array/dict "phonelists" :
#
# phonelists[astn][phonenum].chann[channel] are objects of the class ChannelStatus.
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

import os, posix, select, socket, string, sys, time
import random
import ConfigParser
import SocketServer, telnetlib, urllib
from time import strftime
import threading
import signal
import xivo_sip, xivo_ami

# socket.setdefaulttimeout(0.2)

# configuration options :
session_expiration_time = 60*1

dir_to_string = ">"
dir_from_string = "<"
localchans = {}
notmonitoredsrc = {}
notmonitoreddst = {}

# global : userlist
# liste des champs :
#  user :             user name
#  passwd :           password
#  sessionid :        session id generated at connection
#  sessiontimestamp : last time when the client proved itself to be ALIVE :)
#  ip :               ip address of the client (current session)
#  port :             port here the client is listening.
#  state :            available, away, doesnotdisturb
#                     (??online, not-available, busy)
# The user identifier will likely be its phone number

# user list initialized empty
userlist = {}
userlist_lock = threading.Condition()

pidfile = '/tmp/xivo_daemon.pid'
bufsize_large = 8192
bufsize_udp = 2048
bufsize_any = 512

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
	logfile.write(strftime("%b %2d %H:%M:%S ", time.localtime()) + string + "\n")
	logfile.flush()
	return 0


## \brief Outputs a string to stdout in no-daemon mode
# and always logs it.
# \param string the string to display and log
# \return 0
# \sa varlog
def log_debug(string):
	if sys.argv.count('-d') > 0: print "#debug# " + string
        varlog(string)
	return 0


## \brief Function to load sso.php user file.
# SIP, Zap, mISDN and IAX2 are taken into account there.
# There would remain MGCP, CAPI, h323, ...
# \param url the url where lies the sso, it can be file:/ as well as http://
# \param sipaccount the name of the reserved sip account (typically xivosb)
# \return the new phone numbers list
# \sa update_sipnumlist
def updateuserlistfromurl(url, sipaccount):
	l_sipnumlist = {}
	try:
		f = urllib.urlopen(url)
	except:
		log_debug("unable to open URL : " + url)
		return l_sipnumlist
	try:
		for line in f:
			# remove leading/tailing whitespaces
			line = line.strip()
			l = line.split('|')
			# line is protocol | username | password | rightflag | phone number | initialized | disabled(=1) | callerid
                        if l[0] == "sip" and l[1] != sipaccount and l[5] == "1" and l[6] == "0":
				#print l[1], ": '" + l[4] + "'"
				if l[4] == "":
					l_sipnumlist["SIP/" + l[1]] = l[7]
				else:
					l_sipnumlist["SIP/" + l[4]] = l[7]
					adduser(l[0]+l[4], l[2])
                        elif l[0] == "iax" and l[5] == "1" and l[6] == "0":
				l_sipnumlist["IAX2/" + l[4]] = l[7]
                        elif l[0] == "misdn" and l[5] == "1" and l[6] == "0":
				l_sipnumlist["mISDN/" + l[4]] = l[7]
                        elif l[0] == "zap" and l[5] == "1" and l[6] == "0":
				l_sipnumlist["Zap/" + l[4]] = l[7]
				adduser(l[0]+l[4], l[2])
##			else:
##				deluser(l[0]+l[4])
	finally:
		f.close()
	return l_sipnumlist




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
    if lines[0].find("SIP/2.0") == 0:
        ret = int(lines[0].split(None)[1])
    for x in lines:
        if x.find("CSeq") == 0:
            cseq = int(x.split(None)[1])
            msg = x.split(None)[2]
        elif x.find("From: ") == 0:
            bpart = x.split("<sip:")[1]
            address = bpart.split("@")[0]
        elif x.find("Call-ID:") == 0:
            cid = x.split(None)[1]
        elif x.find("branch=") >= 0:
            b1 = x.split("branch=")[1]
            bbranch = b1.split(";")[0]
        elif x.find("tag=") >= 0:
            b1 = x.split("tag=")[1]
            btag = b1.split(";")[0]
    return [cseq, msg, cid, address, len(lines), ret, bbranch, btag]


## \brief Converts the SIP message to a useful presence information.
# Eventually, it will be done with XML functions.
# \param data the SIP message
# \return the extracted status
def tellpresence(data):
    t1 = "???"
    t2 = "????"
    lines = data.split("\n")

    for x in lines:
        if x.find("<note>") == 0:
            if x.find("Ready") >= 0:
                t2 = "Ready"
            elif x.find("On the phone") >= 0:
                t2 = "On the phone"
            elif x.find("Ringing") >= 0:
                t2 = "Ringing"
            elif x.find("Not online") >= 0:
                t2 = "Not online"
            elif x.find("Unavailable") >= 0:
                t2 = "Unavailable"
            else:
                t2 = "XivoUnknown"
        if x.find("<tuple id") == 0:
            t1 = x.split("\"")[1]
    return t1 + ":" + t2



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
			except:
				log_debug("AMI not connected")
			# if ko : down # if ok : retry / if retry ko : strange
	def printresponse_forever(self):
		# for debug
		while True:
			str = self.f.readline()
			#print self.i, len(str), str,
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
			if str == '\r\n' or str == '':
				break
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
	def login(self):
		try:
			self.sendcommand('login', [('Username', self.loginname), ('Secret', self.password), ('Events', 'off')])
			self.readresponse('')
			return True
		except self.AMIError, e:
			return False
	# \brief Redirects a Channel towards an Extension.
	def redirect(self, channel, extension, context):
		try:
			self.sendcommand('Redirect', [('Channel', channel), ('Exten', extension), ('Context', context), ('Priority', '1')])
			self.readresponse('')
			return True
		except self.AMIError, e:
			return False
	# \brief Hangs up a Channel.
	def hangup(self, astn, ch):
		phone = ch.split("/")[1] + "/" + ch.split("/")[2].split("-")[0]
		channel = ch.split("/")[1] + "/" + ch.split("/")[2]
		if channel in phonelists[astn][phone].chann:
			channel_peer = phonelists[astn][phone].chann[channel].getChannelPeer()
			log_debug("UI action : " + configs[astn].astid + " : hanging up <" + channel + "> and <" + channel_peer + ">")
			try:
				self.sendcommand('Hangup', [('Channel', channel)])
				self.readresponse('')
				self.sendcommand('Hangup', [('Channel', channel_peer)])
				self.readresponse('')
				return True
			except self.AMIError, e:
				return False
		else:
			log_debug("UI action : " + configs[astn].astid + " : no channel " + channel)
			return False
	# \brief Executes a CLI command.
	def execclicommand(self, command):
		# special procession for cli commands.
		self.sendcommand('Command', [('Command', command)])
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
	# \brief Originates a call from a phone towards another.
	def originate(self, src, dst):
		# originate a call btw src and dst
		# src will ring first, and dst will ring when src responds
		phonesrc = src.split("/")[2]
		phonedst = dst.split("/")[2]
		self.sendcommand('Originate', [('Channel', 'SIP/' + phonesrc),
					       ('Exten', phonedst),
					       ('Context', 'local-extensions'),
					       ('Priority', '1'),
					       # ('CallerID', phonesrc + " calls " + phonedst),
					       ('CallerID', phonesrc),
					       ('Async', 'true')])
		self.readresponse('')
	# TODO : replace management with "phone src" to "channel src" => no need to look up the list
	def transfer(self, astn, src, dst):
		phonesrc = src.split("/")[1] + "/" + src.split("/")[2].split("-")[0]
		phonesrcchan = src.split("/")[1] + "/" + src.split("/")[2]
		phonedst = dst.split("/")[2]
		if phonesrc in phonelists[astn].keys():
			channellist = phonelists[astn][phonesrc].chann
			nopens = len(channellist)
			if nopens == 0:
				log_debug("no channel currently open in the phone " + phonesrc)
			else:
				self.redirect(channellist[phonesrcchan][3], phonedst, 'local-extensions')


## \brief Builds the full list of callerIDs in order to send them to the requesting client.
# This should be done after a command called "callerid".
# \return a string containing the full callerIDs list
# \sa manage_tcp_connection
def build_callerids():
	global configs, phonelists
	fullstat = "callerids="
	for astnum in items_asterisks:
		sskeys = phonelists[astnum].keys()
		sskeys.sort()
		for ss in sskeys:
			phoneinfo = "cid:" + configs[astnum].astid + ":" \
				    + phonelists[astnum][ss].tech + ":" \
				    + ss.split("/")[1] + ":" \
				    + phonelists[astnum][ss].callerid
			fullstat += phoneinfo + ";"
	fullstat += "\n"
	return fullstat


## \brief Builds the channel-by-channel part in the hints/update replies.
# \param astnum the Asterisk numerical identifier
# \param sipphone the phone identifier
# \return the string containing the statuses for each channel of the given phone
def build_fullstatlist(astnum, sipphone):
	global phonelists
	nchans = len(phonelists[astnum][sipphone].chann)
	fstat = str(nchans)
	for chan in phonelists[astnum][sipphone].chann.keys():
		fstat += ":" + chan + ":" + phonelists[astnum][sipphone].chann[chan].getStatus() + ":" + \
			 str(phonelists[astnum][sipphone].chann[chan].getDeltaTime()) + ":" + \
			 phonelists[astnum][sipphone].chann[chan].getDirection() + ":" + \
			 phonelists[astnum][sipphone].chann[chan].getChannelPeer() + ":" + \
			 phonelists[astnum][sipphone].chann[chan].getChannelNum()
	return fstat


## \brief Builds the full list of phone statuses in order to send them to the requesting client.
# \return a string containing the full list of statuses
def build_statuses():
	global configs, phonelists
	fullstat = "hints="
	for astnum in items_asterisks:
		sskeys = phonelists[astnum].keys()
		sskeys.sort()
		for ss in sskeys:
			phoneinfo = "hnt:" + configs[astnum].astid + ":" \
				    + phonelists[astnum][ss].tech + ":" \
				    + ss.split("/")[1] + ":" \
				    + phonelists[astnum][ss].imstat + ":" \
				    + phonelists[astnum][ss].sipstatus
			phonelists[astnum][ss].update_time()
			fullstat += phoneinfo + ":" + build_fullstatlist(astnum, ss) + ";"
	fullstat += "\n"
	return fullstat


## \brief Sends a status update to all the connected xivo-switchboard(-like) clients.
# \param astnum the asterisk numerical identifier
# \param sipphone the phone identifier
# \param who a string that tells who has requested such an update
# \return none
def update_GUI_clients(astnum, sipphone, who):
	global tcpopens_sb, phonelists, configs

	phoneinfo = who + ":" \
		    + configs[astnum].astid + ":" \
		    + phonelists[astnum][sipphone].tech + ":" \
		    + sipphone.split("/")[1] +":" \
		    + phonelists[astnum][sipphone].imstat + ":" \
		    + phonelists[astnum][sipphone].sipstatus
	fstatlist = build_fullstatlist(astnum, sipphone)
	for tcpclient in tcpopens_sb:
		try:
			tcpclient[0].send("update=" + phoneinfo + ":" + fstatlist + "\n")
		except:
			log_debug("send has failed on " + str(tcpclient[0]))


## \brief Handles the SIP messages according to their meaning (reply to a formerly sent message).
# \param astnum the asterisk numerical identifier
# \param data   the data read from the socket
# \param l_sipsock the socket identifier in order to reply
# \param l_addrsip the SIP address in order to reply
# \return True if it is an OPTIONS packet
# \sa read_sip_properties
def parseSIP(astnum, data, l_sipsock, l_addrsip):
    global tcpopens_sb, phonelists, configs
    spret = False
    [icseq, imsg, icid, iaddr, ilength, iret, ibranch, itag] = read_sip_properties(data)
    # if ilength != 11:
    #print "###", astnum, ilength, icseq, icid, iaddr, imsg, iret, ibranch, itag
##    if imsg == "REGISTER" and iret == 200 and icid == "reg_cid@xivopy":
##        for k in tcpopens_sb:
##            k[0].send("asterisk=registered_" + configs[astnum].astid + "\n")
    if imsg == "SUBSCRIBE":
	    sipphone = "SIP/" + icid.split("@")[0].split("subscribexivo_")[1]
	    if sipphone in phonelists[astnum].keys(): # else : send sth anyway ?
		    phonelists[astnum][sipphone].set_lasttime(time.time())
		    if iret != 200:
			    if phonelists[astnum][sipphone].sipstatus != "Fail" + str(iret):
				    phonelists[astnum][sipphone].set_sipstatus("Fail" + str(iret))
				    update_GUI_clients(astnum, sipphone, "sip")
    if imsg == "OPTIONS" or imsg == "NOTIFY":
	    command = xivo_sip.sip_ok(configs[astnum], "sip:" + configs[astnum].mysipname,
				      icseq, icid, iaddr, imsg, ibranch, itag)
	    l_sipsock.sendto(command,(configs[astnum].remoteaddr, l_addrsip[1]))
	    if imsg == "NOTIFY":
		    stat = tellpresence(data)
		    if stat != "???:????":
			    sipphone = "SIP/" + stat.split(":")[0]
			    sstatus = stat.split(":")[1]
			    phonelists[astnum][sipphone].set_lasttime(time.time())
			    if phonelists[astnum][sipphone].sipstatus != sstatus:
				    phonelists[astnum][sipphone].set_sipstatus(sstatus)
				    update_GUI_clients(astnum, sipphone, "sip")
	    else:
		    spret = True
    return spret


## \brief Deals with requests from the UI clients.
# \param connid connection identifier
# \param allow_events tells if this connection belongs to events-allowed ones
# (for switchboard) or to events-disallowed ones (for php CLI commands)
# \return none
def manage_tcp_connection(connid, allow_events):
    global AMIclasssock
    global AMIcomms
    try:
	    msg = connid[0].recv(bufsize_large)
    except:
	    msg = ""
	    log_debug("manage_tcp_connection : a problem occured when recv from " + str(connid[0]))
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
		except:
			log_debug("manage_tcp_connection : a problem occured when sending to " + str(connid[0]))
        elif usefulmsg == "callerids":
		try:
			connid[0].send(build_callerids())
		except:
			log_debug("manage_tcp_connection : a problem occured when sending to " + str(connid[0]))
        elif usefulmsg != "":
		l = usefulmsg.split()
		if len(l) == 2 and l[0] == 'hangup':
			idassrc = -1
			assrc = l[1].split("/")[0]
			if assrc in asteriskr: idassrc = asteriskr[assrc]
			if idassrc != -1:
				if not AMIclasssock[idassrc]:
					log_debug("AMI was not connected - attempting to connect again")
					AMIclasssock[idassrc] = connect_to_AMI((configs[idassrc].remoteaddr,
										configs[idassrc].ami_port),
									       configs[idassrc].ami_login,
									       configs[idassrc].ami_pass)
				if AMIclasssock[idassrc]:
					ret = AMIclasssock[idassrc].hangup(idassrc, l[1])
					if ret == True:
						connid[0].send("asterisk=hangup successful\n")
					else:
						connid[0].send("asterisk=hangup KO\n")
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
						AMIclasssock[idassrc].originate(l[1], l[2])
						connid[0].send("asterisk=originate successful\n")
					elif l[0] == 'transfer':
						AMIclasssock[idassrc].transfer(idassrc, l[1], l[2])
						connid[0].send("asterisk=transfer successful " + str(idassrc) + "\n")
			else:
				connid[0].send("asterisk=originate or transfer KO\n")
		else:
			n = -1
			for i in items_asterisks:
				if configs[i].ipaddress_php == connid[1]:
					n = i
			if n == -1:
				connid[0].send("XIVO CLI:NOT ALLOWED\n")
			else:
				connid[0].send("XIVO CLI:" + configs[i].astid + "\n")
				s = AMIclasssock[n].execclicommand(usefulmsg.strip())
				for x in s: connid[0].send(x)
				connid[0].send("XIVO CLI:OK\n")


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
	global localchans
	if is_normal_channel(src):
		sipnum = src.split("-")[0]
		if sipnum in listkeys:
			phonelists[astnum][sipnum].set_chan(src, "Calling", 0, dir_to_string, dst, "")
			update_GUI_clients(astnum, sipnum, "ami-ed")
		else: notmonitoredsrc[src] = "d"
	elif src.find("Local/") == 0:
		if src in localchans:
			localchans[src].set_state("Dial")
			localchans[src].set_peer(dst)
			log_debug("[watch] Local/ Dial : " + src + " " + localchans[src].state + " " + \
				  localchans[src].callerid + " " + localchans[src].peer)
		else: notmonitoredsrc[src] = "d"
	else: notmonitoredsrc[src] = "d"

	if is_normal_channel(dst):
		sipnum = dst.split("-")[0]
		if sipnum in listkeys:
			if clid == "<unknown>" and is_normal_channel(src):
				clid = src.split("-")[0].split("/")[1]
			phonelists[astnum][sipnum].set_chan(dst, "Ringing", 0, dir_from_string, src, clid)
			update_GUI_clients(astnum, sipnum, "ami-ed")
		else: notmonitoreddst[dst] = "d"
	elif dst.find("Local/") == 0:
		log_debug("[watch] Dial to Local/ : " + src + " " + dst)
		notmonitoreddst[dst] = "d"
	else: notmonitoreddst[dst] = "d"


## \brief Updates some channels according to the Link events occuring in the AMI.
# \param listkeys the list of allowed phones
# \param astnum the Asterisk numerical identifier
# \param src the source channel
# \param dst the dest channel
# \param clid1 the src callerid
# \param clid2 the dest callerid
# \return none
def handle_ami_event_link(listkeys, astnum, src, dst, clid1, clid2):
	global localchans
	if is_normal_channel(src):
		sipnum = src.split("-")[0]
		if sipnum in listkeys:
			phonelists[astnum][sipnum].set_chan(src, "On the phone", 0, dir_to_string, dst, clid2)
			update_GUI_clients(astnum, sipnum, "ami-el1")
		else: notmonitoredsrc[src] = "l"
	elif src.find("Local/") == 0:
		if src in localchans:
			localchans[src].set_state("Link")
			localchans[src].set_peer(dst)
			log_debug("[watch] Local/ Link : " + src + " " + localchans[src].state + " " + \
				  localchans[src].callerid + " " + localchans[src].peer)
		else: notmonitoredsrc[src] = "l"
	else: notmonitoredsrc[src] = "l"

	if is_normal_channel(dst):
		sipnum = dst.split("-")[0]
		if sipnum in listkeys:
			if clid1 == "(null)" and is_normal_channel(src):
				clid1 = src.split("-")[0].split("/")[1]
			phonelists[astnum][sipnum].set_chan(dst, "On the phone", 0, dir_from_string, src, clid1)
			update_GUI_clients(astnum, sipnum, "ami-el2")
		else: notmonitoreddst[dst] = "l"
	elif dst.find("Local/") == 0: # occurs when someone picks up the phone
		if dst in notmonitoreddst.keys(): del notmonitoreddst[dst]
		# here dst ends with ",1" => binding with the same with ",2"
		newdst = dst.replace(",1", ",2")
		log_debug("[watch] Link to Local/ : " + src + " " + dst + " => " + newdst)
		#notmonitoreddst[newdst] = "l"
		if newdst in localchans :
			sipnuma = src.split("-")[0]
			sipnumb = localchans[newdst].peer.split("-")[0]

			phonelists[astnum][sipnuma].set_chan(src, "On the phone", 0, dir_to_string, localchans[newdst].peer, localchans[newdst].peer)
			update_GUI_clients(astnum, sipnuma, "ami-eq")
			phonelists[astnum][sipnumb].set_chan(localchans[newdst].peer, "On the phone", 0, dir_from_string, src, localchans[newdst].callerid)
			update_GUI_clients(astnum, sipnumb, "ami-eq")
	else: notmonitoreddst[dst] = "l"


## \brief Updates some channels according to the Hangup events occuring in the AMI.
# \param listkeys the list of allowed phones
# \param astnum the Asterisk numerical identifier
# \param chan the channel
# \param cause the reason why there has been a hangup
# \return
def handle_ami_event_hangup(listkeys, astnum, chan, cause):
	global localchans
	if is_normal_channel(chan):
		sipnum = chan.split("-")[0]
		if sipnum in listkeys:
			phonelists[astnum][sipnum].set_chan_hangup(chan)
			update_GUI_clients(astnum, sipnum, "ami-eh")
			phonelists[astnum][sipnum].del_chan(chan)
			update_GUI_clients(astnum, sipnum, "ami-eh")
		else:
			if chan in notmonitoredsrc.keys(): del notmonitoredsrc[chan]
			if chan in notmonitoreddst.keys(): del notmonitoreddst[chan]
	elif chan.find("Local/") == 0:
		if chan in localchans:
			localchans[chan].set_state("Hup")
			log_debug("[watch] Local/ Hangup : " + chan + " " + localchans[chan].state + " " + \
				  localchans[chan].callerid + " " + localchans[chan].peer)
		else:
			if chan in notmonitoredsrc.keys(): del notmonitoredsrc[chan]
			if chan in notmonitoreddst.keys(): del notmonitoreddst[chan]
	else:
		if chan in notmonitoredsrc.keys(): del notmonitoredsrc[chan]
		if chan in notmonitoreddst.keys(): del notmonitoreddst[chan]


## \brief Returns a given field from an AMI line.
# \param lineami the line extracted from AMI
# \param field the field whose value one is interested in
# \return the value of the field
def getvalue(lineami, field):
	ret = ""
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
	global phonelists, configs, save_for_next_packet_events, localchans
	listkeys = phonelists[astnum].keys()

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
			log_debug("AMI:Reload: " + configs[astnum].astid)
		elif x.find("Shutdown;") == 7:
			log_debug("AMI:Shutdown: " + configs[astnum].astid)
		elif x.find("Join;") == 7:
			clid  = getvalue(x, "CallerID")
			qname = getvalue(x, "Queue")
			if len(clid) > 0:
				for k in tcpopens_sb:
					k[0].send("asterisk=<" + clid + "> is calling the Queue <" + qname + ">\n")
		elif x.find("PeerStatus;") == 7:
			# <-> register's ? notify's ?
			pass
		elif x.find("Agentlogin;") == 7: pass
		elif x.find("Agentlogoff;") == 7: pass
		elif x.find("Alarm;") == 7: pass
		elif x.find("MeetmeJoin;") == 7: pass
		elif x.find("MeetmeLeave;") == 7: pass
		elif x.find("OriginateSuccess;") == 7: pass
		elif x.find("OriginateFailure;") == 7:
			log_debug("AMI:OriginateFailure: " + configs[astnum].astid + \
				  " - reason=" + getvalue(x, "Reason"))
		elif x.find("Rename;") == 7:
			# appears when there is a transfer
			channel_old = getvalue(x, "Oldname")
			channel_new = getvalue(x, "Newname")
			if channel_old.find("<MASQ>") < 0 and channel_new.find("<MASQ>") < 0 and \
			       is_normal_channel(channel_old) and is_normal_channel(channel_new):
				log_debug("AMI:Rename:N: " + configs[astnum].astid + " : " + \
					  "old=" + channel_old + " new=" + channel_new)
				phone_old = channel_old.split("-")[0]
				phone_new = channel_new.split("-")[0]

				channel_p1 = phonelists[astnum][phone_old].chann[channel_old].getChannelPeer()
				channel_p2 = phonelists[astnum][phone_new].chann[channel_new].getChannelPeer()
				phone_p1 = channel_p1.split("-")[0]

				if channel_p2 == "": # occurs when 72 (interception) is called
					# A is calling B, intercepted by C
					# in this case old = B and new = C
					if phone_new in listkeys:
						phonelists[astnum][phone_new].chann[channel_new].setChannelPeer(channel_p1)
						update_GUI_clients(astnum, phone_new, "ami-er")
					if phone_p1 in listkeys:
						phonelists[astnum][phone_p1].chann[channel_p1].setChannelPeer(channel_new)
						update_GUI_clients(astnum, phone_p1, "ami-er")
				else:
					# A -> B  then B' transfers to C
					# in this case old = B' and new = A
					# => channel_p1 = peer(old) = C
					# => channel_p2 = peer(new) = B

					phone_p2 = channel_p2.split("-")[0]
					# the new peer of A is C / the new peer of C is A
					if phone_new in listkeys and phone_old in listkeys:
						phonelists[astnum][phone_new].chann[channel_new].setChannelPeer(channel_p1)
						phonelists[astnum][phone_new].chann[channel_new].setChannelNum(phonelists[astnum][phone_old].chann[channel_old].getChannelNum())
						update_GUI_clients(astnum, phone_new, "ami-er")
					if phone_p1 in listkeys and phone_p2 in listkeys:
						phonelists[astnum][phone_p1].chann[channel_p1].setChannelPeer(channel_new)
						phonelists[astnum][phone_p1].chann[channel_p1].setChannelNum(phonelists[astnum][phone_p2].chann[channel_p2].getChannelNum())
						update_GUI_clients(astnum, phone_p1, "ami-er")
			else:
				log_debug("AMI:Rename:A: " + configs[astnum].astid + " : " + \
					  "old=" + channel_old + " new=" + channel_new)
		elif x.find("ExtensionStatus;") == 7:
			pass
		elif x.find("Newstate;") == 7:
			chan    = getvalue(x, "Channel")
			clid    = getvalue(x, "CallerID")
			state   = getvalue(x, "State")
			sipnum = chan.split("-")[0]
			if sipnum in listkeys:
				phonelists[astnum][sipnum].set_chan(chan, state, 0, "", "", "")
				update_GUI_clients(astnum, sipnum, "ami-ns")
		elif x.find("Newcallerid;") == 7:
			# for tricky queues' management
			chan    = getvalue(x, "Channel")
			cid     = getvalue(x, "CallerID")
			cidname = getvalue(x, "CallerIDName")
			log_debug("AMI:Newcallerid: " + configs[astnum].astid + \
				  " channel=" + chan + " callerid=" + cid + " calleridname=" + cidname)
			if chan.find("Local/") == 0:
				localchans[chan] = TmpLocalChannel("Init", cid)
		elif x.find("Newchannel;") == 7:
			chan    = getvalue(x, "Channel")
			clid    = getvalue(x, "CallerID")
			sipnum = chan.split("-")[0]
			if sipnum in listkeys:
				phonelists[astnum][sipnum].set_chan(chan, "", 0, "", "", "")
				# update_GUI_clients(astnum, sipnum, "ami-nc")
			if not (clid == "" or (clid == "<unknown>" and is_normal_channel(chan))):
				for k in tcpopens_sb:
					k[0].send("asterisk=<" + clid + "> is entering the Asterisk <" + configs[astnum].astid + "> through " + chan + "\n")
		elif x.find("Newexten;") == 7: # in order to handle outgoing calls ?
			chan    = getvalue(x, "Channel")
			exten   = getvalue(x, "Extension")
			if exten != "s" and exten != "h" and exten != "t":
				#print "--- exten :", chan, exten
				if is_normal_channel(chan):
					sipnum = chan.split("-")[0]
					if sipnum in listkeys:
						phonelists[astnum][sipnum].set_chan(chan, "Calling", 0, dir_to_string, "", exten)
						update_GUI_clients(astnum, sipnum, "ami-en")
					else:
						log_debug("AMI:Newexten: " + configs[astnum].astid + " warning : " + sipnum + " does not belong to our phone list")
		elif x.find("MessageWaiting;") == 7:
			mwi_string = getvalue(x,"Mailbox") + " waiting=" + getvalue(x,"Waiting")
			if int(getvalue(x,"Waiting")) > 0:
				mwi_string += "; new=" + getvalue(x, "New") + "; old=" + getvalue(x, "Old")
			log_debug("AMI:MessageWaiting: " + configs[astnum].astid + " : " + mwi_string)
		elif x.find("QueueMemberStatus;") == 7:
			queuenameq = getvalue(x, "Queue")
			location   = getvalue(x, "Location")
			status     = getvalue(x, "Status")
			log_debug("AMI:QueueMemberStatus: " + configs[astnum].astid + " " + queuenameq + " " + location + " " + status)
		elif x.find("Leave;") == 7:
			queuenameq = getvalue(x, "Queue")
			log_debug("AMI:Leave: " + configs[astnum].astid + " " + queuenameq)
		else:
			if len(x) > 0:
				log_debug("AMI:XXX: " + configs[astnum].astid + " <" + x + ">")


## \brief Handling of AMI events for the status.
# These are AMI events received as a reply to a command.
# \param astnum the asterisk numerical identifier
# \param idata the data read from the AMI we want to parse
# \return
def handle_ami_status(astnum, idata):
	global phonelists, configs, save_for_next_packet_status
	listkeys = phonelists[astnum].keys()

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
							sipnum = link.split("-")[0]
							if sipnum in listkeys:
								phonelists[astnum][sipnum].set_chan(link, "On the phone", int(seconds), dir_from_string, chan, clid)
								update_GUI_clients(astnum, sipnum, "001")
						if is_normal_channel(chan):
							sipnum = chan.split("-")[0]
							if sipnum in listkeys:
								phonelists[astnum][sipnum].set_chan(chan, "On the phone", int(seconds), dir_to_string, link, exten)
								update_GUI_clients(astnum, sipnum, "001")
					else:
						log_debug("AMI::Status UP: " + chan + " " + clid + " " + exten + " " + seconds)
				else:
					pass
			elif x.find(";State: Ring;") >= 0:
				log_debug("AMI::Status TO: " + getvalue(x, "Channel") + \
					  " " + getvalue(x, "Extension") + " " + getvalue(x, "Seconds"))
			elif x.find(";State: Ringing;") >= 0:
				log_debug("AMI::Status FROM: " + getvalue(x, "Channel"))

## \brief Sends a SIP register + n x SIP subscribe messages.
# \param astnum the asterisk numerical identifier
# \param l_sipsock the SIP socket where to reply
# \return
def do_sip_register_subscribe(astnum, l_sipsock):
	global tcpopens_sb, phonelists, configs
	rdc = chr(65 + 32 * random.randrange(2) + random.randrange(26))
	command = xivo_sip.sip_register(configs[n], "sip:" + configs[n].mysipname, 1, "reg_cid@xivopy", expires)
	l_sipsock.sendto(command, (configs[n].remoteaddr, configs[n].portsipsrv))
	#command = xivo_sip.sip_options(configs[n], "sip:" + configs[n].mysipname, "testoptions@xivopy", "107")
	#l_sipsock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
	#l_sipsock.sendto(command, ("192.168.0.255", 5060))

	for sipnum in phonelists[astnum].keys():
		if sipnum.find("SIP/") == 0:
			dtnow = time.time() - phonelists[astnum][sipnum].lasttime
			if dtnow > (2 * timeout_between_registers):
				if phonelists[astnum][sipnum].sipstatus != "Timeout":
					phonelists[astnum][sipnum].set_sipstatus("Timeout")
					update_GUI_clients(astnum, sipnum, "sip")
			cid = rdc + "subscribexivo_" + sipnum.split("/")[1] + "@" + configs[n].localaddr
			command = xivo_sip.sip_subscribe(configs[n], "sip:" + configs[n].mysipname, 1,
							 cid,
							 sipnum.split("/")[1], expires)
			l_sipsock.sendto(command, (configs[n].remoteaddr, configs[n].portsipsrv))
			#command = xivo_sip.sip_options(configs[n], "sip:" + configs[n].mysipname,
			#cid, sipnum)
			#l_sipsock.sendto(command, (configs[n].remoteaddr, configs[n].portsipsrv))


## \brief Updates the list of sip numbers according to the SSO then sends old and new peers to the UIs.
# The reconnection to the AMI is also done here when it has been broken.
# If the AMI sockets are dead, a reconnection is also attempted here.
# \param astnum the asterisk numerical identifier
# \return none
# \sa updateuserlistfromurl
def update_sipnumlist(astnum):
	global phonelists, configs
	if len(notmonitoredsrc) > 0: log_debug("WARNING : unmonitored src list : " + str(notmonitoredsrc))
	if len(notmonitoreddst) > 0: log_debug("WARNING : unmonitored dst list : " + str(notmonitoreddst))

	if AMIsocks[astnum] == -1 and AMIcomms[astnum] == -1:
		log_debug(configs[astnum].astid + " : attempting to reconnect to AMI")
		als0 = xivo_ami.ami_socket_login(configs[astnum].remoteaddr,
						 configs[astnum].ami_port,
						 configs[astnum].ami_login,
						 configs[astnum].ami_pass, False)
		als1 = xivo_ami.ami_socket_login(configs[astnum].remoteaddr,
						 configs[astnum].ami_port,
						 configs[astnum].ami_login,
						 configs[astnum].ami_pass, True)
		AMIcomms[astnum] = als0
		AMIsocks[astnum] = als1
		if AMIsocks[astnum] != -1 and AMIcomms[astnum] != -1:
			ins.append(als0)
			ins.append(als1)
			log_debug(configs[astnum].astid + " : AMI is BACK")
		else:
			log_debug(configs[astnum].astid + " : AMI is NOT back")
	sipnumlistold = phonelists[astnum].keys()
	sipnumlistold.sort()
	sipnuml = updateuserlistfromurl(configs[astnum].userlisturl, configs[astnum].mysipname)
	for x in configs[astnum].extrachannels.split(","):
		if x != "": sipnuml[x] = x
	sipnumlistnew = sipnuml.keys()
	sipnumlistnew.sort()
	if sipnumlistnew != sipnumlistold:
		lstdel = ""
		lstadd = ""
		for snl in sipnumlistold:
			if snl not in sipnumlistnew:
				del phonelists[astnum][sipnum] # or = "Absent"/0 ?
				lstdel += "del:" + configs[astnum].astid + ":SIP:" + snl + ";"
		for snl in sipnumlistnew:
			if snl not in sipnumlistold:
				phonelists[astnum][snl] = LineProp()
				phonelists[astnum][snl].set_callerid(sipnuml[snl])
				if snl.find("IAX2") == 0:
					phonelists[astnum][snl].set_tech("IAX2")
					phonelists[astnum][snl].set_sipstatus("Ready")
				elif snl.find("mISDN") == 0:
					phonelists[astnum][snl].set_tech("mISDN")
					phonelists[astnum][snl].set_sipstatus("Ready")
				elif snl.find("Zap") == 0:
					phonelists[astnum][snl].set_tech("Zap")
					phonelists[astnum][snl].set_sipstatus("Ready")
				lstadd += "add:" + configs[astnum].astid + ":" + phonelists[astnum][snl].tech + ":" + snl + ":unknown:0;"
		xivo_ami.ami_socket_status(AMIcomms[astnum])
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

## \class TmpLocalChannel
# \brief Properties of a temporary "Local" channel.
class TmpLocalChannel:
	# \brief Class initialization.
	def __init__(self, istate, icallerid):
		self.state = istate
		self.callerid = icallerid
		self.peer = ""
	# \brief Sets the peer channel name.
	def set_peer(self, ipeer):
		self.peer = ipeer
	def set_state(self, istate):
		self.state = istate
	def set_callerid(self, icallerid):
		self.callerid = icallerid


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
	# \brief Protocol of the phone
	
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
	
	## \var queueavail
	# \brief Queue availability (only the member name for now)
	
	## \var callerid
	# \brief Caller ID
	
	##  \brief Class initialization.
	def __init__(self):
		self.tech = "SIP"
		self.lasttime = 0
		self.chann = {}
		self.sipstatus = "BefSubs" # Asterisk status
		self.imstat = "unknown"  # XMPP / Instant Messaging status
		self.voicemail = ""  # Voicemail status
		self.queueavail = "" # Availability as a queue member
		self.callerid = "nobody"
	def set_tech(self, itech):
		self.tech = itech
	def set_sipstatus(self, isipstatus):
		self.sipstatus = isipstatus
	def set_imstat(self, istatus):
		self.imstat = istatus
	def set_lasttime(self, ilasttime):
		self.lasttime = ilasttime
	def set_callerid(self, icallerid):
		self.callerid = icallerid
	##  \brief Updates the time elapsed on a channel according to current time.
	def update_time(self):
		nowtime = time.time()
		for ic in self.chann:
			dtime = int(nowtime - self.chann[ic].getTime())
			self.chann[ic].updateDeltaTime(dtime)

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
		try:
			del self.chann[nichan]
		except:
			log_debug("a problem occured when trying to remove " + nichan)

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
def adduser(user, passwd):
	global userlist
	if userlist.has_key(user):
		userlist[user]['passwd'] = passwd
	else:
		userlist[user] = {'user':user, 'passwd':passwd}

## \brief Deletes a user from the userlist.
# \param user the user to delete
# \return none
def deluser(user):
	global userlist
	if userlist.has_key(user):
		userlist.pop(user)

## \brief Returns the user from the list.
# \param user searched for
# \return user found, otherwise None
def finduser(user):
	u = userlist.get(user)
	return u


## \class LoginHandler
# \brief The clients connect to this in order to obtain a valid session id.
# This could be enhanced to support a more complete protocol
# supporting commands coming from the client in order to pilot asterisk.
class LoginHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		#print '  request :', self.request
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
		log_debug("LoginHandler : client connected = " + str(self.client_address) + " " + list1[1])
		list1 = self.rfile.readline().strip().split(' ')
		if len(list1) != 2 or list1[0] != 'PASS':
			self.wfile.write('ERROR\r\n')
			return
		passwd = list1[1]
		#print 'user/pass : ' + user + '/' + passwd
		userlist_lock.acquire()
		updateuserlistfromurl(configs[asteriskr[astname_xivoc]].userlisturl,
				      configs[asteriskr[astname_xivoc]].mysipname)
		e = finduser(user)
		goodpass = (e != None) and (e.get('passwd') == passwd)
		userlist_lock.release()
		if not goodpass:
			self.wfile.write('ERROR : WRONG LOGIN PASSWD\r\n')
			return
		self.wfile.write('Send PORT command\r\n')
		list1 = self.rfile.readline().strip().split(' ')
		if len(list1) != 2 or list1[0] != 'PORT':
			self.wfile.write('ERROR\r\n')
			return
		port = list1[1]
		# TODO : random pas au top, faire generation de session id plus luxe
		sessionid = '%u' % random.randint(0,999999999)
		userlist_lock.acquire()
		e['sessionid'] = sessionid
		e['sessiontimestamp'] = time.time()
		e['ip'] = self.client_address[0]
		e['port'] = port
		e['state'] = 'available'
		userlist_lock.release()
		retline = 'OK SESSIONID ' + sessionid + '\r\n'
		self.wfile.write(retline)
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
				e = finduser(user)
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
			except:
				retline = 'ERROR (exception)\r\n'
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
			if len(list) < 4 or list[0] != 'ALIVE' or list[2] != 'SESSIONID':
				response = 'ERROR unknown\r\n'
			else:
				astname_xivoc = list[1].split("/")[0]
				user = list[1].split("/")[1]
				sessionid = list[3]
				state = 'available'
				if len(list) >= 6:
					state = list[5]
				e = finduser(user)
				if e == None:
					response = 'ERROR user unknown\r\n'
				else:
					#print user, e['user']
					#print sessionid, e['sessionid']
					#print ip, e['ip']
					#print timestamp, e['sessiontimestamp']
					#print timestamp - e['sessiontimestamp']
					if sessionid==e['sessionid'] and ip==e['ip'] and e['sessiontimestamp'] + session_expiration_time > timestamp:
						e['state'] = state
						e['sessiontimestamp'] = timestamp
						response = 'OK\r\n'
					else:
						response = 'ERROR SESSION EXPIRED OR INVALID\r\n'
		except:
			response = 'ERROR (exception)\r\n'
		userlist_lock.release()
		self.request[1].sendto(response, self.client_address)
		if response == 'OK\r\n':
			n = asteriskr[astname_xivoc]
			if n >= 0:
				sipnumber = user.split("sip")[1]
				#print "    from Xivo client", self.client_address, user, sipnumber, sessionid, ip, state
				phonelists[n]["SIP/" + sipnumber].set_imstat(state)
				update_GUI_clients(n, "SIP/" + sipnumber, "kfc")


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

xivoconf = ConfigParser.ConfigParser()
xivoconf.readfp(open("/etc/asterisk/xivo_daemon.conf"))
port_ui_srv               = int(xivoconf.get("general", "port_switchboard")) # 5081
port_phpui_srv            = int(xivoconf.get("general", "port_php")) # 5081
port_login                = int(xivoconf.get("general", "port_fiche_login")) # 12345
port_keepalive            = int(xivoconf.get("general", "port_fiche_keepalive")) # 12346
port_request              = int(xivoconf.get("general", "port_fiche_agi")) # 12347
port_switchboard_base_sip = int(xivoconf.get("general", "port_switchboard_base_sip")) # 5080

configs = []
save_for_next_packet_events = []
save_for_next_packet_status = []
n = 0

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
					      port_switchboard_base_sip + 2 * n,
					      int(xivoconf.get(i, "sip_port")),
					      xivoconf.get(i, "sip_presence_account")))
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


# opens the logfile for output
logfile = open("/var/log/xivo_daemon.log", 'a')

phonelists = []
SIPsocks = []
AMIsocks = []
AMIcomms = []
AMIclasssock = []
asteriskr = {}

# We have three sockets to listen to so we cannot use the 
# very easy to use SocketServer.serve_forever()
# So select() is what we need. The SocketServer.handle_request() calls
# won't block the execution. In case of the TCP servers, they will
# spawn a new thread, in case of the UDP server, the request handling
# process should be fast. If it isnt, use a threading UDP server ;)
ins = [loginserver.socket, requestserver.socket, keepaliveserver.socket]

items_asterisks = xrange(len(configs))

for n in items_asterisks:
	phonelists.append({})

	SIPsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	SIPsock.bind(("", configs[n].portsipclt))
	SIPsocks.append(SIPsock)
	ins.append(SIPsock)

	AMIclasssock.append(connect_to_AMI((configs[n].remoteaddr, configs[n].ami_port),
					   configs[n].ami_login, configs[n].ami_pass))
	asteriskr[configs[n].astid] = n
	
	als0 = xivo_ami.ami_socket_login(configs[n].remoteaddr,
					 configs[n].ami_port,
					 configs[n].ami_login,
					 configs[n].ami_pass, False)
	AMIcomms.append(als0)
	if als0 != -1:
		ins.append(als0)
	als1 = xivo_ami.ami_socket_login(configs[n].remoteaddr,
					 configs[n].ami_port,
					 configs[n].ami_login,
					 configs[n].ami_pass, True)
	AMIsocks.append(als1)
	if als1 != -1:
		ins.append(als1)

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

log_debug("# STARTING XIVO Daemon # do_sip_register_subscribe (first)")
for n in items_asterisks:
	update_sipnumlist(n)
	do_sip_register_subscribe(n, SIPsocks[n])
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
	elif [j for j in i if j in SIPsocks]:
		res = [j for j in i if j in SIPsocks][0]
		for n in items_asterisks:
			if SIPsocks[n] == res:
				break
		[data, addrsip] = SIPsocks[n].recvfrom(bufsize_udp)
		is_an_options_packet = parseSIP(n, data, SIPsocks[n], addrsip)
		if is_an_options_packet:
			log_debug(configs[n].astid + " : do_sip_register_subscribe (parse SIP)")
			update_sipnumlist(n)
			do_sip_register_subscribe(n, SIPsocks[n])
			lastrequest_time[n] = time.time()
	# these AMI connections are used in order to manage AMI commands with incoming events
	elif [j for j in i if j in AMIsocks]:
		res = [j for j in i if j in AMIsocks][0]
		for n in items_asterisks:
			if AMIsocks[n] == res:
				break
		a = AMIsocks[n].recv(bufsize_any)
		if len(a) == 0: # end of connection from server side : closing socket
			log_debug(configs[n].astid + " : CLOSING AMI (events = on)")
			AMIsocks[n].close()
			ins.remove(AMIsocks[n])
			AMIsocks[n] = -1
		else:
			handle_ami_event(n, a)
	# these AMI connections are used in order to manage AMI commands without events
	elif i[0] in AMIcomms:
		for n in items_asterisks:
			if AMIcomms[n] in i:
				break
		a = AMIcomms[n].recv(bufsize_any)
		if len(a) == 0: # end of connection from server side : closing socket
			log_debug(configs[n].astid + " : CLOSING AMI (events = off)")
			AMIcomms[n].close()
			ins.remove(AMIcomms[n])
			AMIcomms[n] = -1
		else:
			handle_ami_status(n, a)
        elif UIsock in i:
		[conn, UIsockparams] = UIsock.accept()
		log_debug("TCP (SB)  socket opened on   " + UIsockparams[0] + " " + str(UIsockparams[1]))
		# appending the opened socket to the ones watched
		ins.append(conn)
		conn.setblocking(0)
		tcpopens_sb.append([conn, UIsockparams[0], UIsockparams[1]])
        elif PHPUIsock in i:
		[conn, PHPUIsockparams] = PHPUIsock.accept()
		log_debug("TCP (PHP) socket opened on   " + PHPUIsockparams[0] + " " + str(PHPUIsockparams[1]))
		# appending the opened socket to the ones watched
		ins.append(conn)
		conn.setblocking(0)
		tcpopens_php.append([conn, PHPUIsockparams[0], PHPUIsockparams[1]])
        elif [j for j in tcpopens_sb if j[0] in i]:
		# input connections from the switchboard
		conn = [j for j in tcpopens_sb if j[0] in i][0]
		manage_tcp_connection(conn, True)
        elif [j for j in tcpopens_php if j[0] in i]:
		# input connections from the PHP interface
		conn = [j for j in tcpopens_php if j[0] in i][0]
		manage_tcp_connection(conn, False)
	else:
		log_debug("unknown socket " + str(i))

	for n in items_asterisks:
		if (time.time() - lastrequest_time[n]) > timeout_between_registers:
			lastrequest_time[n] = time.time()
			log_debug(configs[n].astid + " : do_sip_register_subscribe (computed timeout)")
			update_sipnumlist(n)
			do_sip_register_subscribe(n, SIPsocks[n])
    else:
	    log_debug("do_sip_register_subscribe (select's timeout)")
	    for n in items_asterisks:
		    lastrequest_time[n] = time.time()
		    update_sipnumlist(n)
		    do_sip_register_subscribe(n, SIPsocks[n])


try:
	os.unlink(pidfile)
except Exception, e:
	print e

print 'end of the execution flow...'
sys.exit(0)

# Close files and sockets
logfile.close()

