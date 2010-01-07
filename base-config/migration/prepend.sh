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

# Make sure PATH contains all the usual suspects
PATH="$PATH:/sbin:/bin:/usr/sbin:/usr/bin"

# Include /usr/ucb for finding whoami on Solaris
PATH="$PATH:/usr/ucb"

export PATH

: ${SQLITE:=sqlite}

if [ -z "${XIVO_CODENAME}" ];
then
	echo "Missing XIVO_CODENAME variable"
	exit 1
fi

if ! ask_yn_question "Would you like to upgrade Asterisk SQLite Database?";
then
	exit 0
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
	echo "Asterisk ${XIVO_CODENAME} SQLite schema not readable"
	exit 1
fi
