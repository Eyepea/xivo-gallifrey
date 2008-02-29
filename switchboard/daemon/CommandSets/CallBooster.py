# -*- coding: latin-1 -*-

"""
This is the CallBooster core class.
The main inputs are :
- the Agents' commands
- the Asterisk events
- the incoming calls
"""

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

import xivo.to_path
import anysql
import ConfigParser
import random
import re
import os
import pickle
import socket
import threading
import Queue
import time
import xivo_commandsets
from Calls import IncomingCall
from Calls import OutgoingCall
from xivo_commandsets import BaseCommand
from xivo_common import *
from easyslog import *


# XIVO-defined constants

STARTAGENTNUM = 6100
PARK_EXTEN = '700'
GHOST_AGENT = 'Agent/6999'


# CallBooster-defined constants

DATEFMT = '%Y-%m-%d'
DATETIMEFMT = DATEFMT + ' %H:%M:%S'

TAXES_TRUNKNAME = 'EXT' # 'FT'
TAXES_PABX      = 'PBX' # 'PABX'
TAXES_CTI       = 'CTI'

TSLOTTIME = 1800
TIMEOUTPING = 120

DECSTRING = 'Déclenché en automatique'
REPFILES = '/usr/share/asterisk/sounds/callbooster'
MOHFILES = '/usr/share/asterisk/moh/callbooster'

STATACD2_STATUSES = {'TT_RAF' : 0, 'TT_ASD' : 0, 'TT_SND' : 0, 'TT_SFA' : 0, 'TT_SOP' : 0,
                     'TT_TEL' : 0, 'TT_FIC' : 0, 'TT_BAS' : 0, 'TT_MES' : 0, 'TT_REP' : 0}

def varlog(syslogprio, string):
        if syslogprio <= SYSLOG_NOTICE:
                syslogf(syslogprio, 'xivo_fb : ' + string)
        return 0

def log_debug(syslogprio, string):
        if syslogprio <= SYSLOG_INFO:
                print '#debug# %s' % string
        return varlog(syslogprio, string)



