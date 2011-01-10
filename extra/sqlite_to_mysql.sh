#!/bin/bash
#__version__ = "$Revision$ $Date$"
#__license__ = """
#    Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

# Script for converting XiVO sqlite to XiVO mysql
# XXX Only for XiVO 1.1.x
# TODO Import ctilog table
# TODO resume on error management 
# TODO escapte quote (') characters in sqlite dump

#####################################
# FUNCTIONS                         #
#####################################

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

search_replace() {
    key=$1
    val=$2
    file=$3
    arg=$4

    if [ -z $arg ] ; then
        echo "Replace $key with value $val in file $file"
        sed -i "s/^$key.*$/$val/" $file
    else
        echo "Replace $key with value $val in file $file"
        sed -i "s/$key/$val/" $file
    fi
}

import_mysql() {
    sqlfile=$1
    db=$2

    if [ -z $db ]; then
        mysql -uroot --show-warnings=true --debug-info=true --default-character-set=utf8 -p < $sqlfile
    else
        mysql --defaults-file=/etc/mysql/debian.cnf --show-warnings=true --debug-info=true --default-character-set=utf8 $db < $sqlfile
    fi

}


#########################################
# Variables                             #
#########################################
if [ "$#" -ne "2" ]; then
    echo "usage: $0 /path/to/sqlite/asterisk/db /path/to/sqlite/xivo/db"
    echo -e "where :"
    echo -e "\t/path/to/sqlite/asterisk/db\tis the path towards the Asterisk SQLite db"
    echo -e "\t/path/to/sqlite/xivo/db\t\tis the path towards the XiVO SQLite db"
    exit 1
fi

ASTDB_PATH=$1
XIVODB_PATH=$2


#########################################
# MySQL INSTALLATION                    #
#########################################

# Comment: MySQL installation and password generation :
println info "If not already installed, mysql-server packages are going to be installed."
println info "During MySQL installation, you'll be prompted to chose the root password."
println info "We recommend to use a strong password. Generate it with the command 'pwgen -C 8' for example."
println info "WARNING: keep this password carefully. You'll need it afterwards"
println info "Would you like to continue ? [Y/n]"
read INSTALL_CONTINUE
if [ "$INSTALL_CONTINUE" == "n" ]; then
    println error "Installation aborted :"
    println error "\tMySQL :"
    println error "\t\t .install :\taborted"
    println error "\t\t .init db :\t-"
    println error "\tSQLite2MySQL :"
    println error "\t\t .dump :\t-"
    println error "\t\t .import :\t-"
    println error "\tConfiguration :"
    println error "\t\t .update :\t-"
    println error "\t\t .restart :\t-"
    exit 1
fi

# Installation of MySQL packages
MYSQL_PKG="mysql-server asterisk-mysql php5-mysql"
MYSQL_PKG_INSTALLED=$(dpkg -l ${MYSQL_PKG} 2>/dev/null | grep -E "^ii" |wc -l)
if [ "$MYSQL_PKG_INSTALLED" != "3" ]; then
    aptitude update >/dev/null
    aptitude install mysql-server asterisk-mysql php5-mysql
else
    println info "MySQL Packages already installed"
fi

# Database Initialization
println info "Asterisk and XiVO MySQL Databases are going to be initialized"
println info "Would you like to continue ? [Y/n]"
read INSTALL_CONTINUE
if [ "$INSTALL_CONTINUE" == "n" ]; then
    println error "Installation aborted :"
    println error "\tMySQL :"
    println info "\t\t .install :\tok"
    println error "\t\t .init db :\taborted"
    println error "\tSQLite2MySQL :"
    println error "\t\t .dump :\t-"
    println error "\t\t .import :\t-"
    println error "\tConfiguration :"
    println error "\t\t .update :\t-"
    println error "\t\t .restart :\t-"
    exit 1
fi

println info "### MySQL Database Initialization -- You'll be prompted for MySQL root password"
println info "..."
println info "#### >>>> Asterisk Database Init"
if import_mysql /usr/share/pf-xivo-base-config/astmysql.db.sql; then
    println info "....."
    println info "OK - Asterisk Database Init : OK <<<< ####"
else
    println error "ERROR - Asterisk Database Init : ERROR <<<< ####"
    println error "ERROR - Asterisk Database wasn't initialized correctly <<<< ####"
    exit 1
fi
println info "#### >>>> XiVO Database Init"
if import_mysql /usr/share/pf-xivo-base-config/xivo.db.mysql.sql; then
    println info "....."
        println info "OK - XiVO Database Init : OK <<<< ####"
else
        println error "ERROR - XiVO Database Init : ERROR <<<< ####"
        println error "ERROR - XiVO Database wasn't initialized correctly <<<< ####"
        exit 1
