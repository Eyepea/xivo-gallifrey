# -*- coding: UTF-8 -*-

from __future__ import with_statement

__license__ = """
    Copyright (C) 2011  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

from ConfigParser import RawConfigParser
from qlogclient.params import ConfigSpec


_BOOL_TRUE = ['True', 'true', '1']
_BOOL_FALSE = ['False', 'false', '0']

def _bool(raw_value):
    # Return a boolean (type boolean) from a boolean string representation
    if raw_value in _BOOL_TRUE:
        return True
    elif raw_value in _BOOL_FALSE:
        return False
    else:
        raise ValueError('invalid boolean raw value "%s"' % raw_value)


def _new_config_spec():
    cfg_spec = ConfigSpec()
    cfg_spec.add_param('general.qlog_server_uri', default=ConfigSpec.MANDATORY)
    cfg_spec.add_param('general.agents_server_uri', default=ConfigSpec.MANDATORY)
    cfg_spec.add_param('general.https_proxy', default=ConfigSpec.NO_DEFAULT)
    cfg_spec.add_param('general.username', default=ConfigSpec.MANDATORY)
    cfg_spec.add_param('general.password', default=ConfigSpec.MANDATORY)
    cfg_spec.add_param('general.compress', default=True, fun=_bool)
    cfg_spec.add_param('general.state_dir', default='/var/lib/pf-xivo-qlog-client')
    cfg_spec.add_param('general.qlog_basepath', default='/var/log/asterisk/queue_log')
    cfg_spec.add_param('general.maxlines_per_request', default=75000, fun=int)
    cfg_spec.add_param('general.ast_db_uri', default='sqlite:/var/lib/asterisk/astsqlite?timeout_ms=150')
    cfg_spec.add_param('general.asternic_db_uri', default='mysql://stats:stats@localhost/stats')
    return cfg_spec

_CONFIG_SPEC = _new_config_spec()


def read_config(filename):
    config_parser = RawConfigParser()
    with open(filename) as fobj:
        config_parser.readfp(fobj)
    return _CONFIG_SPEC.read_config(config_parser)
