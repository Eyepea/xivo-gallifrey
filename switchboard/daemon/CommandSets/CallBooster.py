
__version__   = '$Revision$ $Date$'
__copyright__ = 'Copyright (C) 2007, Proformatique'
__author__    = 'Corentin Le Gall'

import anysql
import ConfigParser
import random
import re
import os
import pickle
import socket
import time
import xivo_commandsets
from Calls import IncomingCall
from Calls import OutgoingCall
from xivo_commandsets import BaseCommand
from xivo_common import *

STARTAGENTNUM = 6100
DATEFMT = '%Y-%m-%d'
DATETIMEFMT = DATEFMT + ' %H:%M:%S'
PARK_EXTEN = '700'
TRUNKNAME = 'FT'
TSLOTTIME = 600

incoming_calls = {}
outgoing_calls = {}

class ByAgentChannelStatus:
        def __init__(self, direction, call):
                self.dir   = direction
                self.call  = call
                return


class CallBoosterCommand(BaseCommand):
        """
        CallBoosterCommand class.
        Defines the behaviour of commands.
        """
        def __init__(self, ulist, amis, operatsocket, operatport, operatini):
		BaseCommand.__init__(self)
                self.commands = ['Init',
                                 'AppelOpe', 'TransfertOpe', 'RaccrocheOpe',
                                 'Appel', 'Aboute', 'AppelAboute', 'Raccroche',
                                 'Enregistre', 'Alerte',
                                 'Ping',
                                 'Attente', 'Reprise',
                                 'Sonn',
                                 'Prêt', 'ForceACD',
                                 'Pause',
                                 'Sortie',
                                 'Change']
                self.ulist = ulist
                self.amis  = amis
                self.commidcurr = 200000
                self.soperat_socket = operatsocket
                self.soperat_port   = operatport
                self.conn_clients = {}
                opconf = ConfigParser.ConfigParser()
                opconf.readfp(open(operatini))
                opconf_so = dict(opconf.items('SO'))
                d = opconf_so.get('opejd')
                n = opconf_so.get('opend')
                #print d, n
                self.list_svirt = []
                self.pending_sv_fiches = []


        def __sendfiche_a(self, userinfo, incall):
                userinfo['calls'][incall.commid] = ByAgentChannelStatus('i', incall)
                # CLR-ACD to,be sent only if there was an Indispo sent previously
                if incall.dialplan['callerid'] == 1:
                        cnum = incall.cidnum
                else:
                        cnum = ''
                reply = '%s,%s,%s,Fiche/' % (incall.commid, incall.sdanum, cnum)
                incall.waiting = False
                incall.uinfo = userinfo
                if 'connection' in userinfo:
                        incall.uinfo['connection'].send(reply)
                        print 'CallBooster : sent <%s>' % reply
                else:
                        print 'CallBooster : could not send', replies
                return


        # the call comes here when the AGI has directly found a peer
        def __sendfiche(self, astid, dest, incall):
                userinfo = self.ulist[astid].finduser('sip' + dest)
                print '__sendfiche', userinfo
                if userinfo is not None:
                        agentnum = userinfo['agentnum']
                        if 'agentchannel' in userinfo:
                                print 'sendfiche, the agent is online :', userinfo['agentchannel']
                        else:
                                print 'sendfiche, the agent is not online ... we\'re going to call him'
                                phonenum = userinfo['phonenum']
                                agentname = userinfo['user'][3:]
                                self.amis[astid].aoriginate_var('sip', phonenum, 'Log %s' % phonenum,
                                                                agentnum, agentname, 'default', 'CB_MES_LOGAGENT', agentname)
                        self.__sendfiche_a(userinfo, incall)
                        # better wait for 'pret1' before establishing the call ?
                        print '__sendfiche', incall.queuename, agentnum
                        userinfo['queuelist'][incall.queuename] = incall
                        userinfo['sendfiche'] = [incall.queuename, 'Agent/%s' % agentnum]
                return


        def __clear_call_fromqueues(self, astid, incall):
                for agname, userinfo in self.ulist[astid].list.iteritems():
                        if incall.queuename in userinfo['queuelist']:
                                del userinfo['queuelist'][incall.queuename]
                                agentnum = userinfo['agentnum']
                                print '__clear_call_fromqueues : removing %s (queue %s) for agent %s' %(incall.sdanum, incall.queuename, agentnum)
                                self.amis[astid].queueremove(incall.queuename, 'Agent/%s' % agentnum)
                                reply = '%s,%s,,CLR-ACD/' % (incall.commid, incall.sdanum)
                                if 'connection' in userinfo:
                                        userinfo['connection'].send(reply)
                                        print 'CallBooster : sent <%s>' % reply
                                else:
                                        print 'CallBooster : can not send <%s>' % reply
                return


        def __addtoqueue(self, astid, dest, incall):
                print '__addtoqueue', dest, incall.commid, incall.sdanum
                userinfo = self.ulist[astid].finduser('sip' + dest)
                if userinfo is not None:
                        if incall.queuename not in userinfo['queuelist']:
                                agentnum = userinfo['agentnum']
                                # this is the Indispo list
                                userinfo['queuelist'][incall.queuename] = incall
                                self.amis[astid].queueadd(incall.queuename, 'Agent/%s' % agentnum)
                                self.amis[astid].queuepause(incall.queuename, 'Agent/%s' % agentnum, 'true')
                                reply = '%s,%s,,Indispo/' % (incall.commid, incall.sdanum)
                                if 'connection' in userinfo:
                                        userinfo['connection'].send(reply)
                                        print '__addtoqueue : sent <%s>' % reply
                                else:
                                        print '__addtoqueue : can not send <%s>' % reply
                        else:
                                print '__addtoqueue : %s is already in the queuelist of %s' % (incall.queuename, dest)
                return


        def __socname(self, idsoc):
                columns = ('N', 'NOM', 'ID', 'Dossier')
                cursor_system = self.conn_system.cursor()
                cursor_system.query('SELECT ${columns} FROM societes WHERE ID = %s',
                                    columns,
                                    idsoc)
                results = cursor_system.fetchall()
                if len(results) > 0:
                        sname = results[0][3].lower()
                else:
                        sname = 'adh_inconnu'
                return sname


        def getuserlist(self):
                localulist = {}
                sqluri_agents = '%s/agents' % self.uribase
                sqluri_system = '%s/system' % self.uribase

                try:
                        print 'connecting to URI %s - it can take some time' % sqluri_system
                        self.conn_system = anysql.connect_by_uri(sqluri_system)
                        print 'connecting to URI %s - done' % sqluri_system
                        print 'connecting to URI %s - it can take some time' % sqluri_agents
                        self.conn_agents = anysql.connect_by_uri(sqluri_agents)
                        print 'connecting to URI %s - done' % sqluri_agents
                        cursor_agents = self.conn_agents.cursor()
                        
                        columns = ('CODE', 'NOM', 'PASS')
                        cursor_agents.query('SELECT ${columns} FROM agents',
                                            columns)
                        results = cursor_agents.fetchall()
                        for r in results:
                                opername = r[1]
                                passname = r[2]
                                # in order to avoid tricky u'sdlkfs'
                                oname = opername.__repr__()[2:-1].replace('\\xe9', 'é').replace('\\xea', 'è')
                                pname = passname.__repr__()[2:-1].replace('\\xe9', 'é').replace('\\xea', 'è')
                                phlist = ['sip%s' % oname,
                                          'nopasswd', #pname,
                                          'default',
                                          None,
                                          '%d' % (STARTAGENTNUM + r[0]),
                                          True,
                                          '1']
                                localulist['SIP/%s' % oname] = phlist

                except Exception, exc:
                        print '--- exception --- in getuserlist()', exc
                return localulist


        def set_cdr_uri(self, uribase):
                """In this CB case, defines the path to the Operat MySQL database."""
                self.uribase = uribase
                # the following commands do nothing really useful, except initializing some field somewhere
                sqluri_dummy = '%s?charset=%s' % (uribase, 'latin1')
                print 'connecting to URI %s - it can take some time' % sqluri_dummy
                conn_dummy = anysql.connect_by_uri(sqluri_dummy)
                print 'connecting to URI %s - done' % sqluri_dummy
                conn_dummy.close()


        def get_list_commands_clt2srv(self):
                """Defines the list of allowed commands."""
                return self.commands

        def get_list_commands_srv2clt(self):
                return None

        def parsecommand(self, linein):
                params = linein.split(',')
                print 'CallBooster : command', params
                cmd = xivo_commandsets.Command(params[-1], params[:-1])
                if cmd.name == 'Init':
                        cmd.type = xivo_commandsets.CMD_LOGIN
                else:
                        cmd.type = xivo_commandsets.CMD_OTHER
                return cmd

        def get_login_params(self, command, astid):
                print 'CallBooster Login :', command.name, command.args
                agent = command.args[0]
                reference = command.args[1]
                [computername, tagent, phonenum, computeripref, srvnum] = agent.split('|')
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

        def required_login_params(self):
                return ['astid', 'proto', 'ident', 'userid', 'version', 'computername', 'phonenum', 'computeripref', 'srvnum']

        def connected_srv2clt(self, conn, id, issv):
                msg = 'Connect%s/' % id
                print 'CallBooster', 'sending %s' % msg
                conn.send(msg)
                # if issv, we receive an incoming connection from a SV host
                if issv:
                        for p in self.pending_sv_fiches:
                                conn.send(p)
                return


        # kind of calls
        # - agent login / logout
        # - outgoing
        #
        # - incoming (sda) : choice => agent
        #

        # Asterisk AMI events
        def link(self, astid, event):
                ch1 = event.get('Channel1').split('/')
                ch2 = event.get('Channel2').split('/')
                # print 'LINK', ch1, ch2
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
                        self.ulist[astid].acquire()
                        try:
                                userinfo = None
                                for agname, agentinfo in self.ulist[astid].list.iteritems():
                                        if 'logintimestamp' in agentinfo and agentinfo['agentnum'] == agentnum:
                                                userinfo = agentinfo
                                                break

                                if userinfo is not None:
                                        print 'LINK for Agent/%s : %s (peer = %s)' % (userinfo['agentnum'], userinfo['user'], peer)
                                        connid_socket = userinfo['connection']
                                        for callnum, anycall in userinfo['calls'].iteritems():
                                                        if anycall.call.peerchannel is None:
                                                                # New Call
                                                                anycall.call.peerchannel = peer
                                                                print 'LINK (new call)', callnum, anycall.dir, anycall.call.peerchannel, peer
                                                                anycall.call.set_timestamp_stat('link')
                                                                if anycall.dir == 'o':
                                                                        outgoing_calls[peer] = anycall.call
                                                                        # self.__update_taxes(anycall.call, 'Decroche')
                                                                        reply = '%s,1,%s,Decroche/' % (callnum, callerid)
                                                                        connid_socket.send(reply)
                                                                anycall.call.set_timestamp_tax('link')
                                                        elif anycall.call.peerchannel == peer:
                                                                if anycall.call.parking is not None:
                                                                        print 'LINK but UNPARKING :', callnum, anycall.dir, anycall.call.parking, anycall.call.peerchannel, peer
                                                                        anycall.call.parking = None
                                                                else:
                                                                        print 'LINK but ?', callnum, peer
                                                        else:
                                                                print 'LINK', callnum, 'ignoring'
                                else:
                                        print 'LINK : no Agent found for event', event
                        finally:
                                self.ulist[astid].release()
                else:
                        ic1 = self.__incallref_from_channel(event.get('Channel1'))
                        ic2 = self.__incallref_from_channel(event.get('Channel2'))
                        if ic1 is not None:
                                print 'LINK without Agent', ic1.sdanum, ic1.commid
                                ic1.set_timestamp_stat('link')
                return


        def unlink(self, astid, event):
                """
                The AMI has detected an unlink.
                Useful when the peer hangs off.
                """
                # print '// unlink //', event
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
                        self.ulist[astid].acquire()
                        try:
                                userinfo = None
                                for agname, agentinfo in self.ulist[astid].list.iteritems():
                                        if 'logintimestamp' in agentinfo and agentinfo['agentnum'] == agentnum:
                                                userinfo = agentinfo
                                                break

                                if userinfo is not None:
                                                print 'UNLINK : userinfo =', userinfo['calls']
                                                connid_socket = userinfo['connection']

                                                # lookup the call to unlink
                                                calltounlink = None
                                                for callnum, anycall in userinfo['calls'].iteritems():
                                                        print 'UNLINK', callnum, peer, anycall.call.peerchannel,
                                                        if anycall.call.peerchannel is None:
                                                                print 'no action since peerchannel is None'
                                                        elif anycall.call.peerchannel == peer:
                                                                if anycall.call.parking == callnum:
                                                                        print 'but PARKING :', anycall.call.parking
                                                                elif anycall.call.appelaboute is not None:
                                                                        print 'but appelaboute :', anycall.call.appelaboute
                                                                else:
                                                                        print anycall.dir
                                                                        calltounlink = anycall
                                                                        break
                                                        else:
                                                                print 'else', anycall.call.peerchannel

                                                if calltounlink is not None:
                                                        if calltounlink.dir == 'i':
                                                                print 'unlink INCOMING CALL => __update_stat_acd2', peer
                                                                self.__update_stat_acd2(calltounlink.call)
                                                        self.__update_taxes(calltounlink.call, 'Termine')

                                                        # the link had been established => send Annule
                                                        connid_socket.send('%s,1,,Annule/' % callnum)

                                                        ### why ???
                                                        #for queuename in userinfo['queuelist'].keys():
                                                        #       self.amis[astid].queueremove(queuename, 'Agent/%s' % agentnum)
                                                        #userinfo['queuelist'].clear()

                                                        # remove call from incoming or outgoing list
                                                        userinfo['calls'].pop(callnum)
                        finally:
                                self.ulist[astid].release()
                else:
                        ic1 = self.__incallref_from_channel(event.get('Channel1'))
                        ic2 = self.__incallref_from_channel(event.get('Channel2'))
                        if ic1 is not None:
                                print 'UNLINK without Agent', ic1.sdanum, ic1.commid
                                ic1.set_timestamp_stat('unlink')
                return


        def dial(self, astid, event):
                # print 'DIAL', event
                return


        def hangup(self, astid, event):
                chan = event.get('Channel')
                thiscall = self.__incallref_from_channel(chan)
                if thiscall is not None:
                        print 'HANGUP => __update_stat_acd2', chan, thiscall.queuename
                        self.__update_taxes(thiscall, 'Termine')
                        self.__update_stat_acd2(thiscall)
                        self.__clear_call_fromqueues(astid, thiscall)
                        # removes the call from incoming call list
                        incoming_calls[thiscall.sdanum].pop(chan)
                return


        def parkedcall(self, astid, event):
                print 'PARKEDCALL', astid, event
                # PARKEDCALL clg {'From': 'SIP/101-081c0438', 'CallerID': '101', 'Timeout': '45', 'CallerIDName': 'User1'}
                chan  = event.get('Channel')
                exten = event.get('Exten')

                # find the channel among the incoming calls, otherwise among outgoing ones
                thiscall = self.__incallref_from_channel(chan)
                if thiscall is None and chan in outgoing_calls:
                        thiscall = outgoing_calls[chan]

                if thiscall is None:
                        print 'received a parkedcall from an unknown channel <%s>' % chan
                else:
                        thiscall.set_timestamp_stat('parked')
                        thiscall.parkexten = exten
                        reply = '%s,1,,Attente/' % (thiscall.commid)
                        thiscall.uinfo['connection'].send(reply)
                        usercalls = thiscall.uinfo['calls']
                        for commid, usercall in usercalls.iteritems():
                                if commid != thiscall.commid:
                                        if usercall.call.tocall:
                                                print 'ParkedCall Attente', commid, usercall.call.parking, usercall.call.parkexten, usercall.call.appelaboute, usercall.call.tocall
                                                usercall.call.tocall = False
                                                self.__outcall(usercall.call)
                                        elif usercall.call.forceacd is not None:
                                                [uinfo, qname, agchan] = usercall.call.forceacd
                                                print 'ok for forceacd ...', qname, agchan, uinfo
                                                uinfo['sendfiche'] = [qname, agchan]

                                                if usercall.call.dialplan['callerid'] == 1:
                                                        cnum = usercall.call.cidnum
                                                else:
                                                        cnum = ''
                                                reply = '%s,%s,%s,Fiche/' % (usercall.call.commid, usercall.call.sdanum, usercall.call.cidnum)
                                                uinfo['connection'].send(reply)
                                                
                                                reply = '%s,%s,,CLR-ACD/' % (usercall.call.commid, usercall.call.sdanum)
                                                uinfo['connection'].send(reply)
                                                usercall.call.forceacd = None
                                        elif usercall.call.toretrieve is not None:
                                                print 'usercall.call.toretrieve', usercall.call.toretrieve
                                                r = self.amis[astid].aoriginate('Agent', usercall.call.uinfo['agentnum'], 'agentname',
                                                                                usercall.call.toretrieve, 'cid b', 'default')
                                                usercall.call.toretrieve = None
                return


        def unparkedcall(self, astid, event):
                print 'UNPARKEDCALL', astid, event
                chan = event.get('Channel')

                # find the channel among the incoming calls, otherwise among outgoing ones
                thiscall = self.__incallref_from_channel(chan)
                if thiscall is None and chan in outgoing_calls:
                        thiscall = outgoing_calls[chan]

                if thiscall is None:
                        print 'received an unparkedcall from an unknown channel <%s>' % chan
                else:
                        thiscall.set_timestamp_stat('unparked')
                        thiscall.parkexten = None
                        reply = '%s,1,,Reprise/' % (thiscall.commid)
                        thiscall.uinfo['connection'].send(reply)
                        print 'ParkedCall Reprise', thiscall.uinfo['calls']
                return


        def parkedcallgiveup(self, astid, event):
                print 'GIVEUP-PARKEDCALL', astid, event
                chan = event.get('Channel')

                # find the channel among the incoming calls, otherwise among outgoing ones
                thiscall = self.__incallref_from_channel(chan)
                if thiscall is None and chan in outgoing_calls:
                        thiscall = outgoing_calls[chan]

                if thiscall is None:
                        print 'received a parkedcallgiveup from an unknown channel <%s>' % chan
                else:
                        thiscall.set_timestamp_stat('parkgiveup')
                        thiscall.parkexten = None
                        reply = '%s,1,,Annule/' % (thiscall.commid)
                        thiscall.uinfo['connection'].send(reply)
                        print 'ParkedCall Annule', thiscall.uinfo['calls']
                        # remove the call from userinfo + list
                return
        

        def agentlogin(self, astid, event):
                agentnum = event.get('Agent')
                agentchannel = event.get('Channel')
                print 'agentlogin', astid, event
                for agname, v in self.ulist[astid].list.iteritems():
                        if 'agentnum' in v and v['agentnum'] == agentnum:
                                v['agentchannel'] = agentchannel
                                # maybe we don't need to send an AppelOpe reply if it has not been explicitly required
                                reply = ',%d,,AppelOpe/' % (1)
                                # reply = ',%d,,AppelOpe/' % (-3)
                                v['connection'].send(reply)
                                for cnum, xcall in v['calls'].iteritems():
                                        if xcall.call.tocall:
                                                print 'an outgoing call is waiting to be sent ...'
                                                time.sleep(1) # otherwise the Agent's channel is not found
                                                xcall.call.tocall = False
                                                self.__outcall(xcall.call)
                return


        def agentlogoff(self, astid, event):
                agentnum = event.get('Agent')
                print 'agentlogoff', astid, event
                for agname, v in self.ulist[astid].list.iteritems():
                        if 'agentchannel' in v and v['agentnum'] == agentnum:
                                print v['agentnum'], 'has left', v['calls']
                                for j, k in v['calls'].iteritems():
                                        reply = '%s,1,,Annule/' % k.call.commid
                                        k.call.uinfo['connection'].send(reply)
                                # if an outgoing call was there, send an (Annule ?)
                                del v['agentchannel']
                return


        def __outcall(self, call):
                # BEGIN OUTGOING CALL (Agent should be logged then)
                retval = 1
                reply = '%s,%d,%s,Appel/' % (call.commid, retval, call.dest)
                call.uinfo['connection'].send(reply)

                print 'OUTCALL'
                r = self.amis[call.astid].aoriginate('Agent', call.agentnum, call.agentname,
                                                     call.dest,
                                                     'Appel %s' % call.dest,
                                                     'default')
                self.__init_taxes(call, call.dest, call.agentnum, call.dest, 'PABX', TRUNKNAME, int(call.agentnum) - STARTAGENTNUM)
        

        def manage_srv2clt(self, userinfo_by_requester, connid, parsedcommand, cfg):
                """
                Defines the actions to be proceeded according to the client's commands.
                """
                cname = parsedcommand.name
                connid_socket = connid[1]
                astid = userinfo_by_requester[0]
                # print 'userinfo_by_requester', userinfo_by_requester

                if len(parsedcommand.args) > 0:
                        agentname = parsedcommand.args[0]
                else:
                        for agname, v in self.ulist[astid].list.iteritems():
                                if 'connection' in v and v['connection'] == connid_socket:
                                        agentname = v['user'][3:]
                userinfo = self.ulist[astid].finduser('sip' + agentname)
                if 'agentnum' in userinfo:
                        agentnum = userinfo['agentnum']
                        agentid = 'Agent/%s' % agentnum
                else:
                        print '--- no agentnum defined in userinfo'
                # print '%s : %s (agent %s) attempts a <%s>' % (astid, agentname, agentnum, cname)
                
                if cname == 'AppelOpe':
                        if len(parsedcommand.args) == 2:
                                # the phonenum comes from the first Init/ command, therefore doesn't need
                                # to be fetched from the 'postes' table
                                phonenum = userinfo_by_requester[4]
                                self.amis[astid].aoriginate_var('sip', phonenum, 'Log %s' % phonenum,
                                                                agentnum, agentname, 'default', 'CB_MES_LOGAGENT', agentname)
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
                                cchan = userinfo['calls'][reference].call.peerchannel

                                columns = ('N', 'AdrNet')
                                cursor_agents = self.conn_agents.cursor()
                                cursor_agents.query('SELECT ${columns} FROM acd WHERE N = %s',
                                                    columns,
                                                    nope)
                                results = cursor_agents.fetchall()
                                if len(results) > 0:
                                        addposte = results[0][1]

                                columns = ('TEL', 'NET')
                                cursor_system = self.conn_system.cursor()
                                cursor_system.query('SELECT ${columns} FROM postes WHERE NET = %s',
                                                    columns,
                                                    addposte)
                                results = cursor_clients.fetchall()
                                if len(results) > 0:
                                        r = self.amis[astid].transfer(cchan, results[0][0], 'default')

                elif cname == 'ForceACD':
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
                                        topark = []
                                        for j, calls in userinfo['calls'].iteritems():
                                                if calls.call.parking is None and calls.call.peerchannel is not None:
                                                        calls.call.parking = j
                                                        topark.append(j)
                                                        print '#', calls.call.peerchannel
                                                        r = self.amis[astid].transfer(calls.call.peerchannel, PARK_EXTEN, 'default')

                                        userinfo['calls'][calltoforce.commid] = ByAgentChannelStatus('i', calltoforce)
                        else:
                                print 'ForceACD - unknown ref =', reference

                elif cname == 'Appel':
                        mreference = parsedcommand.args[1]
                        [dest, idsoc, idcli, idcol] = mreference.split('|')

                        time.sleep(0.2)
                        self.commidcurr += 1
                        comm_id_outgoing = str(self.commidcurr)

                        socname = self.__socname(idsoc)
                        if socname not in self.conn_clients:
                                sqluri_clients = '%s/%s_clients' % (self.uribase, socname)
                                print 'connecting to URI %s - it can take some time' % sqluri_clients
                                self.conn_clients[socname] = anysql.connect_by_uri(sqluri_clients)
                                print 'connecting to URI %s - done' % sqluri_clients

                        ostatus = OutgoingCall.OutgoingCall(comm_id_outgoing, astid, self.conn_clients[socname],
                                                            userinfo, agentnum, agentname, dest,
                                                            idsoc, idcli, idcol)

                        if len(userinfo['calls']) > 0:
                                print 'Appel : there are already ongoing calls'
                                for reference, anycall in userinfo['calls'].iteritems():
                                        print 'ongoing call', reference, anycall.call.parking, anycall.call.peerchannel
                                        if anycall.call.parking is None and anycall.call.peerchannel is not None:
                                                anycall.call.parking = reference
                                                r = self.amis[astid].transfer(anycall.call.peerchannel, PARK_EXTEN, 'default')
                                userinfo['calls'][comm_id_outgoing] = ByAgentChannelStatus('o', ostatus)
                                userinfo['calls'][comm_id_outgoing].call.tocall = True
                        else:
                                if 'agentchannel' in userinfo:
                                        userinfo['calls'][comm_id_outgoing] = ByAgentChannelStatus('o', ostatus)
                                        self.__outcall(ostatus)
                                else:
                                        userinfo['calls'][comm_id_outgoing] = ByAgentChannelStatus('o', ostatus)
                                        userinfo['calls'][comm_id_outgoing].call.tocall = True
                                        phonenum = userinfo_by_requester[4]
                                        self.amis[astid].aoriginate_var('sip', phonenum, 'Log %s' % phonenum,
                                                                        agentnum, agentname, 'default', 'CB_MES_LOGAGENT', agentname)
                                        

                elif cname == 'Raccroche':
                        reference = parsedcommand.args[1]
                        try:
                            if reference in userinfo['calls']:
                                # END OF INCOMING OR OUTGOING CALL
                                anycall = userinfo['calls'][reference]
                                print 'Raccroche', anycall, anycall.call.appelaboute, anycall.call.parking, anycall.call.parkexten, anycall.call.peerchannel
                                if anycall.call.peerchannel is None:
                                        self.amis[astid].hangup(agentid, '')
                                else:
                                        self.amis[astid].hangup(anycall.call.peerchannel, '')
                                try:
                                        if anycall.dir == 'i':
                                                self.__update_stat_acd2(anycall.call)
                                        self.__update_taxes(anycall.call, 'Termine')
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
                        print 'Aboute', agentname, refcomm_out, refcomm_in

                        if refcomm_in in userinfo['calls']:
                                callbackexten = userinfo['calls'][refcomm_in].call.parkexten
                        if refcomm_out in userinfo['calls']:
                                chan = userinfo['calls'][refcomm_in].call.peerchannel
                        if callbackexten is not None:
                                self.amis[astid].transfer(chan, callbackexten, 'default')


                elif cname == 'AppelAboute': # transfer
                        mreference = parsedcommand.args[1]
                        [dest, refcomm, idsoc, idcli, idcol] = mreference.split('|')

                        time.sleep(0.2)
                        self.commidcurr += 1
                        comm_id_outgoing = str(self.commidcurr)

                        if refcomm in userinfo['calls']:
                                incall = userinfo['calls'][refcomm]
                                print "AppelAboute", incall.dir, incall.call.peerchannel
                                incall.call.appelaboute = comm_id_outgoing
                                incall.call.set_timestamp_stat('appelaboute')
                                r = self.amis[astid].transfer(incall.call.peerchannel, dest, 'default')

                                socname = self.__socname(idsoc)
                                if socname not in self.conn_clients:
                                        sqluri_clients = '%s/%s_clients' % (self.uribase, socname)
                                        self.conn_clients[socname] = anysql.connect_by_uri(sqluri_clients)
                                ostatus = OutgoingCall.OutgoingCall(comm_id_outgoing, astid, self.conn_clients[socname],
                                                                    userinfo, agentnum, agentname, dest,
                                                                    idsoc, idcli, idcol)
                                userinfo['calls'][comm_id_outgoing] = ByAgentChannelStatus('o', ostatus)
                                retval = 1
                                reply = '%s,%d,%s,Appel/' % (comm_id_outgoing, retval, dest)
                                connid_socket.send(reply)


                # the park/unpark process takes ~ 6 steps:
                # - send Attente to Park
                # - 'Unlink' is applied to the parked Channel => we must remember it anyway ...
                # - 'ParkedCall' is received => reply is sent
                # - send Reprise
                # - 'UnparkedCall' is received => reply is sent
                # ' 'Link' is applied to the parked Channel
                elif cname == 'Attente':
                        reference = parsedcommand.args[1]
                        if reference in userinfo['calls']:
                                anycall = userinfo['calls'][reference]
                                if anycall.call.parking is None:
                                        if anycall.call.peerchannel is not None:
                                                anycall.call.parking = reference
                                                r = self.amis[astid].transfer(anycall.call.peerchannel, PARK_EXTEN, 'default')
                                                # the reply will be sent when the parkedcall signal is received
                                        else:
                                                print 'Attente : the agent is not in Attente mode yet but peerchannel is not defined'
                                else:
                                        print 'Attente : the agent is already in Attente mode with callref %s' % anycall.call.parking
                        else:
                                print 'Attente : the requested reference %s does not exist' % reference


                elif cname == 'Reprise':
                        reference = parsedcommand.args[1]
                        if reference in userinfo['calls']:
                                anycall = userinfo['calls'][reference]
                                if anycall.call.parking is not None:
                                        callbackexten = anycall.call.parkexten
                                        if callbackexten is not None:
                                                topark = []
                                                for j, calls in userinfo['calls'].iteritems():
                                                        if calls.call.parking is None and calls.call.peerchannel is not None:
                                                                calls.call.parking = j
                                                                topark.append(j)
                                                                print '#', calls.call.peerchannel
                                                                r = self.amis[astid].transfer(calls.call.peerchannel, PARK_EXTEN, 'default')
                                                if len(topark) > 0:
                                                        anycall.call.toretrieve = callbackexten
                                                else:
                                                        r = self.amis[astid].aoriginate('Agent', agentnum, agentname,
                                                                                        callbackexten, 'cid b', 'default')
                                        else:
                                                print 'Reprise : the parkexten number is not defined'
                                else:
                                        print 'Reprise : the agent is not in Attente mode'
                        else:
                                print 'Reprise : the requested reference %s does not exist' % reference


                elif cname == 'Ping':
                        reference = parsedcommand.args[1]
                        reply = ',,,Pong/'
                        connid_socket.send(reply)

                elif cname == 'Sortie':
                        userinfo['cbstatus'] = 'Sortie'

                elif cname == 'Pause':
                        userinfo['cbstatus'] = 'Pause'

                elif cname == 'Prêt' or cname == 'Change' or cname == 'Sonn':
                        if cname == 'Prêt':
                                reference = parsedcommand.args[1]
                                if userinfo['cbstatus'] == 'Pause':
                                        # XXX send current Indispo's to the user
                                        pass
                                userinfo['cbstatus'] = 'Pret' + reference
                        elif cname == 'Change': # we must take into account new parameters
                                reference = '0'
                        elif cname == 'Sonn':
                                userinfo['cbstatus'] = 'Sonn'
                                reference = '1'

                        if reference == '1':
                                if 'sendfiche' in userinfo:
                                        print 'sendfiche / Pret1', userinfo['sendfiche']
                                        [qname, agname] = userinfo['sendfiche']
                                        self.amis[astid].queueadd(qname, agname)
                                        self.amis[astid].queuepause(qname, agname, 'false')
                                        del userinfo['sendfiche']
                                return

                        try:
                                print 'manage : calling __choose()'
                                todo = self.__choose(astid)
                                print 'pretx', todo
                                for opername, mtd in todo.iteritems():
                                        if len(mtd) > 0:
                                                td = mtd[0]
                                                # print 'pretx', td
                                                if td[0] == 'push':
                                                        self.__clear_call_fromqueues(astid, td[1])
                                                        self.__sendfiche(astid, opername, td[1])
                                                        print 'manage : fiche sent to %s' % opername
                                                elif td[0] == 'queue':
                                                        print 'manage : queue for %s (pret)' % opername
                                                        self.__addtoqueue(astid, opername, td[1])
                        except Exception, exc:
                                print '--- exception --- manage_srv2clt (Pret %s) : %s' % (reference, str(exc))

                elif cname == 'Enregistre':
                        reference = parsedcommand.args[1]
                        reply = ',,,%s/' % (cname)
                        connid_socket.send(reply)
                        self.amis[astid].aoriginate_var('Agent', agentnum, agentname,
                                                       '7444', 'Enregistre', 'default',
                                                       'CB_RECORD_FILENAME', reference[0])

                elif cname == 'Alerte':
                        mreference = parsedcommand.args[1]
                        [nalerte, idsoc, idcli, idcol] = mreference.split('|')
                        reply = ',1,,%s/' % (cname)
                        connid_socket.send(reply)

                        sqluri_mvts = '%s/%s_mvts' % (self.uribase, self.__socname(idsoc))
                        conn_mvts = anysql.connect_by_uri(sqluri_mvts)
                        columns = ('N', 'NAlerteStruct', 'NomFichierMessage', 'ListeGroupes', 'interval_suivi')
                        cursor_mvts = conn_mvts.cursor()
                        cursor_mvts.query('SELECT ${columns} FROM alertes WHERE N = %s',
                                          columns,
                                          nalerte)
                        results = cursor_mvts.fetchall()
                        for rr in results:
                                numstruct = rr[1]
                                filename = rr[2] # message file name
                                grouplist = rr[3].split(',')
                                intervsuivi = rr[4] # in seconds

                                print 'Alerte : filename = %s (groups = %s) intsuivi = %s' %(filename, str(grouplist), intervsuivi)
                                socname = self.__socname(idsoc)
                                if socname not in self.conn_clients:
                                        sqluri_clients = '%s/%s_clients' % (self.uribase, socname)
                                        self.conn_clients[socname] = anysql.connect_by_uri(sqluri_clients)
                                columns = ('N', 'Libelle', 'NCol', 'NQuestion',
                                           'Type_Traitement', 'Nom_table_contact', 'Type_Alerte',
                                           'CallingNumber', 'nbTentatives', 'Alerte_Tous', 'Stop_Decroche')
                                cursor_clients = self.conn_clients[socname].cursor()
                                cursor_clients.query('SELECT ${columns} FROM alerte_struct WHERE N = %s',
                                                     columns,
                                                     numstruct)
                                results2 = cursor_clients.fetchall()
                                nquestion = results2[0][3]
                                print '       : struct =', results2[0][1:]

                                columns = ('N', 'Libelle', 'Descriptif', 'Fichier', 'Type_saisie',
                                           'Touches_autorisees', 'Touches_terminales', 'Touche_repete',
                                           'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9',
                                           'AttenteMax', 'TValidate')
                                cursor_system = self.conn_system.cursor()
                                cursor_system.query('SELECT ${columns} FROM questions WHERE N = %s',
                                                    columns,
                                                    nquestion)
                                results3 = cursor_system.fetchall()
                                print '       : questions =', results3[0][1:8]
                                print '       : questions =', results3[0][8:18]
                                print '       : questions =', results3[0][18:20]

                                columns = ('N', 'JOUR', 'DATED', 'DATEF', 'TYPE',
                                           'PlgD', 'PlgF', 'Valeur')
                                cursor_system = self.conn_system.cursor()
                                cursor_system.query('SELECT ${columns} FROM ressource_struct',
                                                    columns)
                                results5 = cursor_system.fetchall()
                                print results5

                                for gl in grouplist:
                                        try:
                                                sqluri_annexe = '%s/%s_annexe' % (self.uribase, self.__socname(idsoc))
                                                conn_annexe = anysql.connect_by_uri(sqluri_annexe)
                                                columns = ('N', 'Groupe', 'nom', 'prenom',
                                                           'tel1', 'tel2', 'tel3', 'tel4',
                                                           'Civilite', 'Fax', 'eMail', 'SMS', 'Code', 'DureeSonnerie')
                                                cursor_annexe = conn_annexe.cursor()
                                                cursor_annexe.query('SELECT * FROM %s' % gl)
                                                results4 = cursor_annexe.fetchall()
                                                for r4 in results4:
                                                        print r4
                                                        # self.amis[astid].aoriginate('local', phonenum, 'dest %s' % phonenum, 'any', 'any name', 'automa')
                                                        
                                        except Exception, exc:
                                                print 'grouplist %s : %s' % (gl, str(exc))
                                        
                        cursor_mvts = conn_mvts.cursor()
                        cursor_mvts.query('DELETE FROM alertes WHERE N = %s' % nalerte)
                        # system / compteurs, suivis, suivisalertes

                else:
                        print 'CallBooster : unmanaged command <%s>' % cname

        def park_srv2clt(self, function, args):
                return ''
        def update_srv2clt(self, phoneinfo):
                return None
        def message_srv2clt(self, sender, message):
                return 'message=%s::%s' %(sender, message)
        def dmessage_srv2clt(self, message):
                return self.message_srv2clt('daemon-announce', message)
        def loginko_srv2clt(self, cfg, errorstring):
                print 'loginko_srv2clt', cfg, errorstring
                return '%s,1,,Init/' % cfg.get('srvnum')
        def loginok_srv2clt(self, userinfo, capa_user, versions, configs, cfg):
                return '%s,1,,Init/' % cfg.get('srvnum')
        def features_srv2clt(self, direction, message):
                return 'features%s=%s' %(direction, message)
        def phones_srv2clt(self, function, args):
                return None


        # updated when it receives a reply from a remote Operat server
        def svreply(self, msg):
                params = msg.split(chr(3))
                cmd = params[0]
                val = params[1]
                if cmd == 'ACDReponse':
                        print 'ACDReponse', msg
                        sdanum = params[5]
                        commid = params[6]
                        if sdanum in incoming_calls:
                                for chan, ic in incoming_calls[sdanum].iteritems():
                                        if ic.commid == commid :
                                                status = None
                                                if val == 'Absent':
                                                        pass
                                                elif val == 'Trouvé':
                                                        # FicheD
                                                        if ic.dialplan['callerid'] == 1:
                                                                cnum = ic.cidnum
                                                        else:
                                                                cnum = ''
                                                        reply = '%s,%s-%s-%s-%s,%s,FicheD/' % (ic.commid,
                                                                                               ic.sdanum, params[2], params[3], params[4],
                                                                                               cnum)
                                                        self.pending_sv_fiches.append(reply)
                                                        self.list_svirt.append(params)
