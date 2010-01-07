<?php

#
# XiVO Web-Interface
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

define('XIVO_USER__VISITOR',0);
define('XIVO_USER__USER',1);
define('XIVO_USER__ADMIN',2);
define('XIVO_USER__ROOT',4);

define('XIVO_USER_CLASS_PRE','_usr_');

$array = array();

$array['type'] = array(
	XIVO_USER__VISITOR	=> array('meta' => 'visitor',
					'const'	=> 'XIVO_USER__VISITOR'),
	XIVO_USER__USER		=> array('meta' => 'user',
					'const' => 'XIVO_USER__USER'),
	XIVO_USER__ADMIN		=> array('meta' => 'admin',
					'const' => 'XIVO_USER__ADMIN'),
	XIVO_USER__ROOT		=> array('meta' => 'root',
					'const' => 'XIVO_USER__ROOT'));

// $array['admin'] = 'admin';

$array['option'] = array(
	'time_cache'		=> 60,
	'ip_check'		=> false);

?>
