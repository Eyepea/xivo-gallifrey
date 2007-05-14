# -*- coding: iso-8859-15 -*-
"""DBAPI Helper using URI to create talk to various DB

Copyright (C) 2007, Proformatique

WARNING: this module is not DBAPI 2.0 compliant by itself

"""

__version__ = "$Revision$ $Date$"

import urisup

__uri_create_methods = {}

any_paramstyle='format'
any_threadsafety=1
any_apilevel='2.0'

def __compare_api_level(als1, als2):
	lst1 = map(int, als1.split('.'))
	lst2 = map(int, als2.split('.'))
	if lst1 < lst2:
		return -1 - bool(lst1[0] < lst2[0])
	elif lst1 > lst2:
		return 1 + bool(lst1[0] > lst2[0])
	else:
		return 0

def register_uri_backend(uri_scheme, create_method, module):
	"""This method is intended to be used by backends only.
	
	It lets them register their services, identified by the URI scheme,
	at import time. The associated method create_method must take one
	parameter: the complete requested RFC 3986 compliant URI. The
	associated module must be compliant with DBAPI v2.0 but will not be 
	directly used for other purposes than compatibility testing.
	
	If something obviously not compatible is tried to be registred,
	NotImplementedError is raised.
	
	"""
	try:
		delta_api =  __compare_api_level(module.apilevel, any_apilevel)
		mod_paramstyle = module.paramstyle
		mod_threadsafety = module.threadsafety
	except NameError:
		raise NotImplementedError, "This module does not support registration of non DBAPI services of at least apilevel 2.0"
	if delta_api < 0 or delta_api > 1:
		raise NotImplementedError, "This module does not support registration of DBAPI services with a specified apilevel of %s" % module.apilevel
	if mod_paramstyle != 'pyformat' and mod_paramstyle != 'format':
		raise NotImplementedError, "This module only supports registration of DBAPI services with a 'format' or 'pyformat' paramstyle, not '%s'" % mod_paramstyle
	if mod_threadsafety < any_threadsafety:
		raise NotImplementedError, "This module does not support registration of DBAPI services of threadsafety %d (more generally under %d)" % (mod_threadsafety, any_threadsafety)
	if not urisup.valid_scheme(uri_scheme):
		raise urisup.InvalidSchemeError, "Can't register an invalid URI scheme \"%s\"" % uri_scheme
	__uri_create_methods[uri_scheme] = (create_method, module)

def connect_by_uri(sqluri):
	"""Same purpose as the classical DBAPI v2.0 connect constructor, but
	with a unique prototype and routing the request to a registred method
	for this uri. It is not the responsibility of this anysql module to
	load any backend SQL implementation, so be sure the application as
	imported the correct one before calling this constructor.
	
	If no handler is found for this method, a NotImplementedError will be
	raised.
	
	A malformed URI will result in an exception being raised by the
	supporting URI parsing module.
	
	"""
	uri_scheme = urisup.uri_help_split(sqluri)[0]
	if uri_scheme not in __uri_create_methods:
		raise NotImplementedError, 'Unknown URI scheme "%s"' % str(uri_scheme)
	__uri_create_methods[uri_scheme][0](sqluri)

__all__ = ["register_uri", "connect_by_uri",
           "paramstyle", "threadsafety", "apilevel"]
