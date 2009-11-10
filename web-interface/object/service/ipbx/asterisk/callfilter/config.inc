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

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('bosssecretary');
$array['element']['type']['default'] = 'bosssecretary';
$array['element']['type']['forcedef'] = true;

$array['element']['bosssecretary'] = array();
$array['element']['bosssecretary']['value'] = array('bossfirst-serial',
						    'bossfirst-simult',
						    'secretary-serial',
						    'secretary-simult',
						    'all');
$array['element']['bosssecretary']['default'] = 'secretary-simult';
$array['element']['bosssecretary']['null'] = true;

$array['element']['callfrom'] = array();
$array['element']['callfrom']['value'] = array('internal','external','all');
$array['element']['callfrom']['default'] = 'all';

$array['element']['ringseconds'] = array();
$array['element']['ringseconds']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['ringseconds']['default'] = 0;
$array['element']['ringseconds']['setdef'] = true;

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['filter'] = array();
$array['filter']['name'] = array('maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['type'] = array('set' => false);
$array['filter']['bosssecretary'] = array('set' => false,'key' => array('bossfirst-serial',
									'bossfirst-simult',
									'secretary-serial',
									'secretary-simult',
									'all'));
$array['filter']['callfrom'] = array('key' => array('internal','external','all'));
$array['filter']['ringseconds'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,60,5));
$array['filter']['commented'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
