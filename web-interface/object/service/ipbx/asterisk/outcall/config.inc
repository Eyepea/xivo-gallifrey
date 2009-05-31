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

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['externprefix'] = array();
$array['element']['externprefix']['default'] = '';

$array['element']['stripnum'] = array();
$array['element']['stripnum']['value'] = range(0,20);
$array['element']['stripnum']['default'] = 0;
$array['element']['stripnum']['setdef'] = true;

$array['element']['mode'] = array();
$array['element']['mode']['value'] = array('wizard','extension');
$array['element']['mode']['default'] = 'extension';
$array['element']['mode']['unset'] = true;

$array['element']['prefix'] = array();
$array['element']['prefix']['default'] = '';
$array['element']['prefix']['unset'] = true;

$array['element']['numlen'] = array();
$array['element']['numlen']['value'] = array('*',1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
					     21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39);
$array['element']['numlen']['default'] = '';
$array['element']['numlen']['unset'] = true;

$array['element']['callerid'] = array();
$array['element']['callerid']['default'] = '';

$array['element']['setcallerid'] = array();
$array['element']['setcallerid']['value'] = array(0,1);
$array['element']['setcallerid']['default'] = 0;
$array['element']['setcallerid']['set'] = true;

$array['element']['useenum'] = array();
$array['element']['useenum']['value'] = array(0,1);
$array['element']['useenum']['default'] = 0;
$array['element']['useenum']['set'] = true;

$array['element']['internal'] = array();
$array['element']['internal']['value'] = array(0,1);
$array['element']['internal']['default'] = 0;
$array['element']['internal']['set'] = true;

$array['element']['preprocess_subroutine'] = array();
$array['element']['preprocess_subroutine']['default'] = '';
$array['element']['preprocess_subroutine']['null'] = true;

$array['element']['hangupringtime'] = array();
$array['element']['hangupringtime']['value'] = array(0,15,30,45,60,75,90,
						     105,120,135,150,165,180,195,
						     210,225,240,255,270,285,300);
$array['element']['hangupringtime']['default'] = 0;
$array['element']['hangupringtime']['setdef'] = true;

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['filter'] = array();
$array['filter']['name'] = array('maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['exten'] = array('maxlen' => 40,'regexp' => '/^_?\+?[0-9NXZ\*#\-\[\]]+[\.\!]?$/');
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['externprefix'] = array('set' => false,'chk' => 2,'maxlen' => 20,'regexp' => '/^(?:\+|\+?[0-9#\*]+)$/');
$array['filter']['stripnum'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,20));
$array['filter']['mode'] = array('set' => false,'chk' => 2,'key' => array('wizard','extension'));
$array['filter']['prefix'] = array('set' => false,'chk' => 2,'maxlen' => 20,'regexp' => '/^(?:\+|\+?[0-9#\*]+)$/');
$array['filter']['numlen'] = array('set' => false,'chk' => 2,'key' => array('*',1,2,3,4,5,6,7,8,9,
									    10,11,12,13,14,15,16,17,18,19,20,
					     				    21,22,23,24,25,26,27,28,29,30,
									    31,32,33,34,35,36,37,38,39));
$array['filter']['setcallerid'] = array('bool' => true);
$array['filter']['callerid'] = array('set'	=> false,
				     'chk'	=> 2,
				     'maxlen'	=> 80,
				     'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_CALLERID.'$/u',
				     'notag'	=> false);
$array['filter']['useenum'] = array('bool' => true);
$array['filter']['internal'] = array('bool' => true);
$array['filter']['preprocess_subroutine'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['hangupringtime'] = array('set'	=> false,
					   'chk'	=> 2,
					   'key' => array(0,15,30,45,60,75,90,
							  105,120,135,150,165,180,
							  210,225,240,255,270,285,300));
$array['filter']['commented'] = array('bool' => true);

?>
