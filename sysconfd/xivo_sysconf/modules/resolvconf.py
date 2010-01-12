"""resolvconf module

Copyright (C) 2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
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

from time import time
from shutil import copy2
from ConfigParser import ConfigParser

from xivo import http_json_server
from xivo.http_json_server import HttpReqError
from xivo.http_json_server import CMD_RW
from xivo.moresynchro import RWLock
from xivo.xivo_config import txtsubst
from xivo import xys
from xivo import system

from xivo_sysconf import helpers

log = logging.getLogger('xivo_sysconf.modules.resolvconf') # pylint: disable-msg=C0103

RESOLVCONFLOCK          = RWLock()

Rcc =   {'resolvconf_file':     os.path.join(os.path.sep, 'etc', 'resolv.conf'),
         'resolvconf_tpl_file': os.path.join('resolvconf', 'resolv.conf'),
         'lock_timeout':        60}


RESOLVCONF_SCHEMA = xys.load("""
nameservers:    !~~seqlen(1,3) [ !~ipv4_address_or_domain 192.168.0.254 ]
search?:        !~~seqlen(1,6) [ !~search_domain example.com ]
""")

def _resolv_conf_variables(args):
    xvars = {}
    xvars['_XIVO_NAMESERVER_LIST'] = os.linesep.join(["nameserver %s"] * len(args['nameservers'])) \
                                     % tuple(args['nameservers'])

    if args.has_key('search'):
        xvars['_XIVO_DNS_SEARCH'] = "search %s" % " ".join(args['search'])
    else:
        xvars['_XIVO_DNS_SEARCH'] = ""

    return xvars

def ResolvConf(args, options):    # pylint: disable-msg=W0613
    """
    POST /resolv_conf

    >>> resolv_conf({'nameservers': '192.168.0.254'})
    >>> resolv_conf({'nameservers': ['192.168.0.254', '10.0.0.254']})
    >>> resolv_conf({'search': ['toto.tld', 'tutu.tld']
                     'nameservers': ['192.168.0.254', '10.0.0.254']})
    """

    if 'nameservers' in args:
        args['nameservers'] = helpers.extract_scalar(args['nameservers'])
        nameservers = helpers.unique_case_tuple(args['nameservers'])

        if len(nameservers) == len(args['nameservers']):
            args['nameservers'] = list(nameservers)
        else:
            raise HttpReqError(415, "duplicated nameservers in %r" % list(args['nameservers']))

    if 'search' in args:
        args['search'] = helpers.extract_scalar(args['search'])
        search = helpers.unique_case_tuple(args['search'])

        if len(search) == len(args['search']):
            args['search'] = list(search)
        else:
            raise HttpReqError(415, "duplicated search in %r" % list(args['search']))

        if len(''.join(args['search'])) > 255:
            raise HttpReqError(415, "maximum length exceeded for option search: %r" % list(args['search']))

    if not xys.validate(args, RESOLVCONF_SCHEMA):
        raise HttpReqError(415, "invalid arguments for command")

    if not os.access(Rcc['resolvconf_path'], (os.X_OK | os.W_OK)):
        raise HttpReqError(415, "path not found or not writable or not executable: %r" % Rcc['resolvconf_path'])

    if not RESOLVCONFLOCK.acquire_read(Rcc['lock_timeout']):
        raise HttpReqError(503, "unable to take RESOLVCONFLOCK for reading after %s seconds" % Rcc['lock_timeout'])

    resolvconfbakfile = None

    try:
        try:
            if not os.path.isdir(Rcc['resolvconf_backup_path']):
                os.makedirs(Rcc['resolvconf_backup_path'])

            if os.access(Rcc['resolvconf_file'], os.R_OK):
                resolvconfbakfile = "%s.%d" % (Rcc['resolvconf_backup_file'], time())
                copy2(Rcc['resolvconf_file'], resolvconfbakfile)

            if os.access(Rcc['resolvconf_custom_tpl_file'], (os.F_OK | os.R_OK)):
                filename = Rcc['resolvconf_custom_tpl_file']
            else:
                filename = Rcc['resolvconf_tpl_file']

            template_file = open(filename)
            template_lines = template_file.readlines()
            template_file.close()

            txt = txtsubst(template_lines,
                           _resolv_conf_variables(args),
                           Rcc['resolvconf_file'],
                           'utf8')

            system.file_writelines_flush_sync(Rcc['resolvconf_file'], txt)

            return True
        except Exception, e:
            if resolvconfbakfile:
                copy2(resolvconfbakfile, Rcc['resolvconf_file'])
            raise e.__class__(str(e))
    finally:
        RESOLVCONFLOCK.release()

def safe_init(options):
    """Load parameters, etc"""
    global Rcc

    cfg = options.configuration

    tpl_path        = cfg.get('general', 'templates_path')
    custom_tpl_path = cfg.get('general', 'custom_templates_path')
    backup_path     = cfg.get('general', 'backup_path')

    if cfg.has_section('resolvconf'):
        if cfg.has_option('resolvconf', 'lock_timeout'):
            Rcc['lock_timeout'] = cfg.getfloat('resolvconf', 'lock_timeout')

        if cfg.has_option('resolvconf', 'resolvconf_file'):
            Rcc['resolvconf_file'] = cfg.get('resolvconf', 'resolvconf_file')

    Rcc['resolvconf_tpl_file'] = os.path.join(tpl_path,
                                              Rcc['resolvconf_tpl_file'])

    Rcc['resolvconf_custom_tpl_file'] = os.path.join(custom_tpl_path,
                                                     Rcc['resolvconf_tpl_file'])

    Rcc['resolvconf_path'] = os.path.dirname(Rcc['resolvconf_file'])
    Rcc['resolvconf_backup_file'] = os.path.join(backup_path,
                                                 Rcc['resolvconf_file'].lstrip(os.path.sep))
    Rcc['resolvconf_backup_path'] = os.path.join(backup_path,
                                                 Rcc['resolvconf_path'].lstrip(os.path.sep))

http_json_server.register(ResolvConf, CMD_RW, safe_init=safe_init, name='resolv_conf')
