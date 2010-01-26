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
$array['element']['appliname'] = array();
$array['element']['appliname']['default'] = '';
$array['element']['xlets'] = array();
$array['element']['xlets']['default'] = '';
$array['element']['funcs'] = array();
$array['element']['funcs']['default'] = '';
$array['element']['maxgui'] = array();
$array['element']['maxgui']['default'] = '';
$array['element']['presence'] = array();
$array['element']['presence']['default'] = '';
$array['element']['services'] = array();
$array['element']['services']['default'] = '';
$array['element']['preferences'] = array();
$array['element']['preferences']['default'] = '';
$array['element']['deletable'] = array();
$array['element']['deletable']['default'] = 1;



$array['filter'] = array();
$array['filter']['name'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['appliname'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\s\-\,]+$/i');
$array['filter']['xlets'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['maxgui'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/i');
$array['filter']['presence'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\.\,]+$/i');
$array['filter']['services'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['funcs'] 		= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9_\-\,]+$/i');
$array['filter']['preferences'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/i');
$array['filter']['deletable'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/i');
?>
