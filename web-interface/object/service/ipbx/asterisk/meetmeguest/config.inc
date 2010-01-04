<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

$array['element']['meetmefeaturesid'] = array();
$array['element']['meetmefeaturesid']['default'] = '';

$array['element']['fullname'] = array();
$array['element']['fullname']['default'] = '';

$array['element']['telephonenumber'] = array();
$array['element']['telephonenumber']['default'] = '';
$array['element']['telephonenumber']['null'] = true;

$array['element']['email'] = array();
$array['element']['email']['default'] = '';
$array['element']['email']['null'] = true;

$array['element']['sendemail'] = array();
$array['element']['sendemail']['value'] = array('yes','no');
$array['element']['sendemail']['default'] = 'no';
$array['element']['sendemail']['unset'] = true;


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['meetmefeaturesid'] = array('cast' => 'ulongint');
$array['filter']['fullname'] = array('cast' => 'sgraph','minlen' => 1,'maxlen' => 255);
$array['filter']['telephonenumber'] = array('set' => false,'chk' => 2,'regexp' => '/^\+?[0-9\*#]+$/','maxlen' => 40);
$array['filter']['email'] = array('set' => false,'chk' => 2,'maxlen' => 255,'callback' => 'dwho_network::valid_email');

?>
