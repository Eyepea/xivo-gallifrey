#!/bin/bash
#__version__ = "$Revision$ $Date$"
#__license__ = """
#    Copyright (C) 2010  Proformatique <technique@proformatique.com>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#"""

#TODO: make some sanity check on the UPPER LIMIT (is it a correct date ?) otherwise it could erase every cdr ...

println() {
    level=$1
    text=$2

    if [ "$level" == "error" ]; then
        echo -en "\033[0;36;31m$text\033[0;38;39m\n\r"
    elif [ "$level" == "warn" ]; then
        echo -en "\033[0;36;33m$text\033[0;38;39m\n\r"
    else
        echo -en "\033[0;36;40m$text\033[0;38;39m\n\r"
    fi
}

ask_yn_question()
{
    QUESTION=$1
 
    while true;
    do
        println warn "${QUESTION} (y/n) "
        read REPLY
        if [ "${REPLY}" == "y" ];
        then
                return 0;
        fi
        if [ "${REPLY}" == "n" ];
        then
                return 1;
        fi
        echo "Don't tell ya life, reply using 'y' or 'n'"'!'
    done
}


if [ "$#" -eq "0" ]; then
    echo "usage: $0 upperlimit"
    echo -e "\t where upperlimit is the date till which you want to keep the CDR, formated as YYYY-MM-DD"
    exit 1
fi

TEMP_CDR_TABLE=cdr_temp
DB_BACKUP_PATH=/var/backups/pf-xivo
UPPER_LIMIT=$1 
OLD_LIMIT=$(echo $(mysql --defaults-file=/etc/mysql/debian.cnf asterisk -e "SELECT calldate FROM cdr ORDER BY calldate ASC LIMIT 1")|cut -d" " -f2)
DB_BACKUP_FILE=$DB_BACKUP_PATH/cdr_$OLD_LIMIT"_to_"$UPPER_LIMIT.sql


println notice "Create temporary cdr table $TEMP_CDR_TABLE : it will contain all cdr from $OLD_LIMIT to $UPPER_LIMIT"
println notice "\tCREATE TABLE $TEMP_CDR_TABLE AS SELECT * FROM cdr WHERE calldate < '$UPPER_LIMIT' ..."
mysql --defaults-file=/etc/mysql/debian.cnf asterisk -e "CREATE TABLE $TEMP_CDR_TABLE AS SELECT * FROM cdr WHERE calldate < '$UPPER_LIMIT';"

println notice "Dump temporary cdr table $TEMP_CDR_TABLE :"
println notice "\tin $DB_BACKUP_FILE"
mysqldump --defaults-file=/etc/mysql/debian.cnf --no-create-db asterisk cdr_temp > $DB_BACKUP_FILE

println notice "Delete temporary table $TEMP_CDR_TABLE :"
println notice "\tDROP TABLE $TEMP_CDR_TABLE"
mysql --defaults-file=/etc/mysql/debian.cnf asterisk -e "DROP TABLE $TEMP_CDR_TABLE;"

println notice "Deleting cdr entries before $UPPER_LIMIT"
if ! ask_yn_question "Are you sure you want to continue ?"
then
        exit 0
fi
mysql --defaults-file=/etc/mysql/debian.cnf asterisk -e "DELETE FROM cdr WHERE calldate < '$UPPER_LIMIT';"

