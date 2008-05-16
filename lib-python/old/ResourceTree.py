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

from types import MethodType
from pyfunc import *

# NOTE: paths are tuples of their splitted components in this module

class Node(object):
	
	"""Base class of all Resource Trees including leaves nodes"""
	
	@staticmethod
	def fin(ctx, path, pos, eaten):
		"""fin() is called when looking for child applications, to check
		if the lookup must continue and to transform the local context
		to one that will be passed to the fenter() method of childs.
		
		- ctx: local context
		- path: complete path of the request
		- pos: current position in the path
		- pos + eaten: child position in the path
		
		It must return a tuple (new_ctx, cont) where new_ctx is the
		transformed context and cont evaluates to True if traversal
		should continue or to False if it should be immediatly aborted.
		In the last case, the caller will return None regardless of
		new_ctx returned by this function. """
		return (ctx, True)
	
	@staticmethod
	def fout(ctx, path, pos, eaten):
		"""fout() is called just after the return of the fenter() method
		of the child, and can return a transformed new ctx if needed.
		
		Same parameters as in fin(). """
		return ctx
	
	@staticmethod
	def visitable(ctx, path, pos):
		"""If and only if this method returns something True, the current
		node will immediately be visited by entering fvisit().
		
		This default method from Node just returns False.
		
		Same parameters (but eaten) as in fin(). """
		return False
	
	@staticmethod
	def fvisit(ctx, path, pos):
		"""fvisit() is called to visit the current node. It returns
		a context, typically a new or a transformed one. It can also
		return None to indicate a (not so) late dynamic detection of
		a non existing resource.
		
		Also see visitable().
		
		Same parameters (but eaten) as in fin(). """
		return None
	
	@staticmethod
	def lookup_sub(ctx, path, pos):
		"""Finds and returns the next direct child identified by path[pos]
		and possibly path[pos+1], path[pos+2], ...
		
		Returns both the object describing the child and the number of
		path component consumed from path[pos] to select this child, in
		a tuple.
		
		If no child has been found returns (None, -1) --
		Node.lookup_sub() only does that. """
		return None, -1
	
	@classmethod
	def fenter_child(cls, child_t, ctx, path, pos, eaten):
		"""Standard factorized logic to fenter() a child.
		
		This is the place where our local fin() and fout() are called
		around the fenter() of the child call.
		
		Same parameters as in fin(), plus:
		- child_t: child object to enter.
		"""
		ctx, cont = cls.fin(ctx, path, pos, eaten)
		if not cont:
			return None
		else:
			return cls.fout(child_t.fenter(ctx, path, pos + eaten),
			                path, pos, eaten)
	
	@classmethod
	def fenter(cls, ctx, path, pos):
		"""Lookup method called to choose which node is to be visited.
		
		Uses visitable(), fvisit(), lookup_sub() and fenter_child(). """
		if cls.visitable(ctx, path, pos):
			return cls.fvisit(ctx, path, pos)
		child_t, eaten = cls.lookup_sub(ctx, path, pos)
		if child_t:
			return cls.fenter_child(child_t, ctx, path, pos, eaten)
		else:
			return None
	
	@classmethod
	def ctx_path(cls, path, ctx):
		"""Entry point to call on the root node to initiate a lookup
		of the resource identified by path. """
		return cls.fenter(ctx, (None,None)+tuple(path), 2)

class VisitableNode(Node):
	
	"Defines a visitable node when at end of path. "
	
	@staticmethod
	def visitable(ctx, path, pos):
		"""Decides to visit the node when path[pos:] is empty.
		Also see Node.visitable() """
		return pos >= len(path)

	@staticmethod
	def fvisit(ctx, path, pos):
		"""Returns ctx which can be a useful default for a visitable
		node when dispatching is done by the parent in .fin() """
		return ctx

class NodeInst(Node):
	
	"""Base class of Resource Trees when subtree lookup is performed by an
	instance and not the class itself. """
	
	def fenter_child(self, child_t, ctx, path, pos, eaten):
		"Instance method version of Node.fenter_child(). "
		ctx, cont = self.fin(ctx, path, pos, eaten)
		if not cont:
			return None
		else:
			return self.fout(child_t.fenter(ctx, path, pos + eaten),
			                path, pos, eaten)
	
	def fenter(self, ctx, path, pos):
		"Instance method version of Node.fenter(). "
		if self.visitable(ctx, path, pos):
			return self.fvisit(ctx, path, pos)
		child_t, eaten = self.lookup_sub(ctx, path, pos)
		if child_t:
			return self.fenter_child(child_t, ctx, path, pos, eaten)
		else:
			return None
	
	def ctx_path(self, path, ctx):
		"Instance method version of Node.ctx_path(). "
		return self.fenter(ctx, (None,None)+tuple(path), 2)


