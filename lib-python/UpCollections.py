# This module magically adds NamedTuple (standard Python 2.6) to the 
# collections module when it's imported.
#
# Code comes from http://svn.python.org/projects/python/trunk/Lib/collections.py
# at revision 56130, then lightly modified so its usable with python 2.4 and
# maybe 2.5. Under the Python License.

# Proformatique version control:
__version__ = "$Revision$ $Date$"

from operator import itemgetter as _itemgetter

import collections as _collections
try:
    _collections.NamedTuple
except AttributeError:
    def NamedTuple(typename, s):
        """Returns a new subclass of tuple with named fields.

        >>> Point = NamedTuple('Point', 'x y')
        >>> Point.__doc__           # docstring for the new class
        'Point(x, y)'
        >>> p = Point(11, y=22)     # instantiate with positional args or keywords
        >>> p[0] + p[1]             # works just like the tuple (11, 22)
        33
        >>> x, y = p                # unpacks just like a tuple
        >>> x, y
        (11, 22)
        >>> p.x + p.y               # fields also accessable by name
        33
        >>> p                       # readable __repr__ with name=value style
        Point(x=11, y=22)

        """

        field_names = s.split()
        if not ''.join([typename] + field_names).replace('_', '').isalnum():
            raise ValueError('Type names and field names can only contain alphanumeric characters and underscores')
        argtxt = ', '.join(field_names)
        reprtxt = ', '.join('%s=%%r' % name for name in field_names)
        template = '''class %(typename)s(tuple):
            '%(typename)s(%(argtxt)s)'
            __slots__ = ()
            def __new__(cls, %(argtxt)s):
                return tuple.__new__(cls, (%(argtxt)s,))
            def __repr__(self):
                return '%(typename)s(%(reprtxt)s)' %% self
        ''' % locals()
        for i, name in enumerate(field_names):
            template += '\n        %s = property(itemgetter(%d))\n' % (name, i)
        m = dict(itemgetter=_itemgetter)
        exec template in m
        result = m[typename]
        result.__module__ = 'collections'
        return result

    _collections.NamedTuple = NamedTuple

__all__ = []



if __name__ == '__main__':
    # verify that instances are pickable
    from cPickle import loads, dumps
    Point = NamedTuple('Point', 'x y')
    p = Point(x=10, y=20)
    assert p == loads(dumps(p))

    import doctest
    TestResults = NamedTuple('TestResults', 'failed attempted')
    print TestResults(*doctest.testmod())
