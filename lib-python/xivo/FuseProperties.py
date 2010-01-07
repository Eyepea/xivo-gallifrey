"""Fuse Protected Properties

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

from operator import attrgetter as _attrgetter

def _burn_fuse(attrname):
    def burn_fuse(self, v):
        if v:
            setattr(self, attrname, True)
        elif getattr(self, attrname):
            raise AttributeError, "you think you can just replace a burned fuse like that?"
    return burn_fuse

def _set_fused_protected(attrname, fusename, fv = lambda x: x): # pylint: disable-msg=E0602
    def set_fused_bool(self, v):
        if not getattr(self, fusename):
            setattr(self, attrname, fv(v))
        else:
            raise AttributeError, "can't set attribute: burned fuse"
    return set_fused_bool

def _set_fused_multiple(mainfuse_name, subfuses_seq):
    def set_fused_multiple(self, v):
        if v:
            setattr(self, mainfuse_name, True)
            for f in subfuses_seq:
                setattr(self, f, True)
        elif getattr(self, mainfuse_name):
            raise AttributeError, "you think you can just replace a burned fuse like that?"
    return set_fused_multiple

def prop_fuse(attrname):
    return property(_attrgetter(attrname), _burn_fuse(attrname))

def prop_fused_bool(attrname, fusename):
    return property(_attrgetter(attrname),
                    _set_fused_protected(attrname, fusename, bool))

def prop_fused_access(attrname, fusename):
    return property(_attrgetter(attrname),
                    _set_fused_protected(attrname, fusename))

def prop_fuse_multiple(mainfuse_name, subfuses_seq):
    return property(_attrgetter(mainfuse_name),
                    _set_fused_multiple(mainfuse_name, subfuses_seq))
