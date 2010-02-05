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
$array['element']['uri'] = array();
$array['element']['uri']['default'] = '';
$array['element']['delimiter'] = array();
$array['element']['delimiter']['default'] = '';
$array['element']['match_direct'] = array();
$array['element']['match_direct']['default'] = '';
$array['element']['match_reverse'] = array();
$array['element']['match_reverse']['default'] = '';
$array['element']['field_phone'] = array();
$array['element']['field_phone']['default'] = '';
$array['element']['field_firstname'] = array();
$array['element']['field_firstname']['default'] = '';
$array['element']['field_lastname'] = array();
$array['element']['field_lastname']['default'] = '';
$array['element']['field_fullname'] = array();
$array['element']['field_fullname']['default'] = '';
$array['element']['field_company'] = array();
$array['element']['field_company']['default'] = '';
$array['element']['field_mail'] = array();
$array['element']['field_mail']['default'] = '';
$array['element']['display_reverse'] = array();
$array['element']['display_reverse']['default'] = '';
$array['element']['description'] = array();
$array['element']['description']['default'] = '';
$array['element']['deletable'] = array();
$array['element']['deletable']['default'] = 1;



$array['filter'] = array();
$array['filter']['name'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^[a-z0-9]+$/i');
$array['filter']['delimiter'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.$/');
$array['filter']['match_direct'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['match_reverse'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['field_phone'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['field_fullname'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['field_firstname'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['field_lastname'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['field_company'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['uri'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['field_mail'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['display_reverse'] 	= array('set' => false, 'chk' => 2, 'regexp' => '/^.*$/');
$array['filter']['description'] 	= array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);
$array['filter']['deletable'] 	= array('set' => false, 'chk' => 2, 'cast' => 'int');
?>
