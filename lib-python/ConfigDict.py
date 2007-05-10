# -*- coding: iso-8859-15 -*-
"""Support routines to load content of configuration sections to a dictionary

Copyright (C) 2007, Proformatique

"""

REV_DATE = "$Revision$ $Date$"

from ConfigParser import ConfigParser, NoSectionError

def FillDictFromConfigSection(dicttofill, confpars_obj, section):
	"""Fills dicttofill with the content of the wanted section of the
	configuration file represented here by an instance of a ConfigParser
	having already parsed the content of the file.
	
	Names from the configuration file are used as dictionary keys, and
	the value associated with a given name in the config file is assigned
	at the corresponding key position of the dictionary.
	
	No particular care is taken to avoid overriding existing dictionary
	entries, so the caller can prefill it with default values. Indeed when
	an entry found in the configuration file already exists in the
	dictionary and its pre-existing content is not of type type(None), the
	string extracted from the config file is converted in the type the old
	content of the dictionary has, before assignment.
	
	Returns the updated dicttofill so you can do - if you want so:
		dico = FillDictFromConfigSection({}, conf, sec)
	
	"""
	items = confpars_obj.items(section)
	for name,val in items:
		if name in dicttofill:
			t = type(dicttofill[name])
			if t is not type(None):
				oldval = val
				try:
					val = t(val)
				except ValueError:
					raise ValueError, "Content of \"%s\" in configuration file should be of type \"%s\", but the actual value found there \"%s\" is not" % (name, str(t), oldval)
		dicttofill[name] = val
	return dicttofill
	
__all__ = ["NoSectionError", "FillDictFromConfigSection"]
