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

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['trunkfeaturesid'] = array();
$array['element']['trunkfeaturesid']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('emergency','special');
$array['element']['type']['default'] = 'emergency';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['filter'] = array();
$array['filter']['exten'] = array('maxlen' => 40,'regexp' => '/^_?\+?[0-9NXZ\*#\-\[\]]+[\.\!]?$/');
$array['filter']['trunkfeaturesid'] = array('cast' => 'ulongint');
$array['filter']['type'] = array('key' => array('emergency','special'));
$array['filter']['commented'] = array('bool' => true);

?>
