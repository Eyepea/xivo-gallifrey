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
$array['element']['presence_id'] = array();
$array['element']['presence_id']['default'] = 0;
$array['element']['display_name'] = array();
$array['element']['display_name']['default'] = '';
$array['element']['actions'] = array();
$array['element']['actions']['default'] = '';
$array['element']['color'] = array();
$array['element']['color']['default'] = '#FFFFFF';
$array['element']['access_status'] = array();
$array['element']['access_status']['default'] = '';
$array['element']['deletable'] = array();
$array['element']['deletable']['default'] = 1;


$array['filter'] = array();
$array['filter']['name'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['presence_id'] = array('cast' => 'ulongint');
$array['filter']['display_name'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,\s]+$/i');
$array['filter']['actions'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/.*/i');
$array['filter']['actions'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/i');
#$array['filter']['actions'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\'\(\)\-\,]+$/i');
$array['filter']['color'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^\#[a-fA-F0-9]+$/i');
$array['filter']['access_status'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[0-9\-\,]+$/i');
$array['filter']['deletable'] = array('cast' => 'ulongint');
?>
