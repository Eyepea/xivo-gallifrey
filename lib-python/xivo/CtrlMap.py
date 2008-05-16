"""Decorating class to do run time configurable dictionary write access control

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

import operator
from xivo.FuseProperties import *

# Python does not have a class hierarchy for its builtin objects so new style
# classes derivating from 'object' are not compatible with classes derivating
# from 'dict' (for ex.) when it comes to multiple inheritance
#
# This is an absolute mess for us because we can't add filtering capabilities
# by just derivating both our filtering class and underlying implementation
# class without knowing in advance if it's an old style class, a new one
# derivating from 'object', a new one derivating from 'dict', or even a new
# one derivating from <some_other_builtin_object> (would be curious but
# possible and even useful in some rare situations)
#
# So instead of using a derivating class based idiom, the implementation of
# CtrlMap will just keep a reference to an underlying object that presents
# a Python mapping interface.
#
# This has both advantages and disadvantages (Python implementation details
# that leaded us to that approach at the first time put aside), some of them
# follow:
#
# - Advantage: gives the possibility to get one supplementary indirection level
#   without the need of an other object just to do that.
#
# - Disadvantage: you might not want this supplementary indirection and
#   consider it as a risk in context where you would prefer your objects being
#   invariants BUT a fuse attribute can be added so that the object becomes
#   irreversibly invariant when the fuse is burned.
#
# - Disadvantage: The derivation based approach would have given us full speed
#   access to unfiltered methods, the underlying attribute one results in an
#   overhead all the time.
#
# - Disadvantage: probably also more memory consumption and reference counting
#   pressure.

missing = object()

class CtrlMap(object):
	
	__slots__ = ('__allow_insert', '__allow_modify', '__allow_delete',
	             '__underlying', '__fuse_underl', '__fuse_allow',
		     '__fuse_ALL')
	
	def __init__(self, underlying,
	             allow_insert = True, allow_modify = True, allow_delete = True,
		     fuse_underl = False, fuse_allow = False):
		self.__underlying = underlying
		self.__allow_insert = bool(allow_insert)
		self.__allow_modify = bool(allow_modify)
		self.__allow_delete = bool(allow_delete)
		self.__fuse_underl = bool(fuse_underl)
		self.__fuse_allow = bool(fuse_allow)
		self.__fuse_ALL = False
	
	############## LOCAL IMPLEMENTATION ##############
	
	def __repr__(self):
		"x.__repr__() <==> repr(x)"
		return ''.join(('<CtrlMap of ', repr(self.__underlying),'>'))
	
	################ FILTERED METHODS ################
	
	def __setitem__(self, k, v):
		"""x.__setitem__(i, y) <==> x[i]=y
		Perm.: x.allow_modify if i was previously in x, else x.allow_insert
		"""
		if k in self.__underlying:
			if self.__allow_modify:
				self.__underlying[k] = v
			else:
				raise ValueError, "modification not allowed (key %s)" %repr(k)
		else:
			if self.__allow_insert:
				self.__underlying[k] = v
			else:
				raise ValueError, "insertion not allowed (key %s)" %repr(k)
	
	def __delitem__(self, k):
		"""x.__delitem__(y) <==> del x[y]
		Perm.: x.allow_delete
		"""
		if self.__allow_delete:
			del self.__underlying[k]
		else:
			raise ValueError, "deletion not allowed (key %s)" %repr(k)
	
	def clear(self):
		"""D.clear() -> None.  Remove all items from D.
		Perm.: D.allow_delete
		"""
		if self.__allow_delete:
			self.__underlying.clear()
		else:
			raise ValueError, "deletion not allowed"
	
	def copy(self):
		"""D.copy() -> a shallow copy of D
		
		The created copy will unconditionally be authorized for
		creation/insertion/deletion as the holder of the copy can do
		whatever it wants with it without disturbing the original
		object. Also no fuse will be burned in the copy.
		"""
		return type(self)(self.__underlying.copy())
	
	def pop(self, k, d=missing):
		"""D.pop(k[,d]) -> v, remove specified key and return the corresponding value
		If key is not found, d is returned if given, otherwise KeyError is raised
		Perm.: D.allow_delete
		"""
		if self.__allow_delete:
			if d is missing:
				return self.__underlying.pop(k)
			else:
				return self.__underlying.pop(k, d)
		else:
			raise ValueError, "deletion not allowed (key %s)" %repr(k)
	
	def popitem(self):
		"""D.popitem() -> (k, v), remove and return some (key, value) pair as a
		2-tuple; but raise KeyError if D is empty
		Perm.: D.allow_delete
		"""
		if self.__allow_delete:
			return self.__underlying.popitem()
		else:
			raise ValueError, "deletion not allowed"
	
	def setdefault(self, k, d=None):
		"""D.setdefault(k,[d]) -> D.get(k,d), also set D[k]=d if k not in D
		Perm.: D.allow_insert
		"""
		if self.__allow_insert:
			return self.__underlying.setdefault(k, d)
		else:
			raise ValueError, "insertion not allowed (key %s)" %repr(k)
	
	def update(self, *E, **F):
		"""This function force canonical behavior, only using
		self.__setitem__() so control can take place there.
		
		This choice has been made because it is believe that most of
		the time update() is called just as an optimization of the
		corresponding iterative looping way of inserting / modifying
		items, so this time I did not want to uncondionally check both
		__allow_insert and __allow_modify to just allow update() to do
		something.
		
		The main drawback is that an exception that would be raised in
		the middle of update() would leave the underlying mapping with
		some items updated and some not, even if a corresponding
		'map[k] = d' statement the latter would have succeed.
		"""
		if len(E) > 1:
			raise TypeError, 'update expected at most 1 arguments, got %d' % len(E)
		if E:
			if hasattr(E[0], 'iteritems'):
				for k,v in E[0].iteritems():
					self[k] = v
			else:
				for k,v in E[0]:
					self[k] = v
		for k,v in F.iteritems():
			self[k] = v
	
	############### SIMPLE PASSTHROUGH ###############
	
	def __getitem__(self, k):
		"x.__getitem__(y) <==> x[y]"
		return self.__underlying[k]
	
	def __cmp__(self, y):
		"x.__cmp__(y) <==> cmp(x,y)"
		return cmp(self.__underlying, y)
	
	def __eq__(self, y):
		"x.__eq__(y) <==> x==y"
		return self.__underlying == y
	
	def __ge__(self, y):
		"x.__ge__(y) <==> x>=y"
		return self.__underlying >= y
	
	def __gt__(self, y):
		"x.__gt__(y) <==> x>y"
		return self.__underlying > y
	
	def __le__(self, y):
		"x.__le__(y) <==> x<=y"
		return self.__underlying <= y
	
	def __lt__(self, y):
		"x.__lt__(y) <==> x<y"
		return self.__underlying < y
	
	def __ne__(self, y):
		"x.__ne__(y) <==> x!=y"
		return self.__underlying != y
	
	def __iter__(self):
		"x.__iter__() <==> iter(x)"
		return iter(self.__underlying)
	
	def __contains__(self, k):
		"D.__contains__(k) -> True if D has a key k, else False"
		return k in self.__underlying
	
	def __hash__(self):
		"x.__hash__() <==> hash(x)"
		return hash(self.__underlying)
	
	def __len__(self):
		"x.__len__() <==> len(x)"
		return len(self.__underlying)
	
	def get(self, k, d=None):
		"D.get(k[,d]) -> D[k] if k in D, else d.  d defaults to None."
		return self.__underlying.get(k,d)
	
	def has_key(self, k):
		"D.has_key(k) -> True if D has a key k, else False"
		return self.__underlying.has_key(k)
	
	def items(self):
		"D.items() -> list of D's (key, value) pairs, as 2-tuples"
		return self.__underlying.items()
	
	def iteritems(self):
		"D.iteritems() -> an iterator over the (key, value) items of D"
		return self.__underlying.iteritems()
	
	def iterkeys(self):
		"D.iterkeys() -> an iterator over the keys of D"
		return self.__underlying.iterkeys()
	
	def itervalues(self):
		"D.itervalues() -> an iterator over the values of D"
		return self.__underlying.itervalues()
	
	def keys(self):
		"D.keys() -> list of D's keys"
		return self.__underlying.keys()
	
	def values(self):
		"D.values() -> list of D's values"
		return self.__underlying.values()
	
	############### SPECIAL ATTRIBUTES ###############
	
	underlying = prop_fused_access('_CtrlMap__underlying', '_CtrlMap__fuse_underl')
	allow_insert = prop_fused_bool('_CtrlMap__allow_insert', '_CtrlMap__fuse_allow')
	allow_modify = prop_fused_bool('_CtrlMap__allow_modify', '_CtrlMap__fuse_allow')
	allow_delete = prop_fused_bool('_CtrlMap__allow_delete', '_CtrlMap__fuse_allow')
	fuse_underl = prop_fuse('_CtrlMap__fuse_underl')
	fuse_allow = prop_fuse('_CtrlMap__fuse_allow')
	fuse_ALL = prop_fuse_multiple('_CtrlMap__fuse_ALL',
	                              ('fuse_underl', 'fuse_allow'))

__all__ = ['CtrlMap']
