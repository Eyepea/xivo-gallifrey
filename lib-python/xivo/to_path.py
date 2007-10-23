"""Send modules of the package holding this one into the program current path

Copyright (C) 2007, Proformatique
"""

__version__ = "$Revision$ $Date$"

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
__all__ = ()
