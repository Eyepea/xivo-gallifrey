<?php

#
# XiVO Web-Interface
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

$array = array();

$array['ami'] = array();
$array['ami']['bindaddr'] = '127.0.0.1';
$array['ami']['port'] = 5004;

$array['autoprov'] = array();
$array['autoprov']['host'] = '127.0.0.1';
$array['autoprov']['port'] = 8666;

$array['backupfiles'] = array();
$array['backupfiles']['path'] = '/var/backups/pf-xivo';

$array['configfiles'] = array();
$array['configfiles']['path'] = '/etc/asterisk';

$array['musiconhold'] = array();
$array['musiconhold']['path'] = '/var/lib/pf-xivo/moh';

$array['logfiles'] = array();
$array['logfiles']['path'] = '/var/log/asterisk';

$array['sounds'] = array();
$array['sounds']['path'] = '/var/lib/pf-xivo/sounds';

?>
