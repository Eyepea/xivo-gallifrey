from __future__ import with_statement
"""mail module

Copyright (C) 2010  Proformatique

"""
__version__ = "$Revision$ $Date$"
__author__  = "Guillaume Bour <gbour@proformatique.com>"
__license__ = """
    Copyright (C) 2010  Proformatique

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

import os
import logging
import subprocess
#import pprint

from xivo import http_json_server
from xivo.http_json_server import HttpReqError
from xivo.http_json_server import CMD_RW
from xivo.xivo_config import txtsubst
from xivo import xys
from xivo import system

from xivo_sysconf import helpers

log = logging.getLogger('xivo_sysconf.modules.mail') # pylint: disable-msg=C0103
FILES = {}

class MailConfig:
    """
    Mail system configuration class
    """
    def __init__(self):
        pass

    SET_SCHEMA = xys.load("""
    origin:             !!str   xivo-clients.proformatique.com
    relayhost:          !!str   smtp.orange.fr
    fallback_relayhost: !!str   smtp.free.fr
    canonical:          !!str   xivo.proformatique.com
    mydomain:           !!str   avencall.com
    """)

    def set(self, args, options):
        """
        POST /set

        >>> set({'origin':              'xivo-clients.proformatique.com',
                 'relayhost':           'smtp.orange.fr',
                 'fallback_relayhost':  'smtp.free.fr',
                 'canonical':           'xivo.proformatique.com',
                 'mydomain':            'avencall.com')
        """
        if not xys.validate(args, self.SET_SCHEMA):
            raise HttpReqError(415, "invalid arguments for command")

        args = dict(map(lambda key: ("XIVO_SMTP_%s" % key.upper(), args[key]), args))
        #pprint.pprint(args)

        for type, files in FILES.iteritems():
            tpl = files['tpl']
            if os.access(files['custom'], (os.F_OK | os.R_OK)):
                tpl = files['custom']
                   
            with open(tpl, 'r') as f:
                content = f.readlines()

            content = txtsubst(content, args, files['dest'], 'utf8')
            system.file_writelines_flush_sync(files['dest'], content)
            
        subprocess.call("/etc/init.d/postfix reload")
        #TODO: check postfix processes correctly running & reloaded
        return True


def safe_init(options):
    """Load parameters, etc"""
    cfg = options.configuration

    tpl_path        = cfg.get('general', 'templates_path')
    custom_tpl_path = cfg.get('general', 'custom_templates_path')
    backup_path     = cfg.get('general', 'backup_path')

    for fname in ('mailname', 'main.cf', 'canonical'):
        FILES[fname] = {
            "tpl"	: os.path.join(tpl_path, "mail/%s" % fname),
            "custom"	: os.path.join(custom_tpl_path, "mail/%s" % fname),
            #FILES["%s_backup_file" % fname] 	= os.path.join(backup_path, "%s_backup_file" % fname)
	
            "dest"	: cfg.get('mail', "%s_file" % fname),
        }

mailconfig = MailConfig()
http_json_server.register(mailconfig.set, CMD_RW, safe_init=safe_init, name='mailconfig_set')
