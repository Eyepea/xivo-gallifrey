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

$array['element'] = array();

$array['element']['outcallid'] = array();
$array['element']['outcallid']['default'] = 0;

$array['element']['trunkfeaturesid'] = array();
$array['element']['trunkfeaturesid']['default'] = 0;

$array['element']['priority'] = array();
$array['element']['priority']['default'] = 0;

$array['filter'] = array();
$array['filter']['outcallid'] = array('cast' => 'ulongint');
$array['filter']['trunkfeaturesid'] = array('cast' => 'ulongint');
$array['filter']['priority'] = array('cast' => 'uint','between' => array(0,16));

?>
