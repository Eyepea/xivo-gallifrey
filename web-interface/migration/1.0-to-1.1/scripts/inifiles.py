#!/usr/bin/python2.5
from __future__ import with_statement
__version__ = "$Revision$ $Date$"
__author__  = "Guillaume Bour <gbour@proformatique.com>"
__license__ = """
    Copyright (C) 2010  Proformatique <technique@proformatique.com>

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
"""Migrate from 1.0 to 1.1
   Convert ini files (/etc/pf-xivo/web-interface)
   
   NOTE: can be safely reexecuted
"""

import sys, os.path
from ConfigParser import ConfigParser

def migrate_cti(basedir):
	frm = ConfigParser()
	frm.read(basedir + '/xivo.ini')
	
	to = ConfigParser()
	to.read(basedir + '/cti.ini')
	
	datastorage = frm.get('general', 'datastorage')
	to.set('general', 'datastorage', datastorage)

	with open(basedir + '/cti.ini', 'wb') as fp:
		to.write(fp)

def migrate_xivo(basedir):
	cfg = ConfigParser()
	cfg.read(basedir + '/xivo.ini')

	cfg.set('session', 'save_path', '"/var/lib/pf-xivo-web-interface/session"')

	if cfg.get('error', 'report_type') == '1' and \
	   cfg.get('error', 'email') == 'xivo-webinterface-error@proformatique.com':
		cfg.set('error', 'report_type', '0')
		cfg.set('error', 'email'      , 'john.doe@example.com')

	with open(basedir + '/xivo.ini', 'wb') as fp:
		cfg.write(fp)

def migrate_ipbx(basedir):
	cfg = ConfigParser()
	cfg.read(basedir + '/ipbx.ini')

	cfg.set('configfiles', 'path', '/etc/asterisk/extensions_extra.d')
	cfg.set('logaccess'  , 'file', '/var/log/pf-xivo-web-interface/xivo.log')

	with open(basedir + '/ipbx.ini', 'wb') as fp:
		cfg.write(fp)


if __name__ == '__main__':
	if len(sys.argv) != 2:
		print 'Usage: %s base-dir' % sys.argv[0]; sys.exit(1)

	basedir = sys.argv[1]
	if not os.path.isdir(basedir):
		print "%s directory not found or not directory"; sys.exit(1)
		
	migrate_cti(basedir)
	migrate_xivo(basedir)
	migrate_ipbx(basedir)
