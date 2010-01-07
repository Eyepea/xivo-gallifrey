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

$array['element']['rightcallid'] = array();
$array['element']['rightcallid']['default'] = 0;

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user','group','incall','outcall');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['filter'] = array();
$array['filter']['rightcallid'] = array('cast' => 'ulongint');
$array['filter']['type'] = array('key' => array('user','group','incall','outcall'));
$array['filter']['typeval'] = array('maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/');

?>
