"""Ordered Configuration Parser useful when the order of options counts.

Copyright 1991-1995 by Stichting Mathematisch Centrum,
                       Amsterdam, The Netherlands.
Copyright (C) 2007, Proformatique

This module derivates from the ConfigParser module distributed in Python 2.4

A setup file consists of sections, lead by a "[section]" header,
and followed by "name: value" entries, with continuations and such in
the style of RFC 822.

Unlike the ConfigParser module, this one can't contain format strings refering
to other values. Also you can't load multiple files, modify the parsed parsed
configuration, or write it back in a config file. Multiple sections with the
same name are handled differently, and so are multiple options in a given
section.

Keep in mind the following API and behavioral level differences and precisions
when using OrderedRawConf of this module instead of RawConfigParser of the
ConfigParser module, and remember this LEGACY API is provided only for LEGACY
CODE COMPATIBILITY and WONT BE OF ANY USE if you must RETRIEVE OPTIONS in an
ORDERED WAY (in which case you'll have to use the newly introduced methods and
iterators described in the classes PyDoc):

* __init__ takes following parameters:
    (fp=None, filename=None, sect_trans=lambda x:x, opt_trans=lambda x:x)
  They are not compatible at all with the arguments of the RawConfigParser
  __init__.
  See class PyDoc for details about theses parameters.

* defaults() is not available.

* sections() doesn't consider that DEFAULT has a special meaning. Every
  elements of the returned list are in the output set of 'sect_trans'. The
  same element can't be seen two times in the returned list.

* add_section(section) is not available.

* has_section(section): the 'sect_trans' function used at instantiation is used
  on the section parameter before any lookup.

* options(section): the 'sect_trans' function used at instantiation is used
  on the section parameter before any lookup. Every elements of the returned
  list are in the output set of 'opt_trans'. The same element can't be seen
  multiple times in the returned list. If the parsed configuration file
  contained multiple sections for which the result of the 'sect_trans' function
  is the same, the first one in the file order is selected.

* has_option(section, option): 'sect_trans' is applied on 'section' and 
  'opt_trans' on 'option' before lookup.

* read(filenames) is not available.

* readfp(fp[, filename]) can only be called once and can automatically be
  called by the constructor.

* get(section, option)
* getint(section, option)
* getfloat(section, option)
* getboolean(section, option):
  'sect_trans' is applied on 'section' and 'opt_trans' on 'option' before lookup

* items(section): 'sect_trans' is applied on 'section' before lookup. Names of
  the returned list of '(name, value)' pairs are in the output set of
  'opt_trans' and the same name can't be seen multiple times. If the parsed
  configuration file contained multiple sections for which the result of the
  'sect_trans' function is the same, the first one seen in the file order is
  selected. If the selected section contains multiple option names for which
  the result of 'opt_trans' is the same, only the first one seen in the file
  order is returned.

* set(section,option,value) is not available.

* write(fileobject) is not available.

* remove_option(section,option) is not available.

* remove_section(section) is not available.

* optionxform(option) is not available but you can achieve similar functions
  by setting 'opt_trans' at instantiation time.

For improved SAFETY, be sure to USE some of the FOLLOWING TESTS METHODS in an
appropriate way BEFORE USING the OLD API:

* has_conflicting_section_names, has_conflicting_option_names,
  has_any_conflicting_option_name, is_probably_safe_with_old_api

The following methods are recommended for newly written code that intends to
use OrderedRawConf and should be preferred:

  - Methods on OrderedRawConf instances:
      ordered_sections, has_section, get_conflicting_section_names,
      get_conflicting_option_names, get_all_conflicting_option_names,
      __iter__ (via standard python iterations)
  - Methods on SectionDesc instances (during iteration over sections):
      TODO
  - Methods on OptionDesc instances (during iteration over sections):
      TODO

Under some conditions, the following new methods are also useful:

  - Methods on OrderedRawConf instances:
      ordered_items, iter_options
  - Methods on SectionDesc instances (during iteration over sections):
      TODO
  - Methods on OptionDesc instances (during iteration over sections):
      TODO

See the the classes PyDoc for full description.

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright 1991-1995 by Stichting Mathematisch Centrum,
                           Amsterdam, The Netherlands.
    Copyright (C) 2007, Proformatique
    					All Rights Reserved

    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose and without fee is hereby granted, provided
    that the above copyright notice appear in all copies and that both that
    copyright notice and this permission notice appear in supporting
    documentation, and that the names of Stichting Mathematisch Centrum or CWI
    or Corporation for National Research Initiatives or CNRI not be used in
    advertising or publicity pertaining to distribution of the software without
    specific, written prior permission.

    While CWI is the initial source for this software, a modified version is
    made available by the Corporation for National Research Initiatives (CNRI)
    at the Internet address http://www.python.org.

    STICHTING MATHEMATISCH CENTRUM AND CNRI DISCLAIM ALL WARRANTIES WITH REGARD
    TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
    FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH CENTRUM OR CNRI BE LIABLE
    FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
    RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
    CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
    CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
"""

