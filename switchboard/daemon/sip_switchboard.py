#!/usr/bin/python
# $Id$
#

import xml.dom.minidom, xml
import random
import socket, SocketServer, urllib
import os, posix, select, string, sys, time

port_ami     = 5038
port_sip_srv = 5060
port_ui_srv  = 5081

pidfile = '/tmp/sip_switchboard_id_daemon.pid'
bufsize = 2048

expires = "600"
asterisk_login = 'sylvain'
asterisk_pass = 'sylvain'

timeout_between_registers = 100

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
def updateuserlistfromurl(url):
	l_sipnumlist = []
	f = urllib.urlopen(url)
	try:
		for line in f:
			# remove leading/tailing whitespaces
			line = line.strip()
			l = line.split('|')
			# line is protocol | username | password | rightflag | phone number | initialized | disabled(=1)
                        if l[0] == "sip" and l[1] != "xivosb" and l[5] == "1" and l[6] == "0":
                            l_sipnumlist.append(l[4])
	finally:
		f.close()
	return l_sipnumlist

# functions in order to build a SIP packet
# SIP SUBSCRIBE
def sip_subscribe(cfg, me, cseq, callid, sipnumber):
    rsrc = "sip:" + str(sipnumber)
    here = cfg.localaddr + ":" + str(cfg.portsipclt)
    command = "SUBSCRIBE " + rsrc + "@" + cfg.remoteaddr + " SIP/2.0\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=" + str(random.randrange(1000000)) + "\r\n"
    command += "To: <" + rsrc + "@" + cfg.remoteaddr + ">\r\n"
    command += "From: <" + me + "@" + cfg.remoteaddr + ">;tag=" + str(random.randrange(1000000)) + "\r\n"
    command += "Call-ID: " + callid + "\r\n"
    command += "CSeq: " + str(cseq) + " SUBSCRIBE\r\n"
    command += "Max-Forwards: 70\r\n"
    command += "Event: presence\r\n"
    command += "Accept: application/pidf+xml\r\n"
    command += "Contact: <" + me + "@" + here + ">\r\n"
    command += "Expires: " + expires + "\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

# SIP OK (in order to reply to OPTIONS (qualify) and NOTIFY (when presence subscription))
def sip_ok(cfg, me, cseq, callid, sipaddr, smsg, lbranch, ltag):
    rsrc = "sip:" + sipaddr
    here = cfg.localaddr + ":" + str(cfg.portsipclt)
    command = "SIP/2.0 200 OK\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=" + lbranch + "\r\n"
    command += "From: <" + rsrc + "@" + cfg.remoteaddr + ">;tag=" + ltag + "\r\n"
    command += "To: <" + me + "@" + cfg.remoteaddr + ">\r\n"
    command += "Call-ID: " + callid + "\r\n"
    command += "CSeq: " + str(cseq) + " " + smsg + "\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

# SIP REGISTER
def sip_register(cfg, me, cseq, callid):
    here = cfg.localaddr + ":" + str(cfg.portsipclt)
    command = "REGISTER sip:" + cfg.remoteaddr + " SIP/2.0\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=" + str(random.randrange(1000000)) + "\r\n"
    command += "To: <" + me + "@" + cfg.remoteaddr + ">\r\n"
    command += "From: <" + me + "@" + cfg.remoteaddr + ">;tag=" + str(random.randrange(1000000)) + "\r\n"
    command += "Call-ID: " + callid + "\r\n"
    command += "CSeq: " + str(cseq) + " REGISTER\r\n"
    command += "Max-Forwards: 70\r\n"
    command += "Contact: <" + me + "@" + here + ">\r\n"
    command += "Expires: " + expires + "\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

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
	def __init__(self, address):
		self.address = address
		self.i = 1
	def connect(self):
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect(asterisk_ami)
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
		return ret
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
		#print first
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
	def login(self, user, passwd):
		try:
			self.sendcommand('login', [('Username', user), ('Secret', passwd), ('Events', 'off')])
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
	def hangup(self, channel):
		try:
			self.sendcommand('Hangup', [('Channel', channel)])
			self.readresponse('')
			return True
		except self.AMIError, e:
			return False
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
		ret = AMIsock.sendcommand('Originate', [('Channel', 'SIP/'+src),
                                                         ('Exten', dst),
                                                         ('Context', 'local-extensions'),
                                                         ('Priority', '1'),
                                                         ('CallerID', 'Mise en relation <1911>'),
                                                         ('Async', 'true')])
