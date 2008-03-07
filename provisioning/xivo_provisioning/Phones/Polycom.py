"""Support for Polycom phones for XIVO Autoprovisioning

Polycom SoundPoint IP 430 SIP and SoundPoint IP 650 SIP are supported.

Copyright (C) 2007, 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

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
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

import os, sys, syslog
import socket
import provsup
from provsup import BaseProv
from provsup import ProvGeneralConf as pgc

POLYCOM_COMMON_DIR = pgc['tftproot'] + "Polycom/"
POLYCOM_COMMON_HTTP_USER = "Polycom"
POLYCOM_COMMON_HTTP_PASS = "456"
SIP_PORT = 5060

class PolycomProv(BaseProv):
	label = "Polycom"
	def __init__(self, phone):
		BaseProv.__init__(self, phone)
                self.provinfo = {}
                self.provinfo_prev = {}
		# TODO: handle this with a lookup table stored in the DB?
		if self.phone["model"] != "spip_430" and \
		   self.phone["model"] != "spip_650":
			raise ValueError, "Unknown Polycom model '%s'" % self.phone["model"]

        def __sendsipnotify(self):
                phoneip = self.phone['ipv4']
                myip = pgc['asterisk_ipv4']
                if 'number' in self.provinfo_prev:
                        sip_number = self.provinfo_prev['number']
                else:
                        sip_number = 'guest'
                sip_message = [ 'NOTIFY sip:%s@%s:%d SIP/2.0' %(sip_number, phoneip, SIP_PORT),
                                'Via: SIP/2.0/UDP %s' %(myip),
                                'From: <sip:%s@%s>' %(sip_number, myip),
                                'To: <sip:%s@%s>' %(sip_number, phoneip),
                                'Event: check-sync',
                                'Call-ID: callid-reboot@%s' %(myip),
                                'CSeq: 1300 NOTIFY',
                                'Contact: <sip:%s@%s>' %(sip_number, myip),
                                'Content-Length: 0']
                sipsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                sipsocket.sendto('\r\n'.join(sip_message), (phoneip, SIP_PORT))
                self.provinfo_prev = self.provinfo

	def do_reboot(self):
		"Entry point to send the reboot command to the phone."
                self.__sendsipnotify()

	def do_reinit(self):
		"""Entry point to send the (possibly post) reinit command to
		the phone.
		
		"""
                self.__sendsipnotify()


	def __generate(self, myprovinfo):
                template_main_file = open(pgc['templates_dir'] + "polycom-%s.cfg" % self.phone["model"])
		template_main_lines = template_main_file.readlines()
		template_main_file.close()
                template_phone_file = open(pgc['templates_dir'] + "polycom-phone.cfg")
		template_phone_lines = template_phone_file.readlines()
		template_phone_file.close()

                __macaddr = self.phone["macaddr"].replace(':','').lower()
		tmp_main_filename = POLYCOM_COMMON_DIR + __macaddr + ".cfg.tmp"
		cfg_main_filename = tmp_main_filename[:-4]
		tmp_phone_filename = POLYCOM_COMMON_DIR + __macaddr + "-phone.cfg.tmp"
		cfg_phone_filename = tmp_phone_filename[:-4]

                txt_main = provsup.txtsubst(template_main_lines,
                                            { "phone.cfg": __macaddr + "-phone.cfg" },
                                            cfg_main_filename)
		txt_phone = provsup.txtsubst(template_phone_lines,
                                            { "user_display_name": myprovinfo["name"],
                                              "user_phone_ident":  myprovinfo["ident"],
                                              "user_phone_number": myprovinfo["number"],
                                              "user_phone_passwd": myprovinfo["passwd"],
                                              "asterisk_ipv4" : pgc['asterisk_ipv4']
                                              },
                                            cfg_phone_filename)

		tmp_main_file = open(tmp_main_filename, 'w')
		tmp_main_file.writelines(txt_main)
		tmp_main_file.close()
		tmp_phone_file = open(tmp_phone_filename, 'w')
		tmp_phone_file.writelines(txt_phone)
		tmp_phone_file.close()

		os.rename(tmp_main_filename, cfg_main_filename)
		os.rename(tmp_phone_filename, cfg_phone_filename)


	def do_reinitprov(self):
		"""Entry point to generate the reinitialized (GUEST)
		configuration for this phone.
		
		"""
                self.provinfo = { "name":   "guest",
                                  "ident":  "guest",
                                  "number": "guest",
                                  "passwd": "guest"
                                  }
                self.__generate(self.provinfo)


	def do_autoprov(self, provinfo):
		"""Entry point to generate the provisioned configuration for
		this phone.
		
		"""
                self.provinfo = provinfo
		self.__generate(self.provinfo)


	# Introspection entry points

	@classmethod
	def get_phones(cls):
		"Report supported phone models for this vendor."
		return (("spip_430", "SPIP430"), ("spip_650", "SPIP650"))

	# Entry points for the AGI

	@classmethod
	def get_vendor_model_fw(cls, ua):
		"""Extract Vendor / Model / FirmwareRevision from SIP User-Agent
		or return None if we don't deal with this kind of Agent.
		
		"""
                # PolycomSoundPointIP-SPIP_430-UA/2.2.0.0047
                # PolycomSoundPointIP-SPIP_650-UA/2.2.0.0047

                if ua[:7] != 'Polycom':
                        return None
                model = 'unknown'
                fw = 'unknown'
                ua_splitted = ua.split('/', 1)
                if len(ua_splitted) == 2:
                        fw = ua_splitted[1]
                        model = ua_splitted[0].split('-')[1].lower()
		return ("polycom", model, fw)

provsup.PhoneClasses["polycom"] = PolycomProv
