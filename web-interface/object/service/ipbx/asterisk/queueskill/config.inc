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

$array = array();

$array['element'] = array();

$array['element']['catid'] = array();
$array['element']['catid']['default'] = 0;

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['element']['printscreen'] = array();
$array['element']['printscreen']['default'] = '';


$array['filter'] = array();
$array['filter']['name'] 	= array('minlen' => 1, 'maxlen' => 64, 'regexp' => '/^[a-z0-9_-]+$/');
$array['filter']['printscreen'] = array('chk' => 2   , 'maxlen' => 5 , 'regexp' => '/^[a-zA-Z0-9_-]+$/');

?>
