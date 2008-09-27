"""JSON tree operators

Copyright (C) 2008  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008  Proformatique

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


EXISTS = object()


def _get_match(op, tree):
    """
    Return a list of (path, value) extracted from @tree that matches the
    pattern represented by @op.  
    
    last part of @op:
        '=': value is a subtree
        '!': value is EXISTS
        '!!':  value is the type of a subtree
    """
    curop = op[0]
    
    if curop == '=':
        return [((), tree)]
    elif curop == '!':
        return [((), EXISTS)]
    elif curop == '!!':
        return [((), type(tree))]
    
    remop = op[1:]
    
    if curop in ('*', '<', '>'):
        res = []
        if isinstance(tree, list):
            for p, elt in enumerate(tree):
                res.extend([((p,) + path, val) for (path, val) in _get_match(remop, elt)])
        elif isinstance(tree, dict):
            for k in sorted(tree.iterkeys()):
                res.extend([((k,) + path, val) for (path, val) in _get_match(remop, tree[k])])
        return res
    elif isinstance(curop, int):
        if isinstance(tree, list) and 0 <= curop < len(tree):
            return [((curop,) + path, val) for (path, val) in _get_match(remop, tree[curop])]
        else:
            return []
    elif isinstance(curop, basestring):
        if isinstance(tree, dict) and curop in tree:
            return [((curop,) + path, val) for (path, val) in _get_match(remop, tree[curop])]
        else:
            return []
    
    raise TypeError, "invalid curop %r" % curop


def _suppl_ok(op, k, tree, incl_metaop):
    """
    Test if it's ok for a path @k to not be in @tree while being in the
    other.  @incl_metaop is '<' or '>' depending on which metaop allows
    the presence of @k while considering a subtree of @tree.
    """
    node = tree
    for part, comp in zip(op, k):
        if (isinstance(node, list) and 0 <= comp < len(node)) \
           or (isinstance(node, dict) and comp in node):
            node = node[comp]
        else:
            return part == incl_metaop


def _int_comp(op, tree1, tree2):
    """
    @op: internal representation of the tree comparison relational operator
    @tree1: the left tree
    @tree2: the right tree
    NOTE: the algorithm can probably be optimized
          (that would imply not using _get_match() )
    """
    pv1 = _get_match(op, tree1)
    dpv2 = dict(_get_match(op, tree2))
    
    for k, val in pv1:
        if k in dpv2:
            if dpv2[k] != val:
                return False
            else:
                del dpv2[k]
        else:
            if not _suppl_ok(op, k, tree2, '>'):
                return False
    
    for k, val in dpv2.iteritems():
        if not _suppl_ok(op, k, tree1, '<'):
            return False
    
    return True


def _parse_op(op):
    """
    @op: string representation of the tree comparison relational operator
    Return the internal representation of @op.
    """
    res = []
    done = False
    for part in op.split('.'):
        
        if done or part == "":
            raise ValueError, "invalid operator %r" % op
        
        if part[-1] == "!" and part not in ('!!', '!'):
            part = part[:-1]
            exists_term = True
        else:
            exists_term = False
        
        try:
            part = int(part)
        except ValueError: # pylint: disable-msg=W0704
            pass
        
        res.append(part)
        
        if exists_term:
            part = '!'
            res.append(part)
        
        if part in ('=', '!', '!!'):
            done = True
    
    if not done:
        res.append('=')
    
    return tuple(res)


def relate(op, tree1, tree2):
    """
    @op: string representation of the tree comparison relational operator
    @tree1: the left tree
    @tree2: the right tree
    Return the truth value of the proposition:
        @tree1 @op @tree2
    """
    return _int_comp(_parse_op(op), tree1, tree2)


def relate_conj(ops, tree1, tree2):
    """
    Truth value of the proposition:
        for each op in ops, tree1 op tree2
    """
    for op in ops:
        if not relate(op, tree1, tree2):
            return False
    else:
        return True
