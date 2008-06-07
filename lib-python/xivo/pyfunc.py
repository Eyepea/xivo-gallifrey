"""Supporting functions to ease functional programming in Python

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

from itertools import *

from xivo.UpAllAny import all, any

def all_and_count(seq):
	"""Simultaneously check that all items of the given sequence evaluates
	to true and count the number of items. If needed you can distinguish
	between 0 and False with the "is" operator, or you can also do arithmetic
	on the result considering that the value of False is zero.
	
	>>> all_and_count(())
	0
	>>> all_and_count((1,))
	1
	>>> all_and_count((1,1))
	2
	>>> all_and_count((0,))
	False
	>>> all_and_count((1,0))
	False
	
	"""
	cnt = -1
	for cnt, item in enumerate(seq):
		if not item:
			return False
	return cnt + 1

def nth_raw(seq, pos):
	try:
		return seq[pos]
	except (AttributeError, TypeError):
		return islice(seq, pos, pos+1).next()

def nth(seq, pos):
	"""Returns the element at the given position of the sequence, or
	None if seq is not large enough.
	
	- seq is the sequence
	- pos is the position, must be a positive or null integer """
	try:
		return seq[pos]
	except IndexError:
		return None
	except (AttributeError, TypeError):
		try:
			return islice(seq, pos, pos+1).next()
		except StopIteration:
			return None

def first(seq):
	return nth(seq, 0)

def last(seq):
	try:
		return seq[-1]
	except IndexError:
		return None
	except (AttributeError, TypeError):
		elt = None
		for elt in seq:
			pass
		return elt

def find(f, seq):
	"""Return first item in sequence where f(item) evaluates to True,
	or returns None."""
	for item in seq:
		if f(item): 
			return item

def flatten_list(seq_of_seq):
	# return reduce(operator.concat, seq_of_lists, [])
	# The previous line would have been absolutely pure and perfect
	# but its at least of order O(n^2), so:
	lst = []
	for one_seq in seq_of_seq:
		lst.extend(one_seq)
	return lst

def flatten_seq(seq_of_seq):
	for seq in seq_of_seq:
		for x in seq:
			yield x

def at_least(num, check_func, seq):
	"""Returns True if at least the first 'num' items of 'seq' are valids
	as checked by 'check_func', otherwise - i.e. there is less than 'num'
	items in the sequence or at least one of the first 'num' ones has a
	value such as 'check_func(item)' evaluates to False - returns False."""
	if num < 1:
		return True
	cnt = -1
	for cnt, item in enumerate(seq):
		if not check_func(item):
			return False
		if cnt + 1 >= num:
			return True
	return cnt + 1 >= num

def split_pad(s, maxsplit, sep=None, pad=None):
	"""Split 's' at most 'maxsplit' times (that is in at most maxsplit+1
	parts) and pad it until there is maxsplit + 1 parts."""
	split = s.split(sep, maxsplit)
	l = len(split)
	if l < maxsplit + 1:
		split.extend([pad] * (maxsplit + 1 - l))
	return split

def unsplit_none(strseq, sep=''):
	"""Join elements of strseq that are not None, using the value
	of sep as separator. """
	return sep.join(x for x in strseq if x is not None)

def replace_keys(dico, repl_dico):
	"return dict(((repl_dico[k],v) for (k,v) in dico.iteritems()))"
	return dict(((repl_dico[k], v) for (k, v) in dico.iteritems()))
