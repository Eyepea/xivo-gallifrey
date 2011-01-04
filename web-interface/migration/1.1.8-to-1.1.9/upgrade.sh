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


sqlite_migrate() {
	XIVOSQLITE_DB="/var/lib/pf-xivo-web-interface/sqlite/xivo.db"
	return 0;
}

mysql_migrate() {
	DBNAME='xivo'

	isrunning=`dpkg -l mysql-server|grep ii|wc -l`;
	if [[ $isrunning == 0 ]]; then 
		return 0;
	fi

	echo "  . Upgrading MYSQL database schema... ";

	# backuping
	mysqldump --defaults-extra-file=/etc/mysql/debian.cnf ${DBNAME} > "${BACKUP_DIR}/${DBNAME}-mysql.dump-pre-1.1.9-`date +%Y%m%d%H%M%S`";
	if [ $? != 0 ]; then
		echo "  . Can't backup ${DBNAME} mysql database";
		return 1;
	fi

	echo 'DROP INDEX `queue_info_call_time_t_index` ON `queue_info`' | mysql --defaults-extra-file=/etc/mysql/debian.cnf --force ${DBNAME} 2>/dev/null;
	echo 'DROP INDEX `queue_info_queue_name_index` ON `queue_info`' | mysql --defaults-extra-file=/etc/mysql/debian.cnf --force ${DBNAME} 2>/dev/null;
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
