"""A list on which we can take references on elements in a position independant way

Copyright (C) 2008-2010  Proformatique

WARNING: The algorithmic complexity caracteristics of this module are not very
good.  Anyway we rely as most as possible on native Python objects and try to
keep our own layer thin, so that the constant factor is kept small.
Only for use with reasonably sized datasets.
"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008-2010  Proformatique

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

from operator import attrgetter as _attrgetter
from itertools import imap as _imap, izip as _izip
from sys import maxint as _maxint


def _unsigned_word(w):
    """
    Return the value of w reinterpreted as an unsigned word having the same
    bit-level representation in a word machine.
    """
    if w >= 0:
        return w
    else:
        return w + (_maxint + 1) * 2


class _Ref(object):
    """
    A reference to an element of a ListRef list.
    """
    
    __slots__ = ('value', '_parent')
    
    def __init__(self, value, parent):
        self.value = value
        self._parent = parent
    
    def __repr__(self):
        return "<elt.ref. 0x%08x %r>" % (_unsigned_word(id(self)), self.value)
    
    parent = property(_attrgetter('_parent'))


class ListRef(object):
    """
    A list on which we can take references on elements in a position
    independant way
    """
    
    __slots__ = ('_reflist',)
    
    def __init__(self, sequence=()):
        self._reflist = [_Ref(el, self) for el in sequence]
    
    def __add__(self, other):
        "x.__add__(other) <==> x+other"
        if (not isinstance(other, ListRef)) and (not isinstance(other, list)):
            raise TypeError, "can only concatenate ListRef or list to ListRef"
        n = ListRef(self)
        n.extend(other)
        return n
    
    def __cmp__(self, other):
        "x.__cmp__(other) <==> cmp(x,other)"
        if not (isinstance(other, list) or isinstance(other, ListRef)):
            return cmp([], other)
        else:
            iter_b = iter(other)
            for a in iter(self):
                try:
                    b = iter_b.next()
                except StopIteration:
                    return 1
                cmp_ab = cmp(a, b)
                if cmp_ab:
                    return cmp_ab
            else:
                try:
                    b = iter_b.next()
                    return -1
                except StopIteration:
                    return 0
    
    def __contains__(self, value):
        "seq.__contains__(value) <==> value in seq"
        return value in _imap(_attrgetter('value'), self._reflist)
    
    def __delitem__(self, k):
        "x.__delitem__(k) <==> del x[k]"
        if isinstance(k, slice):
            for each in self._reflist[k]:
                each._parent = None
            del self._reflist[k]
        else:
            self._reflist.pop(k)._parent = None
    
    def __getitem__(self, k):
        "x.__getitem__(k) <==> x[k]"
        if isinstance(k, slice):
            return ListRef(map(_attrgetter('value'), self._reflist[k]))
        else:
            return self._reflist[k].value
    
    def __hash__(self):
        "x.__hash__() <==> hash(x)"
        raise TypeError, "ListRef objects are unhashable"
    
    def __iadd__(self, other):
        "x.__iadd__(other) <==> x+=other"
        if (not isinstance(other, ListRef)) and (not isinstance(other, list)):
            raise TypeError, "can only concatenate ListRef or list to ListRef"
        self.extend(other)
        return self
    
    def __imul__(self, n):
        "x.__imul__(n) <==> x*=n"
        if not isinstance(n, int):
            raise TypeError, "can't multiply sequence by non-int"
        if n < 1:
            for each in self._reflist:
                each._parent = None
            self._reflist = []
        else:
            neg_old_len = -len(self._reflist)
            for x in xrange(n - 1): # pylint: disable-msg=W0612
                self.extend(map(_attrgetter('value'), self._reflist[neg_old_len:]))
        return self
    
    def __iter__(self):
        "x.__iter__() <==> iter(x)"
        return _imap(_attrgetter('value'), self._reflist)
    
    def __len__(self):
        "x._len__() <==> len(x)"
        return len(self._reflist)
    
    def __mul__(self, n):
        "x.__mul__(n) <==> x*n"
        if not isinstance(n, int):
            raise TypeError, "can't multiply sequence by non-int"
        if n < 1:
            return ListRef()
        else:
            nlst = ListRef()
            for x in xrange(n): # pylint: disable-msg=W0612
                nlst.extend(self)
            return nlst
    
    def __repr__(self):
        "x.__repr__() <==> repr(x)"
        return "ListRef(%r)" % list(self)
    
    def __reversed__(self):
        "L.__reversed__() -- return a reverse iterator over the list"
        return _imap(_attrgetter('value'), reversed(self._reflist))
    
    def __rmul__(self, n):
        "x.__rmul__(n) <==> n*x"
        return self.__mul__(n)
    
    def __setitem__(self, k, rval):
        """
        x.__setitem__(k, rval) <==> x[k]=rval
        """
        if isinstance(k, slice):
            
            start, stop, step = k.indices(len(self._reflist))
            
            if k.step is None: # simple slice
                
                irval = iter(rval)
                injlim = start
                for p in xrange(start, stop):
                    try:
                        el = irval.next()
                        self._reflist[p].value = el
                        injlim = p + 1
                    except StopIteration:
                        for q in xrange(p, stop):
                            self._reflist[q]._parent = None
                        del self._reflist[p:stop]
                        break
                else:
                    inject = [_Ref(el, self) for el in irval]
                    if inject:
                        self._reflist = self._reflist[:injlim] + inject + self._reflist[injlim:]
            
            else: # extended slice
                
                if hasattr(rval, '__len__'):
                    seq = rval
                else:
                    seq = list(rval)
                
                if step > 0:
                    n_elt = max((stop - start + step - 1) / step, 0)
                else:
                    n_elt = max((start - stop - step - 1) / (-step), 0)
                
                if len(seq) != n_elt:
                    raise ValueError, "attempt to assign sequence of size %d to extended slice of size %d" % (len(seq), n_elt)
                
                for p, el in _izip(xrange(start, stop, step), seq):
                    self._reflist[p].value = el
            
        else:
            self._reflist[k].value = rval
    
    def append(self, obj):
        "L.append(obj) -- append obj to end"
        self._reflist.append(_Ref(obj, self))
    
    def count(self, value):
        "L.count(value) -> integer -- return number of occurrences of value"
        return map(_attrgetter('value'), self._reflist).count(value)
    
    def extend(self, iterable):
        "L.extend(iterable) -- extend list by appending elements from the iterable"
        self._reflist.extend([_Ref(el, self) for el in iterable])
    
    def index(self, value, start=0, stop=None):
        "L.index(value, [start, [stop]]) -> integer -- return first index of value"
        if start < 0:
            start += len(self._reflist)
        start = max(0, start)
        
        return map(_attrgetter('value'), self._reflist[start:stop]).index(value) + start
    
    def insert(self, index, obj):
        "L.insert(index, object) -- insert object before index"
        self._reflist.insert(index, _Ref(obj, self))
    
    def pop(self, index=-1):
        "L.pop([index]) -> item -- remove and return item at index (default last)"
        return self._reflist.pop(index).value
    
    def remove(self, value):
        "L.remove(value) -- remove first occurrence of value"
        index = self.index(value)
        del self[index]
    
    def reverse(self):
        "L.reverse() -- reverse *IN PLACE*"
        self._reflist.reverse()
    
    def sort(self, cmp=None, key=None, reverse=False):
        """
        L.sort(cmp=None, key=None, reverse=False) -- stable sort *IN PLACE*;
        cmp(x, y) -> -1, 0, 1
        """
        if key is None:
            mykey = _attrgetter('value')
        else:
            mykey = lambda x: key(x.value)
        
        self._reflist.sort(cmp=cmp, key=mykey, reverse=reverse)
    
    def ref_get(self, index):
        """
        Return the element reference at position index.
        """
        return self._reflist[index]
    
    def ref_index(self, ref):
        """
        Return the position of the reference object ref.
        Raise ValueError if ref is not a reference to an element of this list.
        """
        return self._reflist.index(ref)
