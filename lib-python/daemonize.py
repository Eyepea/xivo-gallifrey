"""Transforms a process into a daemon from hell

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

import os, sys, traceback

def log_exception(logline_func):
	"""Log the current exception using the passed logline_func function."""
	for x in map(lambda x: x.rstrip(), traceback.format_exception(*sys.exc_info())):
		logline_func(x)

def daemonize(logline_func=lambda x: sys.stderr.write(x+'\n')):
	"""Daemonize the program, ie. make it runs in the 'background',
	detach it from its controlling terminal, and detach it from its
	controlling process group session.
	
	"""
	try:
		pid = os.fork()
		if pid > 0:
			sys.exit(0)
	except SystemExit:
		raise
	except:
		log_exception(logline_func)
		sys.exit(1)
	os.setsid()
	os.umask(0)
	try:
		pid = os.fork()
		if pid > 0:
			sys.exit(0)
	except SystemExit:
		raise
        except:
		log_exception(logline_func)
		sys.exit(1)
	dev_null = file('/dev/null', 'r+')
	os.dup2(dev_null.fileno(), sys.stdin.fileno())
	os.dup2(dev_null.fileno(), sys.stdout.fileno())
	os.dup2(dev_null.fileno(), sys.stderr.fileno())
