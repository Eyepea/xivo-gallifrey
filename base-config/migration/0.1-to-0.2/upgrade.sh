#!/bin/sh

#
# XiVO Base-Config
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

BACKUPDIR=$1
SCRIPTSDIR="`dirname $0`/scripts"

ASTVMDIR="/var/spool/asterisk/voicemail"
ASTMOHDIR="/usr/share/asterisk/moh"
ASTSQLITEDBFILENAME="astsqlite"
ASTSQLITEDB="/var/lib/asterisk/${ASTSQLITEDBFILENAME}"
ASTSQLITEFILE="/usr/share/pf-xivo-base-config/astsqlite.db.sql"

XIVOCONFDIR="/etc/pf-xivo/web-interface"
XIVOSQLITEDBFILENAME="xivo.db"
XIVOSQLITEDB="/var/lib/pf-xivo-web-interface/sqlite/${XIVOSQLITEDBFILENAME}"
XIVOSQLITEFILE="/usr/share/pf-xivo-base-config/xivo.db.sql"

if [ ! -d "${SCRIPTSDIR}" ]
then
	echo "Missing scripts directory"
	exit 1
fi

if [ ! -w "${SCRIPTSDIR}" ]
then
	echo "Scripts directory not writable"
	exit 1
fi

if [ "${BACKUPDIR}" = "" ] || [ "${BACKUPDIR}" = "-h" ] || [ "${BACKUPDIR}" = "--help" ]
then
	echo "Usage: $0 /path/for/backupfile"
	exit 1
fi

if [ -f "${BACKUPDIR}" ]
then
	echo "Invalid backup path"
	exit 1
fi

if [ "`whoami`" != "root" ]
then
	echo "You need to be root to use this script..."
	exit 1
fi

if [ ! -e "${BACKUPDIR}" ]
then
	mkdir -p "${BACKUPDIR}"	
fi

if [ ! -w "${BACKUPDIR}" ]
then
	echo "Backup directory not writable"
	exit 1
fi

if [ ! -f "${ASTSQLITEDB}" ]
then
	echo "SQLite Asterisk DataBase does not exist"
	exit 1
fi

if [ ! -r "${ASTSQLITEFILE}" ]
then
	echo "Asterisk Torchwood SQLite schema not readable"
	exit 1
fi

if [ ! -f "${XIVOSQLITEDB}" ]
then
	echo "SQLite XIVO DataBase does not exist"
fi

if [ ! -r "${XIVOSQLITEFILE}" ]
then
	echo "XIVO Torchwood SQLite schema not readable"
fi

echo "Backup old pf-xivo-web-interface-arcadia datastorage"

if [ -e "${BACKUPDIR}/${ASTSQLITEDBFILENAME}" ]
then
	mv "${BACKUPDIR}/${ASTSQLITEDBFILENAME}" "${BACKUPDIR}/${ASTSQLITEDBFILENAME}-`date +%Y%m%d%H%M%S`"
fi

if [ -e "${BACKUPDIR}/${XIVOSQLITEDBFILENAME}" ]
then
	mv "${BACKUPDIR}/${XIVOSQLITEDBFILENAME}" "${BACKUPDIR}/${XIVOSQLITEDBFILENAME}-`date +%Y%m%d%H%M%S`"
fi

cp -a "${ASTSQLITEDB}" "${BACKUPDIR}"
cp -a "${XIVOSQLITEDB}" "${BACKUPDIR}"

echo "Performing upgrade..."

sed -e 's/^DROP TABLE \([^ ;]\+\);$/DROP TABLE tmp_\1;/' \
	"${ASTSQLITEFILE}" > \
	"${SCRIPTSDIR}/astsqlite_drop-table.db.sql.tmp"

sed -e 's/^CREATE TABLE \([^ ;]\+\) ($/CREATE TABLE tmp_\1 (/' \
	"${SCRIPTSDIR}/astsqlite_drop-table.db.sql.tmp" > \
	"${SCRIPTSDIR}/astsqlite_create-table.db.sql.tmp"

sed -e 's/^CREATE \(UNIQUE \)\?INDEX \([^ ]\+\) ON \([^ (;]\+\)(\([^)]\+\));$/CREATE \1INDEX tmp_\2 ON tmp_\3(\4);/' \
	"${SCRIPTSDIR}/astsqlite_create-table.db.sql.tmp" > \
	"${SCRIPTSDIR}/astsqlite_create-index.db.sql.tmp"

sed -e 's/^INSERT INTO \([^ ;]\+\) VALUES/INSERT INTO tmp_\1 VALUES/' \
	"${SCRIPTSDIR}/astsqlite_create-index.db.sql.tmp" > \
	"${SCRIPTSDIR}/astsqlite.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/astsqlite.sql.tmp" 1>/dev/null

if [ "`sqlite "${ASTSQLITEDB}" 'SELECT id FROM cdr LIMIT 1' 2>/dev/null`" != "" ] 
then
	sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/cdr.sql"
