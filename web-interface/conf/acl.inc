<?php

$array = array();
$array['tree'] = array();
$array['tree']['service'] = array();

$array['tree']['service']['ipbx'] = array();

$array['tree']['service']['ipbx']['general_settings'] = array();
$array['tree']['service']['ipbx']['general_settings']['sip'] = true;
$array['tree']['service']['ipbx']['general_settings']['iax'] = true;
$array['tree']['service']['ipbx']['general_settings']['voicemail'] = true;
$array['tree']['service']['ipbx']['general_settings']['agents'] = true;
$array['tree']['service']['ipbx']['general_settings']['queues'] = true;
$array['tree']['service']['ipbx']['general_settings']['sounds'] = true;
$array['tree']['service']['ipbx']['general_settings']['musiconhold'] = true;
$array['tree']['service']['ipbx']['general_settings']['extenfeatures'] = true;
$array['tree']['service']['ipbx']['general_settings']['outcall'] = true;

$array['tree']['service']['ipbx']['pbx_settings'] = array();
$array['tree']['service']['ipbx']['pbx_settings']['agents'] = true;
$array['tree']['service']['ipbx']['pbx_settings']['users'] = true;
$array['tree']['service']['ipbx']['pbx_settings']['groups'] = true;
$array['tree']['service']['ipbx']['pbx_settings']['queues'] = true;
$array['tree']['service']['ipbx']['pbx_settings']['meetme'] = true;

$array['tree']['service']['ipbx']['call_management'] = array();
$array['tree']['service']['ipbx']['call_management']['incall'] = true;
$array['tree']['service']['ipbx']['call_management']['outcall'] = true;
$array['tree']['service']['ipbx']['call_management']['rightcall'] = true;
$array['tree']['service']['ipbx']['call_management']['schedule'] = true;
#$array['tree']['service']['ipbx']['call_management']['ivr'] = true;
$array['tree']['service']['ipbx']['call_management']['cdr'] = true;

$array['tree']['service']['ipbx']['trunk_management'] = array();
$array['tree']['service']['ipbx']['trunk_management']['sip'] = true;
$array['tree']['service']['ipbx']['trunk_management']['iax'] = true;
$array['tree']['service']['ipbx']['trunk_management']['custom'] = true;

$array['tree']['service']['ipbx']['system_management'] = array();
$array['tree']['service']['ipbx']['system_management']['configfiles'] = true;
#$array['tree']['service']['ipbx']['system_management']['isdncard'] = true;
#$array['tree']['service']['ipbx']['system_management']['modules'] = true;
#$array['tree']['service']['ipbx']['system_management']['manager'] = true;

$array['tree']['service']['ipbx']['control_system'] = array();
$array['tree']['service']['ipbx']['control_system']['reload'] = true;


$array['group'] = array();
$array['group']['visitor'] = false;

$array['group']['user'] = array();
$array['group']['user']['service'] = false;

$array['group']['admin'] = array();
$array['group']['admin']['service'] = false;

$array['group']['root'] = true;

$array['ip'] = array();
$array['ip']['xivo'] = array('192.168.1.','proformatique.com','192.168.2/14');

?>
