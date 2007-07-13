"""Fuse Protected Properties

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

from operator import attrgetter as _attrgetter

def _burn_fuse(attrname):
	def burn_fuse(self, v):
		if v: object.__setattr__(self, attrname, True)
	return burn_fuse

def _set_fused_protected(attrname, fusename, fv = lambda x:x):
	def set_fused_bool(self, v):
		if not object.__getattr__(self, fusename):
			object.__setattr__(self, attrname, fv(v))
		else:
			raise AttributeError, "can't set attribute: burned fuse"
	return set_fused_bool

def prop_fuse(attrname):
	return property(_attrgetter(attrname), _burn_fuse(attrname))

def prop_fused_bool(attrname, fusename):
	return property(_attrgetter(attrname),
		        _set_fused_protected(attrname, fusename, bool))

def prop_fused_access(attrname, fusename):
	return property(_attrgetter(attrname),
		        _set_fused_protected(attrname, fusename))
