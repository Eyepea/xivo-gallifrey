"""packages module

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

import logging

from xivo import http_json_server
from xivo.http_json_server import HttpReqError
from xivo.http_json_server import CMD_R
from xivo.moresynchro import RWLock

from xivo_sysconf import helpers

# XXX Disable apt FutureWarning: apt API not stable yet
import warnings
warnings.simplefilter('ignore', FutureWarning)
import apt
warnings.simplefilter('default', FutureWarning)


log = logging.getLogger('xivo_sysconf.modules.packages') # pylint: disable-msg=C0103

PACKAGES_LOCK_TIMEOUT       = 60 # XXX
PACKAGESLOCK                = RWLock()

PACKAGES_LIST   = {'asterisk':
                                {'depends':     ['asterisk'],
                                 'recommends':  ['asterisk-moh-nonfree']},
                   'mysql':
                                {'depends':     ['php4-mysql'],
                                 'ipbxengine':  {'asterisk':
                                                                {'depends': ['asterisk-mysql']}}},
                   'sqlite':
                                {'depends':     ['sqlite', 'php4-sqlite'],
                                 'ipbxengine':
                                                {'asterisk':
                                                                {'depends':  ['pf-asterisk-res-sqlite2']}}},
                   'xivo':      {'depends':     ['pf-xivo'],
                                 'recommends':  ['pf-stats-munin',
                                                 'pf-asternic-stats',
                                                 'pf-monitoring-monit']}}

PACKAGES_DEPENDENCY_LEVELS   = ('depends',
                                'recommends',
                                'suggests',
                                'enhances',
                                'pre-depends')


class Packages:
    def __init__(self):
        self.aptcache   = None
        self.reqpkg     = None

        self.opts       = {}
        self.args       = {}
        self.options    = {}

    def aptcache_update(self, args, options):    # pylint: disable-msg=W0613
        """
        GET /aptcache_update
        """
        if not PACKAGESLOCK.acquire_read(PACKAGES_LOCK_TIMEOUT):
            raise HttpReqError(503, "unable to take PACKAGESLOCK for reading after %s seconds" % PACKAGES_LOCK_TIMEOUT)

        if not self.aptcache:
            self.aptcache = apt.Cache()

        try:
            return self.aptcache.update()
        finally:
            PACKAGESLOCK.release()

    def _get_dependency_level(self):
        if 'level' in self.options:
            self.options['level'] = helpers.extract_scalar(self.options['level'])

            if not helpers.exists_in_list(self.options['level'],
                                          PACKAGES_DEPENDENCY_LEVELS):
                raise HttpReqError(415, "invalid option 'level'")

            self.opts['level'] = helpers.extract_exists_in_list(self.options['level'],
                                                                PACKAGES_LIST[self.reqpkg].keys())

            if not self.opts['level']:
                return
        else:
            self.opts['level'] = helpers.extract_exists_in_list(PACKAGES_LIST[self.reqpkg].keys(),
                                                                PACKAGES_DEPENDENCY_LEVELS)

    def _get_ipbxengine(self):
        if 'ipbxengine' in self.options:
            if isinstance(self.options['ipbxengine'], basestring) \
               and PACKAGES_LIST[self.reqpkg].has_key('ipbxengine') \
               and PACKAGES_LIST[self.reqpkg]['ipbxengine'].has_key(self.options['ipbxengine']):
                self.opts['ipbxengine'] = self.options['ipbxengine']
            else:
                raise HttpReqError(415, "invalid option 'ipbxengine'")

    def _get_package_info(self, pkgname):
        if not self.aptcache:
            self.aptcache = apt.Cache()

        r = {'status':              'notexists',
             'installedversion':    None,
             'candidateversion':    None}

        if not self.aptcache.has_key(pkgname):
            return r
        elif self.aptcache[pkgname].isInstalled:
            r['status'] = 'installed'
        else:
            r['status'] = 'notinstalled'

        r['installedversion'] = self.aptcache[pkgname].installedVersion
        r['candidateversion'] = self.aptcache[pkgname].candidateVersion

        return r

    def _dependencies_list(self):
        if not PACKAGESLOCK.acquire_read(PACKAGES_LOCK_TIMEOUT):
            raise HttpReqError(503, "unable to take PACKAGESLOCK for reading after %s seconds" % PACKAGES_LOCK_TIMEOUT)

        ret = {'base': {}}

        try:
            for level in self.opts['level']:
                ret['base'][level] = {}
                ref = ret['base'][level]

                for pkgname in PACKAGES_LIST[self.reqpkg][level]:
                    ref[pkgname] = self._get_package_info(pkgname)

            if 'ipbxengine' in self.opts:
                ret['ipbxengine'] = {}

                for level, pkgs in PACKAGES_LIST[self.reqpkg]['ipbxengine'][self.opts['ipbxengine']].iteritems():
                    ret['ipbxengine'][level] = {}
                    ref = ret['ipbxengine'][level]

                    for pkgname in pkgs:
                        ref[pkgname] = self._get_package_info(pkgname)

            return ret
        finally:
            PACKAGESLOCK.release()

    def dependencies_asterisk(self, args, options):
        """
        GET /dependencies_asterisk
        
        Just returns asterisk dependencies and their status
        """

        self.reqpkg     = 'asterisk'
        self.opts       = {}

        self.args       = args
        self.options    = options

        self._get_dependency_level()

        return self._dependencies_list()

    def dependencies_mysql(self, args, options):
        """
        GET /dependencies_mysql
        
        Just returns mysql dependencies and their status
        """

        self.reqpkg     = 'mysql'
        self.opts       = {}

        self.args       = args
        self.options    = options

        self._get_dependency_level()
        self._get_ipbxengine()

        return self._dependencies_list()

    def dependencies_sqlite(self, args, options):
        """
        GET /dependencies_sqlite
        
        Just returns sqlite dependencies and their status
        """

        self.reqpkg     = 'sqlite'
        self.opts       = {}

        self.args       = args
        self.options    = options

        self._get_dependency_level()
        self._get_ipbxengine()

        return self._dependencies_list()

    def dependencies_xivo(self, args, options):
        """
        GET /dependencies_xivo
        
        Just returns xivo dependencies and their status
        """

        self.reqpkg     = 'xivo'
        self.opts       = {}

        self.args       = args
        self.options    = options

        self._get_dependency_level()

        return self._dependencies_list()


packages = Packages()

http_json_server.register(packages.aptcache_update, CMD_R)
http_json_server.register(packages.dependencies_asterisk, CMD_R)
http_json_server.register(packages.dependencies_mysql, CMD_R)
http_json_server.register(packages.dependencies_sqlite, CMD_R)
http_json_server.register(packages.dependencies_xivo, CMD_R)
