#!/usr/bin/python
# -*- coding: iso-8859-15 -*-
"""Autoprovisioning daemon for Xivo"""

# Dependencies : python-sqlite wget

import sys, traceback, string, sqlite
import os, threading, traceback
import cgi, syslog
import getopt

import BaseHTTPServer
from BaseHTTPServer import HTTPServer
from BaseHTTPServer import BaseHTTPRequestHandler

import SocketServer
from SocketServer import socket

import provsup
# Phones is a package containing one module per vendor
from Phones import *

# moresynchro contains R/W Locks
import moresynchro

import daemonize

DB	    = "/var/lib/asterisk/astsqlite"
# sqlite timeout is in ms
SQLITE_BUSY_TIMEOUT = 100

# === used all over this file: ===
TABLE	    = "phone"
UF_TABLE    = "userfeatures"
SIP_TABLE   = "usersip"

# right now the only allowed tech is SIP:
TECH        = "sip"

# deletion (W) / provisioning (R) lock timeout in secs
DEL_OR_PROV_TIMEOUT = 55

def name_from_first_last(first, last):
	"Construct full name from first and last."
	if first and last:
		return first + ' ' + last
	if first:
		return first
	if last:
		return last
	return ''

class SQLiteDB:
	"""An information backend for this provisioning daemon,
	using the Xivo SQLite database.
	
	"""
	def __init__(self, db):
		self.__db = db

	# === INTERNAL FUNCTIONS ===
	
	def generic_sqlite_request(self, foobar, *remain):
		"""Generic function to generate a safe context to issue any
		SQLite request.
		
		"""
		conn = sqlite.connect(self.__db)
		conn.db.sqlite_busy_timeout(SQLITE_BUSY_TIMEOUT)
		try:
			a = foobar(conn, *remain)
		except:
			provsup.log_debug_current_exception()
			a = None
		conn.close()
		return a

	def generic_sqlite_select(self, request, parameters_tuple, mapping):
		"Does a SELECT SQLite query."
		return self.generic_sqlite_request(self.__select_func, request, parameters_tuple, mapping)
	def __select_func(self, conn, request, parameters_tuple, mapping):
		"Internally called in a safe context to execute SELECT queries."
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

	def generic_sqlite_replace(self, request, parameters_tuple):
		"Does a REPLACE SQLite query."
		return self.generic_sqlite_request(self.__replace_func, request, parameters_tuple)
	def __replace_func(self, conn, request, parameters_tuple):
		"Internally called in a safe context to execute REPLACE queries."
		cursor = conn.cursor()
		cursor.execute(request, parameters_tuple)
		conn.commit()
		return None

	# === ENTRY POINTS ===

	# lookup vendor and model of the phone in the DB, by mac address
	def type_by_macaddr(self, macaddr):
		"""Lookup vendor and model of the phone in the database,
		by mac address.
		
		Returns a dictionary with the following keys:
		        "macaddr", "vendor", "model"

		"""
		query = "SELECT * FROM %s WHERE macaddr=%s" % (TABLE, '%s')
		mapping = dict(map(lambda x: (x,x), ("macaddr", "vendor", "model")))
		return self.generic_sqlite_select(query, (macaddr,), mapping)

	def config_by_something_proto(self, something_column, something_content, proto):
		"""Query the database to return a phone configuration.
		
		something_column - name of the column of the table 'userfeatures'
		                   used to select the right phone
		something_content - content to be match against the content of
				    the column identified by something_column
		
		Returns a dictionary with the following keys:
		        "firstname", "lastname", "name": user civil status
		        "iduserfeatures": user id in the userfeatures table
		        "provcode": provisioning code
		        "ident": protocol specific identification
		        "passwd": protocol specific password
		        "number": extension number
		        "proto": protocol
		
		"""
		query = ("SELECT %s.*,%s.* " +
			  "FROM %s LEFT OUTER JOIN %s " +
				"ON %s.protocolid=%s.id AND %s.protocol=%s " +
			  "WHERE %s." + something_column + "=%s")\
			% ( UF_TABLE, SIP_TABLE,
			    UF_TABLE, SIP_TABLE,
			    UF_TABLE, SIP_TABLE, UF_TABLE, '%s',
			    UF_TABLE, '%s')
		mapping = {
			"firstname":		UF_TABLE+".firstname",
			"lastname":		UF_TABLE+".lastname",
			"ident":		SIP_TABLE+".name",
			"passwd":		SIP_TABLE+".secret",
			"number":		UF_TABLE+".number",
			"iduserfeatures":	UF_TABLE+".id",
			"provcode":		UF_TABLE+".provisioningid",
			"proto":		UF_TABLE+".protocol"
		}
		confdico = self.generic_sqlite_select(query,
				(proto, something_content), mapping)
		confdico["name"] = name_from_first_last(confdico["firstname"],
							confdico["lastname"])
		return confdico

	def config_by_iduserfeatures_proto(self, iduserfeatures, proto):
		"""Lookup the configuration information of the phone in the
		database, by ID of the 'userfeatures' table, and protocol.
		
		"""
		return self.config_by_something_proto(
				"id", iduserfeatures, proto)

	def config_by_provcode_proto(self, provcode, proto):
		"""Lookup the configuration information of the phone in the
		database, by provisioning code and protocol.
		
		"""
		return self.config_by_something_proto(
				"provisioningid", provcode, proto)

	def save_phone(self, phone):
		"Save phone informations in the database."
		self.generic_sqlite_replace(
			"REPLACE INTO %s (macaddr, vendor, model, proto, iduserfeatures) VALUES (%s, %s, %s, %s, %s)" \
			% (TABLE, '%s', '%s', '%s', '%s', '%s'),
			map(lambda x: phone[x], ('macaddr', 'vendor', 'model', 'proto', 'iduserfeatures')))

	def user_deletion(self, userinfo):
		pass

