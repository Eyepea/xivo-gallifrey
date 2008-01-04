#!/usr/bin/python
"""Autoprovisioning daemon for Xivo

Copyright (C) 2007, 2008 Proformatique

"""

__version__ = "$Revision$ $Date$"

CONFIG_FILE		= "/etc/xivo/provisioning.conf" # can be overridden by cmd line param
GETOPT_SHORTOPTS	= "b:l:dfc:p:h"
PIDFILE			= "/var/run/autoprov.pid"
TABLE			= 'phone'
UF_TABLE		= 'userfeatures'
SIP_TABLE		= 'usersip'
FK_TABLE		= 'phonefunckey'
XNUM_TABLE		= 'extenumbers'
TECH			= 'sip' # only allowed tech right now

# encodings.latin_1 and _sre mainly for freezing
import encodings.latin_1
import _sre
import sys

def help_screen():
	print >> sys.stderr, \
"""Syntax:
%s [-b <dburi>] [-l <loglevel>] [-d] [-f] [-c <conffile>] [-p <pidfile>] [-h]

-b <dburi>	Override Database URI with <dburi>
-l <loglevel>	Emit traces with <loglevel> details, must be one of:
		emerg, alert, crit, err, warning, notice, info, debug
-d		Don't call the main function, for installation test purposes
-f		Foreground, don't daemonize
-c <conffile>	Use <conffile> instead of %s
-p <pidfile>	Use <pidfile> instead of %s
-h		Display this help screen and exit
""" % (sys.argv[0], repr(CONFIG_FILE), repr(PIDFILE))
	sys.exit(1)

# Magical Path modification:

import xivo.to_path
import xivo_provisioning.to_path

# Loading Xivo modules is possible from this point

from getopt import getopt, GetoptError

import timeoutsocket
from timeoutsocket import Timeout

import os
import re
import cgi
import thread
import threading

import syslog
from easyslog import *

import anysql
from BackSQL import backsqlite
from BackSQL import backmysql

from pyfunc import replace_keys

from BaseHTTPServer import BaseHTTPRequestHandler

from ThreadingHTTPServer import *

import provsup
from provsup import ProvGeneralConf as pgc

from moresynchro import RWLock
from moresynchro import ListLock
import daemonize

import except_tb

def name_from_first_last(first, last):
	"Construct full name from first and last."
	if first and last:
		return first + ' ' + last
	if first:
		return first
	if last:
		return last
	return ''

def field_empty(f):
	return (f is None) or (f == "")

find_ast_meta = re.compile('[[NXZ!.]').search
def pos_ast_meta(ast_pattern):
	mo = find_ast_meta(ast_pattern)
	if not mo:
		return None
	return mo.start()

def exten_from_parties(xleft, xright, fkext):
	"""Returns an extension that is purely function of xnext and fkext
	
	xleft - None or a string that is an Asterisk extension pattern from
	        which the initial underscore specifying that the remaining part
	        of the string is a pattern will be stripped, as well as
	        everything from the first variable part of the pattern to the
	        end of the string.
	        For example:
	            "_5."                => "5"
	            "_[5-7]."            => ""
	            "_666[3-689]XNZ!"    => "666"
	            "_42!XNZ!666[5-7]Z." => "42"
	        The stripped xleft will be the left part of the generated
	        extension.
	xright - None or a string that will be the right part of the generated
	         extension.
	fkext - like xright
	
	WARNING: xright and fkext shall not be both set.
	
	WARNING: what will happen when you pass a really incorrect and 
	stupid extension pattern is really unspecified (but should not
	trigger any exception)
	"""
	if xright and fkext:
		raise ValueError, "(xright, fkext) == " + `(xright, fkext)` + " but both shall not be set"
	elif xright:
		right_part = xright
	elif fkext:
		right_part = fkext
	else:
		right_part = ""
	exten = ""
	if xleft:
		if xleft[0] == '_':
			xleft = xleft[1:]
			e = pos_ast_meta(xleft)
			if e is not None:
				xleft = xleft[:e]
		exten += xleft
	if right_part:
		exten += right_part
	return exten

