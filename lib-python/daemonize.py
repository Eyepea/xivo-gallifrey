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

import os, re, sys, errno, os.path, traceback

SLASH_PROC = os.sep + 'proc'
PROG_SLINK = 'exe'
PROG_CMDLN = 'cmdline'

def log_exception(logline_func):
	"""Log the current exception using the passed logline_func function."""
	for x in map(lambda x: x.rstrip(), traceback.format_exception(*sys.exc_info())):
		logline_func(x)
	sys.exc_clear()

def remove_if_stall_pidfile(pidfile, logline_func):
	try:
		try:
			pid_maydaemon = int(file(pidfile).readline().strip())
		except IOError, e:
			if e.errno == errno.ENOENT:
				return # nothing to suppress, so do nothing...
			raise
		try:
			lock_pgm = \
			    os.path.basename(
			        os.readlink(
		        	    os.path.join(SLASH_PROC,
				                 str(pid_maydaemon),
			        	         PROG_SLINK)))
		except OSError, e:
			if e.errno == errno.ENOENT:
				# no process with the PID extracted from the
				# pidfile, so no problem to remove the latter
				os.unlink(pidfile)
				return
			raise
		# Who are we?
		i_am = os.path.basename(re.sub(r'\.py$', '', sys.argv[0]))
		if i_am == lock_pgm:
			logline_func("A pidfile already exists and a process with our name '%s' is already runnning" % i_am)
			return
		# It may not be us, but we must be quite sure about that
		# so check the whole command line of the other process
		other_cmdline = file(os.path.join(SLASH_PROC,
				                  str(pid_maydaemon),
						  PROG_CMDLN)
				    ).read().split('\0')
	except:
		log_exception(logline_func)

def take_file_lock_or_die(own_file, lock_file, own_content):
	try:
		os.link(own_file, lock_file)
	finally:
		os.unlink(own_file)
	content = file(lock_file).read(len(own_content)+1)
	if content != own_content:
		sys.exit(1)

def daemonize(logline_func=lambda x: sys.stderr.write(x+'\n'),
              pidfile = None, pidfile_lock = False, logout = None, logerr = None):
	"""Daemonize the program, ie. make it runs in the 'background',
	detach it from its controlling terminal, and detach it from its
	controlling process group session.
	
	"""
	try:
		pid = os.fork()
		if pid > 0:
			os._exit(0)
	except SystemExit:
		raise
	except:
		log_exception(logline_func)
		sys.exit(1)
	os.setsid()
	os.umask(0)
	os.chdir('/')
	try:
		pid = os.fork()
		if pid > 0:
			os._exit(0)
	except SystemExit:
		raise
        except:
		log_exception(logline_func)
		sys.exit(1)

	pid = os.getpid()
	try:
	    if pidfile:
		if pidfile_lock:
			remove_if_stall_pidfile(pidfile, logline_func)
			pid_write_file = pidfile + '.' + str(pid)
		else:
			pid_write_file = pidfile
		fpid = open(pid_write_file, 'w')
		try:
			fpid.write("%s\n" % (pid,))
		finally:
			fpid.close()
		if pidfile_lock:
			take_file_lock_or_die(pid_write_file, pidfile, "%s\n" % (pid,))
	except:
		log_exception(logline_func)
		sys.exit(1)

	# Redirect standard file descriptors.
	sys.stdout.flush()
	sys.stderr.flush()
	os.close(sys.__stdin__.fileno())
	os.close(sys.__stdout__.fileno())
	os.close(sys.__stderr__.fileno())

	# stdin always from /dev/null
	newstdin = open(os.devnull, 'r')
	if newstdin.fileno() == 0:
		sys.stdin = newstdin
	else:
		os.dup2(newstdin.fileno(), 0)
		newstdin.close()

	if logout is None:
		sys.stdout = open(os.devnull, 'w')
	else:
		so = sys.stdout = logout
		os.dup2(so.fileno(), 1)

	if logerr is None:
		sys.stdout = open(os.devnull, 'w', 0)
	else:
		se = sys.stderr = logerr
		os.dup2(se.fileno(), 2)

	return pid
