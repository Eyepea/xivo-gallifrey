
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
 
        def update_srv2clt(self, phoneinfo):
                return 'phones-update=' + ':'.join(phoneinfo)
        def fakeupdate_srv2clt(self, phoneinfo):
                return '______=' + ':'.join(phoneinfo)
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

xivo_commandsets.CommandClasses['xivosimple'] = XivoSimpleCommand
