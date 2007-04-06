#!/usr/bin/python
# $Id$
#
# Author of the Kafiche daemon part : Thomas Bernard
#
#

import os, posix, select, socket, string, sys, time
import random
import SocketServer, telnetlib, urllib
import xml.dom.minidom, xml
from time import strftime
##import time
import threading
##import signal
import sip

port_ami     = 5038
port_sip_srv = 5060
port_ui_srv  = 5081

# configuration options :
port_login = 12345
port_keepalive = port_login + 1
port_request = 12347
session_expiration_time = 60*1
guserlisturl = 'file:///home/corentin/sso_obelisk.php' # http://192.168.0.254/service/ipbx/sso.php
astname_xivoc = ""

dir_to_string = ">"
dir_from_string = "<"
save_for_next_packet = ""
save_for_next_packet_status = ""

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

pidfile = '/tmp/sip_switchboard_id_daemon.pid'
bufsize_large = 8192
bufsize_any = 8

asterisk_login = 'sylvain'
asterisk_pass = 'sylvain'

timeout_between_registers = 60
expires = str(2 * timeout_between_registers) # timeout between subscribes

queuenamej = ""
queuemembers = {}

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

# function to load sso.php user file
# other would-be channel types to handle : Zap, MGCP, CAPI, <X>h323, ...
def updateuserlistfromurl(url):
	l_sipnumlist = {}
	f = urllib.urlopen(url)
	try:
		for line in f:
			# remove leading/tailing whitespaces
			line = line.strip()
			l = line.split('|')
			# line is protocol | username | password | rightflag | phone number | initialized | disabled(=1) | cid
                        if l[0] == "sip" and l[1] != "xivosb" and l[5] == "1" and l[6] == "0":
				#			    print l[1], ": '" + l[4] + "'"
				if l[4] == "":
					l_sipnumlist["SIP/" + l[1]] = l[7]
				else:
					l_sipnumlist["SIP/" + l[4]] = l[7]
					adduser(l[0]+l[4], l[2])
                        elif l[0] == "iax" and l[5] == "1" and l[6] == "0":
				l_sipnumlist["IAX2/" + l[4]] = l[7]
                        elif l[0] == "misdn" and l[5] == "1" and l[6] == "0":
				l_sipnumlist["mISDN/" + l[4]] = l[7]
#				adduser(l[0]+l[4], l[2])
##			else:
##				deluser(l[0]+l[4])
	finally:
		f.close()
	return l_sipnumlist

