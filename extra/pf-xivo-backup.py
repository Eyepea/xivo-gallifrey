#!/usr/bin/env python
# -*- coding: utf8 -*-

__license__ = """
    Copyright (C) 2012  Proformatique <technique@proformatique.com>

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
__author__ = "Guillaume Bour <gbour@proformatique.com>"

import sys, os, os.path, glob, subprocess
from datetime import date

BACKUP_DIR = '/var/backups/pf-xivo-backup'
BACKUP_CMD = ["/usr/bin/mysqldump","--defaults-file=/etc/mysql/debian.cnf","--compact","--no-create-info","DB","TBL","--where",
                            "%(fld)s >= \"%(year)04d-%(month)02d-01 %%\" AND %(fld)s <= \"%(year)04d-%(month)02d-31 %%\""]
CLEAR_CMD  = ["/usr/bin/mysql","--defaults-file=/etc/mysql/debian.cnf","DB","-e","DELETE FROM %(tbl)s WHERE %(fld)s <= \"%(year)04d-%(month)02d-31 %%\""]

def build_filename(db, table, month, year):
    """Build backup filename
    """
    return os.path.join(BACKUP_DIR, "%s.%s.%02d%04d" % (db, table, month, year))

def build_quarters(active, backup):
    """Build backup quarters list
    """
    now  = date.today()
    curq = [now.month, now.year]

    quarters = []
    if active == 0:
        quarters.append(list(curq))

    for i in xrange(active+backup-1):
        curq[0] -= 1
        if curq[0] < 1:
            curq[0] = 12; curq[1] -= 1

        if i >= active - 1:
            quarters.append(list(curq))

    return quarters

def execute(msg, cmd, outfile=None):
    """Execute command
    """
    print msg

    stdout = file(outfile, "a+") if outfile is not None else subprocess.PIPE
    proc = subprocess.Popen(cmd, stdout=stdout, stderr=subprocess.PIPE)
    if proc.wait() != 0:
        print proc.stderr.read(); return False

    if outfile is not None:
        stdout.close()
    return True


def backup(db, table, field, active_quarters, backup_quarters):
    print "%s.%s:" % (db, table)

    quarters = build_quarters(active_quarters, backup_quarters)
    #print quarters
    files    = [build_filename(db, table, *q) for q in quarters]
    #print files

    # 1. delete outdated backup files
    for f in set(glob.glob(os.path.join(BACKUP_DIR, "%s.%s.*" % (db, table)))).difference(set(files)):
        print " . Deleting %s" % f
        os.remove(f)
    
    # backup datas
    try:
        if not os.path.exists(BACKUP_DIR):
            os.makedirs(BACKUP_DIR)
    except Exception, e:
        print e; sys.exit(1)

    for i in xrange(backup_quarters):
        cmd = BACKUP_CMD[0:4] + [db, table] + [BACKUP_CMD[6], BACKUP_CMD[-1] % {'fld': field, 'year': quarters[i][1], 'month': quarters[i][0]}]
        if not execute(" . Backuping %04d-%02d quarter" % (quarters[i][1], quarters[i][0]), cmd, files[i]):
            sys.exit(1)

    # delete older datas from database
    cmd = CLEAR_CMD[0:2] + [db, CLEAR_CMD[3], CLEAR_CMD[-1] % {'tbl': table, 'fld': field, 'year': quarters[0][1], 'month': quarters[0][0]}]
    execute(" . Deleting datas from database (older than %04d-%02d-31)" % (quarters[0][1], quarters[0][0]) , cmd)


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "Usage: %s active-quarters backup-quarters" % sys.argv[0]
        sys.exit(1)

    for db, table, field in [('asterisk', 'cdr', 'calldate'),('xivo','ctilog', 'eventdate'),('stats','queue_stat', '')]:
        backup(db, table, field, int(sys.argv[1]), int(sys.argv[2]))
