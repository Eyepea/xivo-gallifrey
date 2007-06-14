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

# any() and all() are available in Python 2.5, we target Python 2.4
try:
	all
except NameError:
	def all(iterable):
		for element in iterable:
			if not element:
				return False
		return True

try:
	any
except NameError:
	def any(iterable):
		for element in iterable:
			if element:
				return True
		return False

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
	c = -1
	for c, item in enumerate(seq):
		if not item:
			return False
	return c+1

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

def at_least(num, check_func, seq):
	"""Returns True if at least the first 'num' items of 'seq' are valids
	as checked by 'check_func', otherwise - i.e. there is less than 'num'
	items in the sequence or at least one of the first 'num' ones has a
	value such as 'check_func(item)' evaluates to False - returns False."""
	if num < 1:
		return True
	count = -1
	for count, item in enumerate(seq):
		if not check_func(item):
			return False
		if count + 1 >= num:
			return True
	return count + 1 >= num