class CallBoosterCommand(BaseCommand):
        """
        CallBoosterCommand class.
        Defines the behaviour of commands and events for CallBooster.
        """

        commidcurr = 200000
        pending_sv_fiches = {}
        tqueue = Queue.Queue()
        alerts = {}
        nalerts_called = 0
        nalerts_simult = 1
        waitingmessages = {}
        alerts_uniqueids = {}
        commands = ['Init',
                    'AppelOpe', 'TransfertOpe', 'RaccrocheOpe',
                    'Appel', 'Aboute', 'AppelAboute', 'Raccroche',
                    'Enregistre', 'Alerte',
                    'Ping',
                    'Attente', 'Reprise',
                    'Sonn',
                    'Prêt', 'ForceACD',
                    'Pause',
                    'Sortie',
                    'Change',
                    'SynchroXivoAgents',
                    'SynchroXivoSDA',
                    'SynchroXivoMOH']
        incoming_calls = {}
        outgoing_calls = {}
        normal_calls = []
        originated_calls = []

        def __init__(self, ulist, amis, operatsocket, operatport, operatini, queued_threads_pipe):
                """
                Defines the basic arguments.
                """
		BaseCommand.__init__(self)
                self.ulist = ulist
                self.amis  = amis
                self.soperat_socket = operatsocket
                self.soperat_port   = operatport
                opconf = ConfigParser.ConfigParser()
                opconf.readfp(open(operatini))
                opconf_so = dict(opconf.items('SO'))
                if 'opejd' in opconf_so:
                        self.opejd = opconf_so.get('opejd')
                else:
                        self.opejd = '08:00'
                if 'opend' in opconf_so:
                        self.opend = opconf_so.get('opend')
                else:
                        self.opend = '20:00'
                self.queued_threads_pipe = queued_threads_pipe


        def __send_msg__(self, uinfo, msg):
                """
                Sends a message 'msg' to destination 'uinfo', after having proceeded a few checks.
                """
                if uinfo is not None and 'connection' in uinfo:
                        try:
                                uinfo['connection'].send(msg)
                                # print '(CallBooster) send <%s> successfully' % msg
                        except Exception, exc:
                                print '--- exception --- (CallBooster) could not send <%s> to user : %s' % (msg, str(exc))
                else:
                        print '(CallBooster) could not send <%s> to user (no connection field defined)' % msg
                return


        def __sendfiche_a__(self, userinfo, incall):
                """
                Sends the Fiche informations (incall) to the appropriate user (userinfo)
                """
                userinfo['calls'][incall.commid] = incall
                # CLR-ACD to be sent only if there was an Indispo sent previously
                if incall.dialplan['callerid'] == 1:
                        cnum = incall.cidnum
                else:
                        cnum = ''
                incall.waiting = False
                incall.uinfo = userinfo
                self.__send_msg__(incall.uinfo, '%s,%s,%s,Fiche/' % (incall.commid, incall.sdanum, cnum))
                return


        def __sendfiche__(self, astid, dest, incall):
                """
                The call comes here when the AGI has directly found a peer.
                """
                userinfo = self.ulist[astid].finduser(dest)
                log_debug(SYSLOG_INFO, '__sendfiche__ : userinfo = %s' % str(userinfo))
                if userinfo is not None:
                        agentnum = userinfo['agentnum']
                        if 'agentchannel' in userinfo:
                                print 'sendfiche, the agent is online :', userinfo['agentchannel']
                        else:
                                phonenum = userinfo['phonenum']
                                opername = userinfo['user']
                                log_debug(SYSLOG_INFO, 'sendfiche, the agent is not online ... we re going to call him : %s (%s)' % (phonenum, str(userinfo)))
                                self.__agentlogin__(astid, userinfo, phonenum, agentnum, opername)
                        incall.statacd2_tt = 'TT_SOP'
                        self.__sendfiche_a__(userinfo, incall)
                        print '__sendfiche__', incall.queuename, agentnum
                        userinfo['queuelist'][incall.queuename] = incall
                        userinfo['sendfiche'] = [incall.queuename, 'Agent/%s' % agentnum]
                return


        def __clear_call_fromqueues__(self, astid, incall):
                """
                Removes the 'incall' call references from all the operator queues, and
                sends the relevant information to the agents.
                """
                for opername, userinfo in self.ulist[astid].list.iteritems():
                        if incall.queuename in userinfo['queuelist']:
                                del userinfo['queuelist'][incall.queuename]
                                agentnum = userinfo['agentnum']
                                print '__clear_call_fromqueues__ : removing %s (queue %s) for agent %s' %(incall.sdanum, incall.queuename, agentnum)
                                if opername in incall.agentlist:
                                        incall.agentlist.remove(opername)
                                else:
                                        print opername, 'not in agentlist', incall.agentlist
                                self.amis[astid].queueremove(incall.queuename, 'Agent/%s' % agentnum)
                                self.__send_msg__(userinfo, '%s,%s,,CLR-ACD/' % (incall.commid, incall.sdanum))
                return


        def __addtoqueue__(self, astid, dest, incall):
                """
                Adds the 'dest' Agent to 'incall' call's queue.
                """
                print '__addtoqueue__', dest, incall.commid, incall.sdanum
                self.amis[astid].queueadd(incall.queuename, GHOST_AGENT)
                self.amis[astid].queuepause(incall.queuename, GHOST_AGENT, 'false')
                userinfo = self.ulist[astid].finduser(dest)
                if userinfo is not None:
                        if incall.queuename not in userinfo['queuelist']:
                                agentnum = userinfo['agentnum']
                                # this is the Indispo list
                                userinfo['queuelist'][incall.queuename] = incall
                                incall.agentlist.append(dest)
                                self.amis[astid].queueadd(incall.queuename, 'Agent/%s' % agentnum)
                                self.amis[astid].queuepause(incall.queuename, 'Agent/%s' % agentnum, 'true')
                                self.__send_msg__(userinfo, '%s,%s,,Indispo/' % (incall.commid, incall.sdanum))
                        else:
                                print '__addtoqueue__ : %s is already in the queuelist of %s' % (incall.queuename, dest)
                return


        def __socname__(self, idsoc):
                """
                Returns the 'Permanence' name that matches 'idsoc'.
                """
                columns = ('ID', 'Dossier')
                self.cursor_operat.query('USE system')
                self.cursor_operat.query('SELECT ${columns} FROM societes WHERE ID = %s',
                                         columns,
                                         idsoc)
                results = self.cursor_operat.fetchall()
                if len(results) > 0:
                        sname = results[0][1].lower()
                else:
                        sname = 'perm_inconnue'
                return sname


        def __local_nsoc__(self, idsoc):
                """
                Returns the company's ID that matches 'idsoc'.
                """
                columns = ('N', 'ID')
                self.cursor_operat.query('USE system')
                self.cursor_operat.query('SELECT ${columns} FROM societes WHERE ID = %s',
                                         columns,
                                         idsoc)
                results = self.cursor_operat.fetchall()
                if len(results) > 0:
                        nsoc = str(results[0][0])
                else:
                        nsoc = '0'
                return nsoc


        def getuserlist(self):
                """
                Returns the Agents' list, as defined in Operat DataBase.
                Sets XIVO's musiconhold class according to the 'silent or not' option.
                """
                localulist = {}
                try:
                        columns = ('CODE', 'NOM', 'PASS', 'AGPI')
                        self.cursor_operat.query('USE agents')
                        self.cursor_operat.query('SELECT ${columns} FROM agents',
                                                 columns)
                        agents_agents = self.cursor_operat.fetchall()
                        for r in agents_agents:
                                nope = r[0]
                                agnum = str(STARTAGENTNUM + nope)
                                opername = r[1]
                                passname = r[2]
                                oname = opername
                                pname = passname

                                columns = ('NOPE', 'AdrNet', 'NoDecroche', 'NoMusique')
                                self.cursor_operat.query("SELECT ${columns} FROM acd WHERE NOPE = %s",
                                                         columns,
                                                         str(nope))
                                agents_acd = self.cursor_operat.fetchall()
                                [nodecr, nomusic] = [agents_acd[0][2], agents_acd[0][3]]

                                columns = ('agentid', 'number')
                                self.cursor_xivo.query("SELECT ${columns} FROM agentfeatures WHERE number = %s",
                                                       columns,
                                                       agnum)
                                a = self.cursor_xivo.fetchall()
                                columns = ('id', 'var_metric')
                                self.cursor_xivo.query("SELECT ${columns} FROM agent WHERE id = %s",
                                                       columns,
                                                       a[0][0])
                                b = self.cursor_xivo.fetchall()
                                columns = ('var_metric', 'var_name', 'var_val')
                                self.cursor_xivo.query("SELECT ${columns} FROM agent WHERE var_metric = %s AND var_name = 'musiconhold'",
                                                       columns,
                                                       str(b[0][1] - 1))
                                c = self.cursor_xivo.fetchall()
                                if nomusic == '0':
                                        if c[0][2] == 'silence':
                                                self.cursor_xivo.query("UPDATE agent SET var_val = '%s' "
                                                                       "WHERE var_metric = '%s' AND var_name = 'musiconhold'"
                                                                       % ('ope', str(b[0][1] - 1)))
                                else:
                                        if c[0][2] == 'ope':
                                                self.cursor_xivo.query("UPDATE agent SET var_val = '%s' "
                                                                       "WHERE var_metric = '%s' AND var_name = 'musiconhold'"
                                                                       % ('silence', str(b[0][1] - 1)))
                                
                                phlist = ['sip',
                                          oname,
                                          'nopasswd', #pname,
                                          'default',
                                          None,
                                          agnum,
                                          True,
                                          '1',
                                          ':'.join([str(r[3]), agents_acd[0][2], agents_acd[0][3]])]
                                localulist['SIP/%s' % oname] = phlist


                except Exception, exc:
                        print '--- exception --- in getuserlist()', exc
                return localulist


        def set_cdr_uri(self, uri_operat, uri_xivo):
                """
                Defines the path to the Operat and XIVO MySQL databases and initiates the connections.                
                """
                sqluri = '%s?charset=%s' % (uri_operat, 'latin1')
                self.conn_operat   = anysql.connect_by_uri(sqluri)
                self.cursor_operat = self.conn_operat.cursor()
                self.conn_xivo     = anysql.connect_by_uri(uri_xivo)
                self.cursor_xivo   = self.conn_xivo.cursor()


        def get_list_commands(self):
                """
                Defines the list of allowed commands (client to server direction).
                """
                return self.commands


        def parsecommand(self, linein):
                """
                Sets the Command kind and args according to this class' syntax.
                """
                params = linein.split(',')
                print 'CallBooster : command', params
                cmd = xivo_commandsets.Command(params[-1], params[:-1])
                if cmd.name == 'Init':
                        cmd.type = xivo_commandsets.CMD_LOGIN
                else:
                        cmd.type = xivo_commandsets.CMD_OTHER
                return cmd


        def get_login_params(self, astid, command, connid):
                print 'CallBooster Login :', command.name, command.args
                agent = command.args[0]
                sagent = agent.split('|')
                reference = command.args[1]
                if len(sagent) == 5:
                        # Init from local connection
                        [computername, tagent, phonenum, computeripref, srvnum] = sagent
                        cfg = {'astid' : astid,
                               'proto' : 'sip',
                               'passwd' : 'nopasswd',
                               'state' : 'available',
                               'ident' : 'OP@WIN',
                               'computername' : computername,
                               'phonenum' : phonenum,
                               'computeripref' : computeripref,
                               'srvnum' : srvnum,
                               'userid' : tagent,
                               'version' : 99999
                               }
                        return cfg
                elif len(sagent) == 6:
                        # Init from remote connection
                        # NRef|NOpe|NIdPerm|NTelOpe|NomOpe|AdherentOpe,0,Init
                        [callref, nope, nperm, phonenum, tagent, nadherent] = sagent
                        print 'remote connection', callref, nope, nperm, phonenum, tagent, nadherent
                        if callref in self.pending_sv_fiches:
                                connid.send(self.pending_sv_fiches[callref])
                                agentnum = str(STARTAGENTNUM + int(nope))

                                self.amis[astid].aoriginate_var('iax2/trunkname', phonenum, 'Log %s' % phonenum,
                                                                agentnum, 'agentname', 'default',
                                                                {'CB_MES_LOGAGENT' : 'agentname',
                                                                 'CB_AGENT_NUMBER' : agentnum}, 100)
                                del self.pending_sv_fiches[callref]

                                # adds the user at once
                                phlist = ['sip',
                                          tagent,
                                          'nopasswd',
                                          'default',
                                          None,
                                          '%d' % (STARTAGENTNUM + int(nope)),
                                          True,
                                          '1',
                                          '0:0:0']
                                self.ulist[astid].adduser(phlist)
                                userinfo = self.ulist[astid].finduser(tagent)
                                userinfo['sendfiche'] = ['qcb_%05d' % (int(callref) - 100000), 'Agent/%s' % agentnum]

                                cfg = {'astid' : astid,
                                       'proto' : 'sip',
                                       'passwd' : 'nopasswd',
                                       'state' : 'available',
                                       'ident' : 'OP@WIN',
                                       'computername' : nope,
                                       'phonenum' : phonenum,
                                       'computeripref' : nadherent,
                                       'srvnum' : nperm,
                                       'userid' : tagent,
                                       'version' : 99999
                                       }
                                return cfg
                        else:
                                pass


        def required_login_params(self):
                """
                Returns the list of required login parameters
                """
                return ['astid', 'proto', 'ident', 'userid', 'version', 'computername', 'phonenum', 'computeripref', 'srvnum']


        def connected_srv2clt(self, conn, id):
                """
                Sends a 'connected' status to the client once the TCP link has been setup.
                """
                msg = 'Connect%s/' % id
                print 'CallBooster', 'sending %s' % msg
                conn.send(msg)
                return


        # kind of calls
        # - agent login / logout
        # - outgoing
        #
        # - incoming (sda) : choice => agent
        #

        # Asterisk AMI events
        def link(self, astid, event):
                """
                Function called when an AMI Link Event is read.
                We have a few different cases to handle here :
                - when a new call is issued
                - when a user is removed from its Parking place (Reprise command)
                - when a connection has been established after an Aboute or AppelAboute command
                """
                ch1 = event.get('Channel1').split('/')
                ch2 = event.get('Channel2').split('/')
                print 'LINK', event
                if ch1[0] == 'Agent':
                        # OUTGOING CALL
                        agentnum = ch1[1]
                        peer = event.get('Channel2')
                        callerid = event.get('CallerID2')
                elif ch2[0] == 'Agent':
                        # INCOMING CALL
                        agentnum = ch2[1]
                        peer = event.get('Channel1')
                        callerid = event.get('CallerID1')
                if ch1[0] == 'Agent' or ch2[0] == 'Agent':
                        userinfo = None
                        for agname, agentinfo in self.ulist[astid].list.iteritems():
                                if 'logintimestamp' in agentinfo and agentinfo['agentnum'] == agentnum:
                                        userinfo = agentinfo
                                        break
                        
                        if userinfo is None:
                                log_debug(SYSLOG_WARNING, '(link) no user found for agent number %s' % agentnum)
                                return

                        print 'LINK for Agent/%s : %s (peer = %s)' % (userinfo['agentnum'], userinfo['user'], peer)
                        for callnum, anycall in userinfo['calls'].iteritems():
                                if anycall.peerchannel is None:
                                        # New Call
                                        anycall.peerchannel = peer
                                        print 'LINK (new call)', callnum, anycall.dir, anycall.peerchannel, peer
                                        anycall.set_timestamp_stat('link')
                                        if anycall.dir == 'o':
                                                self.outgoing_calls[peer] = anycall
                                                # self.__update_taxes__(anycall.call, 'Decroche')
                                                self.__send_msg__(userinfo, '%s,1,%s,Decroche/' % (callnum, callerid))
                                        anycall.set_timestamp_tax('link')
                                elif anycall.peerchannel == peer:
                                        if anycall.parking:
                                                print 'LINK but UNPARKING :', callnum, anycall.dir, anycall.parking, anycall.peerchannel, peer
                                                anycall.parking = False
                                        else:
                                                print 'LINK but ?', callnum, peer
                                else:
                                        print 'LINK', callnum, 'ignoring'
                else:
                        ic1 = self.__incallref_from_channel__(event.get('Channel1'))
                        ic2 = self.__incallref_from_channel__(event.get('Channel2'))
                        if ic1 is not None:
                                print 'LINK without Agent (1)', ic1.sdanum, ic1.commid, ic1.appelaboute, ic1.aboute
                                if ic1.appelaboute is not None:
                                        self.__send_msg__(ic1.uinfo, '%s,1,,Raccroche/' % ic1.commid)
                                        self.__send_msg__(ic1.uinfo, '%s,1,,Raccroche/' % ic1.appelaboute)
                                ic1.set_timestamp_stat('link')
                        if ic2 is not None:
                                print 'LINK without Agent (2)', ic2.sdanum, ic2.commid, ic2.appelaboute, ic2.aboute
                                if ic2.aboute is not None:
                                        self.__send_msg__(ic2.uinfo, '%s,1,,Raccroche/' % ic2.commid)
                                        self.__send_msg__(ic2.uinfo, '%s,1,,Raccroche/' % ic2.appelaboute)
                                ic2.set_timestamp_stat('link')

                uniqueid1 = event.get('Uniqueid1')
                if uniqueid1 in self.normal_calls:
                        print '(NORMAL LINK)', uniqueid1, event
                        # self.__update_taxes__
                
                return


        def unlink(self, astid, event):
                """
                Function called when an AMI Unlink Event is read.
                We have a few different cases to handle here :
                - when an established call is over
                - when a peer is put into a Parking (Attente command)
                We must be careful anyway, not to count twice some of these events, since two
                Hangup Events will also be issued.
                """
                print 'UNLINK', event
                ch1 = event.get('Channel1').split('/')
                ch2 = event.get('Channel2').split('/')
                if ch1[0] == 'Agent':
                        # OUTGOING CALL
                        agentnum = ch1[1]
                        peer = event.get('Channel2')
                        callerid = event.get('CallerID2')
                elif ch2[0] == 'Agent':
                        # INCOMING CALL
                        agentnum = ch2[1]
                        peer = event.get('Channel1')
                        callerid = event.get('CallerID1')
                if ch1[0] == 'Agent' or ch2[0] == 'Agent':
                        userinfo = None
                        for agname, agentinfo in self.ulist[astid].list.iteritems():
                                if 'logintimestamp' in agentinfo and agentinfo['agentnum'] == agentnum:
                                        userinfo = agentinfo
                                        break
                        
                        if userinfo is None:
                                log_debug(SYSLOG_WARNING, '(unlink) no user found for agent number %s' % agentnum)
                                return
                        
                        print 'UNLINK : userinfo =', userinfo['calls']
                        # lookup the call to unlink
                        calltounlink = None
                        for callnum, anycall in userinfo['calls'].iteritems():
                                print 'UNLINK', callnum, peer, anycall.peerchannel,
                                if anycall.peerchannel is None:
                                        print 'no action since peerchannel is None'
                                elif anycall.peerchannel == peer:
                                        if anycall.parking:
                                                print 'but PARKING'
                                        elif anycall.appelaboute is not None:
                                                print 'but appelaboute :', anycall.appelaboute
                                        elif anycall.aboute is not None:
                                                print 'but aboute :', anycall.aboute
                                        else:
                                                print anycall.dir
                                                calltounlink = anycall
                                                break
                                else:
                                        print 'else', anycall.peerchannel
                        
                        if calltounlink is not None:
                                if calltounlink.dir == 'i':
                                        print 'unlink INCOMING CALL => __update_stat_acd2__', peer
                                        self.__update_stat_acd2__(calltounlink)
                                self.__update_taxes__(calltounlink, 'Termine')

                                # the link had been established => send Annule
                                self.__send_msg__(userinfo, '%s,1,,Annule/' % callnum)

                                # remove call from incoming or outgoing list
                                userinfo['calls'].pop(callnum)
                else:
                        ic1 = self.__incallref_from_channel__(event.get('Channel1'))
                        ic2 = self.__incallref_from_channel__(event.get('Channel2'))
                        if ic1 is not None:
                                log_debug(SYSLOG_INFO, 'UNLINK without Agent (1) sda=%s commid=%s' % (ic1.sdanum, ic1.commid))
                                ic1.set_timestamp_stat('unlink')
                        if ic2 is not None:
                                log_debug(SYSLOG_INFO, 'UNLINK without Agent (2) sda=%s commid=%s' % (ic2.sdanum, ic2.commid))
                                ic2.set_timestamp_stat('unlink')
                return


        def __originatesuccess__(self, astid, event, kind):
                """
                Common management for OriginateSuccess and AOriginateSuccess Events.
                We use these events in order to know when an Alert call has properly been emitted,
                and that the Link has been established, in order to be able to tell afterwards
                whether the Question has been answered.
                """
                context = event.get('Context')
                uniqueid = event.get('Uniqueid')
                if context == 'ctx-callbooster-alert':
                        channel = event.get('Channel')
                        print '__originatesuccess__', uniqueid, channel
                        self.alerts_uniqueids[uniqueid] = channel
                        # stores the uniqueid in order to detect the appropriate Hangup later on ...
                elif context == 'ctx-callbooster-agentlogin':
                        channel = event.get('Channel')
                        reason = event.get('Reason')
                        exten = event.get('Exten')
                        print '__originatesuccess__ (agentlogin) (%s) :' % kind, astid, event
                        for opername, uinfo in self.ulist[astid].list.iteritems():
                                if 'agentnum' in uinfo and uinfo['agentnum'] == exten:
                                        if 'sendfiche' in uinfo:
                                                del uinfo['sendfiche']
                else:
                        print '__originatesuccess__ (%s) :' % kind, astid, event
                        self.originated_calls.append(uniqueid)
                return


        def __originatefailure__(self, astid, event, kind):
                """
                Common management for OriginateFailure and AOriginateFailure Events.
                We use these events in order to know when an Alert call has failed
                (mainly if the number is unreachable or did not answer).
                """
                context = event.get('Context')
                if context == 'ctx-callbooster-alert':
                        print '__originatefailure__ (alert) (%s) :' % kind, astid, event
                        channel = event.get('Channel')
                        reason = event.get('Reason')
                        if reason == '0':
                                status = 'Echec'
                        elif reason == '3':
                                status = 'Non Décroché' # Timeout
                        else:
                                status = 'Echec_%s' % reason
                                status = 'Occupé'
                        print context, channel, status

                        num = channel[6:-8]
                        toremove = None
                        for i, h in self.alerts.iteritems():
                                # print num, i, h
                                if num in h['numbers'] and h['status'] > 0:
                                        if h['numbers'][h['status'] - 1] == num:
                                                print num, i, h['numbers'], h['status']
                                                h['locked'] = False
                                                if len(h['numbers']) == h['status']:
                                                        toremove = i
                        if toremove is not None: # otherwise it will naturally step to the next number
                                self.__fill_alert_result__(self.alerts[toremove], status)
                                del self.alerts[toremove]

                        self.nalerts_called -= 1
                        self.__alert_calls__(astid)
                elif context == 'ctx-callbooster-agentlogin':
                        channel = event.get('Channel')
                        reason = event.get('Reason')
                        exten = event.get('Exten')
                        print '__originatefailure__ (agentlogin) (%s) :' % kind, astid, event
                        for opername, uinfo in self.ulist[astid].list.iteritems():
                                if 'agentnum' in uinfo and uinfo['agentnum'] == exten:
                                        # reasons : '1' after timeout, '8' unreachable
                                        if 'timer-agentlogin' in uinfo:
                                                print '__originatefailure__ (agentlogin)', uinfo['timer-agentlogin-iter']

                                                if 'sendfiche' in uinfo:
                                                        self.amis[astid].queueremove(uinfo['sendfiche'][0], GHOST_AGENT)
                                                        self.amis[astid].queueremove(uinfo['sendfiche'][0], uinfo['sendfiche'][1])
                                                        del uinfo['sendfiche']

                                                uinfo['timer-agentlogin'].cancel()
                                                if uinfo['timer-agentlogin-iter'] < 3:
                                                        # we fall here when the failure has not been issued by the timeout
                                                        self.__send_msg__(uinfo, ',0,,AppelOpe/')
                                                del uinfo['timer-agentlogin']
                                                del uinfo['timer-agentlogin-iter']
                                        else:
                                                log_debug(SYSLOG_WARNING, '__originatefailure__ (agentlogin) : No timer was on, should not happen (%s)' % exten)
                                                self.__send_msg__(uinfo, ',0,,AppelOpe/')
                else:
                        print '__originatefailure__ (%s) :' % kind, astid, event
                        uniqueid = event.get('Uniqueid')
                        self.originated_calls.append(uniqueid)
                return


        def aoriginatesuccess(self, astid, event):
                """
                Function called when an AMI AOriginateSuccess Event is read.
                """
                self.__originatesuccess__(astid, event, 'a')
                return
        def originatesuccess(self, astid, event):
                """
                Function called when an AMI OriginateSuccess Event is read.
                """
                self.__originatesuccess__(astid, event, '')
                return
        def aoriginatefailure(self, astid, event):
                """
                Function called when an AMI AOriginateFailure Event is read.
                """
                self.__originatefailure__(astid, event, 'a')
                return
        def originatefailure(self, astid, event):
                """
                Function called when an AMI OriginateFailure Event is read.
                """
                self.__originatefailure__(astid, event, '')
                return


        def messagewaiting(self, astid, event):
                """
                Function called when an AMI MessageWaiting Event is read.
                Appropriate parameters are inserted into the 'suivis' table, in order for FB to tell the
                Tacheron program to send the voicemail.
                """
                print 'messagewaiting', astid, event
                datetime = time.strftime('%Y-%m-%d_%H-%M-%S')
                sdanum = event.get('Mailbox').split('@')[0]
                dirname ='/var/spool/asterisk/voicemail/default/%s/INBOX' % sdanum

                idx = 0
                if sdanum in self.waitingmessages:
                        lst = self.waitingmessages[sdanum]
                        if len(lst) > 0:
                                idx = lst.pop()
                        else:
                                print 'messagewaiting : no item in waitingmessages for %s' % sdanum
                else:
                        print 'messagewaiting : no sda %s in waitingmessages' % sdanum
                print 'found index = %s' % (idx)

                self.cursor_operat.query('USE system')
                columns = ('NSDA', 'NSOC', 'NCLI', 'NCOL', 'NOM')
                self.cursor_operat.query('SELECT ${columns} FROM sda WHERE NSDA = %s',
                                         columns,
                                         sdanum)
                system_sda = self.cursor_operat.fetchall()
                [dsda, nsoc, ncli, ncol, collabname] = ['dummy', 0, 0, 0, 'inconnu']
                if len(system_sda) > 0:
                        [dsda, nsoc, ncli, ncol, collabname] = system_sda[0]

                n = 0
                for msg in os.listdir(dirname):
                        fullpath = '/'.join([dirname, msg])
                        if msg.find('.wav') > 0:
                                print 'full path is', fullpath, 'renaming',
                                newfilepath = '/usr/share/asterisk/sounds/callbooster/%s/%s/%s/Messagerie' % (nsoc, ncli, ncol)
                                newfilename = '%s-%s-%02d.wav' % (sdanum, datetime, n)
                                print 'as %s/%s' % (newfilepath, newfilename)
                                n += 1
                                if not os.path.exists(newfilepath):
                                        os.makedirs(newfilepath, 02775)
                                os.system('mv %s %s/%s' % (fullpath, newfilepath, newfilename))
                                # DateP : Date et Heure d'envoi programmé
                                self.cursor_operat.query("INSERT INTO suivis (`NOM`,`NSOC`,`NCLI`,`NCOL`,`NSTRUCT`,`TypeT`,`DateP`,`NAPL`,`ETAT`,`STATUT`) "
                                                         "VALUES ('%s', %d, %d, %d, %s, '%s', '%s', '%s', '%s', '%s')"
                                                         % (collabname, nsoc, ncli, ncol, idx, 'AUDIO',
                                                            datetime, 'ACD', 'ATT', '%s\\\\%s' % (newfilename, DECSTRING.decode('latin1'))))
                        else:
                                print 'full path is', fullpath, 'deleting'
                                os.remove(fullpath)


        def newcallerid(self, astid, event):
                """
                Function called when an AMI NewCallerId Event is read.
                This is used in order to know which asterisk channel to hang-up in order to stop
                calling the agent (when the timeout has elapsed).
                """
                agentnum = event.get('CallerID')
                channel = event.get('Channel')
                for opername, userinfo in self.ulist[astid].list.iteritems():
                        if 'agentnum' in userinfo and userinfo['agentnum'] == agentnum:
                                userinfo['agent-wouldbe-channel'] = channel
                return


        def dial(self, astid, event):
                """
                Function called when an AMI Dial Event is read.
                """
                srcuniqueid = event.get('SrcUniqueID')
                destuniqueid = event.get('DestUniqueID')
                if srcuniqueid in self.originated_calls:
                        self.originated_calls.remove(srcuniqueid)
                else:
                        self.normal_calls.append(srcuniqueid)
                        print '(NORMAL DIAL)', srcuniqueid, destuniqueid, event
                        # self.__init_taxes__(call, event.get('Destination'), event.get('CallerID'), '103', TAXES_PABX, TAXES_TRUNKNAME, 0)
                return


        def hangup(self, astid, event):
                """
                Function called when an AMI Hangup Event is read.
                """
                print 'HANGUP', event
                chan = event.get('Channel')
                uniqueid = event.get('Uniqueid')
                thiscall = self.__incallref_from_channel__(chan)
                if thiscall is not None:
                        print 'HANGUP => __update_stat_acd2__', uniqueid, chan, thiscall.queuename, thiscall.uinfo
                        if thiscall.uinfo is not None:
                                if 'agent-wouldbe-channel' in thiscall.uinfo and 'agentchannel' not in thiscall.uinfo:
                                        self.__send_msg__(thiscall.uinfo, ',-1,,AppelOpe/')
                                        self.amis[astid].hangup(thiscall.uinfo['agent-wouldbe-channel'], '')
                        self.__update_taxes__(thiscall, 'Termine')
                        self.__update_stat_acd2__(thiscall)
                        self.__clear_call_fromqueues__(astid, thiscall)
                        # removes the call from incoming call list
                        self.incoming_calls[thiscall.sdanum].pop(chan)
                else:
                        print 'HANGUP (not incoming call)', uniqueid, event


                if uniqueid in self.alerts_uniqueids:
                        # should not come here after success since it is deleted in uservent treatment
                        print 'HANGUP ALERT', uniqueid, self.alerts_uniqueids[uniqueid]

                        channel = self.alerts_uniqueids[uniqueid]
                        num = channel[6:-8]
                        toremove = None
                        for i, h in self.alerts.iteritems():
                                stopdecroche = h['stopdecroche']
                                if num in h['numbers'] and h['status'] > 0:
                                        if h['numbers'][h['status'] - 1] == num:
                                                print 'HANGUP ALERT', num, i, h['numbers'], h['status'], h['stopdecroche']
                                                h['locked'] = False
                                                if len(h['numbers']) == h['status'] or stopdecroche == 1:
                                                        toremove = i
                        if toremove is not None: # otherwise it will naturally step to the next number
                                self.__fill_alert_result__(self.alerts[toremove], 'Décroché')
                                del self.alerts[toremove]

                        del self.alerts_uniqueids[uniqueid]
                        self.nalerts_called -= 1
                        self.__alert_calls__(astid)
                elif uniqueid in self.normal_calls:
                        print '(NORMAL HANGUP)', uniqueid, event
                        self.normal_calls.remove(uniqueid)
                        # self.__update_taxes__
                return


        def join(self, astid, event):
                """
                Function called when an AMI Join Event is read.
                """
                chan  = event.get('Channel')
                clid  = event.get('CallerID')
                qname = event.get('Queue')
                count = int(event.get('Count'))
                log_debug(SYSLOG_INFO, 'AMI:Join: %s queue=%s %s count=%s %s' % (astid, qname, chan, count, clid))
                return
        

        def parkedcall(self, astid, event):
                """
                Function called when an AMI ParkedCall Event is read.
                """
                print 'PARKEDCALL', event
                # PARKEDCALL clg {'From': 'SIP/101-081c0438', 'CallerID': '101', 'Timeout': '45', 'CallerIDName': 'User1'}
                chan  = event.get('Channel')
                exten = event.get('Exten')

                # find the channel among the incoming calls, otherwise among outgoing ones
                thiscall = self.__incallref_from_channel__(chan)
                if thiscall is None and chan in self.outgoing_calls:
                        thiscall = self.outgoing_calls[chan]

                if thiscall is None:
                        log_debug(SYSLOG_WARNING, 'received a parkedcall from an unknown channel <%s>' % chan)
                        return

                thiscall.set_timestamp_stat('parked')
                thiscall.parkexten = exten
                self.__send_msg__(thiscall.uinfo, '%s,1,,Attente/' % thiscall.commid)


                usercalls = thiscall.uinfo['calls']
                for commid, usercall in usercalls.iteritems():
                        print 'PARKEDCALL', commid, usercall.parkexten

                for commid, usercall in usercalls.iteritems():
                        if commid != thiscall.commid:
                                if usercall.tocall:
                                        print 'ParkedCall Attente', commid, usercall.parking, usercall.parkexten, usercall.appelaboute, usercall.tocall
                                        usercall.tocall = False
                                        self.__outcall__(usercall)
                                elif usercall.forceacd is not None:
                                        [uinfo, qname, agchan] = usercall.forceacd
                                        print 'ok for forceacd ...', qname, agchan, uinfo
                                        uinfo['sendfiche'] = [qname, agchan]

                                        if usercall.dialplan['callerid'] == 1:
                                                cnum = usercall.cidnum
                                        else:
                                                cnum = ''
                                        self.__send_msg__(uinfo, '%s,%s,%s,Fiche/' % (usercall.commid, usercall.sdanum, usercall.cidnum))
                                        self.__send_msg__(uinfo, '%s,%s,,CLR-ACD/' % (usercall.commid, usercall.sdanum))
                                        usercall.forceacd = None
                                elif usercall.toretrieve is not None:
                                        print 'usercall.toretrieve', usercall.toretrieve
                                        self.amis[astid].aoriginate('Agent', usercall.uinfo['agentnum'], 'agentname',
                                                                    usercall.toretrieve, 'cid b', 'default')
                                        usercall.toretrieve = None
                return


        def unparkedcall(self, astid, event):
                """
                Function called when an AMI UnParkedCall Event is read.
                """
                print 'UNPARKEDCALL', event
                chan = event.get('Channel')

                # find the channel among the incoming calls, otherwise among outgoing ones
                thiscall = self.__incallref_from_channel__(chan)
                if thiscall is None and chan in self.outgoing_calls:
                        thiscall = self.outgoing_calls[chan]

                if thiscall is None:
                        print 'received an unparkedcall from an unknown channel <%s>' % chan
                else:
                        thiscall.set_timestamp_stat('unparked')
                        thiscall.parkexten = None
                        self.__send_msg__(thiscall.uinfo, '%s,1,,Reprise/' % thiscall.commid)
                        print 'ParkedCall Reprise', thiscall.uinfo['calls']
                return


        def parkedcallgiveup(self, astid, event):
                """
                Function called when an AMI ParkedCallGiveUp Event is read.
                """
                print 'GIVEUP-PARKEDCALL', astid, event
                chan = event.get('Channel')

                # find the channel among the incoming calls, otherwise among outgoing ones
                thiscall = self.__incallref_from_channel__(chan)
                if thiscall is None and chan in self.outgoing_calls:
                        thiscall = self.outgoing_calls[chan]

                if thiscall is None:
                        print 'received a parkedcallgiveup from an unknown channel <%s>' % chan
                else:
                        thiscall.set_timestamp_stat('parkgiveup')
                        thiscall.parkexten = None
                        self.__send_msg__(thiscall.uinfo, '%s,1,,Annule/' % thiscall.commid)
                        print 'ParkedCall Annule', thiscall.uinfo['calls']
                        # remove the call from userinfo + list
                        # XXX update taxes & stats !
                return


        def agent_was_logged_in(self, astid, event):
                """
                Function called when an AMI Agents Event is read AND that this Agent is not Logged Off.
                This is primarily called at FB's startup and avoids an already-logged-in agent to be
                logged in again.
                """
                agentnum = event.get('Agent')
                agentchannel = event.get('LoggedInChan')
                print 'agent_was_logged_in', astid, event
                for opername, userinfo in self.ulist[astid].list.iteritems():
                        if 'agentnum' in userinfo and userinfo['agentnum'] == agentnum:
                                userinfo['agentchannel'] = agentchannel


        def agentlogin(self, astid, event):
                """
                Function called when an AMI Agentlogin Event is read.
                This is used to send the right acknowledgement to the agent's Operat.
                In case pending calls were waiting to be issued, they are initiated here.
                """
                agentnum = event.get('Agent')
                agentchannel = event.get('Channel')
                print 'agentlogin', astid, event
                for opername, userinfo in self.ulist[astid].list.iteritems():
                        if 'agentnum' in userinfo and userinfo['agentnum'] == agentnum:
                                userinfo['agentchannel'] = agentchannel
                                if 'agent-wouldbe-channel' in userinfo:
                                        if userinfo['agent-wouldbe-channel'] != agentchannel:
                                                print userinfo['agent-wouldbe-channel'], agentchannel
                                        del userinfo['agent-wouldbe-channel']
                                else:
                                        print 'no agent-wouldbe-channel'

                                if 'timer-agentlogin' in userinfo:
                                        userinfo['timer-agentlogin'].cancel()
                                        del userinfo['timer-agentlogin']
                                        del userinfo['timer-agentlogin-iter']
                                # maybe we don't need to send an AppelOpe reply if it has not been explicitly required
                                # reply = ',%d,,AppelOpe/' % (-3)
                                self.__send_msg__(userinfo, ',1,,AppelOpe/')
                                for callnum, anycall in userinfo['calls'].iteritems():
                                        if anycall.tocall:
                                                log_debug(SYSLOG_INFO, 'an outgoing call is waiting to be sent ...')
                                                time.sleep(1) # otherwise the Agent's channel is not found
                                                anycall.tocall = False
                                                self.__outcall__(anycall)
                return


        def queuememberstatus(self, astid, event):
                return


        def agentlogoff(self, astid, event):
                """
                Function called when an AMI Agentlogoff Event is read.
                This occurs when an agent hangs up.
                """
                agentnum = event.get('Agent')
                print 'agentlogoff', astid, event
                for opername, userinfo in self.ulist[astid].list.iteritems():
                        if 'agentchannel' in userinfo and userinfo['agentnum'] == agentnum:
                                print userinfo['agentnum'], 'has left', userinfo['calls']
                                for callnum, anycall in userinfo['calls'].iteritems():
                                        self.__send_msg__(anycall.uinfo, '%s,1,,Annule/' % anycall.commid)
                                # if an outgoing call was there, send an (Annule ?)
                                del userinfo['agentchannel']
                return


        def agentcallbacklogoff(self, astid, event):
                """
                Function called when an AMI Agentcallbacklogoff Event is read.
                This occurs after the RaccrocheOpe has been sent.
                """
                print 'agentcallbacklogoff', astid, event
                return


        def __callback_walkdir__(self, args, dirname, filenames):
                """
                Function called in order to know the presence of M018 files in a given tree.
                This is used in order to setup an MOH class in XIVO with a symlink towards these files.
                """
                if dirname[-5:] == '/Sons':
                        for filename in filenames:
                                if filename.find('M018.') == 0:
                                        self.listreps.append(dirname)


        def pre_reload(self):
                """
                This function is called when the XIVO web-interface sends an 'update' to the FB daemon.
                We sync the agents and voicemails definitions from Operat to XIVO then.
                """
                print 'pre_reload'
                self.__synchro_agents__()
                self.__synchro_sda_voicemail__()
                return


        def pre_moh_reload(self):
                """
                This function is called when the XIVO web-interface sends an 'moh update' to the FB daemon.
                We sync the moh definitions from Operat to XIVO then.
                """
                print 'pre_moh_reload'
                self.__synchro_moh_filesystem__()
                return


        def __synchro_agents__(self):
                """
                Syncs the 'agent' and 'agentfeatures' tables (XIVO) from 'agents' in Operat.
                """
                columns = ('CODE', 'NOM')
                self.cursor_operat.query('USE agents')
                self.cursor_operat.query('SELECT ${columns} FROM agents',
                                         columns)
                agents_agents = self.cursor_operat.fetchall()
                agent_params = {'ackcall' : 'no',
                                'autologoff' : '0',
                                'wrapuptime' : '0',
                                'maxlogintries' : '3',
                                'goodbye' : 'vm-goodbye',
                                'musiconhold' : 'ope',
                                'updatecdr' : 'no',
                                'recordagentcalls' : 'no',
                                'createlink' : 'no',
                                'recordformat' : 'wav',
                                'urlprefix' : '',
                                'custom_beep' : 'beep',
                                'savecallsin' : '/usr/share/asterisk/sounds/web-interface/monitor'}
                for ag in agents_agents:
                        agnum = str(STARTAGENTNUM + ag[0])
                        columns = ('number',)
                        self.cursor_xivo.query('SELECT ${columns} FROM agentfeatures WHERE number = %s',
                                               columns,
                                               agnum)
                        isaginxivo = self.cursor_xivo.fetchall()
                        if len(isaginxivo) > 0:
                                print 'Agent   defined :  %s  =>  %s' %(ag[1], agnum)
                        else:
                                print 'Agent undefined :  %s  =>  %s' %(ag[1], agnum)
                                try:
                                        columns = ('var_metric',)
                                        self.cursor_xivo.query('SELECT ${columns} FROM agent ORDER BY var_metric DESC LIMIT 1',
                                                               columns)
                                        cnum = self.cursor_xivo.fetchall()
                                        curnum = cnum[0][0] + 2
                                        self.cursor_xivo.query("ALTER TABLE agent AUTO_INCREMENT = 0")
                                        self.cursor_xivo.query("INSERT INTO agent VALUES "
                                                               "(0, %d, %d, 0, 'agents.conf', 'agents', '%s', '%s')"
                                                               % (1, curnum, 'agent', '%s,,%s' % (agnum, ag[1])))
                                        self.cursor_xivo.query('SELECT LAST_INSERT_ID()') # last_insert_id
                                        results = self.cursor_xivo.fetchall()
                                        last_insert_id = results[0][0]
                                        for p, vp in agent_params.iteritems():
                                                self.cursor_xivo.query("INSERT INTO agent VALUES "
                                                                       "(0, %d, %d, 0, 'agents.conf', 'agents', '%s', '%s')"
                                                                       % (1, curnum - 1, p, vp))
                                        self.cursor_xivo.query("ALTER TABLE agentfeatures AUTO_INCREMENT = 0")
                                        self.cursor_xivo.query("INSERT INTO agentfeatures VALUES "
                                                               "(0, %d, %d, '%s', '%s', '%s', '', 0, 1)"
                                                               % (last_insert_id, 1, ag[1], '', agnum))
                                        self.conn_xivo.commit()
                                except Exception, exc:
                                        print '--- exception --- could not insert agent (%s) : %s' % (ag[1], str(exc))
                return


        def __synchro_sda_voicemail__(self):
                """
                Syncs the 'uservoicemail' table (XIVO) from 'sda' in Operat,
                since any SDA number shall be associated with a voicemail entry.
                """
                columns = ('NSDA', 'NOM')
                self.cursor_operat.query('USE system')
                self.cursor_operat.query('SELECT ${columns} FROM sda',
                                         columns)
                system_sda = self.cursor_operat.fetchall()
                for operat_sda in system_sda:
                        sdanum = operat_sda[0]
                        columns = ('mailbox',)
                        self.cursor_xivo.query('SELECT ${columns} FROM uservoicemail WHERE mailbox = %s',
                                               columns,
                                               sdanum)
                        issdainxivo = self.cursor_xivo.fetchall()
                        if len(issdainxivo) > 0:
                                print 'SDA     defined :  %s (%s)' %(sdanum, operat_sda[1])
                        else:
                                print 'SDA   undefined :  %s (%s)' %(sdanum, operat_sda[1])
                                try:
                                        columns = ('context', 'mailbox', 'password', 'fullname', 'email', 'tz',
                                                   'attach', 'saycid' , 'review', 'operator', 'envelope', 'sayduration', 'saydurationm')
                                        self.cursor_xivo.query("INSERT INTO uservoicemail (${columns}) "
                                                               "VALUES (%s, %s, %s, %s, %s, %s, %s, NULL, NULL, NULL, NULL, NULL, NULL)",
                                                               columns,
                                                               ('default', sdanum, '', operat_sda[1], 'none@none.none', 'eu-fr', '0'))
                                        self.conn_xivo.commit()
                                except Exception, exc:
                                        print '--- exception --- could not insert voicemailed SDA (%s) : %s' % (sdanum, str(exc))
                return


        def __synchro_moh_filesystem__(self):
                """
                Syncs the 'musiconhold' table (XIVO) from the CallBooster sound directories.
                """
                columns = ('category', 'var_name', 'var_val')
                self.listreps = []
                os.path.walk(REPFILES, self.__callback_walkdir__, None)
                for r in self.listreps:
                        acc = r[len(REPFILES)+1:-5]
                        self.cursor_xivo.query("SELECT ${columns} FROM musiconhold WHERE category = %s AND var_name = %s",
                                               columns,
                                               (acc, 'directory'))
                        results = self.cursor_xivo.fetchall()
                        if len(results) > 0:
                                thisdir = results[0][2]
                        else:
                                print 'insert %s entry in db' % acc
                                newclass = acc
                                moh_params = {'mode' : 'files',
                                              'application' : None,
                                              'random' : 'no'}
                                moh_params['directory'] = '%s/%s' % (MOHFILES, newclass)
                                thisdir = moh_params['directory']
                                for p, vp in moh_params.iteritems():
                                        if p != 'application':
                                                self.cursor_xivo.query("INSERT INTO musiconhold VALUES "
                                                                       "(0, 0, 0, 0, 'musiconhold.conf', '%s', '%s', '%s')"
                                                                       % (newclass, p, vp))
                                        else:
                                                self.cursor_xivo.query("INSERT INTO musiconhold VALUES "
                                                                       "(0, 0, 0, 1, 'musiconhold.conf', '%s', '%s', NULL)"
                                                                       % (newclass, p))
                        if not os.path.exists(thisdir):
                                os.umask(002)
                                prevuid = os.getuid()
                                print 'mkdir a directory for moh :', thisdir, prevuid, r
                                # os.setuid(102)
                                os.makedirs(thisdir, 02775)
                                os.symlink('/'.join([r, 'M018.wav']),
                                           '/'.join([thisdir, 'M018.wav']))
                                os.setuid(prevuid)
                return


        def __outcall__(self, call):
                """
                Issues an outgoing call according to 'call' definitions.
                The relevant conditions are assumed to hold at this point :
                - other communications have been parked (Attente)
                - the calling Agent has been logged in
                """
                # BEGIN OUTGOING CALL (Agent should be logged then)
                retval = 1
                self.__send_msg__(call.uinfo, '%s,%d,%s,Appel/' % (call.commid, retval, call.dest))
                [dorecord, dummy1, dummy2] = call.uinfo['options'].split(':')

                print 'OUTCALL'
                self.amis[call.astid].aoriginate('Agent', call.agentnum, call.agentname,
                                                 call.dest,
                                                 'Appel %s' % call.dest,
                                                 'default')
