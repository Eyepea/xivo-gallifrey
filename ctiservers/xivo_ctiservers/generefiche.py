#!/usr/bin/python

# XIVO Daemon
# Copyright (C) 2007, 2008  Proformatique
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, you will find one at
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.

# $Revision$
# $Date$

# Thomas Bernard
# generateur de fiche XML pour la remontee de fiche

import sys
import socket

# 1) instancier
# 2) ajouter des infos
# 3) utiliser getxml() ou sendtouser()

class Fiche:
    """Fiche class"""
    def __init__(self, sessionid=''):
            self.sessionid = sessionid
            self.infos = []
            self.message = ''
    def __str__(self):
            return "Fiche : sessionid=" + self.sessionid + " infos=" + str(self.infos)
    def __repr__(self):
            return str(self)
    def addinfo(self, name, type, value):
            """add a field in the profile"""
            self.infos.append( (name, type, value) )
    def setmessage(self, msg):
            """set the message to be displayed in systray message"""
            self.message = msg
    def getxml(self):
            """get a string containing the xml"""
            s = ['<?xml version="1.0" encoding="utf-8"?>']
            s.append('<profile sessionid="%s">' % self.sessionid)
            s.append('<user>')
            if len(self.message) > 0:
                    s.append('<message>%s</message>' % self.message)
            for (name, type, value) in self.infos:
                    s.append('<info name="%s" type="%s"><![CDATA[%s]]></info>' %(name, type, value))
            s.append('</user>')
            s.append('</profile>')
            retstring = ''.join(s) + '\n'
            return retstring
    def sendtouser(self, address, sheetui):
            """send the profile to a user using TCP"""
            try:
                    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    #print 'timeout=', s.gettimeout()
                    #s.settimeout(3.0)
                    s.connect(address)
                    fs = s.makefile('w')
                    s.close()
                    if sheetui is None:
                            fs.write(self.getxml())
                    else:
                            fs.write(sheetui)
                    fs.flush()
                    fs.close()
                    return True
            except Exception, e:
                    #print e
                    return False