class SQLBackEnd:
	"""An information backend for this provisioning daemon,
	using the Xivo SQL database.
	
	"""
	def __init__(self, db):
		self.__db = db

	# === INTERNAL FUNCTIONS ===
	
	def generic_sql_request(self, foobar, *remain):
		"""Generic function to generate a safe context to issue any
		SQL request.
		
		"""
		conn = anysql.connect_by_uri(self.__db)
		try:
			a = foobar(conn, *remain)
		except:
			except_tb.syslog_exception()
			a = None
		conn.close()
		return a

	def sql_select_one(self, request, columns, parameters):
		"""Does a SELECT SQL query and returns only one row.
		If the query returns no result, this method returns None
		
		"""
		return self.generic_sql_request(self.method_select_one, request, columns, parameters)
	def method_select_one(self, conn, request, columns, parameters):
		"""Internally called in a safe context to execute SELECT
		SQL queries and return their result (only one row).
		
		"""
		cursor = conn.cursor()
		cursor.query(request, columns, parameters)
		r = cursor.fetchone()
		if not r:
			return r
		return dict(r.iteritems())

	def sql_select_all(self, request, columns, parameters):
		"Does a SELECT SQL query and returns all rows."
		return self.generic_sql_request(self.method_select_all, request, columns, parameters)
	def method_select_all(self, conn, request, columns, parameters):
		"""Internally called in a safe context to execute SELECT
		SQL queries and return their result (all rows).
		
		"""
		cursor = conn.cursor()
		cursor.query(request, columns, parameters)
		rows = cursor.fetchall()
		if not rows:
			return rows
		return [ dict(r.iteritems()) for r in rows ]

	def sql_modify(self, request, columns, parameters):
		"Does a SQL query that is going to modify the database."
		return self.generic_sql_request(self.method_commit, request, columns, parameters)
	def method_commit(self, conn, request, columns, parameters):
		"""Internally called in a safe context to commit the result of
		SQL queries that modify the database content.
		
		"""
		cursor = conn.cursor()
		cursor.query(request, columns, parameters)
		conn.commit()

	# === ENTRY POINTS ===

	def phone_by_macaddr(self, macaddr):
		"""Lookup a phone description by Mac Address in the database.
		
		Returns a dictionary with the following keys:
			'macaddr', 'vendor', 'model', 'proto', 'iduserfeatures', 'isinalan'
		or None
		
		"""
		return self.sql_select_one(
			"SELECT ${columns} FROM %s WHERE macaddr=%s" % (TABLE, '%s'),
			('macaddr', 'vendor', 'model', 'proto', 'iduserfeatures', 'isinalan'),
			(macaddr,))

	def config_by_something_proto(self, something_column, something_content, proto):
		"""Query the database to return a phone configuration.
		
		something_column - name of the column of the table 'userfeatures'
				   used to select the right phone
		something_content - content to be match against the content of
				    the column identified by something_column
		
		Returns a dictionary with the following keys:
			'firstname', 'lastname', 'name': user civil status
				note that the name is constructed from the
				first and last name
			'iduserfeatures': user id in the userfeatures table
			'provcode': provisioning code
			'ident': protocol specific identification
			'passwd': protocol specific password
			'dtmfmode': DTMF mode
			'simultcalls': number of simultaneous calls (multiline)
			'number': extension number
			'proto': protocol
			'funckey': mapping where keys are function key number
				and values a tuple (exten, supervise).
				.exten is the extension to speed dial/supervise
				.supervise is True if & only if supervision is
				 activated on this function key
		
		Right now proto must evaluate to 'sip'
		
		"""
		if proto != TECH:
			raise ValueError, "proto must be 'sip' for now"
		mapping = {
			UF_TABLE+".firstname":		'firstname',
			UF_TABLE+".lastname":		'lastname',
			SIP_TABLE+".name":		'ident',
			SIP_TABLE+".secret":		'passwd',
			SIP_TABLE+".dtmfmode":		'dtmfmode',
			UF_TABLE+".simultcalls":	'simultcalls',
			UF_TABLE+".number":		'number',
			UF_TABLE+".id":			'iduserfeatures',
			UF_TABLE+".provisioningid":	'provcode',
			UF_TABLE+".protocol":		'proto',
		}
		confdico = self.sql_select_one(
			("SELECT ${columns} "
			  "FROM %s LEFT OUTER JOIN %s "
				"ON %s.protocolid=%s.id AND %s.protocol=%s "
			  "WHERE %s." + something_column + "=%s")
			  % (UF_TABLE, SIP_TABLE,
			     UF_TABLE, SIP_TABLE, UF_TABLE, '%s',
			     UF_TABLE, '%s'),
			mapping.keys(),
			(proto, something_content))
		if not confdico:
			return None
		confdico = replace_keys(confdico, mapping)
		confdico['name'] = name_from_first_last(confdico['firstname'],
							confdico['lastname'])
		fklist = self.sql_select_all(
			("SELECT ${columns} "
			 "FROM %s "
			 "LEFT OUTER JOIN %s AS extenumleft "
			 "ON  %s.typeextenumbers = extenumleft.type "
			 "AND %s.typevalextenumbers = extenumleft.typeval "
			 "LEFT OUTER JOIN %s AS extenumright "
			 "ON  %s.typeextenumbersright = extenumright.type "
			 "AND %s.typevalextenumbersright = extenumright.typeval "
			 "WHERE iduserfeatures=%s")
			 % (FK_TABLE,
			    XNUM_TABLE,
			    FK_TABLE,
			    FK_TABLE,
			    XNUM_TABLE,
			    FK_TABLE,
			    FK_TABLE,
			    '%s'),
			[FK_TABLE+x for x in ('.fknum', '.exten', '.supervision')]
			+ ['extenumleft.exten', 'extenumright.exten'],
			(confdico['iduserfeatures'],))
		funckey = {}
		for fk in fklist:
			funckey[fk[FK_TABLE+'.fknum']] = (
				exten_from_parties(
					fk['extenumleft.exten'],
					fk['extenumright.exten'],
					fk[FK_TABLE+'.exten']),
				bool(int(fk[FK_TABLE+'.supervision']))
			)
		confdico['funckey'] = funckey
		return confdico

	def config_by_iduserfeatures_proto(self, iduserfeatures, proto):
		"""Lookup the configuration information of the phone in the
		database, by ID of the 'userfeatures' table, and protocol.
		
		Right now proto must evaluate to 'sip'
		
		"""
		return self.config_by_something_proto(
				'id', iduserfeatures, proto)

	def config_by_provcode_proto(self, provcode, proto):
		"""Lookup the configuration information of the phone in the
		database, by provisioning code and protocol.
		
		Right now proto must evaluate to 'sip'
		
		"""
		return self.config_by_something_proto(
				'provisioningid', provcode, proto)

	def save_phone(self, phone):
		"""Save phone informations in the database.
		phone must be a dictionary and contain the following keys:
		
		'macaddr', 'vendor', 'model', 'proto', 'iduserfeatures', 'isinalan'
		
		"""
		columns = ('macaddr', 'vendor', 'model', 'proto', 'iduserfeatures', 'isinalan')
		self.sql_modify(
			("REPLACE INTO %s (${columns})"
			 " VALUES (%s, %s, %s, %s, %s, %s)")
			 % (TABLE, '%s', '%s', '%s', '%s', '%s', '%s'),
			columns,
			[ phone[x] for x in columns ])

	def phone_by_iduserfeatures(self, iduserfeatures):
		"""Lookup phone information by user information (iduserfeatures)
		Right now this is limited to the 'sip' protocol, so the result
		is a single phone description in the form of a dictionary
		containing the classic keys.
		
		'macaddr', 'vendor', 'model', 'proto', 'iduserfeatures', 'isinalan'
		
		"""
		return self.sql_select_one(
			("SELECT ${columns} FROM %s "
			 "WHERE iduserfeatures=%s AND proto=%s")
			 % ( TABLE, '%s', '%s' ),
			('macaddr', 'vendor', 'model', 'proto', 'iduserfeatures', 'isinalan'),
			(iduserfeatures, TECH))

	def delete_phone_by_iduserfeatures(self, iduserfeatures):
		"""Delete any phone in the database having the given
		iduserfeatures.
		
		"""
		self.sql_modify(
			"DELETE FROM %s WHERE iduserfeatures=%s" % ( TABLE, '%s'),
			None,
			(iduserfeatures,))

	def find_orphan_phones(self):
		"""Find every phones that do not have a corresponding user
		anymore, but that are neither provisioned in state GUEST. Used
		at startup to maintain the coherency of the whole provisioning
		subsystem.
		
		Returns a list of dictionaries, each of the latter representing
		informations stored in the base about a phone with the classic
		keys :
		
		'macaddr', 'vendor', 'model', 'proto', 'iduserfeatures', 'isinalan'
		
		"""
		mapping = dict([(TABLE+'.'+x,x)
				for x in ('macaddr', 'vendor', 'model',
					  'proto', 'iduserfeatures', 'isinalan')])
		orphans = self.sql_select_all(
			( "SELECT ${columns} FROM %s LEFT JOIN %s"
			  " ON %s.iduserfeatures = %s.id"
			  " WHERE %s.iduserfeatures != 0 AND %s.id is NULL" )
			  % (TABLE, UF_TABLE,
			     TABLE, UF_TABLE,
			     TABLE, UF_TABLE),
			mapping.keys(),
			())
		if not orphans:
			return orphans
		return [replace_keys(row, mapping) for row in orphans]

