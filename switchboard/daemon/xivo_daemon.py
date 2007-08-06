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
import ldap
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

# capabilities
CAPA_CUSTINFO    = 1 << 0
CAPA_PRESENCE    = 1 << 1
CAPA_HISTORY     = 1 << 2
CAPA_DIRECTORY   = 1 << 3
CAPA_DIAL        = 1 << 4
CAPA_FEATURES    = 1 << 5
CAPA_PEERS       = 1 << 6
CAPA_MESSAGE     = 1 << 7
CAPA_SWITCHBOARD = 1 << 8

map_capas = {
        'customerinfo'     : CAPA_CUSTINFO,
        'presence'         : CAPA_PRESENCE,
        'history'          : CAPA_HISTORY,
        'directory'        : CAPA_DIRECTORY,
        'dial'             : CAPA_DIAL,
        'features'         : CAPA_FEATURES,
        'peers'            : CAPA_PEERS,
        'instantmessaging' : CAPA_MESSAGE,
        'switchboard'      : CAPA_SWITCHBOARD
        }


## \class myLDAP
class myLDAP:
        def __init__(self, iuri):
                try:
                        addport = iuri.split("@")[1].split("/")[0]
                        userpass = iuri.split("@")[0].split("://")[1]
                        self.dbname = iuri.split("@")[1].split("/")[1]
                        
                        self.user = userpass.split(":")[0]
                        self.passwd = userpass.split(":")[1]
                        self.uri  = "ldap://" + addport
                        self.l = ldap.initialize(self.uri)
                        self.l.protocol_version = ldap.VERSION3
                        self.l.simple_bind_s(self.user, self.passwd)
                        
                except ldap.LDAPError, exc:
                        print exc
                        sys.exit()

        def getldap(self, filter, attrib):
                try:
                        resultat = self.l.search_s(self.dbname,
                                                   ldap.SCOPE_SUBTREE,
                                                   filter,
                                                   attrib)
                        return resultat
                except ldap.LDAPError, exc:
                        print exc
        def close(self):
                try:
                        pass
                except ldap.LDAPError, exc:
                        print exc


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
                        sql = "SELECT calldate, clid, src, dst, dcontext, channel, dstchannel, " \
                              "lastapp, lastdata, duration, billsec, disposition, amaflags, " \
                              "accountcode, uniqueid, userfield FROM %s " % (table)
                        if kind == "0": # outgoing calls (answered)
                                sql += "WHERE disposition='ANSWERED' "
                                sql += "AND channel LIKE '%s/%s-%%' " \
                                       "ORDER BY calldate DESC LIMIT %s" % (techno, phoneid, nlines)
                        elif kind == "1": # incoming calls (answered)
                                sql += "WHERE disposition='ANSWERED' "
                                sql += "AND dstchannel LIKE '%s/%s-%%' " \
                                       "ORDER BY calldate DESC LIMIT %s" % (techno, phoneid, nlines)
                        else: # missed calls (received but not answered)
                                sql += "WHERE disposition!='ANSWERED' "
                                sql += "AND dstchannel LIKE '%s/%s-%%' " \
                                       "ORDER BY calldate DESC LIMIT %s" % (techno, phoneid, nlines)
                        cursor.execute(sql)
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

        for x in lines:
                if x.find("Subscription-State:") == 0:
                        if x.find("Subscription-State: active") < 0:
                                log_debug(x)
                if x.find("<note>") == 0:
                        if x.find("Ready") >= 0:          stat = "Ready"
                        elif x.find("On the phone") >= 0: stat = "On the phone"
                        elif x.find("Ringing") >= 0:      stat = "Ringing"
                        elif x.find("Not online") >= 0:   stat = "Not online"
                        elif x.find("Unavailable") >= 0:  stat = "Unavailable"
                        else:                             stat = "XivoUnknown"
                if x.find("<tuple id") == 0: num = x.split("\"")[1]
        return [num, stat]


## \class AMIClass
# AMI definition in order to interact with the Asterisk AMI.
class AMIClass:
        class AMIError(Exception):
                def __init__(self, msg):
                    self.msg = msg
                def __str__(self):
                    return msg

        # \brief Class initialization.
        def __init__(self, address, loginname, password):
                self.address   = address
                self.loginname = loginname
                self.password  = password
                self.i = 1
        # \brief Connection to a socket.
        def connect(self):
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(self.address)
                self.f = s.makefile('r', 0)
                s.close()
                str = self.f.readline()
                #print str,
        # \brief Sending any AMI command.
        def sendcommand(self, action, args):
                ret = False
                try:
                        self.f.write('Action: ' + action + '\r\n')
                        for (name, value) in args:
                                self.f.write(name + ': ' + value + '\r\n')
                        self.f.write('\r\n')
                        self.f.flush()
                        ret = True
                except:
                        ret = False
                if ret == False:
                        try:
                                self.connect()
                                self.login()
                                if self:
                                        log_debug("retrying AMI command " + action)
                                        self.sendcommand(action, args)
                        except Exception, exc:
                                log_debug("--- exception --- AMI not connected : " + str(exc))
        # \brief For debug.
        def printresponse_forever(self):
                while True:
                        str = self.f.readline()
                        self.i = self.i + 1
        # \brief Reads a part of a reply.
        def readresponsechunk(self):
                start = True
                list = []
                while True:
                        str = self.f.readline()
                        #print "--------------", self.i, len(str), str,
                        self.i = self.i + 1
                        if start and str == '\r\n': continue
                        start = False
                        if str == '\r\n' or str == '': break
                        l = [ x.strip() for x in str.split(': ') ]
                        if len(l) == 2:
                                list.append((l[0], l[1]))
                return dict(list)
        # \brief Reads the reply.
        def readresponse(self, check):
                first = self.readresponsechunk()
                if first=={}: return []
                if first['Response'] != 'Success':
                        #and first['Response'] != 'Follows':
                        if first.has_key('Message'):
                                raise self.AMIError(first['Message'])
                        else:
                                raise self.AMIError('')
                if check == '':
                        return []
                resp = []
                while True:
                        chunk = self.readresponsechunk()
                        #print "chunk", chunk
                        if chunk=={}:
                                #print 'empty chunk'
                                resp.append(first)
                                break
                        resp.append(chunk)
                        if not chunk.has_key('Event'):
                                continue
                                #break
                        if chunk['Event'] == check:
                                break
                return resp
        # \brief Logins to the AMI.
        def login(self):
                try:
                        self.sendcommand('login',
                                         [('Username', self.loginname),
                                          ('Secret', self.password),
                                          ('Events', 'off')])
                        self.readresponse('')
                        return True
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Executes a CLI command.
        def execclicommand(self, command):
                # special procession for cli commands.
                self.sendcommand('Command',
                                 [('Command', command)])
                resp = []
                for i in (1, 2): str = self.f.readline()
                while True:
                        str = self.f.readline()
                        #print self.i, len(str), str,
                        self.i = self.i + 1
                        if str == '\r\n' or str == '' or str == '--END COMMAND--\r\n':
                                break
                        resp.append(str)
                return resp

        # \brief Hangs up a Channel.
        def hangup(self, channel, channel_peer):
                ret = 0
                try:
                        self.sendcommand('Hangup',
                                         [('Channel', channel)])
                        self.readresponse('')
                        ret += 1
                except self.AMIError, exc:
                        pass
                except Exception, exc:
                        pass

                if channel_peer != "":
                        try:
                                self.sendcommand('Hangup',
                                                 [('Channel', channel_peer)])
                                self.readresponse('')
                                ret += 2
                        except self.AMIError, exc:
                                pass
                        except Exception, exc:
                                pass
                
                return ret

        # \brief Originates a call from a phone towards another.
        def originate(self, phoneproto, phonesrc, phonedst, locext):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                try:
                        self.sendcommand('Originate', [('Channel', phoneproto + '/' + phonesrc),
                                                       ('Exten', phonedst),
                                                       ('Context', locext),
                                                       ('Priority', '1'),
                                                       ('CallerID', "calls %s <%s>" %(phonedst, phonedst)),
                                                       ('Variable', 'ORIGINATE_SRC=%s' %phonesrc),
                                                       ('Async', 'true')])
                        self.readresponse('')
                        return True
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Transfers a channel towards a new extension.
        def transfer(self, channel, extension, context):
                try:
                        self.sendcommand('Redirect', [('Channel', channel),
                                                      ('Exten', extension),
                                                      ('Context', context),
                                                      ('Priority', '1')])
                        self.readresponse('')
                        return True
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False


