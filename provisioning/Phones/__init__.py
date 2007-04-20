# -*- coding: iso-8859-15 -*-

import re, sys, os, os.path

# WARNING: non order preserving
def uniqlist(lst):
	if not lst: return []
	return (dict(map(lambda a: (a,1), lst))).keys()

# WARNING: this function only makes sens for internal packages shipped
# with a calling script living in a parent directory...
def get_this_internal_package_path():
	return os.path.join(os.path.abspath(sys.path[0]), __name__)

# Python doesn't really want us to do that because of
# compatibility with stupid operating systems, but thanks
# to this function we can do it anyway... :)
def get_module_list_from_package_path(package_path):
	return uniqlist([re.sub(r'\.py[^\.]*$','',filename)
		for filename in os.listdir(package_path)
		if re.search(r'\.py[^\.]*$', filename)
		and '__init__' not in filename])

mypath = get_this_internal_package_path()
__all__ = get_module_list_from_package_path(mypath)
# print __all__
