# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007-2009 Proformatique'
__author__    = 'Corentin Le Gall'

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

"""
Asterisk AMI utilities.
"""

import logging
import random
import re
import socket
import string
import time

__alphanums__ = string.uppercase + string.lowercase + string.digits
__dialallowed__ = '[0-9*#+]'

log = logging.getLogger('xivo_ami')

## \class AMIClass
# AMI definition in order to interact with the Asterisk AMI.
class AMIClass:
        class AMIError(Exception):
                def __init__(self, msg):
                    self.msg = msg
                def __str__(self):
                    return self.msg

        # \brief Class initialization.
        def __init__(self, address, loginname, password, events):
                self.address   = address
                self.loginname = loginname
                self.password  = password
                self.events    = events
                self.i = 1
                self.aorgcmd = 'AOriginate'
                self.actionid = None

        def set_aoriginate(self, aoriginatecmd):
                self.aorgcmd = aoriginatecmd

        # \brief Connection to a socket.
        def connect(self):
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(self.address)
                s.settimeout(30)
                self.fd = s.makefile('rw', 0)
                s.close()
                return
        
        # \brief Sending any AMI command.
        def sendcommand(self, action, args, loopnum = 0):
                ret = False
                try:
                        self.fd.write('Action: %s\r\n' % action)
                        for (name, value) in args:
                                try:
                                        towrite = '%s: %s\r\n' % (name, value)
                                        self.fd.write(towrite)
                                except Exception:
                                        log.exception('(sendcommand %s : %s = %s (%r))'
                                                      % (action, name, value, value))
                        if self.actionid:
                                self.fd.write('ActionId: %s\r\n' % self.actionid)
                                self.actionid = None
                        self.fd.write('\r\n')
                        self.fd.flush()
                        ret = True
                except Exception:
                        log.exception('(action %s)' % action)
                        ret = False
                if ret == False:
                        if loopnum == 0:
                                log.warning('second attempt for AMI command (%s)' % action)
                                # tries to reconnect
                                try:
                                        self.connect()
                                        self.login()
                                        if self:
                                                # "retrying AMI command=<%s> args=<%s>" % (action, str(args)))
                                                self.sendcommand(action, args, 1)
                                except Exception:
                                        # log.exception("AMI not connected (action=%s args=%s)" %(action, args))
                                        pass
                        else:
                                log.warning('warning : 2 attempts have failed for AMI command (%s)' % action)
                return ret
        def setactionid(self, actionid):
                self.actionid = actionid
                return
        # \brief Requesting a Status.
        def sendstatus(self):
                ret = self.sendcommand('Status', [])
                return ret
        # \brief Requesting the Agents' Status.
        def sendagents(self):
                ret = self.sendcommand('Agents', [])
                return ret
        # \brief Requesting the Queues' Status.
        def sendqueuestatus(self, queue = None):
                if queue is None:
                        ret = self.sendcommand('QueueStatus', [])
                else:
                        ret = self.sendcommand('QueueStatus',
                                               [('Queue', queue)])
                return ret
        # \brief Requesting an ExtensionState.
        def sendextensionstate(self, exten, context):
                ret = self.sendcommand('ExtensionState',
                                       [('Exten', exten),
                                        ('Context', context)])
                return ret
        def sendparkedcalls(self):
                ret = self.sendcommand('ParkedCalls', [])
                return ret
        
        def sendmeetmelist(self):
                ret = self.sendcommand('MeetMeList', [])
                return ret
        
        # \brief For debug.
        def printresponse_forever(self):
                while True:
                        str = self.fd.readline()
                        self.i = self.i + 1
        # \brief Reads a part of a reply.
        def readresponsechunk(self):
                start = True
                list = []
                while True:
                        str = self.fd.readline()
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
                        ret = False
                        if self.events:
                                ret = self.sendcommand('login',
                                                       [('Username', self.loginname),
                                                        ('Secret', self.password),
                                                        ('Events', 'on')])
                        else:
                                ret = self.sendcommand('login',
                                                       [('Username', self.loginname),
                                                        ('Secret', self.password),
                                                        ('Events', 'off')])
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False

        # \brief Executes a CLI command.
        def execclicommand(self, command):
                # special procession for cli commands.
                self.sendcommand('Command',
                                 [('Command', command)])
                resp = []
                for i in (1, 2):
                        str = self.fd.readline()
                while True:
                        str = self.fd.readline()
                        #print self.i, len(str), str,
                        self.i = self.i + 1
                        if str == '\r\n' or str == '' or str == '--END COMMAND--\r\n':
                                break
                        resp.append(str)
                return resp

        # \brief Hangs up a Channel.
        def hangup(self, channel, channel_peer = None):
                ret = 0
                try:
                        self.sendcommand('Hangup',
                                         [('Channel', channel)])
                        ret += 1
                except self.AMIError:
                        pass
                except Exception:
                        pass
                
                if channel_peer:
                        try:
                                self.sendcommand('Hangup',
                                                 [('Channel', channel_peer)])
                                ret += 2
                        except self.AMIError:
                                pass
                        except Exception:
                                pass
                return ret


        def setvar(self, var, val, chan = None):
                try:
                        if chan is None:
                                ret = self.sendcommand('Setvar', [('Variable', var),
                                                                  ('Value', val)])
                        else:
                                ret = self.sendcommand('Setvar', [('Channel', chan),
                                                                  ('Variable', var),
                                                                  ('Value', val)])
                except self.AMIError:
                        ret = False
                except Exception:
                        ret = False
                return ret
        
        # Asterisk >= 1.4
        def park(self, channel, channel_timeout):
                try:
                        ret = self.sendcommand('Park', [('Channel', channel),
                                                        ('Channel2', channel_timeout)])
                        return ret
                except self.AMIError:
                        ret = False
                except Exception:
                        ret = False
                return ret
        
        def origapplication(self, application, data, phoneproto, phonesrc, context):
                try:
                        ret = self.sendcommand('Originate', [('Channel', phoneproto + '/' + phonesrc),
                                                             ('Context', context),
                                                             ('Priority', '1'),
                                                             ('Application', application),
                                                             ('Data', data),
                                                             ('Variable', 'XIVO_ORIGACTIONID=%s' % self.actionid),
                                                             ('Variable', 'XIVO_ORIGAPPLI=%s' % application),
                                                             ('Async', 'true')])
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False

        # \brief Originates a call from a phone towards another.
        def originate(self, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext, extravars = {}, timeout = 3600):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                ph = re.sub(__dialallowed__, '', phonedst)
                if len(ph) > 0:
                        return False
                try:
                        command_details = [('Channel', phoneproto + '/' + phonesrc),
                                           ('Exten', phonedst),
                                           ('Context', locext),
                                           ('Priority', '1'),
                                           ('Timeout', str(timeout * 1000)),
                                           ('CallerID', '"%s"<%s>' % (cidnamedst, phonedst)),
                                           ('Variable', 'XIVO_ORIGSRCNAME=%s' % cidnamesrc),
                                           ('Variable', 'XIVO_ORIGSRCNUM=%s'  % phonesrc),
                                           ('Variable', 'XIVO_ORIGACTIONID=%s' % self.actionid),
                                           ('Variable', 'XIVO_ORIGAPPLI=%s' % 'OrigDial'),
                                           ('Async', 'true')]
                        for var, val in extravars.iteritems():
                                command_details.append(('Variable', '%s=%s'  % (var, val)))
                        ret = self.sendcommand('Originate', command_details)
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False
                
        # \brief Originates a call from a phone towards another.
        def aoriginate(self, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext, extravars = {}):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                ph = re.sub(__dialallowed__, '', phonedst)
                if len(ph) > 0:
                        return False
                try:
                        #print self.aorgcmd, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext
                        command_details = [('Channel', phoneproto + '/' + phonesrc),
                                           ('Exten', phonedst),
                                           ('Context', locext),
                                           ('Priority', '1'),
                                           ('CallerID', '"%s"<%s>' % (cidnamedst, phonedst)),
                                           ('Variable', 'XIVO_ORIGSRCNAME=%s' % cidnamesrc),
                                           ('Variable', 'XIVO_ORIGSRCNUM=%s'  % phonesrc),
                                           ('Variable', 'XIVO_ORIGACTIONID=%s' % self.actionid),
                                           ('Variable', 'XIVO_ORIGAPPLI=%s' % 'OrigDial'),
                                           ('Async', 'true')]
                        for var, val in extravars.iteritems():
                                command_details.append(('Variable', '%s=%s'  % (var, val)))
                        ret = self.sendcommand(self.aorgcmd, command_details)
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False
                
        # \brief Originates a call from a phone towards another.
        def aoriginate_var(self, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext, extravars, timeout):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                ph = re.sub(__dialallowed__, '', phonedst)
                if len(ph) > 0:
                        return False
                try:
                        #print self.aorgcmd, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext
                        command_details = [('Channel', phoneproto + '/' + phonesrc),
                                           ('Exten', phonedst),
                                           ('Context', locext),
                                           ('Priority', '1'),
                                           ('Timeout', str(timeout * 1000)),
                                           ('CallerID', '"%s"<%s>' %(cidnamedst, phonedst)),
                                           ('Variable', 'XIVO_ORIGSRCNAME=%s' % cidnamesrc),
                                           ('Variable', 'XIVO_ORIGACTIONID=%s' % self.actionid),
                                           ('Variable', 'XIVO_ORIGAPPLI=%s' % 'OrigDial'),
                                           ('Async', 'true')]
                        if 'XIVO_ORIGSRCNUM' not in extravars:
                                extravars['XIVO_ORIGSRCNUM'] = phonesrc
                        for var, val in extravars.iteritems():
                                command_details.append(('Variable', '%s=%s'  % (var, val)))
                        ret = self.sendcommand(self.aorgcmd, command_details)
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False

        # \brief Requests the Extension Statuses
        def extensionstate(self, extension, context):
                try:
                        ret = self.sendcommand('ExtensionState', [('Exten', extension),
                                                                  ('Context', context)])
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False

        # \brief Logs in an Agent
        def agentcallbacklogin(self, agentnum, extension, context, ackcall):
                try:
                        ret = self.sendcommand('AgentCallbackLogin', [('Agent', agentnum),
                                                                      ('Context', context),
                                                                      ('Exten', extension),
                                                                      ('AckCall' , ackcall)])
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False

        # \brief Logs off an Agent
        def agentlogoff(self, agentnum):
                try:
                        ret = self.sendcommand('AgentLogoff', [('Agent', agentnum)])
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False

        # \brief Adds a Queue
        def queueadd(self, queuename, interface, paused):
                try:
                        ret = self.sendcommand('QueueAdd', [('Queue', queuename),
                                                            ('Interface', interface),
                                                            ('Penalty', '1'),
                                                            ('Paused', paused)])
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False

        # \brief Removes a Queue
        def queueremove(self, queuename, interface):
                try:
                        ret = self.sendcommand('QueueRemove', [('Queue', queuename),
                                                               ('Interface', interface)])
                except self.AMIError:
                        ret = False
                except Exception:
                        ret = False
                return ret
        
        # \brief (Un)Pauses a Queue
        def queuepause(self, queuename, interface, paused):
                try:
                        ret = self.sendcommand('QueuePause', [('Queue', queuename),
                                                              ('Interface', interface),
                                                              ('Paused', paused)])
                except self.AMIError:
                        ret = False
                except Exception:
                        ret = False
                return ret
        
        # \brief Requests the Mailbox informations
        def mailbox(self, phone, context):
                try:
                        ret1 = self.sendcommand('MailboxCount', [('Mailbox', '%s@%s' % (phone, context))])
                        ret2 = self.sendcommand('MailboxStatus', [('Mailbox', '%s@%s' % (phone, context))])
                        ret = ret1 and ret2
                except self.AMIError:
                        ret = False
                except Exception:
                        ret = False
                return ret
        
        # \brief Starts monitoring a channel
        def monitor(self, channel, filename):
                try:
                        ret = self.sendcommand('Monitor',
                                               [('Channel', channel),
                                                ('File', filename),
                                                ('Mix', 'true')])
                except self.AMIError:
                        ret = False
                except Exception:
                        ret = False
                return ret
        
        # \brief Stops monitoring a channel
        def stopmonitor(self, channel):
                try:
                        ret = self.sendcommand('StopMonitor',
                                               [('Channel', channel)])
                except self.AMIError:
                        ret = False
                except Exception:
                        ret = False
                return ret
        
        # \brief Retrieves the value of Variable in a Channel
        def getvar(self, channel, varname):
                try:
                        ret = self.sendcommand('Getvar', [('Channel', channel),
                                                          ('Variable', varname)])
                except self.AMIError:
                        ret = False
                except Exception:
                        ret = False
                return ret

        # \brief Transfers a channel towards a new extension.
        def transfer(self, channel, extension, context):
                ph = re.sub(__dialallowed__, '', extension)
                if len(ph) > 0:
                        return False
                try:
                        command_details = [('Channel', channel),
                                           ('Exten', extension),
                                           ('Context', context),
                                           ('Priority', '1')]
                        ret = self.sendcommand('Redirect', command_details)
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False

        # \brief Atxfer a channel towards a new extension.
        def atxfer(self, channel, extension, context):
                ph = re.sub(__dialallowed__, '', extension)
                if len(ph) > 0:
                        return False
                try:
                        ret = self.sendcommand('Atxfer', [('Channel', channel),
                                                          ('Exten', extension),
                                                          ('Context', context),
                                                          ('Priority', '1')])
                        return ret
                except self.AMIError:
                        return False
                except Exception:
                        return False

        def txfax(self, faxdir, faxid, userid, callerid, number, context, reference):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                try:
                        ret = self.sendcommand('Originate', [('Channel', 'Local/%s@%s' % (number, context)),
                                                             ('CallerID', callerid),
                                                             ('Variable', 'FAXDIR=%s' % faxdir),
                                                             ('Variable', 'FAXID=%s' % faxid),
                                                             ('Variable', 'XIVO_USERID=%s' % userid),
                                                             ('Context', 'macro-txfax'),
                                                             ('Exten', 's'),
                                                             ('ActionID', reference),
                                                             ('Priority', '1')])
                        return ret
                except self.AMIError:
                        return False
                except socket.timeout:
                        return False
                except socket:
                        return False
                except Exception:
                        return False

