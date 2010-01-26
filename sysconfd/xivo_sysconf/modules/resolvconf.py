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

Rcc =   {'hostname_file':       os.path.join(os.path.sep, 'etc', 'hostname'),
         'hostname_tpl_file':   os.path.join('resolvconf', 'hostname'),
         'resolvconf_file':     os.path.join(os.path.sep, 'etc', 'resolv.conf'),
         'resolvconf_tpl_file': os.path.join('resolvconf', 'resolv.conf'),
         'lock_timeout':        60}


def _write_config_file(optname, xvars):
    backupfilename = None

    if not os.path.isdir(Rcc["%s_backup_path" % optname]):
        os.makedirs(Rcc["%s_backup_path" % optname])

    if os.access(Rcc["%s_file" % optname], os.R_OK):
        backupfilename = "%s.%d" % (Rcc["%s_backup_file" % optname], time())
        copy2(Rcc["%s_file" % optname], backupfilename)

    if os.access(Rcc["%s_custom_tpl_file" % optname], (os.F_OK | os.R_OK)):
        filename = Rcc["%s_custom_tpl_file" % optname]
    else:
        filename = Rcc["%s_tpl_file" % optname]

    template_file = open(filename)
    template_lines = template_file.readlines()
    template_file.close()

    txt = txtsubst(template_lines,
                   xvars,
                   Rcc["%s_file" % optname],
                   'utf8')

    system.file_writelines_flush_sync(Rcc["%s_file" % optname], txt)

    return backupfilename


HOSTNAME_SCHEMA = xys.load("""
hostname:   !~domain_label
""")

def HostName(args, options):    # pylint: disable-msg=W0613
    """
    POST /hostname

    >>> hostname({'hostname': 'xivo'})
    """

    if not xys.validate(args, HOSTNAME_SCHEMA):
        raise HttpReqError(415, "invalid arguments for command")

    if not os.access(Rcc['hostname_path'], (os.X_OK | os.W_OK)):
        raise HttpReqError(415, "path not found or not writable or not executable: %r" % Rcc['hostname_path'])

    if not RESOLVCONFLOCK.acquire_read(Rcc['lock_timeout']):
        raise HttpReqError(503, "unable to take RESOLVCONFLOCK for reading after %s seconds" % Rcc['lock_timeout'])

    hostnamebakfile = None

    try:
        try:
            hostnamebakfile = _write_config_file('hostname',
                                                 {'_XIVO_HOSTNAME': args['hostname']})
            return True
        except Exception, e:
            if hostnamebakfile:
                copy2(hostnamebakfile, Rcc['hostname_file'])
            raise e.__class__(str(e))
    finally:
        RESOLVCONFLOCK.release()


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
            resolvconfbakfile = _write_config_file('resolvconf',
                                                   _resolv_conf_variables(args))
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

        if cfg.has_option('resolvconf', 'hostname_file'):
            Rcc['hostname_file'] = cfg.get('resolvconf', 'hostname_file')

        if cfg.has_option('resolvconf', 'resolvconf_file'):
            Rcc['resolvconf_file'] = cfg.get('resolvconf', 'resolvconf_file')

    for optname in ('hostname', 'resolvconf'):
        Rcc["%s_tpl_file" % optname] = os.path.join(tpl_path,
                                                    Rcc["%s_tpl_file" % optname])

        Rcc["%s_custom_tpl_file" % optname] = os.path.join(custom_tpl_path,
                                                           Rcc["%s_tpl_file" % optname])

        Rcc["%s_path" % optname] = os.path.dirname(Rcc["%s_file" % optname])
        Rcc["%s_backup_file" % optname] = os.path.join(backup_path,
                                                       Rcc["%s_file" % optname].lstrip(os.path.sep))
        Rcc["%s_backup_path" % optname] = os.path.join(backup_path,
                                                       Rcc["%s_path" % optname].lstrip(os.path.sep))

http_json_server.register(HostName, CMD_RW, safe_init=safe_init, name='hostname')
http_json_server.register(ResolvConf, CMD_RW, name='resolv_conf')