class SetTree(Node):
	
	"""Resource Tree containing a finite set of statically named
	subtrees. You can use an OrdDict to handle the actual set so that it
	will be ordered.
	
	Your derived class needs to provide:
	 - SUBNAME_OBJ      mapping of child names (keys) to associated
	                    indexable objects (values). For a given child name,
	                    cls.SUBNAME_OBJ[child_name][0] must contain the
	                    corresponding Resource Tree description object of
	                    that child.
	"""
	
	@classmethod
	def lookup_sub(cls, ctx, path, pos):
		"""Does a simple lookup in cls.SUBNAME_OBJ, taking the child
		class identified by path[pos] when it exists in there. """
		if (pos < len(path)) and (path[pos] in cls.SUBNAME_OBJ):
			return cls.SUBNAME_OBJ[path[pos]][0], 1
		else:
			return None, -1


class DynTree(Node):
	
	"""Resource Tree containing dynamically addressable resources.
	
	Your derived class needs to provide:
	 - subpath_ok()     Either classmethod or staticmethod.
	                    The number of parameter will be introspected so the
	                    same number of components from the path will be
	                    passed to this function whose goal is to check
	                    whether the name of a resource is well-formed.
	 - SUBOBJ           Object describing the childs
	"""
	
	@classmethod
	def lookup_sub(cls, ctx, path, pos):
		"""Let sublen be the number of (non class/instance) parameters of
		the method cls.subpath_ok(), if this latter returns True when
		called with the right number of path components from the current
		position then lookup_sub() returns childs description object
		cls.SUBOBJ and the number of path components the child is away
		from here. """
		sublen = cls.subpath_ok.func_code.co_argcount \
		         - isinstance(cls.subpath_ok, MethodType)
		if len(path) - pos >= sublen \
		   and cls.subpath_ok(*path[pos:pos+sublen]):
			return cls.SUBOBJ, sublen
		else:
			return None, -1


class MountError(ValueError): pass

class MountTree(NodeInst):
	
	"""A Resource Tree on which one can dynamically mount other Resource
	Trees. Because of the inherently dynamic behavior of this concept and
	in contrast to Node, SetTree and DynTree, management of subtree lookup
	will be performed by an instance of this class -- not the class itself.
	
	The behavior of the mount() and umount() methods is very similar to
	the one of the Unix mount/umount concept, except you don't have to
	pre-create mount points: for example if you mount something on
	('configuration', 'ethernet') in a previously empty MountTree instance,
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
		rt.mount(('configuration',), subrt_a)
		rt.mount(('configuration', 'ethernet'), subrt_b)
	  - The following sequence is forbidden (an exception will be raised
	    during the second call and the state of rt won't change)
		rt.mount(('configuration', 'ethernet'), subrt_b)
		rt.mount(('configuration',), subrt_a)
	
	You can't unmount something that has childs mounted on subpaths:
	  - Allowed:
		rt.mount(('configuration',), subrt_a)
		rt.mount(('configuration', 'ethernet'), subrt_b)
		rt.umount(('configuration', 'ethernet'), subrt_b)
		rt.umount(('configuration',), subrt_a)
	  - Disallowed:
		rt.mount(('configuration',), subrt_a)
		rt.mount(('configuration', 'ethernet'), subrt_b)
		rt.umount(('configuration',), subrt_a)
	
	If a resource tree T is mounted on A and has no mounted childs on a
	subpath of A, it is possible to mount an other resource tree S on A
	that will, during its lifespan, override the first one. Unmounting
	newest overriding mounts is required before unmounting previous ones.
	This rule does not overload the previous ones, both can be used at the
	same time and every constraints must apply:
	  - Allowed
		rt.mount(('configuration',), subrt_T)
		rt.mount(('configuration',), subrt_S)
		rt.mount(('configuration', 'ethernet'), subrt_b)
	  - Allowed
		rt.mount(('configuration',), subrt_T)
		rt.mount(('configuration',), subrt_S)
		rt.umount(('configuration',), subrt_S)
		rt.umount(('configuration',), subrt_T)
	  - Disallowed
		rt.mount(('configuration',), subrt_T)
		rt.mount(('configuration',), subrt_S)
		rt.umount(('configuration',), subrt_T)
	
	"""
	
	def __init__(self):
		self._mount_points = {}  # { name: [sub_mnt_pts, mntstk] }
		                         # where mstk is a stack of Node objects
		                         # with the top element being in use
		super(MountTree, self).__init__()
	
	def mount(self, subtree, path):
		"""Mount subtree on path if nothing is already mounted on a
		subpath. You can't mount anything directly on the root. """
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
		(this is checked) and the given subtree must be on top of all
		mounts on this path. """
		if not path:
			raise RT_MountError, "Can't umount the root"
		self._umount_rec(self._mount_points, path, 0, subtree)
	
	def lookup_sub(self, ctx, path, pos):
		"""Finds and returns the best matching mounted subtree beginning
		with path[pos:]
		Returns (None, -1) if none found, else returns the child subtree
		object and the number of path components it's away from here. """
		found_rt, found_pos = None, -1
		mntpts = self._mount_points
		for i in xrange(0,len(path)-pos):
			if path[pos+i] not in mntpts:
				break
			if len(mntpts[path[pos+i]][1]) > 0:
				found_rt, found_pos = mntpts[path[pos+i]][1][0], i + 1
			mntpts = mntpts[path[pos+i]][0]
		return found_rt, found_pos


__all__ = [ 'Node', 'VisitableNode', 'NodeInst', 'SetTree', 'DynTree',
            'MountTree', 'MountError' ]
