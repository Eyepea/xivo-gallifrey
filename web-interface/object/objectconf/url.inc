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
$array['index'] = 'index.php';

$array['xivo'] = 'xivo/index.php';
$array['xivo/configuration'] = 'xivo/configuration/index.php';
$array['xivo/configuration/manage/user'] = 'xivo/configuration/index.php/manage/user/';
$array['xivo/configuration/manage/entity'] = 'xivo/configuration/index.php/manage/entity/';
$array['xivo/configuration/manage/server'] = 'xivo/configuration/index.php/manage/server/';
$array['xivo/configuration/manage/ldapserver'] = 'xivo/configuration/index.php/manage/ldapserver/';
$array['xivo/configuration/manage/accesswebservice'] = 'xivo/configuration/index.php/manage/accesswebservice/';
$array['xivo/configuration/network/interface'] = 'xivo/configuration/index.php/network/interface/';
$array['xivo/configuration/network/iproute'] = 'xivo/configuration/index.php/network/iproute/';
$array['xivo/configuration/network/resolvconf'] = 'xivo/configuration/index.php/network/resolvconf/';
$array['xivo/preferences'] = 'xivo/preferences.php';
$array['xivo/help'] = 'xivo/help.php';
$array['xivo/contact'] = 'xivo/contact.php';
$array['xivo/logoff'] = 'xivo/logoff.php';

$array['service/ipbx'] = 'service/ipbx/index.php';
$array['service/ipbx/general_settings/sip'] = 'service/ipbx/index.php/general_settings/sip/';
$array['service/ipbx/general_settings/iax'] = 'service/ipbx/index.php/general_settings/iax/';
#$array['service/ipbx/general_settings/cti'] = 'service/ipbx/index.php/general_settings/cti/'; // TO BE REMOVED
$array['service/ipbx/general_settings/voicemail'] = 'service/ipbx/index.php/general_settings/voicemail/';
$array['service/ipbx/general_settings/phonebook'] = 'service/ipbx/index.php/general_settings/phonebook/';
$array['service/ipbx/general_settings/advanced'] = 'service/ipbx/index.php/general_settings/advanced/';

$array['service/ipbx/cti_settings/general'] = 'service/ipbx/index.php/cti_settings/general/';
$array['service/ipbx/cti_settings/presences'] = 'service/ipbx/index.php/cti_settings/presences/';
$array['service/ipbx/cti_settings/phonehints'] = 'service/ipbx/index.php/cti_settings/phonehints/';
$array['service/ipbx/cti_settings/profiles'] = 'service/ipbx/index.php/cti_settings/profiles/';

$array['service/ipbx/pbx_settings/agents'] = 'service/ipbx/index.php/pbx_settings/agents/';
$array['service/ipbx/pbx_settings/users'] = 'service/ipbx/index.php/pbx_settings/users/';
$array['service/ipbx/pbx_settings/groups'] = 'service/ipbx/index.php/pbx_settings/groups/';
$array['service/ipbx/pbx_settings/queues'] = 'service/ipbx/index.php/pbx_settings/queues/';
$array['service/ipbx/pbx_settings/meetme'] = 'service/ipbx/index.php/pbx_settings/meetme/';
$array['service/ipbx/pbx_settings/voicemail'] = 'service/ipbx/index.php/pbx_settings/voicemail/';
$array['service/ipbx/call_management/incall'] = 'service/ipbx/index.php/call_management/incall/';
$array['service/ipbx/call_management/outcall'] = 'service/ipbx/index.php/call_management/outcall/';
$array['service/ipbx/call_management/rightcall'] = 'service/ipbx/index.php/call_management/rightcall/';
$array['service/ipbx/call_management/callfilter'] = 'service/ipbx/index.php/call_management/callfilter/';
$array['service/ipbx/call_management/schedule'] = 'service/ipbx/index.php/call_management/schedule/';
$array['service/ipbx/call_management/voicemenu'] = 'service/ipbx/index.php/call_management/voicemenu/';
$array['service/ipbx/call_management/cdr'] = 'service/ipbx/index.php/call_management/cdr/';
$array['service/ipbx/trunk_management/sip'] = 'service/ipbx/index.php/trunk_management/sip/';
$array['service/ipbx/trunk_management/iax'] = 'service/ipbx/index.php/trunk_management/iax/';
$array['service/ipbx/trunk_management/custom'] = 'service/ipbx/index.php/trunk_management/custom/';
$array['service/ipbx/pbx_services/sounds'] = 'service/ipbx/index.php/pbx_services/sounds/';
$array['service/ipbx/pbx_services/musiconhold'] = 'service/ipbx/index.php/pbx_services/musiconhold/';
$array['service/ipbx/pbx_services/extenfeatures'] = 'service/ipbx/index.php/pbx_services/extenfeatures/';
$array['service/ipbx/pbx_services/handynumbers'] = 'service/ipbx/index.php/pbx_services/handynumbers/';
$array['service/ipbx/pbx_services/phonebook'] = 'service/ipbx/index.php/pbx_services/phonebook/';
$array['service/ipbx/system_management/backupfiles'] = 'service/ipbx/index.php/system_management/backupfiles/';
$array['service/ipbx/system_management/configfiles'] = 'service/ipbx/index.php/system_management/configfiles/';
$array['service/ipbx/system_management/context'] = 'service/ipbx/index.php/system_management/context/';
$array['service/ipbx/system_management/ldapfilter'] = 'service/ipbx/index.php/system_management/ldapfilter/';
$array['service/ipbx/control_system/logfiles'] = 'service/ipbx/index.php/control_system/logfiles/';
$array['service/ipbx/control_system/reload'] = 'service/ipbx/index.php/control_system/reload/';
$array['service/ipbx/control_system/restart'] = 'service/ipbx/index.php/control_system/restart/';
$array['service/ipbx/web_services/phonebook/search'] = 'service/ipbx/web_services.php/phonebook/search/';