class MacLocks:
	"""This class let us enforce that only one provisioning is in progress
	at any time for a given mac address.
	
	"""
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
	"""ThreadingHTTPServer will create one instance of this class for the
	handling of each incoming connection received by this daemon, and each
	instance will run in a separated thread.
	Base methods of BaseHTTPRequestHandler are used to do the basic HTTP
	parsing of the HTTP request that is to be handled, and new methods are
	added that implement the high level behavior of autoprovisioning and
	related operations.
	
	"""
	def __init__(self, request, client_address, server):
		self.my_server = server
		self.my_infos = self.my_server.my_infos
		self.my_maclocks = self.my_server.my_maclocks
		self.my_del_or_prov_lock = self.my_server.my_del_or_prov_lock
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

	# Get POSTED informations in self.posted
	def get_posted(self):
		"""Extract a payload of a POST HTTP method that has the
		following format:
		
		name1=value1
		name2=value2
		...
		nameN=valueN
		
		A dictionary {'name1':'value1,
		              'name2':'value2',
			      ...
			      'nameN':'valueN' } is then stored in self.posted
		"""
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

	# Extract interesting stuff from self.posted
	def save_posted_vars(self, d, variables):
		"""Store in the dictionary d each variable of the variables
		list present in self.posted, raising an exception if some
		are missing.
		
		"""
		for v in variables:
			if v not in self.posted:
				raise MissingParam, v + " missing in posted command"
			d[v] = self.posted[v]
	def save_macaddr_ipv4(self, phone):
		"""Save 'macaddr' in phone from self.posted or raise a
		MissingParam exception, and optionally save 'ipv4'.
		
		"""
		if "macaddr" not in self.posted:
			raise MissingParam, "Mac Address not given"
		phone["macaddr"] = provsup.normalize_mac_address(self.posted["macaddr"])
		if "ipv4" in self.posted:
			phone["ipv4"] = self.posted["ipv4"]
	def save_iduserfeatures_or_provcode(self, phone):
		"""Save 'iduserfeatures' in phone from self.posted in priority,
		or save 'provcode' if 'iduserfeatures' was not present.
		
		"""
		try:
			self.save_posted_vars(phone, ("iduserfeatures",))
		except MissingParam:
			self.save_posted_vars(phone, ("provcode",))

	def posted_infos(self, *thetuple):
		"""Generic function to retrieve every needed provisioning info
		from the request.
		
		"""
		phone = {}
		self.save_posted_vars(phone, thetuple)
		self.save_iduserfeatures_or_provcode(phone)
		self.save_macaddr_ipv4(phone)
		return phone
	def posted_phone_infos(self):
		"Used in 'authoritative' and 'informative' modes"
		return self.posted_infos("proto", "vendor", "model", "actions")
	def posted_light_infos(self):
		"Used in 'notification' mode"
		return self.posted_infos("proto", "actions")
	def posted_userdeleted_infos(self):
		"Used in 'userdeleted' mode"
		userinfo = {}
		self.save_posted_vars(userinfo, ("iduserfeatures","actions"))
		return userinfo

	def log_message(self, fmt, *args):
		"Override default logging method."
		syslog.syslog(fmt % args)

	# High level logic
	def do_userdeleted(self, userinfo):
		"Does what has to be done when a user is deleted."
		syslog.syslog("do_userdeleted(): handling deletion of user %s" % str(userinfo))
		self.my_infos.user_deletion(userinfo)
	def do_provisioning(self, phone):
		"Provisioning high level logic for the described phone"
		# I only allow "sip" right now
		if phone["proto"] != TECH:
			syslog.syslog(syslog.LOG_ERR,			       \
				"do_provisioning(): the only protocol supported " +\
				"right now is SIP, but I got %s" \
				% phone["proto"])
			raise "Unknown protocol '%s' != sip" % (phone["proto"],)

		syslog.syslog("do_provisioning(): handling phone %s" % str(phone))

		if (not phone["vendor"]) or (not phone["model"]):
			syslog.syslog(syslog.LOG_ERR, "do_provisioning(): Missing model or vendor in phone %s" % str(phone))
			raise "Missing model or vendor in phone %s" % str(phone)

		if "provcode" in phone and phone["provcode"] != "0" and \
		   not provsup.well_formed_provcode(phone["provcode"]):
			syslog.syslog(syslog.LOG_ERR, "do_provisioning(): Invalid provcode %s" % (phone["provcode"],))
			raise "Invalid provcode %s" % (phone["provcode"],)

		if phone["actions"] != "no":
			if "ipv4" not in phone:
				syslog.syslog(syslog.LOG_DEBUG, "do_provisioning(): trying to get IPv4 address from Mac Address %s" % (phone["macaddr"],))
				phone["ipv4"] = provsup.ipv4_from_macaddr(phone["macaddr"])
			if phone["ipv4"] is None:
				syslog.syslog(syslog.LOG_ERR, "do_provisioning(): No IP address found for Mac Address %s" % (phone["macaddr"],))
				raise "No IP address found for Mac Address %s" % (phone["macaddr"],)

		syslog.syslog(syslog.LOG_DEBUG, "do_provisioning(): locking %s" % (phone["macaddr"],))
		if not self.my_maclocks.try_acquire(phone["macaddr"]):
		    syslog.syslog(syslog.LOG_WARNING, "do_provisioning(): Provisioning already in progress for %s" % (phone["macaddr"],))
		    raise "Provisioning already in progress for %s" % (phone["macaddr"],)
		try:
		    syslog.syslog(syslog.LOG_DEBUG, "do_provisioning(): phone class from vendor")
		    prov_class = provsup.PhoneClasses[phone["vendor"]] # TODO also use model
		    syslog.syslog(syslog.LOG_DEBUG, "do_provisioning(): phone instance from class")
		    prov_inst = prov_class(phone)

		    if "provcode" in phone and phone["provcode"] == "0":
			    phone["iduserfeatures"] = "0"

		    if "iduserfeatures" in phone and phone["iduserfeatures"] == "0":
			syslog.syslog("do_provisioning(): reinitializing provisioning to GUEST for %s" % (str(phone),))
			prov_inst.reinitprov()
		    else:			
			if "iduserfeatures" in phone:
			    syslog.syslog(syslog.LOG_DEBUG, "do_provisioning(): getting configuration from iduserfeatures for phone %s" % (str(phone),))
			    config = self.my_infos.config_by_iduserfeatures_proto(phone["iduserfeatures"], phone["proto"])
			else:
			    syslog.syslog(syslog.LOG_DEBUG, "do_provisioning(): getting configuration from provcode for phone %s" % (str(phone),))
			    config = self.my_infos.config_by_provcode_proto(phone["provcode"], phone["proto"])
			    # XXX TODO test if config_by_provcode returned useful infos
			    # or send error informations to the caller (ultimately 
			    # http client)
			if config is not None:
			    syslog.syslog("do_provisioning(): AUTOPROV'isioning phone %s with config %s" % (str(phone),str(config)))
			    prov_inst.autoprov(config)
			else:
			    syslog.syslog(syslog.LOG_ERR, "do_provisioning(): not AUTOPROV'isioning phone %s cause no config found" % (str(phone),))

		    if "iduserfeatures" not in phone and config is not None and \
		       "iduserfeatures" in config:
			phone["iduserfeatures"] = config["iduserfeatures"]

		    if self.posted["mode"] != "informative" and "iduserfeatures" in phone:
			    syslog.syslog("do_provisioning(): SAVING phone %s informations to backend" % (str(phone),))
			    self.my_infos.save_phone(phone)
		finally:
		    syslog.syslog(syslog.LOG_DEBUG, "do_provisioning(): unlocking %s" % (phone["macaddr"],))
		    self.my_maclocks.release(phone["macaddr"])

	def lock_and_provision(self, phone):
		"""Will attempt to provision with a global lock
		hold in shared mode
		
		"""
		syslog.syslog(syslog.LOG_DEBUG, "Entering lock_and_provision(phone=%s)" % str(phone))
		if not self.my_del_or_prov_lock.acquire_read(DEL_OR_PROV_TIMEOUT):
			raise "Could not acquire the global lock in shared mode"
		try:
			self.do_provisioning(phone)
		finally:
			self.my_del_or_prov_lock.release()
			syslog.syslog(syslog.LOG_DEBUG, "Leaving lock_and_provision(phone=%s)" % str(phone))
	def lock_and_userdel(self, userinfo):
		"""Will attempt to delete informations related to the user
		described by userinfo from areas we are handling in the
		information backend, with a global lock hold in exclusive
		access.
		
		"""
		syslog.syslog(syslog.LOG_DEBUG, "Entering lock_and_userdel(userinfo=%s)" % str(userinfo))
		if not self.my_del_or_prov_lock.acquire_write(DEL_OR_PROV_TIMEOUT):
			raise "Could not acquire the global lock in exclusive mode"
		try:
			self.do_userdeleted(phone)
		finally:
			self.my_del_or_prov_lock.release()
			syslog.syslog(syslog.LOG_DEBUG, "Leaving lock_and_userdel(userinfo=%s)" % str(userinfo))

	# Main handling functions

	def handle_prov(self):
	    "Does whatever action is asked by the peer"
	    phone = None
	    userinfo = None
	    try:
		syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): parsing posted informations")
		self.get_posted()
		if "mode" not in self.posted:
			syslog.syslog(syslog.LOG_ERR, "handle_prov(): No mode posted")
			raise "No mode posted"

		if self.posted["mode"] == "authoritative" or \
		   self.posted["mode"] == "informative":
			syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): creating phone internal representation using full posted infos.")
			phone = self.posted_phone_infos()
			self.lock_and_provision(phone)
		elif self.posted["mode"] == "notification":
			syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): creating phone internal representation using light posted infos.")
			phone = self.posted_light_infos()
			phonetype = self.my_infos.type_by_macaddr(phone["macaddr"])
			phone["vendor"] = provsup.elem_or_none(phonetype, "vendor")
			phone["model"] = provsup.elem_or_none(phonetype, "model")
			self.lock_and_provision(phone)
		elif self.posted["mode"] == "userdeleted":
			syslog.syslog(syslog.LOG_DEBUG, "handle_prov(): user deletion")
			userinfo = self.posted_userdeleted_infos()
			self.lock_and_userdel(userinfo)
		else:
			syslog.syslog(syslog.LOG_ERR, "handle_prov(): Unknown mode %s" % (self.posted["mode"],))
			raise "Unknown mode %s" % (self.posted["mode"],)

	    except:
		syslog.syslog("handle_prov(): action FAILED - phone %s - userinfo %s" % (str(phone),str(userinfo)))
		tb_line_list = traceback.format_exception(*sys.exc_info())
		err_to_send = "<pre>\n" + cgi.escape(''.join(tb_line_list)) + "</pre>\n"
		for line in tb_line_list:
			syslog.syslog(syslog.LOG_DEBUG, line.rstrip())
		self.send_error_explain(500, err_to_send) # XXX
		return
	    syslog.syslog("handle_prov(): provisioning OK for phone %s" % (str(phone),))
	    self.send_response_lines(('Ok',))

	def handle_list(self):
		"Respond with the list of supported Vendors / Models"
		self.send_response_headers_200()
		for phonekey,phoneclass in provsup.PhoneClasses.iteritems():
			phonelabel = phoneclass.label
			phones = phoneclass.get_phones()
			self.wfile.write(phonekey + '="' + phonelabel.replace('"','\\"') + '"\r\n')
			self.wfile.writelines(map(lambda x: phonekey + '.' + x[0] + '="' + x[1].replace('"','\\"') + '"\r\n', phones))

	# === ENTRY POINTS (called FROM BaseHTTPRequestHandler) ===

	def do_POST(self):
		if self.path == '/prov':
			self.handle_prov()
		else: self.answer_404()
	def do_GET(self):
		if self.path == '/list':
			self.handle_list()
		else: self.answer_404()

