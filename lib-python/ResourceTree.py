"""Core representation of a validating supertree of computer resources

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

from pyfunc import *

# NOTE: path must be a tuple
# TODO: a generic fenter which call fvisit() when visitable(), then
# overloadable methods ?

class RT_node(object):

	"""Both base class of all Resource Trees, and possible near
	complete implementation of leaves nodes (fvisit() just need to
	be overridden. """

	def __init__(self, ropen = True):
		"""When ropen is True, if there is no subpath remaining when this
		node is being scanned to look for a resource, it'll be visited
		(by call of the fvisit() method). This behavior is dictated by
		the visitable() so it can be modified by overloading it. """
		self._ropen = bool(ropen)

	def visitable(self, ctx, path, pos):
		"""If this function returns True, this node will immediately
		be visited by entering fvisit() regardless of whether
		path[pos:] is empty. """
		return self._ropen and pos >= len(path)

	@staticmethod
	def fin(ctx, path, pos):
		"""fin() is called before entering the fenter() functions of
		childs to transform the current context.
		
		It must return a tuple (new_ctx, cont) where new_ctx is the
		transformed context and cont evaluates to True if traversal
		should continue or to False if it should be immediatly aborted.
		In the last case, the caller will return None regardless of
		new_ctx returned by this function. """
		return (ctx, True)

	@staticmethod
	def fout(ctx, path, pos):
		"""fin() is called after the return of the fenter() function
		of the child, and can return a transformed new ctx if needed."""
		return ctx

	@staticmethod
	def fvisit(ctx, path, pos):
		"""fvisit() is called to visit the current node. It returns
		a context, typically a new or a transformed one. It can also
		return None to indicate a (not so) late dynamic detection of
		a non existing resource."""
		return None

	def fenter_child(self, child_t, ctx, path, pos, eaten):
		"""Typically called by base and overloaded versions of fenter()
		to factorise code. """
		ctx, cont = self.fin(ctx, path, pos)
		if not cont:
			return None
		else:
			return self.fout(child_t.fenter(ctx, path, pos + eaten), path, pos)

	def fenter(self, ctx, path, pos):
		"""Called to do a lookup of the resource at peth[pos:]
		in this subtree. """
		if self.visitable(ctx, path, pos):
			return self.fvisit(ctx, path, pos)
		else:
			return None

	def ctx_path(self, path):
		"""Entry point to call on the root node to initiate a lookup
		of the resource identified by path. """
		return self.fenter(None, (None,None)+tuple(path), 2)

class RT_Set(RT_node):

	"""Resource Tree containing a finite ordered set of statically
	named subtrees. """

	def __init__(self, ropen, odict_subtree):
		"""When ropen is True, if there is no subpath remaining when this
		node is being scanned to look for a resource, it'll be visited
		(by call of the fvisit() method). This behavior is dictated by
		the visitable() so it can be modified by overloading it.
		
		odict_subtree is an ordered dictionary in which keys are node
		names and values instances of RT_node (or a subclass). """
		self._odict_subtree = odict_subtree
		super(RT_Set, self).__init__(ropen)

	def fenter(self, ctx, path, pos):
		if self.visitable(ctx, path, pos):
			return self.fvisit(ctx, path, pos)
		elif (pos < len(path)) and (path[pos] in self._odict_subtree):
			return self.fenter_child(self._odict_subtree[path[pos]], ctx, path, pos, 1)
		else:
			return None

class RT_Dyn(RT_node):

	"""Resource Tree containing dynamically addressable resources."""

	def __init__(self, ropen, args_val, subtree):
		"""When ropen is True, if there is no subpath remaining when this
		node is being scanned to look for a resource, it'll be visited
		(by call of the fvisit() method). This behavior is dictated by
		the visitable() so it can be modified by overloading it.
		
		args_val is a list, whose length is the depth of the dynamic
		portion at top of this subtree that is handled by this instance,
		and which contains functions that will be called with the
		corresponding name of the subdirectory at the matching depth of
		the path being looked up.
		
		subtree is a Resource Tree instance that will be traversed when
		all functions of args_val validated their respective node name.
		subtree can use the path[pos-len(args_val):pos-1] slice of the
		path if it needs to know its name. """
		self._args_val = list(args_val)
		self._subtree = subtree
		super(RT_Dyn, self).__init__(ropen)

	def fenter(self, ctx, path, pos):
		if self.visitable(ctx, path, pos):
			return self.fvisit(ctx, path, pos)
		elif (len(path) - pos >= len(self._args_val)
		      and all(val(path[pos+i])
		              for i,val in enumerate(self._args_val))):
			return self.fenter_child(self._subtree, ctx, path, pos, len(self._args_val))
		else:
			return None

# TODO: introspection on / of RT_Mount
# (needs changing visitable()...)

class RT_MountError(ValueError): pass

class RT_Mount(RT_node):

	"""A Resource Tree on which one can dynamically mount other Resource
	Trees.
	
	The behavior of the mount() and umount() methods is very similar to
	the one of the Unix mount/umount concept, except you don't have to
	precreate mount points: for example if you mount something on
	('configuration', 'ethernet') in a previously empty RT_Mount instance,
	both 'configuration' and 'ethernet' internal nodes would be
	automatically created. When later you umount it, 'ethernet' will be
	automatically destroyed and if it has no sibling remaining (that could
	have been created meanwhile) 'configuration' will also be automatically
	removed.
	
	It is possible to mount something on the subpath B of a path A if
	something is already mounted on A, but it is not possible to mount
	something on A after B. This choice has been made because the internal
	data structures and algorithm used would make it difficult that the
	new A mount mask what was previously on B during the lifespan of the
	new A mount. In consequence, rather than exposing a behavior to
	different from the one of Unix mounts, this use case has been
	explicitely disabled. Ex:
	  - The following sequence is allowed
		rt.mount(('configuration'), subrt_a)
		rt.mount(('configuration', 'ethernet'), subrt_b)
	  - The following sequence is forbidden (an exception will be raised
	    during the second call and the state of rt won't change)
		rt.mount(('configuration', 'ethernet'), subrt_b)
		rt.mount(('configuration'), subrt_a)
	
	You can't unmount something that has childs mounted on subpaths:
	  - Allowed:
		rt.mount(('configuration'), subrt_a)
		rt.mount(('configuration', 'ethernet'), subrt_b)
		rt.umount(('configuration', 'ethernet'), subrt_b)
		rt.umount(('configuration'), subrt_a)
	  - Disallowed:
		rt.mount(('configuration'), subrt_a)
		rt.mount(('configuration', 'ethernet'), subrt_b)
		rt.umount(('configuration'), subrt_a)
	
	If a resource tree T is mounted on A and has no mounted childs on a
	subpath of A, it is possible to mount an other resource tree S on A
	that will, during its lifespan, override the first one. Unmounting
	newest overriding mounts is required before unmounting previous ones.
	This rule does not overload the previous ones, both can be used at the
	same time and every constraints must apply:
	  - Allowed
		rt.mount(('configuration'), subrt_T)
		rt.mount(('configuration'), subrt_S)
		rt.mount(('configuration', 'ethernert'), subrt_b)
	  - Allowed
		rt.mount(('configuration'), subrt_T)
		rt.mount(('configuration'), subrt_S)
		rt.umount(('configuration'), subrt_S)
		rt.umount(('configuration'), subrt_T)
	  - Disallowed
		rt.mount(('configuration'), subrt_T)
		rt.mount(('configuration'), subrt_S)
		rt.umount(('configuration'), subrt_T)
	
	"""

	def __init__(self, ropen = False):
		self._mount_points = {}  # { name: [sub_mnt_pts, mntstk] }
		                         # where mstk is a stack of rt_node 
		                         # with the top element being in use
		super(RT_Mount, self).__init__(ropen)

	@staticmethod
	def visitable(ctx, path, pos):
		"This node is never directly visitable."
		return False

	def mount(self, subtree, path):
		"""Mount subtree on path if nothing is already mounted 
		on a subpath. You can't mount something on the root. """
		pos = 0
		mntpts = self._mount_points
		if not path:
			raise RT_MountError, "Can't mount on root"
		while pos < len(path) - 1:
			if path[pos] not in mntpts:
				mntpts[path[pos]] = [{},[]]
			mntpts = mntpts[path[pos]][0]
			pos += 1
		if path[pos] in mntpts:
			sub,mntstk = mntpts[path[pos]]
			if sub:
				raise RT_MountError, "Can't adopt already mounted nodes"
			mntpts[path[pos]][1].insert(0, subtree)
		else:
			mntpts[path[pos]] = [{},[subtree]]

	def _umount_rec(self, mntpts, path, pos, subtree):
		if path[pos] not in mntpts:
			raise RT_MountError, "Nothing mounted there"
		if pos == len(path) - 1:
			sub,mntstk = mntpts[path[pos]]
			if sub:
				raise RT_MountError, "Can't give up mounted children"
			if len(mntstk) == 0 or mntstk[0] != subtree:
				raise RT_MountError, "Not found on top of mount point"
			mntstk.pop(0)
		else:
			self._umount_rec(mntpts[path[pos]][0], path, pos + 1, subtree)
			sub,mntstk = mntpts[path[pos]]
		if (not sub) and (not mntstk):
			del mntpts[path[pos]]

	def umount(self, subtree, path):
		"""Unmount subtree from path. There should be no mounted child
		(checked) and the given subtree must be on top of all mounts
		on this path. """
		if not path:
			raise RT_MountError, "Can't umount the root"
		self._umount_rec(self._mount_points, path, 0, subtree)

	def lookup_mnt(self, path, pos):
		"""Finds and returns the best matching mounted subtree
		begining with path[pos:]"""
		found_rt, found_pos = None, -1
		mntpts = self._mount_points
		while pos < len(path):
			if path[pos] not in mntpts:
				break
			if len(mntpts[path[pos]][1]) > 0:
				found_rt, found_pos = mntpts[path[pos]][1][0], pos + 1
			mntpts = mntpts[path[pos]][0]
			pos += 1
		return found_rt, found_pos

	def fenter(self, ctx, path, pos):
		if self.visitable(ctx, path, pos):
			return self.fvisit(ctx, path, pos)
		found_rt, found_pos = self.lookup_mnt(path, pos)
		if found_pos >= 0:
			return self.fenter_child(found_rt, ctx, path, pos, found_pos - pos)
		else:
			return None

__all__ = ['RT_node', 'RT_Set', 'RT_Dyn', 'RT_Mount']
