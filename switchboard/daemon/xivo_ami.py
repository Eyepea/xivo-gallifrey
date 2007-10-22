#!/usr/bin/python
# $Date$
"""
Asterisk AMI utilities.
Copyright (C) 2007, Proformatique
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
                        ret = self.sendcommand('Originate', [('Channel', phoneproto + '/' + phonesrc),
                                                             ('Exten', phonedst),
                                                             ('Context', locext),
                                                             ('Priority', '1'),
                                                             # ('CallerID', "%s" %(phonesrc)),
                                                             ('CallerID', "calls %s <%s>" %(phonedst, phonedst)),
                                                             ('Variable', 'ORIGINATE_SRC=%s' %phonesrc),
                                                             ('Async', 'true')])
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

        def txfax(self, filename, callerid, number, context, debug):
                # originate a call btw src and dst
                # src will ring first, and dst will ring when src responds
                try:
                        if debug:
                                data = '%s|%s|debug' %(filename, number)
                        else:
                                data = '%s|%s' %(filename, number)
                        ret = self.sendcommand('Originate', [('Channel', "Local/%s@%s" %(number, context)),
                                                             ('Application', 'txfax'),
                                                             ('CallerID', callerid),
                                                             ('Data', data)])
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
