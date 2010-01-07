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
$array['element']['persistentmembers'] = array();
$array['element']['persistentmembers']['value'] = array('no','yes');
$array['element']['persistentmembers']['default'] = 'yes';
$array['element']['persistentmembers']['set'] = true;

$array['element']['autofill'] = array();
$array['element']['autofill']['value'] = array('no','yes');
$array['element']['autofill']['default'] = 'no';
$array['element']['autofill']['set'] = true;

$array['element']['monitor-type'] = array();
$array['element']['monitor-type']['value'] = array('','mixmonitor');
$array['element']['monitor-type']['default'] = '';
$array['element']['monitor-type']['set'] = true;

$array['filter'] = array();
$array['filter']['persistentmembers'] = array('bool' => true);
$array['filter']['autofill'] = array('bool' => true);
$array['filter']['monitor-type'] = array('bool' => true);

?>
