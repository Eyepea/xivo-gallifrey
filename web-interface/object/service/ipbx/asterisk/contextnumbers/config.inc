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

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user','group','queue','meetme','incall');
$array['element']['type']['default'] = 'user';

$array['element']['numberbeg'] = array();
$array['element']['numberbeg']['default'] = '';

$array['element']['numberend'] = array();
$array['element']['numberend']['default'] = '';

$array['element']['didlength'] = array();
$array['element']['didlength']['value'] = range(1,20);
$array['element']['didlength']['default'] = 0;
$array['element']['didlength']['setdef'] = true;


$array['filter'] = array();
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context',
				    'callbackarg' => array(array(
							XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['type'] = array('key' => array('user','group','queue','meetme','incall'));
$array['filter']['numberbeg'] = array('maxlen' => 16,'cast' => 'digit');
$array['filter']['numberend'] = array('set' => false,'chk' => 2,'maxlen' => 16,'cast' => 'digit');
$array['filter']['didlength'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,20));

?>
