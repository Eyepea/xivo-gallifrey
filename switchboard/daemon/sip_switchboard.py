#!/usr/bin/python
# $Id$
#

import xml.dom.minidom, xml
import socket, SocketServer
import os, posix, select, string, sys, time

port_ami     = 5038
port_sip_srv = 5060
port_sip_clt = 5080
port_ui_srv  = 5081

pidfile = '/tmp/sip_switchboard_id_daemon.pid'
bufsize = 1024
addr = ("", port_sip_clt)
#here_addr = "192.168.0.159"
here_addr = "127.0.0.1"
here = here_addr + ":" + str(port_sip_clt)
myname = "xivosb"
expires = "600"
dhost = os.sys.argv[1]
sipnumlist = []
for val in xrange(4):
    sipnumlist.append(101 + val)

asterisk_ami = (dhost, port_ami)
asterisk_sip = (dhost, port_sip_srv)
asterisk_login = 'sylvain'
asterisk_pass = 'sylvain'

# statuses :
# - Ready
# - On the phone
# - Ringing
# - Unavailable
# - 







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

# functions in order to build a SIP packet
# SIP SUBSCRIBE
def sip_subscribe(me, cseq, cid, sipnumber):
    rsrc = "sip:" + str(sipnumber)
    command = "SUBSCRIBE " + rsrc + "@" + dhost + " SIP/2.0\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=z9hG4bKnashds7\r\n"
    command += "To: <" + rsrc + "@" + dhost + ">\r\n"
    command += "From: <" + me + "@" + dhost + ">;tag=xfg9\r\n"
    command += "Call-ID: " + cid + "\r\n"
    command += "CSeq: " + str(cseq) + " SUBSCRIBE\r\n"
    command += "Max-Forwards: 70\r\n"
    command += "Event: presence\r\n"
    command += "Accept: application/pidf+xml\r\n"
    command += "Contact: <" + me + "@" + here + ">\r\n"
    command += "Expires: " + expires + "\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

# SIP OK (in order to reply to OPTIONS (qualify))
def sip_ok(me, cseq, cid, sipaddr, smsg):
    rsrc = "sip:" + sipaddr
    command = "SIP/2.0 200 OK\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=z9hG4bKnashds7\r\n"
    command += "From: <" + rsrc + "@" + dhost + ">;tag=xfg9\r\n"
    command += "To: <" + me + "@" + dhost + ">\r\n"
    command += "Call-ID: " + cid + "\r\n"
    command += "CSeq: " + str(cseq) + " " + smsg + "\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

# SIP REGISTER
def sip_register(me, cseq, cid):
    command = "REGISTER sip:" + dhost + " SIP/2.0\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=z9hG4bKnashds7\r\n"
    command += "To: <" + me + "@" + dhost + ">\r\n"
    command += "From: <" + me + "@" + dhost + ">;tag=xfg9\r\n"
    command += "Call-ID: " + cid + "\r\n"
    command += "CSeq: " + str(cseq) + " REGISTER\r\n"
    command += "Max-Forwards: 70\r\n"
    command += "Contact: <" + me + "@" + here + ">\r\n"
    command += "Expires: " + expires + "\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

# functions in order to read informations from a SIP packet
# reading of the status (200, 404, ...)
def sipstatus(data):
    ret = 0
    lines = data.split("\n")
    if lines[0].find("SIP/2.0") == 0:
        ret = int(lines[0].split(None)[1])
    return ret

# reading of the CSeq
# reading of the message type (REGISTER, OPTION, SUBSCRIBE, ...)
# reading of the callid
def read_sip_properties(data):
    cseq = 1
    msg = "xxx"
    cid = "no_callid@xivo"
    lines = data.split("\n")
    for x in lines:
        if x.find("CSeq") == 0:
            cseq = int(x.split(None)[1])
            msg = x.split(None)[2]
        elif x.find("From: ") == 0:
            bpart = x.split("<sip:")[1]
            address = bpart.split("@")[0]
        elif x.find("Call-ID:") == 0:
            cid = x.split(None)[1]
    return [cseq, msg, cid, address]


