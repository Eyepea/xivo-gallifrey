"""/etc/network/interfaces configuration file reader / writer

Copyright (C) 2007, Proformatique

This module only supports a subset of what is allowed by ifupdown, with at
least the following limitations:
- options must be indented while ifupdown does not require it.
- you can't use line continuation: this would be too complicated to parse and
  then do a formatting preserving rewrite of a modified version, so we just
  pretend this does not exists.

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, Proformatique

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
"""

# XXX don't directly raise Error exceptions

import re
from pyfunc import *
from lzslice import *
from itertools import *

# Empty lines or lines beginning with a "#" are comments:
STANZA_COMMENT_RE = r'^(\s*|#.*)$'
STANZA_COMMENT = re.compile(STANZA_COMMENT_RE)
IS_COMMENT = STANZA_COMMENT.match
IS_OP = lambda x: not STANZA_COMMENT.match(x)

# Lines not indented and not comments are where stanzas start:
STANZA_START_RE = r'^[^\s#].*$'
STANZA_START = re.compile(STANZA_START_RE)
IS_START = STANZA_START.match

# And it's also interesting to recognize non-comment and non-starter
# lines of the configuration file (option lines)
IS_XOPTION = lambda x: (not IS_COMMENT(x)) and (not IS_START(x))

# Space indent at beginning of line
INDENT_RE = r'^(\s*)'
INDENT = re.compile(INDENT_RE)

def xor(a, b):
	return (not(a)) != (not(b))

def equiv(a, b):
	return (not(a)) == (not(b))

class Error(Exception):
	"""Base class for stanza exceptions."""
	def __init__(self, msg=''):
		self.message = msg
		Exception.__init__(self, msg)
	def __repr__(self):
		return self.message
	__str__ = __repr__

class RedundantIfaces(Error):
	def __init__(self, redundant_ifaces_dict):
		Error.__init__(self,
			"The following network interfaces have been detected multiple times:\n"
			+ ''.join('\t' + k + ': ' + v + '\n'
		              for k, v in redundant_ifaces_dict.iteritems()))
		self.redundant_ifaces_dict = redundant_ifaces_dict

class InvalidStanzaPayload(Error):
	def __init__(self, list_tuple_pos_len):
		Error.__init__(self,
			"Invalid config. block(s) (lineno, numlines):\n"
			+ ''.join('\t' + str(tuple_pos_len) + '\n'
			          for tuple_pos_len in list_tuple_pos_len))
		self.list_tuple_pos_len = list_tuple_pos_len

class OptionSetError(Error):
	def __init__(self, name, value):
		Error.__init__(self,
			"Can't set option parameter %s to %s"
			% (repr(name), repr(value)))
		self.name = name
		self.value = value

class BadlyFormedStanza(InvalidStanzaPayload): pass
class BadlyFormedStarter(InvalidStanzaPayload): pass
class WrongStanzaType(InvalidStanzaPayload): pass
class EmptyStanza(InvalidStanzaPayload): pass
class UnsyncStanza(InvalidStanzaPayload): pass