class CommonProvContext:
	def __init__(self, userlocks, maclocks, dbinfos, rwlock):
		# There is no locking order because my ListLocks don't block.
		self.userlocks = userlocks
		self.maclocks = maclocks
		self.dbinfos = dbinfos
		self.rwlock = rwlock

class Error(Exception): pass
class BadRequest(Error): pass
class MissingParam(BadRequest): pass
class ConflictError(Error): pass
class NotFoundError(Error): pass

ExceptToHTTP = {
	BadRequest: 400,
	ConflictError: 409,
	NotFoundError: 404
}

def __mode_dependant_provlogic_locked(mode, ctx, phone, config, prev_iduserfeatures):
	"""This function resolves conflicts or abort by raising an exception
	for the current provisioning in progress.
	
	"""
	if mode == 'authoritative':
		syslogf(SYSLOG_DEBUG, "__mode_dependant_provlogic_locked() in authoritative mode for phone %s and user %s" % (phone['macaddr'], config['iduserfeatures']))
		existing_phone = ctx.dbinfos.phone_by_iduserfeatures(config['iduserfeatures'])
		if existing_phone and existing_phone['macaddr'] != phone['macaddr']:
			syslogf(SYSLOG_NOTICE, "__mode_dependant_provlogic_locked(): phone %s to be put back in guest state, because another one (%s) is being provisioned for the same user" % (existing_phone['macaddr'], phone['macaddr']))
			existing_phone['mode'] = 'authoritative'
			existing_phone['actions'] = 'no'
			existing_phone['iduserfeatures'] = '0'
			__provisioning('__internal_to_guest', ctx, existing_phone)
	elif mode == 'notification':
		syslogf(SYSLOG_DEBUG, "__mode_dependant_provlogic_locked() in notification mode for phone %s and user %s" % (phone['macaddr'], config['iduserfeatures']))
		if prev_iduserfeatures:
			if config['iduserfeatures'] != prev_iduserfeatures:
				syslogf(SYSLOG_ERR, "__mode_dependant_provlogic_locked(): outdated iduserfeatures received for update notification of phone %s; wanted %s; got %s" % (str(phone), prev_iduserfeatures, config['iduserfeatures']))
				raise ConflictError, "Outdated iduserfeatures received for update notification of phone %s; wanted %s; got %s" % (str(phone), prev_iduserfeatures, config['iduserfeatures'])
		existing_phone = ctx.dbinfos.phone_by_iduserfeatures(config['iduserfeatures'])
		if not existing_phone:
			syslogf(SYSLOG_ERR, "__mode_dependant_provlogic_locked(): non existing phone %s to update for iduserfeatures %s" % (str(phone), config['iduserfeatures']))
			raise ConflictError, "Non existing phone %s to update for iduserfeatures %s" % (str(phone), config['iduserfeatures'])
		if existing_phone['macaddr'] != phone['macaddr']:
			syslogf(SYSLOG_ERR, "__mode_dependant_provlogic_locked(): another phone %s already exists instead of %s for iduserfeatures %s" % (str(existing_phone), str(phone), config['iduserfeatures']))
			raise ConflictError, "Another phone %s already exists instead of %s for iduserfeatures %s" % (str(existing_phone), str(phone), config['iduserfeatures'])
		# nothing more to do: no exception has been raised so the caller
		# will continue the correct execution of its own code flow

