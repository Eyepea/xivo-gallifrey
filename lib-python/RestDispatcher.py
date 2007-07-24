"""Representational State Transfer Dispatcher

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

import sys
from pyfunc import *
from ReplTuple import *
from ResourceTree import *
from collections import NamedTuple
from BaseHTTPServer import BaseHTTPRequestHandler
from AttrDict import *
from CtrlMap import *
from OrdDict import *

RESPONSES = BaseHTTPRequestHandler.responses
# keys:		rest/http method
# values:	entity payload in client request
REST_METHODS = {
	'GET':		(False,),
	'HEAD':		(False,),	#XXX keep it or not ? (if dropped, special
					# handling will be done in the HTTP 
					# connector to provide HEAD there
	'PUT':		(True,),
	'POST':		(True,),
	'DELETE':	(False,),
}

def empty_container():
	return AttrDict(CtrlMap(ordDict()))

# ConTypeDesc:
# Content-Type descriptor when communicating with an adaptor.
# type,subtype,extens can be of one of the following types:
#   - str, str, non empty frozenset of (k,v) with no multiple copies of k
#   - str, str, empty frozenset
#   - str, NoneType, empty frozenset
#   - NoneType, NoneType, empty frozenset
# Note that extens is voluntarily not a dictionary so that ConTypeDesc can be
# used as the key of a dictionary, which will help to find the 'q' value.
# Also used internally by RestDispatcher 
ConTypeDesc = NamedTuple('ConTypeDesc', 'type subtype extens')

# ConTypeMatch:
# If this is a static matcher, then fdynam is not used (should be set to None)
# and params is a non empty frozenset of (k,v) with no multiple copies of k
# where k are parameters name and v are parameters content.
# If this is not a static matcher, then params must be searchable with 'in'
# (could be a set / frozenset / dict) and fdynam must be a function which take
# parameters with the same name 
ConTypeMatch = NamedTuple('ConTypeMatch', 'strict static type subtype params fdynam')

# Using CtxType is not really mandatory as most of our Rest modules will
# just access the context with something like ctx.method or
# ctx.ctd_classes_factory(), etc..
CtxType = ReplTuple('CtxType', 'ctd_classes_factory application_factory method path')

# Returned by RestDispatcher.select_adaptor():
SelectedAdaptor = NamedTuple('SelectedAdaptor', 'adaptor_fact contype_match contype_desc q')

def ConTypePossible(contype_match, contype_desc):
	if ((contype_desc.type is not None)
	    and (contype_desc.type != contype_match.type)) \
	   or ((contype_desc.subtype is not None)
	       and (contype_desc.subtype != contype_match.subtype)):
		return False
	if len(contype_desc.extens) < len(contype_match.params) \
	   or (contype_match.strict and len(contype_desc.extens) != len(contype_match.params)):
		return False
	exten_dct = dict(contype_desc.extens)
	param_dct = dict(contype_match.params)
	if contype_match.static:
		return all(((k in exten_dct)
			    and (exten_dct[k] == param_dct[k]))
			   for k in param_dct)
	else:
		pdct = {}
		for k in param_dct:
			if k not in exten_dct:
				return False
			pdct[k] = exten_dct[k]
		return contype_match.fdynam(**pdct)

def CountSupplCtmCtd(ctm, ctd):
	return len(ctd.extens) - len(ctm.params)



class RestError(Exception):
	def __init__(self, msg=''):
		self.message = msg
		Exception.__init__(self, msg)
	def __repr__(self):
		return self.message
	__str__ = __repr__

class RestErrorCode(RestError):
	def __init__(self, code, strcat = None):
		if not strcat: strcat = ''
		else: strcat = ' - ' + strcat
		RestError.__init__(self, RESPONSES[code][0] + strcat)
		self.response_code = code

# XXX do locking (at least minimal support)

class RestDispatcher(object):
	def __init__(self):
		self.__res_root = RT_Mount()
		self.__connector = None # xxx support multiple connectors
		self.__presentation = {}
		self.__reverse_adapt = {}
	def mount_app(self, rt, path):
		self.__res_root.mount(rt, path)
	def umount_app(self, rt, path):
		self.__res_root.umount(rt, path)
	def ctx_path(self, path, ctx):
		return self.__res_root.ctx_path(path, ctx)
	def register_connector(self, connector):
		if self.__connector is not None:
			raise ValueError, "Multiple connectors are not yet supported"
		connector.registering_dispatcher(self)
		self.__connector = connector
	def unregister_connector(self, connector):
		if self.__connector is not connector:
			raise ValueError, "Trying to unregister a connector that has not been registred"
		connector.unregistering_dispatcher(self)
		self.__connector = None
	def register_presentation(self, adaptor):
		if adaptor in self.__reverse_adapt:
			raise ValueError, "Adaptor %s already registred" % repr(adaptor)
		presentation_lst = adaptor.get_supported_reg(self)
		self.__reverse_adapt[adaptor] = presentation_lst
		for ctm in presentation_lst:
			self.__presentation[ctm] = adaptor.get_transfact()
	def unregister_presentation(self, adaptor):
		if adaptor not in self.__reverse_adapt:
			raise ValueError, "Adaptor %s not registred" % repr(adaptor)
		presentation_lst = adaptor.get_supported_unreg(self)
		del self.__reverse_adapt[adaptor]
		for ctm in presentation_lst:
			del self.__presentation[ctm]
	# XXX: complete special handling of Q==0
	# XXX only prioritize
	#   str, str, non empty frozenset of (k,v)
	# above
	#   str, str, empty frozenset of (k,v)
	# if same type/subtype
	def select_adaptor(self, ctx, seq_dico_ctd_q):
		"""Given a request context 'ctx' and a prioritized list
		(highest first) of dictionaries associating a Q value (as in
		RFC 2616) to ConTypeDesc instances, coming from the client
		request, 'seq_dico_ctd_q';
		- this function selects a tuple containing the best matching
		Adaptor, the corresponding ConTypeMatch instance and the
		ConTypeDesc instance selected in the match, and the
		corresponding Q value (as per 2616 definition).
		Criterions that define the "best" match are too complicated,
		just read the code.
		- if no match has been found, None is returned instead of the
		tuple. """
		# ctd_classes: list of list of ConTypeDesc allowed by the
		# application, first lists being of highest priorities, and every
		# ConTypeDesc within a given list being of the same priority
		ctd_classes = ctx.ctd_classes_factory()
		for ctd_class in ctd_classes:
			# keys in behav: strict static
			behav = { (False, False): set(), (False, True): set(),
			          (True, False):  set(), (True, True):  set(), }
			for ctd in ctd_class:
				for ctm in self.__presentation:
					if ConTypePossible(ctm, ctd):
						behav[ctm[0:2]].add(ctm)
			# seq_dico_ctd_q contains a prioritized list (highest
			# first) of dictionaries associating a Q value (as in
			# RFC 2616) to Content Type Descriptors, coming from
			# the client request.
			for dico_ctd_q in seq_dico_ctd_q:
				best_ctd = None
				best_ctm = None
				best_adaptor_fact = None
				best_Q = -1
				for ctd,cur_q in dico_ctd_q.iteritems():
					if not cur_q:
						continue
					# Scans client content type desc in the same client prio class
					this_ctd_done = False
					for behav_k in ((True, True), (True, False), (False, True), (False, False)):
						if this_ctd_done:
							break
						# For a given CTD, associated presentation layer handler is
						# selected from first match in (strict, static) matchers first
						# to (loose, dynamic) last.
						best_adaptor_for_ctd = None
						best_ctm_for_ctd = None
						best_min_suppl = sys.maxint
						q_for_best = 42
						for ctm in behav[behav_k]:
							if ConTypePossible(ctm, ctd):
								if behav_k[0]:	# strict match
									suppl = 0
								else:		# loose match
									suppl = CountSupplCtmCtd(ctm, ctd)
								if suppl < best_min_suppl:
									best_adaptor_for_ctd = self.__presentation[ctm]
									best_min_suppl = suppl
									best_ctm_for_ctd = ctm
									q_for_best = cur_q
							if best_min_suppl == 0:
								break
						if best_adaptor_for_ctd is not None:
							if q_for_best > best_Q:
								best_ctd = ctd
								best_ctm = best_ctm_for_ctd
								best_adaptor_fact = best_adaptor_for_ctd
								best_Q = q_for_best
							this_ctd_done = True
				if (best_adaptor_fact is not None) and (best_Q != 0):
					return SelectedAdaptor(best_adaptor_fact, best_ctm, best_ctd, best_Q)
	def adaptor_from_ctd(self, ctd):
		# XXX: merge with select_adaptor() ?
		behav = { (False, False): set(), (False, True): set(),
			  (True, False):  set(), (True, True):  set(), }
		for ctm in self.__presentation:
			if ConTypePossible(ctm, ctd):
				behav[ctm[0:2]].add(ctm)
		done = False
		for behav_k in ((True, True), (True, False), (False, True), (False, False)):
			if done:
				break
			best_adaptor_for_ctd = None
			best_ctm_for_ctd = None
			best_min_suppl = sys.maxint
			for ctm in behav[behav_k]:
				if behav_k[0]:	# strict match
					suppl = 0
				else:		# loose match
					suppl = CountSupplCtmCtd(ctm, ctd)
				if suppl < best_min_suppl:
					best_adaptor_for_ctd = self.__presentation[ctm]
					best_ctm_for_ctd = ctm
					best_min_suppl = suppl
			if best_adaptor_for_ctd is not None:
				return SelectedAdaptor(
					best_adaptor_for_ctd,
					best_ctm_for_ctd,
					ctd, 1.0)
	def dispatch_in(self, path, method, payload, ctd_in, accept_ctd_q, connector):
		# XXX: catch exceptions on adaptor an translate to http response code
		if method not in REST_METHODS:
			raise RestErrorCode(501)
		# === APPLICATION LOOKUP ===
		ctx = self.ctx_path(path, CtxType(None, None, method, path))
		if ctx is None:
			raise RestErrorCode(404)
		# === REQUEST PAYLOAD ADAPTATION ===
		if payload is not None:
			sai = self.adaptor_from_ctd(ctd_in)
			if sai is None:
				raise RestErrorCode(415)
			adapt_in = sai.adaptor_fact(sai)
			req_payload,req_ctd = adapt_in.to_internal(sai, payload)
		else:
			req_payload,req_ctd = None,None
		del payload
		# === SELECTION OF OUTPUT ADAPTOR ===
		# TODO: early detection if request requires an output entity
		# and don't even launch the application in this case if no
		# accept_ctd_q
		if accept_ctd_q is not None:
			sa = self.select_adaptor(ctx, accept_ctd_q)
			if sa is None:
				raise RestErrorCode(406)
			adaptor = sa.adaptor_fact(sa)
			if adaptor is None:
				raise RestErrorCode(500, 'adaptor')
		else:
			adaptor,sa = None,None
		del accept_ctd_q
		# === APPLICATION DEREFERENCIATION ===
		app = ctx.application_factory()
		if app is None:
			raise RestErrorCode(500, 'application')
		# === CALL TO APPLICATION ===
		rcode,tree_rootname = app.req_in(ctx, req_payload)
		# === OUTPUT ADAPTATION ===
		if tree_rootname:
			if adaptor is not None:
				out_payload,out_ctd = adaptor.to_external(sa, tree_rootname)
			else:
				raise RestErrorCode(406)
		else:
			out_payload,out_ctd = None,None
		del tree_rootname
		# === SEND ANSWER ===
		connector.rest_answer(rcode, out_payload, out_ctd)

__all__ = ['ConTypeDesc', 'ConTypeMatch', 'CtxType', 'ConTypePossible',
           'CountSupplCtmCtd', 'RestDispatcher', 'REST_METHODS',
	   'RestError', 'RestErrorCode', 'empty_container']