## \brief Builds the full list of customers in order to send them to the requesting client.
# This should be done after a command called "customers".
# \return a string containing the full customers list
# \sa manage_tcp_connection
def build_customers(astn, ctx, searchpatterns):
        dir_db_uri = ""
        dir_db_displayfields = ""
        dir_db_matchingfields = ""
        dir_db_tablename = ""
        
        searchpattern = ' '.join(searchpatterns)
        
        if ctx in configs[astn].contexts:
                xivocf = configs[astn].contexts[ctx]
                if "dir_db_uri" in xivocf:
                        dir_db_uri = xivocf["dir_db_uri"]
                if "dir_db_displayfields" in xivocf:
                        dir_db_displayfields = xivocf["dir_db_displayfields"]
                if "dir_db_matchingfields" in xivocf:
                        dir_db_matchingfields = xivocf["dir_db_matchingfields"]
                if "dir_db_tablename" in xivocf:
                        dir_db_tablename = xivocf["dir_db_tablename"]

        if dir_db_displayfields == "":
                ndfields = 0
        else:
                ndfields = len(dir_db_displayfields.split(";"))
        fullstat = "directory-response=%d;%s" %(ndfields,
                                                dir_db_displayfields)
        fullstatlist = []
        dbkind = dir_db_uri.split(":")[0]
        if dbkind == "ldap":
                if dir_db_matchingfields == "":
                        log_debug("dir_db_matchingfields is empty - could not proceed directory-search request")
                else:
                        fnames = dir_db_matchingfields.split(";")
                        selectline = "(|"
                        fieldslist = []
                        for fname in fnames:
                                fieldslist.append(fname)
                                if searchpattern == "*":
                                        selectline += "(%s=*)" %fname
                                elif searchpattern != "":
                                        selectline += "(%s=*%s*)" %(fname, searchpattern)
                        selectline += ")"
                        ldapid = myLDAP(dir_db_uri)
                        result = ldapid.getldap(selectline, fieldslist)
                        ldapid.close()

                        for x in result:
                                [tnum, cn, o, mailn] = ["", "", "", ""]
                                if 'telephoneNumber' in x[1]:
                                        tnum = x[1]['telephoneNumber'][0].replace(" ", "")
                                elif 'mobile' in x[1]:
                                        tnum = x[1]['mobile'][0].replace(" ", "")
                                if 'cn' in x[1]:
                                        cn = x[1]['cn'][0]
                                if 'o' in x[1]:
                                        o = x[1]['o'][0]
                                if 'mail' in x[1]:
                                        mailn = x[1]['mail'][0]
                                fullstatlist.append("%s;%s;%s;%s" %(tnum,cn,o,mailn))
#                               if mailn != "":
#                                       fullstatlist.append("%s;%s;%s;mailto:%s" %(tnum,cn,o,mailn))
#                               else:
#                                       fullstatlist.append("%s;%s;%s;" %(tnum,cn,o))
        elif dbkind != "":
                if dir_db_matchingfields == "":
                        log_debug("dir_db_matchingfields is empty - could not proceed directory-search request")
                elif ndfields != len(dir_db_matchingfields.split(";")):
                        log_debug("dir_db_matchingfields and dir_db_displayfields do not have the same number of fields - could not proceed directory-search request")
                else:
                        fnames = dir_db_matchingfields.split(";")
                        selectline  = ""
                        for fname in fnames:
                                selectline += "%s, " %fname
                        if searchpattern == "*":
                                whereline = ""
                        elif searchpattern != "":
                                whereline = " WHERE "
                                for fname in fnames:
                                        whereline += "%s REGEXP '%s' OR " %(fname, searchpattern)

                        conn = anysql.connect_by_uri(dir_db_uri)
                        cursor = conn.cursor()
                        sqlrequest = "SELECT %s FROM %s %s;" %(selectline[:-2],
                                                               dir_db_tablename,
                                                               whereline[:-4])
                        cursor.execute(sqlrequest)
                        result = cursor.fetchall()
                        conn.close()

                        for x in result:
                                linetodisplay = ""
                                for z in x:
                                        linetodisplay += "%s;" %(str(z))
                                fullstatlist.append("%s" %(linetodisplay[:-1]))
        else:
                log_debug("no database method defined - please fill the dir_db_uri field")

        uniq = {}
        fullstatlist.sort()
        for fsl in [uniq.setdefault(e,e) for e in fullstatlist if e not in uniq]:
                fullstat += ";" + fsl
        fullstat += "\n"
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
def build_features_get(astnum, reqlist, socketid):
        dbfamily = "%s/users/%s" %(reqlist[0], reqlist[1])
        repstr = "featuresget="
        for key in ["VM", "Record", "Screen", "DND"]:
                fullcomm = "database get %s %s" %(dbfamily, key)
                reply = AMIclasssock[astnum].execclicommand(fullcomm)
                for r in reply:
                        if r.find("Value: ") == 0:
                                repstr += "%s;%s;" %(key, r.rstrip().split(" ")[1])

        for key in ["FWD/Unc", "FWD/Busy", "FWD/RNA"]:
                fullcomm = "database get %s %s/Status" %(dbfamily, key)
                reply = AMIclasssock[astnum].execclicommand(fullcomm)
                keystatus = ""
                for r in reply:
                        if r.find("Value: ") == 0:
                                keystatus = r.rstrip().split(" ")[1]
                                
                fullcomm = "database get %s %s/Number" %(dbfamily, key)
                reply = AMIclasssock[astnum].execclicommand(fullcomm)
                keynumber = ""
                for r in reply:
                        if r.find("Value: ") == 0:
                                keynumber = r.rstrip().split(" ")[1]

                repstr += "%s;%s:%s;" %(key, keystatus, keynumber)
        requestersocket_by_featureid[dbfamily] = socketid
        return repstr


## \brief Builds the features reply.
def build_features_put(astnum, reqlist, socketid):
        dbfamily = "%s/users/%s" %(reqlist[0], reqlist[1])
        response = "featuresput="
        if len(reqlist) >= 4:
                key = reqlist[2]
                value = reqlist[3]
                fullcomm = "database put %s %s %s" %(dbfamily, key, value)
                reply = AMIclasssock[astnum].execclicommand(fullcomm)
                repstr = "KO"
                for r in reply:
                        if r.rstrip() == "Updated database successfully":
                                repstr = "OK"
                response = 'PUT %s %s %s' %(repstr, key, value)
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


## \brief Sends a status update to all the connected xivo-switchboard(-like) clients.
# \param astnum the asterisk numerical identifier
# \param phonenum the phone identifier
# \param fromwhom a string that tells who has requested such an update
# \return none
def update_GUI_clients(astnum, phonenum, fromwhom):
        global tcpopens_sb, plist
        phoneinfo = fromwhom + ":" + plist[astnum].astid + ":" + build_basestatus(plist[astnum].normal[phonenum])
        fstatlist = build_fullstatlist(plist[astnum].normal[phonenum])
        strupdate = "update=" + phoneinfo + ":" + fstatlist
        for tcpclient in tcpopens_sb:
                try:
                        tcpclient[0].send(strupdate + "\n")
                except Exception, exc:
                        log_debug("--- exception --- send update has failed on %s : %s"
                                  %(str(tcpclient[0]),str(exc)))
        verboselog(strupdate, False, True)
        for ka_object in requestersocket_by_login.itervalues():
                userlist_lock.acquire()
                try:
                        mysock = ka_object.request[1]
                        mysock.sendto(strupdate,
                                      ka_object.client_address)
                finally:
                        userlist_lock.release()


## \brief Handles the SIP messages according to their meaning (reply to a formerly sent message).
# \param astnum the asterisk numerical identifier
# \param data   the data read from the socket
# \param l_sipsock the socket identifier in order to reply
# \param l_addrsip the SIP address in order to reply
# \return True if it is an OPTIONS packet
# \sa read_sip_properties
def parseSIP(astnum, data, l_sipsock, l_addrsip):
        global tcpopens_sb, plist, configs
        spret = False
        [icseq, imsg, icid, iaccount, ilength, iret, ibranch, itag, iauth] = read_sip_properties(data)
        # if ilength != 11:
        #print "###", astnum, ilength, icseq, icid, iaccount, imsg, iret, ibranch, itag
##    if imsg == "REGISTER" and iret == 200 and icid == "reg_cid@xivopy":
##        for k in tcpopens_sb:
##            k[0].send("message=registered_" + configs[astnum].astid + "\n")

        uri = "sip:%s@%s" %(iaccount, configs[astnum].remoteaddr)
        mycontext = ""
        mysippass = ""
        if iaccount in configs[astnum].xivosb_phoneids:
                mycontext, mysippass = configs[astnum].xivosb_phoneids[iaccount]
        md5_r1 = md5.md5(iaccount + ":asterisk:" + mysippass).hexdigest()

        #print "-----------", iaccount, mysippass
        #print data
        #print "================================="

        realm = "asterisk"
        
        if imsg == "REGISTER":
                if iret == 401:
                        # log_debug("%s : REGISTER %s Passwd?" %(configs[astnum].astid, iaccount))
                        nonce    = iauth.split("nonce=\"")[1].split("\"")[0]
                        md5_r2   = md5.md5(imsg   + ":" + uri).hexdigest()
                        response = md5.md5(md5_r1 + ":" + nonce + ":" + md5_r2).hexdigest()
                        auth = "Authorization: Digest username=\"%s\", realm=\"%s\", nonce=\"%s\", uri=\"%s\", response=\"%s\", algorithm=MD5" \
                               %(iaccount, realm, nonce, uri, response)
                        command = xivo_sip.sip_register(configs[astnum],
                                                        iaccount, 1, "reg_cid@xivopy",
                                                        2 * xivosb_register_frequency, auth)
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
                                                                                 2 * xivosb_register_frequency, "")
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
                                       %(iaccount, realm, nonce, uri, response)
                                command = xivo_sip.sip_subscribe(configs[astnum], iaccount,
                                                                 1, icid, normv.phonenum,
                                                                 2 * xivosb_register_frequency, auth)
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
# \param astnum the asterisk numerical identifier
# \param l_sipsock the SIP socket where to reply
# \return
def do_sip_register(astnum, l_sipsock):
        global configs
        for sipacc in configs[astnum].xivosb_phoneids:
                command = xivo_sip.sip_register(configs[astnum],
                                                sipacc, 1, "reg_cid@xivopy",
                                                2 * xivosb_register_frequency, "")
                l_sipsock.sendto(command, (configs[astnum].remoteaddr, configs[astnum].portsipsrv))
                # command = xivo_sip.sip_options(configs[astnum], configs[astnum].mysipname, cid, sipnum)


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


