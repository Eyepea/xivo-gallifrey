import re, os, os.path

__version__ = "$Revision$ $Date$"

# WARNING: non order preserving
def uniqlist(lst):
	if not lst: return []
	return list(set(lst))

def get_this_internal_package_path():
	return os.path.dirname(os.path.abspath(__file__))

# Python doesn't really want us to do that because of
# compatibility with stupid operating systems, but thanks
# to this function we can do it anyway... :)
def get_module_list_from_package_path(package_path):
	return uniqlist([re.sub(r'\.py[^\.]*$','',filename)
		for filename in os.listdir(package_path)
		if re.search(r'\.py[^\.]*$', filename)
		and '__init__' not in filename])

__all__ = get_module_list_from_package_path(get_this_internal_package_path())
