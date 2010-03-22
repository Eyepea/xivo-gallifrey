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
$array['tree'] = array();
$array['tree']['service'] = array();

$array['tree']['service']['ipbx'] = array();

$array['tree']['service']['ipbx']['general_settings'] = array();
$array['tree']['service']['ipbx']['general_settings']['sip'] = true;
$array['tree']['service']['ipbx']['general_settings']['iax'] = true;
//$array['tree']['service']['ipbx']['general_settings']['cti'] = false; //	 TO BE REMOVED
$array['tree']['service']['ipbx']['general_settings']['voicemail'] = true;
$array['tree']['service']['ipbx']['general_settings']['phonebook'] = true;
$array['tree']['service']['ipbx']['general_settings']['advanced'] = true;

$array['tree']['service']['ipbx']['cti_settings'] = array();
$array['tree']['service']['ipbx']['cti_settings']['general'] = true;
$array['tree']['service']['ipbx']['cti_settings']['presences'] = true;
$array['tree']['service']['ipbx']['cti_settings']['phonehints'] = true;
$array['tree']['service']['ipbx']['cti_settings']['profiles'] = true;
$array['tree']['service']['ipbx']['cti_settings']['directories'] = true;
$array['tree']['service']['ipbx']['cti_settings']['reversedirectories'] = true;
$array['tree']['service']['ipbx']['cti_settings']['displays'] = true;
$array['tree']['service']['ipbx']['cti_settings']['contexts'] = true;
$array['tree']['service']['ipbx']['cti_settings']['sheetactions'] = true;
$array['tree']['service']['ipbx']['cti_settings']['sheetevents'] = true;

$array['tree']['service']['ipbx']['call_center'] = array();
$array['tree']['service']['ipbx']['call_center']['agents'] = true;
$array['tree']['service']['ipbx']['call_center']['queues'] = true;
$array['tree']['service']['ipbx']['call_center']['queueskills'] = true;
$array['tree']['service']['ipbx']['call_center']['queueskillrules'] = true;

$array['tree']['service']['ipbx']['pbx_settings'] = array();
$array['tree']['service']['ipbx']['pbx_settings']['users'] = true;
$array['tree']['service']['ipbx']['pbx_settings']['groups'] = true;
$array['tree']['service']['ipbx']['pbx_settings']['meetme'] = true;
$array['tree']['service']['ipbx']['pbx_settings']['voicemail'] = true;
//
$array['tree']['service']['ipbx']['pbx_settings']['agents'] = true;
$array['tree']['service']['ipbx']['pbx_settings']['queues'] = true;


$array['tree']['service']['ipbx']['call_management'] = array();
$array['tree']['service']['ipbx']['call_management']['incall'] = true;
$array['tree']['service']['ipbx']['call_management']['outcall'] = true;
$array['tree']['service']['ipbx']['call_management']['rightcall'] = true;
$array['tree']['service']['ipbx']['call_management']['callfilter'] = true;
$array['tree']['service']['ipbx']['call_management']['schedule'] = true;
$array['tree']['service']['ipbx']['call_management']['voicemenu'] = true;
$array['tree']['service']['ipbx']['call_management']['cdr'] = true;

$array['tree']['service']['ipbx']['trunk_management'] = array();
$array['tree']['service']['ipbx']['trunk_management']['sip'] = true;
$array['tree']['service']['ipbx']['trunk_management']['iax'] = true;
$array['tree']['service']['ipbx']['trunk_management']['custom'] = true;

$array['tree']['service']['ipbx']['pbx_services'] = array();
$array['tree']['service']['ipbx']['pbx_services']['sounds'] = true;
$array['tree']['service']['ipbx']['pbx_services']['musiconhold'] = true;
$array['tree']['service']['ipbx']['pbx_services']['extenfeatures'] = true;
$array['tree']['service']['ipbx']['pbx_services']['handynumbers'] = true;
$array['tree']['service']['ipbx']['pbx_services']['phonebook'] = true;

$array['tree']['service']['ipbx']['system_management'] = array();
$array['tree']['service']['ipbx']['system_management']['backupfiles'] = true;
$array['tree']['service']['ipbx']['system_management']['configfiles'] = true;
$array['tree']['service']['ipbx']['system_management']['context'] = true;
$array['tree']['service']['ipbx']['system_management']['ldapfilter'] = true;

$array['tree']['service']['ipbx']['control_system'] = array();
$array['tree']['service']['ipbx']['control_system']['logfiles'] = true;
$array['tree']['service']['ipbx']['control_system']['reload'] = true;
$array['tree']['service']['ipbx']['control_system']['restart'] = true;

$array['tree']['service']['monitoring'] = array();
$array['tree']['service']['monitoring']['control_system'] = array();
$array['tree']['service']['monitoring']['control_system']['start'] = true;
$array['tree']['service']['monitoring']['control_system']['stop'] = true;
$array['tree']['service']['monitoring']['control_system']['restart'] = true;
$array['tree']['service']['monitoring']['control_system']['monitor'] = true;
$array['tree']['service']['monitoring']['control_system']['unmonitor'] = true;

$array['tree']['service']['stats'] = array();

$array['group'] = array();
$array['group']['visitor'] = false;

$array['group']['user'] = array();
$array['group']['user']['service'] = false;

$array['group']['admin'] = array();
$array['group']['admin']['service'] = false;

$array['group']['root'] = true;

?>