## \brief Deals with requests from the UI clients.
# \param connid connection identifier
# \param allow_events tells if this connection belongs to events-allowed ones
# (for switchboard) or to events-disallowed ones (for php CLI commands)
# \return none
def manage_tcp_connection(connid, allow_events):
        global AMIclasssock, AMIcomms, ins

        try:
                requester_ip   = connid[1]
                requester_port = connid[2]
                requester      = requester_ip + ":" + str(requester_port)
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
                                        del userinfo_by_requester[requester]
                        else:
                                tcpopens_php.remove(connid)
                                log_debug("TCP (PHP) socket closed from %s" %requester)
                except Exception, exc:
                        log_debug("--- exception --- UI connection [%s] : a problem occured when trying to close %s : %s"
                                  %(msg, str(connid[0]), str(exc)))
        else:
                # what if more lines ???
                usefulmsg = msg.split("\r\n")[0].split("\n")[0]
                if usefulmsg == "infos":
                        try:
                                time_uptime = int(time.time() - time_start)
                                reply = "infos=version=%s;uptime=%d s;logged_sb=%d/%d;logged_xc=%d/%d" \
                                        %(__version__.split()[1], time_uptime,
                                          len(tcpopens_sb), maxgui_sb, conngui_xc, maxgui_xc)
                                for tcpo in tcpopens_sb:
                                        reply += ":%s:%d" %(tcpo[1],tcpo[2])
                                connid[0].send(reply + "\n")
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))


                # debug/setup functions
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
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))
                elif usefulmsg == "show_logged":
                        try:
                                userlist_lock.acquire()
                                try:
                                        for user,info in userlist[0].iteritems():
                                                connid[0].send("%s %s\n" %(user, info))
                                finally:
                                        userlist_lock.release()
                                connid[0].send("%s\n" %str(userinfo_by_requester))
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))
                elif usefulmsg == "show_ami":
                        try:
                                for amis in AMIsocks:
                                        connid[0].send("events off   : %s\n" %(str(amis)))
                                for amis in AMIcomms:
                                        connid[0].send("events on    : %s\n" %(str(amis)))
                                for amis in AMIclasssock:
                                        connid[0].send("sboard comms : %s\n" %(str(amis)))
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))
                elif usefulmsg[0:5] == "label": # for inserting hand-written labels between calls when testing
                        try:
                                log_debug("USER LABEL : %s" %(usefulmsg[6:]))
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))


                elif usefulmsg == "keepalive":
                        try:
                                connid[0].send("keepalive=\n")
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))
                elif usefulmsg == "capabilities":
                        try:
                                connid[0].send("capabilities=%s\n" %("-".join(capabilities_list)))
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : KO when sending to %s : %s"
                                          %(usefulmsg, requester, str(exc)))
                elif usefulmsg == "quit" or usefulmsg == "exit":
                        try:
                                connid[0].close()
                                ins.remove(connid[0])
                                if allow_events == True:
                                        tcpopens_sb.remove(connid)
                                        log_debug("TCP (SB)  socket closed from %s" %requester)
                                else:
                                        tcpopens_php.remove(connid)
                                        log_debug("TCP (PHP) socket closed from %s" %requester)
                        except Exception, exc:
                                log_debug("--- exception --- UI connection [%s] : a problem occured when trying to close %s : %s"
                                          %(usefulmsg, requester, str(exc)))
                elif usefulmsg != "":
                        l = usefulmsg.split()
                        if len(l) == 2 and l[0] == 'hangup':
                                idast_src = -1
                                ast_src = l[1].split("/")[1]
                                if ast_src in asteriskr: idast_src = asteriskr[ast_src]
                                if idast_src == -1:
                                        connid[0].send("message=%s::hangup KO : no such asterisk id (%s)\n" %(DAEMON, ast_src))
                                else:
                                        log_debug("%s is attempting a HANGUP : %s" %(requester, str(l)))
                                        phone, channel = split_from_ui(l[1])
                                        if phone in plist[idast_src].normal:
                                                if channel in plist[idast_src].normal[phone].chann:
                                                        channel_peer = plist[idast_src].normal[phone].chann[channel].getChannelPeer()
                                                        log_debug("UI action : %s : hanging up <%s> and <%s>"
                                                                  %(configs[idast_src].astid , channel, channel_peer))
                                                        if not AMIclasssock[idast_src]:
                                                                log_debug("AMI was not connected - attempting to connect again")
                                                                AMIclasssock[idast_src] = connect_to_AMI((configs[idast_src].remoteaddr,
                                                                                                        configs[idast_src].ami_port),
                                                                                                       configs[idast_src].ami_login,
                                                                                                       configs[idast_src].ami_pass)
                                                        if AMIclasssock[idast_src]:
                                                                ret = AMIclasssock[idast_src].hangup(channel, channel_peer)
                                                                if ret > 0:
                                                                        connid[0].send("message=%s::hangup OK (%d) %s\n"
                                                                                       %(DAEMON, ret, l[1]))
                                                                else:
                                                                        connid[0].send("message=%s::hangup KO : socket request failed\n" %DAEMON)
                                                        else:
                                                                connid[0].send("message=%s::hangup KO : no socket available\n" %DAEMON)
                                                else:
                                                        connid[0].send("message=%s::hangup KO : no such channel\n" %DAEMON)
                                        else:
                                                connid[0].send("message=%s::hangup KO : no such phone\n" %DAEMON)
                        elif l[0] == 'history' or l[0] == 'directory-search' or \
                                 l[0] == 'featuresget' or l[0] == 'featuresput' or \
                                 l[0] == 'hints' or l[0] == 'callerids' or l[0] == 'message':
                                log_debug("%s is attempting a %s : %s" %(l[0], requester, str(l)))
                                try:
                                        if requester in userinfo_by_requester:
                                                repstr = parse_command_and_build_reply(userinfo_by_requester[requester], l)
                                                connid[0].send(repstr + "\n")
                                except Exception, exc:
                                        log_debug("--- exception --- UI connection [%s] : a problem occured when sending to %s : %s"
                                                  %(l[0], requester, str(exc)))
                        elif len(l) == 3 and (l[0] == 'originate' or l[0] == 'transfer'):
                                [dummyp, ast_src, context_src, proto_src, userid_src] = l[1].split("/")
                                [dummyp, ast_dst, context_dst, proto_dst, userid_dst, exten_dst] = l[2].split("/")
                                idast_src = -1
                                idast_dst = -1
                                if ast_src in asteriskr: idast_src = asteriskr[ast_src]
                                if ast_dst in asteriskr: idast_dst = asteriskr[ast_dst]
                                if idast_src != -1 and idast_src == idast_dst:
                                        if not AMIclasssock[idast_src]:
                                                "AMI was not connected - attempting to connect again"
                                                AMIclasssock[idast_src] = connect_to_AMI((configs[idast_src].remoteaddr,
                                                                                        configs[idast_src].ami_port),
                                                                                       configs[idast_src].ami_login,
                                                                                       configs[idast_src].ami_pass)
                                        if AMIclasssock[idast_src]:
                                                if l[0] == 'originate':
                                                        log_debug("%s is attempting an ORIGINATE : %s" %(requester, str(l)))
                                                        if ast_dst != "":
                                                                ret = AMIclasssock[idast_src].originate(proto_src,
                                                                                                        userid_src,
                                                                                                        exten_dst,
                                                                                                        context_dst)
                                                        else:
                                                                ret = False
                                                        if ret:
                                                                connid[0].send("message=%s::originate OK (ast %d) %s %s\n"
                                                                               %(DAEMON, idast_src, l[1], l[2]))
                                                        else:
                                                                connid[0].send("message=%s::originate KO (ast %d) %s %s\n"
                                                                               %(DAEMON, idast_src, l[1], l[2]))
                                                elif l[0] == 'transfer':
                                                        log_debug("%s is attempting a TRANSFER : %s" %(requester, str(l)))
                                                        phonesrc, phonesrcchan = split_from_ui(l[1])
                                                        if phonesrc == phonesrcchan:
                                                                connid[0].send("message=%s::transfer KO : %s not a channel\n"
                                                                               %(DAEMON, phonesrcchan))
                                                        else:
                                                                if phonesrc in plist[idast_src].normal:
                                                                        channellist = plist[idast_src].normal[phonesrc].chann
                                                                        nopens = len(channellist)
                                                                        if nopens == 0:
                                                                                connid[0].send("message=%s::transfer KO - no channel opened on %s\n"
                                                                                               %(DAEMON, phonesrc))
                                                                        else:
                                                                                tchan = channellist[phonesrcchan].getChannelPeer()
                                                                                ret = AMIclasssock[idast_src].transfer(tchan,
                                                                                                                       exten_dst,
                                                                                                                       "local-extensions")
                                                                                if ret:
                                                                                        connid[0].send("message=%s::transfer OK (ast %d) %s %s\n"
                                                                                                       %(DAEMON, idast_src, l[1], l[2]))
                                                                                else:
                                                                                        connid[0].send("message=%s::transfer KO (ast %d) %s %s\n"
                                                                                                       %(DAEMON, idast_src, l[1], l[2]))
                                else:
                                        connid[0].send("message=%s::originate or transfer KO : asterisk id mismatch\n" %DAEMON)
                        elif len(l) >= 4 and l[0] == 'login':
                                if l[1] in asteriskr:
                                        astnum = asteriskr[l[1]]
                                        # log_debug("%i %s" % (astnum,l[3]))
                                        userlist_lock.acquire()
                                        try:
                                                user = finduser(astnum, l[2].lower() + l[3])
                                                if user == None:
                                                        repstr = "loginko=\n"
                                                        log_debug("no user found %s" %str(l))
                                                else:
                                                        repstr = "loginok=" + user.get('context') + ";" + user.get('phonenum') + "\r\n"
                                                        userinfo_by_requester[requester] = [astnum,
                                                                                            user.get('context'),
                                                                                            None,
                                                                                            l[1] + "/" + l[2] + l[3],
                                                                                            user.get('capas')]
                                        finally:
                                                userlist_lock.release()
                                else:
                                        repstr = "loginko=\n"
                                        log_debug("login command attempt from SB : asterisk name <%s> unknown" %l[1])
                                connid[0].send(repstr)
                        elif allow_events == False: # i.e. if PHP-style connection
                                n = -1
                                if requester_ip in ip_reverse_php: n = ip_reverse_php[requester_ip]
                                if n == -1:
                                        connid[0].send("%s:KO <NOT ALLOWED>\n" %(XIVO_CLI_PHP_HEADER))
                                else:
                                        connid[0].send("%s:ID <%s>\n" %(XIVO_CLI_PHP_HEADER, configs[n].astid))
                                        try:
                                                if not AMIclasssock[n]:
                                                        log_debug("AMI was not connected - attempting to connect again")
                                                        AMIclasssock[n] = connect_to_AMI((configs[n].remoteaddr,
                                                                                          configs[n].ami_port),
                                                                                         configs[n].ami_login,
                                                                                         configs[n].ami_pass)
                                                if AMIclasssock[n]:
                                                        try:
                                                                s = AMIclasssock[n].execclicommand(usefulmsg.strip())
                                                        except Exception, exc:
                                                                log_debug("--- exception --- (%s) error : php command <%s> : (client %s) : %s"
                                                                          %(configs[n].astid, str(usefulmsg.strip()), requester, str(exc)))
                                                        try:
                                                                for x in s: connid[0].send(x)
                                                                connid[0].send("%s:OK\n" %(XIVO_CLI_PHP_HEADER))
                                                                connid[0].close()
                                                                ins.remove(connid[0])
                                                                log_debug("TCP (PHP) socket closed towards %s" %requester)
                                                        except Exception, exc:
                                                                log_debug("--- exception --- (%s) error : php command <%s> : (client %s) : %s"
                                                                          %(configs[n].astid, str(usefulmsg.strip()), requester, str(exc)))
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


