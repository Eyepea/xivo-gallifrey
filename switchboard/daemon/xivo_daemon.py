#!/usr/bin/python
# $Revision$
# $Date$
#
# Authors : Thomas Bernard, Corentin Le Gall, Benoit Thinot, Guillaume Knispel
#           Proformatique
#           67, rue Voltaire
#           92800 PUTEAUX
#           (+33/0)1.41.38.99.60
#           mailto:technique@proformatique.com
#           (C) 2007 Proformatique
#

## \mainpage
# \section section_1 General description of XIVO Daemon
# The XIVO Daemon aims to monitor all the actions taking place into one or
# more Asterisk servers, in order to provide 2 basic customer facilities :
# - a monitoring switchboard;
# - a customer information popup.
#
# This is achieved thanks to 3 mechanisms :
# - a notification of the Asterisk hints through SIP NOTIFY messages;
# - one or more connections to the Asterisk Manager Interface (AMI), where
# all the events can be watched;
# - Asterisk AGI's that send informations when a call is issued.
#
# This daemon is able to manage any number of Asterisk's one might wish.
#
# \section section_2 Initializations
#
# - Fetch the phone number lists from the SSO addresses.
# - Sending the first SIP REGISTERs and SUBSCRIBEs
#
# \section section_3 Main loop
# The main loop is triggered by a select() on the file descriptors for :
# - the SIP sockets (SIPsocks);
# - the AMI Event sockets (AMIsocks);
# - the UI sockets (UIsock, PHPUIsock);
# - the Caller Information Popup sockets (authentication, keepalive and identrequest).
#
# On reception of a SIP socket, parseSIP is called in order to either read SIP
# informations that might be useful for presence information, either to send
# a reply.
#
# On reception of AMI Events, handle_ami_event() parses the messages to update
# the detailed status of the channels.
#
# For each UI connection, a list of the open UI connections is updated
# (tcpopens_sb or tcpopens_php according to the kind of connection).
# This list is the one used to broadcast the miscellaneous updates
# (it is up to the UI clients to fetch the initial status with the "hints"
# command).
#
# \section section_5 Presence information from SIP/XML
# On startup, a given account (xivosb for instance) is SIP-REGISTERed for
# each Asterisk.
# This account then SIP-SUBSCRIBEs all the SIP phone numbers.
#
# The REGISTRation is done at 3 places :
#  - when a timeout occurs on the select() system-call, in order to guarantee that,
#  even when nothing occurs, the registration is properly done
#  - when an event has been triggered by select(), provided the given time has
#  elapsed, so that even in busy situations the registration is done
#  - when a SIP message is received, so that the waking up of an Asterisk
#  initiates a registration (if the "qualify" option is set)
#
# \section section_6 Monitoring with AMI
#
# The AMI events are the basis for a channel-by-channel status of the phones.
# The SIP/XML events do not carry enough information, however they are useful
# for when no channel is open.
#
# Many AMI events are watched for, but not all of them are handled yet.
# The most useful ones are now : Dial, Link, Hangup, Rename.
# The following ones : Newexten, Newchannel, Newcallerid, Newstate are useful when dealing
# complex situations (when there are Local/ channels and Queues for instance).
#
# \section section_8 Caller Information Popup management
#
# The daemon has 3 other listening sockets :
# - Login - TCP - (the clients connect to it to login)
# - KeepAlive - UDP - (the clients send datagram to it to inform
#                      of their current state)
# - IdentRequest - TCP - offer a service to ask for localization and
#                        state of the clients.
# we use the SocketServer "framework" to implement the "services"
# see http://docs.python.org/lib/module-SocketServer.html
#
# \section section_9 Data Structures
#
# The statuses of all the lines/channels are stored in the multidimensional array/dict "plist",
# which is an array of PhoneList.
#
# plist[astn].normal[phonenum].chann[channel] are objects of the class ChannelStatus.
# - astn is the Asterisk id
# - phonenum is the phone id (SIP/<xx>, IAX2/<yy>, ...)
# - channel is the full channel name as known by Asterisk
#
## \file xivo_daemon.py
# \brief XIVO CTI server
#
## \namespace xivo_daemon
# \brief XIVO CTI server
#

__version__ = "$Revision$ $Date$"

# debian.org modules
import ConfigParser
import encodings.utf_8
import getopt
import md5
import os
import random
import re
import select
import signal
import socket
import SocketServer
import sys
import syslog
import threading
import time
import urllib
import _sre

# fiche
import sendfiche

# XIVO lib-python modules initialization
from xivo import ConfigPath
from xivo.ConfigPath import *
xivoconffile            = "/etc/asterisk/xivo_daemon.conf"
GETOPT_SHORTOPTS        = 'dc:'
GETOPT_LONGOPTS         = ["daemon", "config="]
CONFIG_LIB_PATH         = 'py_lib_path'
def config_path():
        global xivoconffile
        for opt, arg in getopt.getopt(sys.argv[1:], "dc:", ["daemon", "config="])[0]:
                if opt == "-c":
                        xivoconffile = arg
        ConfiguredPathHelper(xivoconffile, CONFIG_LIB_PATH)
config_path()
debug_mode = (sys.argv.count('-d') > 0)

# XIVO lib-python modules imports
import daemonize
import anysql
from BackSQL import backmysql
from BackSQL import backsqlite

# XIVO modules
import xivo_ami
import xivo_sip
import xivo_ldap

DIR_TO_STRING = ">"
DIR_FROM_STRING = "<"
allowed_states = ["available", "away", "outtolunch", "donotdisturb", "berightback"]

DUMMY_DIR = ""
DUMMY_RCHAN = ""
DUMMY_EXTEN = ""
DUMMY_MYNUM = ""
DUMMY_CLID = ""
DUMMY_STATE = ""

# global : userlist
# liste des champs :
#  user :             user name
#  passwd :           password
#  sessionid :        session id generated at connection
#  sessiontimestamp : last time when the client proved itself to be ALIVE :)
#  ip :               ip address of the client (current session)
#  port :             port here the client is listening.
#  state :            cf. allowed_states
# The user identifier will likely be its phone number

PIDFILE = '/var/run/xivo_daemon.pid'
#PIDFILE = '/home/xilun/xivo_daemon.pid'
# TODO: command line parameter

#PIDFILE = '/home/xilun/xivo_daemon.pid'
# TODO: command line parameter

BUFSIZE_LARGE = 8192
BUFSIZE_UDP = 2048
BUFSIZE_ANY = 512

socket.setdefaulttimeout(2)
DAEMON = "daemon-announce"
HISTSEPAR = ";"
XIVO_CLI_PHP_HEADER = "XIVO-CLI-PHP"
REQUIRED_CLIENT_VERSION = 1441

# capabilities
CAPA_CUSTINFO    = 1 <<  0
CAPA_PRESENCE    = 1 <<  1
CAPA_HISTORY     = 1 <<  2
CAPA_DIRECTORY   = 1 <<  3
CAPA_DIAL        = 1 <<  4
CAPA_FEATURES    = 1 <<  5
CAPA_PEERS       = 1 <<  6
CAPA_MESSAGE     = 1 <<  7
CAPA_SWITCHBOARD = 1 <<  8
CAPA_AGENTS      = 1 <<  9
CAPA_FAX         = 1 << 10

map_capas = {
        'customerinfo'     : CAPA_CUSTINFO,
        'presence'         : CAPA_PRESENCE,
        'history'          : CAPA_HISTORY,
        'directory'        : CAPA_DIRECTORY,
        'dial'             : CAPA_DIAL,
        'features'         : CAPA_FEATURES,
        'peers'            : CAPA_PEERS,
        'instantmessaging' : CAPA_MESSAGE,
        'switchboard'      : CAPA_SWITCHBOARD,
        'agents'           : CAPA_AGENTS,
        'fax'              : CAPA_FAX
        }


## \brief Logs actions to a log file, prepending them with a timestamp.
# \param string the string to log
# \return zero
# \sa log_debug
def varlog(string):
        syslog.syslog(syslog.LOG_NOTICE, "xivo_daemon : " + string)
        return 0


## \brief Logs all events or status updates to a log file, prepending them with a timestamp.
# \param string the string to log
# \param events log to events file
# \param updatesgui log to gui files
# \return zero
def verboselog(string, events, updatesgui):
        if debug_mode:
                if events and evtfile:
                        evtfile.write(time.strftime("%b %2d %H:%M:%S ", time.localtime()) + string + "\n")
                        evtfile.flush()
                if updatesgui and guifile:
                        guifile.write(time.strftime("%b %2d %H:%M:%S ", time.localtime()) + string + "\n")
                        guifile.flush()
        return 0


## \brief Outputs a string to stdout in no-daemon mode
# and always logs it.
# \param string the string to display and log
# \return the return code of the varlog call
# \sa varlog
def log_debug(string):
        if debug_mode: print "#debug# " + string
        return varlog(string)


"""
Functions related to user-driven requests : history, directory, ...
These functions should not deal with CTI clients directly, however.
"""

## \brief Function that fetches the call history from a database
# \param astn the asterisk to connect to
# \param techno technology (SIP/IAX/ZAP/etc...)
# \param phoneid phone id
# \param phonenum the phone number
# \param nlines the number of lines to fetch for the given phone
# \param kind kind of list (ingoing, outgoing, missed calls)
def update_history_call(astn, techno, phoneid, phonenum, nlines, kind):
        results = []
        if configs[astn].cdr_db_uri == "":
                log_debug("%s : no CDR uri defined for this asterisk - see cdr_db_uri parameter" %configs[astn].astid)
        else:
                try:
                        conn = anysql.connect_by_uri(configs[astn].cdr_db_uri)
                        # charset = 'utf8' : add ?charset=utf8 to the URI

                        cursor = conn.cursor()
                        table = "cdr" # configs[astn].cdr_db_tablename
                        sql = ["SELECT calldate, clid, src, dst, dcontext, channel, dstchannel, " \
                               "lastapp, lastdata, duration, billsec, disposition, amaflags, " \
                               "accountcode, uniqueid, userfield FROM %s " % (table)]
                        if kind == "0": # outgoing calls (all)
                                # sql.append("WHERE disposition='ANSWERED' ")
                                sql.append("WHERE channel LIKE '%s/%s-%%' " \
                                           "ORDER BY calldate DESC LIMIT %s" % (techno, phoneid, nlines))
                        elif kind == "1": # incoming calls (answered)
                                sql.append("WHERE disposition='ANSWERED' ")
                                sql.append("AND dstchannel LIKE '%s/%s-%%' " \
                                           "ORDER BY calldate DESC LIMIT %s" % (techno, phoneid, nlines))
                        else: # missed calls (received but not answered)
                                sql.append("WHERE disposition!='ANSWERED' ")
                                sql.append("AND dstchannel LIKE '%s/%s-%%' " \
                                           "ORDER BY calldate DESC LIMIT %s" % (techno, phoneid, nlines))
                        cursor.execute(''.join(sql))
                        results = cursor.fetchall()
                        conn.close()
                except Exception, exc:
                        log_debug("--- exception --- %s : Connection to DataBase %s failed in History request : %s"
                                  %(configs[astn].astid, configs[astn].cdr_db_uri, str(exc)))
        return results


def build_history_string(requester_id, nlines, kind):
        [dummyp, ast_src, dummyx, techno, phoneid, phonenum] = requester_id.split('/')
        if ast_src in asteriskr:
                idast_src = asteriskr[ast_src]
                reply = ["history="]
                try:
                        hist = update_history_call(idast_src, techno, phoneid, phonenum, nlines, kind)
                        for x in hist:
                                try:
                                        reply.append(x[0].isoformat() + HISTSEPAR + x[1] \
                                                     + HISTSEPAR + str(x[10]) + HISTSEPAR + x[11])
                                except:
                                        reply.append(x[0] + HISTSEPAR + x[1] \
                                                     + HISTSEPAR + str(x[10]) + HISTSEPAR + x[11])
                                if kind == '0':
                                        reply.append(HISTSEPAR + x[3] + HISTSEPAR + 'OUT')
                                else:   # display callerid for incoming calls
                                        reply.append(HISTSEPAR + x[1] + HISTSEPAR + 'IN')
                                reply.append(";")
                except Exception, exc:
                        log_debug("--- exception --- (%s) error : history : (client %s) : %s"
                                  %(ast_src, requester, str(exc)))
        else:
                reply = "message=%s::history KO : no such asterisk id\n" %DAEMON
        return ''.join(reply)


## \brief Builds the full list of customers in order to send them to the requesting client.
# This should be done after a command called "customers".
# \return a string containing the full customers list
# \sa manage_tcp_connection
def build_customers(ctx, searchpatterns):
        searchpattern = ' '.join(searchpatterns)
        if ctx in contexts_cl:
                z = contexts_cl[ctx]

        fullstatlist = []
        fullstat_header = "directory-response=%d;" % len(z.search_valid_fields) + ';'.join(z.search_titles)

        if searchpattern == "":
                return fullstat_header

        dbkind = z.uri.split(":")[0]
        if dbkind == "ldap":
                selectline = []
                for fname in z.search_matching_fields:
                        if searchpattern == "*":
                                selectline.append("(%s=*)" % fname)
                        else:
                                selectline.append("(%s=*%s*)" %(fname, searchpattern))

                try:
                        ldapid = xivo_ldap.xivo_ldap(z.uri)
                        results = ldapid.getldap("(|%s)" % ''.join(selectline),
                                        z.search_matching_fields)
                        for result in results:
                                result_v = {}
                                for f in z.search_matching_fields:
                                        if f in result[1]:
                                                result_v[f] = result[1][f][0]
                                fullstatlist.append(';'.join(z.result_by_valid_field(result_v)))
                except Exception, exc:
                        log_debug('--- exception --- ldaprequest : %s' % str(exc))

        elif dbkind != "":
                if searchpattern == "*":
                        whereline = ""
                else:
                        wl = []
                        for fname in z.search_matching_fields:
                                wl.append("%s REGEXP '%s'" %(fname, searchpattern))
                        whereline = "WHERE " + ' OR '.join(wl)

                sqlrequest = "SELECT %s FROM %s %s;" %(', '.join(z.search_matching_fields),
                                                       z.sqltable,
                                                       whereline)
                try:
                        conn = anysql.connect_by_uri(z.uri)
                        cursor = conn.cursor()
                        cursor.execute(sqlrequest)
                        results = cursor.fetchall()
                        conn.close()
                        for result in results:
                                result_v = {}
                                n = 0
                                for f in z.search_matching_fields:
                                        result_v[f] = result[n]
                                        n += 1
                                fullstatlist.append(';'.join(z.result_by_valid_field(result_v)))
                except Exception, exc:
                        log_debug('--- exception --- sqlrequest : %s' % str(exc))
        else:
                log_debug("no database method defined - please fill the dir_db_uri field of the <%s> context" % ctx)

        uniq = {}
        fullstatlist.sort()
        fullstat_body = []
        for fsl in [uniq.setdefault(e,e) for e in fullstatlist if e not in uniq]:
                fullstat_body.append(fsl)
        fullstat = fullstat_header + ";" + ';'.join(fullstat_body)
        return fullstat


## \brief Builds the base status (no channel information) for one phone identifier
# \param phoneid the "pointer" to the Asterisk phone statuses
# \return the string containing the base status of the phone
def build_basestatus(phoneid):
        basestatus = (phoneid.tech,
                      phoneid.phoneid,
                      phoneid.phonenum,
                      phoneid.context,
                      phoneid.imstat,
                      phoneid.sipstatus,
                      phoneid.voicemail,
                      phoneid.queueavail)
        return ":".join(basestatus)


## \brief Builds the base status (no channel information) for one phone identifier
# \param phoneid the "pointer" to the Asterisk phone statuses
# \return the string containing the base status of the phone
def build_cidstatus(phoneid):
        cidstatus = (phoneid.calleridfull,
                     phoneid.calleridfirst,
                     phoneid.calleridlast)
        return ":".join(cidstatus)


## \brief Builds the channel-by-channel part for the hints/update replies.
# \param phoneid the "pointer" to the Asterisk phone statuses
# \return the string containing the statuses for each channel of the given phone
def build_fullstatlist(phoneid):
        nchans = len(phoneid.chann)
        fstat = [str(nchans)]
        for chan,phone_chan in phoneid.chann.iteritems():
                fstat.extend((":", chan, ":",
                              phone_chan.getStatus(), ":",
                              str(phone_chan.getDeltaTime()), ":",
                              phone_chan.getDirection(), ":",
                              phone_chan.getChannelPeer(), ":",
                              phone_chan.getChannelNum()))
        return ''.join(fstat)


