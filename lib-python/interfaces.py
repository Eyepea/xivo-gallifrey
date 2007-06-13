"""/etc/network/interfaces configuration file reader / writer

Copyright (C) 2007, Proformatique

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

import re
from pyfunc import *
from lzslice import *

# Empty lines or lines beginning with a "#" are comments:
STANZA_COMMENT_RE = r'^(\s*|#.*)$'
STANZA_COMMENT = re.compile(STANZA_COMMENT_RE)

# Lines not indented and not comments are where stanzas start:
STANZA_START_RE = r'^[^\s#].*$'
STANZA_START = re.compile(STANZA_START_RE)

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

class Stanza(list):
	"""This class adds very useful "interfaces" like stanza related methods
	to the built-in python lists.
	
	You can pass it (or a derivative) to the constructor of StanzaFormat,
	so that it will split 
	
	"""
	def itercomments(self):
		"Generator to iterate over comments and blank lines."
		return (line for line in self if STANZA_COMMENT.match(line))
	def iteroplines(self):
		"Generator to iterate over operational lines."
		return (line for line in self if not STANZA_COMMENT.match(line))
	def get_stanza_starter(self):
		"""Get the first found stanza starter (there really should be
		one but this can't be guaranteed by this class"""
		return find(STANZA_START.match, self)
	def get_opline_indent(self, default=None):
		"""Get the stanza body indentation according to the one used by
		the first operational non starter line. """
		indent = None
		line = find(lambda l: (not STANZA_COMMENT.match(line)) and \
		                      (not STANZA_START.match(line)), self)
		if line:
			mo = INDENT.match(line)
			if mo:
				indent = mo.group(1)
		if indent is None:
			return default
		else:
			return indent
	def has_payload(self):
		"""Returns True if any line is operational, otherwise
		returns False."""
		return find(lambda x: not STANZA_COMMENT.match(x), self) is not None
	def has_valid_payload(self):
		"""Returns True if exactly one line is a stanza starter and
		every other non comment lines are after that one. """
		return bool(all_and_count(equiv(pos==0, STANZA_START.match(line))
		                          for pos, line in enumerate(self.iteroplines())))
	def has_stanza_starter(self):
		"""Returns True as soon as a valid stanza starter is detected,
		regardless of its position.
		
		"""
		return find(lambda x: STANZA_START.match(x), self) is not None
	def insertcomment(self, index, commentline):
		"""Just insert the comment line at the given index, or raise an
		exception if commentline is indeed not a comment/blank line.

		"""
		if not STANZA_COMMENT.match(commentline):
			raise Error("'%s' is not a comment/blank line" % commentline.rstrip())
		self.insert(index, commentline)
	def insertstarter(self, index, starterline):
		"""Just insert a stanza starter or raise an exception if:
		
		- starterline is indeed not a valid stanza starter
		- there's already one stanza starter in this stanza
		- this starter would be inserted after a non comment line
		  in this stanza
		
		"""
		if not STANZA_START.match(starterline):
			raise Error(
				"'%s' is not a valid stanza starter" % starterline.rstrip())
		if self.has_stanza_starter():
			raise Error(
				"trying to insert the stanza starter '%s' in a stanza where there is already one" % starterline.rstrip())
		if find(lambda x: not STANZA_COMMENT.match(x), lazy_(self)[:index]):
			raise Error(
				"trying to insert the stanza starter '%s' in a stanza after at least one non comment/blank line (first found '%s')" % (starterline.rstrip(), line.rstrip()))
		self.insert(index, starterline)
	def insertopertional(self, index, opline):
		"""Just insert a stanza operational line at given index, or
		raise an exception if:
		
		- this is not an operational line or this is a starter line
		- there is no starter line in this stanza
		- this operational line would be inserted before the starter
		
		"""
		if STANZA_START.match(opline) or STANZA_COMMENT.search(opline):
			raise Error(
				"'%s' is not a simple operational line" % opline.rstrip())
		if not self.has_stanza_starter():
			raise Error(
				"trying to insert a simple operational line '%s' in a stanza where there is no starter line" % opline.rstrip())
		if find(STANZA_START.search, lazy_(self)[index:]):
			raise Error(
				"trying to insert a simple operational line '%s' in a stanza before a stanza starter '%s'" % (opline.rstrip(), line.strip()))
		self.insert(self, index, opline)

class SplittedStanzas:
	"""This class lets parse "/etc/network/interfaces" and "interfaces"
	like files in a generic way, in order to get an internal representation
	of stanzas included the target file.
	
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
				self.stanza_list[len(self.stanza_list)-1].append(line)

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
	)
	def __init__(self, lst):
		"""Will construct a regular Stanza instance and do additional work
		to understand the contents of 'iface' and 'allow-<blah>' stanzas
		('auto' stanzas are indeed the same thing as 'allow-auto')"""
		Stanza.__init__(self, lst)
		self.parse()
	def _clear_unparse(self):
		self.sz_type = UNKNOWN_STANZA
		for attr in PARSED_ATTRIBUTES:
			if hasattr(self, attr):
				delattr(self, attr)
	def parse(self):
		self._clear_unparse()
		szst = self.get_stanza_starter()
		if szst and self.has_payload_valid():
			szst_splitted = re.split(' +', szst.strip(), 1)
			if szst_splitted[0] == 'auto' \
			   or szst_splitted[0].find("allow-") == 0: 
				self.sz_type = STANZA_ALLOW
				self.parse_allow()
			elif szst_splitted[0] == 'iface':
				self.sz_type = STANZA_IFACE
				self.parse_iface()
	def parse_allow(self):
		szst = self.get_stanza_starter()
		szst_splitted = re.split(' +', szst.strip())
		if szst_splitted[0] == 'auto':
			self.allow_type = 'auto'
		elif szst_splitted[0].find('allow-') == 0:
			self.allow_type = szst_splitted[0][len('allow-'):]
		else:
			raise Error(
				"trying to parse an 'auto' stanza, but this is a '%s' one" % szst_splitted[0])
		self.allow_list = filter(lambda x: bool(x), lazy_(szst_splitted)[1:])
	def parse_iface(self):
		pass # TODO
	def get_type(self):
		return self.sz_type
	def get_allow_desc(self):
		"""Returns a tuple of allow-type (auto or something else) and
		the corresponding list of interfaces. This will just raise an
		exception if the instance is indeed not an 'allow' stanza.
		Example:
		
		>>> stanza.get_allow_desc()
		('hotplug', ['eth0', 'eth1'])
		
		"""
		return (self.allow_type, self.allow_list)
	def get_iface_desc(self):
		return (self.iface_name, self.iface_family, self.iface_method)

class NetworkInterfaces(SplittedStanzas):
	"""This class lets parse "/etc/network/interfaces" and understand it in
	greater details than SplittedStanzas. It adds additional methods.
	
	"""
	def __init__(self, iteralines, stanza_class = NetworkInterfacesStanza):
		"""Arguments: see constructor of SplittedStanzas.
		
		stanza_class must provide at least the interface of 
		NetworkInterfacesStanza."""
		SplittedStanzas.__init__(self, iteralines, stanza_class)
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
		tuples where name contains an 'allow-' stanza postfix and
		<interfaces_list> is a list of interfaces for this allow-
		stanza. There won't be the same name multiple times, but for
		a given name there will be a given interface multiple times
		in its associated list if the configuration file contains
		multiple ones.
		"""
		allow_type_list = []
		allow_list_dict = {}
		for stanza in self.iterallow():
			allow_desc = stanza.get_allow_desc()
			if allow_desc[0] not in allow_list_dict:
				allow_type_list.append(allow_desc[0])
				allow_list_dict[allow_desc[0]] = []
			allow_list_dict[allow_desc[0]].extend(allow_desc[1])
		return [(name, allow_list_dict[name]) for name in allow_type_list]