$array['service/ipbx/json/phonebook/local'] = 'service/ipbx/json.php/phonebook/local/';

$array['service/ipbx/json/restricted/ctiserver/configuration'] = 'service/ipbx/json.php/restricted/ctiserver/configuration/';

$array['service/ipbx/json/restricted/pbx_settings/agents'] = 'service/ipbx/json.php/restricted/pbx_settings/agents/';
$array['service/ipbx/json/restricted/pbx_settings/users'] = 'service/ipbx/json.php/restricted/pbx_settings/users/';
$array['service/ipbx/json/restricted/pbx_settings/groups'] = 'service/ipbx/json.php/restricted/pbx_settings/groups/';
$array['service/ipbx/json/restricted/pbx_settings/queues'] = 'service/ipbx/json.php/restricted/pbx_settings/queues/';
$array['service/ipbx/json/restricted/pbx_settings/meetme'] = 'service/ipbx/json.php/restricted/pbx_settings/meetme/';
$array['service/ipbx/json/restricted/pbx_settings/voicemail'] = 'service/ipbx/json.php/restricted/pbx_settings/voicemail/';
$array['service/ipbx/json/restricted/call_management/incall'] = 'service/ipbx/json.php/restricted/call_management/incall/';

$array['service/ipbx/json/private/ctiserver/configuration'] = 'service/ipbx/json.php/private/ctiserver/configuration/';

$array['service/ipbx/json/private/pbx_settings/agents'] = 'service/ipbx/json.php/private/pbx_settings/agents/';
$array['service/ipbx/json/private/pbx_settings/users'] = 'service/ipbx/json.php/private/pbx_settings/users/';
$array['service/ipbx/json/private/pbx_settings/groups'] = 'service/ipbx/json.php/private/pbx_settings/groups/';
$array['service/ipbx/json/private/pbx_settings/queues'] = 'service/ipbx/json.php/private/pbx_settings/queues/';
$array['service/ipbx/json/private/pbx_settings/meetme'] = 'service/ipbx/json.php/private/pbx_settings/meetme/';
$array['service/ipbx/json/private/pbx_settings/voicemail'] = 'service/ipbx/json.php/private/pbx_settings/voicemail/';
$array['service/ipbx/json/private/call_management/incall'] = 'service/ipbx/json.php/private/call_management/incall/';

$array['service/ipbx/ui/pbx_settings/agents'] = 'service/ipbx/ui.php/pbx_settings/agents/';
$array['service/ipbx/ui/pbx_settings/users'] = 'service/ipbx/ui.php/pbx_settings/users/';
$array['service/ipbx/ui/pbx_settings/users/search'] = 'service/ipbx/ui.php/pbx_settings/users/search/';
$array['service/ipbx/ui/pbx_settings/users/groups'] = 'service/ipbx/ui.php/pbx_settings/users/groups/';
$array['service/ipbx/ui/pbx_settings/users/groups/search'] = 'service/ipbx/ui.php/pbx_settings/users/groups/search/';
$array['service/ipbx/ui/pbx_settings/users/queues'] = 'service/ipbx/ui.php/pbx_settings/users/queues/';
$array['service/ipbx/ui/pbx_settings/users/queues/search'] = 'service/ipbx/ui.php/pbx_settings/users/queues/search/';
$array['service/ipbx/ui/pbx_settings/users/meetme'] = 'service/ipbx/ui.php/pbx_settings/users/meetme/';
$array['service/ipbx/ui/pbx_settings/users/meetme/search'] = 'service/ipbx/ui.php/pbx_settings/users/meetme/search/';
$array['service/ipbx/ui/pbx_settings/users/voicemail'] = 'service/ipbx/ui.php/pbx_settings/users/voicemail/';
$array['service/ipbx/ui/pbx_settings/users/voicemail/search'] = 'service/ipbx/ui.php/pbx_settings/users/voicemail/search/';
$array['service/ipbx/ui/pbx_settings/users/voicemail/view'] = 'service/ipbx/ui.php/pbx_settings/users/voicemail/view/';
$array['service/ipbx/ui/pbx_settings/groups'] = 'service/ipbx/ui.php/pbx_settings/groups/';
$array['service/ipbx/ui/pbx_settings/queues'] = 'service/ipbx/ui.php/pbx_settings/queues/';
$array['service/ipbx/ui/pbx_settings/meetme'] = 'service/ipbx/ui.php/pbx_settings/meetme/';
$array['service/ipbx/ui/pbx_settings/voicemail'] = 'service/ipbx/ui.php/pbx_settings/voicemail/';

?>
