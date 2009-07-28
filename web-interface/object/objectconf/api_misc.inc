<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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
$array['xivo/configuration/web_services/private/manage/entity'] = array(
	'path'		=> 'xivo/configuration/web_services/manage/entity',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));

$array['service/ipbx/asterisk/web_services/private/pbx_settings/agents'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/agents',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_settings/users'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/users',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_settings/groups'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/groups',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_settings/queues'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/queues',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_settings/meetme'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/meetme',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_settings/voicemail'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/voicemail',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/call_management/incall'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/incall',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/call_management/outcall'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/outcall',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/call_management/cdr'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/cdr',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/trunk_management/sip'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/sip',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/trunk_management/iax'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/iax',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/trunk_management/custom'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/custom',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_services/phonebook'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_services/phonebook',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/system_management/context'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/system_management/context',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/control_system/reload'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/control_system/reload',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/control_system/restart'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/control_system/restart',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'private'));

$array['xivo/configuration/web_services/restricted/manage/entity'] = array(
	'path'		=> 'xivo/configuration/web_services/manage/entity',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));

$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/agents'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/agents',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/users'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/users',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/groups'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/groups',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/queues'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/queues',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/meetme'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/meetme',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/voicemail'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/voicemail',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/call_management/incall'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/incall',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/call_management/outcall'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/outcall',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/call_management/cdr'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/cdr',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/trunk_management/sip'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/sip',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/trunk_management/iax'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/iax',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/trunk_management/custom'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/custom',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_services/phonebook'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_services/phonebook',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/system_management/context'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/system_management/context',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/control_system/reload'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/control_system/reload',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/control_system/restart'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/control_system/restart',
	'define'	=> array(
			'XIVO_TPL_WEBSERVICES_MODE'	=> 'restricted'));

?>