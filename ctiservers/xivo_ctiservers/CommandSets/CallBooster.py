# -*- coding: latin-1 -*-

"""
This is the CallBooster core class.
The main inputs are :
- the Agents' commands
- the Asterisk events
- the Incoming calls (through the AGI)

According to these inputs, the main functions taken care of are :
- Alerte's
- Taxes adn statistics
- Login / logouts
- Issue the Outgoing calls
- Handle the Incoming calls


#  There are about 5 different kinds of calls to manage here :
#     - (A) the Incoming Calls whose destination is one of the Agents' queue
#     - (B) the Incoming Calls whose destination is finally a phone number (Tel/Fic/Base)
#     - (C) the Outgoing Calls initiated by Operat
#     - (D) the Calls initiated by an Alerte
#     - (E) the regular Calls
#  Properties that can be catched :
#  - (B), (C), (D) and (E) go through Dial()
#  - one can set a given mark to identify (C)
#  - when (B) occurs, we can send an UserEventDialCB from the DialPlan


"""

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

# This is an extension to XIVO Daemon, authorized by Pro-formatique SARL
# for sub-licensing under a separated contract.
#
# Licensing of this code is NOT bounded by the terms of the
# GNU General Public License.
#
# See the LICENSE file at top of the source tree or delivered in the
# installable package in which XIVO Daemon is distributed for more details.

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

ENDCALLSTATUS_TAXES = 'Fini'

STATACD2_STATUSES = {'TT_RAF' : 0, 'TT_ASD' : 0, 'TT_SND' : 0, 'TT_SFA' : 0, 'TT_SOP' : 0,
                     'TT_TEL' : 0, 'TT_FIC' : 0, 'TT_BAS' : 0, 'TT_MES' : 0, 'TT_REP' : 0}

def varlog(syslogprio, string):
        if syslogprio <= SYSLOG_NOTICE:
                syslogf(syslogprio, 'callbooster : ' + string)
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
        xdname = 'XIVO-FB2'
        separator = '/'
        commands = ['Init',
                    'AppelOpe', 'TransfertOpe', 'RaccrocheOpe',
                    'Appel', 'Aboute', 'AppelAboute', 'Conf', 'PutConf', 'ListConf', 'Raccroche',
                    'Enregistre', 'Alerte',
                    'Ping',
                    'Attente', 'Reprise',
                    'Sonn',
                    'Prêt', 'ForceACD', 'ListeACD',
                    'Pause',
                    'Sortie',
                    'Change',
                    'SynchroXivoAgents',
                    'SynchroXivoSDA',
                    'SynchroXivoMOH']

        def __init__(self, amis, ctiports, queued_threads_pipe):
                """
                Defines the basic arguments.
                """
		BaseCommand.__init__(self)
                self.amis  = amis
                self.soperat_port = int(ctiports[1].split(':')[0])
                self.opejd = '08:00'
                self.opend = '20:00'
                self.queued_threads_pipe = queued_threads_pipe

                self.commidcurr = 200000
                self.soperat_socket = None
                self.pending_sv_fiches = {}
                self.tqueue = Queue.Queue()
                self.alerts = {}
                self.nalerts_called = 0
                self.nalerts_simult = 1
                self.waitingmessages = {}
                self.alerts_uniqueids = {}
                self.confrooms = {}
                
                # list of incoming calls, indexed by the channel number (as given by asterisk, so should be unique at a given time)
                self.incoming_calls = {}
                self.incoming_calls_byprio = [[], [], [], [], [], []]
                # number of incoming calls, indexed by SDA number
                self.n_incoming_calls = {}
                
                self.outgoing_calls = {}
                self.outgoing_calls_ng = {}
                self.normal_calls = []
                self.originated_calls = {}
                
        def set_operatini(self, operatini):
                """
                Reads the Operat.ini file (in the way to be deprecated)
                """
                if os.path.exists(operatini):
                        opconf = ConfigParser.ConfigParser()
                        opconf.readfp(open(operatini))
                        opconf_so = dict(opconf.items('SO'))
                        if 'opejd' in opconf_so:
                                self.opejd = opconf_so.get('opejd')
                        if 'opend' in opconf_so:
                                self.opend = opconf_so.get('opend')


        def set_userlist(self, ulist):
                self.ulist = ulist
                return


        def loginko(self, loginparams, errorstring, connid):
                """
                Actions to perform when login was KO.
                """
                print 'loginko', loginparams, errorstring
                connid.send('%s,0,,Init/' % loginparams.get('srvnum'))
                return


        def loginok(self, loginparams, userinfo):
                """
                Actions to perform when login was OK.
                Replies Init/
                """
                userinfo['login']['connection'].send('%s,1,,Init/' % loginparams.get('srvnum'))
                return


        def extrasock(self, extraconn):
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                try:
                        sock.connect((extraconn.split(':')[0], int(extraconn.split(':')[1])))
                        print 'Operat %s connected' % extraconn
                except Exception, exc:
                        log_debug(SYSLOG_WARNING, 'WARNING - Could not connect to Operat Serveur %s : %s'
                                  % (extraconn, str(exc)))
                        sock = None
                self.soperat_socket = sock
                return sock


        def __send_msg__(self, uinfo, msg):
                """
                Sends a message 'msg' to destination 'uinfo', after having proceeded a few checks.
                """
                if uinfo is not None and 'login' in uinfo and 'connection' in uinfo.get('login'):
                        try:
                                uinfo['login']['connection'].send(msg)
                                log_debug(SYSLOG_INFO, '__send_msg__ <%s> sent' % msg)
                        except Exception, exc:
                                print '--- exception --- (CallBooster) could not send <%s> to user : %s' % (msg, str(exc))
                else:
                        print '(CallBooster) could not send <%s> to user (no connection field defined)' % msg
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

                                if self.cursor_xivo is not None:
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

                                localulist[oname] =  {'user'     : oname,
                                                      'agentnum' : agnum,
                                                      'record'   : r[3]}
##                                agconf = { 'astid' : astid,
##                                           'agentnum' : agnum,
##                                           }

