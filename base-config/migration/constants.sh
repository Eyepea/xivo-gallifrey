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

: ${ASTSQLITE_SCRIPTS_DIR:="`dirname $0`/scripts/astsqlite"}
: ${ASTSQLITE_DB_FILENAME:="astsqlite"}
: ${ASTSQLITE_DB:="/var/lib/asterisk/${ASTSQLITE_DB_FILENAME}"}
: ${ASTSQLITE_FILE:="/usr/share/pf-xivo-base-config/astsqlite.db.sql"}
: ${SED_SUPPRESS_ERROR:="/^DROP TABLE /{N; /\nSQL error: no such table:/d}"}
