<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

$array['element']['username'] = array();
$array['element']['username']['default'] = '';

$array['element']['password'] = array();
$array['element']['password']['default'] = '';

$array['element']['authuser'] = array();
$array['element']['authuser']['default'] = '';

$array['element']['host'] = array();
$array['element']['host']['default'] = '';

$array['element']['port'] = array();
$array['element']['port']['default'] = '';

$array['element']['contact'] = array();
$array['element']['contact']['default'] = '';

$array['filter'] = array();
$array['filter']['username'] = array('set' => false,'maxlen' => 80);
$array['filter']['password'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['authuser'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['host'] = array('set' => false,'callback' => 'dwho_filter::chk_host_ipv4_strict','maxlen' => 64);
$array['filter']['port'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['contact'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9]+$/i','maxlen' => 250);

?>
