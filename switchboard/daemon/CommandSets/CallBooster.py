
__version__   = '$Revision$ $Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

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

STARTAGENTNUM = 6100
DATEFMT = '%Y-%m-%d'
DATETIMEFMT = DATEFMT + ' %H:%M:%S'
PARK_EXTEN = '700'
TRUNKNAME = 'FT'
TSLOTTIME = 1800

incoming_calls = {}
outgoing_calls = {}

def varlog(syslogprio, string):
        if syslogprio <= SYSLOG_NOTICE:
                syslogf(syslogprio, 'xivo_fb : ' + string)
        return 0

def log_debug(syslogprio, string):
        if syslogprio <= SYSLOG_INFO:
                print '#debug# %s' % string
        return varlog(syslogprio, string)


# update features set var_val = '36000' where id = 3;
# |  3 |          0 |          0 |         0 | features.conf | general    | parkingtime          | 36000       |
# delete from musiconhold where category = "1/3/0";

class CallBoosterCommand(BaseCommand):
        """
        CallBoosterCommand class.
        Defines the behaviour of commands.
        """
        def __init__(self, ulist, amis, operatsocket, operatport, operatini, queued_threads_pipe):
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
                self.pending_sv_fiches = {}
                self.tqueue = Queue.Queue()
                self.queued_threads_pipe = queued_threads_pipe
                self.alerts = {}


        def __sendfiche_a(self, userinfo, incall):
                userinfo['calls'][incall.commid] = incall
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
                        log_debug(SYSLOG_INFO, 'CallBooster : sent <%s>' % reply)
                else:
                        log_debug(SYSLOG_INFO, 'CallBooster : could not send <%s>' % reply)
                return


        # the call comes here when the AGI has directly found a peer
        def __sendfiche(self, astid, dest, incall):
                userinfo = self.ulist[astid].finduser(dest)
                log_debug(SYSLOG_INFO, '__sendfiche : userinfo = %s' % str(userinfo))
                if userinfo is not None:
                        agentnum = userinfo['agentnum']
                        if 'agentchannel' in userinfo:
                                print 'sendfiche, the agent is online :', userinfo['agentchannel']
                        else:
                                phonenum = userinfo['phonenum']
                                agentname = userinfo['user']
                                log_debug(SYSLOG_INFO, 'sendfiche, the agent is not online ... we re going to call him : %s (%s)' % (phonenum, str(userinfo)))
                                userinfo['startcontact'] = time.time()
                                userinfo['startcontactth'] = 10
                                self.amis[astid].aoriginate_var('sip', phonenum, 'Log %s' % phonenum,
                                                                agentnum, agentname, 'default', {'CB_MES_LOGAGENT' : agentname}, 100)
                        self.__sendfiche_a(userinfo, incall)
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
                                opername = userinfo['user']
                                if opername in incall.agentlist:
                                        incall.agentlist.remove(opername)
                                else:
                                        print opername, 'not in agentlist', incall.agentlist
                                self.amis[astid].queueremove(incall.queuename, 'Agent/%s' % agentnum)
                                reply = '%s,%s,,CLR-ACD/' % (incall.commid, incall.sdanum)
                                if 'connection' in userinfo:
                                        userinfo['connection'].send(reply)
                                        log_debug(SYSLOG_INFO, 'CallBooster : sent <%s>' % reply)
                                else:
                                        log_debug(SYSLOG_INFO, 'CallBooster : could not send <%s>' % reply)
                return


        def __addtoqueue(self, astid, dest, incall):
                print '__addtoqueue', dest, incall.commid, incall.sdanum
                self.amis[astid].queueadd(incall.queuename, 'Agent/6999')
                self.amis[astid].queuepause(incall.queuename, 'Agent/6999', 'false')
                userinfo = self.ulist[astid].finduser(dest)
                if userinfo is not None:
                        if incall.queuename not in userinfo['queuelist']:
                                agentnum = userinfo['agentnum']
                                # this is the Indispo list
                                userinfo['queuelist'][incall.queuename] = incall
                                incall.agentlist.append(dest)
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


        def __local_nsoc(self, idsoc):
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
                                # in order to avoid tricky u'sdlkfs'
                                oname = opername.__repr__()[2:-1].replace('\\xe9', 'é').replace('\\xea', 'è')
                                pname = passname.__repr__()[2:-1].replace('\\xe9', 'é').replace('\\xea', 'è')

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
                """In this CB case, defines the path to the Operat MySQL database."""
                sqluri = '%s?charset=%s' % (uri_operat, 'latin1')
                self.conn_operat   = anysql.connect_by_uri(sqluri)
                self.cursor_operat = self.conn_operat.cursor()
                self.conn_xivo     = anysql.connect_by_uri(uri_xivo)
                self.cursor_xivo   = self.conn_xivo.cursor()


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

                                # XXX : sip/phonenum => IAX2/trunkname/number
                                self.amis[astid].aoriginate_var('sip', phonenum, 'Log %s' % phonenum,
                                                                agentnum, 'agentname', 'default', {'CB_MES_LOGAGENT' : 'agentname'}, 100)
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
                return ['astid', 'proto', 'ident', 'userid', 'version', 'computername', 'phonenum', 'computeripref', 'srvnum']


        def connected_srv2clt(self, conn, id):
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
                ch1 = event.get('Channel1').split('/')
                ch2 = event.get('Channel2').split('/')
                print 'LINK', ch1, ch2, time.time()
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
                                                        if anycall.peerchannel is None:
                                                                # New Call
                                                                anycall.peerchannel = peer
                                                                print 'LINK (new call)', callnum, anycall.dir, anycall.peerchannel, peer
                                                                anycall.set_timestamp_stat('link')
                                                                if anycall.dir == 'o':
                                                                        outgoing_calls[peer] = anycall
                                                                        # self.__update_taxes(anycall.call, 'Decroche')
                                                                        reply = '%s,1,%s,Decroche/' % (callnum, callerid)
                                                                        connid_socket.send(reply)
                                                                anycall.set_timestamp_tax('link')
                                                        elif anycall.peerchannel == peer:
                                                                if anycall.parking is not None:
                                                                        print 'LINK but UNPARKING :', callnum, anycall.dir, anycall.parking, anycall.peerchannel, peer
                                                                        anycall.parking = None
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
                                print 'LINK without Agent (1)', ic1.sdanum, ic1.commid, ic1.appelaboute, ic1.aboute
                                if ic1.appelaboute is not None:
                                        connid_socket = ic1.uinfo['connection']
                                        reply = '%s,%d,,Raccroche/' % (ic1.commid, 1)
                                        connid_socket.send(reply)
                                        reply = '%s,%d,,Raccroche/' % (ic1.appelaboute, 1)
                                        connid_socket.send(reply)
                                ic1.set_timestamp_stat('link')
                        if ic2 is not None:
                                print 'LINK without Agent (2)', ic2.sdanum, ic2.commid, ic2.appelaboute, ic2.aboute
                                if ic2.aboute is not None:
                                        connid_socket = ic2.uinfo['connection']
                                        reply = '%s,%d,,Raccroche/' % (ic2.commid, 1)
                                        connid_socket.send(reply)
                                        reply = '%s,%d,,Raccroche/' % (ic2.aboute, 1)
                                        connid_socket.send(reply)
                                ic2.set_timestamp_stat('link')
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
                                                        print 'UNLINK', callnum, peer, anycall.peerchannel,
                                                        if anycall.peerchannel is None:
                                                                print 'no action since peerchannel is None'
                                                        elif anycall.peerchannel == peer:
                                                                if anycall.parking == callnum:
                                                                        print 'but PARKING :', anycall.parking
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
                                                                print 'unlink INCOMING CALL => __update_stat_acd2', peer
                                                                self.__update_stat_acd2(calltounlink)
                                                        self.__update_taxes(calltounlink, 'Termine')

                                                        # the link had been established => send Annule
                                                        connid_socket.send('%s,1,,Annule/' % callnum)

                                                        # remove call from incoming or outgoing list
                                                        userinfo['calls'].pop(callnum)
                        finally:
                                self.ulist[astid].release()
                else:
                        ic1 = self.__incallref_from_channel(event.get('Channel1'))
                        ic2 = self.__incallref_from_channel(event.get('Channel2'))
                        if ic1 is not None:
                                log_debug(SYSLOG_INFO, 'UNLINK without Agent (1) sda=%s commid=%s' % (ic1.sdanum, ic1.commid))
                                ic1.set_timestamp_stat('unlink')
                        if ic2 is not None:
                                log_debug(SYSLOG_INFO, 'UNLINK without Agent (2) sda=%s commid=%s' % (ic2.sdanum, ic2.commid))
                                ic2.set_timestamp_stat('unlink')
                return


        def __originatefailure(self, astid, event, kind):
                print '__originatefailure (%s) :' % kind, astid, event
                # __originatefailure clg {'Uniqueid': '<null>', 'Exten': '6104', 'Reason': '0', 'Context': 'default', 'Privilege': 'call,all', 'Event': 'AOriginateFailure', 'Channel': 'sip/101'} a
                return


        def aoriginatefailure(self, astid, event):
                self.__originatefailure(astid, event, 'a')
                return


        def originatefailure(self, astid, event):
                self.__originatefailure(astid, event, '')
                return


        def __originatesuccess(self, astid, event, kind):
                print '__originatesuccess (%s) :' % kind, astid, event
                # __originatesuccess clg {'Uniqueid': '<null>', 'Exten': '6104', 'Reason': '0', 'Context': 'default', 'Privilege': 'call,all', 'Event': 'AOriginateSuccess', 'Channel': 'sip/101'} a
                return


        def aoriginatesuccess(self, astid, event):
                self.__originatesuccess(astid, event, 'a')
                return


        def originatesuccess(self, astid, event):
                self.__originatesuccess(astid, event, '')
                return


        def messagewaiting(self, astid, event):
                print 'messagewaiting', astid, event
                datetime = time.strftime('%Y-%m-%d_%H-%M-%S')
                sdanum = event.get('Mailbox').split('@')[0]
                dirname ='/var/spool/asterisk/voicemail/default/%s/INBOX' % sdanum

                nsoc = 0
                ncli = 0
                ncol = 0
                if sdanum in incoming_calls:
                        lst = incoming_calls[sdanum]
                        for chan, ic in lst.iteritems():
                                nsoc = ic.nsoc
                                ncli = ic.ncli
                                ncol = ic.ncol

                n = 0
                # {'Old': '0', 'Mailbox': '101@default', 'Waiting': '1', 'Privilege': 'call,all', 'New': '1', 'Event': 'MessageWaiting'}
                self.cursor_operat.query('USE system')
                for msg in os.listdir(dirname):
                        fullpath = '/'.join([dirname, msg])
                        if msg.find('.wav') > 0:
                                print 'full path is', fullpath, 'renaming'
                                newpath = '/var/spool/asterisk/voicemail/%s.%s.%02d.wav' % (sdanum, datetime, n)
                                n += 1
                                os.rename(fullpath, newpath)
                                self.cursor_operat.query("INSERT INTO suivis (`NOM`,`NSOC`,`NCLI`,`NCOL`,`NSTRUCT`,`TypeT`,`DateP`,`NAPL`,`ETAT`,`STATUT`) "
                                                         "VALUES ('%s', %d, %d, %d, %d, '%s', '%s', '%s', '%s', '%s')"
                                                         % ('collab', nsoc, ncli, ncol, 2, 'AUDIO',
                                                            datetime, 'ACD', 'ATT',
                                                            newpath))
                        else:
                                print 'full path is', fullpath, 'deleting'
                                os.remove(fullpath)


                # ','2008-01-06_09;27;09,060;;.WAV\\Déclenché en automatique');
                # NOM : Nom du collaborateur (ou du client s'il s'agit d'une SDA où NCOL=0)
                # NSOC : Numero de l'adhérent
                # NCLI : Numero du Client
                # NCOL : Numero du Collaborateur
                # NSTRUCT : Indice de l'enregistrement de la table `messagerie` utilisé pour cette messagerie
                # DateP : Date et Heure d'envoi programmé
                # STATUT : Doit contenir le nom du fichier message à envoyer plus la chaine '\Déclenché en automatique'


        def newcallerid(self, astid, event):
                agentnum = event.get('CallerID')
                channel = event.get('Channel')
                for agname, uinfo in self.ulist[astid].list.iteritems():
                        if 'agentnum' in uinfo and uinfo['agentnum'] == agentnum:
                                uinfo['chancon'] = channel
                return


        def queuememberstatus(self, astid, event):
                agentid = event.get('Location')
                status = event.get('Status')
                queue = event.get('Queue')
                for agname, uinfo in self.ulist[astid].list.iteritems():
                        if 'agentnum' in uinfo and uinfo['agentnum'] == agentid.split('/')[1]:
                                if 'startcontact' in uinfo:
                                        dtime = time.time() - int(uinfo['startcontact'])
                                        thresh = uinfo['startcontactth']
                                        print 'queuememberstatus', agentid, status, queue, dtime, uinfo
                                        if status == '5' and dtime > thresh:
                                                if thresh == 10:
                                                        code = -2
                                                        uinfo['startcontactth'] = 20
                                                elif thresh == 20:
                                                        code = -2
                                                        uinfo['startcontactth'] = 30
                                                else:
                                                        code = -3
                                                        del uinfo['startcontact']
                                                        del uinfo['startcontactth']
                                                        if 'chancon' in uinfo:
                                                                self.amis[astid].hangup(uinfo['chancon'], '')
                                                                del uinfo['chancon']
                                                reply = ',%d,,AppelOpe/' % code
                                                uinfo['connection'].send(reply)

        def dial(self, astid, event):
                #print 'DIAL', event
                return


        def hangup(self, astid, event):
                chan = event.get('Channel')
                thiscall = self.__incallref_from_channel(chan)
                if thiscall is not None:
                        print 'HANGUP => __update_stat_acd2', chan, thiscall.queuename, thiscall.uinfo
                        if thiscall.uinfo is not None:
                                if 'startcontact' in thiscall.uinfo and 'chancon' in thiscall.uinfo:
                                        reply = ',-1,,AppelOpe/'
                                        thiscall.uinfo['connection'].send(reply)
                                        self.amis[astid].hangup(thiscall.uinfo['chancon'], '')
                        self.__update_taxes(thiscall, 'Termine')
                        self.__update_stat_acd2(thiscall)
                        self.__clear_call_fromqueues(astid, thiscall)
                        # removes the call from incoming call list
                        incoming_calls[thiscall.sdanum].pop(chan)
                else:
                        print 'HANGUP (not incoming call)', event
                return


        def join(self, astid, event):
                chan  = event.get('Channel')
                clid  = event.get('CallerID')
                qname = event.get('Queue')
                count = int(event.get('Count'))
                log_debug(SYSLOG_INFO, 'AMI:Join: %s queue=%s %s count=%s %s' % (astid, qname, chan, count, clid))
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
                                        if usercall.tocall:
                                                print 'ParkedCall Attente', commid, usercall.parking, usercall.parkexten, usercall.appelaboute, usercall.tocall
                                                usercall.tocall = False
                                                self.__outcall(usercall)
                                        elif usercall.forceacd is not None:
                                                [uinfo, qname, agchan] = usercall.forceacd
                                                print 'ok for forceacd ...', qname, agchan, uinfo
                                                uinfo['sendfiche'] = [qname, agchan]

                                                if usercall.dialplan['callerid'] == 1:
                                                        cnum = usercall.cidnum
                                                else:
                                                        cnum = ''
                                                reply = '%s,%s,%s,Fiche/' % (usercall.commid, usercall.sdanum, usercall.cidnum)
                                                uinfo['connection'].send(reply)
                                                
                                                reply = '%s,%s,,CLR-ACD/' % (usercall.commid, usercall.sdanum)
                                                uinfo['connection'].send(reply)
                                                usercall.forceacd = None
                                        elif usercall.toretrieve is not None:
                                                print 'usercall.toretrieve', usercall.toretrieve
                                                r = self.amis[astid].aoriginate('Agent', usercall.uinfo['agentnum'], 'agentname',
                                                                                usercall.toretrieve, 'cid b', 'default')
                                                usercall.toretrieve = None
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


        def agent_was_logged_in(self, astid, event):
                agentnum = event.get('Agent')
                agentchannel = event.get('LoggedInChan')
                print 'agent_was_logged_in', astid, event
                for agname, v in self.ulist[astid].list.iteritems():
                        if 'agentnum' in v and v['agentnum'] == agentnum:
                                v['agentchannel'] = agentchannel


        def agentlogin(self, astid, event):
                agentnum = event.get('Agent')
                agentchannel = event.get('Channel')
                print 'agentlogin', astid, event
                for agname, v in self.ulist[astid].list.iteritems():
                        if 'agentnum' in v and v['agentnum'] == agentnum:
                                v['agentchannel'] = agentchannel
                                # maybe we don't need to send an AppelOpe reply if it has not been explicitly required
                                reply = ',%d,,AppelOpe/' % (1)
                                if 'startcontact' in v:
                                        del v['startcontact']
                                        del v['startcontactth']
                                # reply = ',%d,,AppelOpe/' % (-3)
                                if 'connection' in v:
                                        v['connection'].send(reply)
                                for cnum, xcall in v['calls'].iteritems():
                                        if xcall.tocall:
                                                log_debug(SYSLOG_INFO, 'an outgoing call is waiting to be sent ...')
                                                time.sleep(1) # otherwise the Agent's channel is not found
                                                xcall.tocall = False
                                                self.__outcall(xcall)
                return


        def agentlogoff(self, astid, event):
                agentnum = event.get('Agent')
                print 'agentlogoff', astid, event
                for agname, v in self.ulist[astid].list.iteritems():
                        if 'agentchannel' in v and v['agentnum'] == agentnum:
                                print v['agentnum'], 'has left', v['calls']
                                for j, k in v['calls'].iteritems():
                                        reply = '%s,1,,Annule/' % k.commid
                                        k.uinfo['connection'].send(reply)
                                # if an outgoing call was there, send an (Annule ?)
                                del v['agentchannel']
                return


        def __walkdir(self, args, dirname, filenames):
                if dirname[-5:] == '/Sons':
                        for filename in filenames:
                                if filename.find('M018.') == 0:
                                        self.listreps.append(dirname)

        def pre_reload(self):
                print 'pre_reload'
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
                                print '  defined :  %s  =>  %s' %(ag[1], agnum)
                        else:
                                print 'undefined :  %s  =>  %s' %(ag[1], agnum)
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
                return


        def pre_moh_reload(self):
                print 'pre_moh_reload'
                REPFILES = '/usr/share/asterisk/sounds/callbooster'
                MOHFILES = '/usr/share/asterisk/moh/callbooster'
                columns = ('category', 'var_name', 'var_val')
                self.listreps = []
                os.path.walk(REPFILES, self.__walkdir, None)
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
                return


        def userevent(self, astid, event):
                print 'userevent :', astid, event
                appdatas = event.get('AppData').split('-', 1)
                dtmfreply = appdatas[0]
                rid = appdatas[1]
                if rid in self.alerts:
                        print self.alerts[rid]
                        del self.alerts[rid]
                # for rid, al in self.alerts.iteritems():
                # issue one or more new calls ?
                return

        # self.cursor_operat.query('USE %s_mvts' % self.__socname(idsoc))
        # self.cursor_operat.query('DELETE FROM alertes WHERE N = %s' % nalerte)
        # system / compteurs, suivisalertes

        def manage_srv2clt(self, userinfo_by_requester, connid, parsedcommand, cfg):
                """
                Defines the actions to be proceeded according to the client's commands.
                """
                cname = parsedcommand.name
                connid_socket = connid[1]
                astid = userinfo_by_requester[0]
                log_debug(SYSLOG_INFO, 'manage_srv2clt : %s / %s' % (cname, str(userinfo_by_requester)))

                if len(parsedcommand.args) > 0:
                        agentname = parsedcommand.args[0]
                else:
                        for agname, v in self.ulist[astid].list.iteritems():
                                if 'connection' in v:
                                        print 'manage_srv2clt (connected)', agname, v
                                        if v['connection'] == connid_socket:
                                                agentname = v['user']
                userinfo = self.ulist[astid].finduser(agentname)
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
                                log_debug(SYSLOG_INFO, 'AppelOpe : aoriginate_var for phonenum = %s' % phonenum)
                                self.amis[astid].aoriginate_var('sip', phonenum, 'Log %s' % phonenum,
                                                                agentnum, agentname, 'default', {'CB_MES_LOGAGENT' : agentname}, 100)
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
                                                if calls.parking is None and calls.peerchannel is not None:
                                                        calls.parking = j
                                                        topark.append(j)
                                                        if calls.dir == 'o':
                                                                mohclass = '/'.join([calls.nsoc, calls.ncli, calls.ncol])
                                                                true_mohclass = self.__moh_check(mohclass)
                                                                print 'the mohclass will be set to %s and not %s' % (true_mohclass, mohclass)
                                                                self.amis[astid].setvar(calls.peerchannel, 'CB_MOH', true_mohclass)
                                                        self.amis[astid].setvar(calls.peerchannel, 'PARKDIGIMUTE', '1')
                                                        self.amis[astid].transfer(calls.peerchannel, PARK_EXTEN, 'default')
                                        userinfo['calls'][calltoforce.commid] = calltoforce
                        else:
                                print 'ForceACD - unknown ref =', reference

                elif cname == 'Appel':
                        mreference = parsedcommand.args[1]
                        [dest, idsoc, idcli, idcol] = mreference.split('|')

                        time.sleep(0.2)
                        self.commidcurr += 1
                        comm_id_outgoing = str(self.commidcurr)

                        socname = self.__socname(idsoc)
                        outCall = OutgoingCall.OutgoingCall(comm_id_outgoing, astid, self.cursor_operat, socname,
                                                            userinfo, agentnum, agentname, dest,
                                                            self.__local_nsoc(idsoc), idcli, idcol)

                        if len(userinfo['calls']) > 0:
                                print 'Appel : there are already ongoing calls'
                                ntowait = 0
                                for reference, anycall in userinfo['calls'].iteritems():
                                        print 'ongoing call', reference, anycall.parking, anycall.peerchannel
                                        if anycall.parking is None and anycall.peerchannel is not None:
                                                anycall.parking = reference
                                                if anycall.dir == 'o':
                                                        mohclass = '/'.join([anycall.nsoc, anycall.ncli, anycall.ncol])
                                                        true_mohclass = self.__moh_check(mohclass)
                                                        print 'the mohclass will be set to %s and not %s' % (true_mohclass, mohclass)
                                                        self.amis[astid].setvar(anycall.peerchannel, 'CB_MOH', true_mohclass)
                                                self.amis[astid].setvar(anycall.peerchannel, 'PARKDIGIMUTE', '1')
                                                self.amis[astid].transfer(anycall.peerchannel, PARK_EXTEN, 'default')
                                                ntowait += 1
                                userinfo['calls'][comm_id_outgoing] = outCall
                                if ntowait > 0:
                                        userinfo['calls'][comm_id_outgoing].tocall = True
                                else:
                                        self.__outcall(outCall)
                        else:
                                if 'agentchannel' in userinfo:
                                        userinfo['calls'][comm_id_outgoing] = outCall
                                        self.__outcall(outCall)
                                else:
                                        userinfo['calls'][comm_id_outgoing] = outCall
                                        userinfo['calls'][comm_id_outgoing].tocall = True
                                        phonenum = userinfo_by_requester[5]
                                        self.amis[astid].aoriginate_var('sip', phonenum, 'Log %s' % phonenum,
                                                                        agentnum, agentname, 'default', {'CB_MES_LOGAGENT' : agentname}, 100)

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
                                                self.__update_stat_acd2(anycall)
                                        self.__update_taxes(anycall, 'Termine')
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

                                socname = self.__socname(idsoc)
                                outCall = OutgoingCall.OutgoingCall(comm_id_outgoing, astid, self.cursor_operat, socname,
                                                                    userinfo, agentnum, agentname, dest,
                                                                    self.__local_nsoc(idsoc), idcli, idcol)
                                userinfo['calls'][comm_id_outgoing] = outCall
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
                                if anycall.parking is None:
                                        if anycall.peerchannel is not None:
                                                anycall.parking = reference
                                                if anycall.dir == 'o':
                                                        mohclass = '/'.join([anycall.nsoc, anycall.ncli, anycall.ncol])
                                                        true_mohclass = self.__moh_check(mohclass)
                                                        print 'the mohclass will be set to %s and not %s' % (true_mohclass, mohclass)
                                                        self.amis[astid].setvar(anycall.peerchannel, 'CB_MOH', true_mohclass)
                                                self.amis[astid].setvar(anycall.peerchannel, 'PARKDIGIMUTE', '1')
                                                self.amis[astid].transfer(anycall.peerchannel, PARK_EXTEN, 'default')
                                                # the reply will be sent when the parkedcall signal is received
                                        else:
                                                print 'Attente : the agent is not in Attente mode yet but peerchannel is not defined'
                                else:
                                        print 'Attente : the agent is already in Attente mode with callref %s' % anycall.parking
                        else:
                                print 'Attente : the requested reference %s does not exist' % reference


                elif cname == 'Reprise':
                        reference = parsedcommand.args[1]
                        if reference in userinfo['calls']:
                                anycall = userinfo['calls'][reference]
                                if anycall.parking is not None:
                                        callbackexten = anycall.parkexten
                                        if callbackexten is not None:
                                                topark = []
                                                for j, calls in userinfo['calls'].iteritems():
                                                        if calls.parking is None and calls.peerchannel is not None:
                                                                calls.parking = j
                                                                topark.append(j)
                                                                print '#', calls.peerchannel
                                                                if calls.dir == 'o':
                                                                        mohclass = '/'.join([calls.nsoc, calls.ncli, calls.ncol])
                                                                        true_mohclass = self.__moh_check(mohclass)
                                                                        print 'the mohclass will be set to %s and not %s' % (true_mohclass, mohclass)
                                                                        self.amis[astid].setvar(calls.peerchannel, 'CB_MOH', true_mohclass)
                                                                self.amis[astid].setvar(calls.peerchannel, 'PARKDIGIMUTE', '1')
                                                                self.amis[astid].transfer(calls.peerchannel, PARK_EXTEN, 'default')
                                                if len(topark) > 0:
                                                        anycall.toretrieve = callbackexten
                                                else:
                                                        self.amis[astid].aoriginate('Agent', agentnum, agentname,
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

                        TIMEOUTPING = 120
                        if 'timer' in userinfo:
                                isalive = userinfo['timer'].isAlive()
                                userinfo['timer'].cancel()
                                del userinfo['timer']
                                if isalive:
                                        print 'timer/Ping : was running, renew the timer'
                                else:
                                        print 'timer/Ping : received a Ping but the timer was out ... should have been removed when out'
                        else:
                                print 'timer/Ping : first setup for this connection'

                        timer = threading.Timer(TIMEOUTPING, self.__ping_noreply)
                        timer.start()
                        userinfo['timer'] = timer


                elif cname == 'Sortie':
                        if 'timer' in userinfo:
                                userinfo['timer'].cancel()
                                del userinfo['timer']
                        userinfo['cbstatus'] = 'Sortie'


                elif cname == 'Pause':
                        userinfo['cbstatus'] = 'Pause'


                elif cname == 'Prêt' or cname == 'Change' or cname == 'Sonn':
                        if cname == 'Prêt':
                                reference = parsedcommand.args[1]
                                if userinfo['cbstatus'] == 'Pause':
                                        # XXX send current Indispo's to the user ?
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
                                        print 'fiche has been sent :', time.time()
                                return

                        try:
                                print 'manage : calling __choose()'
                                todo = self.__choose(astid)
                                print 'CHOOSE (after Pret)', todo
                                for opername, mtd in todo.iteritems():
                                        if len(mtd) > 0:
                                                td = mtd[0]
                                                # print 'pretx', td
                                                if td[0] == 'push':
                                                        self.__clear_call_fromqueues(astid, td[1])
                                                        self.__sendfiche(astid, opername, td[1])
                                                        print 'manage : fiche sent to %s' % opername
                                                elif td[0] == 'enqueue':
                                                        print 'manage : enqueue %s (pret)' % opername
                                                        self.__addtoqueue(astid, opername, td[1])
                                                elif td[0] == 'dequeue':
                                                        print 'manage : dequeue %s (pret)' % opername
                                                        userinfo = self.ulist[astid].finduser(opername)
                                                        td[1].agentlist.remove(opername)
                                                        self.amis[astid].queueremove(td[1].queuename, 'Agent/%s' % userinfo['agentnum'])
                                                        nagents = len(td[1].agentlist)
                                                        print 'nagents = %d' % nagents
                                                        if nagents == 0:
                                                                self.amis[astid].queueremove(td[1].queuename, 'Agent/6999')
                        except Exception, exc:
                                print '--- exception --- manage_srv2clt (Pret %s) : %s' % (reference, str(exc))

                elif cname == 'Enregistre':
                        reference = parsedcommand.args[1]
                        reply = ',,,%s/' % (cname)
                        connid_socket.send(reply)
                        self.amis[astid].aoriginate_var('Agent', agentnum, agentname,
                                                        '7444', 'Enregistre', 'default',
                                                        {'CB_RECORD_FILENAME' : reference[0]}, 100)

                elif cname == 'Alerte':
                        mreference = parsedcommand.args[1]
                        [nalerte, idsoc, idcli, idcol] = mreference.split('|')
                        reply = ',1,,%s/' % (cname)
                        connid_socket.send(reply)


                        # System Resources
                        columns = ('N', 'JOUR', 'DATED', 'DATEF', 'TYPE',
                                   'PlgD', 'PlgF', 'Valeur')
                        self.cursor_operat.query('USE system')
                        self.cursor_operat.query('SELECT ${columns} FROM ressource_struct',
                                                 columns)
                        system_ressource_struct = self.cursor_operat.fetchall()
                        for srs in system_ressource_struct:
                                print 'A/ (# outgoing lines according to date & time) :', srs
                                nsimlines = int(srs[7])

                        columns = ('N', 'NAlerteStruct', 'NomFichierMessage', 'ListeGroupes', 'interval_suivi')
                        self.cursor_operat.query('USE %s_mvts' % self.__socname(idsoc))
                        self.cursor_operat.query('SELECT ${columns} FROM alertes WHERE N = %s',
                                                 columns,
                                                 nalerte)
                        results = self.cursor_operat.fetchall()
                        for rr in results:
                                numstruct = rr[1]
                                filename = rr[2] # message file name
                                grouplist = rr[3].split(',')
                                intervsuivi = rr[4] # in seconds

                                # Alerte command => fetch details
                                print 'B/ Alerte : filename = %s (groups = %s) intsuivi = %s' %(filename, str(grouplist), intervsuivi)
                                columns = ('N', 'Libelle', 'NCol', 'NQuestion',
                                           'Type_Traitement', 'Nom_table_contact', 'Type_Alerte',
                                           'CallingNumber', 'nbTentatives', 'Alerte_Tous', 'Stop_Decroche')
                                self.cursor_operat.query('USE %s_clients' % self.__socname(idsoc))
                                self.cursor_operat.query('SELECT ${columns} FROM alerte_struct WHERE N = %s',
                                                         columns,
                                                         numstruct)
                                results2 = self.cursor_operat.fetchall()
                                nquestion = results2[0][3]
                                callingnum = results2[0][7]

                                # Fetch question details
                                columns = ('N', 'Libelle', 'Descriptif', 'Fichier', 'Type_saisie',
                                           'Touches_autorisees', 'Touches_terminales', 'Touche_repete',
                                           'T0', 'T1', 'T2', 'T3', 'T4', 'T5', 'T6', 'T7', 'T8', 'T9',
                                           'AttenteMax', 'TValidate')
                                self.cursor_operat.query('USE system')
                                self.cursor_operat.query('SELECT ${columns} FROM questions WHERE N = %s',
                                                         columns,
                                                         nquestion)
                                results3 = self.cursor_operat.fetchall()

                                nid = 0
                                for gl in grouplist:
                                        try:
                                                columns = ('N', 'Groupe', 'nom', 'prenom',
                                                           'tel1', 'tel2', 'tel3', 'tel4',
                                                           'Civilite', 'Fax', 'eMail', 'SMS', 'Code', 'DureeSonnerie')
                                                self.cursor_operat.query('USE %s_annexe' % self.__socname(idsoc))
                                                self.cursor_operat.query('SELECT * FROM %s' % gl)
                                                results4 = self.cursor_operat.fetchall()
                                                for r4 in results4:
                                                        rid = '%s-%06d' % (nalerte, nid)
                                                        nid += 1
                                                        self.alerts[rid] = {'nsoc' : idsoc,
                                                                             'group' : gl,
                                                                             'questionkind' : nquestion,
                                                                             'digits-allowed' : results3[0][5],
                                                                             'digits-end' : results3[0][6],
                                                                             'digits-repeat' : results3[0][7],
                                                                             'numbers' : r4[4:8],
                                                                             'timetoring' : int(r4[13]),
                                                                             'status' : 'init'
                                                                             }
                                        except Exception, exc:
                                                print 'grouplist %s : %s' % (gl, str(exc))


##                        for rid, al in self.alerts.iteritems():
##                                # print rid, al
##                                if al['numbers'][0] == '0450029451' and al['status'] == 'init':
##                                        self.amis[astid].aoriginate_var('sip', '102', 'dest 102',
##                                                                        'cidFB', 'automated call', 'alerte-callbooster',
##                                                                        {'CB_ALERT_MESSAGE' : 'fr/ss-noservice',
##                                                                         'CB_ALERT_ID' : rid,
##                                                                         'CB_ALERT_ALLOWED' : al['digits-allowed'],
##                                                                         'CB_ALERT_REPEAT' : al['digits-repeat']},
##                                                                        al['timetoring'])
##                                        al['status'] = 'called'

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


        def __moh_check(self, path):
                columns = ('category', 'var_name', 'var_val')
                self.cursor_xivo.query("SELECT ${columns} FROM musiconhold WHERE var_name = %s AND category = %s",
                                       columns,
                                       ('directory', path))
                results = self.cursor_xivo.fetchall()
                if results == ():
                        return 'callbooster'
                else:
                        return path


        # updated when it receives a reply from a remote Operat server
        def svreply(self, msg):
                params = msg.strip(chr(4)).split(chr(3))
                cmd = params[0]
                val = params[1]
                if cmd == 'ACDReponse':
                        print '(received) ACDReponse', msg
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
                                                        self.pending_sv_fiches[ic.commid] = reply
                                                elif val == 'Attente':
                                                        timer = threading.Timer(5, self.__svcheck)
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
                        for sdanum, lic in incoming_calls.iteritems():
                                for chan, ic in lic.iteritems():
                                        if ic.commid == commid :
                                                iic = ic
                        if iic is not None:
                                timer = threading.Timer(5, self.__svcheck)
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
                buf = os.read(self.queued_threads_pipe[0], 1024)
                print 'checkqueue, size =', self.tqueue.qsize()
                disconnlist = []
                while self.tqueue.qsize() > 0:
                        thisthread = self.tqueue.get()
                        tname = thisthread.getName()
                        if tname.find('Ping') == 0:
                                print 'checkqueue (Ping)', tname
                                for astid in self.ulist:
                                        for agname, userinfo in self.ulist[astid].list.iteritems():
                                                if 'timer' in userinfo:
                                                        if thisthread == userinfo['timer']:
                                                                agentnum = userinfo['agentnum']
                                                                self.cursor_operat.query('USE agents')
                                                                self.cursor_operat.query("UPDATE acd SET Etat = 'Sortie' WHERE NOPE = %d"
                                                                                         % (int(agentnum) - STARTAGENTNUM))
                                                                self.conn_operat.commit()
                                                                disconnlist.append(userinfo)

                        elif tname.find('SV') == 0:
                                print 'checkqueue (SV)', tname
                                for sdanum, lic in incoming_calls.iteritems():
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
                                                                print 'check if someone has taken the call ...'
                        else:
                                print 'checkqueue (unknown event kind)', tname
                return disconnlist


        def __ping_noreply(self):
                thisthread = threading.currentThread()
                tname = thisthread.getName()
                print '__ping_noreply (timer finished)', time.asctime(), tname
                thisthread.setName('Ping-' + tname)
                self.tqueue.put(thisthread)
                os.write(self.queued_threads_pipe[1], 'ping-')
                return

        # checking if any news from pending requests
        def __svcheck(self):
                thisthread = threading.currentThread()
                tname = thisthread.getName()
                print '__svcheck (timer finished)', time.asctime(), tname
                thisthread.setName('SV-' + tname)
                self.tqueue.put(thisthread)
                os.write(self.queued_threads_pipe[1], 'svcheck-')
                return

        def __init_taxes(self, call, numbertobill, fromN, toN, fromS, toS, NOpe):
                try:
                        [juridict, impulsion] = self.__gettaxes(numbertobill)
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

                self.cursor_operat.query('USE system')
                self.cursor_operat.query('UPDATE taxes SET Duree = %d, DureeSonnerie = %d, Etat = %s, nbTaxes = %d WHERE RefFB = %s'
                                         % (duree, dureesonnerie, '"%s"' % state, ntaxes, cs.commid))
                # self.cursor_operat.fetchall()
                self.conn_operat.commit()


        def __update_stat_acd(self, state, t0, in_period,
                              tt_raf, tt_asd, tt_snd, tt_sfa, tt_sop,
                              tt_tel, tt_fic, tt_bas, tt_mes, tt_rep):
                
                datetime = time.strftime(DATETIMEFMT, time.localtime(int(t0 / TSLOTTIME) * TSLOTTIME))
                period = '_'.join(in_period).strip('_')
                log_debug(SYSLOG_INFO, '__update_stat_acd : datetime = %s (period = %s)' %(datetime, period))

                try:
                        columns = ('DATE', 'Periode', 'SDA_NC', 'SDA_NV', 'SDA_HDV', 'SDA_V', 'TTraitement',
                                   'TT_RAF', 'TT_ASD', 'TT_SND', 'TT_SFA', 'TT_SOP',
                                   'TT_TEL', 'TT_FIC', 'TT_BAS', 'TT_MES', 'TT_REP',
                                   'NSOC' )
                        self.cursor_operat.query('USE system')
                        self.cursor_operat.query('SELECT ${columns} FROM stat_acd WHERE DATE = %s',
                                                 columns,
                                                 datetime)
                        results = self.cursor_operat.fetchall()
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
                        print 'exception in __update_stat_acd :', exc
                return


        def __update_stat_acd2(self, incall):
                """
                Updates the 'stat_acd2' table of the 'system' base.
                This is only for incoming calls.
                """
                if incall.statdone:
                        log_debug(SYSLOG_INFO, '__update_stat_acd2 : STAT ALREADY DONE for commid <%s>' % incall.commid)
                        return
                incall.statdone = True

                log_debug(SYSLOG_INFO, '__update_stat_acd2 (END OF INCOMING CALL) for commid <%s>' % incall.commid)
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
                is_appelaboute = False
                print '__update_stat_acd2 : history =',
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
                print '__update_stat_acd2 : history by action =',
                for act in bil:
                        print '(', act, bil[act], ')',
                print
                dtime = sortedtimes[nks - 1] - sortedtimes[0]
                print '__update_stat_acd2 : history - total time =', dtime
                if incall.uinfo is not None:
                        print '__update_stat_acd2 : uinfo calls', incall.uinfo['calls']
                        opername = incall.uinfo['user']
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
                        self.conn_operat.commit()                        
                        self.__update_stat_acd(state, sortedtimes[0], incall.period,
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

                print '__gettaxes :', num, juridict, impulsion

                if impulsion is None:
                        impulsion = [0, 0, 30]
                return [juridict, impulsion]


        def __juridictions(self, num):
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


        def __impulsion(self, jur):
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
                                                print '__choose : (SDA prio = %d) <%s> (%s)' % (sdaprio, opername, opstatus)
                                                userinfo = self.ulist[astid].finduser(opername)
                                                userqueuesize = len(userinfo['queuelist'])
                                                if opstatus is not None:
                                                        if 'connection' in userinfo:
                                                                print '__choose :', opername, userinfo, opstatus, userqueuesize
                                                                [status, dummy, level, prio, busyness] = opstatus
                                                                # print '__choose : incall : %s / opername : %s %s' % (incall.cidnum, opername, status)
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


                                        print '__choose : callid = %s :' % incall.commid, topush, to_enqueue
                                        if len(topush) == 0: # noone available for this call yet
                                                for opername in to_enqueue:
                                                        todo_by_oper[opername].append(['enqueue', incall])
                                        else:
                                                if len(topush) == 1: # somebody available
                                                        opername = topush.keys()[0]
                                                else: # choose among the pushers, according to prio and busyness
                                                        maxp = 0
                                                        minb = 100000
                                                        for opn, sel in topush.iteritems():
                                                                [p, b] = sel
                                                                if p > maxp or p >= maxp and b < minb:
                                                                        opername = opn
                                                                        maxp = p
                                                                        minb = b
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
                        thiscall = IncomingCall.IncomingCall(self.cursor_operat,
                                                             cidnum, sdanum, queuenum,
                                                             self.soperat_socket, self.soperat_port, self.opejd, self.opend)

                        if thiscall.statacd2_state != 'NC':
                                thiscall.setclicolnames()

                        self.__init_taxes(thiscall, cidnum, cidnum, sdanum, TRUNKNAME, 'PABX', 0)

                        if thiscall.statacd2_state == 'V':
                                print ' NCOMING CALL ## calling get_sda_profiles ##'
                                if sdanum not in incoming_calls:
                                        incoming_calls[sdanum] = {}
                                self.__clear_call_fromqueues(astid, thiscall)
                                ret = thiscall.get_sda_profiles(len(incoming_calls[sdanum]))
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
                                # maybe we had been in 'secretariat' mode previously, so that we shouldn't wait anymore
                                thiscall.waiting = False
                                # send CLR-ACD to the users who had received an Indispo as concerning this call
                                self.__clear_call_fromqueues(astid, thiscall)
                        elif action == 'secretariat':
                                thiscall.waiting = True
                                log_debug(SYSLOG_INFO, 'elect : calling __choose()')
                                todo = self.__choose(astid)
                                print 'CHOOSE (after Incall)', todo
                                # once all the queues have been spanned, send the push / queues where needed
                                argument = 'welcome'
                                nevt = 0
                                for opername, couplelist in todo.iteritems():
                                        for td in couplelist:
                                                log_debug(SYSLOG_INFO, 'elect : secretariat / %s / %s / %s' % (opername, td[0], td[1].sdanum))
                                                if td[0] == 'push':
                                                        if thiscall == td[1]:
                                                                nevt += 1
                                                                self.__clear_call_fromqueues(astid, td[1])
                                                                self.__sendfiche(astid, opername, td[1])
                                                                delay = 100
                                                                argument = None
                                                        else:
                                                                pass
                                                elif td[0] == 'enqueue':
                                                        nevt += 1
                                                        self.__addtoqueue(astid, opername, td[1])
                                                elif td[0] == 'dequeue':
                                                        print 'dequeue after Incall ???', opername, td
                                                        pass
                                if nevt == 0:
                                        action = 'noqueue'
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

