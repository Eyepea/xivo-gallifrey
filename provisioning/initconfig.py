#!/usr/bin/python
# -*- coding: iso-8859-15 -*-
"""Provisioning AGI for Xivo

Copyright (C) 2007, Proformatique

"""
# TODO WARNING: must be used only if the caller is of a SIP tech

import timeoutsocket
from timeoutsocket import Timeout

import sys, httplib

import provsup
from Phones import *

DEBUG	     = 1

# SERVER_IPV4 = provsup.LISTEN_IPV4
SERVER_IPV4 = '192.168.0.5'

HTTP_REQUEST_TIMEOUT = 90

def agi_escape_string(s):
	return s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', ' ')

def print_verbose(txt):
	if DEBUG:
		print "VERBOSE \"%s\"" % agi_escape_string(txt)

def return_exit(error, playback=None):
	print_verbose("%s" % error)
	if playback:
		print "EXEC PLAYBACK \"%s\"" % playback
	sys.exit(1)

# TODO: use an RFC compliant regexp instead of
# this stupid way of parsing things
def user_ipv4_from_sip_uri(sip_addr):
	splitted_sip = sip_addr.split(':')
	if len(splitted_sip) < 2:
		return None
	splitted_sip = splitted_sip[1].split('@')
	if len(splitted_sip) < 2:
		return None
	sip_user = splitted_sip[0]
	ip = splitted_sip[1].split('>')[0]
	return (sip_user, ip)

def phone_desc_by_ua(ua):
	for phone_class in provsup.PhoneClasses.itervalues():
		try:
			r = phone_class.get_vendor_model_fw(ua)
		except:
			r = None
			for line in provsup.exception_traceback():
				print_verbose(line)
		if r:
			return r
	return None

def main():
	if len(sys.argv) >= 4:
		print_verbose("Argument number ok")
	else:
		return_exit("Too few arguments")

	sip_uri = sys.argv[1]
	code = sys.argv[2]
	ua = sys.argv[3]

	# Get Sip User, IPv4 and Mac Address
	user_ipv4 = user_ipv4_from_sip_uri(sip_uri)
	if not user_ipv4:
		return_exit("Could not parse Sip URI \"%s\"" % sip_uri)
	sip_user, ipv4 = user_ipv4
	macaddr = provsup.macaddr_from_ipv4(ipv4, print_verbose)
	if not macaddr:
		return_exit("Could not find Mac Address from IPv4 \"%s\"" % ipv4)

	# Get Phone description (if we are able to handle this vendor...)
	phone_desc = phone_desc_by_ua(ua)
	if not phone_desc:
		return_exit("Unknown UA %s" % ua)
	phone_vendor = phone_desc[0]
	phone_model = phone_desc[1]

	if code == 'init':
		code = '0'
	if not provsup.well_formed_provcode(code):
		return_exit("Badly formed provisioning code", "privacy-incorrect")

	command = ( "mode=authoritative\r\nvendor=%s\r\nmodel=%s\r\n" + 
		    "macaddr=%s\r\nipv4=%s\r\nprovcode=%s\r\nactions=yes\r\n" + 
		    "proto=sip\r\n" ) % (phone_vendor, phone_model, macaddr, ipv4, code)

	try:
		timeoutsocket.setDefaultSocketTimeout(HTTP_REQUEST_TIMEOUT)
		conn = httplib.HTTPConnection(SERVER_IPV4 + ':' + str(provsup.LISTEN_PORT))
		conn.request("POST", "/prov", command, {"Content-Type": "text/plain; charset=ISO-8859-1"})
		response = conn.getresponse()
		response.read()	# eat every data sent by the provisioning server
		conn.close()
		reason = response.reason
		status = response.status
	except Exception, xcept:
		reason = str(xcept)
		status = 500
		for line in provsup.exception_traceback():
			print_verbose(line)

	if status != 200:
		return_exit("Provisioning failure; %s" % reason)

if __name__ == '__main__':
	main()