from ConfigParser import Error, NoSectionError, DuplicateSectionError, \
NoOptionError, InterpolationError, InterpolationMissingOptionError, \
InterpolationSyntaxError, InterpolationDepthError, ParsingError, \
MissingSectionHeaderError

import re

SECTCRE = re.compile(
    r'\['                                 # [
    r'(?P<header>[^]]+)'                  # very permissive!
    r'\]'                                 # ]
    )
OPTCRE = re.compile(
    r'(?P<option>[^:=\s][^:=]*)'          # very permissive!
    r'\s*(?P<vi>[:=])\s*'                 # any number of space/tab,
                                          # followed by separator
                                          # (either : or =), followed
                                          # by any # space/tab
    r'(?P<value>.*)$'                     # everything up to eol
    )

class AlreadyLoadedError(Error):
	"""Raised when trying to load multiple files in an instance that
	doesn't support that.
	
	"""
	def __init__(self,done_filename,second_filename):
		Error.__init__(
			self,
			"Trying to load multiple files in an instance that can't do that.\n"
			"\tdone_filename   : %s\n"
			"\tsecond_filename : %s\n"
			% (done_filename, second_filename))
		self.done_filename = done_filename
		self.second_filename = second_filename

class SectionDesc:
	"""DOC TODO"""
	def __init__(self,conf_instance,sect_id_token):
		self.conf_instance = conf_instance
		self.sect_id_token = sect_id_token
	def get_section_token(self):
		return self.sect_id_token
	def __getattr__(self,name):
		try:
			func = getattr(self.conf_instance,'_'+name+'_by_section_token')
			return lambda *x: func(self.sect_id_token, *x)
		except AttributeError:
			raise AttributeError, name
	def __iter__(self):
		return self.conf_instance._iter_options_by_section_token(self.sect_id_token)

class OptionDesc:
	"""DOC TODO"""
	def __init__(self,conf_instance,opt_id_token):
		self.conf_instance = conf_instance
		self.opt_id_token = opt_id_token
	def get_option_token(self):
		return self.opt_id_token
	def __getattr__(self,name):
		try:
			func = getattr(self.conf_instance,'_'+name+'_by_option_token')
			return lambda *x: func(self.opt_id_token, *x)
		except AttributeError:
			raise AttributeError, name

def _str_to_boolean(v):
	_boolean_states = {'1': True, 'yes': True, 'true': True, 'on': True,
			   '0': False, 'no': False, 'false': False, 'off': False}
	if v.lower() not in _boolean_states:
		raise ValueError, 'Not a boolean: %s' % v
	return _boolean_states[v.lower()]

