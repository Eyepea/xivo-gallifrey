"""Interworking with udev

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
import time
import os.path
from itertools import count

PERSISTENT_NET_RULES_FILE = "/etc/udev/rules.d/z25_persistent-net.rules"

LOCKPATH_PREFIX = "/dev/.udev/.lock-"

def lockpath(rules_file):
    """
    Return the path of the directory to create in order to take a lock on
    @rules_file so that udev won't bother us while we are modifying it.
    """
    return LOCKPATH_PREFIX + os.path.basename(rules_file)

def lock_rules_file(rules_file):
    """
    Take a lock on @rules_file as udev does.
    On errors, this function retries to grab the lock for as much as 30 times,
    with pauses of 1 second between successive attempts.
    If the lock could not be grabbed at all, the last exception is re-raised.
    """
    lpath = lockpath(rules_file)
    for x in count():
        try:
            os.mkdir(lpath)
        except OSError:
            if x == 29:
                raise
            time.sleep(1)
        else:
            return

def unlock_rules_file(rules_file):
    """
    Unlock a lock previously taken with lock_rules_file()
    The user should _not_ try to unlock a lock that has not been successfully
    taken with lock_rules_file(), or unlock it twice, etc.
    """
    lpath = lockpath(rules_file)
    os.rmdir(lpath)

def iter_multilines(lines):
    """
    Iterate over @lines and yield the multilines they form.  A multiline stops
    only on a "\\n" (newline) that is not preceded with a "\\\\" (backslash) or at
    end of file.  "\\\\\\n" of continued lines are stripped, as well as regular
    "\\n" at end of multilines and spaces at beginning of multilines.  Comment
    lines are not stripped.
    """
    current = []
    for line in lines:
        if line[-2:] == "\\\n":
            current.append(line[:-2])
        else:
            if line[-1:] == "\n":
                current.append(line[:-1])
            else:
                current.append(line)
            yield ''.join(current).lstrip()
            current = []
    if current:
        yield ''.join(current).lstrip()
