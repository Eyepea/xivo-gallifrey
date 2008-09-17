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
    
    class Ref(object):
        """
        A reference on a list element.
        """
        
        __slots__ = ('_llist', '_scan', '_serial')
        
        def __init__(self, llist, scan, serial):
            self._llist = llist
            self._scan = scan
            self._serial = serial
        
        def __get_scan_at(self, rel_pos):
            """
            Get the link at relative position rel_pos of the pointed list
            element.
            """
            if not isinstance(rel_pos, int):
                raise TypeError, "list indices must be integers"
            scan = self._scan
            if len(scan) == 6:
                if not self._llist._len:
                    raise IndexError, "list index out of range"
                scan = scan[1]
                self._scan = scan
                self._serial = scan[4]
            if rel_pos and not scan[3]:
                raise IndexError, "only index 0 is available on directly referenced links that have been removed from their list"
            abs_rel_pos = abs(rel_pos)
            nxt = (rel_pos > 0) + 1
            for x in xrange(abs_rel_pos):
                scan = scan[nxt]
                if len(scan) == 6:
                    raise IndexError, "list index out of range"
            return scan
        
        def __delitem__(self, rel_pos):
            "x.__delitem__(y) <==> del x[y]"
            scan = self.__get_scan_at(rel_pos)
            
            if scan[4][0] > self._serial:
                raise IndexError, "can not delete a referenced link if it has been rebased"
            if not scan[3]:
                raise IndexError, "trying to remove a link that is already dead"
            
            scan[2][1] = scan[1]
            scan[1][2] = scan[2]
            scan[3] = False
            self._llist._len -= 1
            scan = scan[2]
            self._scan = scan
            self._serial = scan[4]
        
        def __getitem__(self, rel_pos):
            "x.__getitem__(y) <==> x[y]"
            return self.__get_scan_at(rel_pos)[0]
        
        def __setitem__(self, rel_pos, value):
            "x.__setitem__(i, y) <==> x[i]=y"
            self.__get_scan_at(rel_pos)[0] = value
        
        def goto_rel(self, rel_pos=1):
            """
            Move the reference to another link.
            """
            if rel_pos == 0:
                return
            
            scan = self.__get_scan_at(rel_pos)
            
            if scan[4][0] > self._serial:
                raise IndexError, "can not update a link reference if the link has been rebased"
            
            self._scan = scan
            self._serial = scan[4]
        
        def iter_right(self):
            """
            Generate values from the content of the referenced element to the
            right until the end of the list.
            """
            scan = self._scan
            while len(scan) != 6:
                if scan[3]:
                    yield scan[0]
                scan = scan[2]
        
        def iter_left(self):
            """
            Generate values from the content of the referenced element to the
            left until the beginning of the list.
            """
            scan = self._scan
            while len(scan) != 6:
                if scan[3]:
                    yield scan[0]
                scan = scan[1]
    
    __slots__ = ('_base', '_len', '_serial')
    
    def __init__(self, sequence=None):
        self._serial = 0
        self._base = [None, None, None, True, [self._serial], None]
        self._base[1:3] = self._base, self._base
        self._len = 0
        if sequence is not None:
            self.extend(sequence)
    
    def __iter_scan(self):
        """
        Generate each link of the linked list, from the first to the last one.
        
        WARNING: does _not_ test for liveness of any link - this method should
        not be called by anything that yields something derived from these
        links unless the caller checks the liveness of the link itself.
        """
        scan = self._base[2]
        while len(scan) != 6:
            yield scan
            scan = scan[2]
    
    def __get_scan_positive(self, pos):
        """
        Return the link which is at absolute position @pos.
        """
        # TODO: optimize by scanning in reverse if pos near the end
        scan = self._base[2]
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
            pos += self._len
        if not (0 <= pos < self._len):
            raise IndexError, exc_str
        
        return self.__get_scan_positive(pos)
    
    def __remove_scan(self, scan):
        """
        Remove the link @scan from the linked list.
        """
        if not scan[3]:
            raise IndexError, "trying to remove a link that is already dead"
        scan[2][1] = scan[1]
        scan[1][2] = scan[2]
        scan[3] = False
        self._len -= 1
    
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
        start, stop, step = slc.indices(self._len)
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
            self._base[1:3] = self._base, self._base
            self._len = 0
        else:
            iter_self = iter(self)
            for x in xrange((factor - 1) * self._len):
                self.append(iter_self.next())
        return self
    
    def __iter__(self):
        "x.__iter__() <==> iter(x)"
        scan = self._base[2]
        while len(scan) != 6:
            if scan[3]:
                yield scan[0]
            scan = scan[2]
    
    def __len__(self):
        "x._len__() <==> len(x)"
        return self._len
    
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
        scan = self._base[1]
        while len(scan) != 6:
            if scan[3]:
                yield scan[0]
            scan = scan[1]
    
    def __rmul__(self, factor):
        "x.__rmul__(n) <==> n*x"
        return self.__mul__(factor)
    
    def __setitem__(self, key, y):
        "x.__setitem__(i, y) <==> x[i]=y"
        if isinstance(key, slice):
            start, stop, step = key.indices(self._len)
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
                        link = [elt, scan, scan[2], True, self._base[4]]
                        scan[2][1] = link
                        scan[2] = link
                        self._len += 1
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
        link = [obj, self._base[1], self._base, True, self._base[4]]
        self._base[1][2] = link
        self._base[1] = link
        self._len += 1
    
    def count(self, value):
        "L.count(value) -> integer -- return number of occurrences of value"
        cnt = 0
        scan = self._base[2]
        while len(scan) != 6:
            if value == scan[0]:
                cnt += 1
            scan = scan[2]
        return cnt
    
    def extend(self, iterable):
        "L.extend(iterable) -- extend list by appending elements from the iterable"
        for elt in iterable:
            self.append(elt)
    
    def get_ref(self, index):
        """
        Return a reference on a particular element of the list.
        """
        scan = self.__get_scan_from_pos(index, "list index out of range")
        return self.Ref(self, scan, scan[4][0])
    
    def index(self, value, start=0, stop=None):
        "L.index(value, [start, [stop]]) -> integer -- return first index of value"
        if not isinstance(start, int):
            raise TypeError, "start must be an integer"
        if stop is not None and not isinstance(stop, int):
            raise TypeError, "stop must be an integer or None"
        
        if start < 0:
            start += self._len
        if isinstance(stop, int) and stop < 0:
            stop += self._len
        
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
            index += self._len
        
        if index >= self._len:
            self.append(obj)
        else:
            scan = self.__get_scan_positive(index)
            
            link = [obj, scan[1], scan, True, self._base[4]]
            scan[1][2] = link
            scan[1] = link
            self._len += 1
    
    def insert_after(self, lref, obj):
        pass # XXX
    
    def insert_before(self, lref, obj):
        pass # XXX
    
    def merge_extend(self, other):
        """
        L.merge_extend(other) -- extend list by appending elements from the
                                 other linked list, which is simultaneously
                                 emptied
        
        >>> L = LinkedList(range(4))
        >>> O = LinkedList(range(10,14))
        >>> L.merge_extend(O)
        >>> L
        LinkedList([0, 1, 2, 3, 10, 11, 12, 13])
        >>> O
        LinkedList([])
        """
        if other._len:
            self._serial = max(self._serial, other._serial) + 1
            
            self._base[1][2], other._base[2][1] = other._base[2], self._base[1]
            self._base[1], other._base[1][2] = other._base[1], self._base
            self._len += other._len
            other._base[4][0] = self._serial
            
            other._base[1:3] = other._base, other._base
            other._serial = self._serial + 1
            other._base[4] = [other._serial]
            other._len = 0
    
    def pop(self, index=-1):
        "L.pop([index]) -> item -- remove and return item at index (default last)"
        if not self._len:
            raise IndexError, "pop from empty list"
        scan = self.__get_scan_from_pos(index, "pop index out of range")
        self.__remove_scan(scan)
        return scan[0]
    
    def prepend(self, obj):
        "L.prepend(object) -- prepend object to beginning"
        link = [obj, self._base, self._base[2], True, self._base[4]]
        self._base[2][1] = link
        self._base[2] = link
        self._len += 1
    
    def remove(self, value):
        "L.remove(value) -- remove first occurrence of value"
        scan = self._base[2]
        while len(scan) != 6:
            if value == scan[0]:
                self.__remove_scan(scan)
                return
            scan = scan[2]
        else:
            raise ValueError, "LinkedList.remove(x): x not in list"
    
    def reverse(self):
        "L.reverse() -- reverse *IN PLACE*"
        self._base[2:0:-1] = self._base[1:3]
        scan = self._base[1]
        for x in xrange(self._len):
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
        
        prev = self._base
        for link in sorted_links:
            prev[2] = link
            link[1] = prev
            prev = link
        self._base[1] = prev
        prev[2] = self._base
