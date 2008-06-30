# XIVO Daemon

__version__   = '$Revision$'
__date__      = '$Date$'
__copyright__ = 'Copyright (C) 2007, 2008, Proformatique'
__author__    = 'Corentin Le Gall'

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, you will find one at
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.

"""
Asterisk AMI utilities.
"""

import random
import socket
import string
import time
from xivo_log import *

__alphanums__ = string.uppercase + string.lowercase + string.digits

def log_debug(level, text):
        log_debug_file(level, text, 'xivo_ami')

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

        def set_aoriginate(self, aoriginatecmd):
                self.aorgcmd = aoriginatecmd

        # \brief Connection to a socket.
        def connect(self):
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(self.address)
                s.settimeout(30)
                self.fd = s.makefile('rw', 0)
                s.close()
                str = self.fd.readline()
                #print str,
        # \brief Sending any AMI command.
        def sendcommand(self, action, args, loopnum = 0):
                ret = False
                try:
                        self.fd.write('Action: %s\r\n' % action)
                        for (name, value) in args:
                                self.fd.write('%s: %s\r\n' % (name, value))
                        self.fd.write('\r\n')
                        self.fd.flush()
                        ret = True
                except Exception, exc:
                        log_debug(SYSLOG_ERR, '--- exception --- (action %s) %s' % (action, str(exc)))
                        ret = False
                if ret == False:
                        if loopnum == 0:
                                log_debug(SYSLOG_WARNING, 'second attempt for AMI command (%s)' % action)
                                # tries to reconnect
                                try:
                                        self.connect()
                                        self.login()
                                        if self:
                                                # "retrying AMI command=<%s> args=<%s>" % (action, str(args)))
                                                self.sendcommand(action, args, 1)
                                except Exception, exc:
                                        # log_debug("--- exception --- AMI not connected (action=%s args=%s) : %s" %(action, str(args), str(exc)))
                                        pass
                        else:
                                log_debug(SYSLOG_WARNING, 'warning : 2 attempts have failed for AMI command (%s)' % action)
                return ret
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
        def hangup(self, channel, channel_peer):
                ret = 0
                try:
                        self.sendcommand('Hangup',
                                         [('Channel', channel)])
                        ret += 1
                except self.AMIError, exc:
                        pass
                except Exception, exc:
                        pass

                if channel_peer != "":
                        try:
                                self.sendcommand('Hangup',
                                                 [('Channel', channel_peer)])
                                ret += 2
                        except self.AMIError, exc:
                                pass
                        except Exception, exc:
                                pass
                
                return ret


        def setvar(self, var, val, chan = None):
                try:
                        if chan is None:
                                ret = self.sendcommand('SetVar', [('Variable', var),
                                                                  ('Value', val)])
                        else:
                                ret = self.sendcommand('SetVar', [('Channel', chan),
                                                                  ('Variable', var),
                                                                  ('Value', val)])
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False


        # \brief Originates a call from a phone towards another.
        def originate(self, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                try:
                        ret = self.sendcommand('Originate', [('Channel', phoneproto + '/' + phonesrc),
                                                             ('Exten', phonedst),
                                                             ('Context', locext),
                                                             ('Priority', '1'),
                                                             # ('CallerID', "%s" %(phonesrc)),
                                                             ('CallerID', "%s <%s>" %(cidnamedst, phonedst)),
                                                             ('Variable', 'XIVO_ORIGSRCNAME=%s' % cidnamesrc),
                                                             ('Variable', 'XIVO_ORIGSRCNUM=%s'  % phonesrc),
                                                             ('Async', 'true')])
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Originates a call from a phone towards another.
        def aoriginate(self, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                try:
                        #print self.aorgcmd, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext
                        ret = self.sendcommand(self.aorgcmd, [('Channel', phoneproto + '/' + phonesrc),
                                                              ('Exten', phonedst),
                                                              ('Context', locext),
                                                              ('Priority', '1'),
                                                              # ('CallerID', "%s" %(phonesrc)),
                                                              ('CallerID', "%s <%s>" %(cidnamedst, phonedst)),
                                                              ('Variable', 'XIVO_ORIGSRCNAME=%s' % cidnamesrc),
                                                              ('Variable', 'XIVO_ORIGSRCNUM=%s'  % phonesrc),
                                                              ('Async', 'true')])
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Originates a call from a phone towards another.
        def aoriginate_var(self, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext, extravars, timeout):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                try:
                        #print self.aorgcmd, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext
                        command_details = [('Channel', phoneproto + '/' + phonesrc),
                                           ('Exten', phonedst),
                                           ('Context', locext),
                                           ('Priority', '1'),
                                           ('Timeout', str(timeout * 1000)),
                                           # ('CallerID', "%s" %(phonesrc)),
                                           ('CallerID', "%s <%s>" %(cidnamedst, phonedst)),
                                           ('Variable', 'XIVO_ORIGSRCNAME=%s' % cidnamesrc),
                                           ('Async', 'true')]
                        if 'XIVO_ORIGSRCNUM' not in extravars:
                                extravars['XIVO_ORIGSRCNUM'] = phonesrc
                        for var, val in extravars.iteritems():
                                command_details.append(('Variable', '%s=%s'  % (var, val)))
                        ret = self.sendcommand(self.aorgcmd, command_details)
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Requests the Extension Statuses
        def extensionstate(self, extension, context):
                try:
                        ret = self.sendcommand('ExtensionState', [('Exten', extension),
                                                                  ('Context', context)])
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Logs in an Agent
        def agentcallbacklogin(self, agentnum, extension):
                try:
                        ret = self.sendcommand('AgentCallbackLogin', [('Agent', agentnum),
                                                                      ('Exten', extension)])
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Logs off an Agent
        def agentlogoff(self, agentnum):
                try:
                        ret = self.sendcommand('AgentLogoff', [('Agent', agentnum)])
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Adds a Queue
        def queueadd(self, queuename, interface, paused):
                try:
                        ret = self.sendcommand('QueueAdd', [('Queue', queuename),
                                                            ('Interface', interface),
                                                            ('Penalty', '1'),
                                                            ('Paused', paused)])
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Removes a Queue
        def queueremove(self, queuename, interface):
                try:
                        ret = self.sendcommand('QueueRemove', [('Queue', queuename),
                                                               ('Interface', interface)])
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief (Un)Pauses a Queue
        def queuepause(self, queuename, interface, paused):
                try:
                        ret = self.sendcommand('QueuePause', [('Queue', queuename),
                                                              ('Interface', interface),
                                                              ('Paused', paused)])
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Retrieves the value of Variable in a Channel
        def getvar(self, channel, varname):
                try:
                        ret = self.sendcommand('GetVar', [('Channel', channel),
                                                          ('Variable', varname)])
                        if ret:
                                reply = self.readresponsechunk()
                                return reply.get('Value')
                        else:
                                return False
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Transfers a channel towards a new extension.
        def transfer(self, channel, extension, context):
                try:
                        command_details = [('Channel', channel),
                                           ('Exten', extension),
                                           ('Context', context),
                                           ('Priority', '1')]
                        ret = self.sendcommand('Redirect', command_details)
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Atxfer a channel towards a new extension.
        def atxfer(self, channel, extension, context):
                try:
                        ret = self.sendcommand('Atxfer', [('Channel', channel),
                                                          ('Exten', extension),
                                                          ('Context', context),
                                                          ('Priority', '1')])
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        def txfax(self, faxdir, faxid, callerid, number, context):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                try:
                        ret = self.sendcommand('Originate', [('Channel', "Local/%s@%s" % (number, context)),
                                                             ('CallerID', callerid),
                                                             ('Variable', 'FAXDIR=%s' % faxdir),
                                                             ('Variable', 'FAXID=%s' % faxid),
                                                             ('Context', 'macro-txfax'),
                                                             ('Extension', 's'),
                                                             ('Priority', '1')])
                        return ret
                except self.AMIError, exc:
                        return False
                except socket.timeout, exc:
                        return False
                except socket, exc:
                        return False
                except Exception, exc:
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
                        log_debug(SYSLOG_INFO, '%s AMI : attempting to connect' % astid)
                        amicl = AMIClass(address, loginname, password, True)
                        amicl.connect()
                        amicl.login()

                        log_debug(SYSLOG_NOTICE, '%s AMI : OPENED' % astid)
                        amicl.sendcommand('Command', [('Command', 'show version'),
                                                      ('ActionID' , ''.join(random.sample(__alphanums__, 10)) + "-" + hex(int(time.time())))])
                        amicl.sendstatus()
                        amicl.sendagents()
                        amicl.sendqueuestatus()
                        
                        self.ami[astid] = amicl
                        self.rami[amicl.fd] = astid
                else:
                        log_debug(SYSLOG_INFO, '%s AMI : already connected %s'
                                  % (astid, self.ami[astid]))
                return

        def set_aoriginate(self, astid, aoriginatecmd):
                self.ami[astid].set_aoriginate(aoriginatecmd)

        def fdlist(self):
                return self.rami.iterkeys()

        def remove(self, astid):
                fd = self.ami[astid].fd
                del self.ami[astid]
                del self.rami[fd]

        def astid(self, sock):
                return self.rami.get(sock)

        def execute(self, astid, command, *args):
                if astid in self.ami:
                        try:
                                getattr(self.ami[astid], command)(*args)
                        except Exception, exc:
                                log_debug(SYSLOG_ERR, '--- exception --- AMI command %s on %s : %s' % (command, astid, exc))
                else:
                        log_debug(SYSLOG_WARNING, 'ami (command %s) : %s not in list - wait for the next update ? ...'
                                  % (astid, command))
                return
