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

$array['element']['start'] = array();
$array['element']['start']['default'] = '';

$array['element']['end'] = array();
$array['element']['end']['default'] = '';
$array['element']['end']['null'] = true;

$array['element']['created_at'] = array();
$array['element']['created_at']['default'] = '';


$array['filter'] = array();
$array['filter']['name'] = array('minlen' => 1);
$array['filter']['start'] = array('set' => false,'chk' => 1,'callback' => 'dwho_filter::chk_datetime','callbackarg' => array(true));
$array['filter']['end'] = array('set' => false,'chk' => 2,'callback' => 'dwho_filter::chk_datetime','callbackarg' => array(true));
$array['filter']['created_at'] = array('set' => false, 'chk' => 2);

?>
