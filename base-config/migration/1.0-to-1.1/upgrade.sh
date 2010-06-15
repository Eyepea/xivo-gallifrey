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

ASTSQLITE_BACKUP_DIR=$1
XIVO_CODENAME="Gallifrey"
ASTSQLITE_FILE="${ASTSQLITE_SCRIPTS_DIR}/schema/astsqlite.sql"

source "`dirname $0`/../prepend.sh"


echo "Backup old Asterisk Database"
cp -a "${ASTSQLITE_DB}" "${ASTSQLITE_BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-dalek-`date +%Y%m%d%H%M%S`"


echo "Performing upgrade..."
for FILE in "${ASTSQLITE_SCRIPTS_DIR}"/*.sql; do
	echo $FILE;
#	ask_yn_question;
	sqlite "${ASTSQLITE_DB}" < "${FILE}";
done

echo "done !"
