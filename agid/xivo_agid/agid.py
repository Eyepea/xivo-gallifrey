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

from xivo_agid import fastagi

NAME = "agid"
VERSION_MAJOR = 0
VERSION_MINOR = 1
AGI_CONFFILE = "/etc/asterisk/xivo_agi.conf"
LISTEN_ADDR_DEFAULT = "127.0.0.1"
LISTEN_PORT_DEFAULT = 4573
CONN_POOL_SIZE_DEFAULT = 10

server = None
modules = {}
debug_enabled = False

class DBConnectionPool:
	def __init__(self):
		self.conns = []
		self.size = 0
		self.db_uri = None
		self.lock = Lock()

	def reload(self, size, db_uri):
		self.lock.acquire()
		try:
			for conn in self.conns:
				conn.close()

			del self.conns[:]

			while len(self.conns) < size:
				self.conns.append(anysql.connect_by_uri(db_uri))

			self.size = size
			self.db_uri = db_uri
			debug("reloaded db conn pool")
			debug(self)
		finally:
			self.lock.release()

	def acquire(self):
		self.lock.acquire()
		try:
			try:
				conn = self.conns.pop()
				debug("acquiring connection: got connection from pool")
			except IndexError:
				conn = anysql.connect_by_uri(self.db_uri)
				debug("acquiring connection: pool empty, created new connection")
		finally:
			debug(self)
			self.lock.release()

		return conn

	def release(self, conn):
		self.lock.acquire()
		try:
			if len(self.conns) < self.size:
				self.conns.append(conn)
				debug("releasing connection: pool not full, refilled with connection")
			else:
				conn.close()
				debug("releasing connection: pool full, connection closed")

		finally:
			debug(self)
			self.lock.release()

	# The connection pool lock must be hold.
	def __str__(self):
		return ("connection pool: size = %d\n"
			"connection pool: available connections = %d\n"
			"connection pool: db_uri = %s") % (self.size, len(self.conns), self.db_uri)

class FastAGIRequestHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		try:
			debug("handling request")

			self.fastagi = fastagi.FastAGI(self.rfile, self.wfile)
			self.except_hook = agitb.Hook(agi = self.fastagi)

			conn = server.db_conn_pool.acquire()
			self.cursor = conn.cursor()

			module_name = self.fastagi.env['agi_network_script']
			debug("delegating request handling to module %s" % module_name)
			modules[module_name].handle(self.fastagi, self.cursor, self.fastagi.args)

			conn.commit()

			self.fastagi.verbose('AGI module "%s" successfully executed' % module_name)
			debug("request successfully handled")

		# Attempt to relay errors to Asterisk, but if it fails, we
		# just give up.
		# XXX It may be here that dropping database connection
		# exceptions could be catched.
		except fastagi.FastAGIDialPlanBreak, message:
			debug("invalid request, dial plan broken")

			try:
				self.fastagi.verbose(message)
			except:
				pass
		except:
			debug("an unexpected error occurred")

			try:
				self.except_hook.handle()
			except:
				pass

		server.db_conn_pool.release(conn)

class AGID(SocketServer.ThreadingTCPServer):
	allow_reuse_address = True
	initialized = False

	def __init__(self):
		log('%s %s.%s starting...' % (NAME, VERSION_MAJOR, VERSION_MINOR))

		signal.signal(signal.SIGHUP, sighup_handle)

		self.db_conn_pool = DBConnectionPool()
		self.setup()

		SocketServer.ThreadingTCPServer.__init__(self,
			(self.listen_addr, self.listen_port),
			FastAGIRequestHandler)

		self.initialized = True

	def setup(self):
		config = ConfigParser.RawConfigParser()
		config.readfp(open(AGI_CONFFILE))

		if not self.initialized:
			try:
				self.listen_addr = config.get("general", "listen_addr")
			except ConfigParser.NoOptionError:
				self.listen_addr = LISTEN_ADDR_DEFAULT

			debug("listen_addr: %s" % self.listen_addr)

			try:
				self.listen_port = config.getint("general", "listen_port")
			except ConfigParser.NoOptionError:
				self.listen_port = LISTEN_PORT_DEFAULT

			debug("listen_port: %d" % self.listen_port)

		try:
			conn_pool_size = config.getint("general", "conn_pool_size")
		except ConfigParser.NoOptionError:
			conn_pool_size = CONN_POOL_SIZE_DEFAULT

		db_uri = config.get("db", "db_uri")
		self.db_conn_pool.reload(conn_pool_size, db_uri)

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

	def handle(self, agi, cursor, args):
		self.lock.acquire_read()
		self.handle_fn(agi, cursor, args)
		self.lock.release()

def log(s, prefix = None):
	for line in str(s).splitlines():
		print "%s: %s%s" % (NAME, prefix or "", line)

def warning(s):
	log(s, "WARNING: ")

def error(s):
	log(s, "ERROR: ")
	sys.exit(1)

def debug(s):
	if debug_enabled:
		log(s, "DEBUG: ")

def register(handle_fn, setup_fn = None):
	module_name = handle_fn.__name__

	if module_name in modules:
		raise ValueError('module "%s" already registered' % module_name)

	modules[module_name] = Module(module_name, setup_fn, handle_fn)

def sighup_handle(signum, frame):
	try:
		debug("reloading core engine")

		server.setup()

		debug("reloading modules")

		conn = server.db_conn_pool.acquire()
		cursor = conn.cursor()

		for module in modules.itervalues():
			module.reload(cursor)

		conn.commit()

		debug("finished reload")
	finally:
		server.db_conn_pool.release(conn)

def run():
	conn = server.db_conn_pool.acquire()
	cursor = conn.cursor()

	debug("list of modules: %s" % ', '.join(sorted(modules.keys())))

	for module in modules.itervalues():
		module.setup(cursor)

	conn.commit()
	server.db_conn_pool.release(conn)

	server.serve_forever()

def init(debugging_on = False):
	global debug_enabled
	global server

	debug_enabled = debugging_on
	server = AGID()
