__version__ = "$Revision$ $Date$"

import UpCollections
from ResourceTree import *
from OrdDict import *
from interfaces import *
from ReplTuple import *
import os
import os.path
from stat import *

ResCtx = ReplTuple('ResCtx', 'factory foobar')

ATTRIBUTES = ordDict.fromkeys('name family method allow address netmask broadcast gateway'.split())

############### delayed instanciation exemple ###############

class IfAttr(RT_node):
	@staticmethod
	def fvisit(ctx, path, pos):
		if (ctx is not None) and (path[pos-1] in ATTRIBUTES):
			def new_factory():
				obj = ctx.factory()
				try:
					return obj.attr(path[pos-1])
				except (ValueError, AttributeError):
					return None
			return ctx.replace(factory = new_factory)
		else:
			return None

class Iface:
	def __init__(self, iface_name):
		print 'mdr'
		self.truc = NetworkInterfaces(file('/home/xilun/xivo/trunk/lib-python/interfaces'))
		self.truc.parse()
		self.machin = self.truc.get_iface(iface_name)
		self.iface_name = iface_name
	def attr(self, attrname):
		if attrname not in ATTRIBUTES:
			raise ValueError, 'Bad attribute name ' + repr(attrname)
		self.attrname = attrname
		return self
	def get(self):
		if self.attrname == 'name' or self.attrname == 'family' or self.attrname == 'method':
			return self.machin.get_iface_attr(self.attrname)
		if self.attrname == 'allow':
			return list(set([n for (n,iface) in self.truc.get_all_allow_lists() if iface == self.iface_name]))
		if self.attrname in 'address netmask broadcast gateway'.split():
			return self.machin.simple_get_option(self.attrname)
		raise ValueError, 'Bad attribute name ' + repr(self.attrname)

class ResIface(RT_Set):
	@staticmethod
	def fin(ctx, path, pos):
		print 'kikoo'
		def mk_iface():
			try:
				return Iface(path[pos-1])
			except KeyError:
				return None
		print 'lol'
		return (ResCtx(factory=mk_iface,foobar=42), True)

class ResSysIfCfg(RT_node):
	# WARNING: security hole : we don't test for ..!
	@staticmethod
	def visitable(ctx, path, pos):
		filename = os.path.join('/proc/sys/net/ipv4', *path[pos:])
		return S_ISREG(os.stat(filename)[ST_MODE])
	@staticmethod
	def fvisit(ctx, path, pos):
		def facto():
			class F:
				def __init__(self, filename):
					self.filename = filename
				def get(self):
					return file(self.filename).read()
			return F(os.path.join('/proc/sys/net/ipv4', *path[pos:]))
		return ResCtx(factory = facto, foobar = 42)

IfAttrInst = IfAttr()
ResIfaceInst = ResIface(False, ordDict.fromkeys(ATTRIBUTES, IfAttrInst))
ResIfConfigInst = RT_Dyn(False, (lambda x: x.find('eth') == 0,), ResIfaceInst)
ResSysIfCfgInst = ResSysIfCfg()
Root = RT_Set(False, ordDict((('ifconfig', ResIfConfigInst), ('sysifcfg', ResSysIfCfgInst))))
