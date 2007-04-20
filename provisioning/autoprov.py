#!/usr/bin/python
# -*- coding: iso-8859-15 -*-
# Dependencies : python-sqlite wget

import sys, traceback, string, sqlite
import os, threading, traceback
import cgi, syslog

import BaseHTTPServer
from BaseHTTPServer import HTTPServer
from BaseHTTPServer import BaseHTTPRequestHandler

import SocketServer
from SocketServer import socket

import provsup
from Phones import *

DB	    = "/var/lib/asterisk/astsqlite"
# DB          = "/home/xilun/agi-xivo/truc.db" 
SQLITE_BUSY_TIMEOUT = 100

# === used all over this file: ===
TABLE	    = "phone"
UF_TABLE    = "userfeatures"
SIP_TABLE   = "usersip"

# right now the only allowed tech is SIP:
TECH        = "sip"

# Informations backend
class SQLiteDB:

	def __init__(self, db):
		self.db = db

	# === INTERNAL FUNCTIONS ===
	
	# does a generic SQL request with SQLite
	def generic_sqlite_request(self, foobar, *remain):
		conn = sqlite.connect(self.db)
		conn.db.sqlite_busy_timeout(SQLITE_BUSY_TIMEOUT)
		try:
			a = foobar(conn, *remain)
		except:
			provsup.log_debug_current_exception()
			a = None
		conn.close()
		return a

	# used by generic_sqlite_select as a "foobar" for generic_sqlite_request
	def select_func(self, conn, request, parameters_tuple, mapping):
		cursor = conn.cursor()
		cursor.execute(request, parameters_tuple)
		r = cursor.fetchone()
		if not r:
			syslog.syslog(syslog.LOG_WARNING, "No result for request " + request)
			return None
		a = {}
		for k,v in mapping.iteritems():
			a[k] = provsup.elem_or_none(r, v)
		return a

	# does a SELECT query with SQLite
	def generic_sqlite_select(self, request, parameters_tuple, mapping):
		return self.generic_sqlite_request(self.select_func, request, parameters_tuple, mapping)

	# used by generic_sqlite_select as a "foobar" for generic_sqlite_request
	def replace_func(self, conn, request, parameters_tuple):
		cursor = conn.cursor()
		cursor.execute(request, parameters_tuple)
		conn.commit()
		return None

	# does a REPLACE query with SQLite
	def generic_sqlite_replace(self, request, parameters_tuple):
		return self.generic_sqlite_request(self.replace_func, request, parameters_tuple)

	# === ENTRY POINTS ===

	# lookup vendor and model of the phone in the DB, by mac address
	def type_by_macaddr(self, macaddr):
		query = "SELECT * FROM %s WHERE macaddr=%s" % (TABLE, '%s')
		mapping = dict(map(lambda x: (x,x), ("macaddr", "vendor", "model")))
		return self.generic_sqlite_select(query, (macaddr,), mapping)

	# content of something_column in the UserFeatures
	# table will be matched against something_content
	def config_by_something_proto(self, something_column, something_content, proto):
		query = ("SELECT %s.*,%s.* " +
			  "FROM %s LEFT OUTER JOIN %s " +
				"ON %s.protocolid=%s.id AND %s.protocol=%s " +
			  "WHERE %s." + something_column + "=%s")\
			% ( UF_TABLE, SIP_TABLE,
			    UF_TABLE, SIP_TABLE,
			    UF_TABLE, SIP_TABLE, UF_TABLE, '%s',
			    UF_TABLE, '%s')
		mapping = {
			"name":			SIP_TABLE+".callerid",
			"ident":		SIP_TABLE+".name",
			"passwd":		SIP_TABLE+".secret",
			"number":		UF_TABLE+".number",
			"iduserfeatures":	UF_TABLE+".id",
			"provcode":		UF_TABLE+".provisioningid",
			"proto":		UF_TABLE+".protocol"
		}
		return self.generic_sqlite_select(query,
				(proto, something_content), mapping)

	# lookup the configuration information of the phone in the DB,
	# by ID of the UserFeatures table, and protocol
	def config_by_iduserfeatures_proto(self, iduserfeatures, proto):
		return self.config_by_something_proto(
				"id", iduserfeatures, proto)

	# lookup the configuration information of the phone in the DB,
	# by provisioning code and protocol
	def config_by_provcode_proto(self, provcode, proto):
		return self.config_by_something_proto(
				"provisioningid", provcode, proto)

	# save phone informations in the DB
	def save_phone(self, phone):
		self.generic_sqlite_replace(
			"REPLACE INTO %s (macaddr, vendor, model, proto, iduserfeatures) VALUES (%s, %s, %s, %s, %s)" \
			% (TABLE, '%s', '%s', '%s', '%s', '%s'),
			map(lambda x: phone[x], ('macaddr', 'vendor', 'model', 'proto', 'iduserfeatures')))