class FilteredView:
	def __init__(self, f, array, empty_begin = None, empty_end = None):
		self.underlying = array
		self.filter = f
		self.virtual_begin = empty_begin
		self.virtual_end = empty_end
	def __iter__(self):
		return ifilter(self.filter, self.underlying)
	def __len__(self):
		return all_and_count(True for elt in self.underlying if self.filter(elt))
	def __getitem__(self, idx):
		if not isinstance(idx, int):
			raise TypeError, 'list indices must be integers'
		if idx >= 0:
			cnt = 0
			for elt in self.underlying:
				if not self.filter(elt):
					continue
				if cnt == idx:
					return elt
				cnt += 1
			raise IndexError, 'list index out of range'
		else:
			sub = [elt for elt in self.underlying if self.filter(elt)]
			return sub[idx]
	def _common_set_del(self, idx, cb, *data):
		if not isinstance(idx, int):
			raise TypeError, 'list indices must be integers'
		if idx >= 0:
			found_pos = -1
			cnt = 0
			for pos, elt in enumerate(self.underlying):
				if not self.filter(elt):
					continue
				if cnt == idx:
					found_pos = pos
					break
				cnt += 1
			if found_pos < 0:
				raise IndexError, 'list index out of range'
		else:
			found_pos = [pos for pos,elt in self.underlying if self.filter(elt)][idx]
		cb(found_pos, *data)
	def _setitem_cb(self, real_idx, new_elt):
		self.underlying[real_idx] = new_elt
	def __setitem__(self, idx, new_elt):
		self._common_set_del(idx, self._setitem_cb, new_elt)
	def _delitem_cb(self, real_idx):
		del self.underlying[real_idx]
	def __delitem__(self, idx):
		self._common_set_del(idx, self._delitem_cb)
	def prepend(self, new_elt):
		pos = first(pos for (pos,elt) in enumerate(self.underlying) if self.filter(elt))
		if pos is not None:
			self.underlying.insert(pos, new_elt)
		else:
			self.underlying.insert(self.virtual_begin)
	def _generic_insert(self, ins_pos, new_elt, correction):
		if not isinstance(ins_pos, int):
			raise TypeError, 'list indices must be integers'
		if ins_pos <= 0:
			self.prepend(new_elt)
			return 
		real_pos = nth((pos for (pos,elt) in enumerate(self.underlying) if self.filter(elt)), ins_pos)
		if real_pos is not None:
			self.underlying.insert(real_pos, new_elt)
		else:
			self.underlying.insert(self.virtual_end, new_elt)
	def insert_before(self, ins_pos, new_elt):
		self._generic_insert(ins_pos, new_elt, 0)
	insert = insert_before
	def insert_after(self, pos, new_elt):
		self._generic_insert(ins_pos, new_elt, 1)
	def append(self, new_elt):
		pos = last(pos for (pos,elt) in enumerate(self.underlying) if self.filter(elt))
		if pos is not None:
			self.underlying.insert(pos+1, new_elt)
		else:
			self.underlying.insert(self.virtual_end, new_elt)
	def extend(self, seq):
		for elt in seq:
			self.append(elt)

