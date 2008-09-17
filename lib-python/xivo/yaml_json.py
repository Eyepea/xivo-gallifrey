"""YAML <-> JSON interoperability

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

def stringify_keys(obj):
    """
    In YAML, it is possible to code a dictionary with integers keys.
    In JSON, it is not.
    This function returns a *deep* copy of @obj except that integer keys are
    replaced by their decimal representation, ex:
    
    >>> stringify_keys([{'vs_0001': {0: 'static_0001'}}, 12])
    [{'vs_0001': {'0': 'static_0001'}}, 12]
    """
    if isinstance(obj, list):
        return map(stringify_keys, obj)
    elif isinstance(obj, dict):
        return dict(((str(k), stringify_keys(v)) for k, v in obj.iteritems()))
    else:
        return obj


def _try_int(s):
    "return int(s) if possible else s"
    try:
        return int(s)
    except (TypeError, ValueError):
        return s


def unstringify_keys(obj):
    """
    Returns a *deep* copy of @obj except that keys of dictionaries are
    converted to integers when possible.
    
    See also stringify_keys()
    
    WARNING: There is no warranty that
        obj == unstringify_keys(stringify_keys(obj)
    """
    if isinstance(obj, list):
        return map(unstringify_keys, obj)
    elif isinstance(obj, dict):
        return dict(((_try_int(k), unstringify_keys(v)) for k, v in obj.iteritems()))
    else:
        return obj
