__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008  Proformatique

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

import sys
import signal
import socket
import SocketServer
import ConfigParser
from threading import Lock

from xivo import agitb
from xivo import anysql
from xivo import moresynchro
from xivo.BackSQL import backmysql
from xivo.BackSQL import backsqlite

import fastagi

NAME = "agid"
VERSION_MAJOR = 0
VERSION_MINOR = 1
AGI_CONFFILE = "/etc/asterisk/xivo_agi.conf"
LISTEN_ADDR_DEFAULT = "127.0.0.1"
LISTEN_PORT_DEFAULT = 4573
CONN_POOL_SIZE_DEFAULT = 10

debug_enabled = False
modules = {}
conns = []
conns_lock = Lock()
db_uri = None
conn_pool_size = None

class FastAGIRequestHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		try:
			self.fastagi = fastagi.FastAGI(self.rfile, self.wfile)
			self.except_hook = agitb.Hook(agi = self.fastagi)

			conn = acquire_conn()
			self.cursor = conn.cursor()

			module_name = self.fastagi.env['agi_network_script']
			modules[module_name].handle(self, self.fastagi, self.cursor, self.fastagi.args)

			conn.commit()

			self.fastagi.verbose('AGI module "%s" successfully executed' % module_name)

		# Attempt to relay errors to Asterisk, but if it fails, we
		# just give up.
		# XXX It may be here that dropping database connection
		# exceptions could be catched.
		except fastagi.FastAGIDialPlanBreak, message:
			try:
				self.fastagi.verbose(message)
			except:
				pass
		except:
			try:
				self.except_hook.handle()
			except:
				pass

		release_conn(conn)

	def set_fwd_vars(self, type, typeval, appval, type_varname, typeval1_varname, typeval2_varname):
		"""The purpose of this function is to set some variables in the
		Asterisk channel related to redirection. It can set up to 3 variables:
		 - the redirection type (e.g. to a user, a group, a queue)
		 - 2 parameters (typeval1 and typeval2)

		type is the redirection type and is used to determine the corresponding
		behaviour (e.g. how many variables to set, where to fetch their value
		from).

		typeval is a value related to the redirection type. For example, if the
		type is 'user', and the type value is '101', this function will look up
		user ID 101 in the user features table and set 2 variables (a number
		and a context) so that the dial plan is able to forward the call to
		that user. In this example, the number and context are the 2 parameters
		(typeval1 and typeval2) that this function sets before returning.

		Forwarding a call to an application sometimes require an extra
		parameter (since type is 'application' and typeval1 is the application
		name). This extra parameter is given in the appval argument.

		Depending on the redirection type and the application (in case the type
		is 'application'), some parameters can be processed so that commas are
		translated to semicolumns. This is an ugly hack imagined because
		Asterisk evaluates variables early, and commas are used as an argument
		separator. In such cases, the dialplan translates semicolumns back
		to commas/pipes before using the variale.

		This function calls agi.dp_break() upon detection of parameter/database
		inconsistency or when the type parameter is invalid.

		XXX This function and its users should be able to handle more
		variables (if possible, there should be no limit).

		"""

		agi = self.fastagi
		cursor = self.cursor

		agi.set_variable(type_varname, type)

		if type in ('endcall', 'schedule', 'sound'):
			agi.set_variable(typeval1_varname, typeval)
		elif type == 'application':
			agi.set_variable(typeval1_varname, typeval)

			if typeval in ('disa', 'callback'):
				agi.set_variable(typeval2_varname, appval.replace(",", ";").replace("|", ";"))
			else:
				agi.set_variable(typeval2_varname, appval)
		elif type == 'custom':
			agi.set_variable(typeval1_varname, typeval.replace(",", ";").replace("|", ";"))
		elif type == 'user':
			cursor.query("SELECT ${columns} FROM userfeatures "
                                     "WHERE id = %s "
                                     "AND IFNULL(userfeatures.number,'') != '' "
                                     "AND internal = 0 "
                                     "AND commented = 0",
                                     ('number', 'context'),
                                     (typeval,))
			res = cursor.fetchone()

			if not res:
				agi.dp_break("Database inconsistency: unable to find linked destination user '%s'" % typeval)

			agi.set_variable(typeval1_varname, res['number'])
			agi.set_variable(typeval2_varname, res['context'])
		elif type == 'group':
			cursor.query("SELECT ${columns} FROM groupfeatures INNER JOIN queue "
                                     "ON groupfeatures.name = queue.name "
                                     "WHERE groupfeatures.id = %s "
                                     "AND groupfeatures.deleted = 0 "
                                     "AND queue.category = 'group' "
                                     "AND queue.commented = 0",
                                     [('groupfeatures.' + x) for x in ('number', 'context')],
                                     (typeval,))
			res = cursor.fetchone()

			if not res:
				agi.dp_break("Database inconsistency: unable to find linked destination group '%s'" % typeval)

			agi.set_variable(typeval1_varname, res['groupfeatures.number'])
			agi.set_variable(typeval2_varname, res['groupfeatures.context'])
		elif type == 'queue':
			cursor.query("SELECT ${columns} FROM queuefeatures INNER JOIN queue "
                                     "ON queuefeatures.name = queue.name "
                                     "WHERE queuefeatures.id = %s "
                                     "AND queue.category = 'queue' "
                                     "AND queue.commented = 0",
                                     [('queuefeatures.' + x) for x in ('number', 'context')],
                                     (typeval,))
			res = cursor.fetchone()

			if not res:
				agi.dp_break("Database inconsistency: unable to find linked destination queue '%s'" % typeval)

			agi.set_variable(typeval1_varname, res['queuefeatures.number'])
			agi.set_variable(typeval2_varname, res['queuefeatures.context'])
		elif type == 'meetme':
			cursor.query("SELECT ${columns} FROM meetmefeatures INNER JOIN staticmeetme "
                                     "ON meetmefeatures.meetmeid = staticmeetme.id "
                                     "WHERE meetmefeatures.id = %s "
                                     "AND staticmeetme.commented = 0",
                                     [('meetmefeatures.' + x) for x in ('number', 'context')],
                                     (typeval,))
			res = cursor.fetchone()

			if not res:
				agi.dp_break("Database inconsistency: unable to find linked destination conference room '%s'" % typeval)

			agi.set_variable(typeval1_varname, res['meetmefeatures.number'])
			agi.set_variable(typeval2_varname, res['meetmefeatures.context'])
		else:
			agi.dp_break("Unknown destination type '%s'" % type)

	def ds_set_fwd_vars(self, id, status, category, type_varname, typeval1_varname, typeval2_varname):
		"""Front-end to set_fwd_vars() that fetches some data from
		the dialstatus table.

		To make the dial plan aware that redirection values came from the
		dialstatus table, a variable named type_varname + "_FROMDS" is created.

		"""

		cursor = self.cursor

		cursor.query("SELECT ${columns} FROM dialstatus "
                             "WHERE status = %s "
                             "AND category = %s "
                             "AND categoryval = %s "
                             "AND linked = 1",
                             ('type', 'typeval', 'applicationval'),
                             (status, category, id))
		res = cursor.fetchone()

		if not res:
			type = "endcall"
			typeval = "none"
			applicationval = None
		else:
			type = res['type']
			typeval = res['typeval']
			applicationval = res['applicationval']

		self.fastagi.set_variable(type_varname + "_FROMDS", 1)
		self.set_fwd_vars(type, typeval, applicationval, type_varname, typeval1_varname, typeval2_varname)