## \brief Returns a given field from an AMI line.
# \param lineami the line extracted from AMI
# \param field the field whose value one is interested in
# \return the value of the field
def getvalue(lineami, field):
        ret = "<NOFIELD>"
        s1 = lineami.split(";" + field + ": ")
        if len(s1) == 2:
                s2 = s1[1].split(";")[0]
                ret = s2
        return ret


## \brief Handling of AMI events occuring in Events=on mode.
# \param astnum the asterisk numerical identifier
# \param idata the data read from the AMI we want to parse
# \return none
# \sa handle_ami_event_dial, handle_ami_event_link, handle_ami_event_hangup
def handle_ami_event(astnum, idata):
        global plist, save_for_next_packet_events
        listkeys = plist[astnum].normal.keys()

        full_idata = save_for_next_packet_events[astnum] + idata
        evlist = full_idata.split("\r\n\r\n")
        save_for_next_packet_events[astnum] = evlist.pop()

        for z in evlist:
                # we assume no ";" character is present in AMI events fields
                x = z.replace("\r\n", ";")
                verboselog("/%s/ %s" %(plist[astnum].astid, x), True, False)
                if x.find("Dial;") == 7:
                        src     = getvalue(x, "Source")
                        dst     = getvalue(x, "Destination")
                        clid    = getvalue(x, "CallerID")
                        clidn   = getvalue(x, "CallerIDName")
                        context = getvalue(x, "Context")
                        try:
                                handle_ami_event_dial(listkeys, astnum, src, dst, clid, clidn)
                                #print "dial", context, x
                        except Exception, exc:
                                log_debug("--- exception --- handle_ami_event_dial : " + str(exc))
                elif x.find("Link;") == 7:
                        src     = getvalue(x, "Channel1")
                        dst     = getvalue(x, "Channel2")
                        clid1   = getvalue(x, "CallerID1")
                        clid2   = getvalue(x, "CallerID2")
                        context = getvalue(x, "Context")
                        try:
                                handle_ami_event_link(listkeys, astnum, src, dst, clid1, clid2)
                                #print "link", context, x
                        except Exception, exc:
                                log_debug("--- exception --- handle_ami_event_link : " + str(exc))
                elif x.find("Unlink;") == 7:
                        # there might be something to parse here
                        src   = getvalue(x, "Channel1")
                        dst   = getvalue(x, "Channel2")
                        clid1 = getvalue(x, "CallerID1")
                        clid2 = getvalue(x, "CallerID2")
                        try:
                                handle_ami_event_unlink(listkeys, astnum, src, dst, clid1, clid2)
                                #print "unlink", context, x
                        except Exception, exc:
                                log_debug("--- exception --- handle_ami_event_unlink : " + str(exc))
                elif x.find("Hangup;") == 7:
                        chan  = getvalue(x, "Channel")
                        cause = getvalue(x, "Cause-txt")
                        try:
                                #print x
                                handle_ami_event_hangup(listkeys, astnum, chan, cause)
                        except Exception, exc:
                                log_debug("--- exception --- handle_ami_event_hangup: " + str(exc))
                elif x.find("Reload;") == 7:
                        # warning : "reload" as well as "reload manager" can appear here
                        log_debug("AMI:Reload: " + plist[astnum].astid)
                        do_sip_register(astnum, SIPsocks[astnum])
                elif x.find("Shutdown;") == 7:
                        log_debug("AMI:Shutdown: " + plist[astnum].astid)
                elif x.find("Join;") == 7:
                        clid  = getvalue(x, "CallerID")
                        qname = getvalue(x, "Queue")
                        if len(clid) > 0:
                                for k in tcpopens_sb:
                                        k[0].send("message=%s::<%s> is calling the Queue <%s>\n" %(DAEMON, clid, qname))
                elif x.find("PeerStatus;") == 7:
                        # <-> register's ? notify's ?
                        pass
                elif x.find("Agentlogin;") == 7:
                        log_debug("//AMI:Agentlogin// %s : %s" %(plist[astnum].astid, x))
                elif x.find("Agentlogoff;") == 7:
                        log_debug("//AMI:Agentlogoff// %s : %s" %(plist[astnum].astid, x))
                elif x.find("Agentcallbacklogin;") == 7:
                        log_debug("//AMI:Agentcallbacklogin// %s : %s" %(plist[astnum].astid, x))
                elif x.find("Agentcallbacklogoff;") == 7:
                        log_debug("//AMI:Agentcallbacklogoff// %s : %s" %(plist[astnum].astid, x))
                elif x.find("AgentCalled;") == 7:
                        log_debug("//AMI:AgentCalled// %s : %s" %(plist[astnum].astid, x))
                elif x.find("ParkedCallsComplete;") == 7:
                        log_debug("//AMI:ParkedCallsComplete// %s : %s" %(plist[astnum].astid, x))
                elif x.find("ParkedCalled;") == 7:
                        log_debug("//AMI:ParkedCalled// %s : %s" %(plist[astnum].astid, x))
                elif x.find("Cdr;") == 7:
                        log_debug("//AMI:Cdr// %s : %s" %(plist[astnum].astid, x))
                elif x.find("Alarm;") == 7:
                        log_debug("//AMI:Alarm// %s : %s" %(plist[astnum].astid, x))
                elif x.find("AlarmClear;") == 7:
                        log_debug("//AMI:AlarmClear// %s : %s" %(plist[astnum].astid, x))
                elif x.find("FaxReceived;") == 7:
                        log_debug("//AMI:FaxReceived// %s : %s" %(plist[astnum].astid, x))
                elif x.find("MeetmeJoin;") == 7:
                        channel = getvalue(x, "Channel")
                        meetme = getvalue(x, "Meetme")
                        usernum = getvalue(x, "Usernum")
                        log_debug("AMI:MeetmeJoin %s : %s %s %s"
                                  %(plist[astnum].astid, channel, meetme, usernum))
                elif x.find("MeetmeLeave;") == 7:
                        channel = getvalue(x, "Channel")
                        meetme = getvalue(x, "Meetme")
                        usernum = getvalue(x, "Usernum")
                        log_debug("AMI:MeetmeLeave %s : %s %s %s"
                                  %(plist[astnum].astid, channel, meetme, usernum))
                elif x.find("ExtensionStatus;") == 7:
                        exten   = getvalue(x, "Exten")
                        context = getvalue(x, "Context")
                        status  = getvalue(x, "Status")
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
                elif x.find("OriginateSuccess;") == 7: pass
                elif x.find("OriginateFailure;") == 7:
                        log_debug("AMI:OriginateFailure: " + plist[astnum].astid + \
                                  " - reason=" + getvalue(x, "Reason"))
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
                elif x.find("Rename;") == 7:
                        # appears when there is a transfer
                        channel_old = getvalue(x, "Oldname")
                        channel_new = getvalue(x, "Newname")
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
                                        log_debug("--- exception --- %s : renaming (ami-er1) failed : %s" %(configs[astnum].astid,str(exc)))

                                try:
                                        plist[astnum].normal_channel_fills(channel_p1, DUMMY_CLID,
                                                                           DUMMY_STATE, 0, DUMMY_DIR,
                                                                           channel_new, n2, "ami-er2")
                                except Exception, exc:
                                        log_debug("--- exception --- %s : renaming (ami-er2) failed : %s" %(configs[astnum].astid,str(exc)))

                                try:
                                        plist[astnum].normal_channel_hangup(channel_old, "ami-er3")
                                except Exception, exc:
                                        log_debug("--- exception --- %s : renaming (ami-er3 = hangup) failed : %s" %(configs[astnum].astid,str(exc)))

                        else:
                                log_debug("AMI:Rename:A: %s : old=%s new=%s"
                                          %(plist[astnum].astid, channel_old, channel_new))
                elif x.find("Newstate;") == 7:
                        chan    = getvalue(x, "Channel")
                        clid    = getvalue(x, "CallerID")
                        clidn   = getvalue(x, "CallerIDName")
                        state   = getvalue(x, "State")
                        # state = Ringing, Up, Down
                        plist[astnum].normal_channel_fills(chan, clid,
                                                           state, 0, DUMMY_DIR,
                                                           DUMMY_RCHAN, DUMMY_EXTEN, "ami-ns0")
                elif x.find("Newcallerid;") == 7:
                        # for tricky queues' management
                        chan    = getvalue(x, "Channel")
                        clid    = getvalue(x, "CallerID")
                        clidn   = getvalue(x, "CallerIDName")
                        log_debug("AMI:Newcallerid: " + plist[astnum].astid + \
                                  " channel=" + chan + " callerid=" + clid + " calleridname=" + clidn)
                        # plist[astnum].normal_channel_fills(chan, clid,
                        # DUMMY_STATE, 0, DUMMY_DIR,
                        # DUMMY_RCHAN, DUMMY_EXTEN, "ami-ni0")
                elif x.find("Newchannel;") == 7:
                        chan    = getvalue(x, "Channel")
                        clid    = getvalue(x, "CallerID")
                        state   = getvalue(x, "State")
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
                elif x.find("Newexten;") == 7: # in order to handle outgoing calls ?
                        chan    = getvalue(x, "Channel")
                        exten   = getvalue(x, "Extension")
                        context = getvalue(x, "Context")
                        if exten != "s" and exten != "h" and exten != "t" and exten != "enum":
                                #print "--- exten :", chan, exten
                                plist[astnum].normal_channel_fills(chan, DUMMY_MYNUM,
                                                                   "Calling", 0, DIR_TO_STRING,
                                                                   DUMMY_RCHAN, exten, "ami-ne0")
                        else:
                                pass
                        if x.find("Application: Set;AppData: DB") > 0:
                                try:
                                        # appdata = DB(local-extensions/users/103/DND)=0
                                        appdata = getvalue(x, "AppData")
                                        [ctx, grp, userid, feature] = appdata.split("(")[1].split(")=")[0].split("/", 3)
                                        value = appdata.split(")=")[1]
                                        strupdate = "featuresupdate=%s;%s" %(feature, value)
                                        for tcpclient in tcpopens_sb:
                                                try:
                                                        tcpclient[0].send(strupdate + "\n")
                                                except Exception, exc:
                                                        log_debug("--- exception --- send featuresupdate has failed on %s : %s"
                                                                  %(str(tcpclient[0]),str(exc)))
                                        dbfamily = "%s/%s/%s" %(ctx, grp, userid)
                                        if dbfamily in requestersocket_by_featureid:
                                                # XXX CHECK WARNING TODO
                                                # requestersocket_lock.acquire()
                                                ka_object = requestersocket_by_featureid[dbfamily]
                                                mysock = ka_object.request[1]
                                                mysock.sendto(strupdate, ka_object.client_address)
                                                # requestersocket_lock.release()
                                        else:
                                                pass
                                except Exception, exc:
                                        log_debug("--- exception --- (newexten, appdata) %s" %str(exc))
                elif x.find("MessageWaiting;") == 7:
                        mwi_string = getvalue(x,"Mailbox") + " waiting=" + getvalue(x,"Waiting") \
                                     + "; new=" + getvalue(x, "New") + "; old=" + getvalue(x, "Old")
                        log_debug("AMI:MessageWaiting: " + plist[astnum].astid + " : " + mwi_string)
                elif x.find("QueueParams;") == 7:
                        log_debug("//AMI:QueueParams// %s : %s" %(plist[astnum].astid, x))
                elif x.find("QueueMember;") == 7:
                        log_debug("//AMI:QueueMember// %s : %s" %(plist[astnum].astid, x))
                elif x.find("QueueMemberStatus;") == 7:
                        queuenameq = getvalue(x, "Queue")
                        location   = getvalue(x, "Location")
                        status     = getvalue(x, "Status")
                        log_debug("AMI:QueueMemberStatus: " + plist[astnum].astid + " " + queuenameq + " " + location + " " + status)
                elif x.find("Leave;") == 7:
                        queuenameq = getvalue(x, "Queue")
                        log_debug("AMI:Leave: " + plist[astnum].astid + " " + queuenameq)
                else:
                        if len(x) > 0:
                                log_debug("AMI:XXX: " + plist[astnum].astid + " <" + x + ">")


