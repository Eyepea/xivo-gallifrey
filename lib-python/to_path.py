"""Send modules of the package holding this one into the program current path

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

import sys, os, os.path
module_name = __name__
# Must be in a package, so we want an exception to be raised in case our
# name show now trace of a package name...
pkg_name, module_name = module_name.rsplit('.', 1)
pkg = __import__(pkg_name)
for sub_pkg in pkg_name.split('.')[1:]:
	pkg = getattr(pkg, sub_pkg)
add_path = os.path.join(os.getcwd(), pkg.__path__[0])
known_paths = set((os.path.normcase(p) for p in sys.path))
if os.path.normcase(add_path) not in known_paths and os.path.exists(add_path):
	sys.path.append(add_path)
