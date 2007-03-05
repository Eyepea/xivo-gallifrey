#!/usr/bin/python
# $Id$
#
# Client program

import os, sys, posix, string, pexpect
from socket import * 

s = socket(AF_INET,SOCK_DGRAM) # DGRAM = UDP

dhost = os.sys.argv[1]
command = chr(0) + chr(0) + chr(0) + chr(0) + chr(0) + chr(0) + chr(0) + chr(0)

ports=5050

s.bind(('',0))                 # '' hote local, 0 port choisi
s.sendto(command,(dhost,ports))  # envoi
s.close()

