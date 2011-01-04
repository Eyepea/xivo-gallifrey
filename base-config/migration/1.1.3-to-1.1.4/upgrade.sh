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
	if [ ! -f "${ASTSQLITE_DB}" ];	then
		return 1;
	fi

	# backuping
	cp -a "${ASTSQLITE_DB}" "${BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-1.1.3-to-1.1.4-`date +%Y%m%d%H%M%S`"

	echo "  . Upgrading SQLITE database schema..."
	for FILE in `dirname $0`/sqlite/*.sql; do
		sqlite "${ASTSQLITE_DB}" < "${FILE}";
	done

	return 0;
}

mysql_migrate() {
	DBNAME='asterisk'

	isrunning=`dpkg -l mysql-server|grep ii|wc -l`;
	if [[ $isrunning == 0 ]]; then 
		return 0;
	fi

	echo "  . Upgrading MYSQL database schema... ";

	# check mysql status
	if [ ! -f "/var/run/mysqld/mysqld.pid" ]; then
	  invoke-rc.d mysql start  > /dev/null 2>&1
	fi

	# backuping
	mysqldump --defaults-extra-file=/etc/mysql/debian.cnf ${DBNAME} > "${BACKUP_DIR}/${DBNAME}-mysql.dump-1.1.3-to-1.1.4-`date +%Y%m%d%H%M%S`";
	if [ $? != 0 ]; then
		echo "  . Can't backup ${DBNAME} mysql database";
		return 1;
	fi

	for FILE in `dirname $0`/mysql/*.sql; do
		mysql --defaults-extra-file=/etc/mysql/debian.cnf ${DBNAME} < ${FILE};
	done
	
	return 0;
}



if [ "`whoami`" != "root" ]; then
	echo "You need to be root to use this script..."
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
		echo "$0 {sqlite|mysql|both}"
		exit 1
	;;
esac
