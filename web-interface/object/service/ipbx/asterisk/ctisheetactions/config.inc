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

$array['element'] = array();
$array['element']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['whom'] = array();
$array['element']['whom']['default'] = '';

$array['element']['capaids'] = array();
$array['element']['capaids']['default'] = '';

$array['element']['sheet_info'] = array();
$array['element']['sheet_info']['default'] = '';

$array['element']['systray_info'] = array();
$array['element']['systray_info']['default'] = '';

$array['element']['sheet_qtui'] = array();
$array['element']['sheet_qtui']['default'] = '';

$array['element']['action_info'] = array();
$array['element']['action_info']['default'] = '';

$array['element']['focus'] = array();
$array['element']['focus']['default'] = '';

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['element']['deletable'] = array();
$array['element']['deletable']['default'] = 1;

$array['filter'] = array();
$array['filter']['name'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[[:alnum:]]+$/i');
$array['filter']['context'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['whom'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['capaids'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['sheet_info'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['systray_info'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['sheet_qtui'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['action_info'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['focus'] 	= array('set' => false, 'chk' => 2, 'cast' => 'int');
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);
$array['filter']['deletable'] 	= array('set' => false, 'chk' => 2, 'cast' => 'int');
?>