##                                localulist_ng[oname] = { 'username' : oname,
##                                                         'modeagent' : True,
##                                                         'agent' : agconf,
##                                                         'login' : {} }
##                                # capas
##                                # phone : + tech
##                                # login : 'calls', queuelist,
                except Exception, exc:
                        print '--- exception --- in getuserlist()', exc
                return localulist


        def set_options(self, xivoconf_local):
                self.cursor_operat = None
                self.cursor_xivo = None
                if 'operat_db_uri' in xivoconf_local:
                        uri_operat = xivoconf_local['operat_db_uri']
                        try:
                                self.conn_operat   = anysql.connect_by_uri(uri_operat)
                                self.cursor_operat = self.conn_operat.cursor()
                        except Exception, exc:
                                self.cursor_operat = None
                if 'xivo_db_uri' in xivoconf_local:
                        uri_xivo = xivoconf_local['xivo_db_uri']
                        try:
                                self.conn_xivo     = anysql.connect_by_uri(uri_xivo)
                                self.cursor_xivo   = self.conn_xivo.cursor()
                        except Exception, exc:
                                self.cursor_xivo   = None
                return


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
                        cfg = { 'computername' : computername,
                                'phonenum' : phonenum,
                                'computeripref' : computeripref,
                                'srvnum' : srvnum,
                                'user' : tagent }
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
                                phlist = [tagent,
                                          None,
                                          '%d' % (STARTAGENTNUM + int(nope)),
                                          '0:0:0']
                                self.ulist.adduser(astid, phlist)
                                userinfo = self.ulist.finduser(tagent)
                                userinfo['sendfiche'] = ['qcb_%05d' % (int(callref) - 100000), 'Agent/%s' % agentnum]

                                cfg = { 'computername' : nope,
                                        'phonenum' : phonenum,
                                        'computeripref' : nadherent,
                                        'srvnum' : nperm,
                                        'user' : tagent }
                                return cfg
                        else:
                                pass

        userfields = ['user', 'agentnum', 'record']
        def required_login_params(self):
                """
                Returns the list of required login parameters
                """
                return ['user', 'computername', 'phonenum', 'computeripref', 'srvnum']


        def manage_login(self, loginparams):
                """
                Manages login arguments.
                Checks misc credentials if needed.
                Fills initialization arguments.
                """
                # print loginparams
                missings = []
                for argum in self.required_login_params():
                        if argum not in loginparams:
                                missings.append(argum)
                if len(missings) > 0:
                        return 'missing:%s' % ','.join(missings)

                username = loginparams.get('user')
                # print username
                phonenum = loginparams.get('phonenum')

                userinfo = None
                for astid, ulist in self.ulist.byast.iteritems():
                        userinfo = ulist.finduser(username)
                ## userinfo = self.ulist.list.get(username)
                if userinfo == None:
                        return 'user_not_found'

                # check_user_connection(userinfo, whoami)
                userinfo['phonenum'] = phonenum
                userinfo['login'] = {}
                userinfo['login']['calls'] = {}
                userinfo['login']['queuelist'] = {}
                userinfo['login']['cbstatus'] = 'undefined'
                ## userinfo['agent']['phonenum'] = phonenum
                # connect_user() fills userinfo with new arguments
                return userinfo


        def manage_logoff(self, userinfo):
                print 'logoff', userinfo
                if 'login' in userinfo:
                        del userinfo['login']
                return


        def connected(self, connid):
                """
                Sends a 'connected' status to the client once the TCP link has been setup.
                """
                msg = 'Connect%s:%d/' % (connid.getpeername()[0], connid.getpeername()[1])
                connid.send(msg)
                return


        def __uinfo_by_agentnum__(self, astid, agentnum):
                """
                Sets the userinfo according to the agent's phonenumber
                """
                userinfo = None
                for opername, uinfo in self.ulist.byast[astid].list.iteritems():
                        if 'agentnum' in uinfo and uinfo['agentnum'] == agentnum:
                                userinfo = uinfo
                                break
                return userinfo


        # To follow are the methods to handle Asterisk AMI events
        def ami_dial(self, astid, event):
                """
                Function called when an AMI Dial Event is read.
                """
                srcuniqueid = event.get('SrcUniqueID')
                destuniqueid = event.get('DestUniqueID')
                source = event.get('Source')
                destination = event.get('Destination')
                
                if srcuniqueid in self.originated_calls:
                        self.originated_calls[destuniqueid] = self.originated_calls[srcuniqueid]
                        if destination.startswith('Local/'):
                                print 'DIAL originated call', source, destination, 'remembering'
                                dui = destuniqueid.split('.')
                                nextsrcuniqueid = '.'.join([dui[0], str(int(dui[1]) + 1)])
                                self.originated_calls[nextsrcuniqueid] = self.originated_calls[srcuniqueid]
                        else:
                                print 'DIAL originated call', source, destination
                else:
                        print '(NORMAL DIAL)', srcuniqueid, destuniqueid, event
                        self.normal_calls.append(srcuniqueid)
                        # self.__init_taxes__(call, event.get('Destination'), event.get('CallerID'), '103', TAXES_PABX, TAXES_TRUNKNAME, 0)
                return


        def ami_link(self, astid, event):
                """
                Function called when an AMI Link Event is read.
                We have a few different cases to handle here :
                - when a new call is issued
                - when a user is removed from its Parking place (Reprise command)
                - when a connection has been established after an Aboute or AppelAboute command
                """
                uniqueid1 = event.get('Uniqueid1')
                uniqueid2 = event.get('Uniqueid2')
                channel1 = event.get('Channel1')
                channel2 = event.get('Channel2')

                if uniqueid1 in self.originated_calls:
                        # either Incoming+Tel, Outgoing, (Alert)
                        print 'LINK originated call', channel1, channel2, self.originated_calls[uniqueid1]
                        if self.originated_calls[uniqueid1] == 'dialcb':
                                ic1 = self.incoming_calls.get(event.get('Channel1'))
                                if ic1 is not None:
                                        print 'LINK Incoming+Tel', event, ic1
                                return

                        if channel1.startswith('Local/'):
                                # OUTGOING CALL PART 1
                                rchannel1 = channel1.replace(',2', ',1')
                                if channel1.find(',2') > 0:
                                        self.outgoing_calls_ng[channel1] = event

                                if rchannel1 in self.outgoing_calls_ng:
                                        # if 'Agent/' has been received before 'Local/'
                                        nevent = self.outgoing_calls_ng[rchannel1]
                                        agentnum = nevent.get('Channel1').split('/')[1]
                                        userinfo = self.__uinfo_by_agentnum__(astid, agentnum)
                                        if userinfo is None or 'login' not in userinfo:
                                                log_debug(SYSLOG_WARNING, '(link) no logged-in user found for agent number %s' % agentnum)
                                                return
                                        mycall = None
                                        for anycommid, anycall in userinfo['login']['calls'].iteritems():
                                                if anycall.dir == 'o' and anycall.peerchannel is None and nevent.get('Channel2')[6:].split('@')[0] == anycall.dest:
                                                        mycall = anycall
                                                        break
                                        if mycall is not None:
                                                print mycall, mycall.dest, mycall.parking
                                                mycall.peerchannel = channel2
                                                mycall.set_timestamp_tax('link')
                                                # self.__update_taxes__(anycall.call, 'Decroche')
                                                self.__send_msg__(userinfo, '%s,1,%s,Decroche/' % (mycall.commid, mycall.dest))
                                                self.outgoing_calls[mycall.peerchannel] = mycall
                                        del self.outgoing_calls_ng[rchannel1]
                                else:
                                        # no corresponding action
                                        pass

                        if channel1.startswith('Agent/'):
                                # OUTGOING CALL PART 2
                                rchannel2 = channel2.replace(',1', ',2')
                                if channel2.find(',1') > 0:
                                        self.outgoing_calls_ng[channel2] = event

                                agentnum = channel1.split('/')[1]
                                userinfo = self.__uinfo_by_agentnum__(astid, agentnum)
                                if userinfo is None or 'login' not in userinfo:
                                        log_debug(SYSLOG_WARNING, '(link) no logged-in user found for agent number %s' % agentnum)
                                        return
                                if rchannel2 in self.outgoing_calls_ng:
                                        # if 'Local/' has been received before 'Agent/'
                                        # find the call that matches
                                        mycall = None
                                        for anycommid, anycall in userinfo['login']['calls'].iteritems():
                                                if anycall.dir == 'o' and anycall.peerchannel is None and channel2[6:].split('@')[0] == anycall.dest:
                                                        mycall = anycall
                                                        break
                                        if mycall is not None:
                                                print mycall, mycall.dest, mycall.parking
                                                mycall.peerchannel = self.outgoing_calls_ng[rchannel2]['Channel2']
                                                mycall.set_timestamp_tax('link')
                                                # self.__update_taxes__(anycall.call, 'Decroche')
                                                self.__send_msg__(userinfo, '%s,1,%s,Decroche/' % (mycall.commid, mycall.dest))
                                                self.outgoing_calls[mycall.peerchannel] = mycall
                                        else:
                                                print 'found no current call for dest number %s' % channel2[6:].split('@')[0]
                                        del self.outgoing_calls_ng[rchannel2]
                                else:
                                        # for unparking status detection
                                        mycall = None
                                        for anycommid, anycall in userinfo['login']['calls'].iteritems():
                                                if channel2 == anycall.peerchannel:
                                                        anycall.parking = False
                else:
                        # Incoming or Normal/Alert
                        if channel2.startswith('Agent/'):
                                print 'LINK NOT originated call Incoming', channel1, channel2
                                # LINK FOR INCOMING CALL
                                agentnum = channel2.split('/')[1]
                                userinfo = self.__uinfo_by_agentnum__(astid, agentnum)
                                if userinfo is None or 'login' not in userinfo:
                                        log_debug(SYSLOG_WARNING, '(link) no logged-in user found for agent number %s' % agentnum)
                                        return
                                # find the call that matches
                                mycall = None
                                for anycommid, anycall in userinfo['login']['calls'].iteritems():
                                        if anycall.dir == 'i' and anycall.peerchannel is None:
                                                mycall = anycall
                                                break
                                if mycall is not None:
                                        print mycall, mycall.parking
                                        mycall.peerchannel = channel1
                                        mycall.set_timestamp_stat('link')
                                        mycall.set_timestamp_tax('link')
                        else:
                                # Actually, while an Alert event is an Originated one, it might come here since the Dial event
                                # comes before the OriginateSuccess or OriginateFailure
                                ic1 = self.incoming_calls.get(event.get('Channel1'))
                                ic2 = self.incoming_calls.get(event.get('Channel2'))
                                if ic1 is not None:
                                        print 'LINK after Aboute or AppelAboute', ic1.sdanum, ic1.commid, ic1.appelaboute, ic1.aboute
                                        if ic1.appelaboute is not None:
                                                self.__send_msg__(ic1.uinfo, '%s|%s,1,%s,AppelAboute/'
                                                                  % (ic1.appelaboute, ic1.commid, ic1.appelaboute))
                                        elif ic1.aboute is not None:
                                                """When the called party was waiting"""
                                                self.__send_msg__(ic1.uinfo, '%s|%s,1,,Aboute/'
                                                                  % (ic1.aboute, ic1.commid))
                                        else:
                                                log_debug(SYSLOG_WARNING, 'No match for this Link')
                                        ic1.set_timestamp_stat('link')
                                elif ic2 is not None:
                                        print 'LINK after Aboute', ic2.sdanum, ic2.commid, ic2.appelaboute, ic2.aboute
                                        if ic2.aboute is not None:
                                                """When the calling party was waiting"""
                                                self.__send_msg__(ic2.uinfo, '%s|%s,1,,Aboute/'
                                                                  % (ic2.aboute, ic2.commid))
                                        ic2.set_timestamp_stat('link')
                                else:
                                        print '(NORMAL LINK)', event
                                        # self.__update_taxes__
                return


        def ami_unlink(self, astid, event):
                """
                Function called when an AMI Unlink Event is read.
                We have a few different cases to handle here :
                - when an established call is over
                - when a peer is put into a Parking (Attente command)
                We must be careful anyway, not to count twice some of these events, since two
                Hangup Events will also be issued for each Unlink event.
                """
                uniqueid1 = event.get('Uniqueid1')
                uniqueid2 = event.get('Uniqueid2')
                channel1 = event.get('Channel1')
                channel2 = event.get('Channel2')

                if uniqueid1 in self.originated_calls:
                        # either Incoming+Tel, Outgoing, (Alert)
                        print 'UNLINK originated call', channel1, channel2
                        if self.originated_calls[uniqueid1] == 'dialcb':
                                ic1 = self.incoming_calls.get(event.get('Channel1'))
                                if ic1 is not None:
                                        print 'UNLINK Incoming+Tel', event, ic1
                                        self.originated_calls[uniqueid1] = 'unlink'
                                        if uniqueid2 in self.originated_calls:
                                                self.originated_calls[uniqueid2] = 'unlink'
                                        else:
                                                print 'WARNING UNLINK %s but not %s' % (uniqueid1, uniqueid2)
                                else:
                                        del self.originated_calls[uniqueid1]
                                        if uniqueid2 in self.originated_calls:
                                                del self.originated_calls[uniqueid2]
                                return

                        self.originated_calls[uniqueid1] = 'unlink'
                        if uniqueid2 in self.originated_calls:
                                self.originated_calls[uniqueid2] = 'unlink'
                        else:
                                print 'WARNING UNLINK %s but not %s' % (uniqueid1, uniqueid2)

                        if channel1.startswith('Agent/'):
                                # ESTABLISHED OUTGOING CALL hanged up
                                # - or aboute proceeding
                                agentnum = channel1.split('/')[1]
                                userinfo = self.__uinfo_by_agentnum__(astid, agentnum)
                                if userinfo is None or 'login' not in userinfo:
                                        log_debug(SYSLOG_WARNING, 'UNLINK : no user found for agent number %s' % agentnum)
                                        return
                                mycall = None
                                # in 'aboute' case, channel2 is of the kind AsyncGoto/SIP/101-081fafb8<ZOMBIE>,
                                # while peerchannel is of the kind SIP/101-081fafb8, so no call will be found
                                for anycommid, anycall in userinfo['login']['calls'].iteritems():
                                        if anycall.peerchannel == channel2:
                                                mycall = anycall
                                                break
                                if mycall is not None:
                                        print 'I am about to break the OUTGOING call <%s>' % mycall.commid, mycall.parking
                                        self.__send_msg__(userinfo, '%s,1,,Annule/' % mycall.commid)
                                        mycall.annuleraccroche = True
                                        self.__update_taxes__(mycall, ENDCALLSTATUS_TAXES)
                                        userinfo['login']['calls'].pop(mycall.commid)

                else:
                        # Incoming or Normal/Alert
                        if channel2.startswith('Agent/'):
                                print 'UNLINK NOT originated call Incoming', channel1, channel2
                                # ESTABLISHED INCOMING CALL hanged up
                                # - or parking proceeding
                                # - or appelaboute proceeding
                        
                                agentnum = channel2.split('/')[1]
                                userinfo = self.__uinfo_by_agentnum__(astid, agentnum)
                                if userinfo is None or 'login' not in userinfo:
                                        log_debug(SYSLOG_WARNING, 'UNLINK : no user found for agent number %s' % agentnum)
                                        return
                                mycall = None
                                for anycommid, anycall in userinfo['login']['calls'].iteritems():
                                        if anycall.peerchannel == channel1:
                                                mycall = anycall
                                                break
                                if mycall is not None:
                                        print 'UNLINK : I am about to break the INCOMING call <%s>' % mycall.commid, mycall.parking
                                        if mycall.parking:
                                                print 'UNLINK : HOWEVER it seems this is not worth since it had been set as a parked one ...'
                                        elif mycall.appelaboute is not None:
                                                print 'UNLINK : HOWEVER it seems this is not worth since it had been set as an appelaboute one ...', mycall.appelaboute
                                        else:
                                                self.__send_msg__(userinfo, '%s,1,,Annule/' % mycall.commid)
                                                mycall.annuleraccroche = True
                                                self.__update_stat_acd2__(mycall)
                                                self.__update_taxes__(mycall, ENDCALLSTATUS_TAXES)
                                                userinfo['login']['calls'].pop(mycall.commid)
                        else:
                                ic1 = self.incoming_calls.get(event.get('Channel1'))
                                ic2 = self.incoming_calls.get(event.get('Channel2'))
                                if ic1 is not None:
                                        log_debug(SYSLOG_INFO, 'UNLINK without Agent (1) sda=%s commid=%s' % (ic1.sdanum, ic1.commid))
                                        ic1.set_timestamp_stat('unlink')
                                elif ic2 is not None:
                                        log_debug(SYSLOG_INFO, 'UNLINK without Agent (2) sda=%s commid=%s' % (ic2.sdanum, ic2.commid))
                                        ic2.set_timestamp_stat('unlink')
                                else:
                                        print '(NORMAL UNLINK)', event
                                        # self.__update_taxes__
                return


        """
        Hangup Cases :
        * link established
        - peer hangs up
        - agent hangs up (button or phone)
        * link not established
        - only the caller can hangup
        * special cases :
        - during parking (park giveup)
        - during a call establishment (agent is being called but does not reply)
        - alerts
        """
        def ami_hangup(self, astid, event):
                """
                Function called when an AMI Hangup Event is read.
                When the call has already been established (the peers are linked), an Unlink event should
                have been issued previously.
                """
                chan = event.get('Channel')
                uniqueid = event.get('Uniqueid')
                
                thiscall = self.incoming_calls.get(chan)
                if thiscall is not None:
                        print 'HANGUP => __update_stat_acd2__', uniqueid, chan, thiscall.queuename, thiscall.aboute, thiscall.uinfo
                        if thiscall.uinfo is not None:
                                if 'login' in thiscall.uinfo and thiscall.aboute is not None:
                                        del thiscall.uinfo['login']['calls'][thiscall.aboute]
                                if 'agent-wouldbe-channel' in thiscall.uinfo and 'agentchannel' not in thiscall.uinfo:
                                        self.__send_msg__(thiscall.uinfo, ',-1,,AppelOpe/')
                                        self.amis[astid].hangup(thiscall.uinfo['agent-wouldbe-channel'], '')
                        self.__update_taxes__(thiscall, ENDCALLSTATUS_TAXES)
                        self.__update_stat_acd2__(thiscall)
                        self.__clear_call_fromqueues__(astid, thiscall)
                        # removes the call from the incoming calls' list
                        self.__incall_remove__(thiscall, chan)
                        del thiscall


                if uniqueid in self.alerts_uniqueids:
                        # should not come here after success since it is deleted in uservent treatment
                        print 'HANGUP ALERT', uniqueid, self.alerts_uniqueids[uniqueid]

                        channel = self.alerts_uniqueids[uniqueid]
                        num = channel[6:-8] # very rough way to extract number from 'local/<num>@default'
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
                elif uniqueid in self.originated_calls:
                        val = self.originated_calls[uniqueid]
                        if val == 'unlink':
                                print 'HANGUP originated call (already unlinked)', chan
                        else:
                                print 'HANGUP originated call', chan
                        del self.originated_calls[uniqueid]
                elif uniqueid in self.normal_calls:
                        print '(NORMAL HANGUP)', uniqueid, event
                        self.normal_calls.remove(uniqueid)
                else:
                        print 'OTHER HANGUP', event
                        # self.__update_taxes__
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
                        self.originated_calls[uniqueid] = 'origsuccess-alert'
                elif context == 'ctx-callbooster-agentlogin':
                        channel = event.get('Channel')
                        reason = event.get('Reason')
                        exten = event.get('Exten')
                        print '__originatesuccess__ (agentlogin) (%s) :' % kind, astid, event
                        uinfo = self.__uinfo_by_agentnum__(astid, exten)
                        if uinfo is not None:
                                if 'sendfiche' in uinfo:
                                        del uinfo['sendfiche']
                        else:
                                log_debug(SYSLOG_WARNING, '__originatesuccess__ (agentlogin) : No user found for exten %s' % exten)
                else:
                        print '__originatesuccess__ (%s) :' % kind, astid, event
                        self.originated_calls[uniqueid] = 'origsuccess'
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

                        num = channel[6:-8] # very rough way to extract number from 'local/<num>@default'
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
                        uinfo = self.__uinfo_by_agentnum__(astid, exten)
                        if uinfo is not None:
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
                                log_debug(SYSLOG_WARNING, '__originatefailure__ (agentlogin) : No user found for exten %s' % exten)
                else:
                        print '__originatefailure__ (%s) :' % kind, astid, event
                        uniqueid = event.get('Uniqueid')
                        self.originated_calls[uniqueid] = 'origfailure'
                return


        def ami_aoriginatesuccess(self, astid, event):
                """
                Function called when an AMI AOriginateSuccess Event is read.
                """
                self.__originatesuccess__(astid, event, 'a')
                return
        def ami_originatesuccess(self, astid, event):
                """
                Function called when an AMI OriginateSuccess Event is read.
                """
                self.__originatesuccess__(astid, event, '')
                return
        def ami_aoriginatefailure(self, astid, event):
                """
                Function called when an AMI AOriginateFailure Event is read.
                """
                self.__originatefailure__(astid, event, 'a')
                return
        def ami_originatefailure(self, astid, event):
                """
                Function called when an AMI OriginateFailure Event is read.
                """
                self.__originatefailure__(astid, event, '')
                return


        def ami_messagewaiting(self, astid, event):
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


        def ami_newcallerid(self, astid, event):
                """
                Function called when an AMI NewCallerId Event is read.
                This is used in order to know which asterisk channel to hang-up in order to stop
                calling the agent (when the timeout has elapsed).
                """
                agentnum = event.get('CallerID')
                channel = event.get('Channel')
                uniqueid = event.get('Uniqueid')
                
                userinfo = self.__uinfo_by_agentnum__(astid, agentnum)
                if userinfo is not None:
                        userinfo['agent-wouldbe-channel'] = channel
                else:
                        # there might be too much calls coming here to be worth mentioning
                        pass
                return


        def ami_parkedcall(self, astid, event):
                """
                Function called when an AMI ParkedCall Event is read.
                """
                print 'PARKEDCALL', event
                # PARKEDCALL clg {'From': 'SIP/101-081c0438', 'CallerID': '101', 'Timeout': '45', 'CallerIDName': 'User1'}
                chan  = event.get('Channel')
                exten = event.get('Exten')

                # find the channel among the incoming calls, otherwise among outgoing ones
                thiscall = self.incoming_calls.get(chan)
                if thiscall is None and chan in self.outgoing_calls:
                        thiscall = self.outgoing_calls[chan]

                if thiscall is None:
                        log_debug(SYSLOG_WARNING, 'received a parkedcall from an unknown channel <%s>' % chan)
                        return

                thiscall.set_timestamp_stat('parked')
                thiscall.parkexten = exten
                self.__send_msg__(thiscall.uinfo, '%s,1,,Attente/' % thiscall.commid)

                usercalls = thiscall.uinfo['login']['calls']
                for commid, usercall in usercalls.iteritems():
                        if commid != thiscall.commid:
                                print 'PARKEDCALL commid=%s parkexten=%s tocall=%s forceacd=%s toretrieve=%s' % (commid, usercall.parkexten, usercall.tocall, usercall.forceacd, usercall.toretrieve)
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
                                        self.__send_msg__(uinfo, '%s,%s,%s,Fiche/' % (usercall.commid, usercall.sdanum, cnum))
                                        self.__send_msg__(uinfo, '%s,%s,,CLR-ACD/' % (usercall.commid, usercall.sdanum))
                                        usercall.forceacd = None
                                elif usercall.toretrieve is not None:
                                        print 'usercall.toretrieve', usercall.toretrieve
                                        self.amis[astid].aoriginate('Agent', usercall.uinfo['agentnum'], 'agentname',
                                                                    usercall.toretrieve, 'cid b', 'default')
                                        usercall.toretrieve = None
                return


        def ami_unparkedcall(self, astid, event):
                """
                Function called when an AMI UnParkedCall Event is read.
                """
                print 'UNPARKEDCALL', event
                chan = event.get('Channel')

                # find the channel among the incoming calls, otherwise among outgoing ones
                thiscall = self.incoming_calls.get(chan)
                if thiscall is None and chan in self.outgoing_calls:
                        thiscall = self.outgoing_calls[chan]

                if thiscall is None:
                        print 'received an unparkedcall from an unknown channel <%s>' % chan
                else:
                        thiscall.set_timestamp_stat('unparked')
                        thiscall.parkexten = None
                        self.__send_msg__(thiscall.uinfo, '%s,1,,Reprise/' % thiscall.commid)
                        print 'ParkedCall Reprise', thiscall.uinfo['login']['calls']
                return


        def ami_parkedcallgiveup(self, astid, event):
                """
                Function called when an AMI ParkedCallGiveUp Event is read.
                """
                print 'GIVEUP-PARKEDCALL', astid, event
                chan = event.get('Channel')

                # find the channel among the incoming calls, otherwise among outgoing ones
                thiscall = self.incoming_calls.get(chan)
                if thiscall is None and chan in self.outgoing_calls:
                        thiscall = self.outgoing_calls[chan]

                if thiscall is None:
                        print 'received a parkedcallgiveup from an unknown channel <%s>' % chan
                else:
                        thiscall.set_timestamp_stat('parkgiveup')
                        thiscall.parkexten = None
                        self.__send_msg__(thiscall.uinfo, '%s,1,,Annule/' % thiscall.commid)
                        thiscall.annuleraccroche = True
                        print 'ParkedCall Annule', thiscall.parking, thiscall.peerchannel, thiscall.uinfo['login']['calls']
                        # remove the call from userinfo + list
                        # XXX update taxes & stats !
                return


        def ami_agentlogin(self, astid, event):
                """
                Function called when an AMI Agentlogin Event is read.
                This is used to send the right acknowledgement to the agent's Operat.
                In case pending calls were waiting to be issued, they are initiated here.
                """
                agentnum = event.get('Agent')
                agentchannel = event.get('Channel')
                userinfo = self.__uinfo_by_agentnum__(astid, agentnum)
                if userinfo is not None:
                        userinfo['agentchannel'] = agentchannel
                        userinfo['agentchannel-conf'] = agentchannel
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

                        if 'agentlogin-fromconf' in userinfo:
                                del userinfo['agentlogin-fromconf']
                        else:
                                self.__send_msg__(userinfo, ',1,,AppelOpe/')
                                if 'post_agentlogin_outcall' in userinfo['login']:
                                        callnum = userinfo['login'].get('post_agentlogin_outcall')
                                        anycall = userinfo['login']['calls'][callnum]
                                        log_debug(SYSLOG_INFO, 'an outgoing call was waiting to be sent ...')
                                        time.sleep(1) # otherwise the Agent's channel is not found
                                        self.__outcall__(anycall)
                                        del userinfo['login']['post_agentlogin_outcall']
                                elif 'post_agentlogin_record' in userinfo['login']:
                                        reference = userinfo['login'].get('post_agentlogin_record')
                                        log_debug(SYSLOG_INFO, 'a record was waiting to be sent ...')
                                        time.sleep(1) # otherwise the Agent's channel is not found
                                        self.amis[astid].aoriginate_var('Agent', agentnum, userinfo.get('user'),
                                                                        'record_exten', 'Enregistre', 'ctx-callbooster-record',
                                                                        {'CB_RECORD_FILENAME' : reference[0]}, 100)
                                        self.__send_msg__(userinfo, ',,,Enregistre/')
                                        del userinfo['login']['post_agentlogin_record']
                else:
                        log_debug(SYSLOG_WARNING, '(agentlogin) no user found for agent %s' % agentnum)
                return


        def ami_agentlogoff(self, astid, event):
                """
                Function called when an AMI Agentlogoff Event is read.
                This occurs when an agent hangs up.
                """
                agentnum = event.get('Agent')
                userinfo = self.__uinfo_by_agentnum__(astid, agentnum)
                if userinfo is not None:
                        if 'login' in userinfo:
                                print userinfo['agentnum'], 'has left', userinfo['login']['calls']
                                del userinfo['agentchannel']
                                if 'conf' in userinfo:
                                        print 'actually, %s goes into the conf room %s' % (agentnum, userinfo.get('conf'))
                                else:
                                        for callnum, anycall in userinfo['login']['calls'].iteritems():
                                                if anycall.peerchannel is not None:
                                                        self.amis[astid].hangup(anycall.peerchannel, '')
                        else:
                                log_debug(SYSLOG_WARNING, '(agentlogoff) found agent %s, but was not logged in' % userinfo['agentnum'])
                else:
                        log_debug(SYSLOG_WARNING, '(agentlogoff) no user found for agent %s' % agentnum)
                return


        def ami_agentcallbacklogoff(self, astid, event):
                """
                Function called when an AMI Agentcallbacklogoff Event is read.
                This occurs after the RaccrocheOpe has been sent.
                """
                print 'agentcallbacklogoff', astid, event
                return


        def ami_userevent(self, astid, event):
                """
                Function called when an AMI UserEventXXX Event is read, where XXX is almost any string.
                This is used for 2 user-defined events :
                * UserEventAlertCB : we fall here when a 'good' reply has been given by the DTMF Read()
                * UserEventVoiceMailCB : this allows us to connect the voicemail call details to the
                upcoming messagewaiting Event.
                * UserEventDialCB : in order to track Operat-originated Dial's
                """
                print 'userevent :', astid, event
                evfunction = event.get('Event')
                uniqueid = event.get('Uniqueid')
                if uniqueid in self.alerts_uniqueids:
                        del self.alerts_uniqueids[uniqueid]
                if evfunction == 'UserEventAlertCB':
                        appdatas = event.get('AppData').split('-', 1)
                        [dtmfreply, rid] = appdatas
                        if rid in self.alerts:
                                aldetail = self.alerts[rid]
                                print len(self.alerts), self.alerts[rid]
                                self.__fill_alert_result__(aldetail, dtmfreply)
                                mygroup = self.alerts[rid]['group'] # or tableequipier ??
                                del self.alerts[rid]
                                self.nalerts_called -= 1
                                if aldetail['alertetous'] == 1:
                                        for riditer, ald in self.alerts.iteritems():
                                                if ald['group'] == mygroup:
                                                        del self.alerts[riditer]
                        else:
                                log_debug(SYSLOG_WARNING, 'received an Alert ACK %s for a seemingly not requested alert !' % rid)
                        self.__alert_calls__(astid)
                elif evfunction == 'UserEventVoiceMailCB':
                        appdatas = event.get('AppData').split('-')
                        indice = appdatas[0]
                        sdanum = appdatas[1]
                        if sdanum not in self.waitingmessages:
                                self.waitingmessages[sdanum] = []
                        self.waitingmessages[sdanum].append(indice)
                elif evfunction == 'UserEventDialCB':
                        dialid = event.get('AppData')
                        self.originated_calls[dialid] = 'dialcb'
                return


        def ami_agents(self, astid, event):
                """
                Function called when an AMI Agents Event is read.
                Checks which Agents are not Logged Off.
                This is primarily called at FB's startup and avoids an already-logged-in agent to be
                logged in again.
                """
                agentnum = event.get('Agent')
                agentchannel = event.get('LoggedInChan')
                status = event.get('Status')
                
                if status != 'AGENT_LOGGEDOFF':
                        userinfo = self.__uinfo_by_agentnum__(astid, agentnum)
                        if userinfo is not None:
                                userinfo['agentchannel'] = agentchannel
                        else:
                                log_debug(SYSLOG_WARNING, 'agent_was_logged_in : No user found for agent number %s' % agentnum)

                return


        def ami_meetmejoin(self, astid, event):
                """
                Function called when an AMI MeetMeJoin Event is read.
                """
                meetme = event.get('Meetme')
                channel = event.get('Channel')
                usernum = event.get('Usernum')
                if meetme not in self.confrooms:
                        self.confrooms[meetme] = []
                self.confrooms[meetme].append(channel)
                print 'CONF JOIN', meetme, channel, usernum, len(self.confrooms[meetme])
                return


        def ami_meetmeleave(self, astid, event):
                """
                Function called when an AMI MeetMeLeave Event is read.
                """
                meetme = event.get('Meetme')
                channel = event.get('Channel')
                usernum = event.get('Usernum')
                if meetme in self.confrooms and channel in self.confrooms[meetme]:
                        self.confrooms[meetme].remove(channel)
                confroomsize = len(self.confrooms[meetme])
                print 'CONF LEAVE', meetme, channel, usernum, confroomsize
                if confroomsize == 2:
                        # when one of the 2 peers leave the conference, we put the 2 other ones in a regular commuication configuration
                        chantomatch1 = self.confrooms[meetme][0]
                        chantomatch2 = self.confrooms[meetme][1]
                        for opername, userinfo in self.ulist.byast[astid].list.iteritems():
                                if 'agentchannel-conf' in userinfo:
                                        chann_agent = chann_inout = None
                                        if userinfo['agentchannel-conf'] == chantomatch1:
                                                chann_agent = chantomatch1
                                                chann_inout = chantomatch2
                                        elif userinfo['agentchannel-conf'] == chantomatch2:
                                                chann_agent = chantomatch2
                                                chann_inout = chantomatch1
                                        if chann_agent is not None and chann_inout is not None:
                                                if channel in self.outgoing_calls:
                                                        thiscall = self.outgoing_calls[channel]
                                                        if not thiscall.parking: # if the agent has requested a parking, keep quiet
                                                                # set it in order not to send an AppelOpe/ reply when it will be logged back
                                                                userinfo['agentlogin-fromconf'] = None
                                                                self.amis[astid].transfer(chann_agent, 's', 'ctx-callbooster-agentlogin')
                                                                commid = thiscall.commid
                                                                if commid in userinfo['login']['calls']:
                                                                        del userinfo['login']['calls'][commid]
                                                                self.__send_msg__(userinfo, '%s,,,Annule/' % commid)
                                                                if chann_inout in self.incoming_calls:
                                                                        self.__park__(astid, self.incoming_calls[chann_inout])
                                                elif channel in self.incoming_calls:
                                                        thiscall = self.incoming_calls[channel]
                                                        if not thiscall.parking: # if the agent has requested a parking, keep quiet
                                                                # set it in order not to send an AppelOpe/ reply when it will be logged back
                                                                userinfo['agentlogin-fromconf'] = None
                                                                self.amis[astid].transfer(chann_agent, 's', 'ctx-callbooster-agentlogin')
                                                                commid = thiscall.commid
                                                                if commid in userinfo['login']['calls']:
                                                                        del userinfo['login']['calls'][commid]
                                                                self.__send_msg__(userinfo, '%s,,,Annule/' % commid)
                                                                if chann_inout in self.outgoing_calls:
                                                                        self.__park__(astid, self.outgoing_calls[chann_inout])
                elif confroomsize == 1:
                        chantomatch = self.confrooms[meetme][0]
                        for opername, userinfo in self.ulist.byast[astid].list.iteritems():
                                if 'agentchannel-conf' in userinfo:
                                        if userinfo['agentchannel-conf'] == chantomatch:
                                                userinfo['agentlogin-fromconf'] = None
                                                self.amis[astid].transfer(chantomatch, 's', 'ctx-callbooster-agentlogin')
                                                if channel in self.outgoing_calls:
                                                        commid = self.outgoing_calls[channel].commid
                                                        if commid in userinfo['login']['calls']:
                                                                del userinfo['login']['calls'][commid]
                                                        self.__send_msg__(userinfo, '%s,,,Annule/' % commid)
                return

        # END of AMI events
        #
        # XIVO synchronization methods

        def __callback_walkdir__(self, args, dirname, filenames):
                """
                Function called in order to know the presence of M018 files in a given tree.
                This is used in order to setup an MOH class in XIVO with a symlink towards these files.
                """
                if dirname[-5:] == '/Sons':
                        for filename in filenames:
                                if filename.startswith('M018.'):
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


        # INCOMING calls management

        def __incall_list__(self):
                """
                """
                print 'number by SDA = ', self.n_incoming_calls
                print 'calls         = ', self.incoming_calls
                print 'calls         = ', self.incoming_calls_byprio


        def __incall_remove__(self, call, chan):
                """
                """
                print 'removing incoming call from the list ...'
                self.incoming_calls.pop(chan)
                if call in self.incoming_calls_byprio[call.elect_prio]:
                        self.incoming_calls_byprio[call.elect_prio].remove(call)
                if call.sdanum in self.n_incoming_calls:
                        self.n_incoming_calls[call.sdanum] -= 1
                self.__incall_list__()


        def __incall_add__(self, call, chan):
                """
                """
                print 'adding incoming call into the list ...'
                self.incoming_calls[chan] = call
                if call.sdanum not in self.n_incoming_calls:
                        self.n_incoming_calls[call.sdanum] = 0
                self.n_incoming_calls[call.sdanum] += 1
                # if call.waiting:
                self.incoming_calls_byprio[call.elect_prio].append(call)
                self.__incall_list__()


        def __sendfiche_a__(self, userinfo, incall):
                """
                Sends the Fiche informations (incall) to the appropriate user (userinfo)
                """
                userinfo['login']['calls'][incall.commid] = incall
                # CLR-ACD to be sent only if there was an Indispo sent previously
                if incall.dialplan['callerid'] == 1:
                        cnum = incall.cidnum
                else:
                        cnum = ''
                incall.waiting = False
                incall.uinfo = userinfo
                self.__send_msg__(incall.uinfo, '%s,%s,%s,Fiche/' % (incall.commid, incall.sdanum, cnum))
                return


        def __sendfiche__(self, dest, incall):
                """
                The call comes here when the AGI has directly found a peer.
                """
                userinfo = self.ulist.finduser(dest)
                log_debug(SYSLOG_INFO, '__sendfiche__ : userinfo = %s' % str(userinfo))
                if userinfo is not None:
                        agentnum = userinfo['agentnum']
                        if 'agentchannel' in userinfo:
                                print 'sendfiche, the agent is online :', userinfo['agentchannel']
                        else:
                                log_debug(SYSLOG_INFO, 'sendfiche, the agent is not online ... we re going to call him : (%s)' % str(userinfo))
                                self.__schedule_agentlogin__(userinfo)
                        incall.statacd2_tt = 'TT_SOP'
                        self.__sendfiche_a__(userinfo, incall)
                        print '__sendfiche__', incall.queuename, agentnum
                        userinfo['login']['queuelist'][incall.queuename] = incall
                        userinfo['sendfiche'] = [incall.queuename, 'Agent/%s' % agentnum]
                return


        def __clear_call_fromqueues__(self, astid, incall):
                """
                Removes the 'incall' call references from all the operator queues, and
                sends the relevant information to the agents.
                """
                for opername, userinfo in self.ulist.byast[astid].list.iteritems():
                        if 'login' in userinfo and incall.queuename in userinfo['login']['queuelist']:
                                del userinfo['login']['queuelist'][incall.queuename]
                                agentnum = userinfo['agentnum']
                                print '__clear_call_fromqueues__ : removing %s (queue %s) for agent %s (%s)' %(incall.sdanum, incall.queuename, agentnum, incall.waiting)
                                if opername in incall.agentlist:
                                        incall.agentlist.remove(opername)
                                else:
                                        print opername.encode('latin1'), 'not in agentlist', incall.agentlist, '=> probably %s = False' % incall.waiting
                                if incall.waiting:
                                        self.__send_msg__(userinfo, '%s,%s,,CLR-ACD/' % (incall.commid, incall.sdanum))
                                else:
                                        if not incall.annuleraccroche:
                                                self.__send_msg__(userinfo, '%s,,,Annule/' % incall.commid)
                                self.amis[astid].queueremove(incall.queuename, 'Agent/%s' % agentnum)
                return


        def __addtoqueue__(self, astid, dest, incall):
                """
                Adds the 'dest' Agent to 'incall' call's queue.
                """
                print '__addtoqueue__', dest, incall.commid, incall.sdanum
                self.amis[astid].queueadd(incall.queuename, GHOST_AGENT)
                self.amis[astid].queuepause(incall.queuename, GHOST_AGENT, 'false')
                userinfo = self.ulist.finduser(dest)
                if userinfo is not None and 'login' in userinfo:
                        if incall.queuename not in userinfo['login']['queuelist']:
                                agentnum = userinfo['agentnum']
                                # this is the Indispo list
                                userinfo['login']['queuelist'][incall.queuename] = incall
                                incall.agentlist.append(dest)
                                self.amis[astid].queueadd(incall.queuename, 'Agent/%s' % agentnum)
                                self.amis[astid].queuepause(incall.queuename, 'Agent/%s' % agentnum, 'true')
                                self.__send_msg__(userinfo, '%s,%s,,Indispo/' % (incall.commid, incall.sdanum))
                        else:
                                print '__addtoqueue__ : %s is already in the queuelist of %s' % (incall.queuename, dest)
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
                dorecord = call.uinfo['record']

                print 'OUTCALL for destination %s' % call.dest
                self.amis[call.astid].aoriginate_var('Agent', call.agentnum, call.agentname,
                                                     call.dest,
                                                     'Appel %s' % call.dest,
                                                     'ctx-callbooster-outcall',
                                                     {'CB_OUTCALL_NUMBER' : call.dest,
                                                      'CB_OUTCALL_RECORD_FILENAME' : 'myfilenamerecord'},
                                                     100)
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


        def __schedule_agentlogin__(self, userinfo):
                """
                Initiates the timer for log in an Agent, then requests the Originate action.
                """
                userinfo['timer-agentlogin'] = threading.Timer(10, self.__callback_timer__, 'AgentLogin')
                userinfo['timer-agentlogin'].start()
                userinfo['timer-agentlogin-iter'] = 0

                astid = userinfo.get('astid')
                phonenum = userinfo.get('phonenum')
                agentnum = userinfo.get('agentnum')
                username = userinfo.get('user')
                self.amis[astid].aoriginate_var('sip', phonenum, 'Log %s' % phonenum,
                                                agentnum, username, 'ctx-callbooster-agentlogin',
                                                {'CB_MES_LOGAGENT' : username,
                                                 'CB_AGENT_NUMBER' : agentnum}, 100)


        def manage_cticommand(self, userinfo, connid_socket, parsedcommand):
                """
                Defines the actions to be proceeded according to Operat's commands.
                - Aboute       : links together 2 already established calls

                - AppelAboute  : forwards an incoming call to a given phone number

                - Appel        : initiates / originates an outgoing call from an Agent

                - AppelOpe     : calls the Agent's phone at startup (AgentLogin)

                - Alerte       : initiates a call campaign.

                - Attente      : holds a given peer, thanks to the Park() feature, used with a
                10 hours timeout. Please note that this park process takes a few steps :
                -- 'Attente' command => dial the 700 in order to Park() (parking => true)
                -- 'Unlink' is applied to the parked Channel => we must be careful not to handle it
                -- 'ParkedCall' is received => acknowledgement and callback extension setting (701 ...)
                ... and for the unpark part (i.e. 'Reprise' command) :
                -- 'Reprise' command => first checks other calls' parking statuses, parks them if needed
                -- once every call is parked, dials the right number (701 ...) to recover
                -- 'UnparkedCall' is received
                -- 'Link' is applied to the parked Channel (parking => false)

                - Change       : updates the queues and agents' statuses

                - Conf         : initiates a 3-party conference (caller, client and agent)

                - Enregistre   : records a voice message

                - ForceACD     : forces one given incoming/pending call to be chosen by the agent

                - ListConf     : lists the current call references inside a given room

                - ListeACD     : sends the list of ongoing incoming calls

                - Pause        : sets the Pause state

                - Ping         : received every 60 seconds, one replies 'Pong' and set a timer in order for
                broken connections to be tracked

                - PutConf      : sends the agent into a given conference room

                - Prêt         : according to its argument (0 / 1), sets the availability of the client.
                '0' when the client can receive a call and fiche.
                '1' when it can't anymore : this is also useful to acknowledge the fiche reception

                - Raccroche    : hangs up the defined call

                - RaccrocheOpe : hangs up the Agent's phone

                - Reprise      : the pending command for 'Attente', in order to retrieve a parked call.
                We have to be sure that other ongoing calls (if any) are in Attente mode.
                See 'Attente' command above for details.

                - Sonn         : 

                - Sortie       : leaves the FB

                - TransfertOpe : transfers a call to another operator

                XIVO-related commands :
                - SynchroXivoAgents :
                - SynchroXivoSDA :
                - SynchroXivoMOH :
                """

                cname = parsedcommand.name
                astid = userinfo.get('astid')
                opername = userinfo.get('user')
                phonenum = userinfo.get('phonenum')
                agentnum = userinfo.get('agentnum')
                requester_ip = connid_socket.getpeername()[0]
                requester_port = connid_socket.getpeername()[1]

                if userinfo.get('login').get('connection') != connid_socket:
                        log_debug(SYSLOG_WARNING, 'manage_cticommand (%s) : sockets mismatch for %s' % (cname.encode('latin1'),
                                                                                                        opername.encode('latin1')))
                        return
                log_debug(SYSLOG_INFO, 'manage_cticommand (operator %s, phonenum %s, agentnum %s @ %s:%d) : (%s, %s)'
                          % (opername.decode('latin1').encode('utf8'),
                             phonenum, agentnum,
                             requester_ip, requester_port,
                             cname.decode('latin1').encode('utf8'),
                             str(parsedcommand.args)))
                if agentnum is not None:
                        agentid = 'Agent/%s' % agentnum
                else:
                        print '--- no agentnum defined in userinfo'

                if cname == 'AppelOpe':
                        if len(parsedcommand.args) == 2:
                                # the phonenum comes from the first Init/ command, therefore doesn't need
                                # to be fetched from the 'postes' table
                                if 'agentchannel' in userinfo:
                                        log_debug(SYSLOG_WARNING, 'AppelOpe : %s phone is already logged in as agent number %s' % (phonenum, agentnum))
                                self.__schedule_agentlogin__(userinfo)
                        else:
                                reply = ',-3,,AppelOpe/'
                                connid_socket.send(reply)


                elif cname == 'RaccrocheOpe':
                        os.popen('asterisk -rx "agent logoff %s"' % agentid)
                        # stat_acd2


                elif cname == 'TransfertOpe':
                        mreference = parsedcommand.args[1]
                        [reference, nope, ncol] = mreference.split('|')
                        if reference in userinfo['login']['calls']:
                                cchan = userinfo['login']['calls'][reference].peerchannel

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


                elif cname == 'ListeACD':
                        lst = []
                        for anycommid, anycall in userinfo['login']['calls'].iteritems():
                                if anycall.dir == 'i':
                                        lst.append(anycall.commid)
                        connid_socket.send(',%s,,ListeACD/' % (';'.join(lst)))


                elif cname == 'ForceACD':
                        """
                        ForceACD is used in order for an Agent to choose the Incoming Call he wants to treat.
                        """
                        reference = parsedcommand.args[1]
                        if reference in userinfo['login']['calls']:
                                print 'ForceACD with ref =', reference
                        elif reference == '0':
                                qlist = userinfo['login']['queuelist']
                                if len(qlist) > 0:
                                        qlidx = qlist.keys()[0]
                                        calltoforce = qlist[qlidx]
                                        calltoforce.forceacd = [userinfo, qlidx, 'Agent/%s' % agentnum]
                                        # remove the call from the queuelist and set it into the call list

                                        # park the current calls
                                        ntowait = 0
                                        for callnum, anycall in userinfo['login']['calls'].iteritems():
                                                ntowait += self.__park__(astid, anycall)

                                        userinfo['login']['calls'][calltoforce.commid] = calltoforce
                                        if ntowait == 0:
                                                if 'agentchannel' not in userinfo:
                                                        self.__schedule_agentlogin__(userinfo)
                                                userinfo['sendfiche'] = [qlidx, 'Agent/%s' % agentnum]
                                                if calltoforce.dialplan['callerid'] == 1:
                                                        cnum = calltoforce.cidnum
                                                else:
                                                        cnum = ''
                                                self.__send_msg__(userinfo, '%s,%s,%s,Fiche/' % (calltoforce.commid, calltoforce.sdanum, cnum))
                                                self.__send_msg__(userinfo, '%s,%s,,CLR-ACD/' % (calltoforce.commid, calltoforce.sdanum))
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

                        if len(userinfo['login']['calls']) > 0:
                                print 'Appel (call number will be %s) : there are already ongoing calls' % comm_id_outgoing, userinfo['login']['calls']
                                ntowait = 0
                                for callnum, anycall in userinfo['login']['calls'].iteritems():
                                        ntowait += self.__park__(astid, anycall)
                                userinfo['login']['calls'][comm_id_outgoing] = outCall
                                if ntowait > 0:
                                        userinfo['login']['calls'][comm_id_outgoing].tocall = True
                                else:
                                        self.__outcall__(outCall)
                        else:
                                print 'Appel (call number will be %s)' % comm_id_outgoing, userinfo
                                if 'agentchannel' in userinfo:
                                        userinfo['login']['calls'][comm_id_outgoing] = outCall
                                        self.__outcall__(outCall)
                                else:
                                        userinfo['login']['calls'][comm_id_outgoing] = outCall
                                        userinfo['login']['post_agentlogin_outcall'] = comm_id_outgoing
                                        self.__schedule_agentlogin__(userinfo)


                elif cname == 'Raccroche':
                        reference = parsedcommand.args[1]
                        try:
                                if reference in userinfo['login']['calls']:
                                        # END OF INCOMING OR OUTGOING CALL
                                        anycall = userinfo['login']['calls'].pop(reference)
                                        print 'Raccroche', anycall, anycall.appelaboute, anycall.parking, anycall.parkexten, anycall.peerchannel
                                        if anycall.peerchannel is None:
                                                self.amis[astid].hangup(agentid, '')
                                        else:
                                                self.amis[astid].hangup(anycall.peerchannel, '')
                                        try:
                                                if anycall.dir == 'i':
                                                        self.__update_stat_acd2__(anycall)
                                                self.__update_taxes__(anycall, ENDCALLSTATUS_TAXES)
                                        except Exception, exc:
                                                print '--- exception --- (Raccroche) :', exc

                                        retval = 1
                                        reply = '%s,%d,,Raccroche/' % (reference, retval)
                                        anycall.annuleraccroche = True
                                        connid_socket.send(reply)
                                else:
                                        # log_debug(SYSLOG_WARNING, '')
                                        pass
                        except Exception, exc:
                                print '--- exception --- in Raccroche treatment ...', exc


                elif cname == 'PutConf':
                        confnum = parsedcommand.args[1]
                        refcomm = parsedcommand.args[2]
                        # warning : we might need to park one peer before

                        if refcomm in userinfo['login']['calls']:
                                anycall = userinfo['login']['calls'][refcomm]
                                self.amis[astid].setvar(anycall.peerchannel, 'CB_CONFNUM', confnum)
                                self.amis[astid].transfer(anycall.peerchannel, 's', 'ctx-callbooster-conf')
                                reply = '%s,1,,PutConf/' % refcomm
                                connid_socket.send(reply)


                elif cname == 'ListConf':
                        confnum = parsedcommand.args[1]
                        listconf = []
                        reply = '%s,%s,,ListConf/' % (refcomm, ';'.join(listconf))
                        connid_socket.send(reply)


                elif cname == 'Conf':
                        reference = parsedcommand.args[1]
                        [refcomm_out, refcomm_in] = reference.split('|')
                        print 'Conf', opername, refcomm_out, refcomm_in
                        # warning : we might need to park one peer before

                        confnum = refcomm_in
                        if refcomm_in in userinfo['login']['calls']:
                                anycall = userinfo['login']['calls'][refcomm_in]
                                self.amis[astid].setvar(anycall.peerchannel, 'CB_CONFNUM', confnum)
                                self.amis[astid].transfer(anycall.peerchannel, 's', 'ctx-callbooster-conf')
                                anycall.parking = False # in order to enable its repark
                        if refcomm_out in userinfo['login']['calls']:
                                anycall = userinfo['login']['calls'][refcomm_out]
                                self.amis[astid].setvar(anycall.peerchannel, 'CB_CONFNUM', confnum)
                                self.amis[astid].transfer(anycall.peerchannel, 's', 'ctx-callbooster-conf')
                                anycall.parking = False # in order to enable its repark

                        userinfo['conf'] = confnum
                        if 'agentchannel' in userinfo:
                                self.amis[astid].setvar(userinfo['agentchannel'], 'CB_CONFNUM', confnum)
                                self.amis[astid].transfer(userinfo['agentchannel'], 's', 'ctx-callbooster-conf')
                                connid_socket.send('%s|%s,1,,Conf/' % (refcomm_out, refcomm_in))
                        else:
                                log_debug(SYSLOG_WARNING, 'WARNING : agentchannel field not in the current userinfo : %s' % str(userinfo))


                elif cname == 'Aboute':
                        reference = parsedcommand.args[1]
                        [refcomm_out, refcomm_in] = reference.split('|')
                        print 'Aboute', opername, refcomm_out, refcomm_in

                        in_callbackexten = None
                        out_callbackexten = None
                        if refcomm_in in userinfo['login']['calls']:
                                incall = userinfo['login']['calls'][refcomm_in]
                                in_callbackexten = incall.parkexten
                                in_chan = incall.peerchannel
                        if refcomm_out in userinfo['login']['calls']:
                                outcall = userinfo['login']['calls'][refcomm_out]
                                out_callbackexten = outcall.parkexten
                                out_chan = outcall.peerchannel
                                incall.aboute = refcomm_out

                        if in_callbackexten is not None:
                                self.amis[astid].transfer(out_chan, in_callbackexten, 'default')
                        elif out_callbackexten is not None:
                                self.amis[astid].transfer(in_chan, out_callbackexten, 'default')
                        else:
                                log_debug(SYSLOG_WARNING, 'WARNING : could not Aboute %s and %s' % (refcomm_in, refcomm_out))


                elif cname == 'AppelAboute': # transfer
                        mreference = parsedcommand.args[1]
                        [dest, refcomm_in, idsoc, idcli, idcol] = mreference.split('|')

                        time.sleep(0.2)
                        self.commidcurr += 1
                        comm_id_outgoing = str(self.commidcurr)

                        if refcomm_in in userinfo['login']['calls']:
                                incall = userinfo['login']['calls'][refcomm_in]
                                print "AppelAboute", incall.dir, incall.peerchannel
                                incall.appelaboute = dest
                                incall.set_timestamp_stat('appelaboute')
                                r = self.amis[astid].transfer(incall.peerchannel, dest, 'default')

                                socname = self.__socname__(idsoc)
                                outCall = OutgoingCall.OutgoingCall(comm_id_outgoing, astid, self.cursor_operat, socname,
                                                                    userinfo, agentnum, opername, dest,
                                                                    self.__local_nsoc__(idsoc), idcli, idcol)
                                userinfo['login']['calls'][comm_id_outgoing] = outCall
                                reply = '%s,1,,Attente/' % refcomm_in
                                connid_socket.send(reply)


                elif cname == 'Attente':
                        reference = parsedcommand.args[1]
                        if reference in userinfo['login']['calls']:
                                anycall = userinfo['login']['calls'][reference]
                                self.__park__(astid, anycall)
                        else:
                                log_debug(SYSLOG_WARNING, 'Attente : the requested reference %s does not exist' % reference)


                elif cname == 'Reprise':
                        reference = parsedcommand.args[1]
                        if reference in userinfo['login']['calls']:
                                thiscall = userinfo['login']['calls'][reference]
                                if thiscall.parking:
                                        callbackexten = thiscall.parkexten
                                        if callbackexten is not None:
                                                ntowait = 0
                                                for callnum, anycall in userinfo['login']['calls'].iteritems():
                                                        if anycall != thiscall:
                                                                print 'Reprise', anycall
                                                                ntowait += self.__park__(astid, anycall)
                                                if ntowait > 0:
                                                        log_debug(SYSLOG_INFO, 'Reprise : we have to park %d calls before' % ntowait)
                                                        thiscall.toretrieve = callbackexten
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
                                if not isalive:
                                        log_debug(SYSLOG_WARNING, 'timer-ping : received a Ping but the timer was out ... should have been removed when out')
                        else:
                                log_debug(SYSLOG_INFO, 'timer-ping : first setup for this connection')

                        timer = threading.Timer(TIMEOUTPING, self.__callback_timer__, 'Ping')
                        timer.start()
                        userinfo['timer-ping'] = timer


                elif cname == 'Sortie':
                        if 'timer-ping' in userinfo:
                                userinfo['timer-ping'].cancel()
                                del userinfo['timer-ping']
                        userinfo['login']['cbstatus'] = 'Sortie'
                        self.__choose_and_queuepush__(astid, cname)


                elif cname == 'Change':
                        self.__choose_and_queuepush__(astid, cname)


                elif cname == 'Pause':
                        userinfo['login']['cbstatus'] = 'Pause'


                elif cname == 'Prêt' or cname == 'Sonn':
                        if cname == 'Prêt':
                                reference = parsedcommand.args[1]
                                if userinfo['login']['cbstatus'] == 'Pause':
                                        # XXX send current Indispo's to the user ?
                                        pass
                                userinfo['login']['cbstatus'] = 'Pret' + reference
                        elif cname == 'Sonn':
                                userinfo['login']['cbstatus'] = 'Sonn'
                                reference = '1'

                        if reference == '1':
                                if 'sendfiche' in userinfo:
                                        print 'sendfiche / Pret1', userinfo['sendfiche']
                                        [qname, agname] = userinfo['sendfiche']
                                        self.amis[astid].queueadd(qname, agname)
                                        self.amis[astid].queuepause(qname, agname, 'false')
                                        print 'fiche has been sent :', time.time()
                                        del userinfo['sendfiche']
                                return

                        self.__choose_and_queuepush__(astid, 'Pret')


                elif cname == 'Enregistre':
                        reference = parsedcommand.args[1]

                        if 'agentchannel' in userinfo:
                                print 'sendfiche, the agent is online :', userinfo['agentchannel']
                                self.amis[astid].aoriginate_var('Agent', agentnum, opername,
                                                                'record_exten', 'Enregistre', 'ctx-callbooster-record',
                                                                {'CB_RECORD_FILENAME' : reference[0]}, 100)
                                connid_socket.send(',,,Enregistre/')
                        else:
                                log_debug(SYSLOG_INFO, 'sendfiche, the agent is not online ... we re going to call him : %s (%s)' % (phonenum, str(userinfo)))
                                userinfo['login']['post_agentlogin_record'] = reference
                                self.__schedule_agentlogin__(userinfo)


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

                        columns = ('N', 'NAlerteStruct', 'NomFichierMessage', 'ListeGroupes', 'Etat', 'NextSuivi', 'type_suivi', 'dest_suivi', 'interval_suivi')
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

                                # Alerte command => fetch details
                                log_debug(SYSLOG_INFO, 'Alerte : %s' % str(rr))

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
                To be called any time one would like to update the queues' status.
                The choice is done in 2 steps.
                * call to __choose__ in order to know which action is associated to which operator
                * application of these actions
                This is done in 2 steps in order to avoid that some actions decided in the first one would
                change future analysis during its own processing.
                """
                try:
                        todo = self.__choose__(astid)
                        print '__choose_and_queuepush__ : CHOOSE (after %s)' % info_whocalled, todo
                        for opername_iter, couplelist in todo.iteritems():
                                for td in couplelist:
                                        [queueaction, queuecall] = td
                                        if queueaction == 'push':
                                                print 'manage : sending fiche to %s (%s)' % (opername_iter, info_whocalled)
                                                self.__clear_call_fromqueues__(astid, queuecall)
                                                self.__sendfiche__(opername_iter, queuecall)
                                        elif queueaction == 'enqueue':
                                                print 'manage : enqueue %s (%s)' % (opername_iter, info_whocalled)
                                                self.__addtoqueue__(astid, opername_iter, queuecall)
                                        elif queueaction == 'dequeue':
                                                print 'manage : dequeue %s (%s)' % (opername_iter, info_whocalled)
                                                userinfo = self.ulist.finduser(opername_iter)
                                                queuecall.agentlist.remove(opername_iter)
                                                self.amis[astid].queueremove(queuecall.queuename, 'Agent/%s' % userinfo['agentnum'])
                                                nagents = len(queuecall.agentlist)
                                                print 'nagents = %d' % nagents
                                                if nagents == 0:
                                                        self.amis[astid].queueremove(queuecall.queuename, GHOST_AGENT)
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
                                # dstnum = LOCNUMS[al['status']] # XXX for testing purposes only !!
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


        def handle_outsock(self, astid, msg):
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
                        for chan, ic in self.incoming_calls.iteritems():
                                if ic.commid == commid and ic.sdanum == sdanum:
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
                                                        timer = threading.Timer(5, self.__callback_timer__, 'SV')
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
                        for chan, ic in self.incoming_calls.iteritems():
                                if ic.commid == commid :
                                        iic = ic
                                        break
                        if iic is not None:
                                timer = threading.Timer(5, self.__callback_timer__, 'SV')
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
                        if tname.startswith('Ping'):
                                print 'checkqueue (Ping)', tname
                                for astid in self.ulist.byast:
                                        for opername, userinfo in self.ulist.byast[astid].list.iteritems():
                                                if 'timer-ping' in userinfo and userinfo['timer-ping'] == thisthread:
                                                        agentnum = userinfo['agentnum']
                                                        self.cursor_operat.query('USE agents')
                                                        self.cursor_operat.query("UPDATE acd SET Etat = 'Sortie' WHERE NOPE = %d"
                                                                                 % (int(agentnum) - STARTAGENTNUM))
                                                        self.conn_operat.commit()
                                                        self.__choose_and_queuepush__(astid, 'Unping')
                                                        del userinfo['timer-ping']
                                                        disconnlist.append(userinfo)
                        elif tname.startswith('SV'):
                                print 'checkqueue (SV)', tname
                                for chan, ic in self.incoming_calls.iteritems():
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
                        elif tname.startswith('AgentLogin'):
                                print 'checkqueue (AgentLogin)', tname
                                for astid in self.ulist.byast:
                                        for opername, userinfo in self.ulist.byast[astid].list.iteritems():
                                                if 'timer-agentlogin' in userinfo and userinfo['timer-agentlogin'] == thisthread:
                                                        if userinfo['timer-agentlogin-iter'] < 2:
                                                                self.__send_msg__(userinfo, ',-2,,AppelOpe/')
                                                                userinfo['timer-agentlogin'] = threading.Timer(10, self.__callback_timer__, 'AgentLogin')
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


        def __callback_timer__(self, what):
                """
                This function is called :
                - when 2 mins have elapsed since the previously received ping.
                - once the 5 seconds have elapsed after an SV request.
                - once the 10 seconds after agent login attempt have been spent.
                """
                thisthread = threading.currentThread()
                tname = thisthread.getName()
                print '__callback_timer__ (timer finished)', time.asctime(), tname, what
                thisthread.setName('%s-%s' % (what, tname))
                self.tqueue.put(thisthread)
                os.write(self.queued_threads_pipe[1], what)
                return


        # Taxes and statistics

        def __init_taxes__(self, call, numbertobill, fromN, toN, fromS, toS, NOpe):
                """
                Fills the 'taxes' table at the start of a call.
                """
                try:
                        if call.dir == 'o':
                                [juridict, impulsion] = self.__gettaxes__(numbertobill)
                                call.settaxes(impulsion)
                        else:
                                juridict = 0
                                impulsion = [0, 0, 0]
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
                        if anycall.dir == 'o':
                                [tpc, dpc, dlt] = anycall.taxes
                                if duree_int >= dpc and dlt > 0:
                                        ntaxes = tpc + 1 + (duree_int - dpc) / dlt
                                else:
                                        ntaxes = tpc
                        elif anycall.dir == 'i':
                                ntaxes = 0

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

                        print '__update_taxes__ : commid = %s, state = %s, ' \
                              'durees = (%d, sonn = %d), taxes = %d' \
                              %(anycall.commid, state,
                                duree_int, dureesonnerie, ntaxes)

                        self.cursor_operat.query('USE system')
                        self.cursor_operat.query('UPDATE taxes SET Duree = %d, DureeSonnerie = %d, Etat = %s, nbTaxes = %d '
                                                 'WHERE N = %d'
                                                 % (duree_int, dureesonnerie, '"%s"' % state, ntaxes, anycall.insert_taxes_id))
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
                                [nnc, nnv, nhdv, nv,
                                 ttime, 
                                 ntt_raf, ntt_asd, ntt_snd, ntt_sfa, ntt_sop,
                                 ntt_tel, ntt_fic, ntt_bas, ntt_mes, ntt_rep,
                                 nsoc] = [0] * 16

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
                        if 'login' in incall.uinfo:
                                print '__update_stat_acd2__ : uinfo calls', incall.uinfo['login']['calls']
                                if incall.commid in incall.uinfo['login']['calls']:
                                        del incall.uinfo['login']['calls'][incall.commid]
                        opername = incall.uinfo['user']
                else:
                        opername = ''
                
                dec = 0

                [tacd, tope, tatt, tattabo, tabo,
                 ttel, trep, tmes, tsec,    tdec] = [0] * 10

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

        # miscellaneous infos

        def __outputline__(self, connid, line):
                connid.send('%s: %s\n' % (self.xdname, line))

        def cliaction(self, connid, command):
                if command == 'callbooster: whatsup':
                        self.__outputline__(connid, 'incoming calls')
                        self.__outputline__(connid, self.incoming_calls)

        # Taxes and statistics - END
        

        def __listqueues__(self):
                """
                Returns the unsorted list of currently used queues.
                """
                l = []
                for chan, icall in self.incoming_calls.iteritems():
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


        def __choose__(self, astid):
                """
                This is the main entry to choose among availabilities.
                It has to be called after most events, like Login, Change, Pret's, Logoff, new incoming/outgoing calls.

                The choice is done, according to current (unaffected yet) incoming calls and logged in/out agents,
                whether one has to wait or push.

                We need a '2-dimensional' main loop here :
                * the first 'dimension' is over the SDA's, in order for their properties to be correctly handled
                * the second 'dimension' is over the operators and rooms
                """
                myidx = None
                mycall = None
                todo_by_oper = {}
                todo_by_call = {}
                time.sleep(0.1) # wait in order for the database values to be compliant with the state change
                dt1 = time.time()
                for sdaprio in xrange(6):
                        for incall in self.incoming_calls_byprio[sdaprio]:
                                if incall.waiting:
                                        # choose the operator or the list of queues for this incoming call
                                        topush = {}
                                        to_enqueue = []

                                        for opername in incall.list_operators:
                                                if opername not in todo_by_oper:
                                                        todo_by_oper[opername] = []
                                                opstatus = incall.check_operator_status(opername)
                                                print '__choose__ : (SDA prio = %d) <%s> (%s)' % (sdaprio, opername.encode('latin1'), opstatus)
                                                userinfo = self.ulist.finduser(opername)
                                                if 'login' in userinfo:
                                                        userqueuesize = len(userinfo['login']['queuelist'])
                                                else:
                                                        userqueuesize = 0
                                                if opstatus is not None:
                                                        if 'login' in userinfo and 'connection' in userinfo.get('login'):
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
                                                                if incall.queuename in userinfo['login']['queuelist']:
                                                                        todo_by_oper[opername].append(['dequeue', incall])

                                        # for nid, svirtparams in thiscall.list_svirt.iteritems():



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
                dt2 = time.time()
                return todo_by_oper


        def handle_agi(self, astid, msg):
                """
                Called by main loop when an AGI is received
                """
                msgforagi = None
                re_push = re.match('PUSH <(\S+)> <(\S+)> <(\S+)> <(\S+)>', msg.strip())
                if re_push != None:
                        inchannel = re_push.group(1)
                        cidnum    = re_push.group(2)
                        callednum = re_push.group(3)
                        prevupto  = re_push.group(4)
                        print 'INCOMING CALL ## PUSH received #', inchannel, cidnum, callednum, prevupto
                        whattodo = self.__elect__(astid, inchannel, cidnum, callednum, prevupto)
                        print 'INCOMING CALL ## sending pickled reply to AGI'
                        msgforagi = 'PULL_START %s PULL_STOP' % pickle.dumps(whattodo)
                return msgforagi


        def __elect__(self, astid, inchannel, cidnum, callednum, upto):
                """
                Called by an Incoming Call in order to find the proper agent.
                It is actually called once the AGI has given some basic informations.
                When the call comes first, it instantiates an IncomingCall object, which will be updated later on,
                according to the dialplan choices.
                """
                action = 'exit'
                delay = 0
                value = ''
                sdanum = callednum[-4:]
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

                        self.__init_taxes__(thiscall, cidnum, cidnum, callednum, TAXES_TRUNKNAME, TAXES_CTI, 0)

                        if thiscall.statacd2_state == 'V':
                                print ' NCOMING CALL ## calling get_sda_profiles ##'
                                self.__clear_call_fromqueues__(astid, thiscall)
                                ret = thiscall.get_sda_profiles(self.n_incoming_calls.get(sdanum))
                                if ret == True:
                                        self.__incall_add__(thiscall, inchannel)
                                else:
                                        self.__update_taxes__(thiscall, ENDCALLSTATUS_TAXES)
                                        self.__update_stat_acd2__(thiscall)
                                        thiscall = None
                        else:
                                self.__update_taxes__(thiscall, ENDCALLSTATUS_TAXES)
                                self.__update_stat_acd2__(thiscall)
                                thiscall = None
                else:
                        if inchannel in self.incoming_calls:
                                thiscall = self.incoming_calls[inchannel]

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

                                """
                                We set the Ghost Agent into the queue, in order not to leave right now.
                                """
                                self.amis[astid].queueadd(thiscall.queuename, GHOST_AGENT)
                                self.amis[astid].queuepause(thiscall.queuename, GHOST_AGENT, 'false')
                                # once all the queues have been spanned, send the push / queues where needed
                                argument = 'welcome'
                                nevt = 0
                                for opername_iter, couplelist in todo.iteritems():
                                        for td in couplelist:
                                                print 'td', td
                                                [queueaction, queuecall] = td
                                                log_debug(SYSLOG_INFO, 'elect : secretariat / %s / %s / %s' % (opername_iter, queueaction, queuecall.sdanum))
                                                if queueaction == 'push':
                                                        # if thiscall == queuecall # commented since it can occur that an other call can be chosen (after ACD level > 1)
                                                        nevt += 1
                                                        self.__clear_call_fromqueues__(astid, queuecall)
                                                        self.__sendfiche__(opername_iter, queuecall)
                                                        delay = 100
                                                        argument = None
                                                elif queueaction == 'enqueue':
                                                        nevt += 1
                                                        self.__addtoqueue__(astid, opername_iter, queuecall)
                                                elif queueaction == 'dequeue':
                                                        print 'dequeue after Incall ???', opername_iter, td
                                                        pass
                                if nevt == 0:
                                        if len(thiscall.list_svirt) == 0:
                                                action = 'noqueue'
                        elif action == 'exit':
                                self.__update_taxes__(thiscall, ENDCALLSTATUS_TAXES)
                                self.__update_stat_acd2__(thiscall)
                        else:
                                print '### action is <%s> which I don t know, exiting anyway' % action
                                self.__update_taxes__(thiscall, ENDCALLSTATUS_TAXES)
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

        def ami_rename(self, astid, event):
                print event
                return None

xivo_commandsets.CommandClasses['callbooster'] = CallBoosterCommand
