#!/usr/bin/python
# $Revision$
# $Date$
#

# functions in order to build SIP packets

import random

## \brief Builds a SIP REGISTER message.
# \param cfg the Asterisk properties
# \param me the SIP number
# \param cseq the CSeq to send
# \param callid the callerID to send
# \param expires the expiration time
# \return the built message
def sip_register(cfg, me, cseq, callid, expires, authentication):
    here = cfg.localaddr + ":" + str(cfg.portsipclt)
    raddr = cfg.remoteaddr
    #raddr = "192.168.0.255"
    command = "REGISTER sip:" + raddr + " SIP/2.0\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=" + str(random.randrange(1000000)) + "\r\n"
    command += "To: <" + me + "@" + raddr + ">\r\n"
    command += "From: <" + me + "@" + raddr + ">;tag=" + str(random.randrange(1000000)) + "\r\n"
    command += "Call-ID: " + callid + "\r\n"
    command += "CSeq: " + str(cseq) + " REGISTER\r\n"
    command += "Max-Forwards: 70\r\n"
    command += "Contact: <" + me + "@" + here + ">\r\n"
    command += authentication
    command += "User-Agent: Switchboard Watcher\r\n"
    command += "Expires: " + expires + "\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

## \brief Builds a SIP SUBSCRIBE message.
# \param cfg the Asterisk properties
# \param me the SIP number
# \param cseq the CSeq to send
# \param callid the callerID to send
# \param expires the expiration time
# \return the built message
def sip_subscribe(cfg, me, cseq, callid, sipnumber, expires, authentication):
    here = cfg.localaddr + ":" + str(cfg.portsipclt)
    command = "SUBSCRIBE sip:" + sipnumber + "@" + cfg.remoteaddr + " SIP/2.0\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=" + str(random.randrange(1000000)) + "\r\n"
    command += "To: <sip:" + sipnumber + "@" + cfg.remoteaddr + ">\r\n"
    command += "From: <" + me + "@" + cfg.remoteaddr + ">;tag=" + str(random.randrange(1000000)) + "\r\n"
    command += "Call-ID: " + callid + "\r\n"
    command += "CSeq: " + str(cseq) + " SUBSCRIBE\r\n"
    command += "Max-Forwards: 70\r\n"
    command += "Event: presence\r\n"
    command += "Accept: application/pidf+xml\r\n"
    command += "Contact: <" + me + "@" + here + ">\r\n"
    command += authentication
    command += "User-Agent: Switchboard Watcher\r\n"
    command += "Expires: " + expires + "\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

## \brief Builds a SIP OPTIONS message.
# \param cfg the Asterisk properties
# \param me the SIP number
# \param callid the callerID to send
# \param sipnumber the SIP numner
# \return the built message
def sip_options(cfg, me, callid, sipnumber):
    here = cfg.localaddr + ":" + str(cfg.portsipclt)
    raddr = cfg.remoteaddr
    command = "OPTIONS sip:" + sipnumber + "@" + raddr + " SIP/2.0\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=" + str(random.randrange(1000000)) + "\r\n"
    command += "From: <" + me + "@" + here + ">;tag=" + str(random.randrange(1000000)) + "\r\n"
    command += "To: <sip:" + sipnumber + "@" + raddr + ">\r\n"
    command += "Contact: <" + me + "@" + here + ">\r\n"
    command += "Call-ID: " + callid + "\r\n"
    command += "CSeq: 102 OPTIONS\r\n"
    command += "User-Agent: Switchboard Watcher\r\n"
    command += "Max-Forwards: 70\r\n"
    command += "Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, SUBSCRIBE, NOTIFY\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

## \brief Builds a SIP OK message (in order to reply to OPTIONS (qualify) and
# NOTIFY (when presence subscription)).
# \param cfg the Asterisk properties
# \param me the SIP number
# \param cseq the CSeq to send
# \param callid the callerID to send
# \return the built message
def sip_ok(cfg, me, cseq, callid, sipnumber, smsg, lbranch, ltag):
    here = cfg.localaddr + ":" + str(cfg.portsipclt)
    command = "SIP/2.0 200 OK\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=" + lbranch + "\r\n"
    command += "From: <" + sipnumber + "@" + cfg.remoteaddr + ">;tag=" + ltag + "\r\n"
    command += "To: <" + me + "@" + cfg.remoteaddr + ">\r\n"
    command += "Call-ID: " + callid + "\r\n"
    command += "CSeq: " + str(cseq) + " " + smsg + "\r\n"
    command += "User-Agent: Switchboard Watcher\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