class Stanza(list):
	"""This class adds very useful "interfaces" like stanza related methods
	to the built-in python lists.
	
	You can pass it (or a derivative) to the constructor of StanzaFormat,
	so that it will split each stanza in an instance of this type.
	
	"""
	def itercomments(self):
		"Generator to iterate over comments and blank lines."
		return ifilter(IS_COMMENT, self)
	def iteroplines(self):
		"Generator to iterate over starter and option lines."
		return ifilter(IS_OP, self)
	def get_stanza_starter(self):
		"""Get the first found stanza starter (or None). There really
		should be one but this can't be guaranteed by this class"""
		return find(IS_START, self)
	def get_stanza_starter_line(self):
		"""Same as self.get_stanza_starter() except when a stanza
		starter is found, returns (position,line) instead of just line"""
		return first((p,line) for p,line in enumerate(self) if IS_START(line))
	def _get_indent_from_line(self, line, default='\t'):
		indent = None
		if line:
			mo = INDENT.match(line)
			if mo:
				indent = mo.group(1)
		if not indent:
			return default
		else:
			return indent
	def get_opline_indent(self, default='\t'):
		"""Get the stanza body indentation according to the one used by
		the first operational non starter line. """
		self._get_indent_from_line(find(IS_XOPTION, self), default)
	def has_payload(self):
		"""Returns True if any line is operational, otherwise
		returns False."""
		return find(IS_OP, self) is not None
	def has_valid_payload(self):
		"""Returns True if exactly one line is a stanza starter and
		every other non comment lines are after that one. """
		return bool(all_and_count(equiv(pos==0, IS_START(line))
		                          for pos, line in enumerate(self.iteroplines())))
	def has_invalid_payload(self):
		"""Equivalent to
		    self.has_payload() and not self.has_valid_payload() """
		return not all(equiv(pos==0, IS_START(line)) for pos, line in enumerate(self.iteroplines()))
	def has_stanza_starter(self):
		"""Returns True as soon as a valid stanza starter is detected,
		regardless of its position."""
		return find(IS_START, self) is not None
	def has_more_than_starter_payload(self):
		return at_least(2, lambda v: True, self.iteroplines())
	def insertcomment(self, index, commentline):
		"""Just insert the comment line at the given index, or raise an
		exception if commentline is indeed not a comment/blank line."""
		if not IS_COMMENT(commentline):
			raise Error("'%s' is not a comment/blank line" % commentline.rstrip())
		self.insert(index, commentline)
	def insertstarter(self, index, starterline):
		"""Just insert a stanza starter or raise an exception if:
		
		- starterline is indeed not a valid stanza starter
		- there's already one stanza starter in this stanza
		- this starter would be inserted after a non comment line
		  in this stanza
		
		"""
		if not IS_START(starterline):
			raise Error(
				"'%s' is not a valid stanza starter" % starterline.rstrip())
		if self.has_stanza_starter():
			raise Error(
				"trying to insert the stanza starter '%s' in a stanza where there is already one" % starterline.rstrip())
		line = find(IS_OP, lazy_(self)[:index])
		if (line):
			raise Error(
				"trying to insert stanza starter '%s' in a stanza after at least one non comment/blank line (first found '%s')" % (starterline.rstrip(), line.rstrip()))
		self.insert(index, starterline)
	def _common_iterate_to(self, seq_lines, f_first = None, f_last = None, f_stop = None, data = None):
		# This internal function iterates through a well formed stanza
		# contained in seq_lines, raising an exception if it's indeed 
		# a not so well formed one, and using f_first(pos, line, state,
		# data) to detect and extract the first matching line
		# identified by this function, f_last with the same prototype
		# but this time to extract the last matching line, and f_stop
		# still with the same prototype and used to stop iteration.
		#
		# (state, (first_pos, first_line), (last_pos, last_line),
		#  (end_pos, end_line)) is returned.
		#
		# f_first can also be None and in this case (-1, '') will be
		# returned instead of (first_pos, first_line), equivalent
		# behavior for f_last, and if f_stop is None iteration will
		# only end when there is no more line to handle.
		#
		# If there is no line in the stanza,
		# (0, (-1, ''), (-1, ''), (-1, '')) is returned
		#
		# f_first, f_last and f_stop are called after state update
		# caused by the current line
		#
		state = 0 # evaluates to 0 before the stanza starter is found
		pos, line = -1, ''
		first_pos, first_line = -1, ''
		last_pos, last_line = -1, ''
		for pos, line in enumerate(seq_lines):
			if state == 0:
				if IS_START(line):
					state = 1
				elif IS_OP(line):
					raise BadlyFormedStanza([])
			else:
				if IS_START(line):
					raise BadlyFormedStanza([])
			if f_first:
				if first_pos < 0 and f_first(pos, line, state, data):
					first_pos, first_line = pos, line
			if f_last:
				if f_last(pos, line, state, data):
					last_pos, last_line = pos, line
			if f_stop:
				if f_stop(pos, line, state, data):
					break
		return (state, (first_pos, first_line),
		               (last_pos, last_line),
			       (pos, line))
	def find_replace_option(self, finder, new_opt_line, data, auto_adjust = True, default = '\t'):
		if not IS_XOPTION(opline):
			raise Error("'%s' is not an option line" % opline.rstrip())
		(state, (first_pos, first_line),
		               (last_pos, last_line),
			       (pos, line)) \
		    = self._common_iterate_to(
		            self,
			    finder,
			    None,
			    None,
			    data)
		if state != 1:
			raise Error("Can not insert option line if no starter before")
		if first_pos < 0:
			raise Error("Did not found an option line to replace")
		if auto_adjust:
			new_opt_line = new_opt_line.lstrip()
			indent = self._get_indent_from_line(first_line, default)
			new_opt_line = indent + new_opt_line
		self[first_pos] = new_opt_line
	def insert_option(self, index, opline):
		"""Just insert a stanza operational line at given index, or
		raise an exception if:
		
		- this is not an operational line or this is a starter line
		- there is no starter line in this stanza
		- this operational line would be inserted before the starter
		
		"""
		if not IS_XOPTION(opline):
			raise Error("'%s' is not an option line" % opline.rstrip())
		(state, (first_pos, first_line),
		        (last_pos, last_line),
			(pos, line)) \
		    = self._common_iterate_to(lazy_(self)[:index])
		if state != 1:
			raise Error("Trying to insert a simple operational line '%s' in a stanza before any starter line" % opline.rstrip())
		self.insert(self, index, opline)
	def insert_eo_options(self, opline):
		if STANZA_START.match(opline) or STANZA_COMMENT.match(opline):
			raise Error("'%s' is not an option line" % opline.rstrip())
		(state, (first_pos, first_line),
		        (last_pos, last_line),
			(pos, line)) \
		    = self._common_iterate_to(
		            self,
			    None,
		            lambda pos, line, state, data: IS_OP(line))
		if state != 1:
			raise Error("Can not insert option line if no starter")
		if last_pos < 0:
			raise EmptyStanza([])
		self.insert(self, last_pos + 1, opline)

