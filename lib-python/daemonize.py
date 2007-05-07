# -*- coding: iso-8859-15 -*-
"""Transforms a process into a daemon from hell

Copyright (C) 2007, Proformatique

"""

REV_DATE = "$Revision$ $Date$"

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