class AMIList:
        def __init__(self):
                self.config = {}
                self.ami = {}
                self.rami = {}
                return

        def setconfig(self, astid, address, loginname, password):
                self.config[astid] = [address, loginname, password]
                return
        
        def connect(self, astid):
                [address, loginname, password] = self.config[astid]
                if astid not in self.ami:
                        self.ami[astid] = None
                if self.ami[astid] is None:
                        log.info('%s AMI : attempting to connect' % astid)
                        amicl = AMIClass(address, loginname, password, True)
                        amicl.connect()
                        amicl.login()
                        
                        log.info('%s AMI : OPENED' % astid)
                        amicl.sendcommand('Command', [('Command', 'core show version'),
                                                      ('ActionID' , ''.join(random.sample(__alphanums__, 10)) + "-" + hex(int(time.time())))])
                        
                        self.ami[astid] = amicl
                        self.rami[amicl.fd] = astid
                        self.request_initvalues(astid)
                else:
                        log.info('%s AMI : already connected %s'
                                 % (astid, self.ami[astid]))
                        self.request_initvalues(astid)
                return
        
        def request_initvalues(self, astid):
                if astid in self.ami:
                        conn_ami = self.ami.get(astid)
                        
                        # sendparkedcalls before sendstatus : parked calls can be identified later
                        # sendmeetmelist before sendstatus : to fill the times spent for various conf rooms
                        conn_ami.setactionid('00')
                        conn_ami.sendparkedcalls()
                        conn_ami.setactionid('00')
                        conn_ami.sendmeetmelist()
                        conn_ami.setactionid('00')
                        conn_ami.sendstatus()
                        conn_ami.setactionid('00')
                        conn_ami.sendagents()
                        conn_ami.setactionid('00')
                        conn_ami.sendqueuestatus()
                        conn_ami.setactionid('00')
                return
        
        def set_aoriginate(self, astid, aoriginatecmd):
                self.ami[astid].set_aoriginate(aoriginatecmd)
                return
        
        def fdlist(self):
                return self.rami.keys()
        
        def remove(self, astid):
                if astid in self.ami:
                        fd = self.ami[astid].fd
                        del self.ami[astid]
                        if fd in self.rami:
                                del self.rami[fd]
                        else:
                                log.warning('(remove) %s : fd %s not in self.rami' % (astid, fd))
                else:
                        log.warning('(remove) astid %s not in self.ami' % astid)

        def astid(self, sock):
                return self.rami.get(sock)

        def execute(self, astid, command, *args):
                actionid = None
                if astid in self.ami:
                        conn_ami = self.ami.get(astid)
                        if conn_ami is None:
                                log.warning('ami (command %s) : <%s> in list but not connected - wait for the next update ?'
                                            % (command, astid))
                        else:
                                try:
                                        actionid = ''.join(random.sample(__alphanums__, 10))
                                        conn_ami.setactionid(actionid)
                                        ret = getattr(conn_ami, command)(*args)
                                except Exception:
                                        log.exception('AMI command %s on <%s>' % (command, astid))
                else:
                        log.warning('ami (command %s) : %s not in list - wait for the next update ?'
                                    % (command, astid))
                return actionid
