#!/usr/bin/python
# $Date$
"""
Asterisk AMI utilities.
Copyright (C) 2007, Proformatique
"""

__version__ = "$Revision$ $Date$"

import socket

BUFSIZE_LARGE = 8192

## \brief Logins into the Asterisk Manager Interface.
def ami_socket_login(raddr, amiport, loginname, passname, events):
	try:
		sockid = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sockid.connect((raddr, amiport))
		sockid.recv(BUFSIZE_LARGE)
		# check against "Asterisk Call Manager/1.0\r\n"
		if events == True:
			sockid.send("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + passname + "\r\nEvents: on\r\n\r\n")
		else:
			sockid.send("Action: login\r\nUsername: " + loginname + "\r\nSecret: " + passname + "\r\nEvents: off\r\n\r\n")
		# check against "Message: Authentication accepted\r\n"
	except:
		sockid = -1
	return sockid


## \brief Sends a Status request to the AMI.
def ami_socket_status(sockid):
	"""Sends a Status command to the socket sockid"""
	try:
		sockid.send("Action: Status\r\n\r\n")
		return True
	except:
		return False

## \brief Sends a Command to the AMI.
def ami_socket_command(sockid, command):
	"""Sends a <command> command to the socket sockid"""
	try:
		sockid.send("Action: Command\r\nCommand: " + command + "\r\n\r\n")
		return True
	except:
		return False

