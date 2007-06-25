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

LOGLINE_STDERR = lambda x: sys.stderr.write(x+'\n')

class DaemonError(Exception): pass

def log_exception(logline_func = LOGLINE_STDERR, noclear = False):
	"""Log the current exception using the passed 'logline_func' function.
	'logline_func' will be called one line at a time, without any trailing
	\\n (or \\r). Clear the current exception at end of command if
	'noclear' is False. """
	for x in map(lambda x: x.rstrip(), traceback.format_exception(*sys.exc_info())):
		logline_func(x)
	if not noclear:
		sys.exc_clear()

def remove_if_stale_pidfile(pidfile, logline_func = LOGLINE_STDERR):
    """If the given 'pidfile' does not exists, do nothing.
    If it contains a PID that does not correspond to a living process on the
    local host, remove the stale 'pidfile'.
    If it contains a PID for which the corresponding process neither comes from
    an executable with our own name (as per sys.argv[0], any r'\.py$' put
    appart) nor has been launched with a command line containing our own name
    in the command or one of the parameters, remove the stale 'pidfile'.
    
    As this function is intended to be used during the startup process of a
    daemon, deciding what to do exactly when a pidfile was and maybe remains
    present is out of its responsibility. Therefore, it will catch and confine
    any exception that could be raised during its work because of any
    unrespected precondition (in this context, understand bad/unexpected state
    of the environment). Anyway unexpected exceptions are logged thanks to
    the 'logline_func' function, which is called one line at a time when
    applicable. """
    c14n_prog_name = lambda arg: os.path.basename(re.sub(r'\.py$', '', arg))
    try:
	try:
		pid_maydaemon = int(file(pidfile).readline().strip())
	except IOError, e:
		if e.errno == errno.ENOENT:
			return # nothing to suppress, so do nothing...
		raise
	# Who are we?
	i_am = c14n_prog_name(sys.argv[0])
	try:
		other_cmdline = file(os.path.join(SLASH_PROC,
				        	  str(pid_maydaemon),
						  PROG_CMDLN)
				    ).read().split('\0')
		if len(other_cmdline) and other_cmdline[-1] == '':
			other_cmdline.pop()
	except IOError, e:
		if e.errno == errno.ENOENT:
			# no process with the PID extracted from the
			# pidfile, so no problem to remove the latter
			os.unlink(pidfile)
			return
		raise
	# Check the whole command line of the other process
	if i_am in map(c14n_prog_name, other_cmdline):
		logline_func("A pidfile '%s' already exists (contains pid %d) and the correponding process command line contains our own name '%s'" % (pidfile, pid_maydaemon, i_am))
		return
	# It may not be us, but we must be quite sure about that so also try
	# to validate with the name of the executable.
	full_pgm = lock_pgm = None
	try:
		full_pgm = \
		    os.readlink(
		      os.path.join(
			SLASH_PROC,
			str(pid_maydaemon),
			PROG_SLINK))
		lock_pgm = os.path.basename(full_pgm)
	except OSError, e:
		if e.errno == errno.EACCES:
			# We consider it's ok not being able to access
			# "/proc/<pid>/exe" if we could previously access
			# "/proc/<pid>/cmdline", because if we do not have
			# the needed permissions to run the daemon this will
			# be catched latter (potentially when creating our
			# own pidfile)
			lock_pgm = None
		else:
			raise
	if i_am == lock_pgm:
		logline_func("A pidfile '%s' already exists (contains pid %d) and an executable with our name '%s' is runnning with that pid." % (pidfile, pid_maydaemon, i_am))
		return
	# Ok to remove the previously existing pidfile now.
	logline_func("A pidfile '%s' already exists (contains pid %d) but the corresponding process does not seem to match with our own name '%s'. Will remove the pidfile." % (pidfile, pid_maydaemon, i_am))
	logline_func("Splitted command line of the other process: %s" % str(other_cmdline))
	if lock_pgm:
		logline_func("Name of the executable the other process comes from: %s" % full_pgm)
	os.unlink(pidfile)
	return
    except:
	log_exception(logline_func)

def take_file_lock_or_die(own_file, lock_file, own_content):
	"""Creates an hard link from own_file to lock_file, then unconditionally
	unlink own_file. If the hard link has been successfully created check
	that what is inside this file really is what is expected (that is
	what is passed in parameter own_content)
	
	No exception is completely catched by this function, and a failure
	of os.link() would mean the raise of one. OSError are "translated"
	into DaemonError if the OSError is of type EEXIST, so it's easy to
	know when this function raised and exception because an other one
	is already running and when this is for an other reason."""
	try:
	    try:
		os.link(own_file, lock_file)
	    finally:
		os.unlink(own_file)
	except OSError, e:
	    if e.errno == errno.EEXIST:
		raise DaemonError, (
			"The lock file '%s' already exists - won't overwrite it. "
			"An other instance of ourself is probably running."
			% lock_file)
	    else:
		raise
	content = file(lock_file).read(len(own_content)+1)
	if content != own_content:
		raise DaemonError, (
			"I thought I successfully took the lock file '%s' but "
			"it does not contain what was expected. Somebody is "
			"playing with us."
			% lock_file)

def create_pidfile_or_die(logline_func = LOGLINE_STDERR, pidfile = None, pidfile_lock = False):
	"""You must give a writable filename in parameter 'pidfile', or None.
	
	If you pass None no action will be performed.
	
	Otherwise there are two modes of operation to create the pidfile:
	
	* If pidfile_lock is False we can unconditionally create a pidfile,
	  overwriting any previously existing one. If its possible to start
	  the process only when no such pidfile exists or when it contains a
	  PID pointing on nothing or on a totaly unrelated process, this can
	  be acceptable. But this is still subject to race conditions.
	* If pidfile_lock is True the pidfile will be created using a two stage
	  process. First remove_if_stale_pidfile() is called so that any
	  existing stale pidfile will be removed. Then a temporary pidfile
	  whose name is postfixed with our own PID is generated. Eventually,
	  an hard link is created from the temporary pidfile to the
	  non-temporary one and the temporary link is destroyed. At this point
	  if the non-temporary pidfile contains our own PID, we know for sure
	  that we just atomically grabbed a lock and the right to serenely run.

	Exceptions are logged thanks to the 'logline_func' function, which is
	called one line at a time when applicable.	

	This function returns the PID. """
	pid = os.getpid()
	try:
	    if pidfile:
		if pidfile_lock:
			remove_if_stale_pidfile(pidfile, logline_func)
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
	return pid

def daemonize(logline_func = LOGLINE_STDERR, pidfile = None, pidfile_lock = False,
              logout = None, logerr = None):
	"""Daemonize the program, ie. make it runs in the "background",
	detach it from its controlling terminal, and detach it from its
	controlling process group session.
	
	'pidfile' and 'pidfile_lock' will be passed to function
	create_pidfile_or_die() after forking. 'pidfile' can contain None 
	(default value) in which case create_pidfile_or_die() will take no
	action.
	
	If 'logout' is not None, standard output will be directed to this
	Python file interface object instead of /dev/null. Same thing for
	'logerr' and standard error. """
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

	pid = create_pidfile_or_die(logline_func, pidfile, pidfile_lock)

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

if __debug__:
    if __name__ == '__main__':
	import time
	out = open('/home/xilun/daemon_out', 'w')
	err = open('/home/xilun/daemon_err', 'w')
	daemonize(pidfile = '/home/xilun/pidfile', pidfile_lock = True,
	          logout=out, logerr=err)
	time.sleep(42)
