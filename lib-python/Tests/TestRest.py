#!/usr/bin/python

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, Proformatique

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
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
"""

import UpCollections	# ok for tests programs only ; uses mandatory loading 
			# mechanism for use in real Xivo programs
			# MandatoryModulesLoad() in ConfigPath

from RestDispatcher import *
from ResourceTree import *
from interfaces import *
from OrdDict import *
from pyfunc import *

# NOTE: CtxType = ReplTuple('CtxType', 'ctd_classes_factory application_factory method path')

allow_xml = ConTypeDesc('text', 'xml', frozenset())

class XML_Visit(object):
	@classmethod
	def fvisit(cls, ctx, path, pos):
		return ctx.replace(
			ctd_classes_factory = lambda:((allow_xml,),),
			application_factory = cls
		)

class XML_SubVia(object):
	@classmethod
	def fin(cls, ctx, path, pos, eaten):
		return ctx.replace(
		    ctd_classes_factory = lambda:((allow_xml,),),
		    application_factory = 
		        lambda:cls().sub(path[pos:pos+eaten])
		    ), True

class XML_DeepVia(object):
	@classmethod
	def fin(cls, ctx, path, pos, eaten):
		return ctx.replace(
		    ctd_classes_factory = lambda:((allow_xml,),),
		    application_factory = 
		        lambda:ctx.application_factory().sub(path[pos:pos+eaten])
		    ), True

class GetIterContainer(object):
	def get_tree(self):
		r = empty_container()
		for inst in self:
			r[inst.get_name()] = inst.get_tree()
		return r

class ReqIn(object):
	def req_in(self, ctx, req_payload):
		return 200, (self.get_tree(), self.get_name())


always_there = lambda *param:True

def allow_by_iface_name(ni, iface_name):
	return [allow for allow,iface_list in ni.get_all_allow_lists()
	        if iface_name in iface_list]

class StaticOption(ReqIn,VisitableNode):
	def __init__(self, ni, iface, attr):
		self.ni = ni
		self.iface = iface
		self.attr = attr
	def get_tree(self):
		return self.iface.simple_get_option(self.attr)
	def get_name(self):
		return self.attr

class StaticOptions(XML_DeepVia,GetIterContainer,ReqIn,VisitableNode,SetTree):
	def has_static_option(self, opt):
		return self.iface.simple_has_option(opt)
	STATIC_OPT_DESC = (StaticOption,has_static_option)
	SUBNAME_OBJ = ordDict((('address',   STATIC_OPT_DESC),
	                       ('netmask',   STATIC_OPT_DESC),
	                       ('broadcast', STATIC_OPT_DESC),
	                       ('gateway',   STATIC_OPT_DESC)))
	def __init__(self, ni, iface, me):
		self.ni = ni
		self.iface = iface
		self.me = me
	def __iter__(self):
		return (StaticOption(self.ni, self.iface, attr)
		        for (attr,subdsc) in self.SUBNAME_OBJ.iteritems()
			if subdsc[1](self, attr))
	def sub(self, p):
		if not self.SUBNAME_OBJ[p[0]][1](self, p[0]):
			raise RestErrorCode(404)
		return StaticOption(self.ni, self.iface, p[0])
	def get_name(self):
		return 'static'

class BaseIfaceAttr(ReqIn,VisitableNode):
	def __init__(self, ni, iface, attr):
		self.ni = ni
		self.iface = iface
		self.attr = attr
	def get_tree(self):
		if self.attr in ('family', 'method'):
			return self.iface.get_iface_attr(self.attr)
		elif self.attr == 'allow':
			return ','.join(allow_by_iface_name(self.ni, self.iface.get_iface_name()))
	def get_name(self):
		return self.attr

class Interface(XML_DeepVia,GetIterContainer,ReqIn,VisitableNode,SetTree):
	def has_static_block(self):
		return self.iface.get_iface_attr('method') == 'static'
	SUBNAME_OBJ = ordDict((('family', (BaseIfaceAttr,always_there)),
	                       ('method', (BaseIfaceAttr,always_there)),
	                       ('allow',  (BaseIfaceAttr,always_there)),
	                       ('static', (StaticOptions,has_static_block))))
	def __init__(self, ni, iface):
		self.ni = ni
		self.iface = iface
	def __iter__(self):
		return (subdsc[0](self.ni, self.iface, attr) 
		        for (attr,subdsc) in self.SUBNAME_OBJ.iteritems()
			if subdsc[1](self))
	def sub(self, p):
		child_tup = self.SUBNAME_OBJ[p[0]]
		if not child_tup[1](self):
			raise RestErrorCode(404)
		return child_tup[0](self.ni, self.iface, p[0])
	def get_name(self):
		return self.iface.get_iface_name()

class Interfaces(XML_Visit,XML_SubVia,GetIterContainer,ReqIn,VisitableNode,DynTree):
	
	ENI_FILENAME = '/etc/network/interfaces'
	
	SUBOBJ = Interface
	@staticmethod
	def subpath_ok(iface_name):
		return 0 == iface_name.find('eth')
	
	def __init__(self):
		self.ni = NetworkInterfaces(file(self.ENI_FILENAME))
		self.ni.parse()
	def __iter__(self):
		return (Interface(self.ni, iface)
		        for iface in self.ni.iteriface()
			if self.subpath_ok(iface.get_iface_name()))
	def sub(self, p):
		try:
			return Interface(self.ni, self.ni.get_iface(p[0]))
		except KeyError:
			raise RestErrorCode(404)
	def get_name(self):
		return 'interfaces'

#
# TODO: non prioritaire -- surement possible de supprimer sub() pour
# les SetTree/DynTree => standardiser les methodes de filtre comme
# subpath_ok ou lelement 1 des valeurs du tuple SUBNAME_OBJ dans ce
# module. -- argument == un tuple des composants self -> fils du path
#            retourne True ou False
#

# Tests only 

from RestHTTPConnector import *
from RestXmlAdaptor import *

Interfaces.ENI_FILENAME = '/home/xilun/xivo/trunk/lib-python/Tests/interfaces'

http = RestHTTPRegistrar('localhost', 8080)

rest = RestDispatcher()
rest.register_presentation(RestXmlRegistrar())
rest.mount_app(Interfaces, ('network','interfaces'))
rest.register_connector(http)

http.start_listener()

print "kikoo"