## \brief Handling of AMI events for the initial Status Command.
# These are AMI events received as a reply to a command.
# \param astnum the asterisk numerical identifier
# \param idata the data read from the AMI we want to parse
# \return
def handle_ami_status(astnum, idata):
        global plist, save_for_next_packet_status
        listkeys = plist[astnum].normal.keys()

        full_idata = save_for_next_packet_status[astnum] + idata
        evlist = full_idata.split("\r\n\r\n")
        save_for_next_packet_status[astnum] = evlist.pop()

        for z in evlist:
                # we assume no ";" character is present in AMI events fields
                x = z.replace("\r\n", ";")
                #if len(x) > 0:
                #print "statuses --FULL--", x
                if x.find("Status;") == 7:
                        if x.find(";State: Up;") >= 0:
                                if x.find(";Seconds: ") >= 0:
                                        chan    = getvalue(x, "Channel")
                                        clid    = getvalue(x, "CallerID")
                                        exten   = getvalue(x, "Extension")
                                        seconds = getvalue(x, "Seconds")
                                        if x.find(";Link: ") >= 0:
                                                link = getvalue(x, "Link")
                                                #print "statuses up --------", chan, clid, exten, seconds, link
                                                plist[astnum].normal_channel_fills(link, DUMMY_MYNUM,
                                                                                   "On the phone", int(seconds), DIR_FROM_STRING,
                                                                                   chan, clid,
                                                                                   "ami-st1")
                                                plist[astnum].normal_channel_fills(chan, DUMMY_MYNUM,
                                                                                   "On the phone", int(seconds), DIR_TO_STRING,
                                                                                   link, exten,
                                                                                   "ami-st2")
                                        else:
                                                log_debug("AMI::Status UP: " + chan + " " + clid + " " + exten + " " + seconds)
                                else:
                                        pass
                        elif x.find(";State: Ring;") >= 0:
                                log_debug("AMI::Status TO: " + getvalue(x, "Channel") + \
                                          " " + getvalue(x, "Extension") + " " + getvalue(x, "Seconds"))
                        elif x.find(";State: Ringing;") >= 0:
                                log_debug("AMI::Status FROM: " + getvalue(x, "Channel"))
                        else:
                                log_debug("AMI::Status : " + x)
                elif x.find("Response: Follows;Privilege: Command;") == 0:
                        for y in x.split("Response: Follows;Privilege: Command;")[1].split("\n"):
                                log_debug("AMI:Response: " + plist[astnum].astid + " : " + y)
                else:
                        log_debug("AMI:_status_: " + plist[astnum].astid + " : " + x)


## \brief Connects to the AMI if not yet.
# \param astnum Asterisk id to (re)connect
# \return none
def update_amisocks(astnum):
        if AMIcomms[astnum] == -1:
                log_debug(plist[astnum].astid + " : AMI (events = off) : attempting to connect")
                als0 = xivo_ami.ami_socket_login(configs[astnum].remoteaddr,
                                                 configs[astnum].ami_port,
                                                 configs[astnum].ami_login,
                                                 configs[astnum].ami_pass, False)
                AMIcomms[astnum] = als0
                if AMIcomms[astnum] != -1:
                        ins.append(als0)
                        log_debug(configs[astnum].astid + " : AMI (events = off) : connected")
                        """Clears the channels before requesting a new status"""
                        for x in plist[astnum].normal.itervalues():
                                x.clear_channels()
                        ret = xivo_ami.ami_socket_status(AMIcomms[astnum])
                        if not ret:
                                log_debug(configs[astnum].astid + " : could not send status command")
                else:
                        log_debug(configs[astnum].astid + " : AMI (events = off) : could NOT connect")

        if AMIsocks[astnum] == -1:
                log_debug(plist[astnum].astid + " : AMI (events = on)  : attempting to connect")
                als1 = xivo_ami.ami_socket_login(configs[astnum].remoteaddr,
                                                 configs[astnum].ami_port,
                                                 configs[astnum].ami_login,
                                                 configs[astnum].ami_pass, True)
                AMIsocks[astnum] = als1
                if AMIsocks[astnum] != -1:
                        ins.append(als1)
                        log_debug(configs[astnum].astid + " : AMI (events = on)  : connected")
                        """Clears the channels before requesting a new status"""
                        for x in plist[astnum].normal.itervalues():
                                x.clear_channels()
                        ret = xivo_ami.ami_socket_status(AMIcomms[astnum])
                        #xivo_ami.ami_socket_command(AMIcomms[astnum], "show channeltypes")
                        #xivo_ami.ami_socket_command(AMIcomms[astnum], "show uptime")
                        #xivo_ami.ami_socket_command(AMIcomms[astnum], "show version")
                        #xivo_ami.ami_socket_command(AMIcomms[astnum], "meetme")
                        if not ret:
                                log_debug(configs[astnum].astid + " : could not send status command")
                else:
                        log_debug(configs[astnum].astid + " : AMI (events = on)  : could NOT connect")


