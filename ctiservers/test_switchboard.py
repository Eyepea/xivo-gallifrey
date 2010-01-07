#! /usr/bin/python

# XIVO Daemon
# Copyright (C) 2007-2010  Proformatique <technique@proformatique.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Alternatively, XIVO Daemon is available under other licenses directly
# contracted with Pro-formatique SARL. See the LICENSE file at top of the
# source tree or delivered in the installable package in which XIVO Daemon
# is distributed for more details.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# $Revision$
# $Date$

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
	def gethints(self):
		return filter(lambda x: len(x)==6, [s.strip().split() for s in a.execclicommand('show hints')])
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

a = AMI(asterisk_address)
a.connect()
a.login(asterisk_login, asterisk_pass)

#print [ s.strip().split() for s in a.execclicommand('show hints')]
#print a.gethints()

#print a.redirect('test', '123')


a.sendcommand('Status', [])
for x in a.readresponse('StatusComplete'):
	print x
	#if x.has_key('CallerIDName') and x['CallerIDName'] == 'Thinot Benoit':
	#	print "Redirecting", a.redirect(x['Channel'], '109', 'local-extensions')
	#if x.has_key('CallerID') and x['CallerID'] == '104':
	#	print "Redirecting", a.redirect(x['Channel'], '109', 'local-extensions')

#import time
#time.sleep(1)
#print '---- after redirecting !!!! --------------'

#a.sendcommand('Status', [])
#for x in a.readresponse('StatusComplete'):
#	print x
# stuff server
# too bad : pas de sécurité !
import SocketServer
class ConnHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		global a
		str = self.rfile.readline().strip()
		try:
			l = str.split()
			if str=='hints':
				for e in a.gethints():
					self.wfile.write(e[0] + ',' + e[2] + ',' + e[3][6:] + ',' + e[5] + '\n')
			elif l[0] == 'originate':
				a.originate(l[1], l[2])
			elif l[0] == 'transfer':
				a.transfer(l[1], l[2])
			elif l[0] == 'hangup':
				a.sendcommand('Status', [])
				for ch in a.readresponse('StatusComplete'):
					if ch.has_key('CallerID') and ch['CallerID'] == l[1]:
						a.hangup(ch['Channel'])
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


