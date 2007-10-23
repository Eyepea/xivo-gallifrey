"""Dictionary wrapper that exposes an attribute interface

Copyright (C) 2007, Proformatique

Simply assigning __dict__ won't work because Python internally access the
base __builtin__.dict instance, so if this approach was chosen any behavioral
differences from __builtin__.dict would be masked. So the Python mapping to be
used is just stored in a special attribute.

Some additional attributes whose name start with '_attr' and that are not
related to generic attribute manipulation are used to provides specific
functionalities.

Other attributes starting by '_' are handled in a special way: first the
initial '_' is stripped, then the resulting attribute name is used to directly
access the one of the underlying dictionary. This approach has been chosen
because:

a) disabling the attribute/mapping bijection for this additional class of keys
doesn't really matters, as keys that are not string or contain special
characters or begin with a digit are already not directly usable as attribute
name anyway, and symbols starting with a '_' are reserved in lots of contexts

b) it would highly reduce the very interest of AttrDict to only be able to
access standard dictionary methods by directly dereferencing to the underlying
dictionary each time this is needed, as the goal is to simplify the notation
used when manipulating medium to deep trees when node names are statically
known in advance

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

import operator

missing = object()
class AttrDict(object):
	"""WARNING: no call to super(AttrDict, self).__init__ in
	AttrDict.__init__ """
	__slots__ = ('__attr_dict',)
	def __init__(self, dct):
		object.__setattr__(self, '_AttrDict__attr_dict', dct)
	def __repr__(self):
		return ''.join(('AttrDict(', repr(self.__attr_dict), ')'))
	def __lt__(self, y):
		return self.__attr_dict < y
	def __le__(self, y):
		return self.__attr_dict <= y
	def __eq__(self, y):
		return self.__attr_dict == y
	def __ne__(self, y):
		return self.__attr_dict != y
	def __gt__(self, y):
		return self.__attr_dict > y
	def __ge__(self, y):
		return self.__attr_dict >= y
	def __cmp__(self, y):
		return cmp(self.__attr_dict, y)
	def __hash__(self):
		return hash(self.__attr_dict)
	def __getattr__(self, attr, default=missing):
		if '_' != attr[0]:
			if default is missing:
				return self.__attr_dict[attr]
			else:
				return self.__attr_dict.get(attr, default)
		elif '_attr' == attr[0:5]:
			raise AttributeError, "%s instance has no attribute %s" %(type(self).__name__, repr(attr))
		else:
			if default is missing:
				return getattr(self.__attr_dict, attr[1:])
			else:
				return getattr(self.__attr_dict, attr[1:], default)
	def __setattr__(self, attr, value):
		if '_' != attr[0]:
			self.__attr_dict[attr] = value
		elif '_attr' == attr[0:5]:
			object.__setattr__(self, '_AttrDict_'+attr, value)
		else:
			setattr(self.__attr_dict, attr[1:], value)
	def __delattr__(self, attr):
		if '_' != attr[0]:
			del self.__attr_dict[attr]
		elif '_attr' == attr[0:5]:
			raise AttributeError, "no attribute %s or can't delete it in an %s instance" %(repr(attr), type(self).__name__)
		else:
			delattr(self.__attr_dict, attr[1:])
	def __len__(self):
		return len(self.__attr_dict)
	def __getitem__(self, k):
		return self.__attr_dict[k]
	def __setitem__(self, k, v):
		self.__attr_dict[k] = v
	def __delitem__(self, k):
		del self.__attr_dict[k]
	def __iter__(self):
		return iter(self.__attr_dict)
	def __contains__(self, k):
		return k in self.__attr_dict
	_attr_dict = property(operator.attrgetter('_AttrDict__attr_dict'))

__all__ = ['AttrDict']