## \brief Updates the list of sip numbers according to the SSO then sends old and new peers to the UIs.
# The reconnection to the AMI is also done here when it has been broken.
# If the AMI sockets are dead, a reconnection is also attempted here.
# \param astnum the asterisk numerical identifier
# \return none
# \sa update_userlist_fromurl
def update_sipnumlist(astnum):
        global plist, configs, conngui_xc

        userlist_lock.acquire()
        try:
                for user,infos in userlist[astnum].iteritems():
                        if "sessiontimestamp" in infos:
                                if time.time() - infos["sessiontimestamp"] > xivoclient_session_timeout:
                                        del infos["sessionid"]
                                        del infos["sessiontimestamp"]
                                        del infos["ip"]
                                        del infos["port"]
                                        conngui_xc = conngui_xc - 1
                                        infos["state"] = "unknown"
                                        sipnumber = "SIP/" + user.split("sip")[1]
                                        if sipnumber in plist[astnum].normal:
                                                plist[astnum].normal[sipnumber].set_imstat("unknown")
                                                plist[astnum].normal[sipnumber].update_time()
                                                update_GUI_clients(astnum, sipnumber, "kfc-dsc")
                                        log_debug(plist[astnum].astid + " : timeout reached for " + sipnumber)
        finally:
                userlist_lock.release()

        sipnumlistold = filter(lambda j: plist[astnum].normal[j].towatch, plist[astnum].normal)
        sipnumlistold.sort()
        try:
                sipnuml = configs[astnum].update_userlist_fromurl(astnum)
        except Exception, exc:
                log_debug("--- exception --- %s : update_userlist_fromurl failed : %s" %(configs[astnum].astid,str(exc)))
                sipnuml = {}
        for x in configs[astnum].extrachannels.split(","):
                if x != "": sipnuml[x] = [x, "", "", x.split("/")[1], ""]
        sipnumlistnew = sipnuml.keys()
        sipnumlistnew.sort()
        if sipnumlistnew != sipnumlistold:
                lstdel = ""
                lstadd = ""
                for snl in sipnumlistold:
                        if snl not in sipnumlistnew:
                                lstdel += "del:" + configs[astnum].astid + ":" + build_basestatus(plist[astnum].normal[snl]) + ";"
                                del plist[astnum].normal[snl] # or = "Absent"/0 ?
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
                        for tcpclient in tcpopens_sb:
                                try:
                                        tcpclient[0].send(strupdate + "\n")
                                except Exception, exc:
                                        log_debug("--- exception --- send peerremove has failed on %s : %s"
                                                  %(str(tcpclient[0]),str(exc)))
                        verboselog(strupdate, False, True)
                        for ka_object in requestersocket_by_login.itervalues():
                                userlist_lock.acquire()
                                try:
                                        mysock = ka_object.request[1]
                                        mysock.sendto(strupdate,
                                                      ka_object.client_address)
                                finally:
                                        userlist_lock.release()
                if lstadd != "":
                        strupdate = "peeradd=" + lstadd
                        for tcpclient in tcpopens_sb:
                                try:
                                        tcpclient[0].send(strupdate + "\n")
                                except Exception, exc:
                                        log_debug("--- exception --- send peeradd has failed on %s : %s"
                                                  %(str(tcpclient[0]),str(exc)))
                        verboselog(strupdate, False, True)
                        for ka_object in requestersocket_by_login.itervalues():
                                userlist_lock.acquire()
                                try:
                                        mysock = ka_object.request[1]
                                        mysock.sendto(strupdate,
                                                      ka_object.client_address)
                                finally:
                                        userlist_lock.release()


## \brief Connects to the AMI through AMIClass.
# \param address IP address
# \param loginname loginname
# \param password password
# \return the socket
def connect_to_AMI(address, loginname, password):
        lAMIsock = AMIClass(address, loginname, password)
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
                for tcpclient in tcpopens_sb:
                        try:
                                tcpclient[0].send(strupdate + "\n")
                        except Exception, exc:
                                log_debug("--- exception --- send has failed on %s : %s" %(str(tcpclient[0]),str(exc)))
                verboselog(strupdate, False, True)
                for ka_object in requestersocket_by_login.itervalues():
                        userlist_lock.acquire()
                        try:
                                mysock = ka_object.request[1]
                                mysock.sendto(strupdate,
                                              ka_object.client_address)
                        finally:
                                userlist_lock.release()

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
                        if status  == "": newstatus = thischannel.getStatus()
                        if idir    == "": newdir = thischannel.getDirection()
                        if peerch  == "": newpeerch = thischannel.getChannelPeer()
                        if peernum == "": newpeernum = thischannel.getChannelNum()
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
                     cdr_db_uri = ""):

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
        def update_userlist_fromurl(self, astn):
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
                        for l in phone_list:
                                [sso_tech, sso_phoneid, sso_passwd, sso_cinfo_allowed,
                                 sso_phonenum, sso_l5, sso_l6,
                                 fullname, firstname, lastname, sso_context] = l
                                for sipacc in self.xivosb_phoneids:
                                        if sipacc == sso_phoneid:
                                                # if this phoneid is a "xivosb" one
                                                if sso_context not in self.xivosb_contexts:
                                                        # only ONE xivosb is allowed for a given context
                                                        # and a xivo_daemon
                                                        self.xivosb_phoneids[sso_phoneid] = [sso_context, sso_passwd]
                                                        self.xivosb_contexts[sso_context] = sso_phoneid
                                                # elif self.xivosb_phoneids[sso_phoneid][0] == "":
                                                # #removes this xivosb account from the list if no context has been filled
                                                # del self.xivosb_phoneids[sso_phoneid]
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
                                                
                                                # the <b> tag inside the string disables the drag&drop of the widget (4.2.3 ok, 4.2.1 ko)
                                                # fullname = firstname + " " + lastname + " <b>" + sso_phoneid + "</b>"
                                                fullname = firstname + " " + lastname + " <b>" + sso_phonenum + "</b>"
                                                
                                                if sso_l5 == "1" and sso_phoneid != sipaccount and sso_phonenum != "":
                                                        if sso_tech == "sip":
                                                                argg = "SIP/" + sso_phoneid
                                                                adduser(astn, sso_tech + sso_phoneid, sso_passwd, sso_context, sso_phonenum, sso_cinfo_allowed)
                                                        elif sso_tech == "iax":
                                                                argg = "IAX2/" + sso_phoneid
                                                        elif sso_tech == "misdn":
                                                                argg = "mISDN/" + sso_phoneid
                                                        elif sso_tech == "zap":
                                                                argg = "Zap/" + sso_phoneid
                                                                adduser(astn, sso_tech + sso_phoneid, sso_passwd, sso_context, sso_phonenum, sso_cinfo_allowed)
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
def adduser(astn, user, passwd, context, phonenum, cinfo_allowed):
        global userlist
        if userlist[astn].has_key(user):
                userlist[astn][user]['passwd'] = passwd
                userlist[astn][user]['context'] = context
        else:
                userlist[astn][user] = {'user':user,
                                        'passwd':passwd,
                                        'context':context,
                                        'phonenum':phonenum,
                                        'capas':0}
                if cinfo_allowed == '1':
                        userlist[astn][user]['capas'] = CAPA_CUSTINFO
                else:
                        userlist[astn][user]['capas'] = 0
                # this list shall be defined through more options in SSO
                userlist[astn][user]['capas'] |= (CAPA_HISTORY | CAPA_DIRECTORY | CAPA_PEERS | CAPA_PRESENCE | CAPA_DIAL | CAPA_FEATURES)


## \brief Deletes a user from the userlist.
# \param user the user to delete
# \return none
def deluser(astn, user):
        global userlist
        if userlist[astn].has_key(user):
                userlist[astn].pop(user)

