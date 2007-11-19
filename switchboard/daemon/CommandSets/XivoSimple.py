
__version__ = '$Revision$ $Date$'

import xivo_commandsets
from xivo_commandsets import BaseCommand

class XivoSimpleCommand(BaseCommand):
        def __init__(self):
		BaseCommand.__init__(self)
        def get_list_commands_clt2srv(self):
                return ['history', 'directory-search',
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
                return cmd

        def directory_srv2clt(self, context, results):
                header = 'directory-response=%d;%s' %(len(context.search_valid_fields), ';'.join(context.search_titles))
                if len(results) == 0:
                        return header
                else:
                        return header + ';' + ';'.join(results)
 
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
                if function == 'update':
                        strupdate = 'phones-update=' + ':'.join(args)
                elif function == 'noupdate':
                        strupdate = 'phones-noupdate=' + ':'.join(args)
                elif function == 'signal-deloradd':
                        [astid, ndel, nadd, ntotal] = args
                        strupdate = 'phones-signal-deloradd=%s;%d;%d;%d' % (astid, ndel, nadd, ntotal)
                return strupdate

xivo_commandsets.CommandClasses['xivosimple'] = XivoSimpleCommand
