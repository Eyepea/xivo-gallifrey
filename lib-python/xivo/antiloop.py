"""Graph theory routines

Copyright (C) 2007-2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007-2010  Proformatique

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


def list_loop(prev_dict, start, v):
    a = [v, start]
    while prev_dict[v]:
        v = prev_dict[v]
        a.insert(0, v)
    return a


def loop(graph, start):
    """
    Find and return a loop containing the vertex identified by the
    parameter start in the graph described by parameter graph, or return
    None if no such loop exists.

    Parameter graph must be a mapping object (in the Python meaning) and
    have the following structure:
      { v_i: AN(v_i), ... } where v_i scans the full set of vertices
      identifiers of the graph and AN(v_i) is a stable Python iterable
      (can be - non exhaustively - a list or a tuple) which contains the
      full set of vertices that are adjacent to v_i.  Every vertex of the
      graph _MUST_ appear in a key of graph, particularly those that appear
      in any AN(v_i).  If such a v_x exists so that AN(v_i) is empty then a
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
    parameter graph and in elements of iterables in values of the latter
    must be hashable.

    Practical example:
    >>> graph = { 'a': ('b',),    'b': ('e',),    'c': ('b','d'), 'd': ('k',),
    ...           'e': ('d',),    'k': (),        'j': (),        'l': ('m',),
    ...           'm': ('n',),    'n': ('o','q'), 'o': (),        'p': ('m',),
    ...           'q': ('p',) }
    >>> for x in 'abcdekjlmnopq':
    ...     print x, '----------------------------'
    ...     print loop(graph, x)
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
    """
    vertices_done = set()
    vertices_todo_lst = [start]
    vertices_todo_set = set(vertices_todo_lst)
    prev_dict = {start: None}
    while vertices_todo_lst:
        v = vertices_todo_lst.pop(0)
        vertices_todo_set.discard(v)
        vertices_done.add(v) # early so v->v edges won't bother us
        for d in graph[v]:
            if d == start:
                return list_loop(prev_dict, start, v)
            if (d not in vertices_done) and (d not in vertices_todo_set):
                prev_dict[d] = v
                vertices_todo_lst.append(d)
                vertices_todo_set.add(d)


def reverse_ord(graph):
    """
    This functions works on an ordered graph and returns a new one.
    It can be viewed as doing diverse things, depending on the context:
      - returns a new graph with any edge direction reversed
      - returns a new representation of graph, but with the destination of
        edges in keys of the Python mapping, and sources for a given
        destination in a list referenced by the corresponding value of the
        mapping
    """
    rev = {}
    for s, dsts in graph.iteritems():
        rev.setdefault(s, [])
        for d in dsts:
            srcs = rev.setdefault(d, [])
            srcs.append(s)
    return rev


def potential_by_card(graph):
    """
    This function returns a dictionary where a potential is associated
    with each vertex of the graph.  The potential is the number of node
    that are adjacent to the considered one.
    """
    return dict(((v1, len(lst_v2)) for v1, lst_v2 in graph.iteritems()))


# In consolidate_childs(), consolidate_conso_reach() and
# partial_order_from_reversed_ord_pot(), conso_reach is a Python mapping which
# contains the following key -> value associations:
# k: [last_consolidation, list_of_known_reachable_vertices,
#                         list_of_adjacents_but_not_yet_traversed_vertices]

def consolidate_childs(conso_reach, s, seq):
    l1, l2 = len(conso_reach[s][1]), len(conso_reach[s][2])
    for r in conso_reach[s][1]:
        if conso_reach[r][0] < 0:
            consolidate_childs(conso_reach, r, seq)
        if conso_reach[s][0] < conso_reach[r][0]:
            conso_reach[s][1].update(conso_reach[r][1])
    for r in conso_reach[s][2]:
        if conso_reach[r][0] < 0:
            consolidate_childs(conso_reach, r, seq)
        conso_reach[s][1].add(r)
        conso_reach[s][1].update(conso_reach[r][1])
    conso_reach[s][2] = set()
    if (l1, l2) != (len(conso_reach[s][1]), len(conso_reach[s][2])) \
       or conso_reach[s][0] == -1:
        conso_reach[s][0] = seq
        return True
    else:
        return False


def consolidate_conso_reach(conso_reach, start, seq):
    """
    This function returns True if there were changes other than nodes
    from [2] to [1] with no node in [2] being the origin of any edge
    This behavior is dictated by the place it's used in
    partial_order_from_reversed_ord_pot()

    Remaining parts of the behavior of this function come from the final
    call of consolidate_childs()
    """
    optim_neigh_to_reach = [n for n in conso_reach[start][2] if not conso_reach[n][0]]
    if optim_neigh_to_reach:
        conso_reach[start][1].update(optim_neigh_to_reach)
        conso_reach[start][2].difference_update(optim_neigh_to_reach)
    r = consolidate_childs(conso_reach, start, seq)
    if optim_neigh_to_reach:
        conso_reach[start][0] = seq
    return r


def partial_order_from_reversed_ord_pot(rev_graph, v_pot):
    """
    This function takes a "reversed" ordered graph (see reverse_ord()
    and its description) and a Python mapping that associates a potential
    to each vertex, and returns a tuple which contains one with no
    loop (in a "forward" format) as its first element and a list of
    deleted edges where the representation of an edge is a tuple
    (source_vx, destination_vx).
    The work is not done in place so neither rev_graph nor v_pot are
    modified.
    The partial order is created by removal of the smallest possible number
    of edges from the original graph.  Edges that are in a loop are
    preferred for removal when they have the smallest possible "tension",
    where tension is defined by the potential difference between the
    destination and the source of the edge.  When multiple possibilities
    exist because the same tension is present multiple times, the choice
    of an edge for removal shall be considered as an arbitrary one.
    NOTE: in the code, opposite of the "tension" is used so
    v_pot[s] - v_pot[d] is calculated and edges where this value is bigger
    are preferred for removal.
    The algorithm saves known reachables vertices from each vertex in
    conso_reach, as well as not yet traversed neighbours.  The decision
    to keep or remove a given edge is taken by just checking if it would
    introduce a loop in the graph, and this is done as follow:
      - if nothing reaches the source vertex or the destination vertex
        reaches nothing, adding the considered edge can't introduce a loop;
          O(log(E))
      - if the source is already known to be reachable from the destination
        (of considered edge) because its either in the set of known
        reachables or in the set of not yet traversed neighbours of
        the destination, adding the edge would introduce a loop;
          O(log(E))
      - but if the source has not been found in the previous lists, that
        does not guarantee it's not reachable from destination, because
        children could have been modified after the last update of the
        reachability and not traversed neighbours sets of the destination
        vertex.  So all reachables from the destination vertex are
        recursively traversed if it has not yet been done since their last
        respective neighbor insertion, and during recursion the parent is
        updated with 1-transitive reachables after all necessary updates
        of childs took place.
          Amortized for whole graph worst case complexity remains to
          be calculated, but as only absolutely needed operations are
          performed and stored for future use at any point of this
          algorithm, it should not be to hard.  Maybe implementation is a
          little slower because of internal Python data structures that
          could be potentially improved for this algorithm)
      - The previous update is done by consolidate_conso_reach() which
        returns True if something has changed for the destination vertex,
        so in this case a second lookup is done to check for presence of
        the source vertex in the up-to-date reachability set of the
        destination vertex.  Found implies drop the considered edge - not
        found implies keep it.
          O(log(E))
    An initial sort of edges weight is performed: O(V.log(V))
    For data sets we intend to use, complexity will probably not raise
    above O(V.log(E)).  Amortized complexity for worst case scenarios
    remains to be calculated, but would only occur on quite dense graphs,
    so something a little above O(E^2) won't be a real problem :)
    """
    graph_po = dict(((k, []) for k in rev_graph.iterkeys()))
    conso_reach = dict(((k, [0, set(), set()]) for k in rev_graph.iterkeys()))
    stable_prios = {}
    sorted_prios = []
    deletion_list = []
    for d, srcs in rev_graph.iteritems():
        for s in srcs:
            w = v_pot[s] - v_pot[d]
            sorted_prios.append(w)
            lst = stable_prios.setdefault(w, [])
            lst.append((s, d))
    sorted_prios.sort()
    seq = 0
    while sorted_prios:
        seq += 1
        prio = sorted_prios.pop(0)
        (s, d) = stable_prios[prio].pop(0)
        if rev_graph[s] and conso_reach[d][0] \
           and ((s in conso_reach[d][1]) or (s in conso_reach[d][2]) \
                or (consolidate_conso_reach(conso_reach, d, seq)
                    and s in conso_reach[d][1])):
            deletion_list.append((s, d))
            continue
        graph_po[s].append(d)
        conso_reach[s][2].add(d)
        conso_reach[s][0] = -1
    return (graph_po, deletion_list)


def partial_order_sink(graph):
    """
    Given an ordered graph (see loop() for a description of its
    representation) return a partial order, that is remove the necessary
    number of edges from graph (this is not done in-place, a copy is
    created).
    This function indeed returns a tuple with the partial order in its
    first argument and the list of "deleted" edges as described in
    the documentation of partial_order_from_reversed_ord_pot().
    """
    rev_graph = reverse_ord(graph)
    return partial_order_from_reversed_ord_pot(rev_graph, potential_by_card(rev_graph))


def _test():
    import doctest
    doctest.testmod()


if __name__ == "__main__":
    _test()


__all__ = ['loop', 'partial_order_sink']

# XXX unit tests for everything and stats with big graphs for
# partial_order_from_reversed_ord_pot()

# idea of evil graph:
# g = { 'n1': ('n2', 'n3', 'i1', 'i2', 'i3'),
#       'n2': ('n1', 'n3', 'j1', 'j2', 'j3'),
#       'n3': ('n1', 'n2', 'k1', 'k2', 'k3'),
#       'i1': ('n2',), 'i2': ('n2',), 'i3': ('n2',),
#       'j1': ('n3',), 'j2': ('n3',), 'j3': ('n3',),
#       'k1': ('n1',), 'k2': ('n1',), 'k3': ('n1',),
#       'a': ('b',), 'b': ('c',), 'c': ('a',),
#       'z': ('i1',) }