def __save_phone(mode, ctx, phone, config):
	"Save new configuration of the local phone in the database."
	if "iduserfeatures" not in phone and config is not None \
	   and "iduserfeatures" in config:
		syslogf(SYSLOG_DEBUG, "__provisioning(): iduserfeatures='%s' from config to phone" % config["iduserfeatures"])
		phone["iduserfeatures"] = config["iduserfeatures"]
	if "iduserfeatures" in phone:
		syslogf("__provisioning(): SAVING phone %s informations to backend" % str(phone))
		ctx.dbinfos.save_phone(phone)

def __provisioning(mode, ctx, phone):
	"Provisioning high level logic for the described phone"
	# I only allow "sip" right now
	if phone["proto"] != TECH:
		syslogf(SYSLOG_ERR,			       \
			"__provisioning(): the only protocol supported " +\
			"right now is SIP, but I got %s" \
			% phone["proto"])
		raise BadRequest, "Unknown protocol '%s' != sip" % phone["proto"]

	syslogf(SYSLOG_NOTICE, "__provisioning(): handling phone %s" % str(phone))

	prev_iduserfeatures = None
	if mode == "notification":
		phonedesc = ctx.dbinfos.phone_by_macaddr(phone["macaddr"])
		if phonedesc is None:
			syslogf(SYSLOG_ERR, "__provisioning(): No phone has been found in the database for this mac address %s" % phone["macaddr"])
			raise NotFoundError, "No phone has been found in the database for this mac address %s" % phone["macaddr"]
		phone['isinalan'] = phonedesc['isinalan']
		phone['vendor'] = phonedesc['vendor']
		phone['model'] = phonedesc['model']
		prev_iduserfeatures = phonedesc['iduserfeatures']

	if field_empty(phone['vendor']) or field_empty(phone['model']) or field_empty(phone['isinalan']):
		syslogf(SYSLOG_ERR, "__provisioning(): Empty model or vendor or isinalan in phone %s" % str(phone))
		raise BadRequest, "Empty model or vendor or isinalan in phone %s" % str(phone)

	if "provcode" in phone and phone["provcode"] != "0" and \
	   not provsup.well_formed_provcode(phone["provcode"]):
		syslogf(SYSLOG_ERR, "__provisioning(): Invalid provcode %s" % phone["provcode"])
		raise NotFoundError, "Invalid provcode %s" % phone["provcode"]

	if phone["actions"] != "no":
		if "ipv4" not in phone:
			syslogf(SYSLOG_DEBUG, "__provisioning(): trying to get IPv4 address from Mac Address %s" % phone["macaddr"])
			phone["ipv4"] = provsup.ipv4_from_macaddr(phone["macaddr"], lambda x: syslogf(SYSLOG_ERR, x))
			if phone["ipv4"] is None:
				phone["actions"] = "no"
		if phone["actions"] != "no" and phone["ipv4"] is None:
			syslogf(SYSLOG_ERR, "__provisioning(): Actions enabled but got no IP address (for phone with Mac Address %s)" % phone["macaddr"])
			raise NotFoundError, "Actions enabled but got no IP address (for phone with Mac Address %s)" % phone["macaddr"]

	syslogf(SYSLOG_DEBUG, "__provisioning(): locking phone %s" % phone["macaddr"])
	if not ctx.maclocks.try_acquire(phone["macaddr"]):
	    syslogf(SYSLOG_WARNING, "__provisioning(): Operation already in progress for %s" % phone["macaddr"])
	    raise ConflictError, "Operation already in progress for phone %s" % phone["macaddr"]
	try:
	    syslogf(SYSLOG_DEBUG, "__provisioning(): phone class from vendor")
	    prov_class = provsup.PhoneClasses[phone["vendor"]] # TODO also use model
	    syslogf(SYSLOG_DEBUG, "__provisioning(): phone instance from class")
	    prov_inst = prov_class(phone)

	    if "provcode" in phone and phone["provcode"] == "0":
		    phone["iduserfeatures"] = "0"

	    if "iduserfeatures" in phone and phone["iduserfeatures"] == "0":
		    syslogf("__provisioning(): reinitializing provisioning to GUEST for phone %s" % str(phone))
		    prov_inst.generate_reinitprov()
		    __save_phone(mode, ctx, phone, None)
		    prov_inst.action_reinit()
	    else:
		if "iduserfeatures" in phone:
		    syslogf("__provisioning(): getting configuration from iduserfeatures for phone %s" % str(phone))
		    config = ctx.dbinfos.config_by_iduserfeatures_proto(phone["iduserfeatures"], phone["proto"])
		else:
		    syslogf("__provisioning(): getting configuration from provcode for phone %s" % str(phone))
		    config = ctx.dbinfos.config_by_provcode_proto(phone["provcode"], phone["proto"])
		if config is not None \
		   and 'iduserfeatures' in config \
		   and config['iduserfeatures']:
		    syslogf(SYSLOG_DEBUG, "__provisioning(): locking user %s" % config['iduserfeatures'])
		    if not ctx.userlocks.try_acquire(config['iduserfeatures']):
			syslogf(SYSLOG_WARNING, "__provisioning(): Operation already in progress for user %s" % config['iduserfeatures'])
			raise ConflictError, "Operation already in progress for user %s" % config['iduserfeatures']
		    try:
			syslogf(SYSLOG_NOTICE, "__provisioning(): AUTOPROV'isioning phone %s with config %s" % (str(phone),str(config)))
			__mode_dependant_provlogic_locked(mode, ctx, phone, config, prev_iduserfeatures)
			prov_inst.generate_autoprov(config)
			__save_phone(mode, ctx, phone, config)
			prov_inst.action_reboot()
		    finally:
			syslogf(SYSLOG_DEBUG, "__provisioning(): unlocking user %s" % config['iduserfeatures'])
			ctx.userlocks.release(config['iduserfeatures'])
		else:
		    syslogf(SYSLOG_ERR, "__provisioning(): not AUTOPROV'isioning phone %s cause no config found or no iduserfeatures in config" % str(phone))
		    raise NotFoundError, "no config found or no iduserfeatures in config for phone %s" % str(phone)
	finally:
	    syslogf(SYSLOG_DEBUG, "__provisioning(): unlocking phone %s" % phone["macaddr"])
	    ctx.maclocks.release(phone["macaddr"])