## \brief Builds the features reply.
def build_features_get(reqlist):
        dbfamily = "%s/users/%s" %(reqlist[1], reqlist[2])
        repstr = "featuresget="

        for key in ["VM", "Record", "Screen", "DND"]:
                try:
                        fullcomm = "database get %s %s" %(dbfamily, key)
                        reply = AMI_array_user_commands[reqlist[0]].execclicommand(fullcomm)
                        for r in reply:
                                rep = r.rstrip()
                                if rep.find("Value: ") == 0 and len(rep.split(' ')) == 2:
                                        repstr += "%s;%s;" %(key, rep.split(' ')[1])
                except Exception, exc:
                        log_debug("--- exception --- featuresget(bool) id=%s key=%s : %s" %(str(reqlist), key, str(exc)))

        for key in ["FWD/Unc", "FWD/Busy", "FWD/RNA"]:
                keystatus = ""
                keynumber = ""
                try:
                        fullcomm = "database get %s %s/Status" %(dbfamily, key)
                        reply = AMI_array_user_commands[reqlist[0]].execclicommand(fullcomm)
                        for r in reply:
                                rep = r.rstrip()
                                if rep.find("Value: ") == 0 and len(rep.split(' ')) == 2:
                                        keystatus = rep.split(' ')[1]

                        fullcomm = "database get %s %s/Number" %(dbfamily, key)
                        reply = AMI_array_user_commands[reqlist[0]].execclicommand(fullcomm)
                        for r in reply:
                                rep = r.rstrip()
                                if rep.find("Value: ") == 0 and len(rep.split(' ')) == 2:
                                        keynumber = rep.split(' ')[1]
                except Exception, exc:
                        log_debug("--- exception --- featuresget(str) id=%s key=%s : %s" %(str(reqlist), key, str(exc)))

                repstr += "%s;%s:%s;" %(key, keystatus, keynumber)
        return repstr


## \brief Builds the features reply.
def build_features_put(reqlist):
        dbfamily = "%s/users/%s" %(reqlist[1], reqlist[2])
        try:
                len_reqlist = len(reqlist)
                if len_reqlist >= 4:
                        key = reqlist[3]
                        if len_reqlist >= 5:
                                value = reqlist[4]
                        else:
                                value = ""
                        fullcomm = 'database put %s %s "%s"' %(dbfamily, key, value)
                        reply = AMI_array_user_commands[reqlist[0]].execclicommand(fullcomm)
                        repstr = "KO"
                        for r in reply:
                                if r.rstrip() == "Updated database successfully":
                                        repstr = "OK"
                        response = 'featuresput=%s;%s;%s;' %(repstr, key, value)
                else:
                        response = "featuresput=KO"
        except Exception, exc:
                log_debug("--- exception --- featuresput id=%s : %s" %(str(reqlist), str(exc)))
                response = "featuresput=KO"
        return response


## \brief Builds the full list of callerIDNames in order to send them to the requesting client.
# This should be done after a command called "callerid".
# \return a string containing the full callerIDs list
# \sa manage_tcp_connection
def build_callerids():
        global plist
        fullstat = ["callerids="]
        for n in items_asterisks:
                plist_n = plist[n]
                plist_normal_keys = filter(lambda j: plist_n.normal[j].towatch, plist_n.normal.iterkeys())
                plist_normal_keys.sort()
                for phonenum in plist_normal_keys:
                        phoneinfo = ("cid",
                                     plist_n.astid,
                                     plist_n.normal[phonenum].tech,
                                     plist_n.normal[phonenum].phoneid,
                                     plist_n.normal[phonenum].phonenum,
                                     plist_n.normal[phonenum].context,
                                     plist_n.normal[phonenum].calleridfull,
                                     plist_n.normal[phonenum].calleridfirst,
                                     plist_n.normal[phonenum].calleridlast + ";")
                        #    + "groupinfos/technique"
                        fullstat.append(":".join(phoneinfo))
        fullstat.append("\n")
        return ''.join(fullstat)


## \brief Builds the full list of phone statuses in order to send them to the requesting client.
# \return a string containing the full list of statuses
def build_statuses():
        global plist
        fullstat = ["hints="]
        for n in items_asterisks:
                plist_n = plist[n]
                plist_normal_keys = filter(lambda j: plist_n.normal[j].towatch, plist_n.normal.iterkeys())
                plist_normal_keys.sort()
                for phonenum in plist_normal_keys:
                        plist_n.normal[phonenum].update_time()
                        phoneinfo = "hnt:" + plist_n.astid + ":" + build_basestatus(plist_n.normal[phonenum])
                        fullstat.extend((phoneinfo, ":", build_fullstatlist(plist_n.normal[phonenum]), ";"))
        fullstat.append("\n")
        return ''.join(fullstat)


"""
Functions related to SIP presence management
"""

## \brief Extracts the main SIP properties from a received packet
# such as CSeq, message type (REGISTER, OPTION, SUBSCRIBE, ...),
# callid, address, number of lines, reurn code (200, 404, 484, ...).
# \param data the SIP buffer to parse
# \return an array containing these above informations
def read_sip_properties(data):
        cseq = 1
        msg = "xxx"
        cid = "no_callid@xivopy"
        account = ""
        lines = ""
        ret = -99
        bbranch = ""
        btag = "no_tag"
        authenticate = ""

        try:
                lines = data.split("\r\n")
                if lines[0].find("SIP/2.0") == 0: ret = int(lines[0].split(None)[1])

                for x in lines:
                        if x.find("CSeq") == 0:
                                cseq = int(x.split(None)[1])
                                msg = x.split(None)[2]
                        elif x.find("From: ") == 0 or x.find("f: ") == 0:
                                account = x.split("<sip:")[1].split("@")[0]
                        elif x.find("Call-ID:") == 0 or x.find("i: ") == 0:
                                cid = x.split(None)[1]
                        elif x.find("WWW-Authenticate:") == 0:
                                authenticate = x
                        elif x.find("branch=") >= 0:  bbranch = x.split("branch=")[1].split(";")[0]
                        elif x.find("tag=") >= 0:     btag = x.split("tag=")[1].split(";")[0]

        except Exception, exc:
                log_debug("--- exception --- read_sip_properties : " + str(exc))

        return [cseq, msg, cid, account, len(lines), ret, bbranch, btag, authenticate]


## \brief Converts the SIP message to a useful presence information.
# Eventually, it will be done with XML functions.
# \param data the SIP message
# \return the extracted status
def tellpresence(data):
        num, stat = [None, None]
        lines = data.split("\n")
        state_not_active = ""

        for x in lines:
                if x.find("Subscription-State:") == 0:
                        if x.find("Subscription-State: active") < 0 and \
                               x.find("Subscription-State: terminated") < 0:
                                state_not_active = x
                if x.find("<note>") == 0:
                        if x.find("Ready") >= 0:          stat = "Ready"
                        elif x.find("On the phone") >= 0: stat = "On the phone"
                        elif x.find("Ringing") >= 0:      stat = "Ringing"
                        elif x.find("Not online") >= 0:   stat = "Not online"
                        elif x.find("Unavailable") >= 0:  stat = "Unavailable"
                        else:                             stat = "XivoUnknown"
                if x.find("<tuple id") == 0: num = x.split("\"")[1]
        if state_not_active != "":
                log_debug("%s (%s) %s" %(num, stat, state_not_active))
        return [num, stat]


## \brief Handles the SIP messages according to their meaning (reply to a formerly sent message).
# \param astnum the asterisk numerical identifier
# \param data   the data read from the socket
# \param l_sipsock the socket identifier in order to reply
# \param l_addrsip the SIP address in order to reply
# \return True if it is an OPTIONS packet
# \sa read_sip_properties
def parseSIP(astnum, data, l_sipsock, l_addrsip):
        global plist, configs
        spret = False
        [icseq, imsg, icid, iaccount, ilength, iret, ibranch, itag, iauth] = read_sip_properties(data)
        # if ilength != 11:
        #print "###", astnum, ilength, icseq, icid, iaccount, imsg, iret, ibranch, itag

        uri = "sip:%s@%s" %(iaccount, configs[astnum].remoteaddr)
        mycontext = ""
        mysippass = ""
        if iaccount in configs[astnum].xivosb_phoneids:
                mycontext, mysippass = configs[astnum].xivosb_phoneids[iaccount]
        md5_r1 = md5.md5("%s:%s:%s" %(iaccount, configs[astnum].realm, mysippass)).hexdigest()

        #print "-----------", iaccount, mysippass
        #print data
        #print "================================="

        if imsg == "REGISTER":
                if iret == 401:
                        # log_debug("%s : REGISTER %s Passwd?" %(configs[astnum].astid, iaccount))
                        nonce    = iauth.split("nonce=\"")[1].split("\"")[0]
                        md5_r2   = md5.md5(imsg   + ":" + uri).hexdigest()
                        response = md5.md5("%s:%s:%s" %(md5_r1, nonce, md5_r2)).hexdigest()
                        auth = "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\", algorithm=MD5" \
                               %(iaccount, configs[astnum].realm, nonce, uri, response)
                        command = xivo_sip.sip_register(configs[astnum], iaccount,
                                                        1, "reg_cid@xivopy",
                                                        (xivosb_register_frequency + 2), auth)
                        l_sipsock.sendto(command, (configs[astnum].remoteaddr, configs[astnum].portsipsrv))
                elif iret == 403:
                        log_debug("%s : REGISTER %s Unauthorized" %(configs[astnum].astid, iaccount))
                elif iret == 100:
                        # log_debug("%s : REGISTER %s Trying" %(configs[astnum].astid, iaccount))
                        pass
                elif iret == 200:
                        # log_debug("%s : REGISTER %s OK" %(configs[astnum].astid, iaccount))
                        rdc = ''.join(random.sample('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijkLmnopqrstuvwxyz0123456789',6)) + "-" + hex(int(time.time()))[:1:-1]
                        if nukeast: rdc = 'unique-callid-string'
                        for sipnum,normval in plist[astnum].normal.iteritems():
                                if sipnum.find("SIP/") == 0:
                                        if mycontext == normval.context:
                                                dtnow = time.time() - normval.lasttime
                                                if dtnow > (2 * xivosb_register_frequency):
                                                        if normval.sipstatus != "Timeout":
                                                                normval.set_sipstatus("Timeout")
                                                                update_GUI_clients(astnum, sipnum, "sip-tmo")
                                                        else:
                                                                pass
                                                else:
                                                        pass
                                                cid = rdc + "-subsxivo-" + sipnum.split("/")[1] + "@" + configs[astnum].localaddr
                                                command = xivo_sip.sip_subscribe(configs[astnum], iaccount,
                                                                                 1, cid, normval.phonenum,
                                                                                 (xivosb_register_frequency + 2), "")
                                                l_sipsock.sendto(command, (configs[astnum].remoteaddr, configs[astnum].portsipsrv))
                                        else:
                                                pass
                                else:
                                        pass
                else:
                        log_debug("%s : REGISTER %s Failed (code %d)"
                                  %(configs[astnum].astid, iaccount, iret))


        elif imsg == "SUBSCRIBE":
                sipphone = "SIP/" + icid.split("@")[0].split("-subsxivo-")[1]
                if sipphone in plist[astnum].normal: # else : send sth anyway ?
                        normv = plist[astnum].normal[sipphone]
                        normv.set_lasttime(time.time())
                        if iret == 401:
                                # log_debug("%s : SUBSCRIBE %s Passwd? %s" %(configs[astnum].astid, iaccount, icid))
                                nonce    = iauth.split("nonce=\"")[1].split("\"")[0]
                                md5_r2   = md5.md5(imsg   + ":" + uri).hexdigest()
                                response = md5.md5(md5_r1 + ":" + nonce + ":" + md5_r2).hexdigest()
                                auth = "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\", algorithm=MD5" \
                                       %(iaccount, configs[astnum].realm, nonce, uri, response)
                                command = xivo_sip.sip_subscribe(configs[astnum], iaccount,
                                                                 1, icid, normv.phonenum,
                                                                 (xivosb_register_frequency + 2), auth)
                                l_sipsock.sendto(command, (configs[astnum].remoteaddr, configs[astnum].portsipsrv))
                        elif iret == 403:
                                log_debug("%s : SUBSCRIBE %s Unauthorized %s" %(configs[astnum].astid, iaccount, icid))
                                if normv.sipstatus != "Fail" + str(iret):
                                        normv.set_sipstatus("Fail" + str(iret))
                                        update_GUI_clients(astnum, sipphone, "sip_403")
                                else:
                                        pass
                        elif iret == 100:
                                # log_debug("%s : SUBSCRIBE %s Trying %s" %(configs[astnum].astid, iaccount, icid))
                                pass
                        elif iret == 200:
                                # log_debug("%s : SUBSCRIBE %s OK %s" %(configs[astnum].astid, iaccount, icid))
                                pass
                        else:
                                log_debug("%s : SUBSCRIBE %s Failed (code %d) %s"
                                          %(configs[astnum].astid, iaccount, iret, icid))
                                if normv.sipstatus != "Fail" + str(iret):
                                        normv.set_sipstatus("Fail" + str(iret))
                                        update_GUI_clients(astnum, sipphone, "sip-fai")


        elif imsg == "OPTIONS" or imsg == "NOTIFY":
                command = xivo_sip.sip_ok(configs[astnum], iaccount,
                                          icseq, icid, imsg, ibranch, itag)
                l_sipsock.sendto(command,(configs[astnum].remoteaddr, l_addrsip[1]))
                if imsg == "NOTIFY":
                        sipnum, sippresence = tellpresence(data)
                        if [sipnum, sippresence] != [None, None]:
                                sipphone = "SIP/" + icid.split("@")[0].split("-subsxivo-")[1] # vs. sipnum
                                if sipphone in plist[astnum].normal:
                                        normv = plist[astnum].normal[sipphone]
                                        normv.set_lasttime(time.time())
                                        if normv.sipstatus != sippresence:
                                                normv.set_sipstatus(sippresence)
                                                update_GUI_clients(astnum, sipphone, "SIP-NTFY")
                                        else:
                                                pass
                                else:
                                        pass
                else:
                        spret = True
        return spret


## \brief Sends a SIP register + n x SIP subscribe messages.
# \param config_ast the asterisk configuration structure
# \param l_sipsock the SIP socket where to reply
# \return
def do_sip_register(config_ast, l_sipsock):
        for sipacc in config_ast.xivosb_phoneids:
                command = xivo_sip.sip_register(config_ast, sipacc,
                                                1, "reg_cid@xivopy",
                                                (xivosb_register_frequency + 2), "")
                l_sipsock.sendto(command, (config_ast.remoteaddr, config_ast.portsipsrv))
                # command = xivo_sip.sip_options(config_ast, config_ast.mysipname, cid, sipnum)


"""
Communication with CTI clients
"""


def send_msg_to_cti_clients(strupdate):
        # sends to TCP ports
        for tcpclient in tcpopens_sb:
                try:
                        tcpclient[0].send(strupdate + "\n")
                except Exception, exc:
                        log_debug("--- exception --- send %s has failed on %s : %s"
                                  %(strupdate.split('=')[0],
                                    str(tcpclient[0]),
                                    str(exc)))
        for ka_object in requestersocket_by_login.itervalues():
                userlist_lock.acquire()
                try:
                        mysock = ka_object.request[1]
                        mysock.sendto(strupdate,
                                      ka_object.client_address)
                finally:
                        userlist_lock.release()


## \brief Sends a status update to all the connected xivo-switchboard(-like) clients.
# \param astnum the asterisk numerical identifier
# \param phonenum the phone identifier
# \param fromwhom a string that tells who has requested such an update
# \return none
def update_GUI_clients(astnum, phonenum, fromwhom):
        global plist
        phoneinfo = fromwhom + ":" + plist[astnum].astid + ":" + build_basestatus(plist[astnum].normal[phonenum])
        fstatlist = build_fullstatlist(plist[astnum].normal[phonenum])
        strupdate = "update=" + phoneinfo + ":" + fstatlist

        send_msg_to_cti_clients(strupdate)

        verboselog(strupdate, False, True)

"""
"""



## \brief Splits a channel name, allowing for instance local-extensions-3fb2,1 to be correctly split.
# \param channel the full channel name
# \return the phone id
def channel_splitter(channel):
        sp = channel.split("-")
        if len(sp) > 1:
                sp.pop()
        return "-".join(sp)


## \brief Extracts the phone number and the channel name from the asterisk/SIP/num-08abcf
# UI syntax for hangups or transfers
# \param fullname full string sent by the UI
# \return the phone number and the channel name, without the asterisk id
def split_from_ui(fullname):
        phone = ""
        channel = ""
        s1 = fullname.split("/")
        if len(s1) == 5:
                phone = s1[3] + "/" + channel_splitter(s1[4])
                channel = s1[3] + "/" + s1[4]
        return [phone, channel]


