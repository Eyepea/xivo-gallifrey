#! /usr/bin/python
#
# author : Thomas Bernard
# last modified : 2007/03/01

# still to do : timer to clean state of not connected clients ?

# configuration options :
port_login = 12345
port_keepalive = port_login + 1
port_request = 12347
session_expiration_time = 60*1
#userlisturl = 'http://192.168.0.5/service/ipbx/sso.php'
userlisturl = 'http://192.168.0.254/service/ipbx/sso.php'
pidfile = '/tmp/kafiche_id_daemon.pid'

# imported packages
import socket
import SocketServer
import select
import random
import time
import threading
import signal
import sys
import os
import pickle
#
import urllib

# global : userlist
# liste des champs :
#  user :             user name
#  passwd :           password
#  sessionid :        session id generated at connection
#  sessiontimestamp : last time when the client proved itself to be ALIVE :)
#  ip :               ip address of the client (current session)
#  port :             port here the client is listening.
#  state :            available, away, donotdisturb
#                     (??online, not-available, busy)
# The user identifier will likely be its phone number

# user list initialized empty
userlist = {}

userlist_lock = threading.Condition()

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

# TODO: a method to fill user list from a file or a db
#       or asterisk or something :)
def filluserlistfromfile(fname):
	f = open(fname)
	try:
		for line in f:
			# remove leading/tailing whitespaces
			line = line.strip()
			if line[0] != '#':
				l = line.split(':')
				if __debug__:
					print 'user', l[0], 'password', l[1]
				adduser(l[0], l[1])
	finally:
		f.close()

# fill the userlist from a url which is likely to be HTTP :
# http://adc.xivo.pro/service/ipbx/sso.php
def filluserlistfromurl(url):
	f = urllib.urlopen(url)
	try:
		for line in f:
			# remove leading/tailing whitespaces
			line = line.strip()
			l = line.split('|')
			# line is protocol|phone|password|rightflag
			if __debug__:
				print 'user', l[0], l[1] , 'password', l[2], 'droit', l[3]
			if l[3] != '0':
				adduser(l[0]+l[1], l[2])
	finally:
		f.close()

def updateuserlistfromurl(url):
	f = urllib.urlopen(url)
	try:
		for line in f:
			# remove leading/tailing whitespaces
			line = line.strip()
			l = line.split('|')
			# line is protocol|phone|password|rightflag
			if __debug__:
				print 'user', l[0], l[1] , 'password', l[2], 'droit', l[3]
			if l[3] == '0':
				deluser(l[0]+l[1])
			else:
				adduser(l[0]+l[1], l[2])
	finally:
		f.close()

# the following function is for testing
def filluserlist():
	adduser('zorro', 'garcia')
	adduser('bernardo', 'delavega')
	adduser('toto', 'zut')


# finduser() returns the user from the list.
# None is returned if not found
def finduser(user):
	return userlist.get(user)

# dump the user list to the standard output.
# TODO: we could need to dump that to a file using "pickle"
def dumpuserlist():
	print userlist


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
		global userlisturl
		if __debug__:
			print 'LoginHandler'
			print '  client connected :', self.client_address
		#print '  request :', self.request
		list = self.rfile.readline().strip().split(' ')
		if len(list) != 2 or list[0] != 'LOGIN':
			self.wfile.write('ERROR\r\n')
			return
		user = list[1]
		self.wfile.write('Send PASS for authentification\r\n')
		list = self.rfile.readline().strip().split(' ')
		if len(list) != 2 or list[0] != 'PASS':
			self.wfile.write('ERROR\r\n')
			return
		passwd = list[1]
		#print 'user/pass : ' + user + '/' + passwd
		userlist_lock.acquire()
		updateuserlistfromurl(userlisturl)
		e = finduser(user)
		goodpass = (e != None) and (e.get('passwd') == passwd)
		userlist_lock.release()
		if not goodpass:
			self.wfile.write('ERROR : WRONG LOGIN PASSWD\r\n')
			return
		self.wfile.write('Send PORT command\r\n')
		list = self.rfile.readline().strip().split(' ')
		if len(list) != 2 or list[0] != 'PORT':
			self.wfile.write('ERROR\r\n')
			return
		port = list[1]
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
		if __debug__:
			print userlist


# IdentRequestHandler: give client identification to the profile pusher
# the connection is kept alive so several requests can be made on the 
# same open TCP connection.
class IdentRequestHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		if __debug__:
			print 'IdentRequestHandler'
			print '  client : ', self.client_address
		while True:
			list = self.rfile.readline().strip().split(' ')
			retline = 'ERROR\r\n'
			if list[0] == 'QUERY' and len(list) == 2:
				user = list[1]
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
					print 'Exception :', e
				return

# The KeepAliveHandler receives UDP datagrams and sends back 
# a datagram containing whether "OK" or "ERROR <error-text>"
# It could be a good thing to give a numerical code to each error.
class KeepAliveHandler(SocketServer.DatagramRequestHandler):
	def handle(self):
		if __debug__:
			print 'KeepAliveHandler'
			print '  client : ', self.client_address
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
					print user, e['user']
					#print sessionid, e['sessionid']
					#print ip, e['ip']
					#print timestamp, e['sessiontimestamp']
					print timestamp - e['sessiontimestamp']
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

class MyTCPServer(SocketServer.ThreadingTCPServer):
	allow_reuse_address = True

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

#filluserlist()
#filluserlistfromfile('users')
filluserlistfromurl(userlisturl)
#dumpuserlist()

# Instanciate the SocketServer Objects.
loginserver = MyTCPServer(('', port_login), LoginHandler)
# TODO: maybe we should listen on only one interface (localhost ?)
requestserver = MyTCPServer(('', port_request), IdentRequestHandler)
# Do we need a Threading server for the keep alive ? I dont think so,
# packets processing is non blocking so thead creation/start/stop/delete
# overhead is not worth it.
#keepaliveserver = SocketServer.ThreadingUDPServer(('', port_keepalive), KeepAliveHandler)
keepaliveserver = SocketServer.UDPServer(('', port_keepalive), KeepAliveHandler)

# We have three sockets to listen to so we cannot use the 
# very easy to use SocketServer.serve_forever()
# So select() is what we need. The SocketServer.handle_request() calls
# won't block the execution. In case of the TCP servers, they will
# spawn a new thread, in case of the UDP server, the request handling
# process should be fast. If it isnt, use a threading UDP server ;)

# ins is the socket set we are waiting events on.
ins = [loginserver.socket, requestserver.socket, keepaliveserver.socket]
if __debug__:
	print ins

askedtoquit = False

# usefull signals are catched here (in the main thread)
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
#pickle.dump(userlist, sys.stdout)
sys.exit(0)