##                                                        self.amis[astid].queueadd(ic.qname, agname)
##                                                        self.amis[astid].queuepause(ic.qname, agname, 'false')
                                                elif val == 'Attente':
                                                        self.list_svirt.append(params)
                                                print 'OperatSock : received reply for :', sdanum, commid, ic, ic.commid, val
                return None


        # checking if any news from pending requests
        def svcheck(self):
                for areq in self.list_svirt:
                        print 'areq =', areq
                        req = 'ACDCheckRequest' + chr(2) + chr(2).join(areq[:6]) + chr(3)
                        self.soperat_socket.send(req)
                return


        def __init_taxes(self, call, numbertobill, fromN, toN, fromS, toS, NOpe):
                try:
                        [juridict, impulsion] = self.__gettaxes(numbertobill)
                        call.settaxes(impulsion)
                        datetime = time.strftime(DATETIMEFMT)
                        cursor_system = self.conn_system.cursor()
                        cursor_system.query('INSERT INTO taxes VALUES (0, %s, 0, %s, %d, %d, %s, %s, %s, %s, %s, %d, %s, %s, %s, %s, %s, %s, %s)'
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
                        cursor_system.query('SELECT LAST_INSERT_ID()') # last_insert_id
                        results = cursor_system.fetchall()
                        call.insert_taxes_id = results[0][0]

                except Exception, exc:
                        print '--- exception --- (__init_taxes) :', exc


        def __update_taxes(self, cs, state):
                """
                Updates the 'taxes' table of the 'system' base.
                This is for the outgoing calls as well as incoming calls.
                """
                now_t_time = time.localtime()
                duree = time.mktime(now_t_time) - time.mktime(cs.ctime)
                duree_int = int(duree)
                [tpc, dpc, dlt] = cs.taxes
                if duree_int >= dpc and dlt > 0:
                        ntaxes = tpc + 1 + (duree_int - dpc) / dlt
                else:
                        ntaxes = tpc

                cs.set_timestamp_tax('END')
                dureesonnerie = 0
                t1 = -1
                t2 = -1
                t3 = -1
                for jj, k in cs.ttimes.iteritems():
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

                print '__update_taxes : commid = %s, arg = %s, state = %s, ' \
                      'durees = %f, %d, %d, taxes = %d' \
                      %(cs.commid, str(cs.taxes), state, duree, duree_int, dureesonnerie, ntaxes)

                cursor_system = self.conn_system.cursor()
                cursor_system.query('UPDATE taxes SET Duree = %d, DureeSonnerie = %d, Etat = %s, nbTaxes = %d WHERE RefFB = %s'
                                    % (duree, dureesonnerie, '"%s"' % state, ntaxes, cs.commid))
                # cursor_system.fetchall()
                self.conn_system.commit()


        def __update_stat_acd(self, state,
                              tt_raf, tt_asd, tt_snd, tt_sfa, tt_sop,
                              tt_tel, tt_fic, tt_bas, tt_mes, tt_rep):
                
                ntime = int(time.time() / TSLOTTIME) * TSLOTTIME
                datetime = time.strftime(DATETIMEFMT, time.localtime(ntime))
                period = 'JOUR'
                print '__update_stat_acd : datetime = %s (period = %s)' %(datetime, period)

                try:
                        columns = ('DATE', 'Periode', 'SDA_NC', 'SDA_NV', 'SDA_HDV', 'SDA_V', 'TTraitement',
                                   'TT_RAF', 'TT_ASD', 'TT_SND', 'TT_SFA', 'TT_SOP',
                                   'TT_TEL', 'TT_FIC', 'TT_BAS', 'TT_MES', 'TT_REP',
                                   'NSOC' )
                        cursor_system = self.conn_system.cursor()
                        cursor_system.query('SELECT ${columns} FROM stat_acd WHERE DATE = %s',
                                            columns,
                                            datetime)
                        results = cursor_system.fetchall()
                        if len(results) > 0:
                                r = results[0]
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
                        
                        if len(results) > 0:
                                cursor_system.query("UPDATE stat_acd SET SDA_NC = %d, SDA_NV = %d, SDA_HDV = %d, SDA_V = %d,"
                                                    " TT_RAF = %d, TT_ASD = %d, TT_SND = %d, TT_SFA = %d, TT_SOP = %d,"
                                                    " TT_TEL = %d, TT_FIC = %d, TT_BAS = %d, TT_MES = %d, TT_REP = %d"
                                                    " WHERE DATE = '%s'"
                                                    % (nnc, nnv, nhdv, nv,
                                                       ntt_raf, ntt_asd, ntt_snd, ntt_sfa, ntt_sop,
                                                       ntt_tel, ntt_fic, ntt_bas, ntt_mes, ntt_rep,
                                                       datetime))
                        else:
                                cursor_system.query('INSERT INTO stat_acd VALUES'
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
                        self.conn_system.commit()
                except Exception, exc:
                        print 'exception in __update_stat_acd :', exc
                return


        def __update_stat_acd2(self, incall):
                """
                Updates the 'stat_acd2' table of the 'system' base.
                This is only for incoming calls.
                """
                if incall.statdone:
                        print '__update_stat_acd2 : STAT ALREADY DONE for', incall.commid
                        return
                incall.statdone = True

                print '__update_stat_acd2 (END OF INCOMING CALL)', incall.commid
                now_t_time = time.localtime()
                now_f_time = time.strftime(DATETIMEFMT, now_t_time)
                datetime = time.strftime(DATETIMEFMT, incall.ctime)

                [tt_raf, tt_asd, tt_snd, tt_sfa, tt_sop,
                 tt_tel, tt_fic, tt_bas, tt_mes, tt_rep] =  [0, 0, 0, 0, 0,
                                                             0, 0, 0, 0, 0]
                state = incall.statacd2_state
                if incall.statacd2_tt == 'TT_RAF':
                        tt_raf = 1
                elif incall.statacd2_tt == 'TT_ASD':
                        tt_asd = 1
                elif incall.statacd2_tt == 'TT_SND':
                        tt_snd = 1
                elif incall.statacd2_tt == 'TT_SFA':
                        tt_sfa = 1
                elif incall.statacd2_tt == 'TT_SOP':
                        tt_sop = 1
                elif incall.statacd2_tt == 'TT_TEL':
                        tt_tel = 1
                elif incall.statacd2_tt == 'TT_FIC':
                        tt_fic = 1
                elif incall.statacd2_tt == 'TT_BAS':
                        tt_bas = 1
                elif incall.statacd2_tt == 'TT_MES':
                        tt_mes = 1
                elif incall.statacd2_tt == 'TT_REP':
                        tt_rep = 1

                incall.set_timestamp_stat('END')
                sortedtimes = incall.stimes.keys()
                sortedtimes.sort()
                nks = len(sortedtimes)
                bil = {}
                isaa = False
                print '__update_stat_acd2 : history =',
                for t in xrange(nks - 1):
                        act = incall.stimes[sortedtimes[t]]
                        if act == 'appelaboute':
                                isaa = True
                        if act == 'link' and isaa:
                                act = 'linkaboute'
                        dt = sortedtimes[t+1] - sortedtimes[t]
                        print '(', dt, act, ')',
                        if act not in bil:
                                bil[act] = 0
                        bil[act] += dt
                print
                print '__update_stat_acd2 : history by action =',
                for act in bil:
                        print '(', act, bil[act], ')',
                print
                dtime = sortedtimes[nks - 1] - sortedtimes[0]
                nslots = int(sortedtimes[nks - 1]/TSLOTTIME) - int(sortedtimes[0]/TSLOTTIME)
                print '__update_stat_acd2 : history - total time =', dtime, nslots + 1
                for g in xrange(nslots):
                        print (g + 1 + int(sortedtimes[0]/TSLOTTIME)) * TSLOTTIME
                if incall.uinfo is not None:
                        print '__update_stat_acd2 : uinfo calls', incall.uinfo['calls']
                        opername = incall.uinfo['user'][3:]
                else:
                        opername = ''
                
                dec = 0

                [tacd, tope, tatt, tattabo, tabo,
                 ttel, trep, tmes, tsec,    tdec] = [0, 0, 0, 0, 0,
                                                     0, 0, 0, 0, 0]
                # XXX : tsec
                if 'parked' in bil:
                        tatt = int(bil['parked'])
                if 'appelaboute' in bil:
                        tattabo = int(bil['appelaboute'])
                if 'linkaboute' in bil:
                        tabo = int(bil['linkaboute'])
                if 'sec' in bil:
                        tacd = int(bil['sec'])
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

                try:
                        cursor_system = self.conn_system.cursor()
                        cursor_system.query('INSERT INTO stat_acd2 VALUES'
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
                                               tt_raf, tt_asd, tt_snd, tt_sfa, tt_sop,
                                               tt_tel, tt_fic, tt_bas, tt_mes, tt_rep,
                                               incall.nsoc,
                                               incall.ncli,
                                               incall.ncol,
                                               tacd, tope, tatt, tattabo, tabo,
                                               ttel, trep, tmes, tsec,    tdec,
                                               dec,
                                               '"%s"' % opername,
                                               '"%s"' % incall.socname,
                                               incall.insert_taxes_id))

                        self.__update_stat_acd(state,
                                               tt_raf, tt_asd, tt_snd, tt_sfa, tt_sop,
                                               tt_tel, tt_fic, tt_bas, tt_mes, tt_rep)
                except Exception, exc:
                        print 'exception in __update_stat_acd2 :', exc

                return


        def __gettaxes(self, num):
                # num = '003%d' % random.randint(0, 999999999) # fake, for testing purposes ...
                juridict = self.__juridictions(num)
                if juridict is not None:
                        impulsion = self.__impulsion(juridict)
                else:
                        impulsion = None
                        juridict = '0'

                print '___ TAXES :', num, juridict, impulsion

                if impulsion is None:
                        impulsion = [0, 0, 30]
                return [juridict, impulsion]


        def __juridictions(self, num):
                columns = ('Numero', 'Juridiction', 'Type_Num', 'Description', 'Local')
                cursor_system = self.conn_system.cursor()
                cursor_system.query("SELECT ${columns} FROM juridict",
                                    columns)
                results = cursor_system.fetchall()
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


        def __impulsion(self, jur):
                columns = ('Juridiction', 'Info', 'NbTaxePC', 'DureePC', 'DureeTaxe')
                cursor_system = self.conn_system.cursor()
                cursor_system.query('SELECT ${columns} FROM impulsion WHERE Juridiction = %s',
                                    columns,
                                    jur)
                results = cursor_system.fetchall()
                if len(results) > 0:
                        r = results[0]
                        imp = [r[2], r[3], r[4]]
                else:
                        imp = None
                return imp


        def __listqueues(self):
                l = []
                for sdanum, inc in incoming_calls.iteritems():
                        for chan, icall in inc.iteritems():
                                l.append(icall.queuename)
                return l


        def __choosequeuenum(self):
                lst = self.__listqueues()
                for qnum in xrange(10):
                        qname = 'qcb_%05d' % qnum
                        if qname not in lst:
                                return qnum
                return None


        def __incallref_from_channel(self, chan):
                thiscall = None
                for sdanum, inc in incoming_calls.iteritems():
                        if chan in inc:
                                thiscall = inc[chan]
                                break
                return thiscall


        def __choose(self, astid):
                """
                After a login, a Change, a Pret's, a logoff, a new incoming/outgoing call
                """
                # choose, according to other incoming calls & logged in/out, if one has to wait or push
                byprio = []
                for u in xrange(6):
                        byprio.append([])
                for sdanum, inc in incoming_calls.iteritems():
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
                for u in xrange(6):
                        if len(byprio[u]) > 0:
                                print '__choose : prio %d :' % u
                                for incall in byprio[u]:
                                        # choose the operator or the list of queues for this incoming call
                                        topush = {}
                                        tochoose = {}
                                        time.sleep(0.1) # wait in order for the database value to be compliant ...
                                        for opername in incall.list_operators:
                                                if opername not in todo_by_oper:
                                                        todo_by_oper[opername] = []
                                                opstatus = incall.check_operator_status(opername)
                                                print '__choose : <%s> (%s)' % (opername, opstatus)
                                                if opstatus is not None:
                                                        userinfo = self.ulist[astid].finduser('sip' + opername)
                                                        if 'connection' in userinfo:
                                                                userqueuesize = len(userinfo['queuelist'])
                                                                print '__choose :', opername, userinfo, opstatus, userqueuesize
                                                                [status, dummy, level, prio] = opstatus
                                                                # print '__choose : incall : %s / opername : %s %s' % (incall.cidnum, opername, status)
                                                                if status == 'Pret0':
                                                                        if len(todo_by_oper[opername]) == 0 and userqueuesize + 1 >= int(level):
                                                                                topush[opername] = int(prio)
                                                                        else:
                                                                                tochoose[opername] = int(prio)
                                                                elif status in ['Pret1', 'Pause', 'Sonn']:
                                                                        tochoose[opername] = int(prio)

                                        print '__choose : callid = %s :' % incall.commid, topush, tochoose
                                        if len(topush) == 0: # noone available for this call yet
                                                for opername in tochoose:
                                                        todo_by_oper[opername].append(['queue', incall])
                                        else:
                                                if len(topush) == 1: # somebody available
                                                        opername = topush.keys()[0]
                                                else: # choose among the pushers, according to prio
                                                        maxp = 0
                                                        for opn, p in topush.iteritems():
                                                                if p > maxp:
                                                                        opername = opn
                                                                        maxp = p
                                                todo_by_oper[opername].append(['push', incall])
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
                        queuenum = self.__choosequeuenum()
                        print ' NCOMING CALL ## building an IncomingCall structure ##'
                        thiscall = IncomingCall.IncomingCall(self.conn_agents, self.conn_system,
                                                             cidnum, sdanum, queuenum, self.soperat_socket, self.soperat_port)

                        if thiscall.statacd2_state != 'NC':
                                socname = thiscall.socname
                                if socname not in self.conn_clients:
                                        sqluri_clients = '%s/%s_clients' % (self.uribase, socname)
                                        print 'connecting to URI %s - it can take some time' % sqluri_clients
                                        self.conn_clients[socname] = anysql.connect_by_uri(sqluri_clients)
                                        print 'connecting to URI %s - done' % sqluri_clients

                                thiscall.setclicolnames(self.conn_clients[socname])

                        self.__init_taxes(thiscall, cidnum, cidnum, sdanum, TRUNKNAME, 'PABX', 0)

                        if thiscall.statacd2_state == 'V':
                                print ' NCOMING CALL ## calling get_sda_profiles ##'
                                if sdanum not in incoming_calls:
                                        incoming_calls[sdanum] = {}
                                self.__clear_call_fromqueues(astid, thiscall)
                                ret = thiscall.get_sda_profiles(self.conn_clients[socname], len(incoming_calls[sdanum]))
                                if ret == True:
                                        incoming_calls[sdanum][inchannel] = thiscall
                                        print ' NCOMING CALL : list of used SDA :', incoming_calls
                                else:
                                        self.__update_taxes(thiscall, 'Termine')
                                        self.__update_stat_acd2(thiscall)
                                        thiscall = None
                        else:
                                self.__update_taxes(thiscall, 'Termine')
                                self.__update_stat_acd2(thiscall)
                                thiscall = None
                else:
                        if sdanum in incoming_calls and inchannel in incoming_calls[sdanum]:
                                thiscall = incoming_calls[sdanum][inchannel]

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
                                # maybe we had been in 'sec' mode previously, so that we shouldn't wait anymore
                                thiscall.waiting = False
                                # send CLR-ACD to the users who had received an Indispo as concerning this call
                                self.__clear_call_fromqueues(astid, thiscall)
##                        elif action == 'sv':
##                                self.__addtoqueue(astid, opername, thiscall)
                        elif action == 'sec':
                                thiscall.waiting = True
                                print 'elect : calling __choose()'
                                todo = self.__choose(astid)
                                # once all the queues have been spanned, send the push / queues where needed
                                for opername, couplelist in todo.iteritems():
                                        for td in couplelist:
                                                print 'elect : sec / %s / %s / %s' % (opername, td[0], td[1].sdanum)
                                                if td[0] == 'push':
                                                        if thiscall == td[1]:
                                                                self.__clear_call_fromqueues(astid, td[1])
                                                                self.__sendfiche(astid, opername, td[1])
                                                                delay = 10
                                                                argument = None
                                                        else:
                                                                pass
                                                elif td[0] == 'queue':
                                                        self.__addtoqueue(astid, opername, td[1])
                        elif action == 'exit':
                                self.__update_taxes(thiscall, 'Termine')
                                self.__update_stat_acd2(thiscall)
                        else:
                                print '### action is <%s> which I don t know, exiting anyway' % action
                                self.__update_taxes(thiscall, 'Termine')
                                self.__update_stat_acd2(thiscall)

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


xivo_commandsets.CommandClasses['callbooster'] = CallBoosterCommand

