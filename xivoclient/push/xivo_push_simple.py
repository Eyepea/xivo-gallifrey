#! /usr/bin/python
# vim: set fileencoding=utf-8 :
#
# $Revision: 703 $
# $Date: 2007-05-23 17:13:57 +0200 (mer, 23 mai 2007) $
#
# Authors : Thomas Bernard, Corentin Le Gall, Sylvain Boily
#           Proformatique
#           67, rue Voltaire
#           92800 PUTEAUX
#           (+33/0)1.41.38.99.60
#           mailto:technique@proformatique.com
#           (C) 2007 Proformatique
#
# AGI de push de fiche
#

## \mainpage
# \section section_1 XIVO pusher AGI
#
## \file xivo_push
# \brief XIVO CTI pushing AGI
#
## \namespace xivo_push
# \brief XIVO CTI pushing AGI
#

CONFIG_FILE = '/etc/asterisk/xivo_agi.conf'
CONFIG_LIB_PATH = 'py_lib_path'

# XIVO modules
import sys
import socket
from xivo import ConfigPath
from xivo.ConfigPath import *
ConfiguredPathHelper(CONFIG_FILE, CONFIG_LIB_PATH)
import ConfigParser
from ConfigDict import *
from agi import *

agi = AGI()

## \brief Logs a message into the Asterisk CLI
# \param txt message to send to the CLI
def print_verbose(txt):
    agi.verbose("xivo_push : %s" % txt)

# ==============================================================================
# Main Code starts here
# ==============================================================================

if len(sys.argv) < 6:
	print "Usage :", sys.argv[0], "<server> <port> <proto> <user> <callerid> [<msgext>]"
	sys.exit(1)
else:
	shost = sys.argv[1]
	sport = int(sys.argv[2])
	proto = sys.argv[3]
	exten = sys.argv[4]
	callerid = sys.argv[5]
	user = proto + exten
	if len(sys.argv) > 6: msgext = sys.argv[6]
	else: msgext = ""

# send the PUSH command to the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((shost, sport))
fs = s.makefile('r')
s.send('PUSH ' + user + ' ' + callerid + ' ' + msgext + '\r\n')
#s.send('QUERY ' + user + '\r\n')
s.close()
list = fs.readline().strip().split(' ')
fs.close()
#print list
if len(list) < 4 or list[0] == 'ERROR':
	print_verbose("Could not localize user %s" % user)
	sys.exit(3)
else:
    # USER xxx STATE xxx
    clientstate = list[3]

# return status
if clientstate == "available":
	agi.set_variable('STATUS', 0)
elif clientstate == "away":
	agi.set_variable('STATUS', 1)
elif clientstate == "donotdisturb":
	agi.set_variable('STATUS', 2)
elif clientstate == "outtolunch":
	agi.set_variable('STATUS', 3)
elif clientstate == "berightback":
	agi.set_variable('STATUS', 4)
else:
	print_verbose("Unknown user's availability status : %s" % clientstate)

# print_verbose("availability is currently : %s" % clientstate)
