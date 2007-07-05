"""Graph theory routines

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

def list_loop(prev_dict, start, v):
	a = [v, start]
	while prev_dict[v]:
		v = prev_dict[v]
		a.insert(0, v)
	return a

def loop(edges, start):
	"""Finds and returns a loop containing the vertex identified by the 
	parameter start in the graph described by parameter edges, or returns
	None if no such loop exists.
	
	Parameter edges must be a mapping object (in the Python sense) and have
	the following structure:
	  { v_i: AN(v_i), ... } where v_i scans the full set of vertices
	  identifiers of the graph and AN(v_i) is a stable Python iterable
	  (can be - non exhaustively - a list or a tuple) which contains the
	  full set of vertices that are adjacent to v_i. Every vertex of the
	  graph _MUST_ appear in a key of edges, particularly those that appear
	  in any AN(v_i). If such a v_x exists so that AN(v_i) is empty then a
	  v_x:empty_iterable entry must exists in the dictionary (ex: v_x:() )
	ex: The internal representation of:
	
	  a -> b <- c -> d -> k        l -> m -> n -> o
	       |         ^                  ^    |
	       v         |                  |    v
	       e --------+    j             p <- q
	
	could be (with an obvious way of identifying vertices):
	
	  { 'a': ('b',),    'b': ('e',),    'c': ('b','d'), 'd': ('k',),
	    'e': ('d',),    'k': (),        'j': (),        'l': ('m',),
	    'm': ('n',),    'n': ('o','q'), 'o': (),        'p': ('m',),
	    'q': ('p',) }
	
	Vertices identifiers put in the parameter start, in keys of the
	parameter edges and in elements of iterables in values of the latter
	must be hashable.
	
	Practical example:
	>>> graph = { 'a': ('b',),    'b': ('e',),    'c': ('b','d'), 'd': ('k',),
	...         'e': ('d',),    'k': (),        'j': (),        'l': ('m',),
	...         'm': ('n',),    'n': ('o','q'), 'o': (),        'p': ('m',),
	...         'q': ('p',) }
	>>> for x in 'abcdekjlmnopq':
	...     print x, '----------------------------'
	...     print loop(graph, x)
	...     print  
	... 
	a ----------------------------
	None

	b ----------------------------
	None

	c ----------------------------
	None

	d ----------------------------
	None

	e ----------------------------
	None

	k ----------------------------
	None

	j ----------------------------
	None

	l ----------------------------
	None

	m ----------------------------
	['m', 'n', 'q', 'p', 'm']

	n ----------------------------
	['n', 'q', 'p', 'm', 'n']

	o ----------------------------
	None

	p ----------------------------
	['p', 'm', 'n', 'q', 'p']

	q ----------------------------
	['q', 'p', 'm', 'n', 'q']

	>>> 

	"""
	vertices_done = set()
	vertices_todo_lst = [start]
	vertices_todo_set = set(vertices_todo_lst)
	prev_dict = {start: None}
	while vertices_todo_lst:
		v = vertices_todo_lst.pop(0)
		vertices_todo_set.discard(v)
		vertices_done.add(v) # early so v->v edges won't bother us
		for d in edges[v]:
			if d == start:
				return list_loop(prev_dict, start, v)
			if (d not in vertices_done) and (d not in vertices_todo_set):
				prev_dict[d] = v
				vertices_todo_lst.append(d)
				vertices_todo_set.add(d)

__all__ = ['loop']
