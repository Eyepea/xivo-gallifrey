"""Representational State Transfer - HTTP Connector

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

from operator import attrgetter
from itertools import imap, chain
from RestDispatcher import *
from ThreadingHTTPServer import *
from BaseHTTPServer import BaseHTTPRequestHandler
from threading import Thread
from http_pyparsing import HTTP_TRANSFER_CODING_LINE, HTTP_CHUNK_HEADER_LINE, \
                           HTTP_ACCEPT_HEADER_LINE, HTTP_CONTENT_TYPE_LINE, \
                           HTTP_TOKCHAR
from operator import attrgetter
from pyparsing import ParseException

class InvalidHeader(Exception): pass
class UnsupportedEncoding(Exception): pass
class InvalidQ(Exception): pass

__BYTES_VAL = ''.join(map(chr,xrange(0,256)))

def transfer_coding(s):
	ps = HTTP_TRANSFER_CODING_LINE.parseString(s)
	return ([k for k,v in ps], dict(((k,dict(iter(v))) for k,v in ps)))

def get_optional(getitemable, idx, default=None):
	try:
		return getitemable[idx]
	except LookupError:
		return default

def chunk_header(s):
	ps = HTTP_CHUNK_HEADER_LINE.parseString(s)
	return int(ps[0],16), dict(((ext[0], get_optional(ext, 1)) for ext in ps[1]))

def string_starNone(s):
	if s == '*': return None
	return s

def q_from_str(s):
	try:
		q = float(s)
	except ValueError:
		raise InvalidQ, "Invalid Q: %s" % repr(s)
	if (q < 0.0) or (q > 1.0):
		raise InvalidQ, "Invalid Q: %s" % repr(s)
	return q

def quote(s, delim = '"', escape = '\\'):
	return ''.join((delim, s.replace(escape, escape * 2).replace(delim, escape + delim), delim))

def param_to_str(param):
	if param[1].translate(__BYTES_VAL, HTTP_TOKCHAR):
		return param[0] + '=' + quote(param[1])
	else:
		return '='.join(iter(param))

def media_range_to_str(media_range):
	return ';'.join(chain(('/'.join(iter(media_range[0])),),imap(param_to_str, iter(media_range[1]))))

def present_one_media_range(media_range):
	type = string_starNone(media_range[0][0])
	subtype = string_starNone(media_range[0][1])
	if (type is None) and (subtype is not None):
		raise InvalidHeader, "In Accept header, invalid media-range  - %s" % media_range_to_str(media_range)
	q = 1.0
	dct = {}
	for k,v in media_range[1]:
		if k.lower() == 'q':
			q = q_from_str(v)
			break
		dct[k.lower()] = v
	fzset = frozenset(dct.iteritems())
	if fzset and (subtype is None):
		raise InvalidHeader, "In Accept header, invalid media-range  - %s" % media_range_to_str(media_range)
	return ConTypeDesc(type, subtype, fzset), q

def accept_line(s):
	ps = HTTP_ACCEPT_HEADER_LINE.parseString(s)
	return map(present_one_media_range, ps)

def present_media(media):
	type = media[0][0]
	subtype = media[0][1]
	dct = dict(((k.lower(),v) for k,v in media[1]))
	fzset = frozenset(dct.iteritems())
	return ConTypeDesc(type, subtype, fzset)

def content_type_line(s):
	ps = HTTP_CONTENT_TYPE_LINE.parseString(s)
	return present_media(ps)

class RestHTTPHandler(BaseHTTPRequestHandler):
	
	def __init__(self, request, client_address, server):
		self.rest_request = request
		self.rest_client_address = client_address
		self.rest_server = server
		self.rest_connector = server.rest_connector
		self.rest_dispatcher = self.rest_connector.dispatcher
		self.rest_path = None
		self.rest_payload = None
		self.rest_content_type = None
		self.rest_accept = None
		BaseHTTPRequestHandler.__init__(self, request, client_address, server)
	
	def has_payload(self):
		return ('Transfer-Encoding' in self.headers) or ('Content-Length' in self.headers)
	
	# XXX only prioritize
	#   str, str, non empty frozenset of (k,v)
	# above
	#   str, str, empty frozenset of (k,v)
	# if same type/subtype
	# probably implies merge of self.rest_accept[0] and self.rest_accept[1]
	def rest_parse_accept(self):
		"Parse the accept headers in format required by RestDispatcher."
		try:
			if 'Accept' in self.headers:
				pacc = accept_line(self.headers['Accept'])
			else:
				pacc = accept_line('*/*')
			self.rest_accept = [{},{},{},{}]
			for media,q in pacc:
				if media.extens:
					self.rest_accept[0][media] = q
				elif media.subtype:
					self.rest_accept[1][media] = q
				elif media.type:
					self.rest_accept[2][media] = q
				else:
					self.rest_accept[3][media] = q
			return True
		except (ParseException,InvalidHeader,InvalidQ), x:
			self.send_error(400, "Bad Accept header: %s" %x)
			return False
	
	def rest_is_chunked(self):
		if 'Transfer-Encoding' in self.headers:
			codlst,coddct = transfer_coding(self.headers['Transfer-Encoding'])
			if not codlst:
				raise InvalidHeader, "'Transfer-Encoding' HTTP header is empty"
			if len(codlst) == 1:
				zzz = codlst[0].lower()
				if 'identity' == zzz:
					return False
				if 'chunked' == zzz:
					return True
				raise UnsupportedEncoding, "Unsupported 'Transfer-Encoding' - %s" % repr(self.headers['Transfer-Encoding'])
		else:
			return False
	
	def rest_get_frags_bytenb(self, bytenb):
		payload_read = 0
		payl_in_progress = []
		while payload_read < bytenb:
			payl_in_progress.append(self.rfile.read(bytenb - payload_read))
			payload_read += len(payl_in_progress[-1])
		return payl_in_progress
	
	def rest_get_payload_chunked(self):
		full_in_prog = []
		while 1:
			chunk_head_line = self.rfile.readline()
			try:
				sz,ext = chunk_header(chunk_head_line)
			except ParseException:
				self.send_error(400, "Invalid chunk header")
				return False
			if sz == 0:
				break
			full_in_prog.extend(self.rest_get_frags_bytenb(sz))
			not_empty_line = self.rfile.readline().strip()
			if not_empty_line:
				self.send_error(400, "Message chunk not followed by line return")
				return False
		self.rest_payload = ''.join(full_in_prog)
		return True
	
	def rest_get_payload_identity(self):
		payload_length = int(self.headers['Content-Length'])
		# XXX: higher bound limit
		self.rest_payload = ''.join(self.rest_get_frags_bytenb(payload_length))
		return True

	def rest_get_content_type(self):
		if 'Content-Type' not in self.headers:
			self.send_error(415, "Please provide a Content-Type header")
			return False
		try:
			self.rest_content_type = content_type_line(self.headers['Content-Type'])
			return True
		except (ParseException,InvalidHeader), x:
			self.send_error(400, "Invalid 'Transfer-Encoding' header")
			return False
	
	def rest_get_payload(self):
		if not rest_get_content_type():
			return False
		try:
			if self.rest_is_chunked():
				return self.rest_get_payload_chunked()
			else:
				return self.rest_get_payload_identity()
		except (ParseException,InvalidHeader), x:
			self.send_error(400, "Invalid 'Transfer-Encoding' header")
			return False
		except UnsupportedEncoding, x:
			self.send_error(501, str(x))
			return False
	
	def rest_client_payload(self):
		"""Depending upon the method of the request being handled by 'self',
		either check that there is no entity in the request or read and
		decode the entity.
		"""
		if self.command not in REST_METHODS:
			self.send_error(501, "Unsupported method (%r)" % self.command)
			return False
		if self.has_payload():
			# always retrieve the payload if there should be some,
			# to avoid locking the TCP flow (which could possibly
			# result in a client/server deadlock)
			return self.rest_get_payload()
		if REST_METHODS[self.command][0]:
			self.send_error(400, "Bad request; headers do not "
			                     "indicate that a message body "
			                     "follow, but the required method "
			                     "requires one")
			# WARNING: when if front of a broken client this could
			# also deadlock after here (if the client sent no 
			# Content-Length or Transfer-Encoding header but still 
			# wants to send an entity we are not going to read), so 
			# XXX timeouts are really needed! TODO
			return False
		return True
	
	def rest_handle(self):
		"Rest specific code for this HTTP request handler."
		if not self.rest_client_payload():
			return
		if not self.rest_parse_accept():
			return
		try:
			self.rest_dispatcher.dispatch_in(
				self.rest_path, self.command,
				self.rest_payload, self.rest_content_type,
				self.rest_accept)
		except RestErrorCode, x:
			self.send_error(x.response_code, str(x))
	
	def handle_one_request(self):
        	"""Handle a single HTTP request.
		
		We override this method of BaseHTTPRequestHandler so that method
		demultiplexing is not done there but in the Rest Framework: call
		self.rest_handle() replaces what was method demultiplexing in
		BaseHTTPRequestHandler.
		"""
        	self.raw_requestline = self.rfile.readline()
        	if not self.raw_requestline:
			self.close_connection = 1
			return
        	if not self.parse_request(): # An error code has been sent, just exit
        		return
		self.rest_handle()

class RestHTTPRegistrar(object):
	
	def __init__(self, listen_ip, listen_port):
		self.__dispatcher = None
		self.__listen_ip = listen_ip
		self.__listen_port = listen_port
		self.__http_server = None
	
	def registering_dispatcher(self, dispatcher):
		if self.__dispatcher is not None:
			raise ValueError, "Already registred"
		self.__dispatcher = dispatcher
	
	# def unregistering_dispatcher(self, dispatcher):
	# 	pass
	
	# Could be interresting for outgoing connectors if parametrized
	# def get_connfact(self):
	# 	pass
	
	def start_listener(self):
		if self.__dispatcher is None:
			raise ValueError, "Must be registred before trying to start the listener"
		self.__http_server = ThreadingHTTPServer(
			(self.__listen_ip, self.__listen_port), RestHTTPHandler)
		self.__http_server.rest_connector = self
		self.__http_server_thread = Thread(
			None, self.__http_server.serve_forever, None, (), {})
		# self.__http_server.serve_forever()
	
	dispatcher = property(attrgetter('_RestHTTPRegistrar__dispatcher'))

all = ['RestHTTPRegistrar', 'RestHTTPHandler']
