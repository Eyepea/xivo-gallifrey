# $Revision$
# $Date$
"""
Asterisk AMI utilities.
Copyright (C) 2007, 2008, Proformatique
"""

__version__ = "$Revision$ $Date$"

import socket

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
        def sendcommand(self, action, args):
                ret = False
                try:
                        self.fd.write('Action: ' + action + '\r\n')
                        for (name, value) in args:
                                self.fd.write(name + ': ' + value + '\r\n')
                        self.fd.write('\r\n')
                        self.fd.flush()
                        ret = True
                except:
                        ret = False
                if ret == False:
                        # tries to reconnect
                        try:
                                self.connect()
                                self.login()
                                if self:
                                        # "retrying AMI command=<%s> args=<%s>" % (action, str(args)))
                                        self.sendcommand(action, args)
                        except Exception, exc:
                                # log_debug("--- exception --- AMI not connected (action=%s args=%s) : %s" %(action, str(args), str(exc)))
                                pass
                return ret
        # \brief Requesting a Status.
        def sendstatus(self):
                ret = self.sendcommand('Status', [])
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
                        reply = self.readresponse('')
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
                        reply = self.readresponse('')
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
                        print 'AOriginate', phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext
                        ret = self.sendcommand('AOriginate', [('Channel', phoneproto + '/' + phonesrc),
                                                              ('Exten', phonedst),
                                                              ('Context', locext),
                                                              ('Priority', '1'),
                                                              # ('CallerID', "%s" %(phonesrc)),
                                                              ('CallerID', "%s <%s>" %(cidnamedst, phonedst)),
                                                              ('Variable', 'XIVO_ORIGSRCNAME=%s' % cidnamesrc),
                                                              ('Variable', 'XIVO_ORIGSRCNUM=%s'  % phonesrc),
                                                              ('Async', 'true')])
                        reply = self.readresponse('')
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Originates a call from a phone towards another.
        def aoriginate_var(self, phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext, var, val):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                try:
                        print 'AOriginate_var', phoneproto, phonesrc, cidnamesrc, phonedst, cidnamedst, locext
                        ret = self.sendcommand('AOriginate', [('Channel', phoneproto + '/' + phonesrc),
                                                             ('Exten', phonedst),
                                                             ('Context', locext),
                                                             ('Priority', '1'),
                                                             # ('CallerID', "%s" %(phonesrc)),
                                                             ('CallerID', "%s <%s>" %(cidnamedst, phonedst)),
                                                             ('Variable', 'XIVO_ORIGSRCNAME=%s' % cidnamesrc),
                                                             ('Variable', 'XIVO_ORIGSRCNUM=%s'  % phonesrc),
                                                             ('Variable', '%s=%s'  % (var, val)),
                                                             ('Async', 'true')])
                        reply = self.readresponse('')
                        return ret
                except self.AMIError, exc:
                        return False
                except Exception, exc:
                        return False

        # \brief Adds a Queue
        def queueadd(self, queuename, interface):
                try:
                        ret = self.sendcommand('QueueAdd', [('Queue', queuename),
                                                            ('Interface', interface),
                                                            ('Penalty', '1'),
                                                            ('Paused', 'true')])
                        reply = self.readresponse('')
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
                        reply = self.readresponse('')
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
                        reply = self.readresponse('')
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
                        ret = self.sendcommand('Redirect', [('Channel', channel),
                                                            ('Exten', extension),
                                                            ('Context', context),
                                                            ('Priority', '1')])
                        reply = self.readresponse('')
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
                        reply = self.readresponse('')
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
                        reply = self.readresponse('')
                        return ret
                except self.AMIError, exc:
                        return False
                except socket.timeout, exc:
                        return False
                except socket, exc:
                        return False
                except Exception, exc:
                        return False