## \brief Returns the user from the list.
# \param user searched for
# \return user found, otherwise None
def finduser(astn, user):
        if astn >= 0 and astn < len(userlist):
                u = userlist[astn].get(user)
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
                global conngui_xc
                [astnum, user, port, state] = [-1, "", "", ""]
                replystr = "ERROR"
                debugstr = "LoginRequestHandler (TCP) : client = %s:%d" %(self.client_address[0], self.client_address[1])
                list1 = self.rfile.readline().strip().split(' ') # list1 should be "[LOGIN <asteriskname>/sip<nnn>]"
                if len(list1) < 2 or len(list1) > 3 or list1[0] != 'LOGIN':
                        replystr = "ERROR : wrong number of arguments"
                        debugstr += " / LOGIN error args"
                        return [replystr, debugstr], [user, port, state, astnum]

                if list1[1].find("/") >= 0:
                        astname_xivoc = list1[1].split("/")[0]
                        user = list1[1].split("/")[1]
                else:
                        replystr = "ERROR : wrong ID format"
                        debugstr += " / LOGIN error ID"
                        return [replystr, debugstr], [user, port, state, astnum]
                
                whoami = ""
                whatsmyos = ""
                if len(list1) == 3:
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

                # asks for PASS
                self.wfile.write('Send PASS for authentication\r\n')
                list1 = self.rfile.readline().strip().split(' ')
                if len(list1) != 2 or list1[0] != 'PASS':
                        replystr = "ERROR : wrong format for PASS reply"
                        debugstr += " / PASS error"
                        return [replystr, debugstr], [user, port, state, astnum]
                passwd = list1[1]

                if astname_xivoc in asteriskr:
                        astnum = asteriskr[astname_xivoc]
                else:
                        replystr = "ERROR : asterisk name <%s> unknown" %astname_xivoc
                        debugstr += " / asterisk name unknown"
                        return [replystr, debugstr], [user, port, state, astnum]
                userlist_lock.acquire()
                try:
                        e = finduser(astnum, user)
                        goodpass = (e != None) and (e.get('passwd') == passwd)
                finally:
                        userlist_lock.release()
                if not goodpass:
                        replystr = "ERROR : WRONG LOGIN PASSWD"
                        debugstr += " / PASS KO (%s given) for %s on asterisk #%d" %(passwd, user,astnum)
                        return [replystr, debugstr], [user, port, state, astnum]
                
                # asks for PORT
                self.wfile.write('Send PORT command\r\n')
                list1 = self.rfile.readline().strip().split(' ')
                if list1[0] == 'TCPMODE':
                        #print 'TCP MODE !'
                        tcpmode = True
                        port = -1
                elif len(list1) != 2 or list1[0] != 'PORT':
                        replystr = "ERROR PORT"
                        debugstr += " / PORT KO"
                        return [replystr, debugstr], [user, port, state, astnum]
                else:
                        port = list1[1]
                        tcpmode = False
                
                # asks for STATE
                self.wfile.write('Send STATE command\r\n')
                list1 = self.rfile.readline().strip().split(' ')
                if len(list1) != 2 or list1[0] != 'STATE':
                        replystr = "ERROR STATE"
                        debugstr += " / STATE KO"
                        return [replystr, debugstr], [user, port, state, astnum]
                state = list1[1]
                
                # TODO : random pas au top, faire generation de session id plus luxe
                sessionid = '%u' % random.randint(0,999999999)
                userlist_lock.acquire()
                try:
                        if e.has_key('sessiontimestamp'):
                                if time.time() - e.get('sessiontimestamp') < xivoclient_session_timeout:
                                        replystr = "ERROR ALREADY CONNECTED"
                                        debugstr += " / USER %s already connected" %user
                                        return [replystr, debugstr], [user, port, state, astnum]
                        if conngui_xc >= maxgui_xc:
                                replystr = "ERROR %d USERS ALREADY CONNECTED (%d ALLOWED)" %(conngui_xc, maxgui_xc)
                                debugstr += " / %d users already connected (%d allowed)" %(conngui_xc, maxgui_xc)
                                return [replystr, debugstr], [user, port, state, astnum]
                        e['sessionid'] = sessionid
                        e['sessiontimestamp'] = time.time()
                        e['ip'] = self.client_address[0]
                        e['port'] = port
                        e['cticlienttype'] = whoami
                        e['cticlientos'] = whatsmyos
                        e['tcpmode'] = tcpmode
                        capalist_user = e['capas']
                        conngui_xc = conngui_xc + 1
                        if tcpmode:
                                print 'TCPMODE', self.request#, self.request.
                                e['socket'] = self.request.makefile('w')
                        context = e['context']
                        if state in allowed_states:
                                e['state'] = state
                        else:
                                e['state'] = "undefinedstate"
                finally:
                        userlist_lock.release()

                capa_user = []
                for capa in capabilities_list:
                        if (map_capas[capa] & capalist_user):
                                capa_user.append(capa)
                replystr = "OK SESSIONID %s %s %s %s" %(sessionid, context, ",".join(capa_user), __version__.split()[1])
                debugstr += " / user %s, port %s, state %s, astnum %d : connected : %s" %(user,port,state,astnum,replystr)
                return [replystr, debugstr], [user, port, state, astnum]

        def handle(self):
                threading.currentThread().setName('login-%s:%d' %(self.client_address[0], self.client_address[1]))
                try:
                        [rstr, dstr], [user, port, state, astnum] = self.logintalk()
                        self.wfile.write(rstr + "\r\n")
                        log_debug(dstr)
                        if rstr.split()[0] == 'OK' and astnum >= 0:
                                if user.find("sip") == 0:
                                        phoneid = "SIP/" + user.split("sip")[1]
                                elif user.find("iax") == 0:
                                        phoneid = "IAX/" + user.split("iax")[1]
                                else:
                                        phoneid = ""
                                if phoneid in plist[astnum].normal:
                                        plist[astnum].normal[phoneid].set_imstat(state)
                                        plist[astnum].normal[phoneid].update_time()
                                        update_GUI_clients(astnum, phoneid, "kfc-lin")
                                else:
                                        log_debug("%s is not in my phone list" %phoneid)
                except Exception, exc:
                        log_debug("--- exception --- %s" %(str(exc)))


## \class IdentRequestHandler
# \brief Gives client identification to the profile pusher.
# The connection is kept alive so several requests can be made on the same open TCP connection.
class IdentRequestHandler(SocketServer.StreamRequestHandler):
        def handle(self):
                threading.currentThread().setName('ident-%s:%d' %(self.client_address[0], self.client_address[1]))
                line = self.rfile.readline().strip()
                list0 = line.split(' ')
                log_debug("IdentRequestHandler (TCP) : client = %s:%d / %s"
                          %(self.client_address[0],self.client_address[1],str(list0)))
                retline = 'ERROR\r\n'
                action = ""
                # PUSH user callerid msg
                m = re.match("PUSH (\S+) (\S+) ?(.*)", line)
                if m != None:
                        user = m.group(1)
                        callerid = m.group(2)
                        msg = m.group(3)
                        #print 'user', user, 'callerid', callerid, 'msg="%s"'%msg
                        action = "PUSH"
                # QUERY user   (ex: QUERY sipnanard)
                elif list0[0] == 'QUERY' and len(list0) == 2:
                        user = list0[1]
                        action = "QUERY"

                if action != "":
                        userlist_lock.acquire()
                        try:
                                try:
                                        try:
                                                astnum = ip_reverse_sht[self.client_address[0]]
                                        except:
                                                astnum = 0
                                        e = finduser(astnum, user)
                                        if e == None:
                                                retline = 'ERROR USER <' + user + '> NOT FOUND\r\n'
                                        elif e.has_key('ip') and e.has_key('port') \
                                                 and e.has_key('state') and e.has_key('sessionid') \
                                                 and e.has_key('sessiontimestamp'):
                                                if time.time() - e.get('sessiontimestamp') > xivoclient_session_timeout:
                                                        retline = 'ERROR USER SESSION EXPIRED for <%s>\r\n' %user
                                                else:
                                                        capalist = (e['capas'] & capalist_server)
                                                        if (capalist & CAPA_CUSTINFO):
                                                                if action == "PUSH":
                                                                        retline = 'USER %s STATE %s\r\n'%(user,e.get('state'))
                                                                        # print 'sendfiche', e, callerid, msg
                                                                        sendfiche.sendficheasync(e, callerid, msg)
                                                                elif action == "QUERY":
                                                                        retline = 'USER %s SESSIONID %s IP %s PORT %s STATE %s\r\n' \
                                                                                  %(user, e.get('sessionid'), e.get('ip'), e.get('port'), e.get('state'))
                                        else:
                                                retline = 'ERROR USER SESSION NOT DEFINED for <%s>\r\n' %user
                                except Exception, exc:
                                        retline = 'ERROR (exception) : %s\r\n' %(str(exc))
                        finally:
                                userlist_lock.release()

                try:
                        self.wfile.write(retline)
                except Exception, exc:
                        # something bad happened.
                        log_debug("IdentRequestHandler/Exception: " + str(exc))
                        return


def parse_command_and_build_reply(me, myargs):
        repstr = ""
        try:
                capalist = (me[4] & capalist_server)
                if myargs[0] == 'history':
                        if (capalist & CAPA_HISTORY):
                                repstr = build_history_string(myargs[1], myargs[2], myargs[3])
                elif myargs[0] == 'directory-search':
                        if (capalist & CAPA_DIRECTORY):
                                repstr = build_customers(me[0], me[1], myargs[1:])
                elif myargs[0] == 'callerids':
                        if (capalist & CAPA_PEERS):
                                repstr = build_callerids()
                elif myargs[0] == 'hints':
                        if (capalist & CAPA_PEERS):
                                repstr = build_statuses()
                elif myargs[0] == 'featuresget':
                        if (capalist & CAPA_FEATURES):
                                repstr = build_features_get(me[0], myargs[1:], me[2])
                elif myargs[0] == 'featuresput':
                        if (capalist & CAPA_FEATURES):
                                repstr = build_features_put(me[0], myargs[1:], me[2])
                elif myargs[0] == 'message':
                        if (capalist & CAPA_MESSAGE):
                                for k in tcpopens_sb:
                                        k[0].send("message=%s::<%s>\n" %(me[3], myargs[1]))
                elif myargs[0] == 'originate':
                        if (capalist & CAPA_DIAL):
                                [dummyp, ast_src, context_src, proto_src, userid_src, dummy_phoneid_src] = myargs[1].split("/")
                                idast_src = -1
                                if ast_src in asteriskr: idast_src = asteriskr[ast_src]
                                ret = AMIclasssock[idast_src].originate(proto_src.lower(),
                                                                        userid_src,
                                                                        myargs[2],
                                                                        context_src)
        except Exception, exc:
                log_debug("--- exception --- (parse_command_and_build_reply) %s %s" %(str(myargs), str(exc)))
        return repstr