#		print AMIsock.readresponse('')
		return ret
	def transfer(self, src, dst):
		AMIsock.sendcommand('Status', [])
		for ch in AMIsock.readresponse('StatusComplete'):
			if ch.has_key('CallerID') and ch['CallerID'] == src and ch.has_key('Link'):
				AMIsock.redirect(ch['Link'], dst, 'local-extensions')


def build_statuses(plist):
	global cfgs, phonelists
	fullstat = "hints="

##	sskeys = plist.keys()
##	sskeys.sort()
##	for ss in sskeys:
##		fullstat += cfg.astid + ":" + ss + ":" + plist[ss].status + ";"

	for astid in [0, 1]:
#	for phonelist in phonelists:
		sskeys = phonelists[astid].keys()
		sskeys.sort()
		for ss in sskeys:
			fullstat += cfgs[astid].astid + ":" + ss + ":" + phonelists[astid][ss].status + ";"

	fullstat += "\n"
	return fullstat


def parseSIP(cfg, data, l_sipsock, l_addrsip, astid):
    global tcpopens, phonelists, sipnumlists
    spret = 0
    [icseq, imsg, icid, iaddr, ilength, iret, ibranch, itag] = read_sip_properties(data)
    # if ilength != 11:
    print "###", astid, ilength, icseq, icid, iaddr, imsg, iret, ibranch, itag
    if imsg == "REGISTER" and iret == 200 and icid == "reg_cid@xivopy":
        for k in tcpopens:
            k[0].send("asterisk=registered\n")
    if imsg == "SUBSCRIBE":
        fields = icid.split("@")[0].split("subscribexivo_")[1]
	if fields in sipnumlists[astid]: # else : send sth anyway ?
	    phonelists[astid][fields].set_lasttime(time.time())
            if iret != 200:
		    if phonelists[astid][fields].status != "Fail" + str(iret):
			    phonelists[astid][fields].set_status("Fail" + str(iret))
			    for k in tcpopens:
				    k[0].send("update=" + cfg.astid + ":" + fields + ":" + phonelists[astid][fields].status + "\n")
    if imsg == "OPTIONS" or imsg == "NOTIFY":
        command = sip_ok(cfg, "sip:" + cfg.mysipname, icseq, icid, iaddr, imsg, ibranch, itag)
        l_sipsock.sendto(command,(cfg.remoteaddr, l_addrsip[1]))
	if imsg == "NOTIFY":
		stat = tellpresence(data)
		if stat != "???:????":
			sipphone = stat.split(":")[0]
			phonelists[astid][sipphone].set_lasttime(time.time())
			if phonelists[astid][sipphone].status != stat.split(":")[1]:
				phonelists[astid][sipphone].set_status(stat.split(":")[1])
				for k in tcpopens:
					k[0].send("update=" + cfg.astid + ":" + stat + "\n")
        else:
            spret = 1
    return spret

