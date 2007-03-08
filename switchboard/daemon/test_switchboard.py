#! /usr/bin/python
#
# 2007/03/01

#asterisk_address = ('localhost', 5038)
#asterisk_login = 'testuser'
#asterisk_pass = 'secretpass'
asterisk_address = ('192.168.0.254', 5038)
asterisk_login = 'sylvain'
asterisk_pass = 'sylvain'

import socket

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
			self.f.write(name + ': ' + value + '\r\n')
		self.f.write('\r\n')
		self.f.flush()
	def readresponsechunk(self):
		list = []
		while True:
			str = self.f.readline()
			#print self.i, str,
			self.i = self.i + 1
			if str == '\r\n' or str == '':
				break
			l = [ x.strip() for x in str.split(': ') ]
			if len(l) == 2:
				list.append((l[0], l[1]))
		return dict(list)
	def readresponse(self, check):
		first = self.readresponsechunk()
		#print first
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
	def execclicommand(self, command):
		# special procession for cli commands.
		self.sendcommand('Command', [('Command', command)])
		resp = []
		for i in (1, 2): str = self.f.readline()
		while True:
			str = self.f.readline()
			print self.i, len(str), str,
			self.i = self.i + 1
			if str == '\r\n' or str == '' or str == '--END COMMAND--\r\n':
				break
			resp.append(str)
		return resp
	def gethints(self):
		return filter(lambda x: len(x)==6, [s.strip().split() for s in a.execclicommand('show hints')])


a = AMI(asterisk_address)
a.connect()
a.login(asterisk_login, asterisk_pass)

#print [ s.strip().split() for s in a.execclicommand('show hints')]
print a.gethints()

# stuff server
# too bad : pas de sécurité !
import SocketServer
class ConnHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		global a
		str = self.rfile.readline().strip()
		try:
			if str=='hints':
				for e in a.gethints():
					self.wfile.write(e[0] + ',' + e[2] + ',' + e[3][6:] + ',' + e[5] + '\n')
			else:
				for s in a.execclicommand(str): self.wfile.write(s)
		except:
			# TODO : report errors in a better way
			print 'error!'

server = SocketServer.TCPServer(('', 8080), ConnHandler)
server.serve_forever()
import sys
sys.exit(0)

a.sendcommand('Status', [])
for x in a.readresponse('StatusComplete'): print x

peerlist = []
a.sendcommand('SIPpeers', [])
for x in a.readresponse('PeerlistComplete'):
	if x['Event'] == 'PeerlistComplete':
		break
	a.sendcommand('SIPshowpeer', [('Peer', x['ObjectName'])])
	peerlist.extend(a.readresponse('dummy'))

#print peerlist
for e in peerlist:
	print e['ObjectName'], e['Callerid'], e['Address-IP']

#for a in peerlist[9]:
#	print "%20s  %s" % (a, peerlist[9][a])


