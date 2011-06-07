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

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['label'] = array();
$array['element']['label']['default'] = '';

$array['element']['action'] = array();
$array['element']['action']['value'] = array('removenow','keep','purge');
$array['element']['action']['default'] = '';


$array['filter'] = array();
$array['filter']['name']   = array('set' => true,'chk' => 1, 'minlen' => '1');
$array['filter']['label']  = array('set' => false,'chk' => 2);
$array['filter']['action'] = array('set' => false,'chk' => 2, 'key' => array('removenow','keep','purge'));

?>