# converts the SIP message to a useful presence information
def saystatus(data):
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
            if x.find("On the phone") >= 0:
                t2 = "On_the_phone"
            if x.find("Ringing") >= 0:
                t2 = "Ringing"
            if x.find("Unavailable") >= 0:
                t2 = "Unavailable"
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
		self.f.write('Action: ' + action + '\r\n')
		for (name, value) in args:
#			print name + ': ' + value
			self.f.write(name + ': ' + value + '\r\n')
		self.f.write('\r\n')
		self.f.flush()
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
		AMIsock.sendcommand('Originate', [('Channel', 'SIP/'+src),
		                            ('Exten', dst),
		                            ('Context', 'local-extensions'),
									('Priority', '1'),
									('CallerID', 'Mise en relation <1911>'),
									('Async', 'true')])
#		print AMIsock.readresponse('')
		return True
	def transfer(self, src, dst):
		AMIsock.sendcommand('Status', [])
		for ch in AMIsock.readresponse('StatusComplete'):
#			print ch
			if ch.has_key('CallerID') and ch['CallerID'] == src and ch.has_key('Link'):
				AMIsock.redirect(ch['Link'], dst, 'local-extensions')


def build_statuses(sipstat):
    fullstat = "hints="
    sskeys = sipstat.keys()
    sskeys.sort()
    for ss in sskeys:
        fullstat += ss + ":" + sipstat[ss] + ";"
    fullstat += "\n"
    return fullstat


def parseSIP(data):
    global tcpopens, sipstatuses, SIPsock
    [icseq, imsg, icid, iaddr] = read_sip_properties(data)
    command = sip_ok("sip:" + myname, icseq, icid, iaddr, imsg)
    print "###", icseq, icid, iaddr, imsg
    SIPsock.sendto(command,(dhost, addr[1]))
    stat = saystatus(data)
    if stat != "???:????":
        sipstatuses[stat.split(":")[0]] = stat.split(":")[1]
        for k in tcpopens:
            k[0].send("update=" + stat + "\n")


def manage_connection(connid):
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
            connid[0].send(build_statuses(sipstatuses))
        elif usefulmsg != "" and is_ami_available:
            print usefulmsg
            l = usefulmsg.split()
            if l[0] == 'originate':
                AMIsock.originate(l[1], l[2])
            elif l[0] == 'transfer':
                AMIsock.transfer(l[1], l[2])
            elif l[0] == 'hangup':
                AMIsock.sendcommand('Status', [])
                for ch in AMIsock.readresponse('StatusComplete'):
                    if ch.has_key('CallerID') and ch['CallerID'] == l[1]:
                        AMIsock.hangup(ch['Channel'])


# sends a SIP register message and waits for a reply or a 1s timeout
def do_sip_register(l_myname, l_sipsock):
    timeout_sip_register_reply = 1 # unit = second
    if __debug__:
        print "sending REGISTER :",
    command = sip_register("sip:" + l_myname, 1, "aaa@bbb")
    l_sipsock.sendto(command, asterisk_sip)

    rs = 0
    l_ins = [l_sipsock]
    while rs != 200 and rs != -100:
        [i, o, e] = select.select(l_ins, [], [], timeout_sip_register_reply)
        if i:
            if l_sipsock in i:
                [data, addr] = l_sipsock.recvfrom(bufsize)
                rs = sipstatus(data)
        else:
            rs = -100
    return rs


