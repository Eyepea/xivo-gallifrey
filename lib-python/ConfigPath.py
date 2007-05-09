# -*- coding: iso-8859-15 -*-
"""Support routines for ordered injections in sys.path from an .ini file

Copyright (C) 2007, Proformatique

"""

REV_DATE = "$Revision$ $Date$"

import sys
from ConfigParser import ConfigParser, NoSectionError
from os.path import abspath

def SortedValuesFromConfigSection(config_file, config_section):
	"""Returns a list of sorted values appearing in section config_section
	of file config_file, by their respective keys. One cannot retrieves the
	keys, so their name are only useful for the sorting operation.
	
	The purpose of this function is to retrieve and return a list of
	additional directories that can be added to sys.path so that python
	modules from these directories can be transparently used in the rest
	of the program.
	
	Exceptions on config_file IO are propagated.
	NoSectionError is also propagated from ConfigParser if config_section
	is missing.
	
	"""
	conf = ConfigParser()
	conf.readfp(open(config_file))
	list_lib_path = conf.items(config_section)
	list_lib_path.sort()
	return map(lambda (k,v): v, list_lib_path)

def InsertPathListSys(lst_path):
	"""Given a list of paths, insert each one not already present in
	sys.path, at the start, preserving the order of the newly inserted
	ones.
	
	"""
	abssyspath = map(abspath, sys.path)
	for path in lst_path[::-1]:
		x = abspath(path)
		if (x not in abssyspath):
			sys.path.insert(0, x)

__all__ = ["NoSectionError", "SortedValuesFromConfigSection", "InsertPathListSys"]