fi
println info "#### >>>> XiVO CTI Table Init"
if import_mysql /usr/share/pf-xivo-cti-server/init/datastorage/mysql/xivo.db.sql; then
    println info "....."
        println info "OK - XiVO CTI Table Init : OK <<<< ####"
else
        println error "ERROR - XiVO CTI Table Init : ERROR <<<< ####"
        println error "ERROR - XiVO XiVO CTI Table wasn't initialized correctly <<<< ####"
        exit 1
fi
println info "MySQL Installation and Initialization finished"
sleep 2
println info "----------------------------------------------"

######################################
# DUMP sqlite DATA                   #
######################################

DB_NAME=(asterisk xivo)
DB_LOCATION=($ASTDB_PATH $XIVODB_PATH)
DB_COPY_NAME=(astsqlite.bak xivo.bak)
WORKING_DIR=/root/migration_sqlite-to-mysql

println info "\n\n----------------------------------------------"
println info "SQLite databases are now going to be dumped"
println info "Would you like to continue ? [Y/n]"
read INSTALL_CONTINUE
if [ "$INSTALL_CONTINUE" == "n" ]; then
    println error "Installation aborted :"
    println info "\tMySQL :"
    println info "\t\t .install :\tok"
    println info "\t\t .init db :\tok"
    println error "\tSQLite2MySQL :"
    println error "\t\t .dump :\taborted"
    println error "\t\t .import :\t-"
    println error "\tConfiguration :"
    println error "\t\t .update :\t-"
    println error "\t\t .restart :\t-"
    exit 1
fi

#Create Working dir :
mkdir -p $WORKING_DIR
cd $WORKING_DIR