def __userdeleted(ctx, iduserfeatures):
	"Does what has to be done when a user is deleted."
	syslogf(SYSLOG_NOTICE, "__userdeleted(): handling deletion of user %s" % iduserfeatures)
	if not ctx.userlocks.try_acquire(iduserfeatures):
		syslogf(SYSLOG_WARNING, "__userdeleted(): Operation already in progress for user %s" % iduserfeatures)
		raise ConflictError, "Operation already in progress for user %s" % iduserfeatures
	try:
		phone = ctx.dbinfos.phone_by_iduserfeatures(iduserfeatures)
		if phone:
			syslogf("__userdeleted(): phone to destroy, because destruction of its owner - %s" % str(phone))
			phone['mode'] = 'authoritative'
			phone['actions'] = 'no'
			phone['iduserfeatures'] = '0'
			__provisioning('userdeleted', ctx, phone)
		# the following line will just destroy non 'sip' provisioning
		# with the same "iduserfeatures", and as they are none for now
		# it's not really useful but might become so in the future
		ctx.dbinfos.delete_phone_by_iduserfeatures(iduserfeatures)
	finally:
		ctx.userlocks.release(iduserfeatures)


# Safe locking API for provisioning and related stuffs

def lock_and_provision(mode, ctx, phone):
	"""Will attempt to provision with a global lock
	hold in shared mode

	"""
	syslogf(SYSLOG_DEBUG, "Entering lock_and_provision(phone=%s)" % str(phone))
	if not ctx.rwlock.acquire_read(pgc['excl_del_lock_to_s']):
		raise ConflictError, "Could not acquire the global lock in shared mode"
	try:
		__provisioning(mode, ctx, phone)
	finally:
		ctx.rwlock.release()
		syslogf(SYSLOG_DEBUG, "Leaving lock_and_provision for Mac Address %s" % phone["macaddr"])