class AGID(SocketServer.ThreadingTCPServer):
	allow_reuse_address = True

	def __init__(self):
		global debug_enabled
		global conn_pool_size
		global db_uri

		log('%s %s.%s starting...' % (NAME, VERSION_MAJOR, VERSION_MINOR))

		signal.signal(signal.SIGHUP, sighup_handle)

		config = ConfigParser.RawConfigParser()
		config.readfp(open(AGI_CONFFILE))

		try:
			debug_enabled = config.getboolean("general", "debug")
		except ConfigParser.NoOptionError:
			debug_enabled = False

		try:
			listen_addr = config.get("general", "listen_addr")
		except ConfigParser.NoOptionError:
			listen_addr = LISTEN_ADDR_DEFAULT

		try:
			listen_port = config.getint("general", "listen_port")
		except ConfigParser.NoOptionError:
			listen_port = LISTEN_PORT_DEFAULT

		try:
			conn_pool_size = config.getint("general", "conn_pool_size")
		except ConfigParser.NoOptionError:
			conn_pool_size = CONN_POOL_SIZE_DEFAULT

		db_uri = config.get("db", "db_uri")

		debug("debug: %s" % debug_enabled)
		debug("listen_addr: %s" % listen_addr)
		debug("listen_port: %d" % listen_port)
		debug("conn_pool_size: %d" % conn_pool_size)
		debug("db_uri: %s" % db_uri)

		SocketServer.ThreadingTCPServer.__init__(self,
                    (listen_addr, listen_port),
                    FastAGIRequestHandler)

