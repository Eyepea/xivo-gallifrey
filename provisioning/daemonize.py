# -*- coding: iso-8859-15 -*-
"""Transforms a process into a daemon from hell"""

def daemonize():
	"""Daemonize the program, ie. make it runs in the 'background',
	detach it from its controlling session, and detach it from its
	controlling process group session.
	
	"""
	try:
		pid = os.fork()
		if pid > 0:
			sys.exit(0)
	except OSError, e:
		provsup.log_debug_current_exception()
		sys.exit(1)
	os.setsid()
	os.umask(0)
	try:
		pid = os.fork()
		if pid > 0:
			sys.exit(0)
        except OSError, e:
		provsup.log_debug_current_exception()
		sys.exit(1)
	dev_null = file('/dev/null', 'r+')
	os.dup2(dev_null.fileno(), sys.stdin.fileno())
	os.dup2(dev_null.fileno(), sys.stdout.fileno())
	os.dup2(dev_null.fileno(), sys.stderr.fileno())
