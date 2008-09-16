"""Read-Only List Proxy that only allows calls to state-preserving methods

Copyright (C) 2007, 2008  Proformatique

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

class ROListProxy(object):

    __slots__ = ('__underlying',)

    ALLOWED = ('count', 'index')

    # GENERATE = ('__add__', '__contains__', '__eq__', '__ge__',
    #             '__getitem__', '__getslice__', '__gt__', '__hash__',
    #             '__iter__', '__le__', '__len__', '__lt__', '__mul__',
    #             '__ne__', '__repr__', '__reversed__', '__rmul__')

    def __init__(self, lst):
        self.__underlying = lst

    def __getattr__(self, a):
        if a not in self.ALLOWED:
            raise AttributeError, "%s object has no attribute %s" \
                    % (repr(type(self).__name__), repr(a))
        return getattr(self.__underlying, a)

    # I didn't succeed in writing the following in a more generic way
    # so i fallback to that. If one does, there is no problem having some
    # generic code instead (it would indeed be better)

    def __add__(self, *args):
        return self.__underlying.__add__(*args)
    def __contains__(self, *args):
        return self.__underlying.__contains__(*args)
    def __eq__(self, *args):
        return self.__underlying.__eq__(*args)
    def __ge__(self, *args):
        return self.__underlying.__ge__(*args)
    def __getitem__(self, *args):
        return self.__underlying.__getitem__(*args)
    def __getslice__(self, *args):
        return self.__underlying.__getslice__(*args)
    def __gt__(self, *args):
        return self.__underlying.__gt__(*args)
    def __hash__(self, *args):
        return self.__underlying.__hash__(*args)
    def __iter__(self, *args):
        return self.__underlying.__iter__(*args)
    def __le__(self, *args):
        return self.__underlying.__le__(*args)
    def __len__(self, *args):
        return self.__underlying.__len__(*args)
    def __lt__(self, *args):
        return self.__underlying.__lt__(*args)
    def __mul__(self, *args):
        return self.__underlying.__mul__(*args)
    def __ne__(self, *args):
        return self.__underlying.__ne__(*args)
    def __repr__(self, *args):
        return self.__underlying.__repr__(*args)
    def __reversed__(self, *args):
        return self.__underlying.__reversed__(*args)
    def __rmul__(self, *args):
        return self.__underlying.__rmul__(*args)
