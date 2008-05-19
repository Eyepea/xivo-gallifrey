"""Support for Aastra phones for XIVO Autoprovisioning

Aastra 51i, 53i, 55i and 57i are supported.

Copyright (C) 2008  Proformatique

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

import os
import sys
import syslog

from xivo_provisioning import provsup
from xivo_provisioning.provsup import BaseProv
from xivo_provisioning.provsup import ProvGeneralConf as pgc

AASTRA_COMMON_DIR = pgc['tftproot'] + 'Aastra/'
AASTRA_COMMON_HTTP_USER = 'admin'
AASTRA_COMMON_HTTP_PASS = '22222'

class AastraProv(BaseProv):
	label = "Aastra"
	def __init__(self, phone):
		BaseProv.__init__(self, phone)
		# TODO: handle this with a lookup table stored in the DB?
		if self.phone['model'] not in ['51i', '53i', '55i', '57i']:
			raise ValueError, "Unknown Aastra model '%s'" % self.phone['model']

	def __action(self, command, user, passwd):
		## curl options
		# -s			-- silent
		# -o /dev/null		-- dump result
		# --connect-timeout 30  -- timeout after 30s
		# -retry 0		-- don't retry
                # -d DATA               -- post DATA

                # we first make an attempt, which replies : 401 Unauthorized (Authorization failed).
		os.system(pgc['curl_cmd'] + ' --retry 0 --connect-timeout %s -s -o /dev/null -u %s:%s http://%s'
                          % (str(pgc['curl_to_s']), user, passwd, self.phone['ipv4']))
                # we make a second attempt, which does not fail
		os.system(pgc['curl_cmd'] + ' --retry 0 --connect-timeout %s -s -o /dev/null -u %s:%s http://%s'
                          % (str(pgc['curl_to_s']), user, passwd, self.phone['ipv4']))

                # once we have been authenticated, we can POST the appropriate commands

                # first : upgrade
                # this seems to be compulsory when taking the phones out of their box, since the tftpboot parameters
                # can not handle the Aastra/ subdirectory with the out-of-the-box version
                os.system(pgc['curl_cmd'] + ' --retry 0 --connect-timeout %s -s -o /dev/null -u %s:%s http://%s/upgrade.html -d "tftp=%s&file=Aastra/%s.st"'
                          % (str(pgc['curl_to_s']), user, passwd, self.phone['ipv4'], pgc['asterisk_ipv4'], self.phone['model']))

                # then reset
                os.system(pgc['curl_cmd'] + ' --retry 0 --connect-timeout %s -s -o /dev/null -u %s:%s http://%s/reset.html -d "resetOption=0"'
                          % (str(pgc['curl_to_s']), user, passwd, self.phone['ipv4']))

                
	def do_reinit(self):
		"""Entry point to send the (possibly post) reinit command to
		the phone.
		
		"""
		self.__action('reboot', AASTRA_COMMON_HTTP_USER, AASTRA_COMMON_HTTP_PASS)

	def do_reboot(self):
		"Entry point to send the reboot command to the phone."
		self.__action('reboot', AASTRA_COMMON_HTTP_USER, AASTRA_COMMON_HTTP_PASS)

	def __generate(self, myprovinfo):
		"""Entry point to generate the provisioned configuration for
		this phone.
		
		"""
                __model = self.phone['model']
                __macaddr = self.phone['macaddr'].upper().replace(':','')
		template_file = open(pgc['templates_dir'] + "aastra-" + __model + ".cfg")
		template_lines = template_file.readlines()
		template_file.close()
		tmp_filename = AASTRA_COMMON_DIR + __macaddr + '.cfg.tmp'
		cfg_filename = tmp_filename[:-4]
		txt = provsup.txtsubst(template_lines,
                                       { 'user_display_name': myprovinfo['name'],
                                         'user_phone_ident':  myprovinfo['ident'],
                                         'user_phone_number': myprovinfo['number'],
                                         'user_phone_passwd': myprovinfo['passwd']
                                         },
                                       cfg_filename)
		tmp_file = open(tmp_filename, 'w')
		tmp_file.writelines(txt)
		tmp_file.close()
		os.rename(tmp_filename, cfg_filename)

	def do_autoprov(self, provinfo):
		"""Entry point to generate the provisioned configuration for
		this phone.
		
		"""
		self.__generate(provinfo)

	def do_reinitprov(self):
		"""Entry point to generate the reinitialized (GUEST)
		configuration for this phone.
		
		"""
		self.__generate({ 'name':   'guest',
                                  'ident':  'guest',
                                  'number': 'guest',
                                  'passwd': 'guest'
                                  })


	# Introspection entry points

	def get_phones(cls):
		"Report supported phone models for this vendor."
		return (('51i', '51i'), ('53i', '53i'), ('55i', '55i'), ('57i', '57i'))
	get_phones = classmethod(get_phones)

	# Entry points for the AGI

	def get_vendor_model_fw(cls, ua):
		"""Extract Vendor / Model / FirmwareRevision from SIP User-Agent
		or return None if we don't deal with this kind of Agent.
		
		"""
                # Aastra 53i/2.2.0.166
                # Aastra 55i/2.2.0.166

		ua_splitted = ua.split('/', 1)
                if ua_splitted[0] != 'Aastra':
			return None
                model = 'unknown'
                fw = 'unknown'
                if len(ua_splitted) == 2:
			modelfw = ua_splitted[1].split('-', 1)
                        model = modelfw[0].lower()
                        if len(modelfw) == 2:
                                fw = modelfw[1]
		return ("aastra", model, fw)
	get_vendor_model_fw = classmethod(get_vendor_model_fw)

provsup.PhoneClasses['aastra'] = AastraProv
