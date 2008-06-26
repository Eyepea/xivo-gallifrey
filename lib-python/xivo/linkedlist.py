"""Linked list

Copyright (C) 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008  Proformatique

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

from operator import itemgetter
from itertools import izip

class LinkedList(object):
    """
    A list like class that rely on a linked list data structure.
    """
    
    __slots__ = ('__base', '__len')
    
    def __init__(self, sequence=None):
        self.__base = [None, None, None, True, None]
        self.__base[1:3] = self.__base, self.__base
        self.__len = 0
        if sequence is not None:
            self.extend(sequence)
    
    def __iter_scan(self):
        """
        Generate each link of the linked list, from the first to the last one.
        """
        scan = self.__base[2]
        while len(scan) != 5:
            yield scan
            scan = scan[2]
    
    def __get_scan_positive(self, pos):
        """
        Return the link which is at absolute position @pos.
        """
        # TODO: optimize by scanning in reverse if pos near the end
        scan = self.__base[2]
        for x in xrange(pos):
            scan = scan[2]
        
        return scan
    
    def __get_scan_from_pos(self, pos, exc_str):
        """
        Return the link which is at position @pos.
        @pos can be a negative integer, in which case it represents a relative
        position from an element past the last one.
        """
        if not isinstance(pos, int):
            raise TypeError, "list indices must be integers"
        if pos < 0:
            pos += self.__len
        if not (0 <= pos < self.__len):
            raise IndexError, exc_str
        
        return self.__get_scan_positive(pos)
    
    def __remove_scan(self, scan):
        """
        Remove the link @scan from the linked list.
        """
        scan[2][1] = scan[1]
        scan[1][2] = scan[2]
        scan[3] = False
        self.__len -= 1
    
    def __add__(self, other):
        "x.__add__(y) <==> x+y"
        if (not isinstance(other, LinkedList)) and (not isinstance(other, list)):
            raise TypeError, "can only concatenate LinkedList or list to LinkedList"
        n = LinkedList(self)
        n.extend(other)
        return n
    
    def __cmp__(self, other):
        "x.__cmp__(y) <==> cmp(x,y)"
        if not (isinstance(other, list) or isinstance(other, LinkedList)):
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
        "x.__contains__(y) <==> y in x"
        for v in self:
            if v == value:
                return True
        else:
            return False
    
    def __islice_scan(self, slc):
        """
        Generate the links that are at positions represented by the slice
        object @slc.
        """
        start, stop, step = slc.indices(self.__len)
        scan = self.__get_scan_positive(start)
        abs_step = abs(step)
        nxt = (step > 0) + 1
        for pos in xrange(start, stop, step):
            yield scan
            for x in xrange(abs_step):
                scan = scan[nxt]
    
    def __delitem__(self, key):
        "x.__delitem__(y) <==> del x[y]"
        if isinstance(key, slice):
            for scan in self.__islice_scan(key):
                self.__remove_scan(scan)
        else:
            self.__remove_scan(self.__get_scan_from_pos(key, "list assignment index out of range"))
    
    def __getitem__(self, key):
        "x.__getitem__(y) <==> x[y]"
        if isinstance(key, slice):
            return LinkedList(scan[0] for scan in self.__islice_scan(key))
        else:
            return self.__get_scan_from_pos(key, "list index out of range")[0]
    
    def __hash__(self):
        "x.__hash__() <==> hash(x)"
        raise TypeError, "LinkedList objects are unhashable"
    
    def __iadd__(self, other):
        "x.__iadd__(y) <==> x+=y"
        if (not isinstance(other, LinkedList)) and (not isinstance(other, list)):
            raise TypeError, "can only concatenate LinkedList or list to LinkedList"
        self.extend(other)
        return self
    
    def __imul__(self, factor):
        "x.__imul__(y) <==> x*=y"
        if not isinstance(factor, int):
            raise TypeError, "can't multiply sequence by non-int"
        if factor < 1:
            self.__base[1:3] = self.__base, self.__base
            self.__len = 0
        else:
            iter_self = iter(self)
            for x in xrange((factor - 1) * self.__len):
                self.append(iter_self.next())
        return self
    
    def __iter__(self):
        "x.__iter__() <==> iter(x)"
        scan = self.__base[2]
        while len(scan) != 5:
            if scan[3]:
                yield scan[0]
            scan = scan[2]
    
    def __len__(self):
        "x.__len__() <==> len(x)"
        return self.__len
    
    def __mul__(self, factor):
        "x.__mul__(n) <==> x*n"
        if not isinstance(factor, int):
            raise TypeError, "can't multiply sequence by non-int"
        if factor < 1:
            return LinkedList()
        else:
            n = LinkedList()
            for x in xrange(factor):
                n.extend(self)
            return n
    
    def __repr__(self):
        "x.__repr__() <==> repr(x)"
        return "LinkedList(%r)" % list(self)
    
    def __reversed__(self):
        "L.__reversed__() -- return a reverse iterator over the list"
        scan = self.__base[1]
        while len(scan) != 5:
            if scan[3]:
                yield scan[0]
            scan = scan[1]
    
    def __rmul__(self, factor):
        "x.__rmul__(n) <==> n*x"
        return self.__mul__(factor)
    
    def __setitem__(self, key, y):
        "x.__setitem__(i, y) <==> x[i]=y"
        if isinstance(key, slice):
            start, stop, step = key.indices(self.__len)
            if key.step is None: # simple slice
                iter_y = iter(y)
                scan = self.__get_scan_positive(start)
                for x in xrange(start, stop):
                    try:
                        elt = iter_y.next()
                    except StopIteration:
                        for y in xrange(x, stop):
                            self.__remove_scan(scan)
                            scan = scan[2]
                        break
                    scan[0] = elt
                    scan = scan[2]
                else:
                    for elt in iter_y:
                        link = [elt, scan, scan[2], True]
                        scan[2][1] = link
                        scan[2] = link
                        self.__len += 1
                        scan = link
            else: # extended slice
                lst = list(y)
                if step > 0:
                    n_elt = max((stop - start + step - 1) / step, 0)
                else:
                    n_elt = max((start - stop - step - 1) / (-step), 0)
                if len(lst) != n_elt:
                    raise ValueError, "attempt to assign sequence of size %d to extended slice of size %d" % (len(lst), n_elt)
                for scan, elt in izip(self.__islice_scan(key), lst):
                    scan[0] = elt
        else:
            self.__get_scan_from_pos(key, "list assignment index out of range")[0] = y
    
    def append(self, obj):
        "L.append(object) -- append object to end"
        link = [obj, self.__base[1], self.__base, True]
        self.__base[1][2] = link
        self.__base[1] = link
        self.__len += 1
    
    def count(self, value):
        "L.count(value) -> integer -- return number of occurrences of value"
        cnt = 0
        scan = self.__base[2]
        while len(scan) != 5:
            if value == scan[0]:
                cnt += 1
            scan = scan[2]
        return cnt
    
    def extend(self, iterable):
        "L.extend(iterable) -- extend list by appending elements from the iterable"
        for elt in iterable:
            self.append(elt)
    
    def index(self, value, start=0, stop=None):
        "L.index(value, [start, [stop]]) -> integer -- return first index of value"
        if not isinstance(start, int):
            raise TypeError, "start must be an integer"
        if stop is not None and not isinstance(stop, int):
            raise TypeError, "stop must be an integer or None"
        
        if start < 0:
            start += self.__len
        if isinstance(stop, int) and stop < 0:
            stop += self.__len
        
        for pos, v in enumerate(self):
            if (stop is not None) and pos >= stop:
                raise ValueError, "LinkedList.index(x): x not in list"
            if value == v and pos >= start:
                return pos
        else:
            raise ValueError, "LinkedList.index(x): x not in list"
    
    def insert(self, index, obj):
        "L.insert(index, object) -- insert object before index"
        if not isinstance(index, int):
            raise TypeError, "index must be an integer"
        
        if index < 0:
            index += self.__len
        
        if index >= self.__len:
            self.append(obj)
        else:
            scan = self.__get_scan_positive(index)
            
            link = [obj, scan[1], scan, True]
            scan[1][2] = link
            scan[1] = link
            self.__len += 1
    
    def insert_after(self, lref, obj):
        pass # XXX
    
    def insert_before(self, lref, obj):
        pass # XXX
    
    def merge_extend(self, other):
        """
        L.merge_extend(other) -- extend list by appending elements from the
                                 other linked list, which is simultaneously
                                 emptied
        
        >>> L = LinkedList(range(5))
        >>> O = LinkedList(range(10,15))
        >>> L.merge_extend(O)
        >>> L
        LinkedList([0, 1, 2, 3, 4, 10, 11, 12, 13, 14])
        >>> O
        LinkedList([])
        """
        if other.__len:
            self.__base[1][2], other.__base[2][1] = other.__base[2], self.__base[1]
            self.__base[1], other.__base[1][2] = other.__base[1], self.__base
            self.__len += other.__len
            
            other.__base[1:3] = other.__base, other.__base
            other.__len = 0
    
    def pop(self, index=-1):
        "L.pop([index]) -> item -- remove and return item at index (default last)"
        if not isinstance(index, int):
            raise TypeError, "index must be an integer"

        if not self.__len:
            raise IndexError, "pop from empty list"
        scan = self.__get_scan_from_pos(index, "pop index out of range")
        self.__remove_scan(scan)
        return scan[0]
    
    def prepend(self, obj):
        "L.prepend(object) -- prepend object to beginning"
        link = [obj, self.__base, self.__base[2], True]
        self.__base[2][1] = link
        self.__base[2] = link
        self.__len += 1
    
    def remove(self, value):
        "L.remove(value) -- remove first occurrence of value"
        scan = self.__base[2]
        while len(scan) != 5:
            if value == scan[0]:
                self.__remove_scan(scan)
                return
            scan = scan[2]
        else:
            raise ValueError, "LinkedList.remove(x): x not in list"
    
    def reverse(self):
        "L.reverse() -- reverse *IN PLACE*"
        self.__base[2:0:-1] = self.__base[1:3]
        scan = self.__base[1]
        for x in xrange(self.__len):
            scan[2:0:-1] = scan[1:3]
            scan = scan[1]
    
    def sort(self, cmp=None, key=None, reverse=False):
        """
        L.sort(cmp=None, key=None, reverse=False) -- stable sort *IN PLACE*;
        cmp(x, y) -> -1, 0, 1
        """
        if key is None:
            mykey = itemgetter(0)
        else:
            mykey = lambda x: key(x[0])
        
        sorted_links = sorted(self.__iter_scan(), cmp=cmp, key=mykey, reverse=reverse)
        
        prev = self.__base
        for link in sorted_links:
            prev[2] = link
            link[1] = prev
            prev = link
        self.__base[1] = prev
        prev[2] = self.__base
