#! /usr/bin/python
# vim: set fileencoding=utf-8 :
#
# $Revision: 703 $
# $Date: 2007-05-23 17:13:57 +0200 (mer, 23 mai 2007) $
#
# Authors : Thomas Bernard, Corentin Le Gall
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

import sys
# XIVO modules
import socket

## \brief Secures an output string for the AGI VERBOSE
# \param s string to secure
def agi_escape_string(s):
    return s.replace('\\', '\\\\').replace('"', '\\"').replace('\n', ' ')

## \brief Logs a message into the Asterisk CLI
# \param txt message to send to the CLI
def print_verbose(txt):
    print "VERBOSE \"xivo_push : %s\"" % agi_escape_string(txt)

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
	print_verbose("Could not localize user %s" %user)
	sys.exit(3)
else:
    # USER xxx STATE xxx
    clientstate = list[3]

# return status
if clientstate == "available":
	print "SET VARIABLE STATUS 0"
elif clientstate == "away":
	print "SET VARIABLE STATUS 1"
elif clientstate == "donotdisturb":
	print "SET VARIABLE STATUS 2"
elif clientstate == "outtolunch":
	pass
	#print "SET VARIABLE STATUS 2"
elif clientstate == "berightback":
	pass
	#print "SET VARIABLE STATUS 2"
else:
	print_verbose("Unknown user's availability status : %s" %clientstate)

#print "availability is currently :", clientstate

sys.stdout.flush()
sys.stderr.flush()

