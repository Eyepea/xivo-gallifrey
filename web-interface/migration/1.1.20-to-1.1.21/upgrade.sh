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
	#XIVOSQLITE_DB="/var/lib/pf-xivo-web-interface/sqlite/xivo.db"
	XIVOSQLITE_DB="/tmp/plope"
	# backuping
	cp -a "${XIVOSQLITE_DB}" "${BACKUP_DIR}/${XIVOSQLITE_DB_FILENAME}-pre-1.1.21-`date +%Y%m%d%H%M%S`"

	echo "  . Upgrading SQLITE database schema..."
	for FILE in `dirname $0`/sqlite/*.sql; do
		sqlite "${XIVOSQLITE_DB}" < "${FILE}";
	done

	return 0;
}

mysql_migrate() {
	# do nothing
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
