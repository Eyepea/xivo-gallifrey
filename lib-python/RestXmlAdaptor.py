"""Representational State Transfer Xml Adaptor

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

from RestDispatcher import *
from PinstXml import *

class RestXmlRegistrar(object):
	
	XML_CTM = ConTypeMatch(
		strict = False,
		static = True,
		type = 'text',
		subtype = 'xml',
		params = frozenset(),
		fdynam = None
	)
	
	XML_CTD = ConTypeDesc(
		type = 'text',
		subtype = 'xml',
		extens = frozenset()
	)
	
	def __init__(self):
		self.__dispatcher = None
	
	# XXX
	# def get_supported_unreg(self, dispatcher):
	# 	pass
	
	def get_supported_reg(self, dispatcher):
		"Rest registration function"
		if self.__dispatcher is not None:
			raise ValueError, "Already registred"
		self.__dispatcher = dispatcher
		return [self.XML_CTM]
	
	def get_transfact(self):
		"""Returns a callable that takes a SelectedAdaptor argument
		and returns an object with an attribute to_internal that is
		a callable and to_external an other one (for their respective
		prototypes, see the corresponding doc. in this module) """
		return lambda sa: self
	
	# XXX: would be better to allow progressive loading/parsing of the 
	# payload, which would be quite simple using a file like interface
	# on the socket :)
	
	def get_content_type_desc(self):
		return self.XML_CTD
	
	def to_internal(self, selected_adaptor, payload):
		"""Translate an Xml Payload (of a supported informal 'schema')
		into an internal representation made of a tree of Python
		objects.
		selected_adaptor is a:
		NamedTuple('SelectedAdaptor',
		           'adaptor_fact contype_match contype_desc q')
		"""
		if not payload:
			return None
		return pinst_xmlstr(payload)

	def to_external(self, selected_adaptor, payload_int):
		"""Translate an internal representation into an Xml Payload.
		selected_adaptor is a:
		NamedTuple('SelectedAdaptor',
		           'adaptor_fact contype_match contype_desc q')
		"""
		if not payload_int:
			return None
		return xmlstr_pinst(*payload_int)

__all__ = ['RestXmlRegistrar']
