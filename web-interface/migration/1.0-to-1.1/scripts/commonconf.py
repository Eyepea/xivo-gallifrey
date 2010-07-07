#!/usr/bin/python
from __future__ import with_statement
__version__ = "$Revision$ $Date$"
__author__  = "Guillaume Bour <gbour@proformatique.com>"
__license__ = """
    Copyright (C) 2010  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""
"""Migrate from 1.0 to 1.1
	Import common.conf configuration into webi database
"""


import sys, os.path, cjson, httplib, urllib
from xivo import OrderedConf, ConfigDict, xivo_config
from xivo.xivo_helpers import db_connect

COMMON_CONFFILE = '/etc/pf-xivo/common.conf'
XIVO_CONFFILE   = '/etc/pf-xivo/web-interface/xivo.ini'


class CommonConf(object):
	xforms = {
		# key name           : (lambda value transform, table name, field name, condition)
		'': (None, '', '', ''),
		'': (None, '', '', ''),
		'xivo_domain'          : (None, 'resolvconf', 'domain'  , 'id = 1'),
		'xivo_hostname'        : (None, 'resolvconf', 'hostname', 'id = 1'),
		'xivo_extra_dns_search': (None, 'resolvconf', 'search'  , 'id = 1'),

		'xivo_smtp_origin'   : (None, 'mail', 'origin'   , 'id = 1'),
		'xivo_smtp_mydomain' : (None, 'mail', 'mydomain' , 'id = 1'),
		'xivo_smtp_relayhost': (None, 'mail', 'relayhost', 'id = 1'),
		'xivo_smtp_fallback_relayhost': (None, 'mail', 'fallback_relayhost', 'id = 1'),
		'xivo_smtp_canonical': (lambda x: x.replace(" ", "\r\n"), 'mail', 'canonical', 'id = 1'),
		
		'xivo_maintenance'   : (None, 'monitoring', 'maintenance'         , 'id = 1'),
		'alert_emails'       : (lambda x: x.replace(" ", "\r\n"), 'monitoring', 'alert_emails', 'id = 1'),
		'dahdi_monitor_ports': (None, 'monitoring', 'dahdi_monitor_ports' , 'id = 1'),
		'max_call_duration'  : (None, 'monitoring', 'max_call_duration'   , 'id = 1'),
	}
	def __init__(self, cursor):
		self.cursor       = cursor
		self.extra_ifaces = ()

		# get network config
		headers = {
			"Content-type": "application/json",
			"Accept": "text/plain"
		}

		conn = httplib.HTTPConnection('localhost', 8668)
		conn.request('GET', '/discover_netifaces', None, headers)
		response = conn.getresponse()
		if response.status != 200:
			raise Exception('cannot query network configuration !!!')
		self.network = cjson.decode(response.read())
#		import pprint; pprint.pprint(self.network)

		self.ifaces_candidates = []
		self.extra_ifaces      = []
		self.vlanid            = None
		self.net4cidr          = None

	def docomplete(self):
		"""Complete migration (networking part)
		"""
		iface = None

		if len(self.ifaces_candidates) == 0:
			print " * WARNING: no VoIP defined"
		elif len(self.ifaces_candidates) > 1:
			print " * WARNING: you can only set ONE VoIP interface (value is '%s'). Will try to autoselect one..."

		notfound = set(self.ifaces_candidates).difference(self.network)
		if len(notfound) > 0:
			print " * ERROR: *%s* interfaces not configured in the system" % list(notfound)
			return

		if self.vlanid is None:
			iface = self.ifaces_candidates[0]
			del self.ifaces_candidates[0]
		else:
			# found matching device
			vlandev = filter(lambda x: 'vlan-id' in self.network[x] and self.network[x]['vland-id'] == self.vlandid, self.network)
			if len(vlandev) == 0:
				print " * ERROR: *%d* vlanid does not match with any network interface" % self.vlanid
				return
			elif len(vlandev) > 1:
				print " * ERROR: *%d* vlanid match with more than on network interface (%s)" % (self.vlanid, vlandev)
				return
			elif vlandev[0] not in self.ifaces_candidates:
				print " * ERROR: *%d* vlanid does not match any of declared network interfaces (%s)" % (self.vlanid, self.ifaces_candidates)

			del self.ifaces_candidates[self.ifaces_candidates.index(vlandev[0])]
			iface = vlandev[0]

		self.extra_ifaces.extend(self.ifaces_candidates)

		# saving voip network interface
		dct = self.network[iface]
		if self.net4cidr is not None:
			net4, cidr = self.net4cidr.split('/')
			if net4 != dct['address']:
				print " * ERROR: NET4_CIDR (%s) does not match interface address (%s: %s)" % (net4, iface, dct['address'])
			
		parameters = [
			iface,
			iface,
			dct['hwtypeid'],
			'voip',          # networktype
			dct['method'],
			dct.get('address'),
			dct.get('netmask'),
			dct.get('broadcast'),
			dct.get('gateway'),
			dct['mtu']
		]

		self.cursor.query("INSERT INTO netiface VALUES(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, 0, 'created by 1.1 migration tool')", parameters=parameters)

		if len(self.extra_ifaces) > 0:
			xtra_ifaces = ' '.join(self.extra_ifaces)
			for iface in xtra_ifaces:
				dct = self.network[iface]
				parameters = [iface, iface, dct['hwtypeid'], 'voip', dct['method'], dct.get('address'), dct.get('netmask'), dct.get('broadcast'), dct.get('gateway'), dct['mtu']]
				self.cursor.query("INSERT INTO netiface VALUES(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, 0, 'created by 1.1 migration tool')", parameters=parameters)

			self.cursor.query(
				"UPDATE dhcp SET extra_ifaces = %s WHERE id = 1", 
				parameters = (val,)
			)


	# specific callbacks
	def xivo_voip_ifaces(self, val):
		self.ifaces_candidates = val.split(' ')
#		if len(ifaces) > 1:
#			print " * WARNING: you can only set ONE VoIP interface (value is '%s')." % val + \
#				" Selected *%s* as VoIP interface" % ifaces[0]
#			self.extra_ifaces = ifaces[1:]

	def xivo_voip_vlan_id(self, val):
		self.vlanid = int(val)

	def xivo_net4_cidr(self, val):
		self.net4cidr = val

	def xivo_nameservers(self, val):
		if len(val.strip()) == 0:
			return
		
		ns = val.split(' ')
		for i in xrange(0, min(3, len(ns))):
			self.cursor.query(
				"UPDATE resolvconf SET nameserver%d = %%s WHERE id = 1" % (i+1), 
				parameters = (ns[i],)
		);

	def xivo_dhcp_pool(self, val):
		if len(val) == 0:
			return

		(start, end) = val.split(' ')
		self.cursor.query(
				"UPDATE dhcp SET pool_start = %s, pool_end = %s WHERE id = 1", 
				parameters = (start, end)
		)
		
	def xivo_dhcp_extra_ifaces(self, val):
#		if len(self.extra_ifaces) > 0:
#			val += ' ' + ' '.join(self.extra_ifaces)
#		val = val.strip()

#		self.cursor.query(
#				"UPDATE dhcp SET extra_ifaces = %s WHERE id = 1", 
#				parameters = (val,)
#		)
		self.extra_ifaces = val


	# callback function
	def __fallback(self, xformer, val):
		#1 value transform
		if xformer[0] is not None:
			val = xformer[0](val)

		return self.cursor.query(
			"UPDATE %s SET %s = %%s WHERE %s" % xformer[1:], 
			parameters = (val,)
		);

	def __getattr__(self, name):
		if name in self.xforms:
			def fallback(val):
				return self.__fallback(self.xforms[name], val)
			return fallback
			
		def void(self, *args):
			print " * WARNING: callback not found for", name.upper(), "key"
			return True
		return void

if __name__ == '__main__':
	if not os.path.exists(COMMON_CONFFILE):
		print "common.conf file not found. exit..."; sys.exit(0)

	#WARNING: we must remove trailing double quotes
	db_uri = ConfigDict.ReadSingleKey(XIVO_CONFFILE, 'general', 'datastorage')[1:-1]
	cursor = db_connect(db_uri).cursor()
	if cursor is None:
		print "cannot connect to xivo database. exit..."; sys.exit(0)

	print " * INFO: starting common.conf migration"
	cc = CommonConf(cursor)

	with open(COMMON_CONFFILE) as f:
		for l in f.xreadlines():
			if l.startswith('#') or '=' not in l:
				continue
		
			(key, value) = l[:-1].strip().split('=')
#			print "%s = %s" % (key, value)
			getattr(cc, key.lower())(value.replace('"', '').strip())

	cc.docomplete()
	print " * INFO: common.conf migration complete"

