"""Inefficient ordered dictionaries: deletion is O(n) - only use for small stuffs

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

from ROListProxy import *
from pyfunc import *

def repr_dict_seqseq(dct, key_seq):
	first = True
	for k in iter(key_seq):
		if not first:
			yield (', ',)
		yield (repr(k), ': ', repr(dct[k]))
		first = False

# XXX: unit tests

missing = object()
class ordDict(dict):
	__slots__ = ('__seq', '__seq_ro')
	def __init__(self, seq = []):
		super(ordDict, self).__init__()
		self.__seq = []
		self.__seq_ro = ROListProxy(self.__seq)
		for k,v in seq:
			self[k] = v
	def __delitem__(self, k):
		dict.__delitem__(self, k)
		self.__seq.remove(k)
	def __iter__(self):
		return iter(self.__seq)
	def __setitem__(self, k, v):
		plen = len(self)
		dict.__setitem__(self,k,v)
		if len(self) != plen:
			self.__seq.append(k)
	def __repr__(self):
		# Maybe representation should be differentiated from plain 
		# builtin non ordered dictionaries
		return ''.join(chain('{', flatten_seq(repr_dict_seqseq(self, self.__seq)), '}'))
	def clear(self):
		dict.clear(self)
		del self.__seq[:]
	def copy(self):
		return ordDict(self.iteritems())
	def items(self):
		return [(k,self[k]) for k in self.__seq]
	def iteritems(self):
		return ((k,self[k]) for k in self.__seq)
	def iterkeys(self):
		return iter(self.__seq)
	def itervalues(self):
		return (self[k] for k in self.__seq)
	def keys(self):
		return self.__seq[:]
	def pop(self, key, *more):
		rm_from_list = key in self
		r = dict.pop(self, key, *more)
		if rm_from_list:
			self.__seq.remove(key)
		return r
	def popitem(self):
		try:
			k = self.__seq.pop()
		except IndexError:
			raise KeyError, 'popitem(): dictionary is empty'
		v = dict.pop(self, k)
		return (k,v)
	def setdefault(self,k,d=None):
		try:
			return self[k]
		except KeyError:
			self[k] = d
			return d
	def update(self, *E, **F):
		if len(E) > 1:
			raise TypeError, 'update expected at most 1 arguments, got %d' % len(E)
		if E:
			try:
				for k,v in E[0].iteritems():
					self[k] = v
			except AttributeError:
				for k,v in E[0]:
					self[k] = v
		for k,v in F.iteritems():
			self[k] = v
	def values(self):
		return [self[k] for k in self.__seq]
	def reverse(self):
		self.__seq.reverse()
	def sort(self):
		self.__seq.sort()
	_seq = property(operator.attrgetter('_ordDict__seq_ro'))

__all__ = ['ordDict']