fi

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/generaliax-build-update.sql" > "${SCRIPTSDIR}/generaliax-update.sql.tmp"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/generaliax-update.sql.tmp"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/generaliax.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/generalqueue-build-update.sql" > "${SCRIPTSDIR}/generalqueue-update.sql.tmp"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/generalqueue-update.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/generalsip-build-update.sql" > "${SCRIPTSDIR}/generalsip-update.sql.tmp"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/generalsip-update.sql.tmp"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/generalsip.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/generalvoicemail-build-update.sql" > "${SCRIPTSDIR}/generalvoicemail-update.sql.tmp"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/generalvoicemail-update.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/groupfeatures.sql"

sqlite "${ASTSQLITEDB}" "SELECT name, id FROM tmp_groupfeatures WHERE name LIKE '%|%'" | \
	for GROUPVALUES in `sed -e 's/^\([^\|]\+\)|[a-z0-9]\+|\([0-9]\+\)$/\1:\2/g'`;
	do
		GROUPNAME=`echo ${GROUPVALUES}|cut -d: -f1`;
		GROUPID=`echo ${GROUPVALUES}|cut -d: -f2`;
		echo UPDATE tmp_groupfeatures SET name = \'${GROUPNAME}\', deleted = 1 WHERE id = ${GROUPID}\;;
	done > "${SCRIPTSDIR}/groupfeatures-name.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/groupfeatures-name.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/meetme.sql"

sqlite "${ASTSQLITEDB}" "SELECT name, id FROM tmp_meetmefeatures" | \
	for MEETMEVALUES in `sed 's/ /-/g; s/^\(.\+\)|\([0-9]\+\)$/\1:\2/g'`;
	do
		MEETMENAME=`echo ${MEETMEVALUES}|cut -d: -f1|sed 's/[^a-z0-9_\.-]//gi'`;
		MEETMEID=`echo ${MEETMEVALUES}|cut -d: -f2`;
		echo UPDATE tmp_meetmefeatures SET name = \'${MEETMENAME}\' WHERE id = ${MEETMEID}\;;
	done > "${SCRIPTSDIR}/meetmefeatures-name.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/meetmefeatures-name.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/musiconhold.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/queue.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/useriax.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/usersip.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/uservoicemail.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/trunkiax.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/trunksip.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/trunk-check-error.sql" > "${SCRIPTSDIR}/trunk-remove-error.sql.tmp"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/trunk-remove-error.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/queuemember.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/incall.sql"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/incall-build-update.sql" > "${SCRIPTSDIR}/incall-update.sql.tmp"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/incall-update.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/phone.sql"

sqlite "${ASTSQLITEDB}" "SELECT extenhash, id FROM tmp_extenumbers WHERE extenhash LIKE 'tohash;%'" | \
	for EXTENVALUES in `sed -e 's/^tohash;\([0-9]\+\)|\([0-9]\+\)$/\1:\2/g'`;
	do
		EXTENUMBER=`echo ${EXTENVALUES}|echo -n $(cut -d: -f1)|sha1sum|sed 's/ \+\-$//'`;
		EXTENUMID=`echo ${EXTENVALUES}|cut -d: -f2`;
		echo UPDATE tmp_extenumbers SET extenhash = \'${EXTENUMBER}\' WHERE id = ${EXTENUMID}\;;
	done > "${SCRIPTSDIR}/extenumbers-hash.sql.tmp"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/extenumbers-hash.sql.tmp"

for DIR in "${ASTMOHDIR}"/*; do mv "${DIR}" `echo "${DIR}" | tr [:upper:] [:lower:]` 2>/dev/null || true; done

sqlite "${SCRIPTSDIR}/astsqlite.db.sql.tmp" < "${ASTSQLITEFILE}" 1>/dev/null
sqlite "${SCRIPTSDIR}/astsqlite.db.sql.tmp" .schema > "${SCRIPTSDIR}/astsqlite.sql.tmp"

for SQLTABLE in `sqlite "${ASTSQLITEDB}" ".table tmp_"`;
do echo -e .mode insert `echo ${SQLTABLE} | sed 's/^tmp_//'`"\nSELECT * FROM ${SQLTABLE};\n";
done > "${SCRIPTSDIR}/mode-insert.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/mode-insert.sql.tmp" >> "${SCRIPTSDIR}/astsqlite.sql.tmp"

> "${ASTSQLITEDB}"
sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/astsqlite.sql.tmp" 1>/dev/null

rm -rf "${SCRIPTSDIR}"/*.sql.tmp

> "${XIVOSQLITEDB}"
sqlite "${XIVOSQLITEDB}" < "${XIVOSQLITEFILE}" 1>/dev/null

rm -rf "${XIVOCONFDIR}/policy.inc"

if [ -d "${ASTVMDIR}/local-extensions" ]
then
	if [ ! -d "${ASTVMDIR}/default" ]
	then
		mv "${ASTVMDIR}/local-extensions" "${ASTVMDIR}/default"
	else
		cp -aR "${ASTVMDIR}"/local-extensions/. "${ASTVMDIR}/default"
		rm -rf "${ASTVMDIR}/local-extensions"
	fi
fi

echo "done !"
