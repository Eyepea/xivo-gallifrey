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

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('agent',
					   'callfilter',
					   'generalsip',
					   'generaliax',
					   'generalvoicemail',
					   'meetme',
					   'outcall',
					   'queue',
					   'rightcall',
					   'schedule',
					   'trunksip',
					   'trunkiax',
					   'trunkcustom',
					   'voicemail',
					   'voicemenu');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['element']['varname'] = array();
$array['element']['varname']['default'] = '';


$array['filter'] = array();
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context',
				    'callbackarg' => array(array(
							XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['type'] = array('agent',
				 'callfilter',
				 'generalsip',
				 'generaliax',
				 'generalvoicemail',
				 'meetme',
				 'outcall',
				 'queue',
				 'rightcall',
				 'schedule',
				 'trunksip',
				 'trunkiax',
				 'trunkcustom',
				 'voicemail',
				 'voicemenu');
$array['filter']['typeval'] = array('set' => false,'chk' => 2,'maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/');
$array['filter']['varname'] = array('set' => false,'chk' => 2,'maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/');

?>