class SplittedStanzas:
	"""This class lets parse "/etc/network/interfaces" and "interfaces" like
	files in a generic way, in order to get an internal representation of
	stanzas included the target file.
	
	A file can be saved back from the internal representation with or
	without modification, with comments and indentation preserved.
	
	Comments are embedded into stanza objects they are in or after. An
	internal stanza containing only comments at the beginning of the file
	is created if needed. This includes lines starting with `#' and blank
	lines.
	
	Anyway this behavior could change in the future, especially if it's
	believed interesting to be smarter and to try to "detect" comments that
	are before a stanza but related to it, for example thanks to blank
	lines. What will remain guaranteed is that all needed information to
	save back the stanza based file without any loss will be preserved
	(including lines ordering)
	
	If the file is empty there will be no stanza.
	
	"""
	def __init__(self, iteralines, stanza_class = list):
		"""iteralines - an iterable object, traversing lines of the file
		             content to be loaded
		stanza_class - a class exposing a list like interface, whose
		               instance will be used in a list to create the
			       internal representation of the file stanzas """
		self.stanza_list = []
		for line in iteralines:
			if STANZA_START.match(line):
				self.stanza_list.append(stanza_class([line]))
			else: # comment or stanza content
				# does the file starts with comments ?
				if len(self.stanza_list) == 0:
					# yes: create a stanza just for them
					self.stanza_list.append(stanza_class())
				self.stanza_list[-1].append(line)