def manage_connection(connid):
    global AMIsock
    connid[0].setblocking(0)
    msg = connid[0].recv(bufsize)
    if len(msg) == 0:
        print "TCP socket closed from", connid[1], connid[2]
        connid[0].close()
        ins.remove(connid[0])
        tcpopens.remove(connid)
    else:
        usefulmsg_tmp = msg.split("\r\n")[0]
        usefulmsg = usefulmsg_tmp.split("\n")[0]
        if usefulmsg == "hints":
            connid[0].send(build_statuses(phonelists[0]))
        elif usefulmsg != "":
            # different cases are treated whether AMIsock was already defined or not
            # ... this part can certainly be improved
            # only the "originate" command is properly handled right now
            if not AMIsock:
                "AMI was not connected - attempting to connect again"
                AMIsock = connect_to_AMI(asterisk_ami, asterisk_login, asterisk_pass)
            if AMIsock:
                l = usefulmsg.split()
                if l[0] == 'originate':
                    if AMIsock.originate(l[1], l[2]) == False:
                        del AMIsock
                        AMIsock = connect_to_AMI(asterisk_ami, asterisk_login, asterisk_pass)
                        if not AMIsock:
                            print "Asterisk seems to be down"
                        else:
                            if AMIsock.originate(l[1], l[2]) == False:
                                print "Strange"
                elif l[0] == 'transfer':
                    AMIsock.transfer(l[1], l[2])
                elif l[0] == 'hangup':
                    AMIsock.sendcommand('Status', [])
                    for ch in AMIsock.readresponse('StatusComplete'):
                        if ch.has_key('CallerID') and ch['CallerID'] == l[1]:
                            AMIsock.hangup(ch['Channel'])


# sends a SIP register + n x SIP subscribe messages
def do_sip_register_subscribe(cfg, l_sipsock, astid):
    global tcpopens, phonelists, sipnumlists
    for k in tcpopens:
        k[0].send("asterisk=will_register\n")
    command = sip_register(cfg, "sip:" + cfg.mysipname, 1, "reg_cid@xivopy")
    l_sipsock.sendto(command, (cfg.remoteaddr, port_sip_srv))
    for sipnum in sipnumlists[astid]:
        if (time.time() - phonelists[astid][sipnum].lasttime) > (2 * timeout_between_registers):
		if phonelists[astid][sipnum].status != "Timeout":
			phonelists[astid][sipnum].set_status("Timeout")
			for k in tcpopens:
				k[0].send("update=" + cfg.astid + ":" + sipnum + ":" + phonelists[astid][sipnum].status + "\n")
        command = sip_subscribe(cfg, "sip:" + cfg.mysipname, 1, "subscribexivo_" + sipnum + "@" + cfg.localaddr, sipnum)
        l_sipsock.sendto(command, (cfg.remoteaddr, port_sip_srv))

def update_sipnumlist(cfg, astid):
	global phonelists, sipnumlists
	sipnumlistold = sipnumlists[astid]
	sipnumlistnew = updateuserlistfromurl(cfg.userlisturl)
	sipnumlists[astid] = sipnumlistnew
	sipnumlistnew.sort()
	if sipnumlistnew != sipnumlistold:
		lstdel = ""
		lstadd = ""
		for snl in sipnumlistold:
			if snl not in sipnumlistnew:
				del phonelists[astid][sipnum] # or = "Absent"/0 ?
				lstdel += cfg.astid + ":" + snl + ";"
		for snl in sipnumlistnew:
			if snl not in sipnumlistold:
				phonelists[astid][snl] = LineProp()
				lstadd += cfg.astid + ":" + snl + ":" + phonelists[astid][snl].status + ";"
		for k in tcpopens:
			if lstdel != "":
				k[0].send("peerremove=" + lstdel + "\n")
			if lstadd != "":
				k[0].send("peeradd=" + lstadd + "\n")

def connect_to_AMI(address, login, lpass):
    AMIsock = AMI(address)
    try:
        AMIsock.connect()
        AMIsock.login(login, lpass)
        AMIsock.sendcommand('Status', [])
        AMIsock.readresponse('StatusComplete')
    except:
        del AMIsock
        AMIsock = False
    return AMIsock


class LineProp:
	def __init__(self):
		self.status = "BefSubs"
		self.lasttime = 0
	def set_status(self, istatus):
		self.status = istatus
	def set_lasttime(self, ilasttime):
		self.lasttime = ilasttime


