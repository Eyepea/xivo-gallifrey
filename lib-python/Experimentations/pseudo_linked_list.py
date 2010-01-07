"""Pseudo Linked list

Copyright (C) 2008-2010  Proformatique

Not too inefficient linked list like container writen in pure Python
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

from operator import itemgetter, attrgetter
from itertools import chain, izip, imap

CHUNK = 16
HALF = CHUNK / 2

class _Link(object):
    """
    XXX
    """
    
    __slots__ = ('_parent', '_value')
    
    def __init__(self, parent, value):
        self._parent = parent
        self._value = value
    
    def __repr__(self):
        "x.__repr__() <==> repr(x)"
        return '<%r>' % (self._value,)
    
    def __getitem__(self, rel_pos):
        "x.__getitem__(y) <==> x[y]"
        if rel_pos == 0:
            return self._value
        else:
            pass # XXX
    
    def __setitem__(self, rel_pos, value):
        "x.__setitem__(i, y) <==> x[i]=y"
        if rel_pos == 0:
            self._value = value
        else:
            assert False # XXX
    
    def __delitem__(self, rel_pos):
        "x.__delitem__(y) <==> del x[y]"
        assert False # XXX
    
    def get_llist(self):
        """
        XXX
        """
        if self._parent is None:
            return None
        else:
            return self._parent.get_llist()


ATTR_VALUE_GET = attrgetter('_value')


class _Level_1(list):
    """
    XXX for internal use
    """
    
    __slots__ = ('_parent',)
    
    def __init__(self, parent, seq=()):
        self._parent = parent
        list.__init__(self, seq)
        for each in seq:
            each._parent = self
    
    def extend_adopt_nephew(self, other):
        "L.extend_adopt_nephew(lst) -- extend list by appending Link elements from the other"
        self.extend(other)
        for each in other:
            each._parent = self
    
    def ex_left_adopt_nephew(self, other):
        "L.ex_left_adopt_nephew(lst) -- extend list by prepending Link elements from the other"
        self[0:0] = other
        for each in other:
            each._parent = self
    
#         if isinstance(idx, slice):    # KEEP?
#             for each in self[idx]:    # KEEP?
#                 each._parent = None   # KEEP?
#         else:                         # KEEP?

    def merge_left_right(self, value, left, right):
        """
        XXX
        """
        ls = len(self)
        if ls < HALF:
            
            if right:
                lr = len(right)
                if ls + lr <= CHUNK:
                    self.extend_adopt_nephew(right)
                    return False, True, value
            else:
                lr = 0
            
            if left:
                ll = len(left)
                if ls + ll <= CHUNK:
                    left.extend_adopt_nephew(self)
                    return True, False, value
            else:
                ll = 0
            
            if lr:
                self.extend_adopt_nephew(right[:HALF - ls])
                del right[:HALF - ls]
            elif ll:
                self.ex_left_adopt_nephew(left[ls - HALF:])
                del left[ls - HALF:]
            
        return False, False, value
    
    def remove_merge_val(self, idx, left, right):
        """
        XXX
        """
        link = self.pop(idx)
        link._parent = None
        return self.merge_left_right(link._value, left, right)
    
    def delitem_val(self, idx):
        """
        NOTE: only called directly by LinkedList when we are at top-level
        
        Return the object that must replace this layer and the value of
        the deleted item.
        """
        link = self.pop(idx)
        link._parent = None
        return self, link._value
    
    def setitem_val(self, idx, val):
        """
        XXX
        """
        self[idx]._value = val
    
    def getitem_val(self, idx):
        """
        fast path: one element only (not a slice)
        """
        return self[idx]._value
    
    def getslicer_val(self, all, key=None, kstart=None, kstop=None, kstep=None):
        """
        XXX
        """
        if not key:
            if kstop >= 0:
                key = slice(kstart, kstop, kstep)
            else:
                key = slice(kstart, None, kstep)
        all.append(map(ATTR_VALUE_GET, self[key]))
    
    def postins_splitter(self):
        """
        XXX (TODO inline in insert_val?)
        """
        if len(self) > CHUNK:
            right = _Level_1(self._parent, self[HALF:])
            del self[HALF:]
            return right
    
    def insert_val(self, idx, obj):
        """
        XXX
        """
        self.insert(idx, _Link(self, obj))
        return self.postins_splitter()
    
    def append_val(self, obj):
        """
        XXX
        """
        self.append(_Link(self, obj))
        return self.postins_splitter()
    
    link_len = list.__len__
    
    iter_link = list.__iter__
        
    def clear(self):
        """
        XXX
        """
        for link in self:
            link._parent = None
        self._parent = None
        del self[:]

    def iter_val(self):
        """
        XXX
        """
        return imap(ATTR_VALUE_GET, self)

    def reversed_val(self):
        """
        XXX
        """
        return imap(ATTR_VALUE_GET, reversed(self))

    def contains_val(self, val):
        """
        XXX
        """
        return val in imap(ATTR_VALUE_GET, self)

    def count_val(self, val):
        """
        XXX
        """
        return map(ATTR_VALUE_GET, self).count(val)
    
    def index_val(self, val, start, stop):
        """
        XXX
        """
        for p, link in enumerate(self[start:stop]):
            if val == link._value:
                return p + start
    
    def remove_merge_byval(self, value, left, right):
        """
        XXX
        """
        idx = self.index_val(value, 0, None)
        if idx is not None:
            link = self.pop(idx)
            link._parent = None
            return self.merge_left_right(True, left, right)
        else:
            return False, False, False
    
    def remove_val(self, value):
        """
        XXX
        """
        idx = self.index_val(value, 0, None)
        if idx is not None:
            link = self.pop(idx)
            link._parent = None
            return self, True
        else:
            return self, False
    
    def get_llist(self):
        """
        XXX
        """
        if self._parent is None:
            return None
        else:
            return self._parent.get_llist()


class _Level_Sup(list):
    """
    blablabla
    """
    
    __slots__ = ('_parent',)
    
    def gen_under_ends_and_adopt(self, seq):
        """
        """
        accu = 0
        for under in seq:
            accu += under.link_len()
            under._parent = self
            yield [under, accu]
    
    def __init__(self, parent, seq):
        self._parent = parent
        list.__init__(self, self.gen_under_ends_and_adopt(seq))
    
    def __repr__(self):
        """
        x.__repr__() <=> repr(x)
        """
        return "[%s]" % ", ".join("<child %X end %d>" % (id(child), end) for (child, end) in self)
    
    @staticmethod
    def update_ends(prev_end, subs):
        """
        """
        end = prev_end
        for sub in subs:
            end += sub[0].link_len()
            sub[1] = end
    
    def extend_adopt_nephew(self, other):
        "L.extend_adopt_nephew(lst) -- extend list by appending Link elements from the other"
        end = self[-1][1]
        
        self.extend(other)
        
        for each_end in other:
            end += each_end[0].link_len()
            each_end[0]._parent = self
            each_end[1] = end
    
    def ex_left_adopt_nephew(self, other):
        "L.ex_left_adopt_nephew(lst) -- extend list by prepending Link elements from the other"
        self[0:0] = other
        
        for each, end in other:
            each._parent = self
        
        self.update_ends(0, self)
    
    def find_child(self, idx):
        """
        XXX
        """
        if idx < 0:
            idx += self[-1][1]
        if not (0 <= idx < self[-1][1]):
            raise IndexError, "list index out of range"
        
        start = 0
        for child, end in self:
            if end > idx:
                return child, start
            start = end
        else:
            assert False, "out of range condition should have been catched before"
    
    def find_child_pos(self, idx):
        """
        XXX
        """
        if idx < 0:
            idx += self[-1][1]
        if not (0 <= idx < self[-1][1]):
            raise IndexError, "list index out of range"
        
        start = 0
        for p, (child, end) in enumerate(self):
            if end > idx:
                return child, start, p
            start = end
        else:
            assert False, "out of range condition should have been catched before"
    
    def find_child_full(self, idx):
        """
        XXX
        """
        if idx < 0:
            idx += self[-1][1]
        if not (0 <= idx < self[-1][1]):
            raise IndexError, "list index out of range"
        
        left = None
        left_start = 0
        start = 0
        it = iter(self)
        for p, (child, end) in enumerate(it):
            if end > idx:
                if p < len(self) - 1:
                    right = it.next()[0]
                else:
                    right = None
                return child, start, p, left, right, left_start
            left = child
            left_start = start
            start = end
        else:
            assert False, "out of range condition should have been catched before"
    
    def link_len(self):
        """
        XXX
        """
        return self[-1][1]
    
    def getslicer_val(self, all, key=None, kstart=None, kstop=None, kstep=None):
        """
        XXX
        """
        if key:
            kstart, kstop, kstep = key.indices(self[-1][1])
        
        if kstep > 0:
            shifter = (kstep - 1 - kstart) % kstep
            adj = kstart % kstep
            if kstop <= kstart:
                return
            cb = 0
            for child, ce in self:
                if kstop <= cb:
                    break
                if kstart < ce:
                    cstart = max(kstart, ((cb + shifter) / kstep) * kstep + adj)
                    cstop = min(kstop, ((ce + shifter) / kstep) * kstep + adj)
                    child.getslicer_val(all, kstart=(cstart - cb), kstop=(cstop - cb), kstep=kstep)
                cb = ce
        else: # kstep < 0 (NOTE: kstep != 0 guaranteed by caller)
            astep = -kstep
            shifter = kstart % astep
            if kstop >= kstart:
                return
            for (child, ce), (devnull, cb) in izip(reversed(self), self[-2::-1] + [(None, 0)]):
                if kstop >= ce - 1:
                    break
                if kstart >= cb:
                    cstart = min(kstart, ((ce - 1 - shifter) / astep) * astep + shifter)
                    cstop = max(kstop, ((cb - shifter) / astep) * astep + shifter - 1)
                    child.getslicer_val(all, kstart=(cstart - cb), kstop=(cstop - cb), kstep=kstep)
    
    def setitem_val(self, idx, val):
        """
        XXX
        """
        child, start = self.find_child(idx)
        return child.setitem_val(idx - start, val)
    
    def getitem_val(self, idx):
        """
        XXX
        """
        child, start = self.find_child(idx)
        return child.getitem_val(idx - start)
    
    def pack(self, rm_child, rm_right, p, left_start):
        """
        XXX
        """
        if rm_right:
            self.pop(p + 1)[0]._parent = None
        if rm_child:
            self.pop(p)[0]._parent = None
        self.update_ends(left_start, self[max(p - 1, 0):])

    def common_suppr_val(self, idx):
        """
        XXX
        """
        child, start, p, left, right, left_start = self.find_child_full(idx)
        rm_child, rm_right, value = child.remove_merge_val(idx - start, left, right)
        self.pack(rm_child, rm_right, p, left_start)
        return value
    
    def merge_left_right(self, value, left, right):
        """
        XXX
        """
        ls = len(self)
        if ls < HALF:
            
            if right:
                lr = len(right)
                if ls + lr <= CHUNK:
                    self.extend_adopt_nephew(right)
                    return False, True, value
            else:
                lr = 0
            
            if left:
                ll = len(left)
                if ls + ll <= CHUNK:
                    left.extend_adopt_nephew(self)
                    return True, False, value
            else:
                ll = 0
            
            if lr:
                self.extend_adopt_nephew(right[:HALF - ls])
                del right[:HALF - ls]
                right.update_ends(0, right) # NOTE this line
            elif ll:
                self.ex_left_adopt_nephew(left[ls - HALF:])
                del left[ls - HALF:]
        
        return False, False, value
    
    def remove_merge_val(self, idx, left, right):
        """
        XXX
        """
        value = self.common_suppr_val(idx)
        return self.merge_left_right(value, left, right)
    
    def vanish(self, value):
        """
        XXX
        """
        if len(self) == 1:
            self[0][0]._parent = self._parent
            self._parent = None
            return self[0][0], value
        else:
            return self, value
    
    def delitem_val(self, idx):
        """
        NOTE: only called directly by LinkedList when we are at top-level
        
        Return the object that must replace this layer and the value of
        the deleted item.
        """
        value = self.common_suppr_val(idx)
        return self.vanish(value)
    
    def postins_splitter(self):
        """
        XXX (TODO inline in append_val?)
        """
        if len(self) > CHUNK:
            right = _Level_Sup(self._parent, map(itemgetter(0), self[HALF:]))
            del self[HALF:]
            return right
    
    def append_val(self, obj):
        """
        XXX
        """
        right = self[-1][0].append_val(obj)
        if len(self) > 1:
            prev_end = self[-2][1]
        else:
            prev_end = 0
        
        if right:
            self.append([right, 0])
            self.update_ends(prev_end, self[-2:])
            return self.postins_splitter()
        else:
            self.update_ends(prev_end, self[-1:])
    
    def insert_val(self, idx, obj):
        """
        XXX
        """
        child, start, pos = self.find_child_pos(idx)
        right = child.insert_val(idx - start, obj)
        
        if right:
            self.insert(pos + 1, [right, 0])
            self.update_ends(start, self[pos:])
            return self.postins_splitter()
        else:
            self.update_ends(start, self[pos:])
    
    def clear(self):
        """
        XXX
        """
        for child, end in self:
            child.clear()
        self._parent = None
        del self[:]
    
    def iter_val(self):
        """
        XXX
        """
        return chain(*[child.iter_val() for child, end in self])

    def reversed_val(self):
        """
        XXX
        """
        return chain(*[child.reversed_val() for child, end in reversed(self)])

    def contains_val(self, val):
        """
        XXX
        """
        return True in (child.contains_val(val) for child, end in self)

    def count_val(self, val):
        """
        XXX
        """
        return sum([child.count_val(val) for child, end in self])
    
    def index_val(self, val, start, stop):
        """
        XXX
        """
        cb = 0
        for child, ce in self:
            if cb < stop and start < ce:
                where = child.index_val(val, max(0, start - cb), min(child.link_len(), stop - cb))
                if where is not None:
                    return where + cb
            cb = ce
    
    def remove_merge_byval(self, value, left, right):
        """
        XXX
        """
        prev_child = (None, 0)
        left_start = 0
        for p, ((child, ce), (next_child, nce)) in enumerate(izip(self, self[1:] + [(None, 0)])):
            rm_child, rm_next, done = child.remove_merge_byval(value, prev_child, next_child)
            if done:
                self.pack(rm_child, rm_next, p, left_start)
                return self.merge_left_right(True, left, right)
            left_start = prev_child[1]
            prev_child = child
        else:
            return False, False, False
    
    def remove_val(self, value):
        """
        XXX
        """
        prev_child = (None, 0)
        left_start = 0
        for p, ((child, ce), (next_child, nce)) in enumerate(izip(self, self[1:] + [(None, 0)])):
            rm_child, rm_next, done = child.remove_merge_byval(value, prev_child, next_child)
            if done:
                self.pack(rm_child, rm_next, p, left_start)
                return self.vanish(True)
            left_start = prev_child[1]
            prev_child = child
        else:
            return self, False
    
    def get_llist(self):
        """
        XXX
        """
        if self._parent is None:
            return None
        else:
            return self._parent.get_llist()
    
    def iter_link(self):
        """
        XXX
        """
        return chain(*[child.iter_link() for child, end in self])


class LinkedList(object):
    """
    XXX
    """
    
    __slots__ = ('_root')
    
    def __init__(self, sequence=None):
        self._root = _Level_1(self)
        if sequence:
            # TODO acceleration if len(sequence) is known
            self.extend(sequence)
    
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
        return self._root.contains_val(value)
    
    def __delitem__(self, key):
        "x.__delitem__(y) <==> del x[y]"
        if isinstance(key, int):
            self._root, value = self._root.delitem_val(key)
        elif isinstance(key, slice):
            assert False # TODO
        else:
            raise TypeError, "list indices must be integers"
    
    def __getitem__(self, key):
        "x.__getitem__(y) <==> x[y]"
        if isinstance(key, int):
            return self._root.getitem_val(key)
        elif isinstance(key, slice):
            all = []
            self._root.getslicer_val(all, key)
            return list(chain(*all))
        else:
            raise TypeError, "list indices must be integers"
    
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
            self._root.clear()
            self._root = _Level_1(self)
        else:
            iter_self = iter(self)
            for x in xrange((factor - 1) * len(self)):
                self.append(iter_self.next())
        return self
    
    def __iter__(self):
        """
        x.__iter__() <==> iter(x)
        
        WARNING: for now this iterator does not support modification on the
        list that is being iterated
        """
        return self._root.iter_val()
    
    def __len__(self):
        "x._len__() <==> len(x)"
        return self._root.link_len()
    
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
    
    __rmul__ = __mul__
    
    def __repr__(self):
        "x.__repr__() <==> repr(x)"
        return "LinkedList(%r)" % list(self)
    
    def __reversed__(self):
        """
        L.__reversed__() -- return a reverse iterator over the list
        
        WARNING: for now this iterator does not support modification on the
        list that is being iterated
        """
        return self._root.reversed_val()
    
    def __setitem__(self, key, val):
        "x.__setitem__(i, y) <==> x[i]=y"
        if isinstance(key, int):
            return self._root.setitem_val(key, val)
        elif isinstance(key, slice):
            assert False # TODO
        else:
            raise TypeError, "list indices must be integers"
    
    def append(self, obj):
        "L.append(object) -- append object to end"
        right = self._root.append_val(obj)
        if right:
            self._root = _Level_Sup(self, (self._root, right))
    
    def count(self, value):
        "L.count(value) -> integer -- return number of occurrences of value"
        return self._root.count_val(value)
    
    def extend(self, iterable):
        "L.extend(iterable) -- extend list by appending elements from the iterable"
        for val in iterable:
            self.append(val)
    
    def index(self, value, start=0, stop=None):
        """
        L.index(value, [start, [stop]]) -> integer -- return first index of value
        """
        if start < 0:
            start += len(self)
        
        if stop is None:
            stop = len(self)
        elif stop < 0:
            stop += len(self)
        
        where = self._root.index_val(value, start, stop)
        
        if where is None:
            raise ValueError, "LinkedList.index(x): x not in list"
        
        return where
    
    def insert(self, index, obj):
        """
        L.insert(index, object) -- insert object before index
        """
        curlen = self._root.link_len()
        if index < 0:
            index += curlen
        index = max(min(curlen, index), 0)
        
        if index >= curlen:
            right = self._root.append_val(obj)
        else:
            right = self._root.insert_val(index, obj)
        
        if right:
            self._root = _Level_Sup(self, (self._root, right))
    
    def pop(self, index=-1):
        """
        L.pop([index]) -> item -- remove and return item at index (default last)
        """
        if not isinstance(index, int):
            raise TypeError, "list indices must be integers"
        
        self._root, value = self._root.delitem_val(index)
        return value
    
    def prepend(self, obj):
        "L.prepend(object) -- prepend object to beginning"
        self.insert(0, obj)
    
    def remove(self, value):
        "L.remove(value) -- remove first occurrence of value"
        new_root, done = self._root.remove_val(value)
        if not done:
            raise ValueError, "LinkedList.remove(x): x not in list"
        self._root = new_root
    
    def reverse(self):
        "L.reverse() -- reverse *IN PLACE*"
        assert False # XXX
    
    def sort(self, cmp=None, key=None, reverse=False):
        """
        L.sort(cmp=None, key=None, reverse=False) -- stable sort *IN PLACE*;
        cmp(x, y) -> -1, 0, 1
        """
        assert False # XXX
    
    def get_llist(self):
        """
        XXX
        """
        return self
    
    def iter_link(self):
        """
        XXX
        """
        return self._root.iter_link()
