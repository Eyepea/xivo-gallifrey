__version__ = "$Revision$ $Date$"
__license__ = """
    Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>

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
import ConfigParser

from xivo_agid import agid

CONFIG_FILE = '/etc/asterisk/xivo_callback_on_congestion.conf'

max_retries = None
retry_time = None
wait_time = None

def get_uid_gid(name):
    # pylint: disable-msg=W0612
    pw_name, pw_passwd, pw_uid, pw_gid, pw_gecos, pw_dir, pw_shell = pwd.getpwnam(name)
    return pw_uid, pw_gid

ASTERISK_UID, ASTERISK_GID = get_uid_gid("asterisk")

def callback_on_congestion(agi, cursor, args):
    srcnum = agi.get_variable('XIVO_SRCNUM')
    dstnum = agi.get_variable('XIVO_DSTNUM')
    context = agi.get_variable('XIVO_CONTEXT')
    spooldir = agi.get_variable('GETCONF(SPOOL_DIR)')

    if srcnum in (None, ''):
        agi.dp_break("Unable to find srcnum, srcnum = %r" % srcnum)

    if dstnum in (None, ''):
        agi.dp_break("Unable to find dstnum, dstnum = %r" % dstnum)

    if not context:
        agi.dp_break("Unable to find context, context = %r" % context)

    if not spooldir:
        agi.dp_break("Unable to fetch AST_SPOOL_DIR")

    mtime = time.time()
    filepath = "%s/%%s/%s-to-%s-%s.call" % (spooldir, srcnum, dstnum, int(mtime))

    tmpfile = filepath % "tmp"
    realfile = filepath % "outgoing"

    f = open(tmpfile, 'w')
    f.write("Channel: Local/%s\n"
            "MaxRetries: %d\n"
            "RetryTime: %d\n"
            "WaitTime: %d\n"
            "CallerID: %s\n"
            "Context: %s\n"
            "Extension: %s\n"
            "Priority: 1\n" % (srcnum, max_retries, retry_time, wait_time, srcnum, context, dstnum))
    f.close()

    os.utime(tmpfile, (mtime, mtime))
    os.chown(tmpfile, ASTERISK_UID, ASTERISK_GID)
    os.rename(tmpfile, realfile)

def setup(cursor):
    global max_retries
    global retry_time
    global wait_time

    config = ConfigParser.RawConfigParser()
    config.readfp(open(CONFIG_FILE))
    max_retries = config.getint('general', 'max_retries')
    retry_time = config.getint('general', 'retry_time')
    wait_time = config.getint('general', 'wait_time')

agid.register(callback_on_congestion, setup)
