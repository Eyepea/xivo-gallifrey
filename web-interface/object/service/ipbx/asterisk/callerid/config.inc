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

$array['element']['mode'] = array();
$array['element']['mode']['value'] = array('prepend','overwrite','append');
$array['element']['mode']['default'] = '';
$array['element']['mode']['null'] = true;

$array['element']['callerdisplay'] = array();
$array['element']['callerdisplay']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('callfilter','incall','group','queue');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['filter'] = array();
$array['filter']['mode'] = array('set' => false,'chk' => 2,'key' => array('prepend','overwrite','append'));
$array['filter']['callerdisplay'] = array('set'		=> false,
					  'chk'		=> 2,
					  'maxlen'	=> 80,
					  'regexp'	=> '/^'.XIVO_SRE_IPBX_AST_REGEXP_CALLERID.'$/u',
					  'notag'	=> false);
$array['filter']['type'] = array('key' => array('callfilter','incall','group','queue'));
$array['filter']['typeval'] = array('cast' => 'ulongint');

?>
