"""Support for Polycom phones for XIVO Autoprovisioning

Polycom SoundPoint IP 430 SIP and SoundPoint IP 650 SIP are supported.

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import os, sys, syslog
import provsup
from provsup import BaseProv
from provsup import ProvGeneralConf as pgc

class PolycomProv(BaseProv):
	label = "Polycom"
	def __init__(self, phone):
		BaseProv.__init__(self, phone)


	# Introspection entry points

	def get_phones(cls):
		"Report supported phone models for this vendor."
		return (("SoundPoint IP 430 SIP","SoundPoint IP 650 SIP"),)
	get_phones = classmethod(get_phones)

	# Entry points for the AGI

	def get_vendor_model_fw(cls, ua):
		"""Extract Vendor / Model / FirmwareRevision from SIP User-Agent
		or return None if we don't deal with this kind of Agent.
		
		"""
                model = 'unknown'
                fw = 'unknown'
		return ("polycom", model, fw)
	get_vendor_model_fw = classmethod(get_vendor_model_fw)

provsup.PhoneClasses["polycom"] = PolycomProv
