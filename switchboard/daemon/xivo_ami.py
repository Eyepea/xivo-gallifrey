#!/usr/bin/python
# $Revision: 341 $
# $Date: 2007-04-13 19:27:06 +0200 (ven, 13 avr 2007) $
#

import socket

bufsize_large = 8192

# logins into the Asterisk MI
def ami_socket_login(raddr, amiport, loginname, passname, events):
	try:
		sockid = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sockid.connect((raddr, amiport))
		sockid.recv(bufsize_large)
		# check against "Asterisk Call Manager/1.0\r\n"
		if events == 0:
			sockid.send("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + passname + "\r\nEvents: off\r\n\r\n")
		else:
			sockid.send("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + passname + "\r\nEvents: on\r\n\r\n")
		# check against "Message: Authentication accepted\r\n"
	except:
		sockid = -1
	return sockid

# sends a Status command
def ami_socket_status(sockid):
	"""Sends a Status command to the socket sockid"""
	try:
		sockid.send("Action: Status\r\n\r\n")
	except:
		if __debug__: print "failing to send command to sockid", sockid

##def ami_socket_originate
##def ami_socket_transfer
#### def ami_socket_reconnect
##def ami_socket_close
##def ami_socket_channeltypes

# sends a Hangup command
##def ami_socket_hangup(sockid):
##	sockid.send("Action: Command\r\n\r\n")


