#!/bin/bash

#
# XiVO Base-Config
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
source "`dirname $0`/../constants.sh"


sqlite_migrate() {
	if ! ask_yn_question "Would you like to upgrade XIVO Web Interface SQLite Database?";	then
		return 0;
	fi

	if [ ! -f "${ASTSQLITE_DB}" ];	then
		echo "SQLite Asterisk Database does not exist";
		return 1;
	fi

	echo "Backup old Asterisk Database"
	cp -a "${ASTSQLITE_DB}" "${BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-dalek-`date +%Y%m%d%H%M%S`"

	echo "Performing upgrade..."
	for FILE in "${ASTSQLITE_SCRIPTS_DIR}"/*.sql; do
		echo " $FILE";
		sqlite "${ASTSQLITE_DB}" < "${FILE}";
	done

	echo "done !"
	return 0;
}

mysql_migrate() {
	DBNAME='asterisk'

	if ! ask_yn_question "Would you like to upgrade XIVO Asterisk MySQL Database?";
	then
		return 0;
	fi

	echo "Backup old XIVO Asterisk Database";
	mysqldump --defaults-extra-file=/etc/mysql/debian.cnf ${DBNAME} > "${BACKUP_DIR}/${DBNAME}-mysql.dump-1.0-`date +%Y%m%d%H%M%S`";
	if [ $? != 0 ]; then
		echo "Can't backup ${DBNAME} mysql database";
		return 1;
	fi

	echo "Performing upgrade... ";
	for FILE in `dirname $0`/scripts/astmysql/*.sql; do
		echo " $FILE";
		mysql --defaults-extra-file=/etc/mysql/debian.cnf ${DBNAME} < ${FILE};
	done
	
	echo "mysql: done!";
	return 0;
}



if [ "$2" == "" ]; then
	echo "Usage: $0 {sqlite|mysql|both} /path/to/backupfile"
	exit 1
fi

BACKUP_DIR=$2
if [ -f "${BACKUP_DIR}" ]; then
	echo "Invalid backup path"
	exit 1
fi

if [ "`whoami`" != "root" ]; then
	echo "You need to be root to use this script..."
	exit 1
fi

if [ ! -e "${BACKUP_DIR}" ]; then
	mkdir -p "${BACKUP_DIR}"
fi

if [ ! -w "${BACKUP_DIR}" ]; then
	echo "Backup directory not writable"
	exit 1
fi


case $1 in
	sqlite)
		sqlite_migrate;
	;;
	mysql)
		mysql_migrate;
	;;
	both)
		sqlite_migrate;
		mysql_migrate;
	;;
	*)
		echo "$0 {sqlite|mysql|both} /path/to/backupfile"
		exit 1
	;;
esac
