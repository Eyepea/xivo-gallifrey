#!/usr/bin/python
# $Id$

# functions in order to build SIP packets

import random

# SIP REGISTER
def sip_register(cfg, me, cseq, callid, expires):
    here = cfg.localaddr + ":" + str(cfg.portsipclt)
    command = "REGISTER sip:" + cfg.remoteaddr + " SIP/2.0\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=" + str(random.randrange(1000000)) + "\r\n"
    command += "To: <" + me + "@" + cfg.remoteaddr + ">\r\n"
    command += "From: <" + me + "@" + cfg.remoteaddr + ">;tag=" + str(random.randrange(1000000)) + "\r\n"
    command += "Call-ID: " + callid + "\r\n"
    command += "CSeq: " + str(cseq) + " REGISTER\r\n"
    command += "Max-Forwards: 70\r\n"
    command += "Contact: <" + me + "@" + here + ">\r\n"
    command += "User-Agent: Switchboard Watcher $Revision$\r\n"
    command += "Expires: " + expires + "\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

# SIP SUBSCRIBE
def sip_subscribe(cfg, me, cseq, callid, sipnumber, expires):
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
    command += "User-Agent: Switchboard Watcher $Revision$\r\n"
    command += "Expires: " + expires + "\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

# SIP OPTIONS
def sip_options(cfg, me, callid, sipnumber):
    here = cfg.localaddr + ":" + str(cfg.portsipclt)
    command = "OPTIONS sip:" + sipnumber + "@" + cfg.remoteaddr + " SIP/2.0\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=" + str(random.randrange(1000000)) + "\r\n"
    command += "From: <" + me + "@" + here + ">;tag=" + str(random.randrange(1000000)) + "\r\n"
    command += "To: <sip:" + sipnumber + "@" + cfg.remoteaddr + ">\r\n"
    command += "Contact: <" + me + "@" + here + ">\r\n"
    command += "Call-ID: " + callid + "\r\n"
    command += "CSeq: 102 OPTIONS\r\n"
    command += "User-Agent: Switchboard Watcher $Revision$\r\n"
    command += "Max-Forwards: 70\r\n"
    command += "Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, SUBSCRIBE, NOTIFY\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

# SIP OK (in order to reply to OPTIONS (qualify) and NOTIFY (when presence subscription))
def sip_ok(cfg, me, cseq, callid, sipnumber, smsg, lbranch, ltag):
    here = cfg.localaddr + ":" + str(cfg.portsipclt)
    command = "SIP/2.0 200 OK\r\n"
    command += "Via: SIP/2.0/UDP " + here + ";branch=" + lbranch + "\r\n"
    command += "From: <" + sipnumber + "@" + cfg.remoteaddr + ">;tag=" + ltag + "\r\n"
    command += "To: <" + me + "@" + cfg.remoteaddr + ">\r\n"
    command += "Call-ID: " + callid + "\r\n"
    command += "CSeq: " + str(cseq) + " " + smsg + "\r\n"
    command += "User-Agent: Switchboard Watcher $Revision$\r\n"
    command += "Content-Length: 0\r\n"
    command += "\r\n"
    return command