def daemonize():
	try:
		pid = os.fork()
		if pid > 0:
			sys.exit(0)
	except OSError, e:
		log_debug_current_exception()
		sys.exit(1)
	os.setsid()
	os.umask(0)
	try:
		pid = os.fork()
		if pid > 0:
			sys.exit(0)
        except OSError, e:
		log_debug_current_exception()
		sys.exit(1)
	dev_null = file('/dev/null', 'r+')
	os.dup2(dev_null.fileno(), sys.stdin.fileno())
	os.dup2(dev_null.fileno(), sys.stdout.fileno())
	os.dup2(dev_null.fileno(), sys.stderr.fileno())

# We want to do one provisioning at a time for a given mac address
class MacLocks:
	def __init__(self):
		self.lock = threading.Lock()
		self.maclocked = []
	def try_acquire(self, macaddr):
		r = False
		self.lock.acquire()
		try:
			if macaddr not in self.maclocked:
				r = True
				self.maclocked.append(macaddr)
		finally:
			self.lock.release()
		return r
	def release(self, macaddr):
		self.lock.acquire()
		try:
			if macaddr not in self.maclocked:
				raise macaddr + " not locked"
			self.maclocked.remove(macaddr)
		finally:
			self.lock.release()

class MissingParam(Exception):
	pass

