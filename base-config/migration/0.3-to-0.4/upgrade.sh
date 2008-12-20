#!/bin/sh

set -e

source "`dirname $0`/../functions.sh"

ASTSQLITE_BACKUP_DIR=$1
ASTSQLITE_SCRIPTS_DIR="`dirname $0`/scripts/astsqlite"

ASTSQLITE_DB_FILENAME="astsqlite"
ASTSQLITE_DB="/var/lib/asterisk/${ASTSQLITE_DB_FILENAME}"
ASTSQLITE_FILE="/usr/share/pf-xivo-base-config/astsqlite.db.sql"

SED_SUPPRESS_ERROR="/^DROP TABLE /{N; /\nSQL error: no such table:/d}"

SQL_TABLES_EXCEPTS=(
	callfiltermember
	cdr
	extensions
	extenumbers
	features
	handynumbers
	musiconhold
	outcall
	outcalltrunk
	phone
	phonebook
	phonebookaddress
	phonebooknumber
	phonefunckey
	queuemember
	rightcallexten
	rightcallmember)

SQL_TABLES_RENAME=(
	agent
	dialstatus
	generaliax
	generalqueue
	generalsip
	generalvoicemail
	meetme
	uservoicemail)

if ! ask_yn_question "Would you like to upgrade Asterisk SQLite Database?";
then
	exit 0
fi

if [ ! -d "${ASTSQLITE_SCRIPTS_DIR}" ];
then
	echo "Missing Asterisk SQLite scripts directory"
	exit 1
fi

if [ "${ASTSQLITE_BACKUP_DIR}" == "" ]\
|| [ "${ASTSQLITE_BACKUP_DIR}" == "-h" ]\
|| [ "${ASTSQLITE_BACKUP_DIR}" == "--help" ];
then
	echo "Usage: $0 /path/for/backupfile"
	exit 1
fi

if [ -f "${ASTSQLITE_BACKUP_DIR}" ];
then
	echo "Invalid backup path"
	exit 1
fi

if [ "`whoami`" != "root" ];
then
	echo "You need to be root to use this script..."
	exit 1
fi

if [ ! -e "${ASTSQLITE_BACKUP_DIR}" ];
then
	mkdir -p "${ASTSQLITE_BACKUP_DIR}"
fi

if [ ! -w "${ASTSQLITE_BACKUP_DIR}" ];
then
	echo "Backup directory not writable"
	exit 1
fi

if [ ! -f "${ASTSQLITE_DB}" ];
then
	echo "SQLite Asterisk Database does not exist"
	exit 1
fi

if [ ! -r "${ASTSQLITE_FILE}" ];
then
	echo "Asterisk K-9 SQLite schema not readable"
	exit 1
fi

if ! sqlite "${ASTSQLITE_DB}" 'SELECT outcallid FROM outcalltrunk LIMIT 1' 1>/dev/null 2>&1;
then
	echo "Wrong Asterisk Database version: too old"
	exit 1
fi

if sqlite "${ASTSQLITE_DB}" 'SELECT mode FROM callerid LIMIT 1' 1>/dev/null 2>&1;
then
	echo "Wrong Asterisk Database version: already upgraded"
	exit 1
fi

echo "Backup old Asterisk Database"

if [ -e "${ASTSQLITE_BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-k-9" ];
then
	mv	"${ASTSQLITE_BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-k-9"\
		"${ASTSQLITE_BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-k-9-`date +%Y%m%d%H%M%S`"
fi

cp -a "${ASTSQLITE_DB}" "${ASTSQLITE_BACKUP_DIR}/${ASTSQLITE_DB_FILENAME}-k-9"

echo "Performing upgrade..."

echo " \      oo"

ASTSQLITE_FILE_TMP=`mktemp`
ASTSQLITE_UPGRADE_TMP=`mktemp`
ASTSQLITE_DROP_TABLE_TMP=`mktemp`
ASTSQLITE_SCHEMA_TMP=`mktemp`
ASTSQLITE_USERCLIENT_TMP=`mktemp`

