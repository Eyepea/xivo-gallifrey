import UpCollections
from RestHTTPConnector import *
from RestDispatcher import *
from RestXmlAdaptor import *
from ResourceTree import *
from interfaces import *
from AttrDict import *
from CtrlMap import *
from OrdDict import *
from easyslog import *

allow_xml = ConTypeDesc('text', 'xml', frozenset())

class SelfXmlVisit(object):
	def fvisit(self, ctx, path, pos):
		return ctx.replace(
			ctd_classes_factory = lambda:((allow_xml,),),
			application_factory = lambda:self
		)

class XmlVisit(object):
	def fvisit(self, ctx, path, pos):
		return ctx.replace(ctd_classes_factory = lambda:((allow_xml,),))

class BaseIfaceAttrCls(XmlVisit, RT_node):
	def fvisit(self, ctx, path, pos):
		pass

class OneStaticOption(RT_node):
	def fvisit(self, ctx, path, pos):
		pass

class StaticOptionsCls(XmlVisit, RT_Set):
	def __init__(self):
		self._static_option = OneStaticOption()
		super(StaticOptionsCls, self).__init__(
			True,
			ordDict((('address',self._static_option),
			         ('netmask',self._static_option),
				 ('broadcast',self._static_option),
				 ('gateway',self._static_option)))
		)
	def get_tree_repr(self, iface):
		r = empty_container()
		for opt in ('address', 'netmask', 'broadcast', 'gateway'):
			r[opt] = iface.simple_get_option(opt)
		return r

def allow_by_iface_name(ni, iface_name):
	syslogf(str(ni.get_all_allow_lists()))
	syslogf(str(iface_name))
	return [allow for allow,iface_list in ni.get_all_allow_lists()
	        if iface_name in iface_list]

class BoundedIfaceCls:
	def __init__(self, desc, ni, iface):
		self.ni = ni
		self.desc = desc
		self.iface = iface
	def get_tree_repr(self):
		r = empty_container()
		for attr in ('family', 'method'):
			r[attr] = self.iface.get_iface_attr(attr)
		r['allow'] = ','.join(allow_by_iface_name(self.ni, self.iface.get_iface_name()))
		r['static'] = self.desc._static_options.get_tree_repr(self.iface)
		return r
	def req_in(self, ctx, req_payload):
		return 200, (self.get_tree_repr(), self.iface.get_iface_name())

class OneInterfaceCls(RT_Set):
	def __init__(self):
		self._base_iface_attr = BaseIfaceAttrCls()
		self._static_options = StaticOptionsCls()
		super(OneInterfaceCls, self).__init__(
			True,
			ordDict((('family',self._base_iface_attr),
			         ('method',self._base_iface_attr),
				 ('allow',self._base_iface_attr),
				 ('static',self._static_options)))
		)
	def fin(self, ctx, path, pos):
		def app_fact():
			ni = ctx.application_factory().parse_ni()
			iface = ni.get_iface(path[pos-1])
			return BoundedIfaceCls(self, ni, iface)
		return ctx.replace(application_factory = app_fact), True
	def fvisit(self, ctx, path, pos):
		return self.fin(ctx, path, pos)[0]

class InterfacesCls(SelfXmlVisit, RT_Dyn):
	def __init__(self, eni_filename = '/etc/network/interfaces'):
		self._eni_filename = eni_filename
		super(InterfacesCls, self).__init__(
			True,
			[lambda x: x.find('eth') == 0],
			OneInterfaceCls())
	def fin(self, ctx, path, pos):
		return ctx.replace(
			ctd_classes_factory = lambda:((allow_xml,),),
			application_factory = lambda:self), True
	def parse_ni(self):
		ni = NetworkInterfaces(file(self._eni_filename))
		ni.parse()
		return ni
	def req_in(self, ctx, req_payload):
		r = empty_container()
		ni = self.parse_ni()
		for iface in ni.iteriface():
			r[iface.get_iface_name()] = \
				BoundedIfaceCls(self._subtree, ni, iface).get_tree_repr()
		return 200, (r, 'interfaces')

# selected_adaptor, payload_int

Interfaces = InterfacesCls('/home/xilun/xivo/trunk/lib-python/Tests/interfaces')
rest = RestDispatcher()
rest.register_presentation(RestXmlRegistrar())
rest.mount_app(Interfaces, ('network','interfaces'))

http = RestHTTPRegistrar('localhost', 8080)
rest.register_connector(http)
http.start_listener()
print "kikoo"
