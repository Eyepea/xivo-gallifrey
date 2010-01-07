#!/bin/sh

#
# XiVO Base-Config
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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
XIVO_CODENAME="Dalek"
ASTSQLITE_FILE="${ASTSQLITE_SCRIPTS_DIR}/schema/astsqlite.sql"

source "`dirname $0`/../prepend.sh"

if ! ${SQLITE} "${ASTSQLITE_DB}" 'SELECT mode FROM callerid LIMIT 1' 1>/dev/null 2>&1;
then
	echo "Wrong Asterisk Database version: too old"
	exit 1
fi

if ${SQLITE} "${ASTSQLITE_DB}" 'SELECT lastms FROM usersip LIMIT 1' 1>/dev/null 2>&1;
then
	echo "Wrong Asterisk Database version: already upgraded"
	exit 1
fi

echo "Backup old Asterisk Database"

if [ -e "${ASTSQLITE_BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-dalek" ];
then
	mv	"${ASTSQLITE_BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-dalek"\
		"${ASTSQLITE_BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-dalek-`date +%Y%m%d%H%M%S`"
fi

cp -a "${ASTSQLITE_DB}" "${ASTSQLITE_BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-dalek"

echo "Performing upgrade..."

echo "    .'   '===||=|)"

ASTSQLITE_FILE_TMP=`mktemp`
ASTSQLITE_UPGRADE_TMP=`mktemp`
ASTSQLITE_SCHEMA_TMP=`mktemp`
ASTSQLITE_DROP_TABLE_TMP=`mktemp`
ASTSQLITE_EXTENUMBERS_TMP=`mktemp`

echo "   |======|"

sed -r	's/^DROP TABLE ([^ ;]+);$/DROP TABLE tmp_\1;/
	 s/^CREATE TABLE ([^ ;]+) \($/CREATE TABLE tmp_\1 \(/
	 s/^CREATE (UNIQUE )?INDEX ([^ ]+) ON ([^ \(;]+)\(([^\)]+)\);$/CREATE \1INDEX tmp_\2 ON tmp_\3(\4);/
	 s/^INSERT INTO ([^ ;]+) VALUES/INSERT INTO tmp_\1 VALUES/' \
	"${ASTSQLITE_FILE}" > "${ASTSQLITE_FILE_TMP}"

${SQLITE} "${ASTSQLITE_DB}" < "${ASTSQLITE_FILE_TMP}" |sed "${SED_SUPPRESS_ERROR}"

echo "   |======|       ---< EXTERMINATE!!!"

echo 'BEGIN TRANSACTION;' > "${ASTSQLITE_UPGRADE_TMP}"

for FILE in "${ASTSQLITE_SCRIPTS_DIR}"/*.sql;
do
	cat "${FILE}" >> "${ASTSQLITE_UPGRADE_TMP}"
done

echo 'COMMIT;' >> "${ASTSQLITE_UPGRADE_TMP}"

echo "   [IIIIII[\--("

${SQLITE} "${ASTSQLITE_DB}" < "${ASTSQLITE_UPGRADE_TMP}" |sed "${SED_SUPPRESS_ERROR}"

echo "   |_______|"

echo 'BEGIN TRANSACTION;' > "${ASTSQLITE_SCHEMA_TMP}"

${SQLITE} "${ASTSQLITE_DB}" ".tables tmp_"|while read SQL_TMP_TABLES;
do
	for SQL_TMP_TABLE_NAME in `echo -n ${SQL_TMP_TABLES}`;
	do
		echo	"DROP TABLE ${SQL_TMP_TABLE_NAME};" >> "${ASTSQLITE_DROP_TABLE_TMP}"

		SQL_REAL_TABLE_NAME="${SQL_TMP_TABLE_NAME:4}"

		echo	"DROP TABLE ${SQL_REAL_TABLE_NAME};" >> "${ASTSQLITE_SCHEMA_TMP}"

		${SQLITE}	"${ASTSQLITE_DB}" ".schema ${SQL_TMP_TABLE_NAME}" |\
		sed -r	's/^CREATE TABLE tmp_([^ ;]+) \($/CREATE TABLE \1 \(/
			 s/^CREATE (UNIQUE )?INDEX tmp_([^ ]+) ON tmp_([^ \(;]+)\(([^\)]+)\);$/CREATE \1INDEX \2 ON \3(\4);/' >> \
			"${ASTSQLITE_SCHEMA_TMP}"

		echo -e	".mode insert ${SQL_REAL_TABLE_NAME}\nSELECT * FROM ${SQL_TMP_TABLE_NAME};\n" |\
			${SQLITE} "${ASTSQLITE_DB}" >> \
			"${ASTSQLITE_SCHEMA_TMP}"
	done
done

echo 'COMMIT;' >> "${ASTSQLITE_SCHEMA_TMP}"

echo "   ( O O O )"

${SQLITE} "${ASTSQLITE_DB}" < "${ASTSQLITE_SCHEMA_TMP}" |sed "${SED_SUPPRESS_ERROR}"
${SQLITE} "${ASTSQLITE_DB}" < "${ASTSQLITE_DROP_TABLE_TMP}" |sed "${SED_SUPPRESS_ERROR}"

echo "  ( O  O  O )"

ASTSQLITE_FIX_EXTENUMBERS=(`${SQLITE} "${ASTSQLITE_DB}" < "${ASTSQLITE_SCRIPTS_DIR}/fix/extenumbers.sql"`)

echo " (  O  O  O  )"

echo 'BEGIN TRANSACTION;' > "${ASTSQLITE_EXTENUMBERS_TMP}"

for EXTENUMBERS_FIELDS in "${ASTSQLITE_FIX_EXTENUMBERS[@]}";
do
	USERFEATURESID=`echo ${EXTENUMBERS_FIELDS}|cut -d\| -f1`
	EXTENHASH=`echo ${EXTENUMBERS_FIELDS}|echo -n $(cut -d\| -f2)|sha1sum|cut -d' ' -f1`

	echo	"UPDATE extenumbers
		 SET extenhash = '${EXTENHASH}'
		 WHERE id = '${USERFEATURESID}';" >> "${ASTSQLITE_EXTENUMBERS_TMP}"
done

echo 'COMMIT;' >> "${ASTSQLITE_EXTENUMBERS_TMP}"

echo " (__O__O__O__)"

${SQLITE} "${ASTSQLITE_DB}" < "${ASTSQLITE_EXTENUMBERS_TMP}"

rm -f	"${ASTSQLITE_FILE_TMP}" \
	"${ASTSQLITE_UPGRADE_TMP}" \
	"${ASTSQLITE_SCHEMA_TMP}" \
	"${ASTSQLITE_DROP_TABLE_TMP}" \
	"${ASTSQLITE_EXTENUMBERS_TMP}"

echo "[_____________]"

echo "done !"
