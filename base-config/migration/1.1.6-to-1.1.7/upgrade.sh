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
	cp -a "${ASTSQLITE_DB}" "${BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-1.1.6-to-1.1.7-`date +%Y%m%d%H%M%S`"

	echo "  . Upgrading SQLITE database schema..."
	for meetmeval in `sqlite /var/lib/asterisk/astsqlite "SELECT var_val FROM	staticmeetme WHERE category = 'rooms' AND var_name = 'conf'"`; do
		confno=`echo "$meetmeval"|cut -d',' -f1`
		number=`sqlite /var/lib/asterisk/astsqlite "SELECT number FROM meetmefeatures	WHERE name = '$confno'"`;

		if [[ $number != "" ]]; then
			meetmeval2=${meetmeval/$confno/$number}
			`sqlite /var/lib/asterisk/astsqlite "UPDATE staticmeetme SET var_val = '$meetmeval2' WHERE category = 'rooms' AND var_name = 'conf' AND var_val = '$meetmeval'"`;
		fi
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

	# backuping
	mysqldump --defaults-extra-file=/etc/mysql/debian.cnf ${DBNAME} > "${BACKUP_DIR}/${DBNAME}-mysql.dump-1.1.6-to-1.1.7-`date +%Y%m%d%H%M%S`";
	if [ $? != 0 ]; then
		echo "  . Can't backup ${DBNAME} mysql database";
		return 1;
	fi

	for meetmeval in `echo "SELECT var_val FROM	staticmeetme WHERE category = 'rooms' AND var_name = 'conf'"|mysql --defaults-extra-file=/etc/mysql/debian.cnf -N ${DBNAME}`; do
		confno=`echo "$meetmeval"|cut -d',' -f1`
		number=`echo "SELECT number FROM meetmefeatures	WHERE name = '$confno'"|mysql --defaults-extra-file=/etc/mysql/debian.cnf -N ${DBNAME}`;

		if [[ $number != "" ]]; then
			meetmeval2=${meetmeval/$confno/$number}
			`echo "UPDATE staticmeetme SET var_val = '$meetmeval2' WHERE category =	'rooms' AND var_name = 'conf' AND var_val = '$meetmeval'"|mysql	--defaults-extra-file=/etc/mysql/debian.cnf ${DBNAME}`;
		fi
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