def originate_or_transfer(requester, l):
        src_split = l[1].split("/")
        dst_split = l[2].split("/")
        ret_message = "message=%s::originate_or_transfer KO from %s" %(DAEMON, requester)

        if len(src_split) == 5:
                [dummyp, ast_src, context_src, proto_src, userid_src] = src_split
        elif len(src_split) == 6:
                [dummyp, ast_src, context_src, proto_src, userid_src, dummy_exten_src] = src_split

        if len(dst_split) == 6:
                [dummyp, ast_dst, context_dst, proto_dst, userid_dst, exten_dst] = dst_split
        else:
                [dummyp, ast_dst, context_dst, proto_dst, userid_dst, exten_dst] = src_split
                exten_dst = l[2]
        idast_src = -1
        idast_dst = -1
        if ast_src in asteriskr: idast_src = asteriskr[ast_src]
        if ast_dst in asteriskr: idast_dst = asteriskr[ast_dst]
        if idast_src != -1 and idast_src == idast_dst:
                if ast_src in AMI_array_user_commands and AMI_array_user_commands[ast_src]:
                        if l[0] == 'originate':
                                log_debug("%s is attempting an ORIGINATE : %s" %(requester, str(l)))
                                if ast_dst != "":
                                        ret = AMI_array_user_commands[ast_src].originate(proto_src,
                                                                              userid_src,
                                                                              exten_dst,
                                                                              context_dst)
                                else:
                                        ret = False
                                if ret:
                                        ret_message = "message=%s::originate OK (%s) %s %s" %(DAEMON, ast_src, l[1], l[2])
                                else:
                                        ret_message = "message=%s::originate KO (%s) %s %s" %(DAEMON, ast_src, l[1], l[2])
                        elif l[0] == 'transfer':
                                log_debug("%s is attempting a TRANSFER : %s" %(requester, str(l)))
                                phonesrc, phonesrcchan = split_from_ui(l[1])
                                if phonesrc == phonesrcchan:
                                        ret_message = "message=%s::transfer KO : %s not a channel" %(DAEMON, phonesrcchan)
                                else:
                                        if phonesrc in plist[idast_src].normal:
                                                channellist = plist[idast_src].normal[phonesrc].chann
                                                nopens = len(channellist)
                                                if nopens == 0:
                                                        ret_message = "message=%s::transfer KO : no channel opened on %s" %(DAEMON, phonesrc)
                                                else:
                                                        tchan = channellist[phonesrcchan].getChannelPeer()
                                                        ret = AMI_array_user_commands[ast_src].transfer(tchan,
                                                                                             exten_dst,
                                                                                             "local-extensions")
                                                        if ret:
                                                                ret_message = "message=%s::transfer OK (%s) %s %s" %(DAEMON, ast_src, l[1], l[2])
                                                        else:
                                                                ret_message = "message=%s::transfer KO (%s) %s %s" %(DAEMON, ast_src, l[1], l[2])
        else:
                ret_message = "message=%s::originate or transfer KO : asterisk id mismatch (%d %d)" %(DAEMON, idast_src, idast_dst)
        return ret_message


def hangup(requester, l):
        idast_src = -1
        ast_src = l[1].split("/")[1]
        ret_message = "message=%s::hangup KO from %s" %(DAEMON, requester)
        if ast_src in asteriskr: idast_src = asteriskr[ast_src]
        if idast_src != -1:
                log_debug("%s is attempting a HANGUP : %s" %(requester, str(l)))
                phone, channel = split_from_ui(l[1])
                if phone in plist[idast_src].normal:
                        if channel in plist[idast_src].normal[phone].chann:
                                channel_peer = plist[idast_src].normal[phone].chann[channel].getChannelPeer()
                                log_debug("UI action : %s : hanging up <%s> and <%s>"
                                          %(configs[idast_src].astid , channel, channel_peer))
                                if ast_src in AMI_array_user_commands and AMI_array_user_commands[ast_src]:
                                        ret = AMI_array_user_commands[ast_src].hangup(channel, channel_peer)
                                        if ret > 0:
                                                ret_message = "message=%s::hangup OK (%d) %s" %(DAEMON, ret, l[1])
                                        else:
                                                ret_message = "message=%s::hangup KO : socket request failed" %DAEMON
                                else:
                                        ret_message = "message=%s::hangup KO : no socket available" %DAEMON
                        else:
                                ret_message = "message=%s::hangup KO : no such channel" %DAEMON
                else:
                        ret_message = "message=%s::hangup KO : no such phone" %DAEMON
        else:
                ret_message = "message=%s::hangup KO : no such asterisk id (%s)" %(DAEMON, ast_src)
        return ret_message



def manage_login(cfg, requester_ip, requester_port, socket):
        global userinfo_by_requester
        for argum in ['astid', 'proto', 'userid', 'state', 'ident', 'passwd', 'version']:
                if argum not in cfg:
                        repstr = "loginko=missing:%s" % argum
                        return repstr

        if cfg.get('astid') in asteriskr:
                astnum   = asteriskr[cfg.get('astid')]
        else:
                log_debug("login command attempt from SB : asterisk name <%s> unknown" % cfg.get('astid'))
                repstr = "loginko=asterisk_name"
                return repstr
        proto    = cfg.get('proto').lower()
        userid   = cfg.get('userid')
        state    = cfg.get('state')
        [whoami, whatsmyos] = cfg.get('ident').split("@")
        password = cfg.get('passwd')
        version  = cfg.get('version')

        if int(cfg.get('version')) < REQUIRED_CLIENT_VERSION:
                repstr = "loginko=version_client:%s;%d" % (cfg.get('version'), REQUIRED_CLIENT_VERSION)
                return repstr

        capa_user = []
        userlist_lock.acquire()
        try:
                userinfo = finduser(cfg.get('astid'), proto + userid)
                if userinfo == None:
                        repstr = "loginko=user_not_found"
                        log_debug("no user found %s" % str(cfg))
                elif password != userinfo['passwd']:
                        repstr = "loginko=login_passwd"
                else:
                        reterror = check_user_connection(userinfo, whoami)
                        if reterror is None:
                                for capa in capabilities_list:
                                        if (map_capas[capa] & userinfo.get('capas')):
                                                capa_user.append(capa)

                                sessionid = '%u' % random.randint(0,999999999)
                                connect_user(userinfo, sessionid,
                                             requester_ip, requester_port,
                                             whoami, whatsmyos, True, state,
                                             False, socket)

                                repstr = "loginok=" \
                                         "context:%s;phonenum:%s;capas:%s;" \
                                         "version:%s;state:%s" %(userinfo.get('context'),
                                                                 userinfo.get('phonenum'),
                                                                 ",".join(capa_user),
                                                                 __version__.split()[1],
                                                                 userinfo.get('state'))
                                userinfo_by_requester[requester_ip + ":" + requester_port] = [astnum,
                                                                                              cfg.get('astid'),
                                                                                              proto + userid,
                                                                                              userinfo.get('context'),
                                                                                              userinfo.get('capas')]
                                send_availstate_update(astnum, proto + userid, state)
                        else:
                                repstr = "loginko=%s" % reterror
        finally:
                userlist_lock.release()

        return repstr



## \brief Deals with requests from the UI clients.
# \param connid connection identifier
# \param allow_events tells if this connection belongs to events-allowed ones
# (for switchboard) or to events-disallowed ones (for php CLI commands)
# \return none
def manage_tcp_connection(connid, allow_events):
        global AMI_array_user_commands, ins

        try:
                requester_ip   = connid[1]
                requester_port = str(connid[2])
                requester      = requester_ip + ":" + requester_port
        except Exception, exc:
                log_debug("--- exception --- UI connection : could not get IP details of connid = %s : %s" %(str(connid),str(exc)))
                requester = str(connid)

        try:
                msg = connid[0].recv(BUFSIZE_LARGE)
        except Exception, exc:
                msg = ""
                log_debug("--- exception --- UI connection : a problem occured when recv from %s : %s" %(requester, str(exc)))
        if len(msg) == 0:
                try:
                        connid[0].close()
                        ins.remove(connid[0])
                        if allow_events == True:
                                tcpopens_sb.remove(connid)
                                log_debug("TCP (SB)  socket closed from %s" %requester)
                                if requester in userinfo_by_requester:
                                        astnum  = userinfo_by_requester[requester][0]
                                        astname = userinfo_by_requester[requester][1]
                                        username = userinfo_by_requester[requester][2]
                                        userlist_lock.acquire()
                                        try:
                                                userinfo = finduser(astname, username)
                                                if userinfo == None:
                                                        log_debug("no user found for %s/%s" %(astname, username))
                                                else:
                                                        disconnect_user(userinfo)
                                                        send_availstate_update(astnum, username, "unknown")
                                        finally:
                                                userlist_lock.release()
                                        del userinfo_by_requester[requester]
                        else:
                                tcpopens_php.remove(connid)
                                log_debug("TCP (PHP) socket closed from %s" %requester)
                except Exception, exc:
                        log_debug("--- exception --- UI connection [%s] : a problem occured when trying to close %s : %s"
                                  %(msg, str(connid[0]), str(exc)))
        else:
            for usefulmsgpart in msg.split("\n"):
                usefulmsg = usefulmsgpart.split("\r")[0]
                # debug/setup functions
                if usefulmsg == "show_infos":
                        try:
                                time_uptime = int(time.time() - time_start)
                                reply = "infos=version=%s;uptime=%d s;logged_sb=%d/%d;logged_xc=%d/%d" \
                                        %(__version__.split()[1], time_uptime,
                                          conngui_sb, maxgui_sb, conngui_xc, maxgui_xc)
                                for tcpo in tcpopens_sb:
                                        reply += ":%s:%d" %(tcpo[1],tcpo[2])
                                connid[0].send(reply + "\n")
                                connid[0].send("server capabilities = %s\n" %(",".join(capabilities_list)))
                                connid[0].send("%s:OK\n" %(XIVO_CLI_PHP_HEADER))
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))
                elif usefulmsg == "show_phones":
                        try:
                                for plast in plist:
                                        k1 = plast.normal.keys()
                                        k1.sort()
                                        for kk in k1:
                                                canal = plast.normal[kk].chann
                                                connid[0].send("%10s %10s %6s [SIP : %12s - %4d s] %4d %s\n"
                                                               %(plast.astid,
                                                                 kk,
                                                                 plast.normal[kk].towatch,
                                                                 plast.normal[kk].sipstatus,
                                                                 int(time.time() - plast.normal[kk].lasttime),
                                                                 len(canal),
                                                                 str(canal.keys())))
                                connid[0].send("%s:OK\n" %(XIVO_CLI_PHP_HEADER))
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))
                elif usefulmsg == "show_logged":
                        try:
                                userlist_lock.acquire()
                                try:
                                        for astname in userlist:
                                                connid[0].send("on <%s> :\n" % astname)
                                                for user,info in userlist[astname].iteritems():
                                                        connid[0].send("%s %s\n" %(user, info))
                                finally:
                                        userlist_lock.release()
                                if requester in userinfo_by_requester:
                                        connid[0].send("%s\n" %str(userinfo_by_requester[requester]))
                                connid[0].send("%s:OK\n" %(XIVO_CLI_PHP_HEADER))
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))
                elif usefulmsg == "show_ami":
                        try:
                                for amis in AMI_array_events_off:
                                        connid[0].send("events off   : %s : %s\n" %(amis, str(AMI_array_events_off[amis])))
                                for amis in AMI_array_events_on:
                                        connid[0].send("events on    : %s : %s\n" %(amis, str(AMI_array_events_on[amis])))
                                for amis in AMI_array_user_commands:
                                        connid[0].send("commands     : %s : %s\n" %(amis, str(AMI_array_user_commands[amis])))
                                connid[0].send("%s:OK\n" %(XIVO_CLI_PHP_HEADER))
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))

                elif usefulmsg != "":
                        l = usefulmsg.split()
                        if l[0] == 'history' or l[0] == 'directory-search' or \
                               l[0] == 'featuresget' or l[0] == 'featuresput' or \
                               l[0] == 'hints' or l[0] == 'callerids' or l[0] == 'message' or l[0] == 'availstate' or \
                               l[0] == 'originate' or l[0] == 'transfer' or l[0] == 'hangup':
                                log_debug("%s is attempting a %s : %s" %(requester, l[0], str(l)))
                                try:
                                        if requester in userinfo_by_requester:
                                                repstr = parse_command_and_build_reply(userinfo_by_requester[requester], l)
                                                connid[0].send(repstr + "\n")
                                except Exception, exc:
                                        log_debug("--- exception --- UI connection [%s] : a problem occured when sending to %s : %s"
                                                  %(l[0], requester, str(exc)))
                        elif l[0] == 'login':
                                try:
                                        if len(l) == 2:
                                                arglist = l[1].split(";")
                                                cfg = {}
                                                for argm in arglist:
                                                        [param, value] = argm.split("=")
                                                        cfg[param] = value
                                                repstr = manage_login(cfg, requester_ip, requester_port, connid[0].makefile('w'))
                                        else:
                                                repstr = "loginko=version_client:0;%d" % REQUIRED_CLIENT_VERSION
                                        connid[0].send(repstr + '\n')
                                except Exception, exc:
                                        log_debug("--- exception --- UI connection [%s] : a problem occured when sending to %s : %s"
                                                  %(l[0], requester, str(exc)))


                        elif allow_events == False: # i.e. if PHP-style connection
                                n = -1
                                if requester_ip in ip_reverse_php: n = ip_reverse_php[requester_ip]
                                if n == -1:
                                        connid[0].send("%s:KO <NOT ALLOWED>\n" %(XIVO_CLI_PHP_HEADER))
                                else:
                                        astid = configs[n].astid
                                        connid[0].send("%s:ID <%s>\n" %(XIVO_CLI_PHP_HEADER, astid))
                                        try:
                                                if astid in AMI_array_user_commands and AMI_array_user_commands[astid]:
                                                        try:
                                                                s = AMI_array_user_commands[astid].execclicommand(usefulmsg.strip())
                                                        except Exception, exc:
                                                                log_debug("--- exception --- (%s) error : php command <%s> : (client %s) : %s"
                                                                          %(astid, str(usefulmsg.strip()), requester, str(exc)))
                                                        try:
                                                                for x in s: connid[0].send(x)
                                                                connid[0].send("%s:OK\n" %(XIVO_CLI_PHP_HEADER))
                                                                connid[0].close()
                                                                ins.remove(connid[0])
                                                                log_debug("TCP (PHP) socket closed towards %s" %requester)
                                                        except Exception, exc:
                                                                log_debug("--- exception --- (%s) error : php command <%s> : (client %s) : %s"
                                                                          %(astid, str(usefulmsg.strip()), requester, str(exc)))
                                        except Exception, exc:
                                                connid[0].send("%s:KO <Exception : %s>\n" %(XIVO_CLI_PHP_HEADER, str(exc)))
                        else:
                                connid[0].send("%s:KO <NOT ALLOWED from Switchboard>\n" %(XIVO_CLI_PHP_HEADER))


## \brief Tells whether a channel is a "normal" one, i.e. SIP, IAX2, mISDN, Zap
# or not (like Local, Agent, ... anything else).
# \param chan the channel-like string (that should be like "proto/phone-id")
# \return True or False according to the above description
def is_normal_channel(chan):
        if chan.find("SIP/") == 0 or chan.find("IAX2/") == 0 or \
           chan.find("mISDN/") == 0 or chan.find("Zap/") == 0: return True
        else: return False


"""
Management of events that are spied on the AMI
"""


## \brief Updates some channels according to the Dial events occuring in the AMI.
# \param listkeys the list of allowed phones
# \param astnum the Asterisk numerical identifier
# \param src the source channel
# \param dst the dest channel
# \param clid the callerid
# \param clidn the calleridname
# \return none
def handle_ami_event_dial(listkeys, astnum, src, dst, clid, clidn):
        global plist
        plist[astnum].normal_channel_fills(src, DUMMY_MYNUM,
                                           "Calling", 0, DIR_TO_STRING,
                                           dst, DUMMY_EXTEN,
                                           "ami-ed1")
        plist[astnum].normal_channel_fills(dst, DUMMY_MYNUM,
                                           "Ringing", 0, DIR_FROM_STRING,
                                           src, clid,
                                           "ami-ed2")


## \brief Updates some channels according to the Link events occuring in the AMI.
# \param listkeys the list of allowed phones
# \param astnum the Asterisk numerical identifier
# \param src the source channel
# \param dst the dest channel
# \param clid1 the src callerid
# \param clid2 the dest callerid
# \return none
def handle_ami_event_link(listkeys, astnum, src, dst, clid1, clid2):
        global plist
        if src not in plist[astnum].star10:
                plist[astnum].normal_channel_fills(src, DUMMY_MYNUM,
                                                   "On the phone", 0, DIR_TO_STRING,
                                                   dst, clid2,
                                                   "ami-el1")
        if dst not in plist[astnum].star10:
                plist[astnum].normal_channel_fills(dst, DUMMY_MYNUM,
                                                   "On the phone", 0, DIR_FROM_STRING,
                                                   src, clid1,
                                                   "ami-el2")


## \brief Fills the star10 field on unlink events.
# \param listkeys the list of allowed phones
# \param astnum the Asterisk numerical identifier
# \param src the source channel
# \param dst the dest channel
# \param clid1 the src callerid
# \param clid2 the dest callerid
# \return none
def handle_ami_event_unlink(listkeys, astnum, src, dst, clid1, clid2):
        global plist
        if src not in plist[astnum].star10:
                plist[astnum].star10.append(src)
        if dst not in plist[astnum].star10:
                plist[astnum].star10.append(dst)


## \brief Updates some channels according to the Hangup events occuring in the AMI.
# \param listkeys the list of allowed phones
# \param astnum the Asterisk numerical identifier
# \param chan the channel
# \param cause the reason why there has been a hangup (not used)
# \return
def handle_ami_event_hangup(listkeys, astnum, chan, cause):
        global plist
        if chan in plist[astnum].star10:
                plist[astnum].star10.remove(chan)
        plist[astnum].normal_channel_hangup(chan, "ami-eh0")


