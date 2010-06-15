#!/bin/bash

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

set -e

source "`dirname $0`/../functions.sh"

XIVOSQLITE_BACKUP_DIR=$1
XIVOSQLITE_DB="/var/lib/pf-xivo-web-interface/sqlite/xivo.db"


if ! ask_yn_question "Would you like to upgrade XIVO Web Interface SQLite Database?";
then
	exit 0
fi

if [ "${XIVOSQLITE_BACKUP_DIR}" == "" ]\
|| [ "${XIVOSQLITE_BACKUP_DIR}" == "-h" ]\
|| [ "${XIVOSQLITE_BACKUP_DIR}" == "--help" ];
then
	echo "Usage: $0 /path/for/backupfile"
	exit 1
fi

if [ -f "${XIVOSQLITE_BACKUP_DIR}" ];
then
	echo "Invalid backup path"
	exit 1
fi

if [ "`whoami`" != "root" ];
then
	echo "You need to be root to use this script..."
	exit 1
fi

if [ ! -e "${XIVOSQLITE_BACKUP_DIR}" ];
then
	mkdir -p "${XIVOSQLITE_BACKUP_DIR}"
fi

if [ ! -w "${XIVOSQLITE_BACKUP_DIR}" ];
then
	echo "Backup directory not writable"
	exit 1
fi

if [ ! -f "${XIVOSQLITE_DB}" ];
then
	echo "SQLite XIVO Web Interface Database does not exist"
	exit 1
fi

echo "Backup old XIVO Web Interface Database"
cp -a "${XIVOSQLITE_DB}" "${XIVOSQLITE_BACKUP_DIR}/xivo.db-1.0-`date +%Y%m%d%H%M%S`"

echo "Performing upgrade..."
sqlite "${XIVOSQLITE_DB}" < "`dirname $0`/scripts/sqlite.schema.sql"

echo "done !"
