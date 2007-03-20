#!/usr/bin/python
# $Id$

import xml.dom.minidom, xml
import socket, SocketServer
import os, posix, select, string, sys, time

port_sip_clt = 5080
port_sip_srv = 5060
bufsize = 1024
addr = ("", port_sip_clt)
#here = "192.168.0.159" + ":" + str(port_sip_clt)
here = "127.0.0.1" + ":" + str(port_sip_clt)
myname = "xivosb"
expires = "600"
dhost = os.sys.argv[1]

asterisk_address = ('192.168.0.77', 5038)
asterisk_login = 'sylvain'
asterisk_pass = 'sylvain'

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

def sipstatus(data):
    ret = 0
    lines = data.split("\n")
    if lines[0].find("SIP/2.0") == 0:
        ret = int(lines[0].split(None)[1])
    return ret

def sipcseq(data):
    cseq = 1
    lines = data.split("\n")
    for x in lines:
        if x.find("CSeq") == 0:
            cseq = int(x.split(None)[1])
    return cseq

def sipmsg(data):
    cseq = "xxx"
    lines = data.split("\n")
    for x in lines:
        if x.find("CSeq") == 0:
            cseq = x.split(None)[2]
    return cseq

def sipcallid(data):
    cid = "aab@bbb"
    lines = data.split("\n")
    for x in lines:
        if x.find("Call-ID:") == 0:
            cid = x.split(None)[1]
    return cid

def saystatus(data):
    t1 = "???"
    t2 = "????"
    lines = data.split("\n")
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
		s.connect(asterisk_address)
		self.f = s.makefile()
		s.close()
		str = self.f.readline()
		print str,
	def sendcommand(self, action, args):
		self.f.write('Action: ' + action + '\r\n')
		for (name, value) in args:
			print name + ': ' + value
			self.f.write(name + ': ' + value + '\r\n')
		self.f.write('\r\n')
		self.f.flush()
	def printresponse_forever(self):
		# for debug
		while True:
			str = self.f.readline()
			print self.i, len(str), str,
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
		a.sendcommand('Originate', [('Channel', 'SIP/'+src),
		                            ('Exten', dst),
		                            ('Context', 'local-extensions'),
									('Priority', '1'),
									('CallerID', 'Mise en relation <1911>'),
									('Async', 'true')])
		print a.readresponse('')
		return True
	def transfer(self, src, dst):
		a.sendcommand('Status', [])
		for ch in a.readresponse('StatusComplete'):
			print ch
			if ch.has_key('CallerID') and ch['CallerID'] == src and ch.has_key('Link'):
				a.redirect(ch['Link'], dst, 'local-extensions')


def build_statuses(sipstat):
    fullstat = "hints="
    sskeys = sipstat.keys()
    sskeys.sort()
    for ss in sskeys:
        fullstat += ss + ":" + sipstat[ss] + ";"
    fullstat += "\n"
    return fullstat


def parseSIP(data):
    global tcpopens, sipstatuses, UDPSock
    sxml = ""
    ind = 0
    for x in data.split("\n"):
        if x.find("<?xml") == 0:
            ind = 1
        if ind == 1:
            sxml += x + "\n"
        if x.find("From: ") == 0:
            if __debug__:
                print x.split("<sip:")[1]
            add1 = x.split("@")[0]
##    if sxml != "":
##        axml = xml.dom.minidom.parseString(sxml)
##        docu = axml.getElementsByTagName('note')
##        for as in docu:
##            print as
    command = sip_ok("sip:" + myname, sipcseq(data), sipcallid(data), add1, sipmsg(data))
    UDPSock.sendto(command,(dhost, addr[1]))
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
        elif usefulmsg == "":
            if __debug__:
                print "no command"
        else:
#            print usefulmsg
            l = usefulmsg.split()
            if l[0] == 'originate':
                a.originate(l[1], l[2])
            elif l[0] == 'transfer':
                a.transfer(l[1], l[2])
            elif l[0] == 'hangup':
                a.sendcommand('Status', [])
                for ch in a.readresponse('StatusComplete'):
                    if ch.has_key('CallerID') and ch['CallerID'] == l[1]:
                        a.hangup(ch['Channel'])
#            else:
#                for s in a.execclicommand(usefulmsg): connid[0].send(s)


# ==============================================================================
# Main Code starts here
# ==============================================================================

UDPSock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
UDPSock.bind(addr)

TCPSock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
TCPSock.bind(("",5081))
TCPSock.listen(10)

if __debug__:
    print "-- REGISTER --"
command = sip_register("sip:" + myname, 1, "aaa@bbb")
UDPSock.sendto(command,(dhost, port_sip_srv))
rs = 0
while rs != 200:
    data,addr = UDPSock.recvfrom(bufsize)
    rs = sipstatus(data)

sipstatuses = {}
sipnumlist = []
for val in xrange(4):
    sipnumlist.append(101 + val)

for sipnum in sipnumlist:
    ss = str(sipnum)
    sipstatuses[ss] = "none"
    UDPSock.sendto(sip_subscribe("sip:" + myname, 1, "cid_" + str(sipnum) + "@xivo", sipnum),
                   (dhost, port_sip_srv))
    rs = 0
    while rs != 200 and rs != 404:
        data,addr = UDPSock.recvfrom(bufsize)
        rs = sipstatus(data)
    if __debug__:
        if rs == 404:
            print "-- SUBSCRIBE -- :", sipnum, "NOT POSSIBLE (received", str(rs), ")"
        else:
            print "-- SUBSCRIBE -- :", sipnum, "registered (received", str(rs), ")"



a = AMI(asterisk_address)
a.connect()
a.login(asterisk_login, asterisk_pass)
a.sendcommand('Status', [])
for x in a.readresponse('StatusComplete'):
	print x

# test somewhere whether addr[0] == dhost

ins = [UDPSock, TCPSock]
tcpopens = []

# Receive messages
while 1:
    i,o,e = select.select(ins, [], [], 0)
    if i:
        if UDPSock in i:
            data,addr = UDPSock.recvfrom(bufsize)
            parseSIP(data)
        elif TCPSock in i:
            conn,addrTCP = TCPSock.accept()
            print "TCP socket opened on  ", addrTCP[0], addrTCP[1]
            ins.append(conn)
            tcpopens.append([conn, addrTCP[0], addrTCP[1]])
        else:
            for conn in tcpopens:
                if conn[0] in i:
                    manage_connection(conn)