## \brief Handling of AMI events occuring in Events=on mode.
# \param astnum the asterisk numerical identifier
# \param idata the data read from the AMI we want to parse
# \return none
# \sa handle_ami_event_dial, handle_ami_event_link, handle_ami_event_hangup
def handle_ami_event(astid, idata):
        global plist, save_for_next_packet_events
        if astid in asteriskr:
                astnum = asteriskr[astid]
        else:
                log_debug("%s : no such asterisk Id" % astid)
                return

        listkeys = plist[astnum].normal.keys()
        full_idata = save_for_next_packet_events[astnum] + idata
        evlist = full_idata.split("\r\n\r\n")
        save_for_next_packet_events[astnum] = evlist.pop()
        
        for evt in evlist:
                this_event = {}
                for myline in evt.split('\r\n'):
                        myfieldvalue = myline.split(': ', 1)
                        if len(myfieldvalue) == 2:
                                this_event[myfieldvalue[0]] = myfieldvalue[1]
                evfunction = this_event.get('Event')
                verboselog("/%s/ %s" %(plist[astnum].astid, str(this_event)), True, False)
                if evfunction == 'Dial':
                        src     = this_event.get("Source")
                        dst     = this_event.get("Destination")
                        clid    = this_event.get("CallerID")
                        clidn   = this_event.get("CallerIDName")
                        context = this_event.get("Context")
                        try:
                                handle_ami_event_dial(listkeys, astnum, src, dst, clid, clidn)
                                #print "dial", context, x
                        except Exception, exc:
                                log_debug("--- exception --- handle_ami_event_dial : " + str(exc))
                elif evfunction == 'Link':
                        src     = this_event.get("Channel1")
                        dst     = this_event.get("Channel2")
                        clid1   = this_event.get("CallerID1")
                        clid2   = this_event.get("CallerID2")
                        context = this_event.get("Context")
                        try:
                                handle_ami_event_link(listkeys, astnum, src, dst, clid1, clid2)
                                #print "link", context, x
                        except Exception, exc:
                                log_debug("--- exception --- handle_ami_event_link : " + str(exc))
                elif evfunction == 'Unlink':
                        # there might be something to parse here
                        src   = this_event.get("Channel1")
                        dst   = this_event.get("Channel2")
                        clid1 = this_event.get("CallerID1")
                        clid2 = this_event.get("CallerID2")
                        try:
                                handle_ami_event_unlink(listkeys, astnum, src, dst, clid1, clid2)
                                #print "unlink", context, this_event
                        except Exception, exc:
                                log_debug("--- exception --- handle_ami_event_unlink : " + str(exc))
                elif evfunction == 'Hangup':
                        chan  = this_event.get("Channel")
                        cause = this_event.get("Cause-txt")
                        try:
                                handle_ami_event_hangup(listkeys, astnum, chan, cause)
                        except Exception, exc:
                                log_debug("--- exception --- handle_ami_event_hangup: " + str(exc))
                elif evfunction == 'Reload':
                        # warning : "reload" as well as "reload manager" can appear here
                        log_debug("AMI:Reload: " + plist[astnum].astid)
                        do_sip_register(configs[astnum], SIPsocks[astnum])
                elif evfunction == 'Shutdown':
                        log_debug("AMI:Shutdown: " + plist[astnum].astid)
                elif evfunction == 'Join':
                        clid  = this_event.get("CallerID")
                        qname = this_event.get("Queue")
                        if len(clid) > 0:
                                for k in tcpopens_sb:
                                        k[0].send("message=%s::<%s> is calling the Queue <%s>\n" %(DAEMON, clid, qname))
                elif evfunction == 'PeerStatus':
                        # <-> register's ? notify's ?
                        pass
                elif evfunction == 'Agentlogin':
                        log_debug("//AMI:Agentlogin// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'Agentlogoff':
                        log_debug("//AMI:Agentlogoff// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'Agentcallbacklogin':
                        log_debug("//AMI:Agentcallbacklogin// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'Agentcallbacklogoff':
                        log_debug("//AMI:Agentcallbacklogoff// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'AgentCalled':
                        log_debug("//AMI:AgentCalled// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'ParkedCallsComplete':
                        log_debug("//AMI:ParkedCallsComplete// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'ParkedCalled':
                        log_debug("//AMI:ParkedCalled// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'Cdr':
                        log_debug("//AMI:Cdr// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'Alarm':
                        log_debug("//AMI:Alarm// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'AlarmClear':
                        log_debug("//AMI:AlarmClear// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'FaxReceived':
                        log_debug("//AMI:FaxReceived// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'Registry':
                        log_debug("//AMI:Registry// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'MeetmeJoin':
                        channel = this_event.get("Channel")
                        meetme  = this_event.get("Meetme")
                        usernum = this_event.get("Usernum")
                        log_debug("AMI:MeetmeJoin %s : %s %s %s"
                                  %(plist[astnum].astid, channel, meetme, usernum))
                elif evfunction == 'MeetmeLeave':
                        channel = this_event.get("Channel")
                        meetme  = this_event.get("Meetme")
                        usernum = this_event.get("Usernum")
                        log_debug("AMI:MeetmeLeave %s : %s %s %s"
                                  %(plist[astnum].astid, channel, meetme, usernum))
                elif evfunction == 'ExtensionStatus':
                        exten   = this_event.get("Exten")
                        context = this_event.get("Context")
                        status  = this_event.get("Status")
                        log_debug("AMI:ExtensionStatus: %s : %s %s %s"
                                  %(plist[astnum].astid, exten, context, status))
                        # QueueMemberStatus ExtensionStatus
                        #                 0                  AST_DEVICE_UNKNOWN
                        #                 1               0  AST_DEVICE_NOT_INUSE  /  libre
                        #                 2               1  AST_DEVICE IN USE     / en ligne
                        #                 3                  AST_DEVICE_BUSY
                        #                                 4  AST_EXTENSION_UNAVAILABLE ?
                        #                 5                  AST_DEVICE_UNAVAILABLE
                        #                 6 AST_EXTENSION_RINGING = 8  appele
                elif evfunction == 'OriginateSuccess':
                        pass
                elif evfunction == 'OriginateFailure':
                        log_debug("AMI:OriginateFailure: " + plist[astnum].astid + \
                                  " - reason=" + this_event.get("Reason"))
                        #define AST_CONTROL_HANGUP              1
                        #define AST_CONTROL_RING                2
                        #define AST_CONTROL_RINGING             3
                        #define AST_CONTROL_ANSWER              4
                        #define AST_CONTROL_BUSY                5
                        #define AST_CONTROL_TAKEOFFHOOK         6
                        #define AST_CONTROL_OFFHOOK             7
                        #define AST_CONTROL_CONGESTION          8
                        #define AST_CONTROL_FLASH               9
                        #define AST_CONTROL_WINK                10
                elif evfunction == 'Rename':
                        # appears when there is a transfer
                        channel_old = this_event.get("Oldname")
                        channel_new = this_event.get("Newname")
                        if channel_old.find("<MASQ>") < 0 and channel_new.find("<MASQ>") < 0 and \
                               is_normal_channel(channel_old) and is_normal_channel(channel_new):
                                log_debug("AMI:Rename:N: %s : old=%s new=%s"
                                          %(plist[astnum].astid, channel_old, channel_new))
                                phone_old = channel_splitter(channel_old)
                                phone_new = channel_splitter(channel_new)

                                channel_p1 = plist[astnum].normal[phone_old].chann[channel_old].getChannelPeer()
                                channel_p2 = plist[astnum].normal[phone_new].chann[channel_new].getChannelPeer()
                                phone_p1 = channel_splitter(channel_p1)

                                if channel_p2 == "":
                                        # occurs when 72 (interception) is called
                                        # A is calling B, intercepted by C
                                        # in this case old = B and new = C
                                        n1 = DUMMY_EXTEN
                                        n2 = DUMMY_EXTEN
                                else:
                                        phone_p2 = channel_splitter(channel_p2)
                                        n1 = plist[astnum].normal[phone_old].chann[channel_old].getChannelNum()
                                        n2 = plist[astnum].normal[phone_p2].chann[channel_p2].getChannelNum()

                                log_debug("updating channels <%s> (%s) and <%s> (%s) and hanging up <%s>"
                                          %(channel_new, n1, channel_p1, n2, channel_old))

                                try:
                                        plist[astnum].normal_channel_fills(channel_new, DUMMY_CLID,
                                                                           DUMMY_STATE, 0, DUMMY_DIR,
                                                                           channel_p1, n1, "ami-er1")
                                except Exception, exc:
                                        log_debug("--- exception --- %s : renaming (ami-er1) failed : %s" %(configs[astnum].astid, str(exc)))

                                try:
                                        if channel_p1 != "":
                                                plist[astnum].normal_channel_fills(channel_p1, DUMMY_CLID,
                                                                                   DUMMY_STATE, 0, DUMMY_DIR,
                                                                                   channel_new, n2, "ami-er2")
                                        else:
                                                log_debug("channel_p1 is empty - was it a group call that has been intercepted ?")
                                except Exception, exc:
                                        log_debug("--- exception --- %s : renaming (ami-er2) failed : %s" %(configs[astnum].astid, str(exc)))

                                try:
                                        plist[astnum].normal_channel_hangup(channel_old, "ami-er3")
                                except Exception, exc:
                                        log_debug("--- exception --- %s : renaming (ami-er3 = hangup) failed : %s" %(configs[astnum].astid, str(exc)))

                        else:
                                log_debug("AMI:Rename:A: %s : old=%s new=%s"
                                          %(plist[astnum].astid, channel_old, channel_new))
                elif evfunction == 'Newstate':
                        chan    = this_event.get("Channel")
                        clid    = this_event.get("CallerID")
                        clidn   = this_event.get("CallerIDName")
                        state   = this_event.get("State")
                        # state = Ringing, Up, Down
                        plist[astnum].normal_channel_fills(chan, clid,
                                                           state, 0, DUMMY_DIR,
                                                           DUMMY_RCHAN, DUMMY_EXTEN, "ami-ns0")
                elif evfunction == 'Newcallerid':
                        # for tricky queues' management
                        chan    = this_event.get("Channel")
                        clid    = this_event.get("CallerID")
                        clidn   = this_event.get("CallerIDName")
                        log_debug("AMI:Newcallerid: " + plist[astnum].astid + \
                                  " channel=" + chan + " callerid=" + clid + " calleridname=" + clidn)
                        # plist[astnum].normal_channel_fills(chan, clid,
                        # DUMMY_STATE, 0, DUMMY_DIR,
                        # DUMMY_RCHAN, DUMMY_EXTEN, "ami-ni0")
                elif evfunction == 'Newchannel':
                        chan    = this_event.get("Channel")
                        clid    = this_event.get("CallerID")
                        state   = this_event.get("State")
                        # states = Ring, Down
                        if state == "Ring":
                                plist[astnum].normal_channel_fills(chan, clid,
                                                                   "Calling", 0, DIR_TO_STRING,
                                                                   DUMMY_RCHAN, DUMMY_EXTEN, "ami-nc0")
                        elif state == "Down":
                                plist[astnum].normal_channel_fills(chan, clid,
                                                                   "Ringing", 0, DIR_FROM_STRING,
                                                                   DUMMY_RCHAN, DUMMY_EXTEN, "ami-nc1")
                        # if not (clid == "" or (clid == "<unknown>" and is_normal_channel(chan))):
                        # for k in tcpopens_sb:
                        #       k[0].send("message=<" + clid + "> is entering the Asterisk <" + plist[astnum].astid + "> through " + chan + "\n")
                        # else:
                        # pass
                elif evfunction == 'Newexten': # in order to handle outgoing calls ?
                        chan    = this_event.get("Channel")
                        exten   = this_event.get("Extension")
                        context = this_event.get("Context")
                        if exten != "s" and exten != "h" and exten != "t" and exten != "enum":
                                #print "--- exten :", chan, exten
                                plist[astnum].normal_channel_fills(chan, DUMMY_MYNUM,
                                                                   "Calling", 0, DIR_TO_STRING,
                                                                   DUMMY_RCHAN, exten, "ami-ne0")
                        else:
                                pass
                        if this_event.get('Application') == 'Set' and this_event.get('AppData').find('DB') == 0:
                                try:
                                        # appdata = DB(local-extensions/users/103/DND)=0
                                        appdata = this_event.get("AppData")
                                        [ctx, grp, userid, feature] = appdata.split("(")[1].split(")=")[0].split("/", 3)
                                        value = appdata.split(")=")[1]
                                        strupdate = "featuresupdate=%s;%s;%s;%s;%s" %(configs[astnum].astid, ctx, userid, feature, value)
                                        send_msg_to_cti_clients(strupdate)

                                except Exception, exc:
                                        log_debug("--- exception --- (newexten, appdata) %s" %str(exc))
                elif evfunction == 'MessageWaiting':
                        mwi_string = "%s waiting=%s; new=%s; old=%s" \
                                     % (this_event.get('Mailbox'), str(this_event.get('Waiting')), str(this_event.get("New")), str(this_event.get("Old")))
                        log_debug("AMI:MessageWaiting: " + plist[astnum].astid + " : " + mwi_string)
                elif evfunction == 'QueueParams':
                        log_debug("//AMI:QueueParams// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'QueueMember':
                        log_debug("//AMI:QueueMember// %s : %s" %(plist[astnum].astid, str(this_event)))
                elif evfunction == 'QueueMemberStatus':
                        queuenameq = this_event.get("Queue")
                        location   = this_event.get("Location")
                        status     = this_event.get("Status")
                        log_debug("AMI:QueueMemberStatus: " + plist[astnum].astid + " " + queuenameq + " " + location + " " + status)
                elif evfunction == 'Leave':
                        queuenameq = this_event.get("Queue")
                        log_debug("AMI:Leave: " + plist[astnum].astid + " " + queuenameq)
                elif len(this_event) > 0:
                        log_debug("AMI:XXX: <%s> : %s" % (plist[astnum].astid, str(this_event)))


## \brief Handling of AMI events for the initial Status Command.
# These are AMI events received as a reply to a command.
# \param astnum the asterisk numerical identifier
# \param idata the data read from the AMI we want to parse
# \return
def handle_ami_status(astid, idata):
        global plist, save_for_next_packet_status
        if astid in asteriskr:
                astnum = asteriskr[astid]
        else:
                log_debug("%s : no such asterisk Id" % astid)
                return

        listkeys = plist[astnum].normal.keys()
        full_idata = save_for_next_packet_status[astnum] + idata
        evlist = full_idata.split("\r\n\r\n")
        save_for_next_packet_status[astnum] = evlist.pop()

        for evt in evlist:
                this_event = {}
                for myline in evt.split('\r\n'):
                        myfieldvalue = myline.split(': ', 1)
                        if len(myfieldvalue) == 2:
                                this_event[myfieldvalue[0]] = myfieldvalue[1]
                evfunction = this_event.get('Event')
                if evfunction == 'Status':
                        state = this_event.get('State')
                        if state == 'Up':
                                chan    = this_event.get('Channel')
                                clid    = this_event.get('CallerID')
                                link    = this_event.get('Link')
                                exten   = this_event.get('Extension')
                                seconds = this_event.get('Seconds')
                                if link is not None:
                                        if seconds is None:
                                                # this is where the right callerid is set, esp in outgoing calls
                                                plist[astnum].normal_channel_fills(link, DUMMY_MYNUM,
                                                                                   "On the phone", 0, DIR_TO_STRING,
                                                                                   chan, clid,
                                                                                   "ami-st1")
                                                plist[astnum].normal_channel_fills(chan, DUMMY_MYNUM,
                                                                                   "On the phone", 0, DIR_FROM_STRING,
                                                                                   link, '',
                                                                                   "ami-st2")
                                        else:
                                                # this is where the right time of ongoing calls is set
                                                plist[astnum].normal_channel_fills(link, DUMMY_MYNUM,
                                                                                   "On the phone", int(seconds), DIR_FROM_STRING,
                                                                                   chan, clid,
                                                                                   "ami-st1")
                                                plist[astnum].normal_channel_fills(chan, DUMMY_MYNUM,
                                                                                   "On the phone", int(seconds), DIR_TO_STRING,
                                                                                   link, exten,
                                                                                   "ami-st2")
                                else:
                                        # we fall here when there is a MeetMe conference
                                        log_debug("AMI %s Status / linked with noone (Meetme conf, voicemail ...) : chan=<%s>, clid=<%s>, exten=<%s>, seconds=<%s>"
                                                  % (astid, chan, clid, exten, seconds))
                                        # reply = AMI_array_user_commands[astid].execclicommand("show channel %s" % chan)
                                        # for z in reply:
                                        # if z.find('Application') >= 0 or z.find('Data') >= 0:
                                        # print chan, z.split(':')
                                        plist[astnum].normal_channel_fills(chan, DUMMY_MYNUM,
                                                                           "On the phone", int(seconds), DIR_TO_STRING,
                                                                           DUMMY_RCHAN, exten,
                                                                           "ami-st2")

                        elif state == 'Ring': # AST_STATE_RING
                                log_debug("AMI %s Status / Ring (To): %s %s %s"
                                          % (astid, this_event.get('Channel'),
                                             this_event.get('Extension'), this_event.get('Seconds')))
                        elif state == 'Ringing': # AST_STATE_RINGING
                                log_debug("AMI %s Status / Ringing (From): %s"
                                          % (astid, this_event.get("Channel")))
                        elif state == 'Rsrvd': # AST_STATE_RESERVED
                                # occurs in in meetme : AMI obelisk Status / Rsrvd: Zap/pseudo-1397436026
                                log_debug("AMI %s Status / Rsrvd: %s"
                                          % (astid, this_event.get("Channel")))
                        else: # Down, OffHook, Dialing, Up, Busy
                                log_debug("AMI %s Status / (other status event) : %s"
                                          % (astid, str(this_event)))
                elif evfunction == 'StatusComplete':
                        log_debug("AMI %s StatusComplete" % astid)
                elif this_event.get('Response') == 'Follows' and this_event.get('Privilege') == 'Command':
                        log_debug("AMI %s Response=Follows : %s" % (astid, str(this_event)))
                elif this_event.get('Response') == 'Success':
                        log_debug("AMI %s Response=Success : %s" % (astid, str(this_event)))
                else:
                        log_debug("AMI %s Other : %s" % (astid, str(this_event)))

"""
"""


## \brief Connects to the AMI if not yet.
# \param astnum Asterisk id to (re)connect
# \return none
def update_amisocks(astnum, astid):
        try:
                if astid not in AMI_array_events_on or AMI_array_events_on[astid] is False:
                        log_debug("%s : AMI (events = off) : attempting to connect" % astid)
                        als0 = connect_to_AMI((configs[astnum].remoteaddr,
                                               configs[astnum].ami_port),
                                              configs[astnum].ami_login,
                                              configs[astnum].ami_pass,
                                              False)
                        if als0:
                                AMI_array_events_on[astid] = als0.f
                                ins.append(AMI_array_events_on[astid])
                                log_debug("%s : AMI (events = off) : connected" % astid)
                                """Clears the channels before requesting a new status"""
                                for x in plist[astnum].normal.itervalues():
                                        x.clear_channels()
                                ret = als0.sendstatus()
                                if not ret:
                                        log_debug("%s : could not send status command" % astid)
                        else:
                                log_debug("%s : AMI (events = off) : could NOT connect" % astid)
        except Exception, exc:
                log_debug("--- exception --- %s (update_amisocks events = off) : %s" % (astid, str(exc)))
        
        try:
                if astid not in AMI_array_events_off or AMI_array_events_off[astid] is False:
                        log_debug("%s : AMI (events = on)  : attempting to connect" % astid)
                        als1 = connect_to_AMI((configs[astnum].remoteaddr,
                                               configs[astnum].ami_port),
                                              configs[astnum].ami_login,
                                              configs[astnum].ami_pass,
                                              True)
                        if als1:
                                AMI_array_events_off[astid] = als1.f
                                ins.append(als1.f)
                                log_debug("%s : AMI (events = on)  : connected" % astid)
                        else:
                                log_debug("%s : AMI (events = on)  : could NOT connect" % astid)
        except Exception, exc:
                log_debug("--- exception --- %s (update_amisocks events = on) : %s" % (astid, str(exc)))

        try:
                if astid not in AMI_array_user_commands or AMI_array_user_commands[astid] is False:
                        log_debug("%s : AMI (commands)  : attempting to connect" % astid)
                        als1 = connect_to_AMI((configs[astnum].remoteaddr,
                                               configs[astnum].ami_port),
                                              configs[astnum].ami_login,
                                              configs[astnum].ami_pass,
                                              False)
                        if als1:
                                AMI_array_user_commands[astid] = als1
                                log_debug("%s : AMI (commands)  : connected" % astid)
                        else:
                                log_debug("%s : AMI (commands)  : could NOT connect" % astid)
        except Exception, exc:
                log_debug("--- exception --- %s (update_amisocks events = on) : %s" % (astid, str(exc)))



## \brief Updates the list of sip numbers according to the SSO then sends old and new peers to the UIs.
# The reconnection to the AMI is also done here when it has been broken.
# If the AMI sockets are dead, a reconnection is also attempted here.
# \param astnum the asterisk numerical identifier
# \return none
# \sa update_userlist_fromurl
def update_sipnumlist(astnum):
        global plist, configs

        userlist_lock.acquire()
        try:
                for user,userinfo in userlist[configs[astnum].astid].iteritems():
                        if "sessiontimestamp" in userinfo:
                                if time.time() - userinfo.get('sessiontimestamp') > xivoclient_session_timeout:
                                        log_debug("%s : timeout reached for %s" %(plist[astnum].astid, user))
                                        disconnect_user(userinfo)
                                        send_availstate_update(astnum, user, "unknown")
        finally:
                userlist_lock.release()

        sipnumlistold = filter(lambda j: plist[astnum].normal[j].towatch, plist[astnum].normal)
        sipnumlistold.sort()
        try:
                sipnuml = configs[astnum].update_userlist_fromurl()
        except Exception, exc:
                log_debug("--- exception --- %s : update_userlist_fromurl failed : %s" %(configs[astnum].astid,str(exc)))
                sipnuml = {}
        for x in configs[astnum].extrachannels.split(","):
                if x != "": sipnuml[x] = [x, "", "", x.split("/")[1], ""]
        sipnumlistnew = sipnuml.keys()
        sipnumlistnew.sort()

        lstdel = ""
        lstadd = ""
        for snl in sipnumlistold:
                if snl not in sipnumlistnew:
                        lstdel += "del:" + configs[astnum].astid + ":" + build_basestatus(plist[astnum].normal[snl]) + ";"
                        del plist[astnum].normal[snl] # or = "Absent"/0 ?
                else:
                        plist[astnum].normal[snl].updateIfNeeded(sipnuml[snl])
        for snl in sipnumlistnew:
                if snl not in sipnumlistold:
                        if snl.find("SIP") == 0:
                                plist[astnum].normal[snl] = LineProp("SIP",
                                                                     snl.split("/")[1],
                                                                     sipnuml[snl][3],
                                                                     sipnuml[snl][4],
                                                                     "BefSubs", True)
                        elif snl.find("IAX2") == 0:
                                plist[astnum].normal[snl] = LineProp("IAX2",
                                                                     snl.split("/")[1],
                                                                     sipnuml[snl][3],
                                                                     sipnuml[snl][4],
                                                                     "Ready", True)
                        elif snl.find("mISDN") == 0:
                                plist[astnum].normal[snl] = LineProp("mISDN",
                                                                     snl.split("/")[1],
                                                                     sipnuml[snl][3],
                                                                     sipnuml[snl][4],
                                                                     "Ready", True)
                        elif snl.find("Zap") == 0:
                                plist[astnum].normal[snl] = LineProp("Zap",
                                                                     snl.split("/")[1],
                                                                     sipnuml[snl][3],
                                                                     sipnuml[snl][4],
                                                                     "Ready", True)
                        else:
                                log_debug(snl + " format not supported")
                                
                        if snl in plist[astnum].normal:
                                plist[astnum].normal[snl].set_callerid(sipnuml[snl])

                        lstadd += "add:" + configs[astnum].astid + ":" + build_basestatus(plist[astnum].normal[snl]) + ":0:" \
                                  + build_cidstatus(plist[astnum].normal[snl]) + ";"
        if lstdel != "":
                strupdate = "peerremove=" + lstdel
                send_msg_to_cti_clients(strupdate)
                verboselog(strupdate, False, True)


        if lstadd != "":
                strupdate = "peeradd=" + lstadd
                send_msg_to_cti_clients(strupdate)
                verboselog(strupdate, False, True)


## \brief Connects to the AMI through AMIClass.
# \param address IP address
# \param loginname loginname
# \param password password
# \return the socket
def connect_to_AMI(address, loginname, password, events_on):
        lAMIsock = xivo_ami.AMIClass(address, loginname, password, events_on)
        try:
                lAMIsock.connect()
                lAMIsock.login()
        except socket.timeout: pass
        except socket:         pass
        except:
                del lAMIsock
                lAMIsock = False
        return lAMIsock


## \class LocalChannel
# \brief Properties of a temporary "Local" channel.
class LocalChannel:
        # \brief Class initialization.
        def __init__(self, istate, icallerid):
                self.state = istate
                self.callerid = icallerid
                self.peer = ""
        # \brief Sets the state and the peer channel name.
        def set_chan(self, istate, ipeer):
                self.state = istate
                if ipeer != "":
                        self.peer = ipeer
        def set_callerid(self, icallerid):
                self.callerid = icallerid


## \class PhoneList
# \brief Properties of the lines of a given Asterisk
class PhoneList:
        ## \var astid
        # \brief Asterisk id, the same as the one given in the configs

        ## \var normal
        # \brief "Normal" phone lines, like SIP, IAX, Zap, ...

        ##  \brief Class initialization.
        def __init__(self, iastid):
                self.astid = iastid
                self.normal = {}
                self.star10 = []

        def update_GUI_clients(self, phonenum, fromwhom):
                global tcpopens_sb
                phoneinfo = fromwhom + ":" + self.astid + ":" + build_basestatus(self.normal[phonenum])
                fstatlist = build_fullstatlist(self.normal[phonenum])
                if self.normal[phonenum].towatch: fstr = "update="
                else:                             fstr = "______="
                strupdate = fstr + phoneinfo + ":" + fstatlist
                send_msg_to_cti_clients(strupdate)
                verboselog(strupdate, False, True)


        def normal_channel_fills(self, chan_src, num_src,
                                 action, timeup, direction,
                                 chan_dst, num_dst, comment):
                phoneid_src = channel_splitter(chan_src)
                phoneid_dst = channel_splitter(chan_dst)

                if phoneid_src not in self.normal:
                        self.normal[phoneid_src] = LineProp(phoneid_src.split("/")[0],
                                                            phoneid_src.split("/")[1],
                                                            phoneid_src.split("/")[1],
                                                            "which-context?", "sipstatus?", False)
                do_update = self.normal[phoneid_src].set_chan(chan_src, action, timeup, direction, chan_dst, num_dst, num_src)
                if do_update:
                        self.update_GUI_clients(phoneid_src, comment + "F")


        def normal_channel_hangup(self, chan_src, comment):
                phoneid_src = channel_splitter(chan_src)
                if phoneid_src not in self.normal:
                        self.normal[phoneid_src] = LineProp(phoneid_src.split("/")[0],
                                                            phoneid_src.split("/")[1],
                                                            phoneid_src.split("/")[1],
                                                            "which-context?", "sipstatus?", False)
                self.normal[phoneid_src].set_chan_hangup(chan_src)
                self.update_GUI_clients(phoneid_src, comment + "H")
                self.normal[phoneid_src].del_chan(chan_src)
                self.update_GUI_clients(phoneid_src, comment + "D")
                if len(self.normal[phoneid_src].chann) == 0 and self.normal[phoneid_src].towatch == False:
                        del self.normal[phoneid_src]


## \class ChannelStatus
# \brief Properties of a Channel, as given by the AMI.
class ChannelStatus:
        ## \var status
        # \brief Channel status

        ## \var deltatime
        # \brief Elapsed time spent by the channel with the current status

        ## \var time
        # \brief Absolute time

        ## \var direction
        # \brief "To" or "From"

        ## \var channel_peer
        # \brief Channel name of the peer with whom it is in relation

        ## \var channel_num
        # \brief Phone number of the peer with whom it is in relation

        ##  \brief Class initialization.
        def __init__(self, istatus, dtime, idir, ipeerch, ipeernum, itime, imynum):
                self.status = istatus
                self.deltatime = dtime
                self.time = itime
                self.direction = idir
                self.channel_peer = ipeerch
                self.channel_num = ipeernum
                self.channel_mynum = imynum
        def updateDeltaTime(self, dtime):
                self.deltatime = dtime
        def setChannelPeer(self, ipeerch):
                self.channel_peer = ipeerch
        def setChannelNum(self, ipeernum):
                self.channel_num = ipeernum

        def getChannelPeer(self):
                return self.channel_peer
        def getChannelNum(self):
                return self.channel_num
        def getChannelMyNum(self):
                return self.channel_mynum
        def getDirection(self):
                return self.direction
        def getTime(self):
                return self.time
        def getDeltaTime(self):
                return self.deltatime
        def getStatus(self):
                return self.status


## \class LineProp
# \brief Properties of a phone line. It might contain many channels.
class LineProp:
        ## \var tech
        # \brief Protocol of the phone (SIP, IAX2, ...)
        
        ## \var phoneid
        # \brief Phone identifier
        
        ## \var phonenum
        # \brief Phone number
        
        ## \var context
        # \brief Context
        
        ## \var lasttime
        # \brief Last time the phone has received a reply from a SUBSCRIBE
        
        ## \var chann
        # \brief List of Channels, with their properties as ChannelStatus
        
        ## \var sipstatus
        # \brief Status given through SIP presence detection
        
        ## \var imstat
        # \brief Instant Messaging status, as given by Xivo Clients
        
        ## \var voicemail
        # \brief Voicemail status
        
        ## \var queueavail
        # \brief Queue availability
        
        ## \var callerid
        # \brief Caller ID
        
        ## \var towatch
        # \brief Boolean value that tells whether this phone is watched by the switchboards
        
        ##  \brief Class initialization.
        def __init__(self, itech, iphoneid, iphonenum, icontext, isipstatus, itowatch):
                self.tech = itech
                self.phoneid  = iphoneid
                self.phonenum = iphonenum
                self.context = icontext
                self.lasttime = 0
                self.chann = {}
                self.sipstatus = isipstatus # Asterisk "hints" status
                self.imstat = "unknown"  # XMPP / Instant Messaging status
                self.voicemail = ""  # Voicemail status
                self.queueavail = "" # Availability as a queue member
                self.calleridfull = "nobody"
                self.calleridfirst = "nobody"
                self.calleridlast = "nobody"
                self.groups = ""
                self.towatch = itowatch
        def set_tech(self, itech):
                self.tech = itech
        def set_phoneid(self, iphoneid):
                self.phoneid = iphoneid
        def set_phonenum(self, iphonenum):
                self.phonenum = iphonenum
        def set_sipstatus(self, isipstatus):
                self.sipstatus = isipstatus
        def set_imstat(self, istatus):
                self.imstat = istatus
        def set_lasttime(self, ilasttime):
                self.lasttime = ilasttime
        def set_callerid(self, icallerid):
                self.calleridfull  = icallerid[0]
                self.calleridfirst = icallerid[1]
                self.calleridlast  = icallerid[2]
        def updateIfNeeded(self, icallerid):
                if icallerid[0:3] != (self.calleridfull, self.calleridfirst, self.calleridlast):
                        log_debug('updated parameters for user <%s/%s> : %s => %s'
                                  % (self.tech, self.phoneid,
                                     (self.calleridfull, self.calleridfirst, self.calleridlast),
                                     icallerid[0:3]))
                        self.calleridfull  = icallerid[0]
                        self.calleridfirst = icallerid[1]
                        self.calleridlast  = icallerid[2]
        ##  \brief Updates the time elapsed on a channel according to current time.
        def update_time(self):
                nowtime = time.time()
                for ic in self.chann:
                        dtime = int(nowtime - self.chann[ic].getTime())
                        self.chann[ic].updateDeltaTime(dtime)
        ##  \brief Removes all channels.
        def clear_channels(self):
                self.chann = {}
        ##  \brief Adds a channel or changes its properties.
        # If the values of status, itime, peerch and/or peernum are empty,
        # they are not updated : the previous value is kept.
        # \param ichan the Channel to hangup.
        # \param status the status to set
        # \param itime the elapsed time to set
        def set_chan(self, ichan, status, itime, idir, peerch, peernum, mynum):
                # print "<%s> <%s> <%s> <%s> <%s> <%s> <%s>" %(ichan, status, itime, idir, peerch, peernum, mynum)
                do_update = True
                if mynum == "<unknown>" and is_normal_channel(ichan):
                        mynum = channel_splitter(ichan)
                        #               if peernum == "<unknown>" and is_normal_channel(peerch):
                        #                       peernum = channel_splitter(peerch)
                # does not update peerch and peernum if the new values are empty
                newstatus = status
                newdir = idir
                newpeerch = peerch
                newpeernum = peernum
                newmynum = mynum
                if ichan in self.chann:
                        thischannel = self.chann[ichan]
                        oldpeernum = thischannel.getChannelNum()

                        if status  == "": newstatus = thischannel.getStatus()
                        if idir    == "": newdir = thischannel.getDirection()
                        if peerch  == "": newpeerch = thischannel.getChannelPeer()
                        if oldpeernum != "": newpeernum = oldpeernum
                        if mynum   == "": newmynum = thischannel.getChannelMyNum()

                        # mynum != thischannel.getChannelMyNum()
                        # when dialing "*10", there are many successive newextens occuring, that reset
                        # the time counter to 0
                        if status == thischannel.getStatus() and \
                           idir == thischannel.getDirection() and \
                           peerch == thischannel.getChannelPeer() and \
                           peernum == thischannel.getChannelNum():
                                do_update = False

                if do_update:
                        firsttime = time.time()
                        self.chann[ichan] = ChannelStatus(newstatus, itime, newdir,
                                                          newpeerch, newpeernum, firsttime - itime,
                                                          newmynum)
                        for ic in self.chann:
                                self.chann[ic].updateDeltaTime(int(firsttime - self.chann[ic].getTime()))
                return do_update


        ##  \brief Hangs up a Channel.
        # \param ichan the Channel to hangup.
        def set_chan_hangup(self, ichan):
                nichan = ichan
                if ichan.find("<ZOMBIE>") >= 0:
                        log_debug("sch channel contains a <ZOMBIE> part (%s) : sending hup to %s anyway" %(ichan,nichan))
                        nichan = ichan.split("<ZOMBIE>")[0]
                firsttime = time.time()
                self.chann[nichan] = ChannelStatus("Hangup", 0, "", "", "", firsttime, "")
                for ic in self.chann:
                        self.chann[ic].updateDeltaTime(int(firsttime - self.chann[ic].getTime()))

        ##  \brief Removes a Channel.
        # \param ichan the Channel to remove.
        def del_chan(self, ichan):
                nichan = ichan
                if ichan.find("<ZOMBIE>") >= 0:
                        log_debug("dch channel contains a <ZOMBIE> part (%s) : deleting %s anyway" %(ichan,nichan))
                        nichan = ichan.split("<ZOMBIE>")[0]
                if nichan in self.chann: del self.chann[nichan]


class Context:
        def __init__(self):
                self.uri = ""
                self.sqltable = ""
                self.search_titles = []
                self.search_valid_fields = []
                self.search_matching_fields = []
                self.sheet_valid_fields = []
                self.sheet_matching_fields = []
                self.sheet_callidmatch = []
        def setUri(self, uri):
                self.uri = uri
        def setSqlTable(self, sqltable):
                self.sqltable = sqltable

        def setSearchValidFields(self, vf):
                self.search_valid_fields = vf
                for x in vf:
                        self.search_titles.append(x[0])
        def setSearchMatchingFields(self, mf):
                self.search_matching_fields = mf

        def setSheetValidFields(self, vf):
                self.sheet_valid_fields = vf
        def setSheetMatchingFields(self, mf):
                self.sheet_matching_fields = mf
        def setSheetCallidMatch(self, cidm):
                self.sheet_callidmatch = cidm

        def result_by_valid_field(self, result):
                reply_by_field = []
                for [dummydispname, dbnames_list, keepspaces] in self.search_valid_fields:
                        field_value = ""
                        for dbname in dbnames_list:
                                if dbname in result and field_value is "":
                                        field_value = result[dbname]
                        if keepspaces:
                                reply_by_field.append(field_value)
                        else:
                                reply_by_field.append(field_value.replace(' ', ''))
                return reply_by_field



## \class AsteriskRemote
# \brief Properties of an Asterisk server
class AsteriskRemote:
        ## \var astid
        # \brief Asterisk String ID
        
        ## \var userlisturl
        # \brief Asterisk's URL
        
        ## \var extrachannels
        # \brief Comma-separated List of the Channels not present in the SSO

        ## \var localaddr
        # \brief Local IP address

        ## \var remoteaddr
        # \brief Address of the Asterisk server

        ## \var ipaddress_php
        # \brief IP address allowed to send CLI commands

        ## \var portsipclt
        # \brief Local SIP port for the monitored Asterisk

        ## \var portsipsrv
        # \brief SIP port of the monitored Asterisk

        ## \var mysipaccounts
        # \brief SIP identifier as registered on the monitored Asterisk

        ## \var ami_port
        # \brief AMI port of the monitored Asterisk

        ## \var ami_login
        # \brief AMI login of the monitored Asterisk

        ## \var ami_pass
        # \brief AMI password of the monitored Asterisk
        
        ##  \brief Class initialization.
        def __init__(self,
                     astid,
                     userlisturl,
                     extrachannels,
                     localaddr = "127.0.0.1",
                     remoteaddr = "127.0.0.1",
                     ipaddress_php = "127.0.0.1",
                     ami_port = 5038,
                     ami_login = "xivouser",
                     ami_pass = "xivouser",
                     portsipclt = 5005,
                     portsipsrv = 5060,
                     sipaccounts = "",
                     contexts = "",
                     cdr_db_uri = "",
                     realm = "asterisk"):

                self.astid = astid
                self.userlisturl = userlisturl
                self.extrachannels = extrachannels
                self.localaddr = localaddr
                self.remoteaddr = remoteaddr
                self.ipaddress_php = ipaddress_php
                self.portsipclt = portsipclt
                self.portsipsrv = portsipsrv
                self.ami_port = ami_port
                self.ami_login = ami_login
                self.ami_pass = ami_pass
                self.cdr_db_uri = cdr_db_uri
                self.realm = realm

                self.xivosb_phoneids = {}
                self.xivosb_contexts = {}
                if sipaccounts != "":
                        for sipacc in sipaccounts.split(","):
                                self.xivosb_phoneids[sipacc] = ["", ""]

                self.contexts = {}
                if contexts != "":
                        for ctx in contexts.split(","):
                                if ctx in xivoconf.sections():
                                        self.contexts[ctx] = dict(xivoconf.items(ctx))


        ## \brief Function to load sso.php user file.
        # SIP, Zap, mISDN and IAX2 are taken into account there.
        # There would remain MGCP, CAPI, h323, ...
        # \param url the url where lies the sso, it can be file:/ as well as http://
        # \param sipaccount the name of the reserved sip account (typically xivosb)
        # \return the new phone numbers list
        # \sa update_sipnumlist
        def update_userlist_fromurl(self):
                numlist = {}
                try:
                        f = urllib.urlopen(self.userlisturl)
                except Exception, exc:
                        log_debug("--- exception --- %s : unable to open URL %s : %s" %(self.astid, self.userlisturl, str(exc)))
                        return numlist

                try:
                        phone_list = []
                        # builds the phone_list from the SSO
                        for line in f:
                                # remove leading/tailing whitespaces
                                line = line.strip()
                                l = line.split('|')
                                if len(l) == 11 and l[6] == "0":
                                        phone_list.append(l)

                        # retrieves the xivosb account informations
                        found_xivosb = False
                        for l in phone_list:
                                [sso_tech, sso_phoneid, sso_passwd, sso_cinfo_allowed,
                                 sso_phonenum, sso_l5, sso_l6,
                                 fullname, firstname, lastname, sso_context] = l
                                for sipacc in self.xivosb_phoneids:
                                        if sipacc == sso_phoneid:
                                                found_xivosb = True
                                                # if this phoneid is a "xivosb" one
                                                if sso_context not in self.xivosb_contexts:
                                                        # only ONE xivosb is allowed for a given context
                                                        # and a xivo_daemon
                                                        self.xivosb_phoneids[sso_phoneid] = [sso_context, sso_passwd]
                                                        self.xivosb_contexts[sso_context] = sso_phoneid
                                                # elif self.xivosb_phoneids[sso_phoneid][0] == "":
                                                # #removes this xivosb account from the list if no context has been filled
                                                # del self.xivosb_phoneids[sso_phoneid]
                        if not found_xivosb:
                                log_debug("%s : WARNING : no xivosb-like account has been found on this asterisk" % self.astid)
                        log_debug("%s : xivosb_contexts = %s"
                                  %(self.astid, str(self.xivosb_contexts)))
                        log_debug("%s : xivosb_phoneids = %s"
                                  %(self.astid, str(self.xivosb_phoneids)))
                except Exception, exc:
                        log_debug("--- exception --- %s : a problem occured when building phone list and xivosb accounts : %s" %(self.astid, str(exc)))
                        return numlist
                
                try:
                        # updates other accounts
                        for l in phone_list:
                                try:
                                        # line is protocol | username | password | rightflag |
                                        #         phone number | initialized | disabled(=1) | callerid |
                                        #         firstname | lastname | context
                                        [sso_tech, sso_phoneid, sso_passwd, sso_cinfo_allowed,
                                         sso_phonenum, sso_l5, sso_l6,
                                         fullname, firstname, lastname, sso_context] = l
                                        
                                        if sso_context in self.xivosb_contexts:
                                                sipaccount = self.xivosb_contexts[sso_context]
                                                fullname = firstname + " " + lastname + " <b>" + sso_phonenum + "</b>"
                                                
                                                if sso_l5 == "1" and sso_phoneid != sipaccount and sso_phonenum != "":
                                                        if sso_tech == "sip":
                                                                argg = "SIP/" + sso_phoneid
                                                                adduser(self.astid, sso_tech + sso_phoneid, sso_passwd, sso_context, sso_phonenum, sso_cinfo_allowed)
                                                        elif sso_tech == "iax":
                                                                argg = "IAX2/" + sso_phoneid
                                                        elif sso_tech == "misdn":
                                                                argg = "mISDN/" + sso_phoneid
                                                        elif sso_tech == "zap":
                                                                argg = "Zap/" + sso_phoneid
                                                                adduser(self.astid, sso_tech + sso_phoneid, sso_passwd, sso_context, sso_phonenum, sso_cinfo_allowed)
                                                        numlist[argg] = fullname, firstname, lastname, sso_phonenum, sso_context
                                except Exception, exc:
                                        log_debug("--- exception --- %s : a problem occured when building phone list : %s" %(self.astid, str(exc)))
                                        return numlist
                finally:
                        f.close()
                return numlist


## \brief Adds (or updates) a user in the userlist.
# \param user the user to add
# \param passwd the user's passwd
# \return none
def adduser(astname, user, passwd, context, phonenum, cinfo_allowed):
        global userlist
        if userlist[astname].has_key(user):
                userlist[astname][user]['passwd']   = passwd
                userlist[astname][user]['context']  = context
                userlist[astname][user]['phonenum'] = phonenum
        else:
                userlist[astname][user] = {'user':user,
                                           'passwd':passwd,
                                           'context':context,
                                           'phonenum':phonenum,
                                           'capas':0}
        if cinfo_allowed == '1':
                userlist[astname][user]['capas'] = CAPA_CUSTINFO
        else:
                userlist[astname][user]['capas'] = 0
        # this list shall be defined through more options in SSO
        userlist[astname][user]['capas'] |= (CAPA_HISTORY  | CAPA_DIRECTORY | CAPA_PEERS |
                                             CAPA_PRESENCE | CAPA_DIAL      | CAPA_FEATURES |
                                             CAPA_AGENTS   | CAPA_FAX       | CAPA_SWITCHBOARD)


## \brief Deletes a user from the userlist.
# \param user the user to delete
# \return none
def deluser(astname, user):
        global userlist
        if userlist[astname].has_key(user):
                userlist[astname].pop(user)


def check_user_connection(userinfo, whoami):
        if userinfo.has_key('sessiontimestamp'):
                if time.time() - userinfo.get('sessiontimestamp') < xivoclient_session_timeout:
                        return "already_connected"
        if whoami == 'XC':
                if conngui_xc >= maxgui_xc:
                        return "xcusers:%d" % maxgui_xc
        else:
                if conngui_sb >= maxgui_sb:
                        return "sbusers:%d" % maxgui_sb
        return None


def connect_user(userinfo, sessionid, iip, iport,
                 whoami, whatsmyos, tcpmode, state,
                 lastconnwins, socket):
        global conngui_xc, conngui_sb
        try:
                userinfo['sessionid'] = sessionid
                userinfo['sessiontimestamp'] = time.time()
                userinfo['ip'] = iip
                userinfo['port'] = iport
                userinfo['cticlienttype'] = whoami
                userinfo['cticlientos'] = whatsmyos
                userinfo['tcpmode'] = tcpmode
                userinfo['socket'] = socket
                # lastconnwins was introduced in the aim of forcing a new connection to take on for
                # a given user, however it might breed problems if the previously logged-in process
                # tries to reconnect ... unless we send something asking to Kill the process
                userinfo['lastconnwins'] = lastconnwins

                # we first check if 'state' has already been set for this customer, in which case
                # the CTI clients will be sent back this previous state
                if 'state' in userinfo:
                        futurestate = userinfo.get('state')
                        # only if it was a "defined" state anyway
                        if futurestate in allowed_states:
                                state = futurestate

                if state in allowed_states:
                        userinfo['state'] = state
                else:
                        userinfo['state'] = "undefinedstate"
                if whoami == 'XC':
                        conngui_xc = conngui_xc + 1
                else:
                        conngui_sb = conngui_sb + 1
        except Exception, exc:
                log_debug("--- exception --- connect_user %s : %s" %(str(userinfo), str(exc)))


def disconnect_user(userinfo):
        global conngui_xc, conngui_sb
        try:
                if 'sessionid' in userinfo:
                        if userinfo.get('cticlienttype') == 'XC':
                                conngui_xc = conngui_xc - 1
                        else:
                                conngui_sb = conngui_sb - 1
                        del userinfo['sessionid']
                        del userinfo['sessiontimestamp']
                        del userinfo['ip']
                        del userinfo['port']
                        del userinfo['cticlienttype']
                        del userinfo['cticlientos']
                        del userinfo['tcpmode']
                        del userinfo['lastconnwins']
                        del userinfo['socket']
        except Exception, exc:
                log_debug("--- exception --- disconnect_user %s : %s" %(str(userinfo), str(exc)))


## \brief Returns the user from the list.
# \param user searched for
# \return user found, otherwise None
def finduser(astname, user):
        if astname in userlist:
                u = userlist[astname].get(user)
        else:
                u = None
        return u


##def askforparam(reqstring, rfile, wfile, debugstr):
##      wfile.write('Send %s for authentication\r\n' %reqstring)
##      list1 = rfile.readline().strip().split(' ')
##      if len(list1) != 2 or list1[0] != reqstring:
##              replystr = "ERROR : wrong format for %s reply" %reqstring
##              debugstr += " / %s error" %reqstring
##              return [replystr, debugstr], [user, port, state, astnum]
##      return list1[1]


## \class LoginHandler
# \brief The clients connect to this in order to obtain a valid session id.
# This could be enhanced to support a more complete protocol
# supporting commands coming from the client in order to pilot asterisk.
class LoginHandler(SocketServer.StreamRequestHandler):
        def logintalk(self):
                [astnum, user, port, state] = [-1, "", "", ""]
                replystr = "ERROR"
                debugstr = "LoginRequestHandler (TCP) : client = %s:%d" %(self.client_address[0], self.client_address[1])
                list1 = self.rfile.readline().strip().split(' ') # list1 should be "[LOGIN <asteriskname>/sip<nnn>]"
                nlist1 = len(list1)
                if nlist1 < 2 or nlist1 > 4 or list1[0] != 'LOGIN':
                        replystr = "ERROR number_of_arguments"
                        debugstr += " / LOGIN error args"
                        return [replystr, debugstr], [user, port, state, astnum]

                if list1[1].find("/") >= 0:
                        astname_xivoc = list1[1].split("/")[0]
                        user = list1[1].split("/")[1]
                else:
                        replystr = "ERROR id_format"
                        debugstr += " / LOGIN error ID"
                        return [replystr, debugstr], [user, port, state, astnum]
                
                whoami = ""
                whatsmyos = ""
                if nlist1 >= 3:
                        nwhoami = list1[2].split("@")
                        whoami  = nwhoami[0]
                        if len(nwhoami) == 2:
                                whatsmyos = nwhoami[1]
                if whoami not in ["XC", "SB"]:
                        log_debug("WARNING : %s/%s attempts to log in from %s:%d but has given no meaningful XC/SB hint (%s)"
                                  %(astname_xivoc, user, self.client_address[0], self.client_address[1], whoami))
                if whatsmyos not in ["X11", "WIN", "MAC"]:
                        log_debug("WARNING : %s/%s attempts to log in from %s:%d but has given no meaningful OS hint (%s)"
                                  %(astname_xivoc, user, self.client_address[0], self.client_address[1], whatsmyos))

                if nlist1 >= 4:
                        clientversion = int(list1[3])
                        if clientversion < REQUIRED_CLIENT_VERSION:
                                replystr = "ERROR version_client:%d;%d" % (clientversion, REQUIRED_CLIENT_VERSION)
                                debugstr += " / Client version Error %d < %d" %(clientversion, REQUIRED_CLIENT_VERSION)
                                return [replystr, debugstr], [user, port, state, astnum]

                # asks for PASS
                self.wfile.write('Send PASS for authentication\r\n')
                list1 = self.rfile.readline().strip().split(' ')
                if list1[0] == 'PASS':
                        passwd = ""
                        if len(list1) > 1: passwd = list1[1]
                else:
                        replystr = "ERROR pass_format"
                        debugstr += " / PASS error"
                        return [replystr, debugstr], [user, port, state, astnum]

                if astname_xivoc in asteriskr:
                        astnum = asteriskr[astname_xivoc]
                else:
                        replystr = "ERROR asterisk_name"
                        debugstr += " / asterisk name <%s> unknown" % astname_xivoc
                        return [replystr, debugstr], [user, port, state, astnum]
                userlist_lock.acquire()
                try:
                        userinfo = finduser(astname_xivoc, user)
                        goodpass = (userinfo != None) and (userinfo.get('passwd') == passwd)
                finally:
                        userlist_lock.release()
                if not goodpass:
                        replystr = "ERROR login_passwd"
                        debugstr += " / PASS KO (%s given) for %s on asterisk #%d" %(passwd, user,astnum)
                        return [replystr, debugstr], [user, port, state, astnum]
                
                # asks for PORT
                self.wfile.write('Send PORT command\r\n')
                list1 = self.rfile.readline().strip().split(' ')
                if len(list1) != 2 or list1[0] != 'PORT':
                        replystr = "ERROR PORT"
                        debugstr += " / PORT KO"
                        return [replystr, debugstr], [user, port, state, astnum]
                port = list1[1]
                
                # asks for STATE
                self.wfile.write('Send STATE command\r\n')
                list1 = self.rfile.readline().strip().split(' ')
                if len(list1) != 2 or list1[0] != 'STATE':
                        replystr = "ERROR STATE"
                        debugstr += " / STATE KO"
                        return [replystr, debugstr], [user, port, state, astnum]
                state = list1[1]
                
                capa_user = []
                userlist_lock.acquire()
                try:
                        reterror = check_user_connection(userinfo, whoami)
                        if reterror is None:
                                for capa in capabilities_list:
                                        if (map_capas[capa] & userinfo.get('capas')):
                                                capa_user.append(capa)

                                # TODO : random pas au top, faire generation de session id plus luxe
                                sessionid = '%u' % random.randint(0,999999999)
                                connect_user(userinfo, sessionid,
                                             self.client_address[0], port,
                                             whoami, whatsmyos, False, state,
                                             False, self.request.makefile('w'))

                                replystr = "OK SESSIONID %s " \
                                           "context:%s;phonenum:%s;capas:%s;" \
                                           "version:%s;state:%s" %(sessionid,
                                                                   userinfo.get('context'),
                                                                   userinfo.get('phonenum'),
                                                                   ",".join(capa_user),
                                                                   __version__.split()[1],
                                                                   userinfo.get('state'))
                        else:
                                replystr = "ERROR %s" % reterror
                                debugstr += " / USER %s (%s)" %(user, reterror)
                                return [replystr, debugstr], [user, port, state, astnum]

                finally:
                        userlist_lock.release()

                debugstr += " / user %s, port %s, state %s, astnum %d, cticlient %s/%s : connected : %s" %(user, port, state, astnum,
                                                                                                           whoami, whatsmyos,
                                                                                                           replystr)
                return [replystr, debugstr], [user, port, state, astnum]


        def handle(self):
                threading.currentThread().setName('login-%s:%d' %(self.client_address[0], self.client_address[1]))
                try:
                        [rstr, dstr], [user, port, state, astnum] = self.logintalk()
                        self.wfile.write(rstr + "\r\n")
                        log_debug(dstr)
                        if rstr.split()[0] == 'OK' and astnum >= 0:
                                send_availstate_update(astnum, user, state)
                except Exception, exc:
                        log_debug("--- exception --- %s" %(str(exc)))


## \class IdentRequestHandler
# \brief Gives client identification to the profile pusher.
# The connection is kept alive so several requests can be made on the same open TCP connection.
class IdentRequestHandler(SocketServer.StreamRequestHandler):
        def handle(self):
                threading.currentThread().setName('ident-%s:%d' %(self.client_address[0], self.client_address[1]))
                line = self.rfile.readline().strip()
                log_debug("IdentRequestHandler (TCP) : client = %s:%d / <%s>"
                          %(self.client_address[0],
                            self.client_address[1],
                            line))
                retline = 'ERROR'
                action = ""
                # PUSH user callerid msg
                m = re.match("PUSH (\S+) (\S+) <(\S*)> ?(.*)", line)
                if m != None:
                        user = m.group(1)
                        callerid = m.group(2)
                        callerctx = m.group(3)
                        msg = m.group(4)
                        action = "PUSH"
                else:
                        log_debug('PUSH command <%s> invalid' % line)
                        return

                if callerctx in contexts_cl:
                        ctxinfo = contexts_cl.get(callerctx)
                else:
                        log_debug('WARNING - no section has been defined for the context <%s>' % callerctx)
                        ctxinfo = contexts_cl.get('')

		userlist_lock.acquire()
		try:
			try:
				astnum = ip_reverse_sht[self.client_address[0]]
				userinfo = finduser(configs[astnum].astid, user)
                                state_userinfo = 'unknown'
                                
				if userinfo == None:
					log_debug('User <%s> not found' % user)
				elif userinfo.has_key('ip') and userinfo.has_key('port') \
					 and userinfo.has_key('state') and userinfo.has_key('sessionid') \
					 and userinfo.has_key('sessiontimestamp'):
					if time.time() - userinfo.get('sessiontimestamp') > xivoclient_session_timeout:
                                                log_debug('User <%s> session expired' % user)
                                                userinfo = None
					else:
						capalist = (userinfo.get('capas') & capalist_server)
						if (capalist & CAPA_CUSTINFO):
                                                        state_userinfo = userinfo.get('state')
                                                else:
                                                        userinfo = None
				else:
					log_debug('User <%s> session not defined' % user)
                                        userinfo = None

                                calleridname = sendfiche.sendficheasync(userinfo,
                                                                        ctxinfo,
                                                                        callerid,
                                                                        msg,
                                                                        xivoconf)
                                retline = 'USER %s STATE %s CIDNAME "%s <%s>"' %(user, state_userinfo, calleridname, callerid)
			except Exception, exc:
				retline = 'ERROR PUSH %s' %(str(exc))
		finally:
			userlist_lock.release()

                try:
                        log_debug("PUSH : replying <%s>" % retline)
                        self.wfile.write(retline + '\r\n')
                except Exception, exc:
                        # something bad happened.
                        log_debug("IdentRequestHandler/Exception: " + str(exc))
                        return


def send_availstate_update(astnum, username, state):
        try:
                if username.find("sip") == 0:
                        phoneid = "SIP/" + username.split("sip")[1]
                elif username.find("iax") == 0:
                        phoneid = "IAX/" + username.split("iax")[1]
                else:
                        phoneid = ""

                if phoneid in plist[astnum].normal:
                        if state == "unknown" or plist[astnum].normal[phoneid].imstat != state:
                                plist[astnum].normal[phoneid].set_imstat(state)
                                plist[astnum].normal[phoneid].update_time()
                                update_GUI_clients(astnum, phoneid, "kfc-sau")
                else:
                        log_debug("<%s> is not in my phone list" % phoneid)
        except Exception, exc:
                log_debug('--- exception --- send_availstate_update : %s' % str(exc))


def update_availstate(astnum, astid, username, state):
        do_state_update = False
        userlist_lock.acquire()
        try:
                userinfo = finduser(astid, username)
                if userinfo != None:
                        if 'sessiontimestamp' in userinfo:
                                userinfo['sessiontimestamp'] = time.time()
                        if state in allowed_states:
                                userinfo['state'] = state
                        else:
                                userinfo['state'] = "undefinedstate"
                        do_state_update = True
        finally:
                userlist_lock.release()

        if do_state_update:
                send_availstate_update(astnum, username, state)
        return ""


def parse_command_and_build_reply(me, myargs):
        repstr = ""
        astnum = me[0]
        try:
                capalist = (me[4] & capalist_server)
                if myargs[0] == 'history':
                        if (capalist & CAPA_HISTORY):
                                repstr = build_history_string(myargs[1], myargs[2], myargs[3])
                elif myargs[0] == 'directory-search':
                        if (capalist & CAPA_DIRECTORY):
                                repstr = build_customers(me[3], myargs[1:])
                elif myargs[0] == 'callerids':
                        if (capalist & (CAPA_PEERS | CAPA_HISTORY)):
                                repstr = build_callerids()
                elif myargs[0] == 'availstate':
                        if (capalist & CAPA_PRESENCE):
                                repstr = update_availstate(astnum, me[1], me[2], myargs[1])
                elif myargs[0] == 'hints':
                        if (capalist & (CAPA_PEERS | CAPA_HISTORY)):
                                repstr = build_statuses()
                elif myargs[0] == 'featuresget':
                        if (capalist & CAPA_FEATURES):
                                repstr = build_features_get(myargs[1:])
                elif myargs[0] == 'featuresput':
                        if (capalist & CAPA_FEATURES):
                                repstr = build_features_put(myargs[1:])
                elif myargs[0] == 'message':
                        if (capalist & CAPA_MESSAGE):
                                send_msg_to_cti_clients("message=%s/%s::<%s>\n" %(me[1], me[2], myargs[1]))
                elif myargs[0] == 'originate' or myargs[0] == 'transfer':
                        if (capalist & CAPA_DIAL):
                                repstr = originate_or_transfer("%s/%s" %(me[1], me[2]), [myargs[0], myargs[1], myargs[2]])
                elif myargs[0] == 'hangup':
                        if (capalist & CAPA_DIAL):
                                repstr = hangup("%s/%s" %(me[1], me[2]), [myargs[0], myargs[1]])
        except Exception, exc:
                log_debug("--- exception --- (parse_command_and_build_reply) %s %s" %(str(myargs), str(exc)))
        return repstr


def getboolean(fname, string):
        if string not in fname:
                return True
        else:
                value = fname.get(string)
                if value in ['false', '0', 'False']:
                        return False
                else:
                        return True


## \class KeepAliveHandler
# \brief It receives UDP datagrams and sends back a datagram containing whether
# "OK" or "ERROR <error-text>".
# It could be a good thing to give a numerical code to each error.
class KeepAliveHandler(SocketServer.DatagramRequestHandler):
        def handle(self):
                threading.currentThread().setName('keepalive-%s:%d' %(self.client_address[0], self.client_address[1]))
                requester = "%s:%d" %(self.client_address[0],self.client_address[1])
                log_debug("KeepAliveHandler    (UDP) : client = %s" %requester)
                astnum = -1
                response = "ERROR unknown"

                try:
                        ip = self.client_address[0]
                        list = self.request[0].strip().split(' ')
                        timestamp = time.time()
                        log_debug("received the message <%s> from %s" %(str(list), requester))

                        if len(list) < 4:
                                raise NameError, "not enough arguments (%d < 4)" %(len(list))
                        if list[0] != 'ALIVE' and list[0] != 'STOP' and list[0] != 'COMMAND':
                                raise NameError, "command %s not allowed" %(list[0])
                        if list[2] != 'SESSIONID':
                                raise NameError, "no SESSIONID defined"
                        [astname_xivoc, user] = list[1].split("/")
                        if astname_xivoc not in asteriskr:
                                raise NameError, "unknown asterisk name <%s>" %astname_xivoc
                        
                        astnum = asteriskr[astname_xivoc]
                        sessionid = list[3]
                        capalist_user = 0

                        # first we check that the requester has the good sessionid and that its
                        # session has not expired
                        userlist_lock.acquire()
                        try:
                                userinfo = finduser(astname_xivoc, user)
                                if userinfo == None:
                                        raise NameError, "unknown user %s" %user
                                if userinfo.has_key('sessionid') and userinfo.has_key('ip') and userinfo.has_key('sessiontimestamp') \
                                       and sessionid == userinfo.get('sessionid') and ip == userinfo.get('ip') \
                                       and userinfo.get('sessiontimestamp') + xivoclient_session_timeout > timestamp:
                                        userinfo['sessiontimestamp'] = timestamp
                                        response = 'OK'
                                        capalist_user = userinfo.get('capas')
                                else:
                                        raise NameError, "session_expired"
                        finally:
                                userlist_lock.release()
                        
                        if list[0] == 'ALIVE' and len(list) == 6 and list[4] == 'STATE':
                                # ALIVE user SESSIONID sessionid STATE state
                                state = list[5]
                                if state in allowed_states:
                                        userinfo['state'] = state
                                else:
                                        userinfo['state'] = "undefinedstate"
                                send_availstate_update(astnum, user, state)
                                response = 'OK'
                        elif list[0] == 'STOP' and len(list) == 4:
                                # STOP user SESSIONID sessionid
                                userlist_lock.acquire()
                                try:
                                        if list[1] in requestersocket_by_login:
                                                del requestersocket_by_login[list[1]]
                                        else:
                                                log_debug("warning : %s unknown" %(list[1]))
                                        disconnect_user(userinfo)
                                        send_availstate_update(astnum, user, "unknown")
                                finally:
                                        userlist_lock.release()
                                response = 'DISC'

                        elif list[0] == 'COMMAND':
                                try:
                                        userlist_lock.acquire()
                                        try:
                                                requestersocket_by_login[list[1]] = self
                                        finally:
                                                userlist_lock.release()
                                        
                                        response = parse_command_and_build_reply([astnum, astname_xivoc, user,
                                                                                  userinfo.get('context'), capalist_user],
                                                                                 list[4:])
                                except Exception, exc:
                                        log_debug("--- exception --- (command) %s" %str(exc))
                        else:
                                raise NameError, "unknown message <%s>" %str(list)
                except Exception, exc:
                        response = 'ERROR %s' %(str(exc))

                # whatever has been received, we must reply something to the client who asked
                log_debug("replying <%s> to %s" %(response, requester))
                self.request[1].sendto(response + '\r\n', self.client_address)


## \class MyTCPServer
# \brief TCPServer with the reuse address on.
class MyTCPServer(SocketServer.ThreadingTCPServer):
        allow_reuse_address = True


## \brief Handler for catching signals (in the main thread)
# \param signum signal number
# \param frame frame
# \return none
def sighandler(signum, frame):
        global askedtoquit
        print "--- signal", signum, "received : quits"
        askedtoquit = True


## \brief Handler for catching signals (in the main thread)
# \param signum signal number
# \param frame frame
# \return none
def sighandler_reload(signum, frame):
        global askedtoquit
        print "--- signal", signum, "received : reloads"
        askedtoquit = False

# ==============================================================================
# ==============================================================================

def log_stderr_and_syslog(x):
        print >> sys.stderr, x
        syslog.syslog(syslog.LOG_ERR, x)

# ==============================================================================
# Main Code starts here
# ==============================================================================

# daemonize if not in debug mode
if not debug_mode:
        daemonize.daemonize(log_stderr_and_syslog, PIDFILE, True)
else:
        daemonize.create_pidfile_or_die(log_stderr_and_syslog, PIDFILE, True)

signal.signal(signal.SIGINT, sighandler)
signal.signal(signal.SIGTERM, sighandler)
signal.signal(signal.SIGHUP, sighandler_reload)

nreload = 0

while True: # loops over the reloads
        askedtoquit = False

        time_start = time.time()
        if nreload == 0:
                log_debug("# STARTING XIVO Daemon # (0/3) Starting")
        else:
                log_debug("# STARTING XIVO Daemon # (0/3) Reloading (%d)" %nreload)
        nreload += 1
        
        # global default definitions
        port_login = 5000
        port_keepalive = 5001
        port_request = 5002
        port_ui_srv = 5003
        port_phpui_srv = 5004
        port_switchboard_base_sip = 5005
        xivoclient_session_timeout = 60
        xivosb_register_frequency = 60
        capabilities_list = []
        capalist_server = 0
        asterisklist = []
        contextlist = []
        maxgui_sb = 3
        maxgui_xc = 10
        conngui_xc = 0
        conngui_sb = 0
        evt_filename = "/var/log/pf-xivo-cti-server/ami_events.log"
        gui_filename = "/var/log/pf-xivo-cti-server/gui.log"
        with_ami = True
        with_advert = False
        nukeast = False

        userinfo_by_requester = {}
        requestersocket_by_login = {}

        xivoconf = ConfigParser.ConfigParser()
        xivoconf.readfp(open(xivoconffile))
        xivoconf_general = dict(xivoconf.items("general"))

        # loads the general configuration
        if "port_fiche_login" in xivoconf_general:
                port_login = int(xivoconf_general["port_fiche_login"])
        if "port_fiche_keepalive" in xivoconf_general:
                port_keepalive = int(xivoconf_general["port_fiche_keepalive"])
        if "port_fiche_agi" in xivoconf_general:
                port_request = int(xivoconf_general["port_fiche_agi"])
        if "port_switchboard" in xivoconf_general:
                port_ui_srv = int(xivoconf_general["port_switchboard"])
        if "port_php" in xivoconf_general:
                port_phpui_srv = int(xivoconf_general["port_php"])
        if "port_switchboard_base_sip" in xivoconf_general:
                port_switchboard_base_sip = int(xivoconf_general["port_switchboard_base_sip"])
        if "xivoclient_session_timeout" in xivoconf_general:
                xivoclient_session_timeout = int(xivoconf_general["xivoclient_session_timeout"])
        if "xivosb_register_frequency" in xivoconf_general:
                xivosb_register_frequency = int(xivoconf_general["xivosb_register_frequency"])
        if "capabilities" in xivoconf_general and xivoconf_general["capabilities"] != "":
                capabilities_list = xivoconf_general["capabilities"].split(",")
                for capa in capabilities_list:
                        if capa in map_capas: capalist_server |= map_capas[capa]
        if "asterisklist" in xivoconf_general:
                asterisklist = xivoconf_general["asterisklist"].split(",")
        if "maxgui_sb" in xivoconf_general:
                maxgui_sb = int(xivoconf_general["maxgui_sb"])
        if "maxgui_xc" in xivoconf_general:
                maxgui_xc = int(xivoconf_general["maxgui_xc"])
        if "evtfile" in xivoconf_general:
                evt_filename = xivoconf_general["evtfile"]
        if "guifile" in xivoconf_general:
                gui_filename = xivoconf_general["guifile"]
        if "nukeast" in xivoconf_general:
                nukeast = True

        if "noami" in xivoconf_general: with_ami = False
        if "advert" in xivoconf_general: with_advert = True

        configs = []
        save_for_next_packet_events = []
        save_for_next_packet_status = []
        n = 0
        ip_reverse_php = {}
        ip_reverse_sht = {}

        # loads the configuration for each asterisk
        for i in xivoconf.sections():
                if i != "general" and i in asterisklist:
                        xivoconf_local = dict(xivoconf.items(i))

                        localaddr = "127.0.0.1"
                        userlisturl = "sso.php"
                        ipaddress = "127.0.0.1"
                        ipaddress_php = "127.0.0.1"
                        extrachannels = ""
                        ami_port = 5038
                        ami_login = "xivouser"
                        ami_pass = "xivouser"
                        sip_port = 5060
                        sip_presence = ""
                        contexts = ""
                        cdr_db_uri = ""
                        realm = "asterisk"

                        if "localaddr" in xivoconf_local:
                                localaddr = xivoconf_local["localaddr"]
                        if "userlisturl" in xivoconf_local:
                                userlisturl = xivoconf_local["userlisturl"]
                        if "ipaddress" in xivoconf_local:
                                ipaddress = xivoconf_local["ipaddress"]
                        if "ipaddress_php" in xivoconf_local:
                                ipaddress_php = xivoconf_local["ipaddress_php"]
                        if "extrachannels" in xivoconf_local:
                                extrachannels = xivoconf_local["extrachannels"]
                        if "ami_port" in xivoconf_local:
                                ami_port = int(xivoconf_local["ami_port"])
                        if "ami_login" in xivoconf_local:
                                ami_login = xivoconf_local["ami_login"]
                        if "ami_pass" in xivoconf_local:
                                ami_pass = xivoconf_local["ami_pass"]
                        if "sip_port" in xivoconf_local:
                                sip_port = int(xivoconf_local["sip_port"])
                        if "sip_presence" in xivoconf_local:
                                sip_presence = xivoconf_local["sip_presence"]
                        if "contexts" in xivoconf_local:
                                contexts = xivoconf_local["contexts"]
                                if contexts != "":
                                        for c in contexts.split(','):
                                                contextlist.append(c)
                        if "cdr_db_uri" in xivoconf_local:
                                cdr_db_uri = xivoconf_local["cdr_db_uri"]
                        if "realm" in xivoconf_local:
                                realm = xivoconf_local["realm"]
                        for capauser, capadefs in xivoconf_local.iteritems():
                                if capauser.find('capas_') == 0:
                                        cuser = capauser[6:].split('/')
                                        cdefs = capadefs.split(',')

                        configs.append(AsteriskRemote(i,
                                                      userlisturl,
                                                      extrachannels,
                                                      localaddr,
                                                      ipaddress,
                                                      ipaddress_php,
                                                      ami_port,
                                                      ami_login,
                                                      ami_pass,
                                                      port_switchboard_base_sip + n,
                                                      sip_port,
                                                      sip_presence,
                                                      contexts,
                                                      cdr_db_uri,
                                                      realm))

                        if ipaddress not in ip_reverse_sht:
                                ip_reverse_sht[ipaddress] = n
                        else:
                                log_debug('WARNING - IP address already exists for asterisk #%d - can not set it for #%d'
                                          % (ip_reverse_sht[ipaddress], n))
                        if ipaddress_php not in ip_reverse_php:
                                ip_reverse_php[ipaddress_php] = n
                        else:
                                log_debug('WARNING - IP address (PHP) already exists for asterisk #%d - can not set it for #%d'
                                          % (ip_reverse_php[ipaddress_php], n))
                        save_for_next_packet_events.append("")
                        save_for_next_packet_status.append("")
                        n += 1


        contexts_cl = {}
        contexts_cl[''] = Context()
        # loads the configuration for each context
        for i in xivoconf.sections():
                if i != "general" and i in contextlist:
                        xivoconf_local = dict(xivoconf.items(i))
                        dir_db_uri = ""
                        dir_db_sqltable = ""

                        if "dir_db_uri" in xivoconf_local:
                                dir_db_uri = xivoconf_local["dir_db_uri"]
                        if "dir_db_sqltable" in xivoconf_local:
                                dir_db_sqltable = xivoconf_local["dir_db_sqltable"]

                        z = Context()
                        z.setUri(dir_db_uri)
                        z.setSqlTable(dir_db_sqltable)

                        fnames = {}
                        snames = {}
                        for field in xivoconf_local:
                                if field.find('dir_db_search') == 0:
                                        ffs = field.split('.')
                                        if len(ffs) == 3:
                                                if ffs[1] not in fnames:
                                                        fnames[ffs[1]] = {}
                                                fnames[ffs[1]][ffs[2]] = xivoconf_local[field]
                                elif field.find('dir_db_sheet') == 0:
                                        ffs = field.split('.')
                                        if len(ffs) == 3:
                                                if ffs[1] not in snames:
                                                        snames[ffs[1]] = {}
                                                snames[ffs[1]][ffs[2]] = xivoconf_local[field]
                                        elif len(ffs) == 2 and ffs[1] == 'callidmatch':
                                                z.setSheetCallidMatch(xivoconf_local[field].split(','))

                        search_vfields = []
                        search_mfields = []
                        for fname in fnames.itervalues():
                                if 'display' in fname and 'match' in fname:
                                        dbnames = fname['match']
                                        if dbnames != "":
                                                dbnames_list = dbnames.split(",")
                                                for dbn in dbnames_list:
                                                        if dbn not in search_mfields:
                                                                search_mfields.append(dbn)
                                                keepspaces = getboolean(fname, 'space')
                                                search_vfields.append([fname['display'], dbnames_list, keepspaces])
                        z.setSearchValidFields(search_vfields)
                        z.setSearchMatchingFields(search_mfields)
                        
                        sheet_vfields = []
                        sheet_mfields = []
                        for fname in snames.itervalues():
                                if 'field' in fname and 'match' in fname:
                                        dbnames = fname['match']
                                        if dbnames != "":
                                                dbnames_list = dbnames.split(",")
                                                for dbn in dbnames_list:
                                                        if dbn not in sheet_mfields:
                                                                sheet_mfields.append(dbn)
                                                sheet_vfields.append([fname['field'], dbnames_list, False])

                        z.setSheetValidFields(sheet_vfields)
                        z.setSheetMatchingFields(sheet_mfields)
                        
                        contexts_cl[i] = z

        # Instantiate the SocketServer Objects.
        loginserver = MyTCPServer(('', port_login), LoginHandler)
        # TODO: maybe we should listen on only one interface (localhost ?)
        requestserver = MyTCPServer(('', port_request), IdentRequestHandler)
        # Do we need a Threading server for the keep alive ? I dont think so,
        # packets processing is non blocking so thread creation/start/stop/delete
        # overhead is not worth it.
        # keepaliveserver = SocketServer.ThreadingUDPServer(('', port_keepalive), KeepAliveHandler)
        keepaliveserver = SocketServer.UDPServer(('', port_keepalive), KeepAliveHandler)

        # We have three sockets to listen to so we cannot use the
        # very easy to use SocketServer.serve_forever()
        # So select() is what we need. The SocketServer.handle_request() calls
        # won't block the execution. In case of the TCP servers, they will
        # spawn a new thread, in case of the UDP server, the request handling
        # process should be fast. If it isnt, use a threading UDP server ;)
        ins = [loginserver.socket, requestserver.socket, keepaliveserver.socket]

        if debug_mode:
                # opens the evtfile for output in append mode
                try:
                        evtfile = open(evt_filename, 'a')
                except Exception, exc:
                        print "Could not open %s in append mode : %s" %(evt_filename,exc)
                        evtfile = False
                # opens the guifile for output in append mode
                try:
                        guifile = open(gui_filename, 'a')
                except Exception, exc:
                        print "Could not open %s in append mode : %s" %(gui_filename,exc)
                        guifile = False

        # user list initialized empty
        userlist = {}
        userlist_lock = threading.Condition()

        plist = []
        SIPsocks = []
        AMI_array_events_off = {}
        AMI_array_events_on = {}
        AMI_array_user_commands = {}
        asteriskr = {}

        items_asterisks = xrange(len(configs))
        advertise = "xivo_daemon:" + str(len(items_asterisks))

        log_debug("the monitored asterisk's is/are : " + str(asterisklist))
        log_debug("# STARTING XIVO Daemon # (1/3) AMI socket connections")

        for n in items_asterisks:
                plist.append(PhoneList(configs[n].astid))
                userlist[configs[n].astid] = {}
                asteriskr[configs[n].astid] = n

                SIPsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                SIPsock.bind(("", configs[n].portsipclt))
                SIPsocks.append(SIPsock)
                ins.append(SIPsock)
                advertise = advertise + ":" + configs[n].astid

        xdal = None
        # xivo daemon advertising
        if with_advert:
                xda = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                xda.bind(("", 5010))
                xda.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
                xda.sendto(advertise, ("255.255.255.255", 5011))

                xdal = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                xdal.bind(("", 5011))
                ins.append(xdal)

        log_debug("# STARTING XIVO Daemon # (2/3) listening UI sockets")

        # opens the listening socket for UI connections
        UIsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        UIsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        UIsock.bind(("", port_ui_srv))
        UIsock.listen(10)
        ins.append(UIsock)

        # opens the listening socket for PHP/CLI connections
        PHPUIsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        PHPUIsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        PHPUIsock.bind(("", port_phpui_srv))
        PHPUIsock.listen(10)
        ins.append(PHPUIsock)

        tcpopens_sb = []
        tcpopens_php = []
        lastrequest_time = []

        log_debug("# STARTING XIVO Daemon # (3/3) fetch SSO, SIP register and subscribe")
        for n in items_asterisks:
                try:
                        update_sipnumlist(n)
                        if with_ami: update_amisocks(n, configs[n].astid)
                        do_sip_register(configs[n], SIPsocks[n])
                        lastrequest_time.append(time.time())
                except Exception, exc:
                        log_debug(configs[n].astid + " : failed while updating lists and sockets : %s" %(str(exc)))


        # Receive messages
        while not askedtoquit:
                try:
                        [i, o, e] = select.select(ins, [], [], xivosb_register_frequency)
                except Exception, exc:
                        if askedtoquit:
                                try:
                                        send_msg_to_cti_clients("ERROR server_stopped")
                                        os.unlink(PIDFILE)
                                except Exception, exc:
                                        print exc
                                if debug_mode:
                                        # Close files and sockets
                                        evtfile.close()
                                        guifile.close()
                                for t in filter(lambda x: x.getName()<>"MainThread", threading.enumerate()):
                                        print "--- (stop) killing thread <%s>" %(t.getName())
                                        t._Thread__stop()
                                sys.exit(5)
                        else:
                                send_msg_to_cti_clients("ERROR server_reloaded")
                                askedtoquit = True
                                for s in ins:
                                        s.close()
                                for t in filter(lambda x: x.getName()<>"MainThread", threading.enumerate()):
                                        print "--- (reload) the thread <%s> remains" %(t.getName())
                                        # t._Thread__stop() # does not work in reload case (vs. stop case)
                                continue
                if i:
                        if loginserver.socket in i:
                                loginserver.handle_request()
                        elif requestserver.socket in i:
                                requestserver.handle_request()
                        elif keepaliveserver.socket in i:
                                keepaliveserver.handle_request()
                        # SIP incoming packets are catched here
                        elif filter(lambda j: j in SIPsocks, i):
                                res = filter(lambda j: j in SIPsocks, i)[0]
                                for n in items_asterisks:
                                        if SIPsocks[n] is res: break
                                [data, addrsip] = SIPsocks[n].recvfrom(BUFSIZE_UDP)
                                is_an_options_packet = parseSIP(n, data, SIPsocks[n], addrsip)
                                # if the packet is an OPTIONS one (sent for instance when * is restarted)
                                if is_an_options_packet:
                                        log_debug("%s : do_sip_register (parse SIP) %s" %(configs[n].astid,
                                                                                          time.strftime("%H:%M:%S", time.localtime())))
                                        try:
                                                update_sipnumlist(n)
                                                if with_ami: update_amisocks(n, configs[n].astid)
                                                do_sip_register(configs[n], SIPsocks[n])
                                                lastrequest_time[n] = time.time()
                                        except Exception, exc:
                                                log_debug("%s : failed while updating lists and sockets : %s" %(configs[n].astid, str(exc)))
                        # these AMI connections are used in order to manage AMI commands with incoming events
                        elif filter(lambda j: j in AMI_array_events_off.itervalues(), i):
                                res = filter(lambda j: j in AMI_array_events_off.itervalues(), i)[0]
                                for astid, val in AMI_array_events_off.iteritems():
                                        if val is res: break
                                try:
                                        a = AMI_array_events_off[astid].readline() # (BUFSIZE_ANY)
                                        if len(a) == 0: # end of connection from server side : closing socket
                                                log_debug("%s : AMI (events = on)  : CLOSING" % astid)
                                                AMI_array_events_off[astid].close()
                                                ins.remove(AMI_array_events_off[astid])
                                                del AMI_array_events_off[astid]
                                        else:
                                                handle_ami_event(astid, a)
                                except Exception, exc:
                                        log_debug("--- exception --- AMI <%s> (events = on) : %s" % (astid, str(exc)))
                        # these AMI connections are used in order to manage AMI commands without events
                        elif filter(lambda j: j in AMI_array_events_on.itervalues(), i):
                                res = filter(lambda j: j in AMI_array_events_on.itervalues(), i)[0]
                                for astid, val in AMI_array_events_on.iteritems():
                                        if val is res: break
                                try:
                                        a = AMI_array_events_on[astid].readline() # (BUFSIZE_ANY)
                                        if len(a) == 0: # end of connection from server side : closing socket
                                                log_debug("%s : AMI (events = off) : CLOSING" % astid)
                                                AMI_array_events_on[astid].close()
                                                ins.remove(AMI_array_events_on[astid])
                                                del AMI_array_events_on[astid]
                                        else:
                                                handle_ami_status(astid, a)
                                except Exception, exc:
                                        log_debug("--- exception --- AMI <%s> (events = off) : %s" % (astid, str(exc)))
                        # the new UI (SB) connections are catched here
                        elif UIsock in i:
                                [conn, UIsockparams] = UIsock.accept()
                                log_debug("TCP (SB)  socket opened on   %s:%s" %(UIsockparams[0],str(UIsockparams[1])))
                                # appending the opened socket to the ones watched
                                ins.append(conn)
                                conn.setblocking(0)
                                tcpopens_sb.append([conn, UIsockparams[0], UIsockparams[1]])
                        # the new UI (PHP) connections are catched here
                        elif PHPUIsock in i:
                                [conn, PHPUIsockparams] = PHPUIsock.accept()
                                log_debug("TCP (PHP) socket opened on   %s:%s" %(PHPUIsockparams[0],str(PHPUIsockparams[1])))
                                # appending the opened socket to the ones watched
                                ins.append(conn)
                                conn.setblocking(0)
                                tcpopens_php.append([conn, PHPUIsockparams[0], PHPUIsockparams[1]])
                        # open UI (SB) connections
                        elif filter(lambda j: j[0] in i, tcpopens_sb):
                                conn = filter(lambda j: j[0] in i, tcpopens_sb)[0]
                                try:
                                        manage_tcp_connection(conn, True)
                                except Exception, exc:
                                        log_debug("--- exception --- XC/SB tcp connection : " + str(exc))
                        # open UI (PHP) connections
                        elif filter(lambda j: j[0] in i, tcpopens_php):
                                conn = filter(lambda j: j[0] in i, tcpopens_php)[0]
                                try:
                                        manage_tcp_connection(conn, False)
                                except Exception, exc:
                                        log_debug("-- exception --- PHP tcp connection : " + str(exc))
                        # advertising from other xivo_daemon's around
                        elif xdal in i:
                                [data, addrsip] = xdal.recvfrom(BUFSIZE_UDP)
                                log_debug("a xivo_daemon is around : " + str(addrsip))
                        else:
                                log_debug("unknown socket <%s>" % str(i))

                        for n in items_asterisks:
                                if (time.time() - lastrequest_time[n]) > xivosb_register_frequency:
                                        lastrequest_time[n] = time.time()
                                        log_debug(configs[n].astid + " : do_sip_register (computed timeout) " + time.strftime("%H:%M:%S", time.localtime()))
                                        try:
                                                update_sipnumlist(n)
                                                if with_ami: update_amisocks(n, configs[n].astid)
                                                do_sip_register(configs[n], SIPsocks[n])
                                        except Exception, exc:
                                                log_debug(configs[n].astid + " : failed while updating lists and sockets : %s" %(str(exc)))
                else: # when nothing happens on the sockets, we fall here sooner or later
                        log_debug("do_sip_register (select's timeout) " + time.strftime("%H:%M:%S", time.localtime()))
                        for n in items_asterisks:
                                lastrequest_time[n] = time.time()
                                update_sipnumlist(n)
                                if with_ami: update_amisocks(n, configs[n].astid)
                                do_sip_register(configs[n], SIPsocks[n])