##                self.amis[call.astid].aoriginate_var('Agent', call.agentnum, call.agentname,
##                                                     call.dest,
##                                                     'Appel %s' % call.dest,
##                                                     'ctx-callbooster-outcall',
##                                                     {'CB_OUTCALL_NUMBER' : call.dest,
##                                                      'CB_OUTCALL_RECORD_FILENAME' : 'myfilenamerecord'},
##                                                     100)
                self.__init_taxes__(call, call.dest, call.agentnum, call.dest,
                                    TAXES_CTI, TAXES_TRUNKNAME, int(call.agentnum) - STARTAGENTNUM)
                return


        def __fill_alert_result__(self, alertdetail, status):
                """
                Fills the 'suivisalertes' table according to each alert call result.
                """
                print '__fill_alert_result__', status
                self.cursor_operat.query("USE system")
                self.cursor_operat.query("INSERT INTO suivisalertes VALUES "
                                         "(0, %s, %d, '%s', '%s', %s, %s, %s, "
                                         "'%s', '%s', '%s', %s, '%s', %s, '%s', '%s', '%s', '%s', %s)"
                                         % (alertdetail['refalerte'],
                                            alertdetail['refequipier'],
                                            'Fini', # Init, Traitement ...
                                            status.decode('latin1'),
                                            alertdetail['nsoc'],
                                            alertdetail['ncli'],
                                            alertdetail['ncol'],
                                            alertdetail['tableequipier'],
                                            alertdetail['typealerte'],
                                            alertdetail['filename'],
                                            alertdetail['refquestion'],
                                            alertdetail['callingnum'],
                                            alertdetail['nbtentatives'],
                                            alertdetail['group'],
                                            alertdetail['nom'],
                                            alertdetail['prenom'],
                                            alertdetail['civ'],
                                            alertdetail['stopdecroche']))
                self.conn_operat.commit()
                return


        def userevent(self, astid, event):
                """
                Function called when an AMI UserEventXXX Event is read, where XXX is almost any string.
                This is used for 2 user-defined events :
                * UserEventAlertCB : we fall here when a 'good' reply has been given by the DTMF Read()
                * UserEventVoiceMailCB : this allows us to connect the voicemail call details to the
                upcoming messagewaiting Event.
                """
                print 'userevent :', astid, event
                evfunction = event.get('Event')
                uniqueid = event.get('Uniqueid')
                if uniqueid in self.alerts_uniqueids:
                        del self.alerts_uniqueids[uniqueid]
                if evfunction == 'UserEventAlertCB':
                        appdatas = event.get('AppData').split('-', 1)
                        dtmfreply = appdatas[0]
                        rid = appdatas[1]
                        if rid in self.alerts:
                                aldetail = self.alerts[rid]
                                print len(self.alerts), self.alerts[rid]
                                self.__fill_alert_result__(aldetail, dtmfreply)
                                del self.alerts[rid]
                                self.nalerts_called -= 1
                        self.__alert_calls__(astid)
                elif evfunction == 'UserEventVoiceMailCB':
                        appdatas = event.get('AppData').split('-')
                        indice = appdatas[0]
                        sdanum = appdatas[1]
                        if sdanum not in self.waitingmessages:
                                self.waitingmessages[sdanum] = []
                        self.waitingmessages[sdanum].append(indice)
                return

        def __agentlogin__(self, astid, userinfo, phonenum, agentnum, opername):
                userinfo['timer-agentlogin'] = threading.Timer(10, self.__callback_agentlogin__)
                userinfo['timer-agentlogin'].start()
                userinfo['timer-agentlogin-iter'] = 0
                self.amis[astid].aoriginate_var('sip', phonenum, 'Log %s' % phonenum,
                                                agentnum, opername, 'ctx-callbooster-agentlogin',
                                                {'CB_MES_LOGAGENT' : opername,
                                                 'CB_AGENT_NUMBER' : agentnum}, 100)


        def manage_srv2clt(self, userinfo_by_requester, connid, parsedcommand, cfg):
                """
                Defines the actions to be proceeded according to the client's commands.
                - AppelOpe :
                - RaccrocheOpe :
                - TransfertOpe :
                - ForceACD :
                """
                cname = parsedcommand.name
                connid_socket = connid[1]
                astid = userinfo_by_requester[0]
                log_debug(SYSLOG_INFO, 'manage_srv2clt : %s / %s' % (cname, str(userinfo_by_requester)))

                if len(parsedcommand.args) > 0:
                        opername = parsedcommand.args[0]
                else:
                        for opername, userinfo in self.ulist[astid].list.iteritems():
                                if 'connection' in userinfo:
                                        print 'manage_srv2clt (connected)', opername, userinfo
                                        if userinfo['connection'] == connid_socket:
                                                opername = userinfo['user']
                userinfo = self.ulist[astid].finduser(opername)
                if 'agentnum' in userinfo:
                        agentnum = userinfo['agentnum']
                        agentid = 'Agent/%s' % agentnum
                else:
                        print '--- no agentnum defined in userinfo'
                
                if cname == 'AppelOpe':
                        if len(parsedcommand.args) == 2:
                                # the phonenum comes from the first Init/ command, therefore doesn't need
                                # to be fetched from the 'postes' table
                                phonenum = userinfo_by_requester[5]
                                if 'agentchannel' in userinfo:
                                        log_debug(SYSLOG_WARNING, 'AppelOpe : %s phone is already logged in as agent number %s' % (phonenum, agentnum))
                                log_debug(SYSLOG_INFO, 'AppelOpe : aoriginate_var for phonenum = %s' % phonenum)
                                self.__agentlogin__(astid, userinfo, phonenum, agentnum, opername)
                        else:
                                reply = ',%d,,AppelOpe/' % (-3)
                                connid_socket.send(reply)

                elif cname == 'RaccrocheOpe':
                        os.popen('asterisk -rx "agent logoff %s"' % agentid)
                        # stat_acd2

                elif cname == 'TransfertOpe':
                        mreference = parsedcommand.args[1]
                        [reference, nope, ncol] = mreference.split('|')
                        if reference in userinfo['calls']:
                                cchan = userinfo['calls'][reference].peerchannel

                                columns = ('N', 'AdrNet')
                                self.cursor_operat.query('USE agents')
                                self.cursor_operat.query('SELECT ${columns} FROM acd WHERE N = %s',
                                                         columns,
                                                         nope)
                                results = self.cursor_operat.fetchall()
                                if len(results) > 0:
                                        addposte = results[0][1]

                                columns = ('TEL', 'NET')
                                self.cursor_operat.query('USE system')
                                self.cursor_operat.query('SELECT ${columns} FROM postes WHERE NET = %s',
                                                         columns,
                                                         addposte)
                                results = self.cursor_operat.fetchall()
                                if len(results) > 0:
                                        print 'TransfertOpe', cchan, addposte, results[0][0]
                                        r = self.amis[astid].transfer(cchan, results[0][0], 'default')

                elif cname == 'ForceACD':
                        """
                        ForceACD is used in order for an Agent to choose the Incoming Call he wants to treat.
                        """
                        reference = parsedcommand.args[1]
                        if reference in userinfo['calls']:
                                print 'ForceACD with ref =', reference
                        elif reference == '0':
                                qlist = userinfo['queuelist']
                                if len(qlist) > 0:
                                        qlidx = qlist.keys()[0]
                                        calltoforce = qlist[qlidx]
                                        calltoforce.forceacd = [userinfo, qlidx, 'Agent/%s' % agentnum]
                                        # remove the call from the queuelist and set it into the call list

                                        # park the current calls
                                        for callnum, anycall in userinfo['calls'].iteritems():
                                                self.__park__(astid, anycall)
                                        userinfo['calls'][calltoforce.commid] = calltoforce
                        else:
                                print 'ForceACD - unknown ref =', reference

                elif cname == 'Appel':
                        mreference = parsedcommand.args[1]
                        [dest, idsoc, idcli, idcol] = mreference.split('|')

                        time.sleep(0.2)
                        self.commidcurr += 1
                        comm_id_outgoing = str(self.commidcurr)

                        socname = self.__socname__(idsoc)
                        outCall = OutgoingCall.OutgoingCall(comm_id_outgoing, astid, self.cursor_operat, socname,
                                                            userinfo, agentnum, opername, dest,
                                                            self.__local_nsoc__(idsoc), idcli, idcol)

                        if len(userinfo['calls']) > 0:
                                print 'Appel : there are already ongoing calls'
                                ntowait = 0
                                for callnum, anycall in userinfo['calls'].iteritems():
                                        ntowait += self.__park__(astid, anycall)
                                userinfo['calls'][comm_id_outgoing] = outCall
                                if ntowait > 0:
                                        userinfo['calls'][comm_id_outgoing].tocall = True
                                else:
                                        self.__outcall__(outCall)
                        else:
                                if 'agentchannel' in userinfo:
                                        userinfo['calls'][comm_id_outgoing] = outCall
                                        self.__outcall__(outCall)
                                else:
                                        userinfo['calls'][comm_id_outgoing] = outCall
                                        userinfo['calls'][comm_id_outgoing].tocall = True
                                        phonenum = userinfo_by_requester[5]
                                        self.__agentlogin__(astid, userinfo, phonenum, agentnum, opername)
                elif cname == 'Raccroche':
                        reference = parsedcommand.args[1]
                        try:
                            if reference in userinfo['calls']:
                                # END OF INCOMING OR OUTGOING CALL
                                anycall = userinfo['calls'][reference]
                                print 'Raccroche', anycall, anycall.appelaboute, anycall.parking, anycall.parkexten, anycall.peerchannel
                                if anycall.peerchannel is None:
                                        self.amis[astid].hangup(agentid, '')
                                else:
                                        self.amis[astid].hangup(anycall.peerchannel, '')
                                try:
                                        if anycall.dir == 'i':
                                                self.__update_stat_acd2__(anycall)
                                        self.__update_taxes__(anycall, 'Termine')
                                except Exception, exc:
                                        print '--- exception --- (Raccroche) :', exc

                                # XXX remove calls
                                userinfo['calls'].pop(reference)

                                retval = 1
                                reply = '%s,%d,,Raccroche/' % (reference, retval)
                                connid_socket.send(reply)
                        except Exception, exc:
                                print '--- exception --- in Raccroche treatment ...', exc


                elif cname == 'Aboute':
                        reference = parsedcommand.args[1]
                        [refcomm_out, refcomm_in] = reference.split('|')
                        print 'Aboute', opername, refcomm_out, refcomm_in

                        if refcomm_in in userinfo['calls']:
                                incall = userinfo['calls'][refcomm_in]
                                callbackexten = incall.parkexten
                        if refcomm_out in userinfo['calls']:
                                chan = userinfo['calls'][refcomm_out].peerchannel
                                incall.aboute = refcomm_out
                        if callbackexten is not None:
                                self.amis[astid].transfer(chan, callbackexten, 'default')


                elif cname == 'AppelAboute': # transfer
                        mreference = parsedcommand.args[1]
                        [dest, refcomm_in, idsoc, idcli, idcol] = mreference.split('|')

                        time.sleep(0.2)
                        self.commidcurr += 1
                        comm_id_outgoing = str(self.commidcurr)

                        if refcomm_in in userinfo['calls']:
                                incall = userinfo['calls'][refcomm_in]
                                print "AppelAboute", incall.dir, incall.peerchannel
                                incall.appelaboute = comm_id_outgoing
                                incall.set_timestamp_stat('appelaboute')
                                r = self.amis[astid].transfer(incall.peerchannel, dest, 'default')

                                socname = self.__socname__(idsoc)
                                outCall = OutgoingCall.OutgoingCall(comm_id_outgoing, astid, self.cursor_operat, socname,
                                                                    userinfo, agentnum, opername, dest,
                                                                    self.__local_nsoc__(idsoc), idcli, idcol)
                                userinfo['calls'][comm_id_outgoing] = outCall
                                retval = 1
                                reply = '%s,%d,%s,Appel/' % (comm_id_outgoing, retval, dest)
                                connid_socket.send(reply)


                # the park/unpark process takes ~ 6 steps:
                # - send Attente to Park (parking => true)
                # - 'Unlink' is applied to the parked Channel => we must remember it anyway ...
                # - 'ParkedCall' is received => reply is sent
                # - send Reprise
                # - 'UnparkedCall' is received => reply is sent
                # ' 'Link' is applied to the parked Channel (parking => false)
                elif cname == 'Attente':
                        reference = parsedcommand.args[1]
                        if reference in userinfo['calls']:
                                anycall = userinfo['calls'][reference]
                                self.__park__(astid, anycall)
                        else:
                                log_debug(SYSLOG_WARNING, 'Attente : the requested reference %s does not exist' % reference)


                elif cname == 'Reprise':
                        reference = parsedcommand.args[1]
                        if reference in userinfo['calls']:
                                anycall = userinfo['calls'][reference]
                                if anycall.parking:
                                        callbackexten = anycall.parkexten
                                        if callbackexten is not None:
                                                ntowait = 0
                                                for callnum, anycall in userinfo['calls'].iteritems():
                                                        ntowait += self.__park__(astid, anycall)
                                                if ntowait > 0:
                                                        anycall.toretrieve = callbackexten
                                                else:
                                                        self.amis[astid].aoriginate('Agent', agentnum, opername,
                                                                                    callbackexten, 'cid b', 'default')
                                        else:
                                                log_debug(SYSLOG_WARNING, 'Reprise : the parkexten number is not defined')
                                else:
                                        log_debug(SYSLOG_WARNING, 'Reprise : the agent is not in Attente mode')
                        else:
                                log_debug(SYSLOG_WARNING, 'Reprise : the requested reference %s does not exist' % reference)


                elif cname == 'Ping':
                        reference = parsedcommand.args[1]
                        reply = ',,,Pong/'
                        connid_socket.send(reply)

                        if 'timer-ping' in userinfo:
                                isalive = userinfo['timer-ping'].isAlive()
                                userinfo['timer-ping'].cancel()
                                del userinfo['timer-ping']
                                if isalive:
                                        print 'timer-ping : was running, renew the timer'
                                else:
                                        print 'timer-ping : received a Ping but the timer was out ... should have been removed when out'
                        else:
                                print 'timer-ping : first setup for this connection'

                        timer = threading.Timer(TIMEOUTPING, self.__callback_ping_noreply__)
                        timer.start()
                        userinfo['timer-ping'] = timer


                elif cname == 'Sortie':
                        if 'timer-ping' in userinfo:
                                userinfo['timer-ping'].cancel()
                                del userinfo['timer-ping']
                        userinfo['cbstatus'] = 'Sortie'
                        self.__choose_and_queuepush__(astid, cname)


                elif cname == 'Change':
                        self.__choose_and_queuepush__(astid, cname)


                elif cname == 'Pause':
                        userinfo['cbstatus'] = 'Pause'


                elif cname == 'Prêt' or cname == 'Sonn':
                        if cname == 'Prêt':
                                reference = parsedcommand.args[1]
                                if userinfo['cbstatus'] == 'Pause':
                                        # XXX send current Indispo's to the user ?
                                        pass
                                userinfo['cbstatus'] = 'Pret' + reference
                        elif cname == 'Sonn':
                                userinfo['cbstatus'] = 'Sonn'
                                reference = '1'

                        if reference == '1':
                                if 'sendfiche' in userinfo:
                                        print 'sendfiche / Pret1', userinfo['sendfiche']
                                        [qname, agname] = userinfo['sendfiche']
                                        self.amis[astid].queueadd(qname, agname)
                                        self.amis[astid].queuepause(qname, agname, 'false')
                                        print 'fiche has been sent :', time.time()
                                return

                        self.__choose_and_queuepush__(astid, 'Pret')


                elif cname == 'Enregistre':
                        reference = parsedcommand.args[1]
                        connid_socket.send(',,,Enregistre/')