## \class KeepAliveHandler
# \brief It receives UDP datagrams and sends back a datagram containing whether
# "OK" or "ERROR <error-text>".
# It could be a good thing to give a numerical code to each error.
class KeepAliveHandler(SocketServer.DatagramRequestHandler):
        def handle(self):
                global conngui_xc
                threading.currentThread().setName('keepalive-%s:%d' %(self.client_address[0], self.client_address[1]))
                requester = "%s:%d" %(self.client_address[0],self.client_address[1])
                log_debug("KeepAliveHandler    (UDP) : client = %s" %requester)
                astnum = -1
                response = "ERROR unknown"
                do_state_update = False

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
                                e = finduser(astnum, user)
                                if e == None:
                                        raise NameError, "unknown user %s" %user
                                if e.has_key('sessionid') and e.has_key('ip') and e.has_key('sessiontimestamp') \
                                       and sessionid == e['sessionid'] and ip == e['ip'] \
                                       and e['sessiontimestamp'] + xivoclient_session_timeout > timestamp:
                                        e['sessiontimestamp'] = timestamp
                                        response = 'OK'
                                        capalist_user = e['capas']
                                else:
                                        raise NameError, "session expired"
                        finally:
                                userlist_lock.release()
                        
                        if list[0] == 'ALIVE' and len(list) == 6 and list[4] == 'STATE':
                                # ALIVE user SESSIONID sessionid STATE state
                                state = list[5]
                                if state in allowed_states:
                                        e['state'] = state
                                else:
                                        e['state'] = "undefinedstate"
                                do_state_update = True
                                response = 'OK'
                        elif list[0] == 'STOP' and len(list) == 4:
                                # STOP user SESSIONID sessionid
                                userlist_lock.acquire()
                                try:
                                        if list[1] in requestersocket_by_login:
                                                del requestersocket_by_login[list[1]]
                                        else:
                                                log_debug("warning : %s unknown" %(list[1]))
                                        del e['sessionid']
                                        del e['sessiontimestamp']
                                        del e['ip']
                                        del e['port']
                                        del e['state']
                                        del e['tcpmode']
                                        del e['cticlienttype']
                                        del e['cticlientos']
                                        conngui_xc = conngui_xc - 1
                                        sipnumber = "SIP/" + user.split("sip")[1]
                                        if sipnumber in plist[astnum].normal:
                                                plist[astnum].normal[sipnumber].set_imstat("unkown")
                                                plist[astnum].normal[sipnumber].update_time()
                                                update_GUI_clients(astnum, sipnumber, "kfc-dcc")
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
                                        
                                        response = parse_command_and_build_reply([astnum, e['context'], self, list[1], capalist_user], list[4:])
                                except Exception, exc:
                                        log_debug("--- exception --- (command) %s" %str(exc))
                        else:
                                raise NameError, "unknown message <%s>" %str(list)
                except Exception, exc:
                        response = 'ERROR %s' %(str(exc))

                # whatever has been received, we must reply something to the client who asked
                log_debug("replying <%s> to %s" %(response, requester))
                self.request[1].sendto(response + '\r\n', self.client_address)

                if do_state_update:
                        sipnumber = "SIP/" + user.split("sip")[1]
                        if sipnumber in plist[astnum].normal:
                                if(plist[astnum].normal[sipnumber].imstat != state):
                                        plist[astnum].normal[sipnumber].set_imstat(state)
                                        plist[astnum].normal[sipnumber].update_time()
                                        update_GUI_clients(astnum, sipnumber, "kfc-kah")


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
        maxgui_sb = 3
        maxgui_xc = 10
        conngui_xc = 0
        evt_filename = "/var/log/pf-xivo-cti-server/ami_events.log"
        gui_filename = "/var/log/pf-xivo-cti-server/gui.log"
        with_ami = True
        with_sip = True
        with_advert = False
        nukeast = False

        userinfo_by_requester = {}
        requestersocket_by_featureid = {}
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
        if "capabilities" in xivoconf_general:
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
        if "nosip" in xivoconf_general: with_sip = False
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
                        if "cdr_db_uri" in xivoconf_local:
                                cdr_db_uri = xivoconf_local["cdr_db_uri"]

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
                                                      cdr_db_uri))

                        ip_reverse_sht[ipaddress] = n
                        ip_reverse_php[ipaddress_php] = n
                        save_for_next_packet_events.append("")
                        save_for_next_packet_status.append("")
                        n += 1

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
        userlist = []
        userlist_lock = threading.Condition()

        plist = []
        SIPsocks = []
        AMIsocks = []
        AMIcomms = []
        AMIclasssock = []
        asteriskr = {}

        items_asterisks = xrange(len(configs))
        advertise = "xivo_daemon:" + str(len(items_asterisks))

        log_debug("the monitored asterisk's is/are : " + str(asterisklist))
        log_debug("# STARTING XIVO Daemon # (1/3) AMI socket connections")

        for n in items_asterisks:
                plist.append(PhoneList(configs[n].astid))
                userlist.append({})
                asteriskr[configs[n].astid] = n
                AMIcomms.append(-1)
                AMIsocks.append(-1)

                SIPsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                SIPsock.bind(("", configs[n].portsipclt))
                SIPsocks.append(SIPsock)
                ins.append(SIPsock)

                AMIclasssock.append(connect_to_AMI((configs[n].remoteaddr,
                                                    configs[n].ami_port),
                                                   configs[n].ami_login,
                                                   configs[n].ami_pass))
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
                        if with_ami: update_amisocks(n)
                        if with_sip: do_sip_register(n, SIPsocks[n])
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
                                        log_debug(configs[n].astid + " : do_sip_register (parse SIP) " + time.strftime("%H:%M:%S", time.localtime()))
                                        try:
                                                update_sipnumlist(n)
                                                if with_ami: update_amisocks(n)
                                                if with_sip: do_sip_register(n, SIPsocks[n])
                                                lastrequest_time[n] = time.time()
                                        except Exception, exc:
                                                log_debug(configs[n].astid + " : failed while updating lists and sockets : %s" %(str(exc)))
                        # these AMI connections are used in order to manage AMI commands with incoming events
                        elif filter(lambda j: j in AMIsocks, i):
                                res = filter(lambda j: j in AMIsocks, i)[0]
                                for n in items_asterisks:
                                        if AMIsocks[n] is res: break
                                try:
                                        a = AMIsocks[n].recv(BUFSIZE_ANY)
                                        if len(a) == 0: # end of connection from server side : closing socket
                                                log_debug(configs[n].astid + " : AMI (events = on)  : CLOSING")
                                                AMIsocks[n].close()
                                                ins.remove(AMIsocks[n])
                                                AMIsocks[n] = -1
                                        else:
                                                handle_ami_event(n, a)
                                except Exception, exc:
                                        pass
                        # these AMI connections are used in order to manage AMI commands without events
                        elif filter(lambda j: j in AMIcomms, i):
                                res = filter(lambda j: j in AMIcomms, i)[0]
                                for n in items_asterisks:
                                        if AMIcomms[n] is res: break
                                try:
                                        a = AMIcomms[n].recv(BUFSIZE_ANY)
                                        if len(a) == 0: # end of connection from server side : closing socket
                                                log_debug(configs[n].astid + " : AMI (events = off) : CLOSING")
                                                AMIcomms[n].close()
                                                ins.remove(AMIcomms[n])
                                                AMIcomms[n] = -1
                                        else:
                                                handle_ami_status(n, a)
                                except Exception, exc:
                                        pass
                        # the new UI (SB) connections are catched here
                        elif UIsock in i:
                                [conn, UIsockparams] = UIsock.accept()
                                if len(tcpopens_sb) >= maxgui_sb:
                                        conn.close()
                                else:
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
                                        log_debug("--- exception --- SB tcp connection : " + str(exc))
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
                                log_debug("unknown socket " + str(i))
                
                        for n in items_asterisks:
                                if (time.time() - lastrequest_time[n]) > xivosb_register_frequency:
                                        lastrequest_time[n] = time.time()
                                        log_debug(configs[n].astid + " : do_sip_register (computed timeout) " + time.strftime("%H:%M:%S", time.localtime()))
                                        try:
                                                update_sipnumlist(n)
                                                if with_ami: update_amisocks(n)
                                                if with_sip: do_sip_register(n, SIPsocks[n])
                                        except Exception, exc:
                                                log_debug(configs[n].astid + " : failed while updating lists and sockets : %s" %(str(exc)))
                else: # when nothing happens on the sockets, we fall here sooner or later
                        log_debug("do_sip_register (select's timeout) " + time.strftime("%H:%M:%S", time.localtime()))
                        for n in items_asterisks:
                                lastrequest_time[n] = time.time()
                                update_sipnumlist(n)
                                if with_ami: update_amisocks(n)
                                if with_sip: do_sip_register(n, SIPsocks[n])

