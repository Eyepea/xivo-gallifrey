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

$array['element']['callfilterid'] = array();
$array['element']['callfilterid']['default'] = 0;

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['element']['ringseconds'] = array();
$array['element']['ringseconds']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['ringseconds']['default'] = 0;
$array['element']['ringseconds']['setdef'] = true;

$array['element']['priority'] = array();
$array['element']['priority']['default'] = 0;

$array['element']['bstype'] = array();
$array['element']['bstype']['value'] = array('boss','secretary');
$array['element']['bstype']['default'] = '';
$array['element']['bstype']['null'] = true;

$array['element']['active'] = array();
$array['element']['active']['value'] = array(0,1);
$array['element']['active']['default'] = 0;
$array['element']['active']['set'] = true;

$array['filter'] = array();
$array['filter']['callfilterid'] = array('cast' => 'ulongint');
$array['filter']['type'] = array('key' => array('user'));
$array['filter']['typeval'] = array('maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/');
$array['filter']['ringseconds'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,60,5));
$array['filter']['priority'] = array('cast' => 'uint','between' => array(0,16));
$array['filter']['bstype'] = array('set' => false,'chk' => 2,'key' => array('boss','secretary'));
$array['filter']['active'] = array('bool' => true);

?>