##                        if 'agentchannel' in userinfo:
##                                print 'sendfiche, the agent is online :', userinfo['agentchannel']
##                        else:
##                                phonenum = userinfo['phonenum']
##                                opername = userinfo['user']
##                                log_debug(SYSLOG_INFO, 'sendfiche, the agent is not online ... we re going to call him : %s (%s)' % (phonenum, str(userinfo)))
##                                self.__agentlogin__(astid, userinfo, phonenum, agentnum, opername)

                        self.amis[astid].aoriginate_var('Agent', agentnum, opername,
                                                        'record_exten', 'Enregistre', 'ctx-callbooster-record',
                                                        {'CB_RECORD_FILENAME' : reference[0]}, 100)


                elif cname == 'Alerte':
                        mreference = parsedcommand.args[1]
                        [nalerte, idsoc, idcli, idcol] = mreference.split('|')
                        connid_socket.send(',1,,Alerte/')


                        # System Resources
                        columns = ('N', 'JOUR', 'DATED', 'DATEF', 'TYPE',
                                   'PlgD', 'PlgF', 'Valeur')
                        self.cursor_operat.query('USE system')
                        self.cursor_operat.query('SELECT ${columns} FROM ressource_struct',
                                                 columns)
                        system_ressource_struct = self.cursor_operat.fetchall()
                        for srs in system_ressource_struct:
                                print 'A/ (# outgoing lines according to date & time) :', srs
                                if srs[4] == 'ALERTE':
                                        self.nalerts_simult = int(srs[7])

                        columns = ('N', 'NAlerteStruct', 'NomFichierMessage', 'ListeGroupes', 'interval_suivi')
                        self.cursor_operat.query('USE %s_mvts' % self.__socname__(idsoc))
                        self.cursor_operat.query('SELECT ${columns} FROM alertes WHERE N = %s',
                                                 columns,
                                                 nalerte)
                        mvts_alertes = self.cursor_operat.fetchall()
                        self.cursor_operat.query("UPDATE alertes SET Etat = 'Traitement' WHERE N = %s" % nalerte)
                        self.conn_operat.commit()
                        for rr in mvts_alertes:
                                numstruct = rr[1]
                                filename = rr[2] # message file name
                                grouplist = rr[3].split(';')
                                intervsuivi = rr[4] # in seconds

                                # Alerte command => fetch details
                                print 'B/ Alerte : filename = %s (groups = %s) intsuivi = %s' %(filename, str(grouplist), intervsuivi)
                                columns = ('N', 'Libelle', 'NCol', 'NQuestion',
                                           'Type_Traitement', 'Nom_table_contact', 'Type_Alerte',
                                           'CallingNumber', 'nbTentatives', 'Alerte_Tous', 'Stop_Decroche')
                                self.cursor_operat.query('USE %s_clients' % self.__socname__(idsoc))
                                self.cursor_operat.query('SELECT ${columns} FROM alerte_struct WHERE N = %s',
                                                         columns,
                                                         numstruct)
                                clients_alerte_struct = self.cursor_operat.fetchall()
                                if len(clients_alerte_struct) > 0:
                                        nquestion     = clients_alerte_struct[0][3]
                                        typealerte    = clients_alerte_struct[0][6]
                                        callingnum    = clients_alerte_struct[0][7]
                                        nbtentatives  = clients_alerte_struct[0][8]
                                        alertetous    = clients_alerte_struct[0][9]
                                        stopdecroche  = clients_alerte_struct[0][10]

                                if typealerte != 'Tel':
                                        break

                                # Fetch question details
                                columns = ('N', 'Libelle', 'Descriptif', 'Fichier', 'Type_saisie',
                                           'Touches_autorisees', 'Touches_terminales', 'Touche_repete',
                                           'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9',
                                           'AttenteMax', 'TValidate')
                                self.cursor_operat.query('USE system')
                                self.cursor_operat.query('SELECT ${columns} FROM questions WHERE N = %s',
                                                         columns,
                                                         nquestion)
                                system_questions = self.cursor_operat.fetchall()
                                if len(system_questions) > 0:
                                        filename = 'callbooster/Questions/%s' % system_questions[0][3]
                                        # filename = 'fr/ss-noservice' # XXX
                                        digits_allowed = system_questions[0][5]
                                        digits_end = system_questions[0][6]
                                        digits_repeat = system_questions[0][7]

                                nid = 0
                                for gl in grouplist:
                                        try:
                                                columns = ('N', 'Groupe', 'nom', 'prenom',
                                                           'tel1', 'tel2', 'tel3', 'tel4',
                                                           'Civilite', 'Fax', 'eMail', 'SMS', 'Code', 'DureeSonnerie')
                                                self.cursor_operat.query('USE %s_annexe' % self.__socname__(idsoc))
                                                self.cursor_operat.query('SELECT * FROM %s ORDER BY N' % gl)
                                                base_annexe_results = self.cursor_operat.fetchall()
                                                for annx in base_annexe_results:
                                                        rid = '%s-%06d' % (nalerte, nid)
                                                        nid += 1
                                                        listnums = []
                                                        blistnums = []
                                                        for inum in annx[4:8]:
                                                                if inum != '': blistnums.append(inum)
                                                        for k in xrange(int(nbtentatives)):
                                                                listnums.extend(blistnums)                                                        
                                                        self.alerts[rid] = {'nsoc' : idsoc,
                                                                            'filename' : filename,
                                                                            'digits-allowed' : digits_allowed,
                                                                            'digits-end' : digits_end,
                                                                            'digits-repeat' : digits_repeat,
                                                                            'numbers' : listnums,
                                                                            'timetoring' : int(annx[13]),
                                                                            'refalerte' : nalerte,
                                                                            'refequipier' : annx[0],
                                                                            'group' : annx[1],
                                                                            'tableequipier' : gl,
                                                                            'nom' : annx[2],
                                                                            'prenom' : annx[3],
                                                                            'civ' : annx[8],
                                                                            'nsoc' : idsoc,
                                                                            'ncli' : idcli,
                                                                            'ncol' : idcol,
                                                                            'refquestion' : nquestion,
                                                                            'typealerte' : typealerte,
                                                                            'callingnum' : callingnum,
                                                                            'nbtentatives' : nbtentatives,
                                                                            'alertetous' : alertetous,
                                                                            'stopdecroche' : stopdecroche,
                                                                            'status' : 0,
                                                                            'locked' : False
                                                                            }
                                        except Exception, exc:
                                                print 'grouplist %s : %s' % (gl, str(exc))
                        # initiate alerts if needed
                        self.__alert_calls__(astid)

                elif cname == 'SynchroXivoAgents':
                        self.__synchro_agents__()
                elif cname == 'SynchroXivoSDA':
                        self.__synchro_sda_voicemail__()
                elif cname == 'SynchroXivoMOH':
                        self.__synchro_moh_filesystem__()
                else:
                        print 'CallBooster : unmanaged command <%s>' % cname
                return


        def __choose_and_queuepush__(self, astid, info_whocalled):
                """
                To be called any time one would like to update the queues's status.
                """
                try:
                        print '__choose_and_queuepush__ : MANAGE (after %s)' % info_whocalled
                        todo = self.__choose__(astid)
                        print '__choose_and_queuepush__ : CHOOSE (after %s)' % info_whocalled, todo
                        for opername_iter, mtd in todo.iteritems():
                                if len(mtd) > 0:
                                        td = mtd[0]
                                        # print 'pretx', td
                                        if td[0] == 'push':
                                                self.__clear_call_fromqueues__(astid, td[1])
                                                self.__sendfiche__(astid, opername_iter, td[1])
                                                print 'manage : fiche sent to %s' % opername_iter
                                        elif td[0] == 'enqueue':
                                                print 'manage : enqueue %s (pret)' % opername_iter
                                                self.__addtoqueue__(astid, opername_iter, td[1])
                                        elif td[0] == 'dequeue':
                                                print 'manage : dequeue %s (pret)' % opername_iter
                                                userinfo = self.ulist[astid].finduser(opername_iter)
                                                td[1].agentlist.remove(opername_iter)
                                                self.amis[astid].queueremove(td[1].queuename, 'Agent/%s' % userinfo['agentnum'])
                                                nagents = len(td[1].agentlist)
                                                print 'nagents = %d' % nagents
                                                if nagents == 0:
                                                        self.amis[astid].queueremove(td[1].queuename, GHOST_AGENT)
                except Exception, exc:
                        print '--- exception --- __choose_and_queuepush__ (%s) : %s' % (info_whocalled, str(exc))


        def __alert_calls__(self, astid):
                """
                Spans the still-to-call numbers and calls them if all the allowed lines are not used.
                """
                # XXX status when all the numbers have been spanned ?
                LOCNUMS = ['103', '101', '103', '102'] # for testing purposes only !!
                ncalls = 0
                for rid, al in self.alerts.iteritems():
                        if al['status'] < len(al['numbers']) and not al['locked'] and self.nalerts_called < self.nalerts_simult:
                                ncalls += 1
                                dstnum = al['numbers'][al['status']]
                                print 'calling', al['nom'], '(%s) (or should be ...)' % dstnum
                                ### dstnum = LOCNUMS[al['status']] # XXX for testing purposes only !!
                                self.amis[astid].aoriginate_var('local', '%s@default' % dstnum, 'dest %s' % dstnum,
                                                                'cidFB', 'automated call', 'ctx-callbooster-alert',
                                                                {'CB_ALERT_MESSAGE' : al['filename'],
                                                                 'CB_ALERT_ID' : rid,
                                                                 'CB_ALERT_ALLOWED' : al['digits-allowed'],
                                                                 'CB_ALERT_REPEAT' : al['digits-repeat']},
                                                                al['timetoring'])
                                al['status'] += 1
                                al['locked'] = True
                                self.nalerts_called += 1
                print '%d calls issued' % ncalls
                return


        def loginko(self, cfg, errorstring):
                """
                Actions to perform when login was KO.
                """
                print 'loginko', cfg, errorstring
                return '%s,0,,Init/' % cfg.get('srvnum')


        def loginok(self, userinfo, capa_user, versions, configs, cfg):
                """
                Actions to perform when login was OK.
                Replies Init/
                """
                return '%s,1,,Init/' % cfg.get('srvnum')


        def __moh_check__(self, path):
                """
                Checks whether the 'path' class is defined in XIVO, and falls back to the common moh class
                if needed.
                """
                columns = ('category', 'var_name', 'var_val')
                self.cursor_xivo.query("SELECT ${columns} FROM musiconhold WHERE var_name = %s AND category = %s",
                                       columns,
                                       ('directory', path))
                results = self.cursor_xivo.fetchall()
                if results == ():
                        return 'callbooster'
                else:
                        return path


        def __park__(self, astid, anycall):
                """
                Sends the Parking command into the appropriate channel.
                The acknowledgement will come from the AMI event ParkedCall.
                """
                print '__park__ : attempt : current .parking is %s, .peerchannel is %s' % (anycall.parking, anycall.peerchannel)
                if anycall.parking:
                        print '__park__ : the call #%s is already parked' % anycall.commid
                        return 0
                if anycall.peerchannel is None:
                        print '__park__ : no peerchannel is defined for the call #%s' % anycall.commid
                        return 0
                
                anycall.parking = True
                if anycall.dir == 'o':
                        true_mohclass = self.__moh_check__(anycall.mohclass)
                        print '__park__ : the mohclass will be set to %s and not %s' % (true_mohclass, anycall.mohclass)
                        self.amis[astid].setvar(anycall.peerchannel, 'CB_MOH', true_mohclass)
                self.amis[astid].setvar(anycall.peerchannel, 'PARKDIGIMUTE', '1')
                self.amis[astid].transfer(anycall.peerchannel, PARK_EXTEN, 'default')
                return 1


        def svreply(self, astid, msg):
                """
                Manages the replies from remote Operat servers
                """
                params = msg.strip(chr(4)).split(chr(3))
                cmd = params[0]
                val = params[1]
                if cmd == 'ACDReponse':
                        print '(received) ACDReponse', msg
                        sdanum = params[5]
                        commid = params[6]
                        if sdanum in self.incoming_calls:
                                for chan, ic in self.incoming_calls[sdanum].iteritems():
                                        if ic.commid == commid :
                                                status = None
                                                if val == 'Absent':
                                                        toremove = None
                                                        for j, k in ic.list_svirt.iteritems():
                                                                if k['request'][:6] == params[2:]:
                                                                        toremove = j
                                                        if toremove is not None:
                                                                del ic.list_svirt[toremove]
                                                                if len(ic.list_svirt) == 0:
                                                                        self.amis[astid].queueremove(ic.queuename, GHOST_AGENT)
                                                                else:
                                                                        print ic.list_svirt
                                                        else:
                                                                print 'found nobody to remove ...'
                                                elif val == 'Trouvé':
                                                        # FicheD
                                                        if ic.dialplan['callerid'] == 1:
                                                                cnum = ic.cidnum
                                                        else:
                                                                cnum = ''
                                                        reply = '%s,%s-%s-%s-%s,%s,FicheD/' % (ic.commid,
                                                                                               ic.sdanum, params[2], params[3], params[4],
                                                                                               cnum)
                                                        self.pending_sv_fiches[ic.commid] = reply
                                                elif val == 'Attente':
                                                        timer = threading.Timer(5, self.__callback_svcheck__)
                                                        timer.start()
                                                        ic.svirt = {'params' : params,
                                                                    'timer' : timer,
                                                                    'val' : '1'}
                                                        print '(Attente) : starting a Timer :', timer
                                                print 'OperatSock : received reply for :', sdanum, commid, ic, ic.commid, val
                elif cmd == 'ACDCheckRequest':
                        print '(received) ACDCheckRequest', msg
                        commid = params[2]
                        iic = None
                        for sdanum, lic in self.incoming_calls.iteritems():
                                for chan, ic in lic.iteritems():
                                        if ic.commid == commid :
                                                iic = ic
                        if iic is not None:
                                timer = threading.Timer(5, self.__callback_svcheck__)
                                timer.start()
                                iic.svirt['timer'] = timer
                                iic.svirt['val'] = val
                                print '(ACDCheckRequest) : starting a Timer :', timer
                                
                                if val == '0':
                                        print 'ACDCheckRequest : request not in SV'
                                elif val == '-1':
                                        print 'ACDCheckRequest : request still in SV'
                return None


        def checkqueue(self):
                """
                Checks the contents of queues (when triggered by the local pipe).
                """
                buf = os.read(self.queued_threads_pipe[0], 1024)
                print 'checkqueue, size =', self.tqueue.qsize()
                disconnlist = []
                while self.tqueue.qsize() > 0:
                        thisthread = self.tqueue.get()
                        tname = thisthread.getName()
                        if tname.find('Ping') == 0:
                                print 'checkqueue (Ping)', tname
                                for astid in self.ulist:
                                        for opername, userinfo in self.ulist[astid].list.iteritems():
                                                if 'timer-ping' in userinfo and userinfo['timer-ping'] == thisthread:
                                                        agentnum = userinfo['agentnum']
                                                        self.cursor_operat.query('USE agents')
                                                        self.cursor_operat.query("UPDATE acd SET Etat = 'Sortie' WHERE NOPE = %d"
                                                                                 % (int(agentnum) - STARTAGENTNUM))
                                                        self.conn_operat.commit()
                                                        self.__choose_and_queuepush__(astid, 'Unping')
                                                        del userinfo['timer-ping']
                                                        disconnlist.append(userinfo)

                        elif tname.find('SV') == 0:
                                print 'checkqueue (SV)', tname
                                for sdanum, lic in self.incoming_calls.iteritems():
                                        for chan, ic in lic.iteritems():
                                                if ic.svirt is not None and ic.svirt['timer'] == thisthread:
                                                        v = ic.svirt['params']
                                                        val = int(ic.svirt['val'])
                                                        if val != 0:
                                                                req = 'ACDCheckRequest' + chr(2) + chr(2).join(v[2:8]) + chr(3)
                                                                print 'req =', v, req
                                                                if self.soperat_socket is not None:
                                                                        self.soperat_socket.send(req)
                                                                ic.svirt['timer'] = None
                                                        else:
                                                                if ic.commid in self.pending_sv_fiches:
                                                                        req = 'ACDCheckRequest' + chr(2) + chr(2).join(v[2:8]) + chr(3)
                                                                        if self.soperat_socket is not None:
                                                                                self.soperat_socket.send(req)
                                                                        ic.svirt['timer'] = None
                        elif tname.find('AgentLogin') == 0:
                                print 'checkqueue (AgentLogin)', tname
                                for astid in self.ulist:
                                        for opername, userinfo in self.ulist[astid].list.iteritems():
                                                if 'timer-agentlogin' in userinfo and userinfo['timer-agentlogin'] == thisthread:
                                                        if userinfo['timer-agentlogin-iter'] < 2:
                                                                self.__send_msg__(userinfo, ',-2,,AppelOpe/')
                                                                userinfo['timer-agentlogin'] = threading.Timer(10, self.__callback_agentlogin__)
                                                                userinfo['timer-agentlogin'].start()
                                                                userinfo['timer-agentlogin-iter'] += 1
                                                        else:
                                                                self.__send_msg__(userinfo, ',-3,,AppelOpe/')
                                                                userinfo['timer-agentlogin-iter'] += 1
                                                                if 'agent-wouldbe-channel' in userinfo:
                                                                        self.amis[astid].hangup(userinfo['agent-wouldbe-channel'], '')
                                                                        del userinfo['agent-wouldbe-channel']
                        else:
                                print 'checkqueue (unknown event kind)', tname
                return disconnlist


        def __callback_ping_noreply__(self):
                """
                This function is called when 2 mins have elapsed since the previously received ping.
                """
                thisthread = threading.currentThread()
                tname = thisthread.getName()
                print '__callback_ping_noreply__ (timer finished)', time.asctime(), tname
                thisthread.setName('Ping-' + tname)
                self.tqueue.put(thisthread)
                os.write(self.queued_threads_pipe[1], 'ping-')
                return


        def __callback_svcheck__(self):
                """
                This function is called once the 5 seconds have elapsed after an SV request.
                """
                thisthread = threading.currentThread()
                tname = thisthread.getName()
                print '__callback_svcheck__ (timer finished)', time.asctime(), tname
                thisthread.setName('SV-' + tname)
                self.tqueue.put(thisthread)
                os.write(self.queued_threads_pipe[1], 'svcheck-')
                return


        def __callback_agentlogin__(self):
                """
                This function is called once the 10 seconds after agent login attempt have been spent.
                """
                thisthread = threading.currentThread()
                tname = thisthread.getName()
                print '__callback_agentlogin__ (timer finished)', time.asctime(), tname
                thisthread.setName('AgentLogin-' + tname)
                self.tqueue.put(thisthread)
                os.write(self.queued_threads_pipe[1], 'agentlogin-')
                return


        def __init_taxes__(self, call, numbertobill, fromN, toN, fromS, toS, NOpe):
                """
                Fills the 'taxes' table at the start of a call.
                """
                try:
                        [juridict, impulsion] = self.__gettaxes__(numbertobill)
                        call.settaxes(impulsion)
                        datetime = time.strftime(DATETIMEFMT)
                        self.cursor_operat.query('USE system')
                        self.cursor_operat.query('INSERT INTO taxes VALUES (0, %s, 0, %s, %d, %d, %s, %s, %s, %s, %s, %d, %s, %s, %s, %s, %s, %s, %s)'
                                                 % (call.commid,
                                                    '"%s"' % datetime,
                                                    0, # Duree
                                                    0, # DureeSonnerie
                                                    call.nsoc,
                                                    call.ncli,
                                                    '"%s"' % call.cliname,
                                                    call.ncol,
                                                    '"%s"' % call.colname,
                                                    NOpe,
                                                    fromN,
                                                    toN,
                                                    '"%s"' % fromS,
                                                    '"%s"' % toS,
                                                    '"Sonnerie"',
                                                    impulsion[0],
                                                    juridict))
                        self.conn_operat.commit()
                        self.cursor_operat.query('SELECT LAST_INSERT_ID()') # last_insert_id
                        results = self.cursor_operat.fetchall()
                        call.insert_taxes_id = results[0][0]
                except Exception, exc:
                        print '--- exception --- (__init_taxes__) :', exc


        def __update_taxes__(self, anycall, state):
                """
                Updates the 'taxes' table of the 'system' base.
                This is for the outgoing calls as well as incoming calls.
                """
                try:
                        now_t_time = time.localtime()
                        duree = time.mktime(now_t_time) - time.mktime(anycall.ctime)
                        duree_int = int(duree)
                        [tpc, dpc, dlt] = anycall.taxes
                        if duree_int >= dpc and dlt > 0:
                                ntaxes = tpc + 1 + (duree_int - dpc) / dlt
                        else:
                                ntaxes = tpc

                        anycall.set_timestamp_tax('END')
                        dureesonnerie = 0
                        t1 = -1
                        t2 = -1
                        t3 = -1
                        for jj, k in anycall.ttimes.iteritems():
                                if k == 'init':
                                        t1 = jj
                                elif k == 'link':
                                        t2 = jj
                                elif k == 'END':
                                        t3 = jj
                        if t2 > -1:
                                dureesonnerie = t2 - t1
                        else:
                                dureesonnerie = t3 - t1

                        print '__update_taxes__ : commid = %s, arg = %s, state = %s, ' \
                              'durees = %f, %d, %d, taxes = %d' \
                              %(anycall.commid, str(anycall.taxes), state, duree, duree_int, dureesonnerie, ntaxes)

                        self.cursor_operat.query('USE system')
                        self.cursor_operat.query('UPDATE taxes SET Duree = %d, DureeSonnerie = %d, Etat = %s, nbTaxes = %d WHERE RefFB = %s'
                                                 % (duree, dureesonnerie, '"%s"' % state, ntaxes, anycall.commid))
                        self.conn_operat.commit()
                except Exception, exc:
                        print '--- exception --- (__update_taxes__) :', exc, anycall, state
                return


        def __update_stat_acd__(self, state, t0, in_period,
                                tt_raf, tt_asd, tt_snd, tt_sfa, tt_sop,
                                tt_tel, tt_fic, tt_bas, tt_mes, tt_rep):
                """
                Changes the 30min-updated stats (stat_acd) once the per-call ones (stat_acd2)
                have been filled in.
                """
                datetime = time.strftime(DATETIMEFMT, time.localtime(int(t0 / TSLOTTIME) * TSLOTTIME))
                period = '_'.join(in_period).strip('_')
                log_debug(SYSLOG_INFO, '__update_stat_acd__ : datetime = %s (period = %s)' %(datetime, period))

                try:
                        columns = ('DATE', 'Periode', 'SDA_NC', 'SDA_NV', 'SDA_HDV', 'SDA_V', 'TTraitement',
                                   'TT_RAF', 'TT_ASD', 'TT_SND', 'TT_SFA', 'TT_SOP',
                                   'TT_TEL', 'TT_FIC', 'TT_BAS', 'TT_MES', 'TT_REP',
                                   'NSOC' )
                        self.cursor_operat.query('USE system')
                        self.cursor_operat.query('SELECT ${columns} FROM stat_acd WHERE DATE = %s',
                                                 columns,
                                                 datetime)
                        system_stat_acd = self.cursor_operat.fetchall()
                        if len(system_stat_acd) > 0:
                                r = system_stat_acd[0]
                                [nnc, nnv, nhdv, nv, ttime,
                                 ntt_raf, ntt_asd, ntt_snd, ntt_sfa, ntt_sop,
                                 ntt_tel, ntt_fic, ntt_bas, ntt_mes, ntt_rep,
                                 nsoc] = r[2:18]
                        else:
                                [nnc, nnv, nhdv, nv] = [0, 0, 0, 0]
                                [ttime, 
                                 ntt_raf, ntt_asd, ntt_snd, ntt_sfa, ntt_sop,
                                 ntt_tel, ntt_fic, ntt_bas, ntt_mes, ntt_rep,
                                 nsoc] = [0, 0, 0, 0, 0, 0,
                                          0, 0, 0, 0, 0, 0]

                        if state == 'V':
                                nv += 1
                        elif state == 'NV':
                                nnv += 1
                        elif state == 'NC':
                                nnc += 1
                        elif state == 'HDV':
                                nhdv += 1
                        
                        ntt_raf = ntt_raf + tt_raf
                        ntt_asd = ntt_asd + tt_asd
                        ntt_snd = ntt_snd + tt_snd
                        ntt_sfa = ntt_sfa + tt_sfa
                        ntt_sop = ntt_sop + tt_sop
                        ntt_tel = ntt_tel + tt_tel
                        ntt_fic = ntt_fic + tt_fic
                        ntt_bas = ntt_bas + tt_bas
                        ntt_mes = ntt_mes + tt_mes
                        ntt_rep = ntt_rep + tt_rep
                        
                        if len(system_stat_acd) > 0:
                                self.cursor_operat.query("UPDATE stat_acd SET SDA_NC = %d, SDA_NV = %d, SDA_HDV = %d, SDA_V = %d,"
                                                         " TT_RAF = %d, TT_ASD = %d, TT_SND = %d, TT_SFA = %d, TT_SOP = %d,"
                                                         " TT_TEL = %d, TT_FIC = %d, TT_BAS = %d, TT_MES = %d, TT_REP = %d"
                                                         " WHERE DATE = '%s'"
                                                         % (nnc, nnv, nhdv, nv,
                                                            ntt_raf, ntt_asd, ntt_snd, ntt_sfa, ntt_sop,
                                                            ntt_tel, ntt_fic, ntt_bas, ntt_mes, ntt_rep,
                                                            datetime))
                        else:
                                self.cursor_operat.query('INSERT INTO stat_acd VALUES'
                                                         ' (0, %s, %s,'
                                                         ' %d, %d, %d, %d,'
                                                         ' %d,'
                                                         ' %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,'
                                                         ' %d)'
                                                         % ('"%s"' % datetime,
                                                            '"%s"' % period,
                                                            nnc, nnv, nhdv, nv,
                                                            ttime,
                                                            ntt_raf, ntt_asd, ntt_snd, ntt_sfa, ntt_sop,
                                                            ntt_tel, ntt_fic, ntt_bas, ntt_mes, ntt_rep,
                                                            nsoc))
                        self.conn_operat.commit()
                except Exception, exc:
                        print 'exception in __update_stat_acd__ :', exc
                return


        def __update_stat_acd2__(self, incall):
                """
                Updates the 'stat_acd2' table of the 'system' base.
                This is only for incoming calls.
                """
                if incall.statdone:
                        log_debug(SYSLOG_INFO, '__update_stat_acd2__ : STAT ALREADY DONE for commid <%s>' % incall.commid)
                        return
                incall.statdone = True

                log_debug(SYSLOG_INFO, '__update_stat_acd2__ (END OF INCOMING CALL) for commid <%s>' % incall.commid)
                now_t_time = time.localtime()
                now_f_time = time.strftime(DATETIMEFMT, now_t_time)
                datetime = time.strftime(DATETIMEFMT, incall.ctime)

                state = incall.statacd2_state
                tts   =  STATACD2_STATUSES
                if incall.statacd2_tt in tts.keys():
                        tts[incall.statacd2_tt] = 1
                else:
                        log_debug(SYSLOG_WARNING, '__update_stat_acd2__ : %s is not a valid status' % incall.statacd2_tt)

                incall.set_timestamp_stat('END')
                sortedtimes = incall.stimes.keys()
                sortedtimes.sort()
                nks = len(sortedtimes)
                bil = {}
                is_appelaboute = False
                print '__update_stat_acd2__ : history =',
                for t in xrange(nks - 1):
                        act = incall.stimes[sortedtimes[t]]
                        if act == 'appelaboute':
                                is_appelaboute = True
                        if act == 'link' and is_appelaboute:
                                act = 'linkaboute'
                        dt = sortedtimes[t+1] - sortedtimes[t]
                        print '(', dt, act, ')',
                        if act not in bil:
                                bil[act] = 0
                        bil[act] += dt
                print
                print '__update_stat_acd2__ : history by action =',
                for act in bil:
                        print '(', act, bil[act], ')',
                print
                dtime = sortedtimes[nks - 1] - sortedtimes[0]
                print '__update_stat_acd2__ : history - total time =', dtime
                if incall.uinfo is not None:
                        print '__update_stat_acd2__ : uinfo calls', incall.uinfo['calls']
                        opername = incall.uinfo['user']
                else:
                        opername = ''
                
                dec = 0

                [tacd, tope, tatt, tattabo, tabo,
                 ttel, trep, tmes, tsec,    tdec] = [0, 0, 0, 0, 0,
                                                     0, 0, 0, 0, 0]
                if 'parked' in bil:
                        tatt = int(bil['parked'])
                if 'appelaboute' in bil:
                        tattabo = int(bil['appelaboute'])
                if 'linkaboute' in bil:
                        tabo = int(bil['linkaboute'])
                if 'secretariat' in bil:
                        tacd = int(bil['secretariat'])
                        if 'link' in bil:
                                tope = int(bil['link'])
                if 'tel' in bil:
                        ttel = int(bil['tel'])
                        if 'link' in bil:
                                tdec = int(bil['link'])
                if 'mes' in bil:
                        tmes = int(bil['mes'])
                if 'rep' in bil:
                        trep = int(bil['rep'])
                if 'secours' in bil:
                        tsec = int(bil['secours'])

                try:
                        self.cursor_operat.query('USE system')
                        self.cursor_operat.query('INSERT INTO stat_acd2 VALUES'
                                                 ' (0, %s, %s, %s, %s, %s, %d,'
                                                 ' %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,'
                                                 ' %s, %s, %s,'
                                                 ' %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,'
                                                 ' %s, %s, %d)'
                                                 % ('"%s"' % datetime,
                                                    '"%s"' % now_f_time,
                                                    '"%s"' % incall.sdanum,
                                                    '"%s"' % incall.cidnum,
                                                    '"%s"' % state,
                                                    int(dtime),
                                                    tts['TT_RAF'], tts['TT_ASD'], tts['TT_SND'], tts['TT_SFA'], tts['TT_SOP'],
                                                    tts['TT_TEL'], tts['TT_FIC'], tts['TT_BAS'], tts['TT_MES'], tts['TT_REP'],
                                                    incall.nsoc,
                                                    incall.ncli,
                                                    incall.ncol,
                                                    tacd, tope, tatt, tattabo, tabo,
                                                    ttel, trep, tmes, tsec,    tdec,
                                                    dec,
                                                    '"%s"' % opername,
                                                    '"%s"' % incall.socname,
                                                    incall.insert_taxes_id))
                        self.conn_operat.commit()
                        self.__update_stat_acd__(state, sortedtimes[0], incall.period,
                                                 tts['TT_RAF'], tts['TT_ASD'], tts['TT_SND'], tts['TT_SFA'], tts['TT_SOP'],
                                                 tts['TT_TEL'], tts['TT_FIC'], tts['TT_BAS'], tts['TT_MES'], tts['TT_REP'])
                except Exception, exc:
                        print 'exception in __update_stat_acd2__ :', exc

                return


        def __gettaxes__(self, num):
                """
                Returns the tax informations according to the phone number 'num'.
                """
                # num = '003%d' % random.randint(0, 999999999) # fake, for testing purposes ...
                juridict = self.__juridictions__(num)
                if juridict is not None:
                        impulsion = self.__impulsion__(juridict)
                else:
                        impulsion = None
                        juridict = '0'

                print '__gettaxes__ :', num, juridict, impulsion

                if impulsion is None:
                        impulsion = [0, 0, 30]
                return [juridict, impulsion]


        def __juridictions__(self, num):
                """
                Returns the 'Juridiction' ID according to the phone number 'num'.
                """
                columns = ('Numero', 'Juridiction', 'Type_Num', 'Description', 'Local')
                self.cursor_operat.query('USE system')
                self.cursor_operat.query("SELECT ${columns} FROM juridict",
                                         columns)
                results = self.cursor_operat.fetchall()
                jurs = None
                maxlen = 0
                for r in results:
                        v = r[0]
                        if len(v) > maxlen and num.find(v) == 0:
                                print r
                                maxlen = len(v)
                                jurs = r
                if jurs is None:
                        return None
                else:
                        return jurs[1]


        def __impulsion__(self, jur):
                """
                Returns the tax informations according to the 'Juridiction' ID.
                """
                columns = ('Juridiction', 'Info', 'NbTaxePC', 'DureePC', 'DureeTaxe')
                self.cursor_operat.query('USE system')
                self.cursor_operat.query('SELECT ${columns} FROM impulsion WHERE Juridiction = %s',
                                         columns,
                                         jur)
                results = self.cursor_operat.fetchall()
                if len(results) > 0:
                        r = results[0]
                        imp = [r[2], r[3], r[4]]
                else:
                        imp = None
                return imp


        def __listqueues__(self):
                """
                Returns the unsorted list of currently used queues.
                """
                l = []
                for sdanum, inc in self.incoming_calls.iteritems():
                        for chan, icall in inc.iteritems():
                                l.append(icall.queuename)
                return l


        def __choosequeuenum__(self):
                """
                Chooses a queue number among the available ones.
                """
                lst = self.__listqueues__()
                for qnum in xrange(10):
                        qname = 'qcb_%05d' % qnum
                        if qname not in lst:
                                return qnum
                return None


        def __incallref_from_channel__(self, chan):
                """
                Returns the IncomingCall reference that is associated with Asterisk channel reference 'chan'.
                """
                thiscall = None
                for sdanum, inc in self.incoming_calls.iteritems():
                        if chan in inc:
                                thiscall = inc[chan]
                                break
                return thiscall


        def __choose__(self, astid):
                """
                After a login, a Change, a Pret's, a logoff, a new incoming/outgoing call.
                """
                # choose, according to other incoming calls & logged in/out, if one has to wait or push
                byprio = []
                for u in xrange(6):
                        byprio.append([])
                for sdanum, inc in self.incoming_calls.iteritems():
                        for chan in inc:
                                tc = inc[chan]
                                if tc.waiting:
                                        # beware : when is tc.elect_prio set ?
                                        # maybe it would be useful to have everything sorted as soon as the call comes first
                                        byprio[tc.elect_prio].append(tc)
                myidx = None
                mycall = None
                todo_by_oper = {}
                todo_by_call = {}
                time.sleep(0.1) # wait in order for the database values to be compliant ...
                for sdaprio in xrange(6):
                        if len(byprio[sdaprio]) > 0:
                                for incall in byprio[sdaprio]:
                                        # choose the operator or the list of queues for this incoming call
                                        topush = {}
                                        to_enqueue = []
                                        for opername in incall.list_operators:
                                                if opername not in todo_by_oper:
                                                        todo_by_oper[opername] = []
                                                opstatus = incall.check_operator_status(opername)
                                                print '__choose__ : (SDA prio = %d) <%s> (%s)' % (sdaprio, opername.encode('latin1'), opstatus)
                                                userinfo = self.ulist[astid].finduser(opername)
                                                userqueuesize = len(userinfo['queuelist'])
                                                if opstatus is not None:
                                                        if 'connection' in userinfo:
                                                                print '__choose__ :', opername.encode('latin1'), userinfo, opstatus, userqueuesize
                                                                [status, dummy, level, prio, busyness] = opstatus
                                                                # print '__choose__ : incall : %s / opername : %s %s' % (incall.cidnum, opername, status)
                                                                if status == 'Pret0':
                                                                        if len(todo_by_oper[opername]) == 0 and userqueuesize + 1 >= int(level):
                                                                                topush[opername] = [int(prio), int(busyness)]
                                                                        else:
                                                                                to_enqueue.append(opername)
                                                                elif status in ['Pret1', 'Pause', 'Sonn']:
                                                                        to_enqueue.append(opername)
                                                else:
                                                        if userqueuesize > 0:
                                                                if incall.queuename in userinfo['queuelist']:
                                                                        todo_by_oper[opername].append(['dequeue', incall])


                                        print '__choose__ : callid = %s :' % incall.commid, topush, to_enqueue
                                        if len(topush) == 0: # noone available for this call yet
                                                for opername in to_enqueue:
                                                        todo_by_oper[opername].append(['enqueue', incall])
                                        else:
                                                opername_push = None
                                                if len(topush) == 1: # somebody available
                                                        opername_push = topush.keys()[0]
                                                else: # choose among the pushers, according to prio and busyness
                                                        maxp = 0
                                                        minb = 100000
                                                        for opn, sel in topush.iteritems():
                                                                [p, b] = sel
                                                                if p > maxp or p >= maxp and b < minb:
                                                                        opername_push = opn
                                                                        maxp = p
                                                                        minb = b
                                                if opername_push is not None:
                                                        todo_by_oper[opername_push].append(['push', incall])
                return todo_by_oper


        def elect(self, astid, inchannel, cidnum, sdanum, upto):
                """
                Called by an Incoming Call in order to find the proper agent.
                """
                action = 'exit'
                delay = 0
                value = ''
                newupto = upto
                thiscall = None
                if upto == '0':
                        queuenum = self.__choosequeuenum__()
                        print ' NCOMING CALL ## building an IncomingCall structure ##'
                        thiscall = IncomingCall.IncomingCall(self.cursor_operat,
                                                             cidnum, sdanum, queuenum,
                                                             self.opejd, self.opend)

                        if thiscall.statacd2_state != 'NC':
                                thiscall.setclicolnames()

                        self.__init_taxes__(thiscall, cidnum, cidnum, sdanum, TAXES_TRUNKNAME, TAXES_CTI, 0)

                        if thiscall.statacd2_state == 'V':
                                print ' NCOMING CALL ## calling get_sda_profiles ##'
                                if sdanum not in self.incoming_calls:
                                        self.incoming_calls[sdanum] = {}
                                self.__clear_call_fromqueues__(astid, thiscall)
                                ret = thiscall.get_sda_profiles(len(self.incoming_calls[sdanum]))
                                if ret == True:
                                        self.incoming_calls[sdanum][inchannel] = thiscall
                                        print ' NCOMING CALL : list of used SDA :', self.incoming_calls
                                else:
                                        self.__update_taxes__(thiscall, 'Termine')
                                        self.__update_stat_acd2__(thiscall)
                                        thiscall = None
                        else:
                                self.__update_taxes__(thiscall, 'Termine')
                                self.__update_stat_acd2__(thiscall)
                                thiscall = None
                else:
                        if sdanum in self.incoming_calls and inchannel in self.incoming_calls[sdanum]:
                                thiscall = self.incoming_calls[sdanum][inchannel]

                #  retstatus = welcome if needed
                argument = None
                if thiscall is not None:
                        print ' NCOMING CALL ## calling findaction ##'
                        [action, delay, argument, newupto] = thiscall.findaction(upto)
                        # findaction will find who might be available for this incoming call
                        # however me must take care of other current calls (those which are not yet 'solved')
                        # in order to be able to deal with the SDA priorities meanwhile : thiscall.elect_prio

                        print ' NCOMING CALL : findaction result : %s / %s / %s / %s' % (action, delay, argument, newupto)

                        if action == 'mes' or action == 'rep' or action == 'tel' or action == 'fic' or action == 'bas' or action == 'intro':
                                # maybe we had been in 'secretariat' mode previously, so that we shouldn't wait anymore
                                thiscall.waiting = False
                                # send CLR-ACD to the users who had received an Indispo as concerning this call
                                self.__clear_call_fromqueues__(astid, thiscall)
                        elif action == 'secretariat':
                                thiscall.waiting = True
                                log_debug(SYSLOG_INFO, 'elect : calling __choose__()')
                                print ' NCOMING CALL : secretariat /////', thiscall.list_operators, thiscall.list_svirt
                                for nid, svirtparams in thiscall.list_svirt.iteritems():
                                        if svirtparams['status'] == 'init':
                                                # warning 'status' field of list_svirt is reset to 'init' at each findaction() ...
                                                request = svirtparams['request']
                                                thiscall.list_svirt[nid]['status'] = 'acdaddrequest'
                                                req = 'ACDAddRequest' + chr(2) \
                                                      + chr(2).join(request[:6]) + chr(2) \
                                                      + chr(2).join(request[6:]) \
                                                      + chr(2) + str(self.soperat_port) + chr(3)
                                                if self.soperat_socket is not None:
                                                        self.soperat_socket.send(req)
                                        else:
                                                print 'request already sent :', svirtparams['request']
                                todo = self.__choose__(astid)
                                print 'CHOOSE (after Incall)', todo

                                self.amis[astid].queueadd(thiscall.queuename, GHOST_AGENT)
                                self.amis[astid].queuepause(thiscall.queuename, GHOST_AGENT, 'false')
                                # once all the queues have been spanned, send the push / queues where needed
                                argument = 'welcome'
                                nevt = 0
                                for opername_iter, couplelist in todo.iteritems():
                                        for td in couplelist:
                                                log_debug(SYSLOG_INFO, 'elect : secretariat / %s / %s / %s' % (opername_iter, td[0], td[1].sdanum))
                                                if td[0] == 'push':
                                                        if thiscall == td[1]:
                                                                nevt += 1
                                                                self.__clear_call_fromqueues__(astid, td[1])
                                                                self.__sendfiche__(astid, opername_iter, td[1])
                                                                delay = 100
                                                                argument = None
                                                        else:
                                                                pass
                                                elif td[0] == 'enqueue':
                                                        nevt += 1
                                                        self.__addtoqueue__(astid, opername_iter, td[1])
                                                elif td[0] == 'dequeue':
                                                        print 'dequeue after Incall ???', opername_iter, td
                                                        pass
                                if nevt == 0:
                                        if len(thiscall.list_svirt) == 0:
                                                action = 'noqueue'
                        elif action == 'exit':
                                self.__update_taxes__(thiscall, 'Termine')
                                self.__update_stat_acd2__(thiscall)
                        else:
                                print '### action is <%s> which I don t know, exiting anyway' % action
                                self.__update_taxes__(thiscall, 'Termine')
                                self.__update_stat_acd2__(thiscall)

                        # here we go back to the AGI : we must tell the dialplan what to do ...
                        # - 'mes', 'rep' => go where needed
                        # - 'again' : if, among the chosen agents, all are 'sortie'

                        whattodo = {'action' : action,
                                    'delay' : delay,
                                    'argument' : argument,
                                    'newupto' : newupto,
                                    'queuename' : thiscall.queuename,
                                    'dialplan' : thiscall.dialplan}
                else:
                        print ' NCOMING CALL : no call processed'
                        whattodo = {'action' : action,
                                    'delay' : delay,
                                    'argument' : argument,
                                    'newupto' : newupto,
                                    'queuename' : None,
                                    'dialplan' : None}
                return whattodo



        # No-action methods for CallBooster class.

        def park_srv2clt(self, function, args):
                """Does nothing in CallBooster"""
                return None
        
        def update_srv2clt(self, phoneinfo):
                """Does nothing in CallBooster"""
                return None
        
        def message_srv2clt(self, sender, message):
                """Does nothing in CallBooster"""
                return None
        
        def dmessage_srv2clt(self, message):
                """Does nothing in CallBooster"""
                return None
        
        def features_srv2clt(self, direction, message):
                """Does nothing in CallBooster"""
                return None
        
        def phones_srv2clt(self, function, args):
                """Does nothing in CallBooster"""
                return None


xivo_commandsets.CommandClasses['callbooster'] = CallBoosterCommand
