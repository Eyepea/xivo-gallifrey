"""Helper functions for XIVO Sysconf

Copyright (C) 2010  Proformatique

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2010  Proformatique

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

import re


def castint(s):
    if str(s).isdigit():
        return int(s)
    else:
        return s

def splitint(s):
    return map(castint, re.findall(r'(\d+|\D+)', str(s)))

def natsort(a, b):
    return cmp(splitint(a), splitint(b))

def is_scalar(var):
    """ Returns True if is scalar or False otherwise """
    return isinstance(var, (basestring, bool, int, float))

def extract_scalar_from_list(xlist):
    """ Extract scalar values from a list or tuple """
    return [x for x in xlist if is_scalar(x)]

def extract_scalar_from_dict(xdict):
    """ Extract scalar values from a dict natural ordered by key """
    return [xdict[key] for key in sorted(xdict.iterkeys(), natsort)
                            if is_scalar(xdict[key])]

def extract_scalar(var):
    """
    Extract scalar from tuple, list and dict
    Return tuple of scalar values
    """
    if isinstance(var, (tuple, list)):
        return tuple(extract_scalar_from_list(var))
    elif isinstance(var, dict):
        return tuple(extract_scalar_from_dict(var))
    elif is_scalar(var):
        return (var,)
    else:
        return

def extract_exists_in_list(var, xlist):
    """ Test if all elements in a variable exists in a list """
    if not isinstance(xlist, (tuple, list)):
        return False

    if is_scalar(var):
        if var in xlist:
            return (var,)
        else:
            return
    elif isinstance(var, dict):
        var = var.keys()
    elif not isinstance(var, (tuple, list)):
        return False

    return tuple(set(x for x in var if x in xlist)) or None

def exists_in_list(var, xlist):
    """ Test if all elements in a variable exists in a list """
    if not isinstance(xlist, (tuple, list)):
        return False

    if is_scalar(var):
        return var in xlist
    elif isinstance(var, dict):
        var = var.keys()
    elif not isinstance(var, (tuple, list)):
        return False

    for x in var:
        if x not in xlist:
            return False

    return True

def unique_case_tuple(sequence):
    """ Build an ordered case-insensitive collection """
    xlist = dict(zip(map(str.lower, sequence), sequence)).values()
    return tuple([x for x in sequence if x in xlist])

def combine_dict(dict1, dict2):
    """ Creates a dict by using one array for keys and another for its values """
    if not isinstance(dict1, dict) or not isinstance(dict2, dict):
        return False

    ret = {}

    for key, value in dict1.iteritems():
        if dict2.has_key(key):
            ret[value] = dict2[key]

    return ret
