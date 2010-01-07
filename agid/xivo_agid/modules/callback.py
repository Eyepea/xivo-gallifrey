__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import os
import pwd
import time

from xivo_agid import agid

def get_uid_gid(name):
    # pylint: disable-msg=W0612
    pw_name, pw_passwd, pw_uid, pw_gid, pw_gecos, pw_dir, pw_shell = pwd.getpwnam(name)
    return pw_uid, pw_gid

ASTERISK_UID, ASTERISK_GID = get_uid_gid("asterisk")

def callback(agi, cursor, args):
    context = args[0]
    srcnum = agi.get_variable('XIVO_SRCNUM')
    spooldir = agi.get_variable('GETCONF(SPOOL_DIR)')

    if srcnum in (None, ''):
        agi.dp_break("Unable to find srcnum, srcnum = %r" % srcnum)

    if not spooldir:
        agi.dp_break("Unable to fetch AST_SPOOL_DIR")

    mtime = time.time() + 5
    filepath = "%s/%%s/%s-%s.call" % (spooldir, srcnum, int(mtime))

    tmpfile = filepath % "tmp"
    realfile = filepath % "outgoing"

    f = open(tmpfile, 'w')
    f.write("Channel: Local/%s@%s\n"
            "MaxRetries: 0\n"
            "RetryTime: 30\n"
            "WaitTime: 30\n"
            "CallerID: %s\n"
            "Set: XIVO_DISACONTEXT=%s\n"
            "Context: xivo-callbackdisa\n"
            "Extension: s" % (srcnum, context, srcnum, context))
    f.close()

    os.utime(tmpfile, (mtime, mtime))
    os.chown(tmpfile, ASTERISK_UID, ASTERISK_GID)
    os.rename(tmpfile, realfile)

agid.register(callback)