class NetworkInterfacesStanza(Stanza):
	"""This class adds methods specific to "/etc/network/interfaces"
	content.

	Basic Stanza behavior is left unchanged anyway, but additional methods
	are provided.
	
	"""
	UNKNOWN_STANZA = 0	# comments only / unrecognized stanzas / not yet parsed stanzas
	STANZA_ALLOW = 1	# auto or allow-something lines (auto is a shortcut for allow-auto)
	STANZA_IFACE = 2	# interface blocks
	PARSED_ATTRIBUTES = (
		'allow_list', 'allow_type',		# allow / auto stanzas
		'iface_name', 'iface_family', 'iface_method', # iface stanzas
		'options_list', 'options_dict', 'options_multicnt',
	)
	def __init__(self, lst):
		"""Will construct a regular Stanza instance and do additional work
		to understand the contents of 'iface' and 'allow-<blah>' stanzas
		('auto' stanzas are indeed the same thing as 'allow-auto')"""
		Stanza.__init__(self, lst)
		self.parse()
	def _clear_unparse(self):
		for attr in PARSED_ATTRIBUTES:
			if hasattr(self, attr):
				delattr(self, attr)
	def _parse_allow(self):
		szst = self.get_stanza_starter()
		szst_splitted = szst.strip().split()
		if szst_splitted[0] == 'auto':
			self.allow_type = 'auto'
		elif szst_splitted[0].find('allow-') == 0:
			self.allow_type = szst_splitted[0][len('allow-'):]
		else:
			raise WrongStanzaType([])
		self.allow_list = filter(lambda x: bool(x), lazy_(szst_splitted)[1:])
	def _parse_options(self, oplines):
		self.options_multicnt = {}
		self.options_dict = {}
		self.options_list = []
		for option in oplines:
			opt_name, opt_value = split_pad(option.strip(), 1)
			if opt_name not in self.options_multicnt:
				self.options_multicnt[opt_name] = 0
			self.options_multicnt[opt_name] += 1
			self.options_dict[opt_name] = opt_value
			self.options_list.append((opt_name, opt_value))
	def _split_and_check_iface_starter(self, starter):
		splitted = starter.strip().split(None, 4)
		if len(splitted) != 4:
			raise BadlyFormedStarter([])
		if splitted[0] != 'iface':
			raise WrongStanzaType([])
		return splitted
	def _parse_iface(self):
		oplines = self.iteroplines()
		starter = oplines.next()
		if not STANZA_START.match(starter):
			raise BadlyFormedStarter([])
		splitted = self._split_and_check_iface_starter(starter)
		self.iface_name = splitted[1]
		self.iface_family = splitted[2]
		self.iface_method = splitted[3]
		self._parse_options(oplines)
	def parse(self):
		self.sz_type = UNKNOWN_STANZA
		self._clear_unparse()
		if self.has_payload() and not self.has_valid_payload():
			raise BadlyFormedStanza([])
		try:
			szst = self.get_stanza_starter()
			if szst and self.has_valid_payload():
				szst_splitted = szst.strip().split(None, 1)
				if szst_splitted[0] == 'auto' \
				   or szst_splitted[0].find("allow-") == 0: 
					self._parse_allow()
					self.sz_type = STANZA_ALLOW
				elif szst_splitted[0] == 'iface':
					self._parse_iface()
					self.sz_type = STANZA_IFACE
		except:
			self.sz_type = UNKNOWN_STANZA
			self._clear_unparse()
			raise
	def get_type(self):
		return self.sz_type
	def get_allow_desc(self):
		"""Returns a tuple of allow-type (auto or something else) and
		the corresponding list of interfaces. This will just raise an
		exception if the instance is indeed not an 'allow' stanza.
		Example if the line
		    allow-hotplug eth0 eth1
		has been processed for the current stanza:
		
		>>> stanza.get_allow_desc()
		('hotplug', ['eth0', 'eth1'])
		
		"""
		return (self.allow_type, self.allow_list)
	def get_iface_desc(self):
		return (self.iface_name, self.iface_family, self.iface_method)
	def get_iface_name(self):
		return self.iface_name
	def get_iface_family(self):
		return self.iface_family
	def get_iface_method(self):
		return self.iface_method
	def get_iface_attr(self, attr):
		return {'name': self.iface_name,
			'family': self.iface_family,
			'method': self.iface_method}[attr]
	def get_options_list(self):
		return self.options_list
	def get_options_dict(self):
		return self.options_dict
	def get_options_multicnt(self):
		return self.options_multicnt
	def get_options_pack(self):
		return (self.options_list, self.options_dict, self.options_multicnt)
	def iteroptions(self, k=None):
		if k is None:
			return iter(self.options_list)
		else:
			return (opt for opt in self.options_list if opt[0] == k)
	def _option_finder(self, pos, line, state, opt_name):
		return state and split_pad(line.strip(), 1)[0] == opt_name
	def set_or_change_opt(self, opt_name, opt_value):
		if opt_name not in options_multicnt:
			options_multicnt[opt_name] = 1
			options_dict[opt_name] = opt_value
			options_list.append((opt_name, opt_value))
			self.insert_eo_options(
				self.get_opline_indent('\t')
				+ unsplit_none((opt_name, opt_value), ' '))
			return
		if options_multicnt[opt_name] > 1:
			raise OptionSetError(opt_name, opt_value)
		options_dict[opt_name] = opt_value
		p = -1
		for p, (k, v) in enumerate(options_list):
			if k == opt_name: break
		if p == -1:
			raise OptionSetError(opt_name, opt_value)
		options_list[p] = (opt_name, opt_value)
		self.find_replace_option(
			self._option_finder,
			unsplit_none((opt_name, opt_value), ' '),
			opt_name)
	def set_iface_attr(self, attr, value):
		attrs = ('name', 'family', 'method')
		if attr not in attrs:
			raise NameError, \
				"name '%s' is not defined" % attr
		setattr(self, 'iface_' + attr, value)
		p_line = self.get_stanza_starter_line()
		if p_line is None:
			raise EmptyStanza([])
		p, line = p_line
		self._split_and_check_iface_starter(line)
		splitted = ['iface']
		splitted.extend(getattr(self, 'iface_' + name) for name in attrs)
		self[p] = ' '.join(splitted)