sed -r	's/^DROP TABLE ([^ ;]+);$/DROP TABLE tmp_\1;/
	 s/^CREATE TABLE ([^ ;]+) \($/CREATE TABLE tmp_\1 \(/
	 s/^CREATE (UNIQUE )?INDEX ([^ ]+) ON ([^ \(;]+)\(([^\)]+)\);$/CREATE \1INDEX tmp_\2 ON tmp_\3(\4);/
	 s/^INSERT INTO ([^ ;]+) VALUES/INSERT INTO tmp_\1 VALUES/' \
	"${ASTSQLITE_FILE}" > "${ASTSQLITE_FILE_TMP}"

sqlite "${ASTSQLITE_DB}" < "${ASTSQLITE_FILE_TMP}" |sed "${SED_SUPPRESS_ERROR}"

echo 'BEGIN TRANSACTION;' > "${ASTSQLITE_UPGRADE_TMP}"

for FILE in "${ASTSQLITE_SCRIPTS_DIR}"/*.sql;
do
	cat "${FILE}" >> "${ASTSQLITE_UPGRADE_TMP}"
done

for FILE in "${ASTSQLITE_SCRIPTS_DIR}"/build/*.sql;
do
	sqlite "${ASTSQLITE_DB}" < "${FILE}" >> "${ASTSQLITE_UPGRADE_TMP}"
done

cat "${ASTSQLITE_SCRIPTS_DIR}/fix/context.sql" >> "${ASTSQLITE_UPGRADE_TMP}"

echo 'COMMIT;' >> "${ASTSQLITE_UPGRADE_TMP}"

echo "  \____|\mm"

sqlite "${ASTSQLITE_DB}" < "${ASTSQLITE_UPGRADE_TMP}" |sed "${SED_SUPPRESS_ERROR}"

echo "  //_//\ \_\\"

ASTSQLITE_DIALACTION_FIX=(`sqlite "${ASTSQLITE_DB}" < "${ASTSQLITE_SCRIPTS_DIR}/fix/dialaction-actionargs.sql"`)

for ACTIONARGS in "${ASTSQLITE_DIALACTION_FIX[@]}";
do
	ACTIONARG1=`echo ${ACTIONARGS}|cut -d\| -f1`
	ACTIONARG2=`echo ${ACTIONARGS}|cut -d\| -f2-`

	echo	"UPDATE tmp_dialaction
		 SET actionarg1 = '${ACTIONARG1}', actionarg2 = '${ACTIONARG2}'
		 WHERE action IN('application:callbackdisa','application:disa')
		 AND actionarg1 = '${ACTIONARGS}';" >> "${ASTSQLITE_UPGRADE_TMP}"
done

echo 'BEGIN TRANSACTION;' > "${ASTSQLITE_SCHEMA_TMP}"

sqlite "${ASTSQLITE_DB}" ".tables tmp_"|while read SQL_TMP_TABLES;
do
	for SQL_TMP_TABLE_NAME in `echo -n ${SQL_TMP_TABLES}`;
	do
		echo	"DROP TABLE ${SQL_TMP_TABLE_NAME};" >> "${ASTSQLITE_DROP_TABLE_TMP}"

		for SQL_TABLE_EXCEPT_NAME in "${SQL_TABLES_EXCEPTS[@]}";
		do
			if [ "tmp_${SQL_TABLE_EXCEPT_NAME}" == "${SQL_TMP_TABLE_NAME}" ];
			then
				continue 2
			fi
		done

		SQL_REAL_TABLE_NAME="${SQL_TMP_TABLE_NAME:4}"

		echo	"DROP TABLE ${SQL_REAL_TABLE_NAME};" >> "${ASTSQLITE_SCHEMA_TMP}"

		sqlite	"${ASTSQLITE_DB}" ".schema ${SQL_TMP_TABLE_NAME}" |\
		sed -r	's/^CREATE TABLE tmp_([^ ;]+) \($/CREATE TABLE \1 \(/
			 s/^CREATE (UNIQUE )?INDEX tmp_([^ ]+) ON tmp_([^ \(;]+)\(([^\)]+)\);$/CREATE \1INDEX \2 ON \3(\4);/' >> \
			"${ASTSQLITE_SCHEMA_TMP}"

		echo -e	".mode insert ${SQL_REAL_TABLE_NAME}\nSELECT * FROM ${SQL_TMP_TABLE_NAME};\n" |\
			sqlite "${ASTSQLITE_DB}" >> \
			"${ASTSQLITE_SCHEMA_TMP}"
	done
done

for SQL_TABLE_RENAME_NAME in "${SQL_TABLES_RENAME[@]}";
do
	echo "DROP TABLE ${SQL_TABLE_RENAME_NAME};" >> "${ASTSQLITE_SCHEMA_TMP}"
done

echo 'COMMIT;' >> "${ASTSQLITE_SCHEMA_TMP}"

echo " /K-9/  \/_/"

sqlite "${ASTSQLITE_DB}" < "${ASTSQLITE_SCHEMA_TMP}" |sed "${SED_SUPPRESS_ERROR}"
sqlite "${ASTSQLITE_DB}" < "${ASTSQLITE_DROP_TABLE_TMP}" |sed "${SED_SUPPRESS_ERROR}"
sqlite "${ASTSQLITE_DB}" < "${ASTSQLITE_SCRIPTS_DIR}/fix/contextnumbers.sql"

echo "/___/_____\\"

ASTSQLITE_USER_CLIENT_FIX=(`sqlite "${ASTSQLITE_DB}" < "${ASTSQLITE_SCRIPTS_DIR}/fix/userfeatures-client.sql"`)

echo 'BEGIN TRANSACTION;' >> "${ASTSQLITE_USERCLIENT_TMP}"

for CLIENT_FIELDS in "${ASTSQLITE_USER_CLIENT_FIX[@]}";
do
	USERFEATURESID=`echo ${CLIENT_FIELDS}|cut -d\| -f1`
	LOGINCLIENT=`echo ${CLIENT_FIELDS}|sed -r 's/[^\|]+\|([^\|@]+).*/\1/'`
	PASSWDCLIENT=`echo ${CLIENT_FIELDS}|cut -d\| -f3`

	echo	"UPDATE userfeatures
		 SET loginclient = '${LOGINCLIENT}', passwdclient = '${PASSWDCLIENT}'
		 WHERE id = '${USERFEATURESID}';" >> "${ASTSQLITE_USERCLIENT_TMP}"
done

echo 'COMMIT;' >> "${ASTSQLITE_USERCLIENT_TMP}"

sqlite "${ASTSQLITE_DB}" < "${ASTSQLITE_USERCLIENT_TMP}"

rm -f	"${ASTSQLITE_FILE_TMP}" \
	"${ASTSQLITE_UPGRADE_TMP}" \
	"${ASTSQLITE_DROP_TABLE_TMP}" \
	"${ASTSQLITE_SCHEMA_TMP}" \
	"${ASTSQLITE_USERCLIENT_TMP}"

echo "-----------"

echo "done !"
