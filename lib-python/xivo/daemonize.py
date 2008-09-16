"""Transforms a process into a daemon from hell

Copyright (C) 2007, 2008  Proformatique

WARNING: Linux specific module, needs /proc/

"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2007, 2008  Proformatique

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

import os
import re
import sys
import errno
import logging


SLASH_PROC = os.sep + 'proc'
PROG_SLINK = 'exe'
PROG_CMDLN = 'cmdline'


log = logging.getLogger("xivo.daemonize")


def remove_if_stale_pidfile(pidfile):
    """
    @pidfile: PID file to remove if it is staled.

    Exceptions are logged and are not propagated.
    """
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
            other_cmdline = file(os.path.join(SLASH_PROC, str(pid_maydaemon), PROG_CMDLN)).read().split('\0')
            if len(other_cmdline) and other_cmdline[-1] == "":
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
            log.warning("A pidfile %r already exists (contains pid %d) and the correponding process command line contains our own name %r",
                        pidfile, pid_maydaemon, i_am)
            return
        # It may not be us, but we must be quite sure about that so also try
        # to validate with the name of the executable.
        full_pgm = lock_pgm = None
        try:
            full_pgm = os.readlink(os.path.join(SLASH_PROC, str(pid_maydaemon), PROG_SLINK))
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
            log.warning("A pidfile %r already exists (contains pid %d) and an executable with our name %r is runnning with that pid.",
                        pidfile, pid_maydaemon, i_am)
            return
        # Ok to remove the previously existing pidfile now.
        log.info("A pidfile %r already exists (contains pid %d) but the corresponding process does not seem to match with our own name %r.  "
                 "Will remove the pidfile.", pidfile, pid_maydaemon, i_am)
        log.info("Splitted command line of the other process: %s", other_cmdline)
        if lock_pgm:
            log.info("Name of the executable the other process comes from: %s", full_pgm)
        os.unlink(pidfile)
        return
    except:
        log.exception("unexpected error")


def take_file_lock(own_file, lock_file, own_content):
    """
    Atomically "move" @own_file to @lock_file if the latter does not exists,
    else just remove @own_file.

    @own_file: filepath of the temporary file that contains our PID
    @lock_file: destination filepath
    @own_content: content of @own_file

    Return True if the lock has been successfully taken, else False.
    (Caller should also be prepared for OSError exceptions)
    """
    try:
        try:
            os.link(own_file, lock_file)
        finally:
            os.unlink(own_file)
    except OSError, e:
        if e.errno == errno.EEXIST:
            log.warning("The lock file %r already exists - won't "
                    "overwrite it.  An other instance of ourself "
                    "is probably running.", lock_file)
            return False
        else:
            raise
    content = file(lock_file).read(len(own_content) + 1)
    if content != own_content:
        log.warning(
                "I thought I successfully took the lock file %r but "
                "it does not contain what was expected.  Somebody is "
                "playing with us.", lock_file)
        return False
    return True


def create_pidfile_or_die(pidfile, pidfile_lock):
    """
    @pidfile:
            None => this function just returns the PID
            Else @pidfile must be a writable filepath

    @pidfile_lock:
            False => Create the pidfile unconditionally
                    (WARNING: potentially subject to race conditions)
            True => Remove potentially existing staled PID file
                    Create a new one atomically
                    If the new one is ours: success

    Exceptions are logged.

    This function returns the PID.
    """
    pid = os.getpid()
    try:
        if pidfile:
            if pidfile_lock:
                remove_if_stale_pidfile(pidfile)
                pid_write_file = pidfile + '.' + str(pid)
            else:
                pid_write_file = pidfile
            fpid = open(pid_write_file, 'w')
            try:
                fpid.write("%s\n" % pid)
            finally:
                fpid.close()
            if pidfile_lock:
                if not take_file_lock(pid_write_file, pidfile, "%s\n" % pid):
                    sys.exit(1)
    except SystemExit:
        raise
    except:
        log.exception("unable to take pidfile")
        sys.exit(1)
    return pid


def daemonize(pidfile=None, pidfile_lock=True, logout=None, logerr=None):
    """
    Daemonize the program, ie. make it run in the "background", detach
    it from its controlling terminal and from its controlling process
    group session.

    @pidfile:
            PID file path, or None

    @pidfile_lock: (significant only when @pidfile is not None)
            False => just write @pidfile unconditionally
            True => @pidfile is a lockfile, take the lock or die

    @logout:
            File object to use to replace sys.stdout
            Warning: original file object potentially closed
            None => stdout will be /dev/null

    @logerr:
            File object to use to replace sys.stderr
            Warning: original file object potentially closed
            None => stderr will be /dev/null
    """
    try:
        pid = os.fork()
        if pid > 0:
            os._exit(0)
    except Exception:
        log.exception("first fork() failed")
        sys.exit(1)
    os.setsid()
    os.umask(0)
    os.chdir('/')
    try:
        pid = os.fork()
        if pid > 0:
            os._exit(0)
    except Exception:
        log.exception("second fork() failed")
        sys.exit(1)

    pid = create_pidfile_or_die(pidfile, pidfile_lock)

    try:
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
        elif logout.fileno() == 1:
            sys.stdout = logout
        else:
            os.dup2(logout.fileno(), 1)
            logout.close()

        if logerr is None:
            sys.stderr = open(os.devnull, 'w', 0)
        elif logerr.fileno() == 2:
            sys.stderr = logerr
        else:
            os.dup2(logerr.fileno(), 2)
            logerr.close()
    except Exception:
        log.exception("error during file descriptor redirection")

    return pid


if __name__ == '__main__':
    def stupidtest():
        import time
        logging.basicConfig(level=logging.DEBUG, filename="/home/xilun/daemon_log")
        out = open("/home/xilun/daemon_out", "w")
        err = open("/home/xilun/daemon_err", "w")
        daemonize(pidfile="/home/xilun/pidfile", pidfile_lock=True, logout=out, logerr=err)
        print "loleuh"
        print >> sys.stderr, "loleuhmais"
        time.sleep(10)
    stupidtest()