def lock_and_userdel(ctx, iduserfeatures):
	"""Will attempt to delete informations related to the user identified by
	iduserfeatures from areas we are handling in the information backend,
	with a global lock hold for exclusive access.

	"""
	syslogf(SYSLOG_DEBUG, "Entering lock_and_userdel %s" % iduserfeatures)
	if not ctx.rwlock.acquire_write(pgc['excl_del_lock_to_s']):
		raise ConflictError, "Could not acquire the global lock in exclusive mode"
	try:
		__userdeleted(ctx, iduserfeatures)
	finally:
		ctx.rwlock.release()
		syslogf(SYSLOG_DEBUG, "Leaving lock_and_userdel %s" % iduserfeatures)

def clean_at_startup(ctx):
	"Put back every non guest orphan phones in GUEST state at startup."
	orphans = ctx.dbinfos.find_orphan_phones()
	for phone in orphans:
		syslogf(SYSLOG_NOTICE, "clean_at_startup(): about to remove orphan %s at startup" % str(phone))
		lock_and_userdel(ctx, phone['iduserfeatures'])

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
		self.my_ctx = self.my_server.my_ctx
		
		self.my_infos = self.my_ctx.dbinfos # compability with legacy code in this class
		
		self.posted = None
		BaseHTTPRequestHandler.__init__(self, request, client_address, server)

	# Override the setup method, taken from SocketServer.py and modified.

	def setup(self):
		self.connection = self.request
		self.connection.set_timeout(pgc['http_read_request_to_s'])
		self.rfile = self.connection.makefile('rb', self.rbufsize)
		self.wfile = self.connection.makefile('wb', self.wbufsize)

	# HTTP responses

	def send_response_headers_200(self, content_type = "text/plain"):
		self.send_response(200)
		self.send_header("Content-Type", content_type)
		self.send_header("Cache-Control", "no-cache")
		self.send_header("Pragma", "no-cache")
		self.end_headers()
	def send_response_lines(self, req_lines):
		self.send_response_headers_200()
		self.wfile.writelines([x+"\r\n" for x in req_lines])
	def answer_404(self, err_str = None):
		syslogf(SYSLOG_NOTICE, "answer_404(): sending not found message to %s" % str(self.client_address))
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
		"Used in 'authoritative' mode"
		return self.posted_infos("proto", "vendor", "model", "actions", "isinalan")
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
		syslogf(fmt % args)

	def full_xcept_sender(self, errcode):
		return lambda x:self.send_error_explain(
			errcode,
			''.join(("<pre>\n", cgi.escape(x), "</pre>\n"))
		)

	# Main handling functions

	def handle_prov(self):
	    "Does whatever action is asked by the peer"
	    phone = None
	    userinfo = None
	    syslogf(SYSLOG_NOTICE, "handle_prov(): handling /prov POST request for peer %s" % str(self.client_address))
	    try:
		self.get_posted()
		if "mode" not in self.posted:
			syslogf(SYSLOG_ERR, "handle_prov(): No mode posted")
			raise BadRequest, "No mode posted"

		if self.posted["mode"] == "authoritative":
			syslogf("handle_prov(): creating phone internal representation using full posted infos.")
			phone = self.posted_phone_infos()
			lock_and_provision(self.posted['mode'], self.my_ctx, phone)
		elif self.posted["mode"] == "notification":
			syslogf("handle_prov(): creating phone internal representation using light posted infos.")
			phone = self.posted_light_infos()
			lock_and_provision(self.posted['mode'], self.my_ctx, phone)
		elif self.posted["mode"] == "userdeleted":
			syslogf("handle_prov(): user deletion")
			userinfo = self.posted_userdeleted_infos()
			lock_and_userdel(self.my_ctx, userinfo['iduserfeatures'])
		else:
			syslogf(SYSLOG_ERR, "handle_prov(): Unknown mode %s" % self.posted["mode"])
			raise BadRequest, "Unknown mode %s" % self.posted["mode"]
	    except Exception, x:
		syslogf(SYSLOG_NOTICE, "handle_prov(): action FAILED - phone %s - userinfo %s" % (str(phone), str(userinfo)))
		errcode = 500
		for t,rcode in ExceptToHTTP.iteritems():
			if isinstance(x, t):
				errcode = rcode
				break
		except_tb.log_full_exception(self.full_xcept_sender(errcode),
					     provsup.SYSLOG_EXCEPT(SYSLOG_ERR))
		return
	    syslogf(SYSLOG_NOTICE, "handle_prov(): provisioning OK for phone %s" % str(phone))
	    self.send_response_lines(('Ok',))

	def handle_list(self):
		"Respond with the list of supported Vendors / Models"
		syslogf(SYSLOG_NOTICE, "handle_list(): handling /list GET request for peer %s" % str(self.client_address))
		self.send_response_headers_200()
		for phonekey,phoneclass in provsup.PhoneClasses.iteritems():
			phonelabel = phoneclass.label
			phones = phoneclass.get_phones()
			self.wfile.write(phonekey + '="' + phonelabel.replace('"','\\"') + '"\r\n')
			self.wfile.writelines([
				(phonekey + '.' + x[0] + '="' +
					x[1].replace('"','\\"') + '"\r\n')
				for x in phones])

	# === ENTRY POINTS (called FROM BaseHTTPRequestHandler) ===

	def do_POST(self):
		syslogf("do_POST(): handling POST request to path %s for peer %s" % (self.path, str(self.client_address)))
		if self.path == '/prov':
			self.handle_prov()
		else: self.answer_404()
	def do_GET(self):
		syslogf("do_GET(): handling GET request to path %s for peer %s" % (self.path, str(self.client_address)))
		if self.path == '/list':
			self.handle_list()
		else: self.answer_404()

