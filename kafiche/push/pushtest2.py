#! /usr/bin/python
# vim: set encoding=utf-8 :
# Thomas Bernard
# script de test du push de fiche

import socket
import sys
import generefiche

#print sys.argv[0], len(sys.argv)
if len(sys.argv) < 4:
	print "Usage :", sys.argv[0], "<server> <port> <user>"
	sys.exit(1)
else:
	shost = sys.argv[1]
	sport = int(sys.argv[2])
	user = sys.argv[3]

# first connect to the server to get ip, port of the client
z = generefiche.getuserlocation(shost, sport, user)
print z
if z == None:
	print "could not localize user", user
	sys.exit(3)
sessionid = z.get('sessionid')
clientaddress = z.get('address')

fiche = generefiche.Fiche(sessionid)
fiche.addinfo('Nom', 'text', 'Schiffer')
fiche.addinfo('Prénom', 'text', 'Claudia')
fiche.addinfo('Numéro', 'phone', '+33 1 42 42 42 42')
fiche.addinfo('Photo', 'picture', 'http://www.lesitedeclaudia.com/photos/claudia38.jpg')
fiche.addinfo('Adresse email', 'url', 'mailto:claudia.schiffer@claudia.com')
fiche.addinfo('Homepage', 'url', 'http://www.google.fr/')
fiche.addinfo('TestHtml', 'text', 'coucou les <b>Gars</b>')
fiche.addinfo('TestHtml2', 'text', '<table><tr><td>0</td><td>1</td></tr><tr><td>2</td><td>3</td></tr></table>')
#print fiche
#print fiche.getxml()

#connect to the client and send the stuff !
if fiche.sendtouser(clientaddress):
	print "Profile sent"
else:
	print "could not send profile to user"

