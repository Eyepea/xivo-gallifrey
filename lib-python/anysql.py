# -*- coding: iso-8859-15 -*-
"""DBAPI Helper using URI to create talk to various DB

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date$"

import uriparse

__uri_create_methods = {}

def register_uri_backend(uri_scheme, create_method, module):
	"""This method is intended to be used by backends only.
	It lets them register their services, identified by the URI scheme,
	at import time. The associated method create_method must take one
	parameter: the complete requested URI. The associated module must
	be compliant with DBAPI v2.0 but will not be directly used for other
	purposes than compatibility testing.
	
	"""
	sal = map(module.apilevel.split('.')
	if int(sal[0]) != 2 or 
	__uri_create_methods[uri] = (create_method, module)

def connect_by_uri(sqluri):
	"""Same purpose as the classical DBAPI v2.0 connect constructor, but
	with a unique prototype and routing the request to a registred method
	for this uri. It is not the responsibility of this anysql module to
	load any backend SQL implementation, so be sure the application as
	imported the correct one before calling this constructor.
	
	If no handler is found for this method, a NotImplementedError will be
	raised.
	
	"""
	# todo: parse the sqluri
	__uri_create_methods[uri][0](sqluri)

__all__ = ["register_uri", "connect_by_uri"]
