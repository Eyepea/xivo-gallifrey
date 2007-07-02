"""Core representation of a validating supertree of computer resources

Copyright (C) 2007, Proformatique

"""

__version__ = "$Revision$ $Date"
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

from pyfunc import *

# NOTE: path must be a tuple
# TODO: a generic fenter which call fvisit() when visitable(), then
# overloadable methods ?

class RT_node(object):
	def __init__(self, ropen = True):
		# Set r(oot)open to True if the root can be visited when
		# no more path is defined
		# Note: you can also overload the visitable() method to
		# provide more dynamic decisioning
		self._ropen = bool(ropen)

	def visitable(self, ctx, path, pos):
		# If this function returns True, root of the current subtree
		# will immediately be visited by entering fvisit without any
		# further scan of childs, even if this would be possible.
		return self._ropen and pos >= len(path)

	@staticmethod
	def fin(ctx, path, pos):
		# fin is a little special: it doesn't just return the ctx but
		# also a boolean that will result in early abortion when set to
		# False. So it's possible to navigate through the tree with a
		# None ctx, while both allowing early cut if necessary
		return (ctx, True)

	@staticmethod
	def fout(ctx, path, pos):
		# must just return None if ctx is None
		# (can also return non None so a node can substituate
		#  to missing child)
		return ctx

	@staticmethod
	def fvisit(ctx, path, pos):
		# either just returns None if ctx is None 
		# and None input ctx is unsupported, or generate
		# a standalone one thanks to path, pos
		return None

	def fenter_child(self, child_t, ctx, path, pos):
		ctx, cont = self.fin(ctx, path, pos)
		if not cont:
			return None
		else:
			return self.fout(child_t.fenter(ctx, path, pos + 1), path, pos)

	def fenter(self, ctx, path, pos):
		# Can always access path[pos-1] (our name or None if root) 
		# and path[pos-2] (name of the parent or None if we or parent
		# root)
		if self.visitable(ctx, path, pos):
			return self.fvisit(ctx, path, pos)
		else:
			return None

	# From Root only
	def ctx_from_scratch(self, path):
		return self.fenter(None, (None,None)+path, 2)

class RT_Set(RT_node):
	def __init__(self, ropen, odict_rt):
		# odict_rt is an ordered dict where keys are node name and
		# values instances of RT_node (or a subclass)
		self._odict_rt = odict_rt
		super(RT_Set, self).__init__(ropen)

	def fenter(self, ctx, path, pos):
		if self.visitable(ctx, path, pos):
			return self.fvisit(ctx, path, pos)
		elif (pos < len(path)) and (path[pos] in self._odict_rt):
			return self.fenter_child(self._odict_rt[path[pos]], ctx, path, pos)
		else:
			return None

class RT_Dyn(RT_node):
	def __init__(self, ropen, args_val, out_rt):
		self._args_val = list(args_val) # positional node name validator
		self._out_rt = out_rt
		super(RT_Dyn, self).__init__(ropen)

	def fenter(self, ctx, path, pos):
		if self.visitable(ctx, path, pos):
			return self.fvisit(ctx, path, pos)
		elif (len(path) - pos >= len(self._args_val)
		      and all(val(path[pos+i])
		              for i,val in enumerate(self._args_val))):
			return self.fenter_child(self._out_rt, ctx, path, pos)
		else:
			return None

__all__ = ['RT_node', 'RT_Set', 'RT_Dyn']
