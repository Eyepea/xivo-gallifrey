#!/bin/sh

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
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
XIVOSQLITE_SCRIPTS_DIR="`dirname $0`/scripts/xivo-sqlite"

XIVOSQLITE_DB_FILENAME="xivo.db"
XIVOSQLITE_DB="/var/lib/pf-xivo-web-interface/sqlite/${XIVOSQLITE_DB_FILENAME}"

XIVOSQLITE_FILE="${XIVOSQLITE_SCRIPTS_DIR}/schema.sql"

SED_SUPPRESS_ERROR="/^DROP TABLE /{N; /\nSQL error: no such table:/d}"

if ! ask_yn_question "Would you like to upgrade XIVO Web Interface SQLite Database?";
then
	exit 0
fi

if [ ! -d "${XIVOSQLITE_SCRIPTS_DIR}" ];
then
	echo "Missing XIVO Web Interface SQLite scripts directory"
	exit 1
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

if [ ! -r "${XIVOSQLITE_FILE}" ];
then
	echo "XIVO Web Interface K-9 SQLite schema not readable"
	exit 1
fi

if ! sqlite "${XIVOSQLITE_DB}" 'SELECT id FROM server LIMIT 1' 1>/dev/null 2>&1;
then
	echo "Wrong XIVO Web Interface Database version: too old"
	exit 1
fi

if sqlite "${XIVOSQLITE_DB}" 'SELECT id FROM entity LIMIT 1' 1>/dev/null 2>&1;
then
	echo "Wrong XIVO Web Interface Database version: already upgraded"
	exit 1
fi

echo "Backup old XIVO Web Interface Database"

if [ -e "${XIVOSQLITE_BACKUP_DIR}/${XIVOSQLITE_DB_FILENAME}-k-9" ];
then
	mv	"${XIVOSQLITE_BACKUP_DIR}/${XIVOSQLITE_DB_FILENAME}-k-9"\
		"${XIVOSQLITE_BACKUP_DIR}/${XIVOSQLITE_DB_FILENAME}-k-9-`date +%Y%m%d%H%M%S`"
fi

cp -a "${XIVOSQLITE_DB}" "${XIVOSQLITE_BACKUP_DIR}/${XIVOSQLITE_DB_FILENAME}-k-9"

echo "Performing upgrade..."

sqlite "${XIVOSQLITE_DB}" < "${XIVOSQLITE_FILE}" |sed "${SED_SUPPRESS_ERROR}"

echo "done !"
