#!/bin/sh
#__version__ = "$Revision$ $Date$"
#__license__ =
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
#

# This script removes voicemail messages under VOICEMAIL_PATH older than REMOVE_OLDER_THAN

# Note : VOICEMAIL_PATH = /var/spool/asterisk/voicemail/<context>/<voicemail number>/<message folders>/<messages>
VOICEMAIL_PATH='/var/spool/asterisk/voicemail/*/*/*/*'
REMOVE_OLDER_THAN="-1 month"

OLDER_THAN=$(date -d "${REMOVE_OLDER_THAN}" +%s )
echo removing all thingh older than time_t: $OLDER_THAN  \( $(date -d "${REMOVE_OLDER_THAN}") \)

for FILE in $VOICEMAIL_PATH ; do
  if [ $FILE != "$VOICEMAIL_PATH" ] && [ -f $FILE ]; then
    FILE_CREATION=$(/usr/bin/stat -c "%Y" "$FILE")
    if [ $FILE_CREATION -lt $OLDER_THAN ] ; then
      echo "removing.. $FILE"
      rm -f "$FILE"
    fi
  fi
done