class ThreadingHTTPServer(SocketServer.ThreadingTCPServer):
	"""Same as HTTPServer, but derivates from ThreadingTCPServer instead
	of TCPServer so that any instance of ProvHttpHandler created for any
	incoming connection runs in its own thread.
	
	"""
	allow_reuse_address = 1    # Seems to make sense in testing environment
	def server_bind(self):
		"""Override server_bind to store the server name."""
		SocketServer.TCPServer.server_bind(self)
		host, port = self.socket.getsockname()[:2]
		self.server_name = socket.getfqdn(host)
		self.server_port = port

def main(log_level, foreground):
	"""log_level - one of syslog.LOG_EMERG to syslog.LOG_DEBUG
	            nothing will be logged below this limit
	foreground - don't daemonize if true
	
	"""
	syslog.openlog('autoprovisioning', 0, syslog.LOG_DAEMON)
	syslog.setlogmask(syslog.LOG_UPTO(log_level))
	if not foreground:
		daemonize.daemonize()
	http_server = ThreadingHTTPServer((provsup.LISTEN_IPV4, provsup.LISTEN_PORT), ProvHttpHandler)
	http_server.my_infos = SQLiteDB(DB)
	http_server.my_maclocks = MacLocks()
	http_server.my_del_or_prov_lock = moresynchro.RWLock()
	http_server.serve_forever()
	syslog.closelog()

