"""System functions

Copyright (C) 2008  Proformatique

WARNING: Linux specific module - and maybe even Debian specific module
"""

__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2008  Proformatique

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA..
"""

import os
import shutil
import subprocess

from xivo import trace_null
from xivo import except_tb


def sync_no_oserror(trace=trace_null):
    """
    Call /bin/sync.
    Catch and trace OSError exceptions.
    """
    try:
        subprocess.call("/bin/sync", close_fds=True)
    except OSError:
        trace.warning("sync_no_oserror: call of /bin/sync failed")
        except_tb.log_exception(trace.warning)


def rm_rf(path):
    """
    Recursively (if needed) delete path.
    """
    if os.path.isdir(path) and not os.path.islink(path):
        shutil.rmtree(path)
    elif os.path.lexists(path):
        os.remove(path)


def flush_sync_file_object(fo):
    """
    Flush internal buffers of @fo, then ask the OS to flush its own buffers.
    """
    fo.flush()
    os.fsync(fo.fileno())


def file_writelines_flush_sync(path, lines):
    """
    Fill file at @path with @lines then flush all buffers
    (Python and system buffers)
    """
    fp = file(path, "w")
    try:
        fp.writelines(lines)
        flush_sync_file_object(fp)
    finally:
        fp.close()


def file_w_create_directories(filepath):
    """
    Recursively create some directories if needed so that the directory where
    @filepath must be written exists, then open it in "w" mode and return the
    file object.
    """
    dirname = os.path.dirname(filepath)
    if dirname and dirname != os.path.curdir and not os.path.isdir(dirname):
        os.makedirs(dirname)
    return file(filepath, "w")
