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

import xivo_commandsets
from xivo_commandsets import BaseCommand

class XivoSimpleCommand(BaseCommand):
        def __init__(self):
		BaseCommand.__init__(self)
        def get_list_commands_clt2srv(self):
                return ['login',
                        'history', 'directory-search',
                        'featuresget', 'featuresput',
                        'phones-list', 'phones-add', 'phones-del',
                        'faxsend',
                        'database',
                        'message',
                        'availstate',
                        'originate', 'transfer', 'atxfer', 'hangup']

        def get_list_commands_srv2clt(self):
                return ['phones-update', 'message', 'loginko', 'featuresupdate']
        def parsecommand(self, linein):
                params = linein.split()
                cmd = xivo_commandsets.Command(params[0], params[1:])
                if cmd.name == 'login':
                        cmd.type = xivo_commandsets.CMD_LOGIN
                else:
                        cmd.type = xivo_commandsets.CMD_OTHER
                return cmd

        def get_login_params(self, command):
                arglist = command.args[0].split(';')
                cfg = {}
                for argm in arglist:
                        [param, value] = argm.split('=')
                        cfg[param] = value
                return cfg

        def required_login_params(self):
                return ['astid', 'proto', 'userid', 'state', 'ident', 'passwd', 'version']

        def directory_srv2clt(self, context, results):
                header = 'directory-response=%d;%s' %(len(context.search_valid_fields), ';'.join(context.search_titles))
                if len(results) == 0:
                        return header
                else:
                        return header + ';' + ';'.join(results)

        def connected_srv2clt(self, conn, num):
                return
        def manage_srv2clt(self, conn, parsedcommand):
                return
        
        def message_srv2clt(self, sender, message):
                return 'message=%s::%s' %(sender, message)
        def dmessage_srv2clt(self, message):
                return self.message_srv2clt('daemon-announce', message)
        def loginko_srv2clt(self, errorstring):
                return 'loginko=%s' % errorstring
        def history_srv2clt(self, historytab):
                return 'history=%s' % ''.join(historytab)
        def features_srv2clt(self, direction, message):
                return 'features%s=%s' %(direction, message)
        def connect_srv2clt(self, num):
                pass
        def park_srv2clt(self, function, args):
                strupdate = ''
                if function == 'parked':
                        [astid, channel, cfrom, exten, timeout] = args
                        strupdate = 'parkedcall=%s;%s;%s;%s;%s' %(astid, channel, cfrom, exten, timeout)
                elif function == 'unparked':
                        [astid, channel, cfrom, exten] = args
                        strupdate = 'unparkedcall=%s;%s;%s;%s;unpark' %(astid, channel, cfrom, exten)
                elif function == 'timeout':
                        [astid, channel, exten] = args
                        strupdate = 'parkedcalltimeout=%s;%s;;%s;timeout' %(astid, channel, exten)
                elif function == 'giveup':
                        [astid, channel, exten] = args
                        strupdate = 'parkedcallgiveup=%s;%s;;%s;giveup' %(astid, channel, exten)
                return strupdate
        def phones_srv2clt(self, function, args):
                strupdate = ''
                if function == 'update':
                        strupdate = 'phones-update=' + ':'.join(args)
                elif function == 'noupdate':
                        strupdate = 'phones-noupdate=' + ':'.join(args)
                elif function == 'signal-deloradd':
                        [astid, ndel, nadd, ntotal] = args
                        strupdate = 'phones-signal-deloradd=%s;%d;%d;%d' % (astid, ndel, nadd, ntotal)
                return strupdate

xivo_commandsets.CommandClasses['xivosimple'] = XivoSimpleCommand
