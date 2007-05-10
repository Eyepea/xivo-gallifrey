# -*- coding: iso-8859-15 -*-
"""Support routines to load content of configuration sections to a dictionary

Copyright (C) 2007, Proformatique

"""

REV_DATE = "$Revision$ $Date$"

from ConfigParser import ConfigParser, NoSectionError, NoOptionError

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
	for name,val in confpars_obj.items(section):
		if name in dicttofill:
			t = type(dicttofill[name])
			if t is not type(None):
				oldval = val
				try:
					val = t(val)
				except ValueError:
					raise ValueError, "Content of \"%s\" in section \"%s\" of configuration file should be of type \"%s\", but the actual value found there \"%s\" is not" % (name, section, str(t), oldval)
		dicttofill[name] = val
	return dicttofill

def FillDictFromMultipleConfig(dicttofill, confpars_obj, sect_mapping):
	"""Acts the same as FillDictFromConfigSection(), but instead of filling
	the dictionary with every entries of a given section of the
	configuration file, uses the other dictionary sect_mapping to get the
	list of keys to populate in dicttofill and their corresponding section
	and name in the configuration file.
	
	Also the wanted type is not deduced from an existing entry in
	dicttofill but is instead explicitely given in a sect_mapping entry.
	
	The structure of sect_mapping is the following:
	
		{ 'key_for_dicttofill': ('section', 'name', type), ... }
	
	An entry missing in the configuration file will result in the raise of
	the corresponding ConfigParser exception.
	
	"""
	for key,(section, name, t) in sect_mapping.iteritems():
		val = confpars_obj.get(section, name)
		if t is not None and t is not type(None):
			oldval = val
			try:
				val = t(val)
			except ValueError:
				raise ValueError, "Content of \"%s\" in section \"%s\" of configuration file should be of type \"%s\", but the actual value found there \"%s\" is not" % (name, section, str(t), oldval)
		dicttofill[key] = val
	return dicttofill

def FillDictFromMultipleConfigOpt(dicttofill, confpars_obj, sect_mapping):
	"""Acts the same as FillDictFromMultipleConfig(), but won't propagate
	exception in case an entry is missing - that is
	ConfigParser.NoSectionError and ConfigParser.NoOptionError are catched.
	
	When a configuration file entry is missing, the corresponding key is not
	inserted in dicttofill. Nevertheless if the corresponding key
	pre-existed in dicttofill, it is not removed.
	
	"""
	for key,(section, name, t) in sect_mapping.iteritems():
		try:
			val = confpars_obj.get(section, name)
			if t is not None and t is not type(None):
				oldval = val
				try:
					val = t(val)
				except ValueError:
					raise ValueError, "Content of \"%s\" in section \"%s\" of configuration file should be of type \"%s\", but the actual value found there \"%s\" is not" % (name, section, str(t), oldval)
			dicttofill[key] = val
		except NoSectionError, NoOptionError:
			pass
	return dicttofill

__all__ = ["NoSectionError", "NoOptionError",
           "FillDictFromConfigSection",
           "FillDictFromMultipleConfig",
	   "FillDictFromMultipleConfigOpt"]
