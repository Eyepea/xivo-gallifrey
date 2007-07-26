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

import UpCollections
from RestHTTPConnector import *
from RestDispatcher import *
from RestXmlAdaptor import *
from ResourceTree import *
from interfaces import *
from easyslog import *
from AttrDict import *
from CtrlMap import *
from OrdDict import *
from pyfunc import *

# CtxType = ReplTuple('CtxType', 'ctd_classes_factory application_factory method path')

from types import MethodType

allow_xml = ConTypeDesc('text', 'xml', frozenset())

class XML_Visit(object):
	@classmethod
	def fvisit(cls, ctx, path, pos):
		syslogf(str(cls))
		syslogf(str(cls.sub))
		return ctx.replace(
			ctd_classes_factory = lambda:((allow_xml,),),
			application_factory = cls
		)

class XML_SubVia(object):
	@classmethod
	def fin(cls, ctx, path, pos, eaten):
		syslogf(str(cls))
		syslogf(str(cls.sub))
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
	SUBNAME_CLASS = ordDict((('address',StaticOption),
			         ('netmask',StaticOption),
				 ('broadcast',StaticOption),
				 ('gateway',StaticOption)))
	def __init__(self, ni, iface, me):
		self.ni = ni
		self.iface = iface
		self.me = me
	def __iter__(self):
		return (StaticOption(self.ni, self.iface, attr)
		        for attr in self.SUBNAME_CLASS)
	def sub(self, p):
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
	SUBNAME_CLASS = ordDict((('family',BaseIfaceAttr),
			         ('method',BaseIfaceAttr),
				 ('allow',BaseIfaceAttr),
				 ('static',StaticOptions)))
	def __init__(self, ni, iface):
		self.ni = ni
		self.iface = iface
	def __iter__(self):
		return (subcls(self.ni, self.iface, attr) 
		        for (attr,subcls) in self.SUBNAME_CLASS.iteritems())
	def sub(self, p):
		return self.SUBNAME_CLASS[p[0]](self.ni, self.iface, p[0])
	def get_name(self):
		return self.iface.get_iface_name()

class Interfaces(XML_Visit,XML_SubVia,GetIterContainer,ReqIn,VisitableNode,DynTree):
	
	ENI_FILENAME = '/etc/network/interfaces'
	
	SUBCLASS = Interface
	@staticmethod
	def subpath_ok(iface_name):
		return 0 == iface_name.find('eth')

	def __init__(self):
		self.ni = NetworkInterfaces(file(self.ENI_FILENAME))
		self.ni.parse()
	def __iter__(self):
		return (Interface(self.ni, iface) for iface in self.ni.iteriface())
	def sub(self, p):
		return Interface(self.ni, self.ni.get_iface(p[0]))
	def get_name(self):
		return 'interfaces'

#
# XXX balancer les effectifs et non pas les theoriques par __iter__
#
#
# path_sub par defaut ?
#
# XXX: merger __iter__ et sub mais garder sub (requis par subvia et deepvia) ???
# XXX faire aussi un req_in generique je pense
# XXX: catch excepts
#
# probablement possible de generifier le sub() et de passer en full declaratif
# (ou presque)
#
# TODO: propre nom stocke d'une maniere std (pour avoir un req_in generique)
#
# TODO: instantiation des fils de maniere declarative ?
#
# TODO: surement possible de supprimer sub() pour les SetTree
#

# Tests only 
Interfaces.ENI_FILENAME = '/home/xilun/xivo/trunk/lib-python/Tests/interfaces'

http = RestHTTPRegistrar('localhost', 8080)

rest = RestDispatcher()
rest.register_presentation(RestXmlRegistrar())
rest.mount_app(Interfaces, ('network','interfaces'))
rest.register_connector(http)

http.start_listener()

print "kikoo"