class AsteriskRemote:
	def __init__(self, astid, localaddr = "127.0.0.1", remoteaddr = "127.0.0.1", portsipclt = 5080):
		self.userlisturl = "http://192.168.0.254/service/ipbx/sso.php"
		self.localaddr = localaddr
		self.remoteaddr = remoteaddr
		self.mysipname = "xivosb"
		self.portsipclt = portsipclt
		self.astid = astid

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

# opens the logfile for output
logfile = open("/var/log/sip_switchboard.log", 'a')

cfg1 = AsteriskRemote("clg")
cfg2 = AsteriskRemote("obelisk", "192.168.0.77", "192.168.0.254", 5082)
asterisk_ami = (cfg1.remoteaddr, port_ami)

cfgs = [cfg1, cfg2]
sipnumlists = [[], []]
phonelists = [{}, {}]
for val in xrange(4):
	sipnum = str(101 + val)
	sipnumlists[0].append(sipnum)
	phonelists[0][sipnum] = LineProp()

SIPsock1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
SIPsock1.bind(("", cfg1.portsipclt))
SIPsock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
SIPsock2.bind(("", cfg2.portsipclt))

UIsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
UIsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
UIsock.bind(("", port_ui_srv))
UIsock.listen(10)

tcpopens = []

AMIsock = connect_to_AMI(asterisk_ami, asterisk_login, asterisk_pass)

print "do_sip_register_subscribe (first)"
update_sipnumlist(cfg1, 0)
update_sipnumlist(cfg2, 1)
do_sip_register_subscribe(cfg1, SIPsock1, 0)
do_sip_register_subscribe(cfg2, SIPsock2, 1)
lastrequest_time = time.time()

ins = [SIPsock1, SIPsock2, UIsock]

# Receive messages
while True:
    [i, o, e] = select.select(ins, [], [], timeout_between_registers)
    if i:
        if SIPsock1 in i:
            [data1, addrsip1] = SIPsock1.recvfrom(bufsize)
            current_time = time.time()
            sp = parseSIP(cfg1, data1, SIPsock1, addrsip1, 0)
            if sp == 1:
                if __debug__:
                    print "do_sip_register_subscribe (parse SIP)"
		update_sipnumlist(cfg1, 0)
                do_sip_register_subscribe(cfg1, SIPsock1, 0)
                lastrequest_time = current_time
            if (current_time - lastrequest_time) > timeout_between_registers:
                if __debug__:
                    print "do_sip_register_subscribe (computed timeout)"
		update_sipnumlist(cfg1, 0)
                do_sip_register_subscribe(cfg1, SIPsock1, 0)
                lastrequest_time = current_time

	elif SIPsock2 in i:
            [data2, addrsip2] = SIPsock2.recvfrom(bufsize)
            current_time = time.time()
            sp = parseSIP(cfg2, data2, SIPsock2, addrsip2, 1)
            if sp == 1:
                if __debug__:
                    print "do_sip_register_subscribe (parse SIP)"
		update_sipnumlist(cfg2, 1)
                do_sip_register_subscribe(cfg2, SIPsock2, 1)
                lastrequest_time = current_time
            if (current_time - lastrequest_time) > timeout_between_registers:
                if __debug__:
                    print "do_sip_register_subscribe (computed timeout)"
		update_sipnumlist(cfg2, 1)
                do_sip_register_subscribe(cfg2, SIPsock2, 1)
                lastrequest_time = current_time

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
    else:
        lastrequest_time = time.time()
        print "do_sip_register_subscribe (select's timeout)"
	update_sipnumlist(cfg1, 0)
        do_sip_register_subscribe(cfg1, SIPsock1, 0)
	update_sipnumlist(cfg2, 1)
        do_sip_register_subscribe(cfg2, SIPsock2, 1)

# Close files and sockets
logfile.close()

