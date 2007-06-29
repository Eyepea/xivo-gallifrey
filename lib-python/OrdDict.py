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

from itertools import tee

missing = object()
class ordDict(dict):
	__slots__ = ('_seq',)
	def __init__(self, seq = missing):
		if seq is not missing:
			seq1,seq2 = tee(seq)
			self._seq = [k for (k,v) in seq1]
			super(ordDict, self).__init__(seq2)
		else:
			self._seq = []
			super(ordDict, self).__init__()
	def __delitem__(self, k):
		super(ordDict, self).__delitem__(k)
		self._seq.remove(k)
	def __iter__(self):
		return iter(self._seq)
	def __setitem__(self, k, v):
		plen = len(self)
		super(ordDict, self).__setitem__(k,v)
		if len(self) != plen:
			self._seq.append(k)
	def clear(self):
		super(ordDict, self).clear()
		self._seq = []
	def copy(self):
		return ordDict(self.iteritems())
	def items(self):
		return [(k,self[k]) for k in self._seq]
	def iteritems(self):
		return ((k,self[k]) for k in self._seq)
	def iterkeys(self):
		return iter(self._seq)
	def itervalues(self):
		return (self[k] for k in self._seq)
	def keys(self):
		return self._seq[:]
	def pop(self, *args):
		r = super(ordDict, self).pop(*args)
		try:
			self._seq.remove(args[0])
		except ValueError:
			pass # any arror already catched by super dict
		return r
	def popitem(self):
		(k,v) = super(ordDict, self).popitem()
		self._seq.remove(k)
		return (k,v)
	def setdefault(self,k,d=None):
		try:
			return self[k]
		except KeyError:
			super(ordDict, self).__setitem__(k,d)
			self._seq.append(k)
			return d
	def update(self, E=missing, **F):
		if isinstance(E, dict):
			for k,v in E.iteritems():
				self[k] = v
		elif E is not missing:
			for k,v in E:
				self[k] = v
		for k,v in F.iteritems():
			self[k] = v
	def values(self):
		return [self[k] for k in self._seq]

__all__ = ['ordDict']
