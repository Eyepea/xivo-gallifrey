<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

$array['service/ipbx/asterisk/ui/pbx_settings/users/agents/search'] = array(
	'path'		=> 'service/ipbx/asterisk/ui/pbx_settings/agents',
	'define'	=> array(
			'XIVO_LOC_UI_ACL_CATEGORY'	=> 'pbx_settings',
			'XIVO_LOC_UI_ACL_SUBCATEGORY'	=> 'agents',
			'XIVO_LOC_UI_ACTION'		=> 'search'));

$array['service/ipbx/asterisk/ui/pbx_settings/users/search'] = array(
	'path'		=> 'service/ipbx/asterisk/ui/pbx_settings/users',
	'define'	=> array('XIVO_LOC_UI_ACTION'	=> 'search'));

$array['service/ipbx/asterisk/ui/pbx_settings/users/groups/search'] = array(
	'path'		=> 'service/ipbx/asterisk/ui/pbx_settings/groups',
	'define'	=> array(
			'XIVO_LOC_UI_ACL_CATEGORY'	=> 'pbx_settings',
			'XIVO_LOC_UI_ACL_SUBCATEGORY'	=> 'users',
			'XIVO_LOC_UI_ACTION'		=> 'search'));

$array['service/ipbx/asterisk/ui/pbx_settings/users/queues/search'] = array(
	'path'		=> 'service/ipbx/asterisk/ui/pbx_settings/queues',
	'define'	=> array(
			'XIVO_LOC_UI_ACL_CATEGORY'	=> 'pbx_settings',
			'XIVO_LOC_UI_ACL_SUBCATEGORY'	=> 'users',
			'XIVO_LOC_UI_ACTION'		=> 'search'));

$array['service/ipbx/asterisk/ui/pbx_settings/users/meetme/search'] = array(
	'path'		=> 'service/ipbx/asterisk/ui/pbx_settings/meetme',
	'define'	=> array(
			'XIVO_LOC_UI_ACL_CATEGORY'	=> 'pbx_settings',
			'XIVO_LOC_UI_ACL_SUBCATEGORY'	=> 'users',
			'XIVO_LOC_UI_ACTION'		=> 'search'));

$array['service/ipbx/asterisk/ui/pbx_settings/users/voicemail/search'] = array(
	'path'		=> 'service/ipbx/asterisk/ui/pbx_settings/voicemail',
	'define'	=> array('XIVO_LOC_UI_ACTION'	=> 'search'));

$array['service/ipbx/asterisk/ui/pbx_settings/users/voicemail/view'] = array(
	'path'		=> 'service/ipbx/asterisk/ui/pbx_settings/voicemail',
	'define'	=> array('XIVO_LOC_UI_ACTION'	=> 'view'));

$array['service/ipbx/asterisk/ui/pbx_settings/meetme/users/search'] = array(
	'path'		=> 'service/ipbx/asterisk/ui/pbx_settings/users',
	'define'	=> array(
			'XIVO_LOC_UI_ACL_CATEGORY'	=> 'pbx_settings',
			'XIVO_LOC_UI_ACL_SUBCATEGORY'	=> 'meetme',
			'XIVO_LOC_UI_ACTION'		=> 'search'));

$array['xivo/configuration/web_services/private/manage/entity'] = array(
	'path'		=> 'xivo/configuration/web_services/manage/entity',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['xivo/configuration/web_services/private/network/interface'] = array(
	'path'		=> 'xivo/configuration/web_services/network/interface',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['xivo/configuration/web_services/private/network/iproute'] = array(
	'path'		=> 'xivo/configuration/web_services/network/iproute',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['xivo/configuration/web_services/private/network/resolvconf'] = array(
	'path'		=> 'xivo/configuration/web_services/network/resolvconf',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));



$array['service/ipbx/asterisk/web_services/private/ctiserver/configuration'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/ctiserver/configuration',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));

$array['service/ipbx/asterisk/web_services/private/pbx_settings/agents'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/agents',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_settings/users'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/users',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_settings/groups'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/groups',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_settings/queues'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/queues',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_settings/meetme'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/meetme',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_settings/voicemail'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/voicemail',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/call_management/incall'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/incall',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/call_management/outcall'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/outcall',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/call_management/cdr'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/cdr',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/trunk_management/sip'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/sip',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/trunk_management/iax'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/iax',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/trunk_management/custom'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/custom',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/pbx_services/phonebook'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_services/phonebook',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/system_management/context'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/system_management/context',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/control_system/reload'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/control_system/reload',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));
$array['service/ipbx/asterisk/web_services/private/control_system/restart'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/control_system/restart',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'private'));


// RESTRICTED
$array['xivo/configuration/web_services/restricted/manage/entity'] = array(
	'path'		=> 'xivo/configuration/web_services/manage/entity',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['xivo/configuration/web_services/restricted/network/interface'] = array(
	'path'		=> 'xivo/configuration/web_services/network/interface',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['xivo/configuration/web_services/restricted/network/iproute'] = array(
	'path'		=> 'xivo/configuration/web_services/network/iproute',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['xivo/configuration/web_services/restricted/network/resolvconf'] = array(
	'path'		=> 'xivo/configuration/web_services/network/resolvconf',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['xivo/configuration/web_services/restricted/network/mail'] = array(
	'path'		=> 'xivo/configuration/web_services/network/mail',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['xivo/configuration/web_services/restricted/network/dhcp'] = array(
	'path'		=> 'xivo/configuration/web_services/network/dhcp',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['xivo/configuration/web_services/restricted/network/ha'] = array(
	'path'		=> 'xivo/configuration/web_services/network/ha',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));

$array['xivo/configuration/web_services/restricted/support/monitoring'] = array(
	'path'		=> 'xivo/configuration/web_services/support/monitoring',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
			

$array['service/ipbx/asterisk/web_services/restricted/ctiserver/configuration'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/ctiserver/configuration',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));

$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/agents'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/agents',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/users'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/users',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/groups'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/groups',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/queues'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/queues',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/meetme'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/meetme',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_settings/voicemail'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_settings/voicemail',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/call_management/incall'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/incall',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/call_management/outcall'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/outcall',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/call_management/cdr'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_management/cdr',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/trunk_management/sip'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/sip',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/trunk_management/iax'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/iax',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/trunk_management/custom'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/trunk_management/custom',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/pbx_services/phonebook'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/pbx_services/phonebook',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/system_management/context'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/system_management/context',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/control_system/reload'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/control_system/reload',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/control_system/restart'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/control_system/restart',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));

$array['service/ipbx/asterisk/web_services/restricted/call_center/queueskills'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_center/queueskills',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));
$array['service/ipbx/asterisk/web_services/restricted/call_center/queueskillrules'] = array(
	'path'		=> 'service/ipbx/asterisk/web_services/call_center/queueskillrules',
	'define'	=> array(
			'XIVO_LOC_WEBSERVICES_MODE'	=> 'restricted'));

?>