def log_stderr_and_syslog(x):
	"""This function logs the string x to both stderr and the system log.
	The trailing '\\n' of the string to be logged must have been stripped
	
	"""
	print >> sys.stderr, x
	syslogf(SYSLOG_ERR, x)

def main(log_level, foreground):
	"""log_level - one of SYSLOG_EMERG to SYSLOG_DEBUG
		    nothing will be logged below this limit
		    (this is automatically upgraded to SYSLOG_INFO
		     during startup)
	foreground - don't daemonize if true
	
	"""
	syslog.openlog('autoprovisioning', syslog.LOG_PID, syslog.LOG_DAEMON)
	try:
		if log_level == SYSLOG_DEBUG:
			syslog.setlogmask(syslog.LOG_UPTO(SYSLOG_DEBUG))
		else:
			syslog.setlogmask(syslog.LOG_UPTO(SYSLOG_INFO))
		syslogf(SYSLOG_NOTICE, "Starting up")
		if not foreground:
			syslogf(SYSLOG_NOTICE, "Transforming into a daemon from hell")
			daemonize.daemonize(log_stderr_and_syslog, PIDFILE, True)
		else:
			syslogf(SYSLOG_NOTICE, "Not daemonizing, trying to take PID anyway")
			daemonize.create_pidfile_or_die(log_stderr_and_syslog, PIDFILE, True)
		syslogf(SYSLOG_NOTICE, "HTTP server creation")
		http_server = ThreadingHTTPServer((pgc['listen_ipv4'], pgc['listen_port']), ProvHttpHandler)
		http_server.my_ctx = CommonProvContext(
			ListLock(), # userlocks
			ListLock(), # maclocks
			SQLBackEnd(pgc['database_uri']),
			RWLock()
		)
		syslogf(SYSLOG_NOTICE, "Orphan phones cleanup")
		clean_at_startup(http_server.my_ctx)
		syslog.setlogmask(syslog.LOG_UPTO(log_level))
		syslogf(SYSLOG_NOTICE, "Will now serve incoming HTTP requests")
		http_server.serve_forever()
	except SystemExit:
		raise
	except:
		except_tb.log_exception(log_stderr_and_syslog)
	syslog.closelog()

