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

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['uri'] = array();
$array['element']['uri']['default'] = '';

$array['element']['tablename'] = array();
$array['element']['tablename']['default'] = '';

$array['element']['dirtype'] = array();
$array['element']['dirtype']['default'] = 'sgbd';

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


dwho::load_class('dwho_network');

$array['filter'] = array();
$array['filter']['name'] = array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\.-]+$/','maxlen' => 64);
$array['filter']['uri'] = array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/','maxlen' => 255);
$array['filter']['tablename'] = array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\.-]+$/','maxlen' => 64);
$array['filter']['dirtype'] = array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/','maxlen' => 255);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