# sends a SIP register message and waits for a reply or a 1s timeout
def do_sip_subscribe(l_myname, l_sipsock, l_sipnum):
    timeout_sip_subscribe_reply = 1 # unit = second
    if __debug__:
        print "sending SUBSCRIBE for peer", l_sipnum, ":",
    command = sip_subscribe("sip:" + l_myname, 1, "cid_" + str(l_sipnum) + "@" + here_addr, l_sipnum)
    l_sipsock.sendto(command, asterisk_sip)

    rs = 0
    l_ins = [l_sipsock]
    while rs != 200 and rs != 404 and rs != -100:
        [i, o, e] = select.select(l_ins, [], [], timeout_sip_subscribe_reply)
        if i:
            if l_sipsock in i:
                [data, addr] = l_sipsock.recvfrom(bufsize)
                rs = sipstatus(data)
        else:
            rs = -100
    return rs


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

SIPsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
SIPsock.bind(addr)

UIsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
UIsock.bind(("",port_ui_srv))
UIsock.listen(10)

is_ami_available = False
tcpopens = []

# infinite main loop : re-registers and re-subscribes
while True:
    AMIsock = AMI(asterisk_ami)
    try:
        AMIsock.connect()
        AMIsock.login(asterisk_login, asterisk_pass)
        AMIsock.sendcommand('Status', [])
        AMIsock.readresponse('StatusComplete')
        is_ami_available = True
    except:
        print "AMI is unavalaible - may be Asterisk as well"
        is_ami_available = False

    for k in tcpopens:
        k[0].send("asterisk=will_register_again\n")
    regist = do_sip_register(myname, SIPsock)
    if __debug__:
        print "registered SIP account <" + myname + "> with status code", regist

    for k in tcpopens:
        k[0].send("asterisk=will_subscribe_again\n")
    sipstatuses = {}
    for sipnum in sipnumlist:
        ss = str(sipnum)
        sipstatuses[ss] = "BeforeSubscription"
        subsc = do_sip_subscribe(myname, SIPsock, sipnum)
        if subsc == 200:
            sipstatuses[ss] = "SubscriptionSucceeded"
        elif subsc == 404:
            sipstatuses[ss] = "SubscriptionFailed_404"
        elif subsc == -100:
            sipstatuses[ss] = "SubscriptionTimedOut"
        else:
            sipstatuses[ss] = "SubscriptionFailed_" + str(subsc)
        if __debug__:
            print "subscribed SIP account <" + myname + "> with status code", subsc
        # the statuses are updated only if there has been a problem with the subscription
        if subsc != 200:
            for k in tcpopens:
                k[0].send("update=" + ss + ":" + sipstatuses[ss] + "\n")

    # sockets monitored by select : AMIsock.f is useful in order to detect an Asterisk shutdown
    if is_ami_available:
        ins = [AMIsock.f, SIPsock, UIsock]
        amisf = AMIsock.f
    else:
        ins = [SIPsock, UIsock]
        amisf = 0
    askedtoquit = False

    # the previous TCP connections are added
    for conn in tcpopens:
        ins.append(conn[0])

    # Receive messages
    while not askedtoquit:
        [i, o, e] = select.select(ins, [], [], 10)
        if i:
            if SIPsock in i:
                [data, addr] = SIPsock.recvfrom(bufsize)
                parseSIP(data)
                # TBD : if parsing == NOTIFY/OPTION and status = -100 ==> askedtoquit = True
            elif UIsock in i:
                [conn, UIsockparams] = UIsock.accept()
                print "TCP socket opened on  ", UIsockparams[0], UIsockparams[1]
                # appending the opened socket to the ones watched
                ins.append(conn)
                tcpopens.append([conn, UIsockparams[0], UIsockparams[1]])
            elif amisf in i:
                print "AMI connection is broken : maybe Asterisk has just been stopped"
                for k in tcpopens:
                    k[0].send("asterisk=ami_down\n")
                AMIsock.f.close()
                askedtoquit = True
            else:
                for conn in tcpopens:
                    if conn[0] in i:
                        manage_connection(conn)
        else:
            askedtoquit = True

# Close files and sockets
logfile.close()