class OrderedRawConf:
	"""OrderedRawConf instances work a little like RawConfigParser with the
	main purpose to preserve order of both sections in configuration file
	and options in sections. While exposing a backward compability API
	consisting of a subset of the one of RawConfigParser (see module
	documentation for main variations and other details), it also provides
	a new API specialy designed to retrieve ordered entities in various
	pythonic ways :)
	
	"""
	def __init__(self,fp=None,filename=None,sect_trans=lambda x:x,opt_trans=lambda x:x):
		"""The constructor can optionally parse an open file or open
		and parse a file using its filename. You can also provide it
		canonicalization functions for section names and/or option
		names.
		
		- If 'fp' does not evaluate to False, the configuration will be
		  loaded from this opened file at instantiation time. If 'fp'
		  does evaluate to False but filename does not, the file linked
		  by 'filename' will be opened while the constructor is running
		  and the configuration will also be loaded from the opened
		  file.
		
		- Whether or not 'fp' evaluate to False, 'filename' has
		  precedence over 'fp.name' if not None, the latter being used
		  if it exists. The file name is used in exceptions.
		
		- 'sect_trans' is a function that must take a string as its
		  arguments and must return a string. It should behave as a
		  mathematical function, with no side effect. Section names
		  are stored as dictionary keys for latter use by a subset of
		  the RawConfigParser API after having been transformed by 
		  'sect_trans', so if for section names 'x' and 'y',
		  'sect_trans(x) == sect_trans(y)', only the first one in
		  configuration file order will be retrievable with this
		  compatibility API. Also for a given section name 't' any
		  lookup is of course done using 'sect_trans(t)' and not
		  directly 't'. Besides the default identity function, useful
		  ones could be 'tolower', or maybe even one stripping some
		  special character, performing multiple charset detection and
		  translation or performing other forms of canonicalization.
		
		- 'opt_trans' does the same thing as 'sect_trans' is for
		  similar use, but is the function to be applied to option
		  names instead of section names.
		
		"""
		self._sections = ([],{})
		self.loaded_filename = None
		self.sect_trans = sect_trans
		self.opt_trans = opt_trans
		auto_open = False
		if (not fp) and filename:
			auto_open = True
			fp = open(filename)
		if fp:
			self.readfp(fp,filename)
		if auto_open:
			fp.close()

	def sections(self):
		"""OLD API FOR RawConfigParser COMPATIBILITY
		
		Returns a list of section canonical names (as per
		'sect_trans') with at max one occurence of a given canonical
		name in the returned list in case multiple sections with the
		same canonical name exists.
		
		"""
		return self._sections[1].keys()

	def ordered_sections(self):
		"""NEW API
		
		Returns an ordered list of section names.
		
		"""
		return [n for n,v in self._sections[0]]

	def has_section(self,section):
		"""OLD API FOR RawConfigParser COMPATIBILITY
		But probably always safe to call.

		Test if a section with canonical name sect_trans(section) exists
		
		"""
		sec = self.sect_trans(section)
		return sec in self._sections[1]

	def readfp(self,fp,filename=None):
		"""Loads and parse opened file described by fp.
		fp.readline() will be used, and if filename is None fp.name
		will also be tried but is not mandatory.
		The file name is used in exceptions.
		
		Note that this function can only be called once and can be
		automatically called at instantiation time by __init__.

		"""
		if filename is None:
			try:
				filename = fp.name
			except AttributeError:
				filename = '<???>'
		if self.loaded_filename is not None:
			raise AlreadyLoadedError(self.loaded_filename,filename)
		self._read(fp, filename)
		self.loaded_filename = filename

	def _sectup_by_name(self,section):
		sec = self.sect_trans(section)
		if sec not in self._sections[1]:
			raise NoSectionError(section)
		return self._sections[1][sec]

	def get(self,section,option):
		"""OLD API FOR RawConfigParser COMPATIBILITY
		
		Get first option with canonical name opt_trans(option)
		from first section with canonical name sect_trans(section).
		If such a section is not found, NoOptionError is raised.
		Else if such an option is not found in the selected section,
		NoOptionError is raised.
		
		"""
		s = self._sectup_by_name(section)
		opt = self.opt_trans(option)
		if opt not in s[1]:
			raise NoOptionError(option,section)
		return s[1][opt]

	def items(self,section):
		"""OLD API FOR RawConfigParser COMPATIBILITY
		
		Returns the unordered list of (name, value) option pairs of
		the first section with canonical name sect_trans(section).
		If such a section is not found, NoOptionError is raised.
		If multiple options with the same canonical name exists in the
		selected section, only the first one of each different set will
		be returned.
		
		"""
		return self._sectup_by_name(section)[1].items()

	def ordered_items(self,section):
		"""NEW API - but could still be unsafe if
		has_conflicting_section_names() returns True.
		
		Returns the ordered list of (name, value) option pairs of
		the first section with canonical name sect_trans(section).
		If such a section is not found, NoOptionError is raised.
		It is possible to retrieve multiple options with the same
		canonical name, and their respective order are also preserved.
		
		"""
		return self._sectup_by_name(section)[0][:]

	def getint(self, section, option):
		"""OLD API FOR RawConfigParser COMPATIBILITY
		
		Same as get() but also cast the to be returned value as an int
		
		"""
		return int(self.get(section, option))

	def getfloat(self, section, option):
		"""OLD API FOR RawConfigParser COMPATIBILITY
		
		Same as get() but also cast the to be returned value as a float
		
		"""
		return float(self.get(section, option))

	def getboolean(self, section, option):
		"""OLD API FOR RawConfigParser COMPATIBILITY
		
		Same as get() but also cast the to be returned value as a 
		boolean. Case insensitive 'yes', 'true', 'on' and '1' evaluates
		to True. Case insensitive 'no', 'false', 'off' and '0' evaluates
		to False. Other values are forbidden and result in a ValueError
		exception to be raised.
		
		"""
		return _str_to_boolean(self.get(section, option))

	def options(self, section):
		"""OLD API FOR RawConfigParser COMPATIBILITY
		
		Returns the unordered list of canonical option names of
		the first section with canonical name sect_trans(section).
		If such a section is not found, NoOptionError is raised.
		If multiple options with the same canonical name exists in the
		selected section, only the first one of each different set will
		be returned.
		
		"""
		return self._sectup_by_name(section)[1].keys()

	def has_option(self, section, option):
		"""OLD API FOR RawConfigParser COMPATIBILITY
		
		Test if at least a section with canonical name
		sect_trans(section) exists and if the first matching one
		contains at least an option with canonical name
		opt_trans(option).
		
		"""
		sec = self.sect_trans(section)
		if sec not in self._sections[1]:
			return False
		return self.opt_trans(option) in self._sections[1][sec][1]

	def has_conflicting_section_names(self):
		"""TEST THAT BEFORE USING OLD API
		
		Test if multiple sections exists with the same canonical name.
		If this is the case it is probably unsafe to use the old API,
		even if the order is not important, because a RawConfigParser
		object would have merged the content of such sections in that
		situation and we do not.
		The user can decide by itself if such a situation is an error
		and if it is not he can still safely use the new ordered API.
		
		"""
		return len(self._sections[0]) != len(self._sections[1])

	def get_conflicting_section_names(self):
		"""NEW API - probably useful for diagnostic purposes in both
		NEW and OLD API oriented code.
		
		Returns the list of non canonicalized section names sets
		which corresponding canonicalized names conflicts.
		
		"""
		cf = []
		cd = {}
		for sec,opts in self._sections[0]:
			tsec = self.sect_trans(sec)
			if tsec not in cd:
				cf.append([sec])
				cd[tsec] = len(cf-1)
			else:
				cf[cd[tsec]].append(sec)
		return [tuple(c) for c in cf if len(c) > 1]

	def _has_conflicting_option_names_int(self,s):
		return (len(s[0]) != len(s[1]))

	def _get_conflicting_option_names_int(self,s):
		cf = []
		cd = {}
		for opt,val in s[0]:
			topt = self.opt_trans(opt)
			if topt not in cd:
				cf.append([opt])
				cd[topt] = len(cf-1)
			else:
				cf[cd[topt]].append(opt)
		return filter(lambda c: len(c) > 1, cf)

	def has_conflicting_option_names(self,section):
		"""TEST THAT BEFORE USING OLD API
		This function makes most sense when
		has_conflicting_section_names() returns False.
		
		Test if multiple options exists with the same canonical name
		in the first section with canonical name sect_trans(section).
		If such a section is not found, NoOptionError is raised.
		If multiple options exists it is probably unsafe to use the
		old API, even if the order is not important, because a
		RawConfigParser object might have behaved differently in that
		situation.
		The user can decide by itself if such a situation is an error
		and if it is not he can still safely use the new ordered API.
		
		"""
		return self._has_conflicting_option_names_int(
			self._sectup_by_name(section))

	def get_conflicting_option_names(self,section):
		"""NEW API - probably useful for diagnostic purposes in both
		NEW and OLD API oriented code.
		This function makes most sense when
		has_conflicting_section_names() returns False.
		
		Returns the list of non canonicalized option names sets
		which corresponding canonicalized names conflicts, in the
		first found section with a canonical name of sect_trans(section)
		If such a section is not found, NoOptionError is raised.
		
		"""
		return self._get_conflicting_option_names_int(
			self._sectup_by_name(section))

	def has_any_conflicting_option_name(self):
		"""TEST THAT BEFORE USING OLD API
		
		Test if any section contains options with conflicting canonical
		names.
		
		"""
		for sec,opts in self._sections[0]:
			if self._has_conflicting_option_names_int(opts):
				return True
		return False

	def get_all_conflicting_option_names(self):
		"""NEW API - probably useful for diagnostic purposes in both
		NEW and OLD API oriented code.
		
		Returns a list of tuples containing section name in the first
		element, and the list of conflicting canonical option name in
		the second.
		
		"""
		lst = []
		for sec,opts in self._sections[0]:
			lst.append((sec,self._get_conflicting_option_names_int(opts)))
		return lst

	def has_no_key_duplication(self):
		"""TEST THAT BEFORE USING OLD API
		
		Check that there is neither any conflicting section names, nor
		any conflicting option name in each sections.
		
		It is believed that you can safely use any public method if
		this one returns True, and that you should retain to do so for
		most OLD API ones and probably even some of the NEW API if it
		returns False.
		
		You might even want to abort report errors when this function
		returns False if you plan to use the NEW API, in some
		situations where it does not make sense to have duplicated
		section or option names.
		
		"""
		return (not self.has_conflicting_section_names()) \
			and (not self.has_any_conflicting_option_name())
	is_probably_safe_with_old_api = has_no_key_duplication

	def __iter__(self):
		"""NEW API
		
		Returns a section iterator for this parsed configuration object.
		
		The object returned at each iteration is an instance of
		SectionDesc and can be used to transparently access 
		_(.*)_by_section_token() methods of this class with the
		correct iterable section token abstract descriptor.
		
		"""
		for i in xrange(len(self._sections[0])):
			yield SectionDesc(self,i)

	def _sectup_by_tok(self,token):
		return self._sections[0][token][1]

	def _get_name_by_section_token(self,i):
		return self._sections[0][i][0]

	def _has_conflicting_option_names_by_section_token(self,i):
		return self._has_conflicting_option_names_int(
			self._sectup_by_tok(i))

	def _get_conflicting_option_names_by_section_token(self,i):
		return self._get_conflicting_option_names_int(
			self._sectup_by_tok(i))

	def _items_by_section_token(self,i):
		return self._sectup_by_tok(i)[1].items()

	def _ordered_items_by_section_token(self,i):
		return self._sectup_by_tok(i)[0][:]

	def _get_by_section_token(self,i,option):
		s = self._sectup_by_tok(i)
		section = self._get_name_by_section_token(i)
		opt = self.opt_trans(option)
		if opt not in s[1]:
			raise NoOptionError(option,section)
		return s[1][opt]

	def _getint_by_section_token(self,i,option):
		return int(self._get_by_section_token(i,option))

	def _getfloat_by_section_token(self,i,option):
		return float(self._get_by_section_token(i,option))

	def _getboolean_by_section_token(self,i,option):
		return _str_to_boolean(self._get_by_section_token(i,option))

	def _has_option_by_section_token(self,i,option):
		opt = self.opt_trans(option)
		return opt in self._sectup_by_tok(i)[1]

	def _iter_options_by_sectup(self,sectup):
		for j in xrange(len(sectup[0])):
			yield OptionDesc(self,(sectup,j))

	def _iter_options_by_section_token(self,i):
		return self._iter_options_by_sectup(self._sectup_by_tok(i))

	def iter_options(self,section):
		"""NEW API - but could still be unsafe if
		has_conflicting_section_names() returns True.
		
		Returns an option iterator for the first section having a
		canonical name of sect_trans(section).
		
		The object returned at each iteration is an instance of 
		OptionDesc and can be used to transparently access 
		_(.*)_by_option_token() methods of this class with the
		correct iterable option token abstract descriptor.
		
		"""
		return self._iter_options_by_sectup(self._sectup_by_name(section))

	def _get_name_by_option_token(self,(sectup,ot)):
		return sectup[0][ot][0]

	def _get_value_by_option_token(self,(sectup,ot)):
		return sectup[0][ot][1]

	def _getint_by_option_token(self,tok):
		return int(self._get_value_by_option_token(tok))

	def _getfloat_by_option_token(self,tok):
		return float(self._get_value_by_option_token(tok))

	def _getboolean_by_option_token(self,tok):
		return _str_to_boolean(self._get_value_by_option_token(tok))

	def _read(self,fp,filename):
		lineno = 0
		cur_sect = None
		cur_dict_sect = None
		cur_opt = None
		cur_dict_opt = None
		e = None			# None, or an exception
		while True:
			line = fp.readline()
			if not line:
				break
			lineno = lineno + 1
			# comment or blank line?
			if line.strip() == '' or line[0] in '#;':
				continue
			if line.split(None, 1)[0].lower() == 'rem' and line[0] in "rR":
				# no leading whitespace
				continue
			# continuation line?
			if line[0].isspace() and (cur_sect is not None) \
					     and (cur_opt is not None):
				value = line.strip()
				if value:
					key = cur_sect[0][cur_opt][0]
					newval = cur_sect[0][cur_opt][1] + '\n'\
						 + value
					cur_sect[0][cur_opt] = (key,newval)
					if cur_dict_opt is not None:
						cur_sect[1][cur_dict_opt] \
						    = newval
				continue
			# a section header or option header?
			# is it a section header?
			mo = SECTCRE.match(line)
			if mo:
				sectname = mo.group('header')
				tsec = self.sect_trans(sectname)
				newopt = ([],{})
				self._sections[0].append((sectname,newopt))
				cur_sect = newopt
				if tsec not in self._sections[1]:
					self._sections[1][tsec] = newopt
					cur_dict_sect = tsec
				else:
					cur_dict_sect = None
				cur_opt = None
				cur_dict_opt = None
				continue
			if cur_sect is None:
				raise MissingSectionHeaderError(filename, lineno, line)
			# an option line?
			mo = OPTCRE.match(line)
			if mo:
				optname, vi, optval = mo.group('option', 'vi', 'value')
				if vi in ('=', ':') and ';' in optval:
					# ';' is a comment delimiter only if it follows
					# a spacing character
					pos = optval.find(';')
					if pos > 0 and optval[pos-1].isspace():
						optval = optval[:pos]
				optname = optname.rstrip()
				topt = self.opt_trans(optname)
				optval = optval.strip()
				cur_sect[0].append((optname,optval))
				cur_opt = len(cur_sect[0]) - 1
				if topt not in cur_sect[1]:
					cur_dict_opt = topt
					cur_sect[1][topt] = optval
				else:
					cur_dict_opt = None
				continue
			if not e:
				e = ParsingError(filename)
			e.append(lineno, repr(line))
		if e:
			raise e
