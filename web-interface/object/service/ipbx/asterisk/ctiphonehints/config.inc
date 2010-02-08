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
$array['element']['color'] = array();
$array['element']['color']['default'] = 'green';
$array['element']['number'] = array();
$array['element']['number']['default'] = 0;



$array['filter'] = array();
$array['filter']['name'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9]+$/i');
$array['filter']['color'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^\#?[a-z0-9]+$/i');
$array['filter']['number'] 	= array('set' => false, 'chk' => 2, 'cast' => 'int');
?>
