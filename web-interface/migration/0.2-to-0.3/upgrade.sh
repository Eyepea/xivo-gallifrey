#!/bin/sh

set -e

BACKUPDIR=$1
SCRIPTSDIR="`dirname $0`/scripts"

ASTSQLITEDBFILENAME="astsqlite"
ASTSQLITEDB="/var/lib/asterisk/${ASTSQLITEDBFILENAME}"
ASTSQLITEFILE="${SCRIPTSDIR}/sqlite.tmp.schema.sql"

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
	echo "Asterisk Tardis SQLite schema not readable"
	exit 1
fi

if [ "`sqlite "${ASTSQLITEDB}" 'SELECT id FROM dialstatus LIMIT 1' 2>&1 1>/dev/null`" != "" ]
then
	echo "Wrong Asterisk Database version"
	exit 1
fi

echo "Backup old pf-xivo-web-interface-torchwood datastorage"

if [ -e "${BACKUPDIR}/${ASTSQLITEDBFILENAME}-torchwood" ]
then
	mv "${BACKUPDIR}/${ASTSQLITEDBFILENAME}-torchwood" "${BACKUPDIR}/${ASTSQLITEDBFILENAME}-torchwood-`date +%Y%m%d%H%M%S`"
fi

cp -a "${ASTSQLITEDB}" "${BACKUPDIR}"

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

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/dialstatus.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/outcall.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/userfeatures.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/usercustom.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/useriax.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/usersip.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/uservoicemail.sql"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/${ASTSQLITEFILE}"

for SQLTABLE in `sqlite "${ASTSQLITEDB}" ".table tmp_"`;
do echo -e .mode insert `echo ${SQLTABLE} | sed 's/^tmp_//'`"\nSELECT * FROM ${SQLTABLE};\n";
done > "${SCRIPTSDIR}/mode-insert.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/mode-insert.sql.tmp"

sqlite "${ASTSQLITEDB}" < "${SCRIPTSDIR}/add-insert.sql"

sqlite "${ASTSQLITEDB}" < `grep '^DROP TABLE tmp_' "${SCRIPTSDIR}/astsqlite_drop-table.db.sql.tmp"` 1>/dev/null

rm -rf "${SCRIPTSDIR}"/*.sql.tmp

echo "done !"