class Module:
	def __init__(self, module_name, setup_fn, handle_fn):
		self.module_name = module_name
		self.setup_fn = setup_fn
		self.handle_fn = handle_fn
		self.lock = moresynchro.RWLock()

		if not self.handle_fn:
			raise ValueError("invalid module handler")

	def setup(self, cursor):
		if self.setup_fn:
			self.setup_fn(cursor)

	def reload(self, cursor):
		if not self.setup_fn:
			return

		self.lock.acquire_write()
		self.setup_fn(cursor)
		debug('module "%s" reloaded' % self.module_name)
		self.lock.release()

	def handle(self, request_handler, agi, cursor, args):
		self.lock.acquire_read()
		self.handle_fn(request_handler, agi, cursor, args)
		self.lock.release()

def log(s):
	print "%s: %s" % (NAME, s)

def warning(s):
	log("WARNING: %s" % s)

def error(s):
	log("ERROR: %s" % s)
	sys.exit(1)

def debug(s):
	if debug_enabled:
		log("DEBUG: %s" % s)

def register(handle_fn, setup_fn = None):
	module_name = handle_fn.__name__

	if module_name in modules:
		raise ValueError('module "%s" already registered' % module_name)

	modules[module_name] = Module(module_name, setup_fn, handle_fn)

def display_conns():
	debug("connection pool: size = %d, available connections = %d" % (conn_pool_size, len(conns)))

def acquire_conn():
	try:
		conn = conns.pop()
		debug("got connection from pool")
	except IndexError:
		conn = anysql.connect_by_uri(db_uri)
		debug("pool empty, created new connection")

	display_conns()
	return conn

def release_conn(conn):
	if not conn:
		return

	conns_lock.acquire()

	if len(conns) < conn_pool_size:
		conns.append(conn)
		debug("pool not full, refilled with connection")
	else:
		conn.close()
		debug("pool full, connection closed")

	display_conns()

	conns_lock.release()

def sighup_handle(signum, frame):
	try:
		conn = acquire_conn()
		cursor = conn.cursor()

		for module in modules.itervalues():
			module.reload(cursor)

		conn.commit()
	finally:
		release_conn(conn)

def run():
	while len(conns) < conn_pool_size:
		conns.append(anysql.connect_by_uri(db_uri))

	cursor = conns[0].cursor()

	debug("list of modules: %s" % ', '.join(sorted(modules.keys())))

	for module in modules.itervalues():
		module.setup(cursor)

	server.serve_forever()

server = AGID()
