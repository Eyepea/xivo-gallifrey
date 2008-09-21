"""Inefficient ordered dictionaries: deletion is O(n) - only use for small stuffs

Copyright (C) 2007, 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

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

from xivo.ROListProxy import ROListProxy

# TODO: unit tests

class OrdDict(dict):
    """
    Inefficient ordered dictionaries
    WARNING: deletion is O(n)
    """
    __slots__ = ('__seq', '__seq_ro')
    def __init__(self, seq=None):
        super(OrdDict, self).__init__()
        self.__seq = []
        self.__seq_ro = ROListProxy(self.__seq)
        if seq:
            for k, v in seq:
                self[k] = v
    def __delitem__(self, k):
        """x.__delitem__(y) <==> del x[y]"""
        dict.__delitem__(self, k)
        self.__seq.remove(k)
    def __iter__(self):
        """x.__iter__() <==> iter(x)"""
        return iter(self.__seq)
    def __setitem__(self, k, v):
        """x.__setitem__(i, y) <==> x[i]=y"""
        plen = len(self)
        dict.__setitem__(self, k, v)
        if len(self) != plen:
            self.__seq.append(k)
    def __repr__(self):
        """x.__repr__() <==> repr(x)"""
        return "OrdDict(%s)" % `self.items()`
    def clear(self):
        """D.clear() -> None.  Remove all items from D."""
        dict.clear(self)
        del self.__seq[:]
    def copy(self):
        """D.copy() -> a shallow copy of D"""
        return OrdDict(self.iteritems())
    def items(self):
        """D.items() -> list of D's (key, value) pairs, as 2-tuples"""
        return [(k, self[k]) for k in self.__seq]
    def iteritems(self):
        """D.iteritems() -> an iterator over the (key, value) items of D"""
        return ((k, self[k]) for k in self.__seq)
    def iterkeys(self):
        """D.iterkeys() -> an iterator over the keys of D"""
        return iter(self.__seq)
    def itervalues(self):
        """D.itervalues() -> an iterator over the values of D"""
        return (self[k] for k in self.__seq)
    def keys(self):
        """D.keys() -> list of D's keys"""
        return self.__seq[:]
    def pop(self, key, *more):
        """D.pop(k[,d]) -> v, remove specified key and return the corresponding value
        If key is not found, d is returned if given, otherwise KeyError is raised"""
        rm_from_list = key in self
        r = dict.pop(self, key, *more)
        if rm_from_list:
            self.__seq.remove(key)
        return r
    def popitem(self):
        """D.popitem() -> (k, v), remove and return some (key, value) pair as a
        2-tuple; but raise KeyError if D is empty"""
        try:
            k = self.__seq.pop()
        except IndexError:
            raise KeyError, 'popitem(): dictionary is empty'
        v = dict.pop(self, k)
        return (k, v)
    def setdefault(self, k, d=None):
        """D.setdefault(k[,d]) -> D.get(k,d), also set D[k]=d if k not in D"""
        try:
            return self[k]
        except KeyError:
            self[k] = d
            return d
    def update(self, *E, **F):
        """D.update(U, **F) -> None.  Update D from U and F: for k in U: D[k] = U[k]
        (if U has keys else: for (k, v) in U: D[k] = v) then: for k in F: D[k] = F[k]"""
        if len(E) > 1:
            raise TypeError, 'update expected at most 1 arguments, got %d' % len(E)
        if E:
            if hasattr(E[0], 'iteritems'):
                for k, v in E[0].iteritems():
                    self[k] = v
            else:
                for k, v in E[0]:
                    self[k] = v
        for k, v in F.iteritems():
            self[k] = v
    def values(self):
        """D.values() -> list of D's values"""
        return [self[k] for k in self.__seq]
    def reverse(self):
        """D.reverse() -- reverse *IN PLACE*"""
        self.__seq.reverse()
    def sort(self, *E, **F):
        """L.sort(cmp=None, key=None, reverse=False) -- stable sort *IN PLACE*;"""
        self.__seq.sort(*E, **F)
    _seq = property(operator.attrgetter('_OrdDict__seq_ro'))

__all__ = ['OrdDict']
