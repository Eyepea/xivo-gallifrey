#!/usr/bin/python
"""Provisioning AGI for Xivo

Copyright (C) 2007, Proformatique

"""
# TODO WARNING: must be used only if the caller is of a SIP tech

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""
GETOPT_SHORTOPTS = 'c:'

import sys

# === BEGIN of early configuration handling, so that the sys.path can be altered
CONFIG_FILE = '/etc/xivo/provisioning.conf' # can be overridded by cmd line param
CONFIG_LIB_PATH = 'py_lib_path'
from getopt import getopt
from xivo import ConfigPath
from xivo.ConfigPath import *
def config_path():
	global CONFIG_FILE
	opts,args = getopt(sys.argv[1:], GETOPT_SHORTOPTS)
	sys.argv[1:] = args # strip options for legacy code behind
	for v in [v for k,v in opts if k == '-c']:
		CONFIG_FILE = v
	ConfiguredPathHelper(CONFIG_FILE, CONFIG_LIB_PATH)
config_path()
# === END of early configuration handling


# Loading personnal modules is possible from this point

import timeoutsocket
from timeoutsocket import Timeout

import httplib

import provsup
from provsup import ProvGeneralConf as pgc
from Phones import *

from agi import *
agi = AGI()

def agi_verbose_debug(txt):
	if pgc['debug_agi']:
		agi.verbose(txt)

def return_exit(error, playback=None):
	agi.verbose("%s" % error)
	if playback:
		agi.appexec("PLAYBACK", playback)
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
				agi.verbose(line)
			sys.exc_clear()
		if r:
			return r
	return None

def main():
	provsup.LoadConfig(CONFIG_FILE)
	if len(sys.argv) >= 4:
		agi_verbose_debug("Argument number ok")
	else:
		return_exit("Too few arguments")

	sip_uri = sys.argv[1]
	code = sys.argv[2]
	ua = sys.argv[3]
	isinalan = "1"

	# Get Sip User, IPv4 and Mac Address
	user_ipv4 = user_ipv4_from_sip_uri(sip_uri)
	if not user_ipv4:
		return_exit("Could not parse Sip URI \"%s\"" % sip_uri)
	sip_user, ipv4 = user_ipv4
	macaddr = provsup.macaddr_from_ipv4(ipv4, agi.verbose)
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
		    "proto=sip\r\nisinalan=%s\r\n" ) % (phone_vendor, phone_model,
							macaddr, ipv4, code, isinalan)

	try:
		timeoutsocket.setDefaultSocketTimeout(pgc['http_request_to_s'])
		conn = httplib.HTTPConnection(pgc['connect_ipv4'] + ':' + str(pgc['connect_port']))
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
			agi.verbose(line)
		sys.exc_clear()

	if status != 200:
		return_exit("Provisioning failure; %s" % reason)

if __name__ == '__main__':
	main()
