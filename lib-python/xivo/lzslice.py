"""Lazy List Slicer that let us get multiple sliced views of the very same array

Copyright (C) 2007, 2008  Proformatique

Some fondamental concepts in functional programming are pattern matching, lists
comprehension and lazy evaluation. When trying to apply simultaneously all these
idioms in Python, one will realize that it might result in syntaxicaly horrible
things, partially defeating the very purpose of functional programming by
requiring direct manipulation of various states in higher levels of code.

For example let's imagine a big list which contains some slices begining with
an header of 2 elements that must have a particular format, then a banalized
payload with every elements having the same meaning (but also required to be of
a given format). We will write a piece of code to check whether a given slice of
this list is of the format described above or not.

Doing that in a functional langage is trivial and what we get is highly usable,
even in some situations we did not think of previously.

Let's consider the following haskell code:

check_format :: [a] -> Bool
check_format  [] = False
check_format [x] = False
check_format (h1:h2:p) =
    check_header_line1 h1
    && check_header_line2 h2
    && check_payload p where
        check_payload [] = True
        check_payload (l:r) =
            check_payload_line l && check_payload r

Even if nothing as been specialy written for that use case in check_format, it
is possible for the caller to do on demand filtering and/or adaptation so that
code reuse is greatly facilitated by the fact check_format will neither need
adaptation nor duplication in a more general form.

Trying to do it the imperative way (in Python) can lead us to a (very!) naive
solution like:

def check_format(lot_of_lines):
    if len(lot_of_lines) < 2: return False
    if not check_header_line1(lot_of_lines[0]): return False
    if not check_header_line2(lot_of_lines[1]): return False
    for line in lot_of_lines[2:]:
        if not check_payload_line(line): return False
        return True

This way of doing things is highly readable and can even be near directly mapped
to the corresponding Haskell code, but has at least the following drawbacks:
- the function is not scalable as evaluation of lot_of_lines[2:] will require
  a full copy to be created even before iteration starts.
  We should walk around that by using a generator.
- because the whole array is scanned, if as in the description we previously
  made you are interested in checking that a subpart of all lines are of the
  checked format, you will have to extract a slice of this part. The obvious
  problem is that it again won't scale well up to "lot of lines", because as
  Python is an imperative langage the slice is simply created by copying its
  whole content (it would be too complicated to make things work differently)
- lot_of_lines must be an array, so trying to work around the previous problem
  using a generator won't work.

So maybe it could be better to use an iterable approach?
Look at this:

def check_format(lines_seq):
    lines_iter = iter(lines_seq)
    try:
        if not check_header_line1(lines_iter.next()): return False
        if not check_header_line2(lines_iter.next()): return False
    except StopIteration:
        return False
    for line in lines_iter:
        if not check_payload_line(line): return False
    return True

Some comments:
- This solves all the above problems. Indeed when used in a functional way by
  some other Python code I think this is as powerful as the Haskell code above.
- IMHO this looks terrible. Achiving something similar to what we got thanks
  to a simple pattern matching on a list by catching an exception gives me
  heart attacks :)
- It uses side effects in a complicated fashion (indeed this is quite simple
  here, but in real world with more complicated work to do this would be hard
  to follow, and extremely hard to check compared to a more fonctional approach)
  It's a little frightening to see non-atomic state changes in a try except
  bloc. It will hurt safety in an evolutive context: if the exception handling
  is modified to do something else than returning, the state of the iterator
  will be unknown and if inadvertently reused it will either lead to a bug, or
  if this is really what the developer intended, emergency measures should be
  taken so that he does not touch a keyboard anymore for the rest of his life :)

The problem is that list comprehensions and lazy evaluation doesn't mix so well
in Python, so you can't write something more friendly while still obtaining full
power and yet keeping the cost low.

Anyway, when living in an imperative world and trying to do fun functional
stuffs, you can't for sure obtain the combined power of both. But in certain
situation you are more limited than you should.

So you uses idioms to make hybrid programing looks prettier and retains at
least the power you needs:
- imagine you know for sure that all lines are already loaded in a list
- you neither want to write iterator related black magic, nor accept to
  loose efficiency with useless copies or power by deactivation of the
  possibility to evaluate the function on just a slice.

Thanks to the lzslice you can create an object which will adapt indexing
according to the slice configured at instantiation time. Still while
instantiating, it can compose the given slice with an old one from the
underlying object if it is itself a lzslice instance, so its cheap to stack
multiples views and you don't have to worry much to know if an object is a
lzslice or a real container when creating a new one. The way this is achived
is that the resulting combined slice is calculated at instantiation time, and
never touched after that (living lzslice instances are read only) so a
condition for this to work is that the size of the underlying object don't
changes.

Be aware that a lzslice instance does not implement COW or any other
similar thing; what you get really is just a different view from the same data.
It is ok to change a cell content (but you wont do that because you prefer pure
functions, don't you ? :) if you are prepared for the original data to be
modified in this case, but as stated before it is absolutely not OK to modify
the size of the underlying object after instantiation of a lzslice adapter for
that one and before dismissal of this adapter. In other words you should think
of lzslice as a static topology adapter, letting you access the same data (or a
subset of) using a different geometry.

Now check_format is written like that:

def check_format(lot_of_lines):
    if len(lot_of_lines) < 2: return False
    if not check_header_line1(lot_of_lines[0]): return False
    if not check_header_line2(lot_of_lines[1]): return False
    for line in lazy_(lot_of_lines)[2:]):
        if not check_payload_line(line): return False
    return True

You can even replace the loop by the following to be even more functional:
    if any((not check_payload_line(line) for line in lazy_(lot_of_lines)[2:])):
        return False

I think that even if it seems not mandatory in this case, the use of lzslice()
is better than trying to do the same thing directly with xrange, because
lzslice() guarantee you won't index the list out of bounds (lzslice() will
ultimately use xrange for iteration anyway). Also remember that lot_of_lines
itself can already be a lzslice object (after all this is the reason we created
it at the beginning) or even can come from a stack of thousands of lazy slices,
and that in these cases only one transformation of the index is performed.

If you can live with it, this module also provides the same kind of wrapper
than lazy_ for itertools.islice, named lazislice, so you can use negative
integers to create some islice if needed. (only usable if the object has a
length and is iterable)

Conclusion: The lazier you are, the more you can profit from functional
programming ;)

--
Note that even if lot_of_lines can be a lzslice instance, the underlying
object must still be indexable.
So now I should think of something to remove this limitation and let this idiom
be usable in more cases :D

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

from itertools import imap, izip, islice, ifilter
from xivo.pyfunc import *

def xor(a, b):
    return (not(a)) != (not(b))

def equiv(a, b):
    return (not(a)) == (not(b))

def _indices_len(start, stop, stride):
    if not stride:
        raise ValueError, "slice step cannot be zero"
    if (not isinstance(start, int)) \
       or (not isinstance(stop, int)) \
       or (not isinstance(stride, int)):
        raise TypeError, "parameters of _indices_len must be integers"
    if stride > 0:
        if start >= stop:
            return 0
        add = stride - 1
    else:
        if start <= stop:
            return 0
        add = stride + 1
    return (stop - start + add) / stride

def _slice_len(slicer, objlen):
    return _indices_len(*slicer.indices(objlen))

def _check_slice(slicer):
    if any((x is not None) and (not isinstance(x, int))
           for x in (slicer.start, slicer.stop, slicer.step)):
        raise TypeError, "slice indices must be integers or None"
    if (slicer.step is not None) and (not slicer.step):
        raise ValueError, "slice step cannot be zero"

def slice_compose(ext_slc, int_slc, obj_len):
    map(_check_slice, (ext_slc, int_slc))
    int_start, int_stop, int_step = int_slc.indices(obj_len)
    slice_len = _indices_len(int_start, int_stop, int_step)
    ext_start, ext_stop, ext_step = ext_slc.indices(slice_len)
    if equiv(ext_step > 0, ext_stop < ext_start):
        ext_stop = ext_start
    start = int_start + int_step * ext_start
    stop = int_start + int_step * ext_stop
    step = int_step * ext_step
    if equiv(int_step > 0, stop > int_stop):
        stop = int_stop
    if equiv(step > 0, stop < start):
        stop = start
    if stop < 0:
        stop = None
    return slice(start, stop, step)

SIZE_CHANGE_ERRMSG = "size change of the underlying object unsupported"

class lzslice(object):
    def __setattr__(self, *args):
        "This methods is present to give immutability to an instance."
        raise TypeError("object does not support item assignment")
    __delattr__ = __setattr__
    def _set_anyway(self, attr, val):
        super(lzslice, self).__setattr__(attr, val)
    def __init__(self, obj, mand, *args):
        """You can create lzslice objects with
                lzslice(obj, slice([start,] stop[, step]))
        or
                lzslice(obj, [start,] stop[, step])

        - obj must be an indexable and measurable object
        - you can directly pass a slice object that has previously
          been created by Python or by you if you filled it with sane
          compatible values
        - or instead of a slice object you can use the same kind of
          argument (obj put aside) you would use when constructing one
          with slice()

        Please note that while you intend to use the created lazy slice
        object, the size of the underlying array _must not change_.
        If any method detects the size of the underlying array changed
        between instantiation and the time of its, a TypeError
        exception will be raised."""
        obj.__getitem__
        obj.__len__
        self._set_anyway('obj', obj)
        if len(args) == 0 and isinstance(mand, slice):
            self._set_anyway('slicer', mand)
        else:
            self._set_anyway('slicer', slice(mand, *args))
        _check_slice(self.slicer)
        if isinstance(self.obj, lzslice):
            self._set_anyway('_direct', self.obj._direct)
            _check_slice(self.obj._blade)
            self._set_anyway('_blade',
                    slice_compose(self.slicer, self.obj._blade,
                                  len(self.obj._direct)))
        else:
            self._set_anyway('_direct', self.obj)
            self._set_anyway('_blade', self.slicer)
        _check_slice(self._blade)
        dir_len = len(self._direct)
        self._set_anyway('_dir_len', dir_len)
        self._set_anyway('_len', _slice_len(self._blade, dir_len))
        start, stop, step = self._blade.indices(dir_len)
        self._set_anyway('_start', start)
        self._set_anyway('_stop', stop)
        self._set_anyway('_step', step)
    def __len__(self):
        "Returns the slice len calculated at instantiation time."
        if self._dir_len != len(self._direct):
            raise TypeError, SIZE_CHANGE_ERRMSG
        return self._len
    def _slice_compose(self, slicer):
        return slice_compose(slicer, self._blade, self._dir_len)
    def _resolv_idx(self, idx):
        if not isinstance(idx, int):
            raise TypeError, "list indices must be integers"
        slen = len(self)
        if not (-slen <= idx < slen):
            raise IndexError, "list index out of range"
        if idx < 0:
            idx = slen + idx
        return self._start + idx * self._step
    def __getitem__(self, key):
        "key can be an index or a slice"
        if self._dir_len != len(self._direct):
            raise TypeError, SIZE_CHANGE_ERRMSG
        if isinstance(key, slice):
            return self._direct[self._slice_compose(key)]
        else:
            return self._direct[self._resolv_idx(key)]
    def __setitem__(self, key, value):
        """key can be an index or a slice.
        When this is a slice, value must be of the same length than the
        slice one, because the total length of the underlying object
        must never change while using lazy slices."""
        if self._dir_len != len(self._direct):
            raise TypeError, SIZE_CHANGE_ERRMSG
        if isinstance(key, slice):
            slicer = self._slice_compose(key)
            slen = _slice_len(slicer, self._dir_len)
            if slen != len(value):
                raise ValueError, "you can't change the size of an " \
		                  "underlying array with a lzslice adaptor"
            self._direct[slicer] = value
        else:
            self._direct[self._resolv_idx(key)] = value
    def __iter__(self):
        "Iterate through the lazy slice."
        if self._dir_len != len(self._direct):
            raise TypeError, SIZE_CHANGE_ERRMSG
        return (self._direct[i]
	        for i in xrange(self._start, self._stop, self._step))
    def __cmp__(self, other):
        """Let T be a set of authorized types:
                {tuple, str, list}

        Lexicographic comparison will work well if the underlying
        object is of a type in T, and if the other object is of a type
        in T or a lzslice instance whose underlying object is of a type
        in T. We can't really know what to do if this is not the case
        so if this happens a TypeError will just be raised.

        In any of these cases, the behavior of this method is the same
        as native Python comparison: strings are "greater" then lists
        and tuples are "greater" than strings, and if of the same type
        a lexicographic comparison of their relative content is
        performed.

        Python is smart enough to call us even if a real list or a real
        tuple or a real string is on the left side of the comparison
        operator, so existing code to which you passes such an object
        will be less likely to break."""
        if self._dir_len != len(self._direct):
            raise TypeError, SIZE_CHANGE_ERRMSG
        T = (tuple, str, list)
        iam = tuple(isinstance(self._direct, x) for x in T)
        if not any(iam):
            raise TypeError, "Underlying object of a lazy slice is not of a " \
                             "type in %s, refusing to compare anything." \
                             % str(T)
        if isinstance(other, lzslice):
            other_for_type = other._direct
        else:
            other_for_type = other
        its = tuple(isinstance(other_for_type, x) for x in T)
        if not any(its):
            raise TypeError, "Object being compared to a lazy slice is " \
                             "neither of a type in %s, nor another lazy " \
                             "slice with an underlying object of such a " \
                             "type. Cannot compare anything." % str(T)
        if iam > its:
            return 1
        if iam < its:
            return -1
        for my, ot in izip(self, other):
            if my < ot:
                return -1
            if my > ot:
                return  1
        return len(self) - len(other)

class lazy_:
    def __init__(self, obj):
        """You will probably like this factory wrapper class to create
        lzslice objects instead of instantiating by
        lzslice(obj, slice(...)) directly, because it's syntaxically
        easier to write something like
                lazy_(obj)[::2]
        and because it supports automatic calculation of negative
        offsets, so you can also do
                lazy_(obj)[:-1]

        (in previous attempts we tried to use another wrapper so we
         could wrote lzslice(obj, SL_[::2]) but lzslice(obj, SL_[:-1])
         broke because __len__ is called before __getitem__ when
         negative offset are used)
        """
        obj.__len__
        self.obj = obj
        self.len = len(obj)
    def __getitem__(self, k):
        if self.len != len(self.obj):
            raise TypeError, SIZE_CHANGE_ERRMSG
        if not isinstance(k, slice):
            raise TypeError, "lazy_ indexing needs a slice, got a %r" \
                             % type(k).__name__
        return lzslice(self.obj, k)
    def __len__(self):
        if self.len != len(self.obj):
            raise TypeError, SIZE_CHANGE_ERRMSG
        return self.len

class lazislice:
    def __init__(self, obj):
        obj.__iter__
        obj.__len__
        self.obj = obj
        self.len = len(obj)
    def __getitem__(self, k):
        if self.len != len(self.obj):
            raise TypeError, SIZE_CHANGE_ERRMSG
        if not isinstance(k, slice):
            raise TypeError, "lazislice indexing needs a slice, got a %r" \
                             % type(k).__name__
        start, stop, step = k.indices(self.len)
        if stop < 0:
            start, stop = 0, 0
        return islice(self.obj, start, stop, step)
    def __len__(self):
        if self.len != len(self.obj):
            raise TypeError, SIZE_CHANGE_ERRMSG
        return self.len

class FilteredView:
    def __init__(self, f, array, empty_begin = None, empty_end = None,
                 part = None):
        self._underlying = array
        self._filter = f
        self._part = part
        if part:
            a, b, c = slice(part[0], part[1], 1).indices(len(array))
            a, b = max(a, 0), max(b, 0)
            self._start, self._stop = a, max(a, b)
        else:
            self._start, self._stop = 0, len(array)
        vlen = self._stop - self._start
        if empty_begin is not None:
            self._virtual_begin = max(min(empty_begin, vlen), 0)
        else:
            self._virtual_begin = None
        if empty_end is not None:
            self._virtual_end = max(min(empty_end, vlen), 0,
                                    self._virtual_begin)
        else:
            self._virtual_end = None
    def _underl_iter(self):
        return islice(self._underlying, self._start, self._stop)
    def __iter__(self):
        return ifilter(self._filter, self._underl_iter())
    def __len__(self):
        return all_and_count(True for elt in self._underl_iter()
                                  if self._filter(elt))
    def __getitem__(self, idx):
        if (idx ^ 0) >= 0:
            try:
                return islice(iter(self), idx, idx+1).next()
            except StopIteration:
                raise IndexError, 'list index out of range'
        else:
            return [elt for elt in self._underl_iter()
                        if self._filter(elt)][idx]
    def idxmap_list(self):
        return [pos for pos, elt in enumerate(self._underl_iter())
                    if self._filter(elt)]
    def idxmap_iter(self):
        return (pos for pos, elt in enumerate(self._underl_iter())
                    if self._filter(elt))
    def real_idx(self, idx):
        if (idx^0) >= 0:
            try:
                return islice(self.idxmap_iter(), idx, idx+1).next()
            except StopIteration:
                raise IndexError, 'list index out of range'
        else:
            return self.idxmap_list()[idx]
    def real_enumerate(self):
        return ((pos, elt) for pos, elt in enumerate(self._underl_iter())
                           if self._filter(elt))
    def idxmap_unsliced_list(self):
        return [pos + self._start for pos, elt in enumerate(self._underl_iter())
                                  if self._filter(elt)]
    def idxmap_unsliced_iter(self):
        return (pos + self._start for pos, elt in enumerate(self._underl_iter())
                                  if self._filter(elt))
    def real_unsliced_idx(self, idx):
        return self.real_idx(idx) + self._start
    def real_unsliced_enumerate(self):
        return ((pos + self._start, elt)
                for pos, elt in enumerate(self._underl_iter())
                if self._filter(elt))
    def __setitem__(self, idx, new_elt):
        self._underlying[self.real_idx(idx) + self._start] = new_elt
    def __delitem__(self, idx):
        del self._underlying[self.real_idx(idx) + self._start]
        self._stop -= 1
        vlen = self._stop - self._start
        self._virtual_begin = max(self._virtual_begin, vlen)
        self._virtual_end = max(self._virtual_end, vlen)
    def prepend(self, new_elt):
        pos = first(self.idxmap_iter())
        if pos is not None:
            self._underlying.insert(self._start + pos, new_elt)
        else:
            self._underlying.insert(self._start + self._virtual_begin, new_elt)
        self._stop += 1
    def _generic_insert(self, ins_pos, new_elt, correction):
        try:
            real_pos = self.real_idx(ins_pos)
            self._underlying.insert(self._start + real_pos + correction,
                                    new_elt)
            self._stop += 1
        except IndexError:
            if ins_pos >= 0:
                self.append(new_elt)
            else:
                self.prepend(new_elt)
    def common_insert(self, ins_pos, new_elt, after = False):
        self._generic_insert(ins_pos, new_elt, bool(after))
    def insert_before(self, ins_pos, new_elt):
        self._generic_insert(ins_pos, new_elt, 0)
    insert = insert_before
    def insert_after(self, ins_pos, new_elt):
        self._generic_insert(ins_pos, new_elt, 1)
    def append(self, new_elt):
        pos = last(self.idxmap_iter())
        if pos is not None:
            self._underlying.insert(self._start + pos + 1, new_elt)
        else:
            self._underlying.insert(self._start + self._virtual_end, new_elt)
        self._stop += 1
    def extend(self, seq):
        for elt in seq:
            self.append(elt)

### XXX TODO Regression testing for part argument of FilteredView, as well as
###    idxmap_list, idxmap_iter, real_idx, and real_enumerate and
###    their unsliced variant
### XXX TODO add something to move the start

__all__ = ("lazy_", "lzslice", "slice_compose", "lazislice", "FilteredView")

if __name__ == '__main__':
    import unittest
    class LazyCase(unittest.TestCase):
        def testInstNonLenRaises(self):
            self.assertRaises(AttributeError, lazy_, None)
        def testLazyEquivSlice(self):
            class slk:
                def __getitem__(self, k):
                    return k
                def __len__(self):
                    return 42
            sl_ = slk()
            a = range(42)
            lza = lazy_(a)[:]
            self.assertEqual(lza.slicer, sl_[:])
            lza = lazy_(a)[0:]
            self.assertEqual(lza.slicer, sl_[:])
            lza = lazy_(a)[:33]
            self.assertEqual(lza.slicer, sl_[:33])
            lza = lazy_(a)[4:33]
            self.assertEqual(lza.slicer, sl_[4:33])
            lza = lazy_(a)[:-1]
            self.assertEqual(lza.slicer, sl_[:-1])
            lza = lazy_(a)[-1:]
            self.assertEqual(lza.slicer, sl_[-1:])
            lza = lazy_(a)[::]
            self.assertEqual(lza.slicer, slice(None, None, None))
            lza = lazy_(a)[1::]
            self.assertEqual(lza.slicer, slice(1, None, None))
            lza = lazy_(a)[:1:]
            self.assertEqual(lza.slicer, slice(None, 1, None))
            lza = lazy_(a)[::-1]
            self.assertEqual(lza.slicer, slice(None, None, -1))
    class LazisliceCase(unittest.TestCase):
        def testNonIterRaises(self):
            class blah:
                def __len__(self):
                    return 42
            self.assertRaises(AttributeError, lazislice, blah())
        def testNonLenRaises(self):
            class blah:
                def __iter__(self):
                    yield 42
            self.assertRaises(AttributeError, lazislice, blah())
        def testLenChangeRaises(self):
            a = range(42)
            lz = lazislice(a)
            a.append(666)
            self.assertRaises(TypeError, lz.__len__)
            self.assertRaises(TypeError, lz.__getitem__, slice(0, 1, 1))
        def testGetItemNonSliceRaises(self):
            self.assertRaises(TypeError, lazislice(range(42)).__getitem__, 17)
        def noneAndXrange(self, low, high):
            yield None
            for i in xrange(low, high):
                yield i
        def testSlicesEquiv(self):
            for l in xrange(10):
                lst = range(l)
                for start in self.noneAndXrange(-l-2, l+2):
                    for stop in self.noneAndXrange(-l-2, l+2):
                        for step in self.noneAndXrange(1, l+2):
                            pos_start, pos_stop, pos_step = slice(start, stop, step).indices(l)
                            if pos_stop < 0:
                                pos_start, pos_stop = 0, 0
                            lzi = lazislice(lst)[start:stop:step]
                            isl = islice(lst, pos_start, pos_stop, pos_step)
                            sli = lst[start:stop:step]
                            lzi_iter, isl_iter, sli_iter = map(iter, (lzi, isl, sli))
                            for bla in xrange(len(sli)):
                                reference = sli_iter.next()
                                self.assertEqual(reference, isl_iter.next())
                                self.assertEqual(reference, lzi_iter.next())
                            self.assertRaises(StopIteration, sli_iter.next)
                            self.assertRaises(StopIteration, isl_iter.next)
                            self.assertRaises(StopIteration, lzi_iter.next)
        def testSlicesNegStep(self):
            for l in xrange(10):
                lst = range(l)
                for start in self.noneAndXrange(-l, l):
                    for stop in self.noneAndXrange(-l, l):
                        step = -1
                        pos_start, pos_stop, pos_step = slice(start, stop, step).indices(l)
                        if pos_stop < 0:
                            pos_start, pos_stop = 0, 0
                        self.assertRaises(ValueError, islice, lst, pos_start, pos_stop, step)
                        self.assertRaises(ValueError, lazislice(lst).__getitem__, slice(start, stop, step))
    class LzSliceCase(unittest.TestCase):
        def testInstNonArrayRaises(self):
            self.assertRaises(AttributeError, lzslice, None, slice(None))
        def testInstArrayWithSlice(self):
            for l in xrange(4):
                lazy_(range(l))[:]
        def testInstArrayOne(self):
            for l in xrange(4):
                lzslice(range(l), 0)
        def testInstArrayTwo(self):
            for l in xrange(4):
                lzslice(range(l), 0, 0)
        def testInstArrayThree(self):
            for l in xrange(4):
                lzslice(range(l), 0, 0, 1)
        def commonMkSquare(self, l):
            lst = [i*i for i in xrange(l)]
            lzlst_s = lazy_(lst)[:]
            lzlst_1 = lzslice(lst, None)
            lzlst_2 = lzslice(lst, 0, None)
            lzlst_3 = lzslice(lst, 0, None, 1)
            return (lst, lzlst_s, lzlst_1, lzlst_2, lzlst_3)
        def testFullRange(self):
            for l in xrange(4):
                lst, lzlst_s, lzlst_1, lzlst_2, lzlst_3 = self.commonMkSquare(l)
                for truc in (lzlst_s, lzlst_1, lzlst_2, lzlst_3):
                    self.assertEqual(all(truc[i] == lst[i] for i in xrange(-l, l)), True)
        def testStartShift(self):
            for l in xrange(10):
                lst = [i*i for i in xrange(l)]
                for st in xrange(l+1):
                    offislice = lst[st:]
                    lzsl_1 = lazy_(lst)[st:]
                    lzsl_2 = lazy_(lst)[st::]
                    for truc in (lzsl_1, lzsl_2):
                        self.assertEqual(len(truc), len(offislice))
                        self.assertEqual(all(truc[i] == offislice[i] for i in xrange(len(offislice))), True)
        def testStopShift(self):
            for l in xrange(10):
                lst = [i*i for i in xrange(l)]
                for st in xrange(l+1):
                    offislice = lst[:st]
                    lzsl_1 = lazy_(lst)[:st]
                    lzsl_2 = lazy_(lst)[:st:]
                    for truc in (lzsl_1, lzsl_2):
                        self.assertEqual(len(truc), len(offislice))
                        self.assertEqual(all(truc[i] == offislice[i] for i in xrange(len(offislice))), True)
        def testStepShift(self):
            for l in xrange(10):
                lst = [i*i for i in xrange(l)]
                for st in xrange(2*l+1):
                    if st-l+1 == 0:
                        continue
                    offislice = lst[::st-l+1]
                    lzsl_1 = lazy_(lst)[::st-l+1]
                    self.assertEqual(len(lzsl_1), len(offislice))
                    self.assertEqual(all(lzsl_1[i] == offislice[i] for i in xrange(len(offislice))), True)
        def testStep0Raises(self):
            for l in xrange(10):
                lst = [i*i for i in xrange(l)]
                self.assertRaises(ValueError, lazy_(lst).__getitem__, slice(None, None, 0))
                self.assertRaises(ValueError, lzslice, lst, slice(None, None, 0))
        def testSliceNonInt(self):
            for l in xrange(10):
                lst = [i*i for i in xrange(l)]
                self.assertRaises(TypeError, lazy_(lst).__getitem__, slice(1.1, None, None))
                self.assertRaises(TypeError, lazy_(lst).__getitem__, slice(None, 1.1, None))
                self.assertRaises(TypeError, lazy_(lst).__getitem__, slice(None, None, 1.1))
                self.assertRaises(TypeError, lzslice, lst, slice(1.1, None, None))
                self.assertRaises(TypeError, lzslice, lst, slice(None, 1.1, None))
                self.assertRaises(TypeError, lzslice, lst, slice(None, None, 1.1))
        def testAttrAssignmentRaises(self):
            for l in xrange(10):
                lst = [i*i for i in xrange(l)]
                lzsl_1 = lazy_(lst)[:]
                lzsl_1.obj
                lzsl_1.slicer
                self.assertRaises(TypeError, lzsl_1.__setattr__, "obj", 42)
                self.assertRaises(TypeError, lzsl_1.__setattr__, "slicer", 42)
        def testFullRangeIter(self):
            for l in xrange(10):
                lst, lzlst_s, lzlst_1, lzlst_2, lzlst_3 = self.commonMkSquare(l)
                for le, lse, l1e, l2e, l3e in zip(lst, lzlst_s, lzlst_1, lzlst_2, lzlst_3):
                    self.assertEqual(le, lse)
                    self.assertEqual(le, l1e)
                    self.assertEqual(le, l2e)
                    self.assertEqual(le, l3e)
                for truc in (lzlst_s, lzlst_1, lzlst_2, lzlst_3):
                    truciter = iter(truc)
                    for i in xrange(len(lst)):
                        truciter.next()
                    self.assertRaises(StopIteration, truciter.next)
        def commonCombi(self, cb_loop_slice):
            for l in xrange(6):
                lst = [i*i for i in xrange(l)]
                for start in xrange(l+1):
                    for stop in xrange(l+1):
                        for step in xrange(2*l+1):
                            if step-l == 0: continue
                            offislice = lst[start:stop:step-l]
                            lzsl = lazy_(lst)[start:stop:step-l]
                            cb_loop_slice(l, start, stop, step-l, offislice, lzsl, lst)
        def cbGetItem(self, l, start, stop, step, offislice, lzsl, lst):
            self.assertEqual(all(lzsl[i] == offislice[i] for i in xrange(len(offislice))), True)
        def testGetItem(self):
            self.commonCombi(self.cbGetItem)
        def cbGetLen(self, l, start, stop, step, offislice, lzsl, lst):
            self.assertEqual(len(lzsl), len(offislice))
        def testGetLen(self):
            self.commonCombi(self.cbGetLen)
        def cbIter(self, l, start, stop, step, offislice, lzsl, lst):
            itlzsl = iter(lzsl)
            for offiel in lzsl:
                el = itlzsl.next()
                self.assertEqual(el, offiel)
            self.assertRaises(StopIteration, itlzsl.next)
        def testCombiItem(self):
            self.commonCombi(self.cbIter)
        def cbLenChange(self, l, start, stop, step, offislice, lzsl, lst):
            lst.append(666)
            self.assertRaises(TypeError, lzsl.__iter__)
            self.assertRaises(TypeError, lzsl.__getitem__, 0)
            self.assertRaises(TypeError, lzsl.__setitem__, 0, 666)
            self.assertRaises(TypeError, lzsl.__cmp__, ())
        def testLenChange(self):
            self.commonCombi(self.cbLenChange)
        def cbSetItem(self, l, start, stop, step, offislice, lzsl, lst):
            (rstart, rstop, rstep) = slice(start, stop, step).indices(len(lst))
            for i in xrange(len(offislice)):
                lzsl[i] = -42 - i*i*i
            self.assertEqual(all(lst[i] == (-42 - x*x*x) for x, i in enumerate(xrange(rstart, rstop, rstep))), True)
        def testSetItem(self):
            self.commonCombi(self.cbSetItem)
        def commonGetGenCmpFactory(self, ):
            idt = lambda x:x
            Lst = range
            Str = lambda x:''.join(map(chr, xrange(x)))
            Tpl = lambda x:tuple(xrange(x))
            Wrp = lambda x:lazy_(x)[:]
            return idt, Lst, Str, Tpl, Wrp
        def combiCmpTypes(self, types_combination, wrap_filter, max_sz):
            for sz in xrange(max_sz):
                for facto_A, facto_B in types_combination:
                    base_A = facto_A(sz)
                    base_B = facto_B(sz)
                    for reverser_A in (1, -1):
                        for reverser_B in (1, -1):
                            for filter_A, filter_B in wrap_filter:
                                under_A = base_A[::reverser_A]
                                under_B = base_B[::reverser_B]
                                fin_A = filter_A(under_A)
                                fin_B = filter_B(under_B)
                                yield fin_A, fin_B, under_A, under_B, filter_A, filter_B
        def testCmpTypes(self):
            idt, Lst, Str, Tpl, Wrp = self.commonGetGenCmpFactory()
            types_combination = ((Lst, Str), (Lst, Tpl), (Str, Tpl))
            wrap_filter = (
              (idt, idt), #if this does not pass the test itself is false
              (Wrp, idt),
              (idt, Wrp),
              (Wrp, Wrp)
            )
            for a, b, un1, un2, un3, un4 in self.combiCmpTypes(types_combination, wrap_filter, 4):
                self.assertEqual(a < b, True)
                self.assertEqual(b > a, True)
        def commonCmpAssert(self, a, b, ua, ub):
            self.assertEqual(a == b, ua == ub)
            self.assertEqual(b == a, ub == ua)
            self.assertEqual(a < b, ua < ub)
            self.assertEqual(b < a, ub < ua)
            self.assertEqual(a > b, ua > ub)
            self.assertEqual(b > a, ub > ua)
        def testCmpContent(self):
            idt, Lst, Str, Tpl, Wrp = self.commonGetGenCmpFactory()
            types_combination = ((Lst, Lst), (Str, Str), (Tpl, Tpl))
            wrap_filter = (
              (idt, idt), #if this does not pass the test itself is false
              (Wrp, idt),
              (idt, Wrp),
              (Wrp, Wrp)
            )
            insert = (
              { str: chr(0), list: [0], tuple: (0,) },
              { str: chr(1), list: [1], tuple: (1,) },
              { str: chr(2), list: [2], tuple: (2,) },
              { str: chr(255), list: [255], tuple: (255,) }
            )
            for a, b, ua, ub, fa, fb in self.combiCmpTypes(types_combination, wrap_filter, 4):
                self.commonCmpAssert(a, b, ua, ub)
                for p in xrange(len(ua)):
                    for ins in insert:
                        new_ua = ua[:p-1] + ins[type(ua)] + ua[p+1:]
                        new_ub = ub[:p-1] + ins[type(ub)] + ub[p+1:]
                        new_fa, new_fb = fa(new_ua), fb(new_ub)
                        self.commonCmpAssert(new_fa, new_fb, new_ua, new_ub)
                for ins in insert:
                    new_ua = ua[:] + ins[type(ua)]
                    new_ub = ub[:]
                    new_fa, new_fb = fa(new_ua), fb(new_ub)
                    self.commonCmpAssert(new_fa, new_fb, new_ua, new_ub)
        def cbOutOfRange(self, l, start, stop, step, offislice, lzsl, lst):
            slen = len(offislice)
            for i in xrange(-slen-1, -slen-5, -1):
                self.assertRaises(IndexError, lzsl.__getitem__, i)
            for i in xrange(slen, slen+4, -1):
                self.assertRaises(IndexError, lzsl.__getitem__, i)
        def testOutOfRange(self):
            self.commonCombi(self.cbOutOfRange)
        def cbSetUnder(self, l, start, stop, step, offislice, lzsl, lst):
            (rstart, rstop, rstep) = slice(start, stop, step).indices(len(lst))
            for i in xrange(rstart, rstop, rstep):
                lst[i] = -666 - i*i*i/2
            self.assertEqual(all(lzsl[x] == -666 - i*i*i/2 for x, i in enumerate(xrange(rstart, rstop, rstep))), True)
        def testSetUnder(self):
            self.commonCombi(self.cbSetItem)
    class LzDoubleCase(unittest.TestCase):
        def iterStartStopStep(self, itstart, itstop, itstep):
            for start in itstart:
                for stop in itstop:
                    for step in itstep:
                        yield start, stop, step
        def noneMinMax(self, mini, maxi):
            yield None
            for i in xrange(mini, maxi):
                yield i
        def testDblSlice(self):
            Idt = lambda x:x
            filters = ((Idt, Idt), (Idt, lazy_), (lazy_, Idt), (lazy_, lazy_))
            for l in xrange(9):
                arr = range(l)
                for ext_start, ext_stop, ext_step in self.iterStartStopStep(
                      self.noneMinMax(-l, l-1),
                      self.noneMinMax(-l, l-1),
                      self.noneMinMax(-l, l)):
                    if ext_step == 0:
                        continue
                    for int_start, int_stop, int_step in self.iterStartStopStep(
                          self.noneMinMax(-l, l-1),
                          self.noneMinMax(-l, l-1),
                          self.noneMinMax(-l, l)):
                        if int_step == 0:
                            continue
                        trans = [fext(fint(arr)[int_start:int_stop:int_step])[ext_start:ext_stop:ext_step] for fext, fint in filters]
                        self.assertEqual(all(len(trans[0]) == len(trans[x]) for x in xrange(1, len(trans))), True)
                        self.assertEqual(all(trans[0] == trans[x] for x in xrange(1, len(trans))), True)
                        lent0 = len(trans[0])
                        for x in xrange(1, len(trans)):
                            itblah = iter(trans[x])
                            for i, e in enumerate(trans[0]):
                                self.assertEqual(e, itblah.next())
                            self.assertRaises(StopIteration, itblah.next)
    class FilterBase:
        FILTER = ((lambda x: x%5 == 0),)
        EMPTY  = []
        BEGIN  = [10, 11, 12, 13, 14]
        END    = [11, 12, 13, 14, 15]
        BOTH   = [10, 11, 12, 13, 14, 15]
        MIDDLE = [9, 10, 11, 12, 13, 14, 15, 16]
        NO     = [9, 11, 12, 13, 14, 16]
        ONLY   = [10, 15, 20, 25]
        BIG    = range(5, 555)
        FILTERED = [(unfiltered, [x for x in unfiltered if FILTER[0](x)])
                    for unfiltered in (EMPTY, BEGIN, END, BOTH, MIDDLE,
                                       NO, ONLY, BIG)]
    class ClassicFilter(FilterBase):
        def filterFactory(self, unfil):
            return FilteredView(self.FILTER[0], unfil)
    class BoundedFilter(FilterBase):
        def filterFactory(self, unfil):
            return FilteredView(self.FILTER[0], unfil, 0, len(unfil))
    class MyFiltered(FilterBase):
        def testIter(self):
            for unfil, fil in self.FILTERED:
                refil = self.filterFactory(unfil)
                self.assertEqual(all(x == y for (x, y) in izip(refil, fil)), True)
        def testIterLen(self):
            for unfil, fil in self.FILTERED:
                refil = self.filterFactory(unfil)
                self.assertEqual(all_and_count(True for x in refil), all_and_count(True for x in fil))
        def testLen(self):
            for unfil, fil in self.FILTERED:
                refil = self.filterFactory(unfil)
                self.assertEqual(len(refil), len(fil))
        def testGetItem(self):
            for unfil, fil in self.FILTERED:
                refil = self.filterFactory(unfil)
                self.assertEqual(all(refil[p] == fil[p] for p in xrange(-len(fil), len(fil))), True)
        def testGetItemBad(self):
            for unfil, fil in self.FILTERED:
                refil = self.filterFactory(unfil)
                self.assertRaises(TypeError, refil.__getitem__, 2.0)
                self.assertRaises(IndexError, refil.__getitem__, len(fil))
                self.assertRaises(IndexError, refil.__getitem__, -len(fil)-1)
        def commonSetItem(self, unfil, fil, start, stop):
            copy_unfil = unfil[:]
            copy_fil = fil[:]
            refil = self.filterFactory(copy_unfil)
            for p in xrange(start, stop):
                refil[p] = 1000000+p*5
                copy_fil[p] = 1000000+p*5
                self.assertEqual(refil[p], copy_fil[p])
            for orig, mod in izip(unfil, copy_unfil):
                if not self.FILTER[0](orig):
                    self.assertEqual(orig, mod)
        def testSetItem(self):
            for unfil, fil in self.FILTERED:
                self.commonSetItem(unfil, fil, -len(fil), 0)
                self.commonSetItem(unfil, fil, 0, len(fil))
        def testSetItemBad(self):
            for unfil, fil in self.FILTERED:
                refil = self.filterFactory(unfil[:])
                self.assertRaises(TypeError, refil.__setitem__, 2.0, 40)
                self.assertRaises(IndexError, refil.__setitem__, len(fil), 40)
                self.assertRaises(IndexError, refil.__setitem__, -len(fil)-1, 40)
        def commonDelItem(self, unfil, fil, start, stop):
            for p in xrange(start, stop):
                copy_unfil = unfil[:]
                copy_fil = fil[:]
                refil = self.filterFactory(copy_unfil)
                del refil[p]
                del copy_fil[p]
                self.assertEqual(list(refil), list(copy_fil))
                a, b, c = slice(p-3, p+3, 1).indices(len(copy_fil))
                a, b, c = (a-len(copy_fil), b-len(copy_fil), c)
                self.assertEqual(all(refil[p] == copy_fil[p] for p in xrange(a, b)), True)
                copy_notfil = [x for x in copy_unfil if not self.FILTER[0](x)]
                notfil = [x for x in unfil if not self.FILTER[0](x)]
                self.assertEqual(copy_notfil, notfil)
        def testDelItem(self):
            for unfil, fil in self.FILTERED:
                self.commonDelItem(unfil, fil, -len(fil), 0)
                self.commonDelItem(unfil, fil, 0, len(fil))
        def testDelItemBad(self):
            for unfil, fil in self.FILTERED:
                refil = self.filterFactory(unfil[:])
                self.assertRaises(TypeError, refil.__delitem__, 2.0)
                self.assertRaises(IndexError, refil.__delitem__, len(fil))
                self.assertRaises(IndexError, refil.__delitem__, -len(fil)-1)
        def testPrepend(self):
            for unfil, fil in self.FILTERED:
                if len(fil):
                    copy_unfil = unfil[:]
                    copy_fil = fil[:]
                    refil = self.filterFactory(copy_unfil)
                    refil.prepend(1000000)
                    self.assertEqual(refil[0], 1000000)
                    back = [x for x in copy_unfil if x != 1000000]
                    self.assertEqual(back, unfil)
        def testAppend(self):
            for unfil, fil in self.FILTERED:
                if len(fil):
                    copy_unfil = unfil[:]
                    copy_fil = fil[:]
                    refil = self.filterFactory(copy_unfil)
                    refil.append(1000000)
                    self.assertEqual(refil[-1], 1000000)
                    back = [x for x in copy_unfil if x != 1000000]
                    self.assertEqual(back, unfil)
        def testExtend(self):
            for unfil, fil in self.FILTERED:
                if len(fil):
                    copy_unfil = unfil[:]
                    copy_fil = fil[:]
                    refil = self.filterFactory(copy_unfil)
                    refil.extend(range(1000000, 1000050, 5))
                    for p, want in zip(range(-10, 0), range(1000000, 1000050, 5)):
                        self.assertEqual(refil[p], want)
                    back = [x for x in copy_unfil if x < 1000000]
                    self.assertEqual(back, unfil)
        def commonInsertBefore(self, unfil, fil, method_name, p):
            copy_unfil = unfil[:]
            copy_fil = fil[:]
            refil = self.filterFactory(copy_unfil)
            method = getattr(refil, method_name)
            method(p, 1000000)
            if p < 0:
                p -= 1
            self.assertEqual(refil[p], 1000000)
            back = [x for x in copy_unfil if x < 1000000]
            self.assertEqual(back, unfil)
        def testInsertBefore(self):
            for unfil, fil in self.FILTERED:
                for method_name in ('insert_before', 'insert'):
                    for p in xrange(-len(fil), len(fil)):
                        self.commonInsertBefore(unfil, fil, method_name, p)
        def testInsertAfter(self):
            for unfil, fil in self.FILTERED:
                for p in xrange(-len(fil), len(fil)):
                    copy_unfil = unfil[:]
                    copy_fil = fil[:]
                    refil = self.filterFactory(copy_unfil)
                    refil.insert_after(p, 1000000)
                    if p < 0:
                        p += len(fil)
                    self.assertEqual(refil[p+1], 1000000)
                    back = [x for x in copy_unfil if x < 1000000]
                    self.assertEqual(back, unfil)
        def testInsertBeyondLimits(self):
            for unfil, fil in self.FILTERED:
                if len(fil):
                    copy_unfil_append          = unfil[:]
                    copy_unfil_A_insert_after  = unfil[:]
                    copy_unfil_A_insert_before = unfil[:]
                    copy_unfil_A_insert        = unfil[:]
                    copy_unfil_prepend         = unfil[:]
                    copy_unfil_P_insert_after  = unfil[:]
                    copy_unfil_P_insert_before = unfil[:]
                    copy_unfil_P_insert        = unfil[:]
                    (refil_append         ,
                     refil_A_insert_after ,
                     refil_A_insert_before,
                     refil_A_insert       ,
                     refil_prepend        ,
                     refil_P_insert_after ,
                     refil_P_insert_before,
                     refil_P_insert       ) = map(self.filterFactory,
                    (copy_unfil_append         ,
                     copy_unfil_A_insert_after ,
                     copy_unfil_A_insert_before,
                     copy_unfil_A_insert       ,
                     copy_unfil_prepend        ,
                     copy_unfil_P_insert_after ,
                     copy_unfil_P_insert_before,
                     copy_unfil_P_insert       ))
                    refil_append.append(1000000)
                    refil_A_insert_after.insert_after(len(fil), 1000000)
                    refil_A_insert_before.insert_before(len(fil), 1000000)
                    refil_A_insert.insert(len(fil), 1000000)
                    self.assertEqual(refil_append[-1], 1000000)
                    self.assertEqual(refil_append[-1], refil_A_insert_after[-1])
                    self.assertEqual(refil_append[-1], refil_A_insert_before[-1])
                    self.assertEqual(refil_append[-1], refil_A_insert[-1])
                    refil_prepend.prepend(1000000)
                    refil_P_insert_after.insert_after(-len(fil)-1, 1000000)
                    refil_P_insert_before.insert_before(-len(fil)-1, 1000000)
                    refil_P_insert.insert(-len(fil)-1, 1000000)
                    self.assertEqual(refil_prepend[0], 1000000)
                    self.assertEqual(refil_prepend[0], refil_P_insert_after[0])
                    self.assertEqual(refil_prepend[0], refil_P_insert_before[0])
                    self.assertEqual(refil_prepend[0], refil_P_insert[0])
    class FilteredCase(unittest.TestCase, ClassicFilter, MyFiltered):
        def testBadAppend(self):
            for unfil, fil in self.FILTERED:
                if len(fil) == 0:
                    refil = self.filterFactory(unfil)
                    self.assertRaises(TypeError, refil.append, 1000000)
        def testBadPrepend(self):
            for unfil, fil in self.FILTERED:
                if len(fil) == 0:
                    refil = self.filterFactory(unfil)
                    self.assertRaises(TypeError, refil.prepend, 1000000)
        def testBadInsertBefore(self):
            for unfil, fil in self.FILTERED:
                if len(fil) == 0:
                    refil = self.filterFactory(unfil)
                    self.assertRaises(TypeError, refil.insert_before, 1000, 1000000)
                    self.assertRaises(TypeError, refil.insert_before, -1000, 1000000)
        def testBadInsertAfter(self):
            for unfil, fil in self.FILTERED:
                if len(fil) == 0:
                    refil = self.filterFactory(unfil)
                    self.assertRaises(TypeError, refil.insert_after, 1000, 1000000)
                    self.assertRaises(TypeError, refil.insert_after, -1000, 1000000)
    class BoundedFilteredCase(unittest.TestCase, BoundedFilter, MyFiltered):
        def testBoundedAppend(self):
            for unfil, fil in self.FILTERED:
                if len(fil) == 0:
                    copy_unfil = unfil[:]
                    refil = self.filterFactory(copy_unfil)
                    refil.append(1000000)
                    self.assertEqual(refil[-1], 1000000)
        def testBoundedPrepend(self):
            for unfil, fil in self.FILTERED:
                if len(fil) == 0:
                    copy_unfil = unfil[:]
                    refil = self.filterFactory(copy_unfil)
                    refil.prepend(1000000)
                    self.assertEqual(refil[0], 1000000)
        def testBoundedInsertBefore(self):
            for unfil, fil in self.FILTERED:
                if len(fil) == 0:
                    copy_unfil = unfil[:]
                    refil = self.filterFactory(copy_unfil)
                    refil.insert_before(0, 1000000)
                    self.assertEqual(refil[0], 1000000)
        def testBoundedInsertAfter(self):
            for unfil, fil in self.FILTERED:
                if len(fil) == 0:
                    copy_unfil = unfil[:]
                    refil = self.filterFactory(copy_unfil)
                    refil.insert_after(0, 1000000)
                    self.assertEqual(refil[-1], 1000000)
    unittest.main()