# logins into the Asterisk MI
def ami_socket_login(raddr, loginname, events):
	try:
		sockid = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sockid.connect((raddr, port_ami))
		sockid.recv(bufsize_any)
		# check against "Asterisk Call Manager/1.0\r\n"
		if events == 0:
			sockid.send("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + loginname + "\r\nEvents: off\r\n\r\n");
		else:
			sockid.send("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + loginname + "\r\nEvents: on\r\n\r\n");
		# check against "Message: Authentication accepted\r\n"
	except:
		sockid = 0
	return sockid

# sends a Status command
def ami_socket_status(sockid):
	sockid.send("Action: Status\r\n\r\n")

# reading of the CSeq / message type (REGISTER, OPTION, SUBSCRIBE, ...)
#                callid / address / # of lines / return code (200, 404, 484, ...)
def read_sip_properties(data):
    cseq = 1
    msg = "xxx"
    cid = "no_callid@xivopy"
    ret = -99
    btag = "no_tag"

    lines = data.split("\n")
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


# converts the SIP message to a useful presence information
def tellpresence(data):
    t1 = "???"
    t2 = "????"
    lines = data.split("\n")

##    sxml = ""
##    ind = 0
##    for x in data.split("\n"):
##        if x.find("<?xml") == 0:
##            ind = 1
##        if ind == 1:
##            sxml += x + "\n"
##        if x.find("From: ") == 0:
##            if __debug__:
##                print x.split("<sip:")[1]
##            add1 = x.split("@")[0]

##    if sxml != "":
##        axml = xml.dom.minidom.parseString(sxml)
##        docu = axml.getElementsByTagName('note')
##        for as in docu:
##            print as

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



# class AMI definition in order to interact with the Asterisk AMI
class AMI:
	class AMIError(Exception):
		def __init__(self, msg):
                    self.msg = msg
		def __str__(self):
                    return msg
	def __init__(self, address, loginname, password):
		self.address   = address
		self.loginname = loginname
		self.password  = password
		self.i = 1
	def connect(self):
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect(self.address)
		self.f = s.makefile()
		s.close()
		str = self.f.readline()
#		print str,
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
					print "retrying AMI command " + action
					self.sendcommand(action, args)
			except:
				print "AMI not connected"
			# if ko : down # if ok : retry / if retry ko : strange
	def printresponse_forever(self):
		# for debug
		while True:
			str = self.f.readline()
#			print self.i, len(str), str,
			self.i = self.i + 1
	def readresponsechunk(self):
		start = True
		list = []
		while True:
			str = self.f.readline()
			#print self.i, len(str), str,
			self.i = self.i + 1
			if start and str == '\r\n': continue
			start = False
			if str == '\r\n' or str == '':
				break
			l = [ x.strip() for x in str.split(': ') ]
			if len(l) == 2:
				list.append((l[0], l[1]))
		return dict(list)
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
	def redirect(self, channel, extension, context):
		try:
			self.sendcommand('Redirect', [('Channel', channel), ('Exten', extension), ('Context', context), ('Priority', '1')])
			self.readresponse('')
			return True
		except self.AMIError, e:
			return False
	def hangup(self, astn, channel):
		phone = channel.split("-")[0]
		if channel in phonelists[astn][phone].chans:
			peer = phonelists[astn][phone].chans[channel][3]
			print "hanging up " + channel + " and " + peer
			try:
				self.sendcommand('Hangup', [('Channel', channel)])
				self.readresponse('')
				self.sendcommand('Hangup', [('Channel', peer)])
				self.readresponse('')
				return True
			except self.AMIError, e:
				return False
		else:
			print "no channel", channel, "in Asterisk", configs[astn].astid
	def execclicommand(self, command):
		# special procession for cli commands.
		self.sendcommand('Command', [('Command', command)])
		resp = []
		for i in (1, 2): str = self.f.readline()
		#for i in (1, 2): print 'discarding line:', self.f.readline(),
		while True:
			str = self.f.readline()
			#print self.i, len(str), str,
			self.i = self.i + 1
			if str == '\r\n' or str == '' or str == '--END COMMAND--\r\n':
				break
			resp.append(str)
		return resp
	def originate(self, src, dst):
		# originate a call btw src and dst
		# src will ring first, and dst will ring when src responds
		self.sendcommand('Originate', [('Channel', 'SIP/'+src),
					       ('Exten', dst),
					       ('Context', 'local-extensions'),
					       ('Priority', '1'),
					       ('CallerID', src + " calls " + dst),
					       ('Async', 'true')])
	# TODO : replace management with "phone src" to "channel src" => no need to look up the list
	def transfer(self, astn, src, dst):
		print "transfer", astn, src, dst
		phonesrc = "SIP/" + src
		if phonesrc in phonelists[astn].keys():
			channellist = phonelists[astn][phonesrc].chans
			nopens = len(channellist)
			if nopens == 0:
				print "no channel currently open in the phone", phonesrc
			else:
				for ch in channellist:
					print ch, channellist[ch][3]
					self.redirect(channellist[ch][3], dst, 'local-extensions')


# builds the full list of phone statuses in order to send them to the requesting client
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

# builds the channel-by-channel part in the hints/update replies
def build_fullstatlist(astnum, sipphone):
	global phonelists
	nchans = len(phonelists[astnum][sipphone].chans)
	fstat = str(nchans)
	for chan in phonelists[astnum][sipphone].chans.keys():
		fstat += ":" + chan + ":" + phonelists[astnum][sipphone].chans[chan][0] + ":" + \
			 str(phonelists[astnum][sipphone].chans[chan][1]) + ":" + \
			 phonelists[astnum][sipphone].chans[chan][2] + ":" + \
			 phonelists[astnum][sipphone].chans[chan][3] + ":" + \
			 phonelists[astnum][sipphone].chans[chan][4]
	return fstat

# builds the full list of phone statuses in order to send them to the requesting client
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

# sends a status update to all the connected xivo-switchboard(-like) clients
def update_GUI_clients(config, astnum, sipphone, who):
	global tcpopens, phonelists

	phoneinfo = who + ":" \
		    + config.astid + ":" \
		    + phonelists[astnum][sipphone].tech + ":" \
		    + sipphone.split("/")[1] +":" \
		    + phonelists[astnum][sipphone].imstat + ":" \
		    + phonelists[astnum][sipphone].sipstatus
	aaa = build_fullstatlist(astnum, sipphone)
	for tcpclient in tcpopens:
		try:
			tcpclient[0].send("update=" + phoneinfo + ":" + aaa + "\n")
		except:
			print "send has failed on", tcpclient[0]

# the incoming SIP messages are parsed (usually as a reply to a formerly sent message)
def parseSIP(cfg, data, l_sipsock, l_addrsip, astnum):
    global tcpopens, phonelists
    spret = 0
    [icseq, imsg, icid, iaddr, ilength, iret, ibranch, itag] = read_sip_properties(data)
    # if ilength != 11:
    # print "###", astnum, ilength, icseq, icid, iaddr, imsg, iret, ibranch, itag
    if imsg == "REGISTER" and iret == 200 and icid == "reg_cid@xivopy":
        for k in tcpopens:
            k[0].send("asterisk=registered_" + cfg.astid + "\n")
    if imsg == "SUBSCRIBE":
        sipphone = icid.split("@")[0].split("subscribexivo_")[1]
	if sipphone in phonelists[astnum].keys(): # else : send sth anyway ?
	    phonelists[astnum][sipphone].set_lasttime(time.time())
            if iret != 200:
		    if phonelists[astnum][sipphone].sipstatus != "Fail" + str(iret):
			    phonelists[astnum][sipphone].set_sipstatus("Fail" + str(iret))
			    update_GUI_clients(cfg, astnum, sipphone, "sip")
    if imsg == "OPTIONS" or imsg == "NOTIFY":
        command = sip.sip_ok(cfg, "sip:" + cfg.mysipname, icseq, icid, iaddr, imsg, ibranch, itag)
        l_sipsock.sendto(command,(cfg.remoteaddr, l_addrsip[1]))
	if imsg == "NOTIFY":
		stat = tellpresence(data)
		if stat != "???:????":
			sipphone = "SIP/" + stat.split(":")[0]
			sstatus = stat.split(":")[1]
			phonelists[astnum][sipphone].set_lasttime(time.time())
			if phonelists[astnum][sipphone].sipstatus != sstatus:
				phonelists[astnum][sipphone].set_sipstatus(sstatus)
				update_GUI_clients(cfg, astnum, sipphone, "sip")
        else:
		spret = 1
    return spret

def manage_connection(connid):
    global AMIconns
    global AMIcomms
    connid[0].setblocking(0)
    try:
	    msg = connid[0].recv(bufsize_large)
    except:
	    print "pb occurred"
    if len(msg) == 0:
        print "TCP socket closed from", connid[1], connid[2]
        connid[0].close()
        ins.remove(connid[0])
        tcpopens.remove(connid)
    else:
        usefulmsg_tmp = msg.split("\r\n")[0]
        usefulmsg = usefulmsg_tmp.split("\n")[0]
        if usefulmsg == "hints":
		try:
			connid[0].send(build_statuses())
		except:
			print "warning : there might have been a connection problem"
        elif usefulmsg == "callerids":
		try:
			connid[0].send(build_callerids())
		except:
			print "warning : there might have been a connection problem"
        elif usefulmsg != "":
            # different cases are treated whether AMIconns was already defined or not ... this part can certainly be improved
	    l = usefulmsg.split()
	    idast = asteriskr[l[1]]
	    print l, ":", idast
	    if l[0] == 'originate' or l[0] == 'transfer' or l[0] == 'hangup':
		    if not AMIconns[idast]:
			    "AMI was not connected - attempting to connect again"
			    AMIconns[idast] = connect_to_AMI((configs[idast].remoteaddr, port_ami), asterisk_login, asterisk_pass)
		    if AMIconns[idast]:
			    if l[0] == 'originate':
				    AMIconns[idast].originate(l[2], l[3])
			    elif l[0] == 'transfer':
				    AMIconns[idast].transfer(idast, l[2], l[3])
			    elif l[0] == 'hangup':
				    AMIconns[idast].hangup(idast, l[2])

def handle_ami_event_dial(listkeys, astnum, src, dst, clid, clidn):
	if src.find("SIP/") == 0 or src.find("IAX2/") == 0 or src.find("mISDN/") == 0:
		sipnum = src.split("-")[0]
		if sipnum in listkeys:
			phonelists[astnum][sipnum].set_chan(src, "Calling", 0, dir_to_string, dst, "")
			update_GUI_clients(configs[astnum], astnum, sipnum, "ami-ed")
		else:
			print "warning :", sipnum, "does not belong to our phone list"
	else:
		print "handle_ami_event_dial src", src
	if dst.find("SIP/") == 0 or dst.find("IAX2/") == 0 or dst.find("mISDN/") == 0:
		sipnum = dst.split("-")[0]
		if sipnum in listkeys:
			phonelists[astnum][sipnum].set_chan(dst, "Ringing", 0, dir_from_string, src, clid)
			update_GUI_clients(configs[astnum], astnum, sipnum, "ami-ed")
		else:
			print "warning :", sipnum, "does not belong to our phone list"
	else:
		print "handle_ami_event_dial dst", dst

def handle_ami_event_link(listkeys, astnum, src, dst, clid1, clid2):
	if src.find("SIP/") == 0 or src.find("IAX2/") == 0 or src.find("mISDN/") == 0:
		sipnum = src.split("-")[0]
		if sipnum in listkeys:
			phonelists[astnum][sipnum].set_chan(src, "On the phone", 0, dir_to_string, dst, clid2)
			update_GUI_clients(configs[astnum], astnum, sipnum, "ami-el")
		else:
			print "warning :", sipnum, "does not belong to our phone list"
	else:
		print "handle_ami_event_link src", src
	if dst.find("SIP/") == 0 or dst.find("IAX2/") == 0 or dst.find("mISDN/") == 0:
		sipnum = dst.split("-")[0]
		if sipnum in listkeys:
			phonelists[astnum][sipnum].set_chan(dst, "On the phone", 0, dir_from_string, src, clid1)
			update_GUI_clients(configs[astnum], astnum, sipnum, "ami-el")
		else:
			print "warning :", sipnum, "does not belong to our phone list"
	else:
		print "handle_ami_event_link dst", dst

def handle_ami_event_hangup(listkeys, astnum, chan, cause):
	if chan.find("SIP/") == 0 or chan.find("IAX2/") == 0 or chan.find("mISDN/") == 0:
		sipnum = chan.split("-")[0]
		if sipnum in listkeys:
			phonelists[astnum][sipnum].set_chan_hangup(chan)
			update_GUI_clients(configs[astnum], astnum, sipnum, "ami-eh")
			phonelists[astnum][sipnum].del_chan(chan)
			update_GUI_clients(configs[astnum], astnum, sipnum, "ami-eh")
		else:
			print "warning :", sipnum, "does not belong to our phone list"

# handling of AMI events
def handle_ami_event(astnum, idata):
	global phonelists, configs, queuenamej, queuemembers, save_for_next_packet
	listkeys = phonelists[astnum].keys()

	full_idata = save_for_next_packet + idata
	evlist = full_idata.split("\r\n\r\n")
	save_for_next_packet = evlist.pop()

	for z in evlist:
		# we assume no ";" character is present in AMI events fields
		x = z.replace("\r\n", ";")
		if x.find("Dial;") == 7:
#			print x
			src = x.split(";Source: ")[1].split(";")[0]
			dst = x.split(";Destination: ")[1].split(";")[0]
			clid = x.split(";CallerID: ")[1].split(";")[0]
			clidn = x.split(";CallerIDName: ")[1].split(";")[0]
			handle_ami_event_dial(listkeys, astnum, src, dst, clid, clidn)
		elif x.find("Link;") == 7:
			src = x.split(";Channel1: ")[1].split(";")[0]
			dst = x.split(";Channel2: ")[1].split(";")[0]
			clid1 = x.split(";CallerID1: ")[1].split(";")[0]
			clid2 = x.split(";CallerID2: ")[1].split(";")[0]
			handle_ami_event_link(listkeys, astnum, src, dst, clid1, clid2)
		elif x.find("Unlink;") == 7:
			# might be something to parse here
			chan1 = x.split(";Channel1: ")[1].split(";")[0]
			chan2 = x.split(";Channel2: ")[1].split(";")[0]
			cid1 = x.split(";CallerID1: ")[1].split(";")[0]
			cid2 = x.split(";CallerID2: ")[1].split(";")[0]
		elif x.find("Hangup;") == 7:
			chan = x.split(";Channel: ")[1].split(";")[0]
			cause = x.split(";Cause-txt: ")[1].split(";")[0]
			handle_ami_event_hangup(listkeys, astnum, chan, cause)
		elif x.find("Reload;") == 7:
			print "Reloading Asterisk : ", configs[astnum].astid
		elif x.find("Join;") == 7:
			clidq = x.split(";CallerID: ")[1].split(";")[0]
			queuenamej = x.split(";Queue: ")[1].split(";")[0]
			if queuenamej in queuemembers.keys():
				zstr = ""
				for q in queuemembers[queuenamej].keys():
					zstr += " " + q
				if len(clidq) > 0:
					for k in tcpopens:
						k[0].send("asterisk=<" + clidq + "> is calling the Queue <" + queuenamej + "> :" + zstr + "\n")
			else:
				print "#### queue", queuenamej, "not defined !"
				if len(clidq) > 0:
					for k in tcpopens:
						k[0].send("asterisk=<" + clidq + "> is calling the Queue <" + queuenamej + ">\n")
		elif x.find("PeerStatus;") == 7:
			abc = 0
		elif x.find("MeetmeJoin;") == 7:
			abc = 0
		elif x.find("MeetmeLeave;") == 7:
			abc = 0
		elif x.find("Rename;") == 7:
			abc = 0
		elif x.find("Newstate;") == 7:
			abc = 0
		elif x.find("ExtensionStatus;") == 7:
			abc = 0
		elif x.find("Newcallerid;") == 7:
			abc = 0
		elif x.find("Newchannel;") == 7:
			chan = x.split(";Channel: ")[1].split(";")[0]
			clid = x.split(";CallerID: ")[1].split(";")[0]
			if not (clid == "" or (clid == "<unknown>" and chan.find("SIP/") == 0)):
				for k in tcpopens:
					k[0].send("asterisk=<" + clid + "> is entering the Asterisk <" + configs[astnum].astid + "> through " + chan + "\n")
		elif x.find("MessageWaiting;") == 7:
			print "MWI", x.split(";Mailbox: ")[1].split(";")[0], \
			      x.split(";Waiting: ")[1].split(";")[0],
			if int(x.split(";Waiting: ")[1].split(";")[0]) > 0:
				print x.split(";New: ")[1].split(";")[0], \
				      x.split(";Old: ")[1].split(";")[0]
			else:
				print
		elif x.find("Newexten;") == 7: # in order to handle outgoing calls ?
			chan = x.split(";Channel: ")[1].split(";")[0]
			exten = x.split(";Extension: ")[1].split(";")[0]
			if exten != "s" and exten != "h" and exten != "t":
				#				print "--- exten :", chan, exten
				if chan.find("SIP/") == 0 or chan.find("IAX2/") == 0 or chan.find("mISDN/") == 0:
					sipnum = chan.split("-")[0]
					if sipnum in listkeys:
						phonelists[astnum][sipnum].set_chan(chan, "Calling", 0, dir_to_string, "", exten)
						update_GUI_clients(configs[astnum], astnum, sipnum, "ami-en")
					else:
						print "warning :", sipnum, "does not belong to our phone list"
		elif x.find("QueueMemberStatus;") == 7:
			print x
			queuenameq = x.split(";Queue: ")[1].split(";")[0]
			location = x.split(";Location: ")[1].split(";")[0]
			status = x.split(";Status: ")[1].split(";")[0]
			if queuenameq not in queuemembers.keys():
				queuemembers[queuenameq] = {}
			queuemembers[queuenameq][location] = 1
		elif x.find("Leave;") == 7:
			queuenameq = x.split(";Queue: ")[1].split(";")[0]
			print "leaving the queue ", queuenameq
		else:
			if len(x) > 0:
				print "      <" + x + ">"

# handling of AMI events for the status
def handle_ami_event_status(astnum, idata):
	global phonelists, configs, queuenamej, queuemembers, save_for_next_packet_status
	listkeys = phonelists[astnum].keys()

	full_idata = save_for_next_packet_status + idata
	evlist = full_idata.split("\r\n\r\n")
	save_for_next_packet_status = evlist.pop()

	for z in evlist:
		# we assume no ";" character is present in AMI events fields
		x = z.replace("\r\n", ";")
##		if len(x) > 0:
##			print "statuses --FULL--", x
		if x.find("Status;") == 7:
			if x.find(";State: Up;") >= 0:
				if x.find(";Seconds: ") >= 0:
					chan = x.split(";Channel: ")[1].split(";")[0]
					clid = x.split(";CallerID: ")[1].split(";")[0]
					exten = x.split(";Extension: ")[1].split(";")[0]
					seconds = x.split(";Seconds: ")[1].split(";")[0]
					if x.find(";Link: ") >= 0:
						link = x.split(";Link: ")[1].split(";")[0]
#						print "statuses up --------", chan, clid, exten, seconds, link
						if link.find("SIP/") == 0 or link.find("IAX2/") == 0 or link.find("mISDN/") == 0:
							sipnum = link.split("-")[0]
							if sipnum in listkeys:
								phonelists[astnum][sipnum].set_chan(link, "On the phone", int(seconds), dir_from_string, chan, clid)
								update_GUI_clients(configs[astnum], astnum, sipnum, "001")
						if chan.find("SIP/") == 0 or chan.find("IAX2/") == 0 or chan.find("mISDN/") == 0:
							sipnum = chan.split("-")[0]
							if sipnum in listkeys:
								phonelists[astnum][sipnum].set_chan(chan, "On the phone", int(seconds), dir_to_string, link, exten)
								update_GUI_clients(configs[astnum], astnum, sipnum, "001")
					else:
						print "statuses up --------", chan, clid, exten, seconds
			elif x.find(";State: Ring;") >= 0:
				print "statuses to --------", \
				      x.split(";Channel: ")[1].split(";")[0], \
				      x.split(";Extension: ")[1].split(";")[0], \
				      x.split(";Seconds: ")[1].split(";")[0]
			elif x.find(";State: Ringing;") >= 0:
				print "statuses fr --------", \
					  x.split(";Channel: ")[1].split(";")[0]

# sends a SIP register + n x SIP subscribe messages
def do_sip_register_subscribe(cfg, l_sipsock, astnum):
    global tcpopens, phonelists, rdc
    rdc = chr(65 + 32 * random.randrange(2) + random.randrange(26))
    for k in tcpopens:
        k[0].send("asterisk=will_register_" + cfg.astid + "\n")
    command = sip.sip_register(cfg, "sip:" + cfg.mysipname, 1, "reg_cid@xivopy", expires)
    l_sipsock.sendto(command, (cfg.remoteaddr, port_sip_srv))
##    command = sip.sip_options(cfg, "sip:" + cfg.mysipname, "testoptions@xivopy", "107")
##    l_sipsock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
##    l_sipsock.sendto(command, ("192.168.0.255", 5060))

    for sipnum in phonelists[astnum].keys():
	    if sipnum.find("SIP/") == 0:
		    dtnow = time.time() - phonelists[astnum][sipnum].lasttime
		    if dtnow > (2 * timeout_between_registers):
			    if phonelists[astnum][sipnum].sipstatus != "Timeout":
				    phonelists[astnum][sipnum].set_sipstatus("Timeout")
				    update_GUI_clients(cfg, astnum, sipnum, "sip")
		    command = sip.sip_subscribe(cfg, "sip:" + cfg.mysipname,
						1, rdc + "subscribexivo_" + sipnum.split("/")[1] + "@" + cfg.localaddr,
						sipnum.split("/")[1], expires)
		    l_sipsock.sendto(command, (cfg.remoteaddr, port_sip_srv))
##        command = sip.sip_options(cfg, "sip:" + cfg.mysipname,
##				  rdc + "subscribexivo_" + sipnum + "@" + cfg.localaddr,
##				  sipnum)
##        l_sipsock.sendto(command, (cfg.remoteaddr, port_sip_srv))


# updates the list of sip numbers according to the sso
# then sends old and new peers to the UIs
def update_sipnumlist(cfg, astnum):
	global phonelists
	sipnumlistold = phonelists[astnum].keys()
	sipnumlistold.sort()

	sipnuml = updateuserlistfromurl(cfg.userlisturl)
	sipnumlistnew = sipnuml.keys()
	sipnumlistnew.sort()
	if sipnumlistnew != sipnumlistold:
		lstdel = ""
		lstadd = ""
		for snl in sipnumlistold:
			if snl not in sipnumlistnew:
				del phonelists[astnum][sipnum] # or = "Absent"/0 ?
				lstdel += "del:" + cfg.astid + ":SIP:" + snl + ";"
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
				lstadd += "add:" + cfg.astid + ":" + phonelists[astnum][snl].tech + ":" + snl + ":unknown:0;"
		ami_socket_status(AMIcomms[astnum])
		for k in tcpopens:
			if lstdel != "":
				k[0].send("peerremove=" + lstdel + "\n")
			if lstadd != "":
				k[0].send("peeradd=" + lstadd + "\n")

def connect_to_AMI(address, loginname, password):
	lAMIsock = AMI(address, loginname, password)
	try:
		lAMIsock.connect()
		lAMIsock.login()
	except:
		del lAMIsock
		lAMIsock = False
	return lAMIsock


class LineProp:
	def __init__(self):
		self.tech = "SIP"
		self.lasttime = 0
		self.chans = {}
		self.sipstatus = "BefSubs" # Asterisk status
		self.imstat = "unknown"  # XMPP / Instant Messaging status
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
	def update_time(self):
		nowtime = time.time()
		for ic in self.chans:
			dtime = int(nowtime - self.chans[ic][5])
			self.chans[ic][1] = dtime
	def set_chan(self, ichan, status, itime, dir, peerch, peernum):
		# does not update peerch and peernum if the new values are empty
		newpeerch = peerch
		newpeernum = peernum
		if ichan in self.chans:
			if peerch == "":
				newpeerch = self.chans[ichan][3]
			if peernum == "":
				newpeernum = self.chans[ichan][4]
		firsttime = time.time()
		self.chans[ichan] = [status, itime, dir, newpeerch, newpeernum, firsttime - itime]
		for ic in self.chans:
			self.chans[ic][1] = int(firsttime - self.chans[ic][5])
	def set_chan_hangup(self, ichan):
		nichan = ichan
		if ichan.find("<ZOMBIE>") >= 0:
			print "sch channel contains a <ZOMBIE> part :", ichan, ": sending hup to", nichan, "anyway"
			nichan = ichan.split("<ZOMBIE>")[0]
		firsttime = time.time()
		self.chans[nichan] = ["Hangup", 0, "", "", "", firsttime]
		for ic in self.chans:
			self.chans[ic][1] = int(firsttime - self.chans[ic][5])
	def del_chan(self, ichan):
		nichan = ichan
		if ichan.find("<ZOMBIE>") >= 0:
			print "dch channel contains a <ZOMBIE> part :", ichan, ": deleting", nichan, "anyway"
			nichan = ichan.split("<ZOMBIE>")[0]
		try:
			del self.chans[nichan]
		except:
			print "a problem occured when trying to remove", nichan


class AsteriskRemote:
	def __init__(self,
		     astid,
		     userlisturl,
		     localaddr = "127.0.0.1",
		     remoteaddr = "127.0.0.1",
		     portsipclt = 5080):

		self.astid = astid
		self.userlisturl = userlisturl
		self.localaddr = localaddr
		self.remoteaddr = remoteaddr
		self.portsipclt = portsipclt
		self.mysipname = "xivosb"

# ==============================================================================
# from kafiche daemon
# ==============================================================================
# add (or update) a user in the userlist
def adduser(user, passwd):
	global userlist
	if userlist.has_key(user):
		userlist[user]['passwd'] = passwd
	else:
		userlist[user] = {'user':user, 'passwd':passwd}

# delete a user from the userlist
def deluser(user):
	global userlist
	if userlist.has_key(user):
		userlist.pop(user)

# finduser() returns the user from the list.
# None is returned if not found
def finduser(user):
	return userlist.get(user)

# The daemon has 3 listening sockets :
# - Login - TCP - (the clients connect to it to login) - need SSL ?
# - KeepAlive - UDP - (the clients send datagram to it to inform
#                      of their current state)
# - IdentRequest - TCP - offer a service to ask for localization and 
#                        state of the clients.

# we use the SocketServer "framework" to implement the "services"
# see http://docs.python.org/lib/module-SocketServer.html

# LoginHandler : the client connect to this in order to obtain a
# valid session id.
# This could be enhanced to support a more complete protocol
# supporting commands coming from the client in order to pilot asterisk.
class LoginHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		global guserlisturl, astname_xivoc
		if __debug__:
			print ' ## LoginHandler / client connected :', self.client_address
		#print '  request :', self.request
		list0 = self.rfile.readline()
		list1 = list0.strip().split(' ')
		if len(list1) != 2 or list1[0] != 'LOGIN':
			self.wfile.write('ERROR\r\n')
			return
		if list1[1].find("/"):
			astname_xivoc = list1[1].split("/")[0]
			user = list1[1].split("/")[1]
		else:
			astname_xivoc = "obelisk"
			user = list1[1]
		self.wfile.write('Send PASS for authentification\r\n')
		list1 = self.rfile.readline().strip().split(' ')
		if len(list1) != 2 or list1[0] != 'PASS':
			self.wfile.write('ERROR\r\n')
			return
		passwd = list1[1]
		#print 'user/pass : ' + user + '/' + passwd
		userlist_lock.acquire()
		updateuserlistfromurl(guserlisturl)
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
##		if __debug__:
##			print userlist


# IdentRequestHandler: give client identification to the profile pusher
# the connection is kept alive so several requests can be made on the 
# same open TCP connection.
class IdentRequestHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		if __debug__:
			print ' ## IdentRequestHandler from client', self.client_address
		while True:
			list0 = self.rfile.readline().strip().split(' ')
			retline = 'ERROR\r\n'
			if list0[0] == 'QUERY' and len(list0) == 2:
				user = list0[1]
				userlist_lock.acquire()
				try:
					e = finduser(user)
					if e == None:
						retline = 'ERROR USER NOT FOUND\r\n'
					elif time.time() - e.get('sessiontimestamp') > session_expiration_time:
						retline = 'ERROR USER SESSION EXPIRED\r\n'
					else:
						retline = 'USER ' + user
						retline += ' SESSIONID ' + e.get('sessionid')
						retline += ' IP ' + e.get('ip')
						retline += ' PORT ' + e.get('port')
						retline += ' STATE ' + e.get('state')
						retline += '\r\n'
				except:
					retline = 'ERROR (exception)\r\n'
				userlist_lock.release()
			try:
				self.wfile.write(retline)
			except Exception, e:
				# something bad happened.
				if __debug__:
					print ' ## Exception :', e
				return

# The KeepAliveHandler receives UDP datagrams and sends back 
# a datagram containing wether "OK" or "ERROR <error-text>"
# It could be a good thing to give a numerical code to each error.
class KeepAliveHandler(SocketServer.DatagramRequestHandler):
	def handle(self):
##		if __debug__:
##			print ' ## KeepAliveHandler from client', self.client_address
		userlist_lock.acquire()
		try:
			ip = self.client_address[0]
			list = self.request[0].strip().split(' ')
			timestamp = time.time()
			# ALIVE user SESSIONID sessionid
			if len(list) < 4 or list[0] != 'ALIVE' or list[2] != 'SESSIONID':
				response = 'ERROR unknown\r\n'
			else:
				user = list[1]
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
			n = -1
			for n in items_asterisks:
				if configs[n].astid == astname_xivoc:
					break
			if n >= 0:
				sipnumber = user.split("sip")[1]
#				print "    from Xivo client", self.client_address, user, sipnumber, sessionid, ip, state
				phonelists[n]["SIP/" + sipnumber].set_imstat(state)
				update_GUI_clients(configs[n], n, "SIP/" + sipnumber, "kfc")


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
logfile = open("/var/log/sip_switchboard.log", 'a')

configs = [AsteriskRemote("clg", "file:///home/corentin/sso_clg.php"),
	   AsteriskRemote("obelisk", "file:///home/corentin/sso_obelisk.php", "192.168.0.77", "192.168.0.254", 5082)]

phonelists = []
SIPsocks = []
AMIsocks = []
AMIcomms = []
AMIconns = []
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

	AMIconns.append(connect_to_AMI((configs[n].remoteaddr, port_ami), asterisk_login, asterisk_pass))
	asteriskr[configs[n].astid] = n
	
	als0 = ami_socket_login(configs[n].remoteaddr, asterisk_login, 0)
	if als0:
		AMIcomms.append(als0)
		ins.append(als0)
	als1 = ami_socket_login(configs[n].remoteaddr, asterisk_login, 1)
	if als1:
		AMIsocks.append(als1)
		ins.append(als1)

UIsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
UIsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
UIsock.bind(("", port_ui_srv))
UIsock.listen(10)
ins.append(UIsock)

tcpopens = []
lastrequest_time = []

askedtoquit = False


print strftime("%b %2d %H:%M:%S ", time.localtime()), "do_sip_register_subscribe (first)"
for n in items_asterisks:
	update_sipnumlist(configs[n], n)
	do_sip_register_subscribe(configs[n], SIPsocks[n], n)
	lastrequest_time.append(time.time())


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
        if i[0] in SIPsocks:
		for n in items_asterisks:
			if SIPsocks[n] in i:
				break
		[data, addrsip] = SIPsocks[n].recvfrom(bufsize_large)
		sp = parseSIP(configs[n], data, SIPsocks[n], addrsip, n)
		if sp == 1:
			if __debug__:
				print strftime("%b %2d %H:%M:%S ", time.localtime()), "do_sip_register_subscribe (parse SIP)", n
			update_sipnumlist(configs[n], n)
			do_sip_register_subscribe(configs[n], SIPsocks[n], n)
			lastrequest_time[n] = time.time()
	# these AMI connections are used in order to manage AMI commands with incoming events
	elif i[0] in AMIsocks:
		for n in items_asterisks:
			if AMIsocks[n] in i:
				break
		a = AMIsocks[n].recv(bufsize_any)
		if len(a) == 0: # end of connection from server side : closing socket
			AMIsocks[n].close()
			ins.remove(AMIsocks[n])
			AMIsocks[n] = 0
		else:
			handle_ami_event(n, a)
	# these AMI connections are used in order to manage AMI commands without events
	elif i[0] in AMIcomms:
		for n in items_asterisks:
			if AMIcomms[n] in i:
				break
		a = AMIcomms[n].recv(bufsize_any)
		if len(a) == 0: # end of connection from server side : closing socket
			AMIcomms[n].close()
			ins.remove(AMIcomms[n])
			AMIcomms[n] = 0
		else:
			handle_ami_event_status(n, a)
        elif UIsock in i:
            [conn, UIsockparams] = UIsock.accept()
            print "TCP socket opened on  ", UIsockparams[0], UIsockparams[1]
            # appending the opened socket to the ones watched
            ins.append(conn)
            tcpopens.append([conn, UIsockparams[0], UIsockparams[1]])
        else:
            for conn in tcpopens:
                if conn[0] in i:
                    manage_connection(conn)
	for n in items_asterisks:
		if (time.time() - lastrequest_time[n]) > timeout_between_registers:
			lastrequest_time[n] = time.time()
			if __debug__:
				print strftime("%b %2d %H:%M:%S ", time.localtime()), "do_sip_register_subscribe (computed timeout)", n
			update_sipnumlist(configs[n], n)
			do_sip_register_subscribe(configs[n], SIPsocks[n], n)
    else:
	    if __debug__:
		    print strftime("%b %2d %H:%M:%S ", time.localtime()), "do_sip_register_subscribe (select's timeout)"
	    for n in items_asterisks:
		    lastrequest_time[n] = time.time()
		    update_sipnumlist(configs[n], n)
		    do_sip_register_subscribe(configs[n], SIPsocks[n], n)

# Close files and sockets
logfile.close()





askedtoquit = False

# useful signals are catched here (in the main thread)
def sighandler(signum, frame):
	global askedtoquit
	print 'signal', signum, 'received, quitting' 
	askedtoquit = True

signal.signal(signal.SIGINT, sighandler)
signal.signal(signal.SIGTERM, sighandler)
signal.signal(signal.SIGHUP, sighandler)

# never ending loop handling events on the sockets.
while not askedtoquit:
	try:
		i, o, e = select.select(ins, [], [])
	except:
		# select was interupted by a signal. just continue
		# TODO: if it is not the case (=another error) catch it
		continue
	for x in i:
		if x == loginserver.socket:
			loginserver.handle_request()
		elif x == requestserver.socket:
			requestserver.handle_request()
		elif x == keepaliveserver.socket:
			keepaliveserver.handle_request()

try:
	os.unlink(pidfile)
except Exception, e:
	print e

print 'end of the execution flow...'
sys.exit(0)