for (( i=0 ; $i < ${#DB_NAME[*]} ; i++ )) ; do
    # Work on a copy
    cp -a ${DB_LOCATION[$i]} ${DB_COPY_NAME[$i]}
    # Create a working directory
    mkdir -p {insert-${DB_NAME[$i]},dump-${DB_NAME[$i]}}
    
    # List tables
    TABLES=`sqlite ${DB_COPY_NAME[$i]} ".tables"`

    for TABLE in $TABLES; do
        if [ "$TABLE" == "cdr" ]; then
            println info "Are you sure you want to dump $TABLE table ? [Y/n]"
            DUMP_CDR=0
            read DUMP_CONTINUE
            if [ "$DUMP_CONTINUE" == "n" ]; then
                DUMP_CDR=1
                println warn "dump of $TABLE table was skipped"
            else
                # Create insert input file
                echo -e ".mode insert $TABLE\nSELECT * FROM $TABLE;" > insert-${DB_NAME[$i]}/insert-cmd.$TABLE
                # Dump SQLite datas in insert mode
                sqlite ${DB_COPY_NAME[$i]} < insert-${DB_NAME[$i]}/insert-cmd.$TABLE > dump-${DB_NAME[$i]}/sqlite-dump_$TABLE.sql
            fi
        else
            # Create insert input file
            echo -e ".mode insert $TABLE\nSELECT * FROM $TABLE;" > insert-${DB_NAME[$i]}/insert-cmd.$TABLE
            # Dump SQLite datas in insert mode
            sqlite ${DB_COPY_NAME[$i]} < insert-${DB_NAME[$i]}/insert-cmd.$TABLE > dump-${DB_NAME[$i]}/sqlite-dump_$TABLE.sql
        fi
    done;
done;

######################################
# IMPORT sqlite DATAS TO mysql       #
######################################

println info "SQLite dumped files are now going to be imported in MySQL"
println error "WARNING : all datas in Asterisk and XiVO MySQL databases will be lost"
println info "Would you like to continue ? [Y/n]"
read INSTALL_CONTINUE
if [ "$INSTALL_CONTINUE" == "n" ]; then
    println error "Installation aborted :"
    println info "\tMySQL :"
    println info "\t\t .install :\tok"
    println info "\t\t .init db :\tok"
    println error "\tSQLite2MySQL :"
    println info "\t\t .dump :\tok"
    println error "\t\t .import :\taborted"
    println error "\tConfiguration :"
    println error "\t\t .update :\t-"
    println error "\t\t .restart :\t-"
    exit 1
fi

println info "\n------------"
println info "SQLite does not store every values with the same type than MySQL does."
println info "Particularly some fields stored as tinyint values in MySQL are imported as string in SQLite dump files."
println info "If you meet such issues, edit the dump file of the table and change it."
println info "Before you start, you should verify these tables and these fields :"
println info "\t.table USERFEATURES: passwdclient -> if you have numerical passwords, check that they don't start by 0"
println info "\t.table QUEUE: reportholdtime, servicelevel, timeoutrestart"
println info "\t.table VOICEMAIL: sendvoicemail"
println info "\t.table CDR: answer, end (verify that timestamp value is correct)"
println info "Type Y when you are ready..."
read INSTALL_CONTINUE
if [ "$INSTALL_CONTINUE" != "Y" ]; then
    println error "Installation aborted :"
    println info "\tMySQL :"
    println info "\t\t .install :\tok"
    println info "\t\t .init db :\tok"
    println error "\tSQLite2MySQL :"
    println info "\t\t .dump :\tok"
    println error "\t\t .import :\taborted"
    println error "\tConfiguration :"
    println error "\t\t .update :\t-"
    println error "\t\t .restart :\t-"
    exit 1
fi


for (( i=0 ; $i < ${#DB_NAME[*]} ; i++ )) ; do
    # List tables
    TABLES=`sqlite ${DB_COPY_NAME[$i]} ".tables"`

    # Erases all datas in MySQL : export structure without data and then import it
    # it is needed otherwise importing sqlite data will give a lot of errors
    mysqldump --defaults-extra-file=/etc/mysql/debian.cnf --add-drop-table=true --no-data=true ${DB_NAME[$i]} > ${DB_NAME[$i]}mysql-struct.sql
    mysql --defaults-extra-file=/etc/mysql/debian.cnf --show-warnings=true ${DB_NAME[$i]} < ${DB_NAME[$i]}mysql-struct.sql
    
    # init to null
    IMPORT_CONTINUE=""
    for TABLE in $TABLES; do
        # Import dumped data in MySQL
        if [ "$TABLE" == "cdr" ] && [ "$DUMP_CDR" == "1" ]; then
            #do not ask the question for cdr table if dump was skipped
            continue
        elif [ "$TABLE" == "i18ncache" ] || [ "$TABLE" == "session" ]; then
            #do not import session and i18ncache table since it is not needed
            continue
        else
            if [ -z $IMPORT_CONTINUE ] || [ "$IMPORT_CONTINUE" != "a" ]; then
                println info "### ${DB_NAME[$i]} import : would you like to import TABLE $TABLE ? Y/n/a (yes for this table/no for this table/yes for all tables)"
                read IMPORT_CONTINUE
            else
                IMPORT_CONTINUE=$IMPORT_CONTINUE
            fi
            if [ "$IMPORT_CONTINUE" == "Y" ] || [ "$IMPORT_CONTINUE" == "a" ]; then
                println info "#### >>>> $TABLE Database Import"
                if import_mysql dump-${DB_NAME[$i]}/sqlite-dump_$TABLE.sql ${DB_NAME[$i]}; then
                    println info ".....\n....."
                    println info "OK - $TABLE Database Import : OK <<<< ####"
                else
                    println error "ERROR - $TABLE Database Import : ERROR <<<< ####"
                    println error "ERROR - $TABLE Database was imported with ERRORS <<<< ####"
                    exit 1
                fi
                sleep 1
            fi
        fi
    done;
done;



#######################################
# Change configuration to use MySQL   #
#######################################

println info "----------------------------"
println info "The system configuration is now going to be changed in order to use MySQL"
println warn "** YOU SHOULD SKIP THIS STEP IF YOU ALREADY HAVE CONFIGURED YOUR XiVO in MySQL via the WIZARD **"
println info "Would you like to continue ? S[kip]/y[es]/n[o]"
read INSTALL_CONTINUE
if [ "$INSTALL_CONTINUE" == "n" ]; then
    println error "Installation aborted :"
    println info "\tMySQL :"
    println info "\t\t .install :\tok"
    println info "\t\t .init db :\tok"
    println info "\tSQLite2MySQL :"
    println info "\t\t .dump :\tok"
    println info "\t\t .import :\tok"
    println error "\tConfiguration :"
    println error "\t\t .update :\taborted"
    println error "\t\t .restart :\t-"
    exit 1
elif [ "$INSTALL_CONTINUE" == "y" ]; then
    # sqlite replacement by mysql in .ini files.
    println "Replace SQLite configuration by MySQL configuration in web-interface ini files"
    search_replace "datastorage = " "datastorage = \"mysql:\/\/asterisk:proformatique@localhost\/asterisk?charset=utf8\"" /etc/pf-xivo/web-interface/ipbx.ini
    search_replace "datastorage = " "datastorage = \"mysql:\/\/xivo:proformatique@localhost\/xivo?charset=utf8\"" /etc/pf-xivo/web-interface/xivo.ini
    search_replace "datastorage = " "datastorage = \"mysql:\/\/xivo:proformatique@localhost\/xivo?charset=utf8\"" /etc/pf-xivo/web-interface/cti.ini
    
    search_replace "XIVO_XIVODB=" "XIVO_XIVODB=\"mysql:\/\/xivo:proformatique@localhost:3306\/xivo?charset=utf8\"" /etc/pf-xivo/common.conf
    search_replace "XIVO_ASTDB=" "XIVO_ASTDB=\"mysql:\/\/asterisk:proformatique@localhost:3306\/asterisk?charset=utf8\"" /etc/pf-xivo/common.conf
    
    # Update base configuration
    update-pf-xivo-base-config
    
    # Asterisk modules configuration
    println "Replace SQLite configuration by MySQL configuration in Asterisk configuration files"
    if [ -e "/etc/asterisk/modules.conf" ]; then
        println "/etc/asterisk/modules.conf exists : backuping it"
        cp -a /etc/asterisk/modules.conf /etc/asterisk/modules.conf.without-MySQL
    fi
    search_replace "preload = res_config_sqlite.so" "noload = res_config_sqlite.so" /etc/asterisk/modules.conf
    search_replace "noload = cdr_addon_mysql.so" "preload = cdr_addon_mysql.so" /etc/asterisk/modules.conf
    search_replace "noload = res_config_mysql.so" "preload = res_config_mysql.so" /etc/asterisk/modules.conf
    
    # File /etc/asterisk/extconfig.conf
    search_replace sqlite mysql /etc/asterisk/extconfig.conf 1
    
    # File /etc/asterisk/cdr_mysql.conf
    if [ -e "/etc/asterisk/cdr_mysql.conf" ]; then
        println "/etc/asterisk/cdr_mysql.conf exists : backuping and replacing it"
        cp -a /etc/asterisk/cdr_mysql.conf /etc/asterisk/cdr_mysql.conf.without-MySQL
    fi
    cat << EOF > /etc/asterisk/cdr_mysql.conf
; XIVO: FILE AUTOMATICALLY GENERATED BY THE XIVO CONFIGURATION SUBSYSTEM
[global]
hostname = localhost
port = 3306
sock = /var/run/mysqld/mysqld.sock
dbname = asterisk
user = asterisk
password = proformatique
table = cdr
charset = utf8
userfield = 1
EOF
    
    # File /etc/asterisk/res_mysql.conf
    if [ -e "/etc/asterisk/res_mysql.conf" ]; then
        println "/etc/asterisk/res_mysql.conf exists : backuping and replacing it"
        mv /etc/asterisk/res_mysql.conf /etc/asterisk/res_mysql.conf.without-MySQL
    fi
    touch /etc/asterisk/res_mysql.conf
    chmod 660 /etc/asterisk/res_mysql.conf
    chown asterisk:www-data /etc/asterisk/res_mysql.conf
    
    cat << EOF > /etc/asterisk/res_mysql.conf
; XIVO: FILE AUTOMATICALLY GENERATED BY THE XIVO CONFIGURATION SUBSYSTEM
[general]
dbname = asterisk
dbuser = asterisk
dbpass = proformatique
dbhost = localhost
dbsock = /var/run/mysqld/mysqld.sock
dbcharset = utf8
comment_support = yes
dbport = 3306
EOF

else
    println info "Switching configuration from SQLite to MySQL was skipped"
fi

############################
# Restart services         #
############################

println info "----------------------------"
println info "The services are now going to be restarted"
println info "Would you like to continue ? [Y/n]"
read INSTALL_CONTINUE
if [ "$INSTALL_CONTINUE" == "n" ]; then
    println error "Installation aborted :"
    println info "\tMySQL :"
    println info "\t\t .install :\tok"
    println info "\t\t .init db :\tok"
    println info "\tSQLite2MySQL :"
    println info "\t\t .dump :\tok"
    println info "\t\t .import :\tok"
    println error "\tConfiguration :"
    println info "\t\t .update :\tok"
    println error "\t\t .restart :\taborted"
    exit 1
fi


invoke-rc.d mysql restart
invoke-rc.d apache2 restart
invoke-rc.d pf-xivo-cti-server restart
invoke-rc.d pf-xivo-provisioning restart
invoke-rc.d pf-xivo-agid restart


println info "---------SUCCESS--------------"
println info "Your XiVO has succesfully been configured in MySQL"
println info "Installation aborted :"
println info "\tMySQL :"
println info "\t\t .install :\tok"
println info "\t\t .init db :\tok"
println info "\tSQLite2MySQL :"
println info "\t\t .dump :\tok"
println info "\t\t .import :\tok"
println info "\tConfiguration :"
println info "\t\t .update :\tok"
println info "\t\t .restart :\tok"