dontlauchmain = False
foreground = False
log_level = SYSLOG_NOTICE

dburi_override = None
log_level_override = None
try:
	opts,args = getopt(sys.argv[1:], GETOPT_SHORTOPTS)
except GetoptError, x:
	print >> sys.stderr, x
	help_screen()
for k,v in opts: # DO NOT MERGE THE TWO LOOPS
	if k == '-h':
		help_screen()
for k,v in opts:
	if '-l' == k:
		log_level_override = v
	elif '-d' == k:
		dontlauchmain = True
	elif '-f' == k:
		foreground = True
	elif '-b' == k:
		dburi_override = v
	elif '-p' == k:
		PIDFILE = v
	elif '-c' == k:
		CONFIG_FILE = v

provsup.LoadConfig(CONFIG_FILE)

if log_level_override is not None:
	pgc['log_level'] = log_level_override
log_level = sysloglevel_from_str(pgc['log_level'])
if dburi_override is not None:
	pgc['database_uri'] = dburi_override

# We could daemonize and if we do we will chdir to '/',
# so get absolute pathname or anything else relative to
# database uri that could depends upon current envt.
pgc['database_uri'] = anysql.c14n_uri(pgc['database_uri'])

# provsup.LoadConfig must be called before
from Phones import * # package containing one module per vendor
if __name__ == '__main__' and not dontlauchmain:
	main(log_level, foreground)