class NetworkInterfaces(SplittedStanzas):
	"""This class lets parse "/etc/network/interfaces" and understand it in
	greater details than SplittedStanzas. It adds additional methods.
	
	"""
	def __init__(self, iteralines, stanza_class = NetworkInterfacesStanza):
		"""Arguments: see constructor of SplittedStanzas.
		
		stanza_class must provide at least the interface of 
		NetworkInterfacesStanza."""
		SplittedStanzas.__init__(self, iteralines, stanza_class)
	def parse(self):
		for stanza in self.stanza_list:
			stanza.parse()
	def _no_redundant_ifaces_check(self):
		multiples = {}
		for stanza in self.iteriface():
			iface_name = stanza.get_iface_desc()[0]
			if iface_name not in multiples:
				multiples[iface_name] = 1
			else:
				multiples[iface_name] += 1
		filtered_multiples = dict((k,v) for k,v in multiples.iteritems() if v > 1)
		if filtered_multiples:
			raise RedundantIfaces(filtered_multiples)
	def sanity_check(self):
		self._no_redundant_ifaces_check()
	def iterallow(self, what=None):
		return (stanza for stanza in self.stanza_list
		        if stanza.get_type() == STANZA_ALLOW
			and (what is None
			     or stanza.get_allow_desc()[0] == what))
	def iteriface(self):
		return (stanza for stanza in self.stanza_list
		        if stanza.get_type() == STANZA_IFACE)
	def get_allow_list(self, what='auto'):
		"""Returns the list of interfaces to be automatically brought up
		when "ifup" is run with the "-a" option (like at system startup)
		if the parameter "what" contains 'auto' (default value), or when
		"ifup" is run with a "--allow BLAH" option if the parameter
		"what" contains 'BLAH'

		No filtering is performed on interface names, and if the same
		one is encountered multiple times then multiple copies will be
		present in the returned list.
		
		An empty list will be returned if there is no interface listed
		in the corresponding "allow-" stanzas, or no corresponding
		"allow-" stanzas at all."""
		return flatten_list(stanza.get_allow_desc()[1]
		                    for stanza in self.iterallow(what))
	def get_all_allow_lists(self):
		"""Returns a list of consolidated (name, <interfaces_list>)
		tuples where name contains an 'allow-' stanza qualifier and
		<interfaces_list> is a list of interfaces for this 'allow-'
		stanza. There won't be the same name multiple times, but for
		a given name there will be a given interface multiple times
		in its associated list if the configuration file contains such
		multiple interfaces in the considered 'allow-' stanza. """
		allow_type_list = []
		allow_list_dict = {}
		for stanza in self.iterallow():
			allow_desc = stanza.get_allow_desc()
			if allow_desc[0] not in allow_list_dict:
				allow_type_list.append(allow_desc[0])
				allow_list_dict[allow_desc[0]] = []
			allow_list_dict[allow_desc[0]].extend(allow_desc[1])
		return [(name, allow_list_dict[name]) for name in allow_type_list]