class ProvHttpHandler(BaseHTTPRequestHandler):

	def __init__(self, request, client_address, server):
		self.my_server = server
		self.my_infos = self.my_server.my_infos
		self.my_maclocks = self.my_server.my_maclocks
		self.posted = None
		BaseHTTPRequestHandler.__init__(self, request, client_address, server)

	# HTTP responses
	def send_response_headers_200(self, content_type = "text/plain"):
		self.send_response(200)
		self.send_header("Content-Type", content_type)
		self.send_header("Cache-Control", "no-cache")
		self.send_header("Pragma", "no-cache")
		self.end_headers()
	def send_response_lines(self, req_lines):
		self.send_response_headers_200()
		self.wfile.writelines(map(lambda x: x+"\r\n", req_lines))
	def answer_404(self, err_str = None):
		self.send_error(404, err_str)
	def send_error_explain(self, errno, perso_message):
		self.send_response(errno)
		self.end_headers()
		self.wfile.write(self.error_message_format % {
			'code': errno,
			'message': perso_message,
			'explain': self.responses[errno][1]
		})

	# Get POSTED informations
	def get_posted(self):
		lines = []
		# This will raise an exception if not present or not an integer
		# This is the wanted behavior
		datalength = int(self.headers['Content-Length'])
		readbytes = 0
		if not datalength:
			return {}
		line = self.rfile.readline()
		while line and line.strip() and readbytes < datalength:
			lines.append(tuple(line.strip().split('=', 1)))
			readbytes += len(line)
			if readbytes < datalength:
				line = self.rfile.readline()
			else:
				line = None
		self.posted = dict(lines)
	def save_posted_vars(self, d, variables):
		for v in variables:
			if v not in self.posted:
				raise MissingParam, v + " missing in posted command"
			d[v] = self.posted[v]

	# Get infos and basic checks on wanted messages
	def save_macaddr_ipv4(self, phone):
		if "macaddr" not in self.posted:
			raise "Mac Address not given"
		phone["macaddr"] = provsup.normalize_mac_address(self.posted["macaddr"])
		if "ipv4" in self.posted:
			phone["ipv4"] = self.posted["ipv4"]

	def save_iduserfeatures_or_provcode(self, phone):
		try:
			self.save_posted_vars(phone, ("iduserfeatures",))
		except MissingParam:
			self.save_posted_vars(phone, ("provcode",))

	def posted_infos(self, *thetuple):
		phone = {}
		self.save_posted_vars(phone, thetuple)
		self.save_iduserfeatures_or_provcode(phone)
		self.save_macaddr_ipv4(phone)
		return phone

	def posted_phone_infos(self):
		return self.posted_infos("proto", "vendor", "model", "actions")

	def posted_light_infos(self):
		return self.posted_infos("proto", "actions")

	# Override default logging method
	def log_message(self, fmt, *args):
		syslog.syslog(fmt % args)

	# Main provisioning function
	def handle_prov(self):
	    phone = None
	    try:
		syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): parsing posted informations")
		self.get_posted()
		if "mode" not in self.posted:
			syslog.syslog(syslog.LOG_ERR, "handle_prov(): No mode posted")
			raise "No mode posted"

		syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): creating phone internal representation...")
		if self.posted["mode"] == "authoritative" or \
		   self.posted["mode"] == "informative":
			syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): ... using full posted infos.")
			phone = self.posted_phone_infos()
		elif self.posted["mode"] == "notification":
			syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): ... using light posted infos...")
			phone = self.posted_light_infos()
			syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): ... using light posted infos...")
			phonetype = self.my_infos.type_by_macaddr(phone["macaddr"])
			phone["vendor"] = provsup.elem_or_none(phonetype, "vendor")
			phone["model"] = provsup.elem_or_none(phonetype, "model")
		else:
			syslog.syslog(syslog.LOG_ERR, "handle_prov(): Unknown mode %s" % (self.posted["mode"],))
			raise "Unknown mode %s" % (self.posted["mode"],)

		# I only allow "sip" right now
		if phone["proto"] != TECH:
			syslog.syslog(syslog.LOG_ERR,			       \
				"handle_prov(): the only protocol supported " +\
				"right now is SIP, but I got %s" \
				% phone["proto"])
			raise "Unknown protocol '%s' != sip" % (phone["proto"],)

		syslog.syslog("handle_prov(): handling phone %s" % str(phone))

		if (not phone["vendor"]) or (not phone["model"]):
			syslog.syslog(syslog.LOG_ERR, "handle_prov(): Missing model or vendor in phone %s" % str(phone))
			raise "Missing model or vendor in phone %s" % str(phone)

		if "provcode" in phone and phone["provcode"] != "0" and \
		   not provsup.well_formed_provcode(phone["provcode"]):
			syslog.syslog(syslog.LOG_ERR, "handle_prov(): Invalid provcode %s" % (phone["provcode"],))
			raise "Invalid provcode %s" % (phone["provcode"],)

		if phone["actions"] != "no":
			if "ipv4" not in phone:
				syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): trying to get IPv4 address from Mac Address %s" % (phone["macaddr"],))
				phone["ipv4"] = provsup.ipv4_from_macaddr(phone["macaddr"])
			if phone["ipv4"] is None:
				syslog.syslog(syslog.LOG_ERR, "handle_prov(): No IP address found for Mac Address %s" % (phone["macaddr"],))
				raise "No IP address found for Mac Address %s" % (phone["macaddr"],)

		syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): locking %s" % (phone["macaddr"],))
		if not self.my_maclocks.try_acquire(phone["macaddr"]):
		    syslog.syslog(syslog.LOG_WARNING, "handle_prov(): Provisioning already in progress for %s" % (phone["macaddr"],))
		    raise "Provisioning already in progress for %s" % (phone["macaddr"],)
		try:
		    syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): phone class from vendor")
		    prov_class = provsup.PhoneClasses[phone["vendor"]] # TODO also use model
		    syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): phone instance from class")
		    prov_inst = prov_class(phone)

		    if "provcode" in phone and phone["provcode"] == "0":
			    phone["iduserfeatures"] = "0"

		    if "iduserfeatures" in phone and phone["iduserfeatures"] == "0":
			syslog.syslog("handle_prov(): reinitializing provisioning to GUEST for %s" % (str(phone),))
			prov_inst.reinitprov()
		    else:			
			if "iduserfeatures" in phone:
			    syslog.syslog("handle_prov(): getting configuration from iduserfeatures for phone %s" % (str(phone),))
			    config = self.my_infos.config_by_iduserfeatures_proto(phone["iduserfeatures"], phone["proto"])
			else:
			    syslog.syslog("handle_prov(): getting configuration from provcode for phone %s" % (str(phone),))
			    config = self.my_infos.config_by_provcode_proto(phone["provcode"], phone["proto"])
			    # XXX TODO test if config_by_provcode returned useful infos
			    # or send error informations to the caller (ultimately 
			    # http client)
			if config is not None:
			    syslog.syslog("handle_prov(): AUTOPROV'isioning phone %s with config %s" % (str(phone),str(config)))
			    prov_inst.autoprov(config)
			else:
			    syslog.syslog(syslog.LOG_ERR, "handle_prov(): not AUTOPROV'isioning phone %s cause no config found" % (str(phone),))

		    if "iduserfeatures" not in phone and config is not None and \
		       "iduserfeatures" in config:
			phone["iduserfeatures"] = config["iduserfeatures"]

		    if self.posted["mode"] != "informative" and "iduserfeatures" in phone:
			    syslog.syslog("handle_prov(): SAVING phone %s informations to backend" % (str(phone),))
			    self.my_infos.save_phone(phone)
		finally:
		    syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): unlocking %s" % (phone["macaddr"],))
		    self.my_maclocks.release(phone["macaddr"])
	    except:
		syslog.syslog("handle_prov(): provisioning FAILED for phone %s" % (str(phone),))
		tb_line_list = traceback.format_exception(*sys.exc_info())
		err_to_send = "<pre>\n" + cgi.escape(''.join(tb_line_list)) + "</pre>\n"
		for line in tb_line_list:
			syslog.syslog(syslog.LOG_DEBUG, line.rstrip())
		self.send_error_explain(500, err_to_send) # XXX
		return
	    syslog.syslog("handle_prov(): provisioning OK for phone %s" % (str(phone),))
	    self.send_response_lines(('Ok',))

	# Returns the list of supported Vendors / Models
	def handle_list(self):
		self.send_response_headers_200()
		for phonekey,phoneclass in provsup.PhoneClasses.iteritems():
			phonelabel = phoneclass.label
			phones = phoneclass.get_phones()
			self.wfile.write(phonekey + '="' + phonelabel.replace('"','\\"') + '"\r\n')
			self.wfile.writelines(map(lambda x: phonekey + '.' + x[0] + '="' + x[1].replace('"','\\"') + '"\r\n', phones))

	# === ENTRY POINTS ===

	def do_POST(self):
		if self.path == '/prov':
			self.handle_prov()
		else: self.answer_404()
	def do_GET(self):
		if self.path == '/list':
			self.handle_list()
		else: self.answer_404()

class ThreadingHTTPServer(SocketServer.ThreadingTCPServer):
	allow_reuse_address = 1    # Seems to make sense in testing environment
	def server_bind(self):
		"""Override server_bind to store the server name."""
		SocketServer.TCPServer.server_bind(self)
		host, port = self.socket.getsockname()[:2]
		self.server_name = socket.getfqdn(host)
		self.server_port = port

def main():
	syslog.openlog('autoprovisioning', 0, syslog.LOG_DAEMON)
#	TODO: add syslog filtering
	daemonize() # todo conditional
	http_server = ThreadingHTTPServer((provsup.LISTEN_IPV4, provsup.LISTEN_PORT), ProvHttpHandler)
	http_server.my_infos = SQLiteDB(DB)
	http_server.my_maclocks = MacLocks()
	http_server.serve_forever()
	syslog.closelog()

if sqlite.paramstyle != 'pyformat':
	raise "This script expect pysqlite 1 with sqlite.paramstyle != 'pyformat', but sqlite.paramstyle has been detected as %s" % sqlite.paramstyle

main()