if sqlite.paramstyle != 'pyformat':
	raise "This script expect pysqlite 1 with sqlite.paramstyle != 'pyformat', but sqlite.paramstyle has been detected as %s" % sqlite.paramstyle

dontlauchmain = False
foreground = False
logmap = {
	'emerg':   syslog.LOG_EMERG,
	'alert':   syslog.LOG_ALERT,
	'crit':    syslog.LOG_CRIT,
	'err':     syslog.LOG_ERR,
	'warning': syslog.LOG_WARNING,
	'notice':  syslog.LOG_NOTICE,
	'info':    syslog.LOG_INFO,
	'debug':   syslog.LOG_DEBUG
}
log_level = syslog.LOG_NOTICE

# l: log filter up to EMERG, ALERT, CRIT, ERR, WARNING, NOTICE, INFO or DEBUG
# d: don't launch the main function (useful for python -i invocations)
# f: keep the program on foreground, don't daemonize
# b: override the default sqlite DB filename

opts,args = getopt.getopt(sys.argv[1:], 'b:l:df')
for k,v in opts:
	if '-l' == k:
		if v.lower() not in logmap:
			raise "Unknown log filter '%s'" % v
		log_level = logmap[v.lower()]
	elif '-d' == k:
		dontlauchmain = True
	elif '-f' == k:
		foreground = True
	elif '-b' == k:
		DB = v

if __name__ == '__main__' and not dontlauchmain:
	main(log_level, foreground)
