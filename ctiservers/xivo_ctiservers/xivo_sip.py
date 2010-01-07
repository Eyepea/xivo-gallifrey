#!/usr/bin/python
# $Date$
"""
Functions in order to build SIP packets.
Copyright (C) 2007-2010 Proformatique <technique@proformatique.com>
"""

# XIVO Daemon
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
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
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

__version__ = "$Revision$ $Date$"

import random

## \brief Builds a SIP REGISTER message.
# \param cfg the Asterisk properties
# \param me the SIP number
# \param cseq the CSeq to send
# \param callid the callerID to send
# \param expires the expiration time
# \return the built message
def sip_register(cfg, me, cseq, callid, expires, authentication):
        here = "%s:%d" %(cfg.localaddr, cfg.portsipclt)
        raddr = cfg.remoteaddr
        # raddr = "192.168.0.255"
        try:
                command = ["REGISTER sip:%s SIP/2.0"       %(raddr),
                           "Via: SIP/2.0/UDP %s;branch=%s" %(here, str(random.randrange(1000000))),
                           "To: <sip:%s@%s>"               %(me, raddr),
                           "From: <sip:%s@%s>;tag=%s"      %(me, raddr, str(random.randrange(1000000))),
                           "Call-ID: %s"                   %(callid),
                           "CSeq: %d REGISTER"             %(cseq),
                           "Max-Forwards: 70",
                           "Contact: <sip:%s@%s>"          %(me, here)]
                if authentication is not "" : command.append(authentication)
                command.extend(("User-Agent: Switchboard Watcher",
                                "Expires: %d"                   %(expires),
                                "Content-Length: 0"))
                command.append("\r\n")
                return '\r\n'.join(command)
        except Exception, exc:
                print "sip_register : exception occured in %s" %str(exc)
                return '\r\n'


## \brief Builds a SIP SUBSCRIBE message.
# \param cfg the Asterisk properties
# \param me the SIP number
# \param cseq the CSeq to send
# \param callid the callerID to send
# \param expires the expiration time
# \return the built message
def sip_subscribe(cfg, me, cseq, callid, sipnumber, expires, authentication):
        here = "%s:%d" %(cfg.localaddr, cfg.portsipclt)
        raddr = cfg.remoteaddr
        try:
                command = ["SUBSCRIBE sip:%s@%s SIP/2.0"   %(sipnumber, raddr),
                           "Via: SIP/2.0/UDP %s;branch=%s" %(here, str(random.randrange(1000000))),
                           "To: <sip:%s@%s>"               %(sipnumber, raddr),
                           "From: <sip:%s@%s>;tag=%s"      %(me, raddr, str(random.randrange(1000000))),
                           "Call-ID: %s"                   %(callid),
                           "CSeq: %d SUBSCRIBE"            %(cseq),
                           "Max-Forwards: 70",
                           "Event: presence",
                           "Accept: application/pidf+xml",
                           "Contact: <sip:%s@%s>"          %(me, here)]
                if authentication is not "" : command.append(authentication)
                command.extend(("User-Agent: Switchboard Watcher",
                                "Expires: %d"                   %(expires),
                                "Content-Length: 0"))
                command.append("\r\n")
                return '\r\n'.join(command)
        except Exception, exc:
                print "sip_subscribe : exception occured in %s" %str(exc)
                return '\r\n'


## \brief Builds a SIP OPTIONS message.
# \param cfg the Asterisk properties
# \param me the SIP number
# \param callid the callerID to send
# \param sipnumber the SIP numner
# \return the built message
def sip_options(cfg, me, callid, sipnumber):
        here = "%s:%d" %(cfg.localaddr, cfg.portsipclt)
        raddr = cfg.remoteaddr
        try:
                command = ["OPTIONS sip:%s@%s SIP/2.0"     %(sipnumber, raddr),
                           "Via: SIP/2.0/UDP %s;branch=%s" %(here, str(random.randrange(1000000))),
                           "From: <sip:%s@%s>;tag=%s"      %(me, here, str(random.randrange(1000000))),
                           "To: <sip:%s@%s>"               %(sipnumber, raddr),
                           "Call-ID: %s"                   %(callid),
                           "CSeq: 102 OPTIONS",
                           "User-Agent: Switchboard Watcher",
                           "Max-Forwards: 70",
                           "Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, SUBSCRIBE, NOTIFY",
                           "Content-Length: 0"]
                command.append("\r\n")
                return '\r\n'.join(command)
        except Exception, exc:
                print "sip_options : exception occured in %s" %str(exc)
                return '\r\n'


## \brief Builds a SIP OK message (in order to reply to OPTIONS (qualify) and
# NOTIFY (when presence subscription)).
# \param cfg the Asterisk properties
# \param me the SIP number
# \param cseq the CSeq to send
# \param callid the callerID to send
# \return the built message
def sip_ok(cfg, me, cseq, callid, smsg, lbranch, ltag):
        here = "%s:%d" %(cfg.localaddr, cfg.portsipclt)
        raddr = cfg.remoteaddr
        try:
                command = ["SIP/2.0 200 OK",
                           "Via: SIP/2.0/UDP %s;branch=%s" %(here, lbranch),
                           "From: <sip:%s@%s>;tag=%s"      %(me, raddr, ltag),
                           "To: <sip:%s@%s>"               %(me, raddr),
                           "Call-ID: %s"                   %(callid),
                           "CSeq: %d %s"                   %(cseq, smsg),
                           "User-Agent: Switchboard Watcher",
                           "Content-Length: 0"]
                command.append("\r\n")
                return '\r\n'.join(command)
        except Exception, exc:
                print "sip_ok : exception occured in %s" %str(exc)
                return '\r\n'
