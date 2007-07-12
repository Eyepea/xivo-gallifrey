<?php

$array = array();
$array['tree'] = array();
$array['tree']['service'] = array();
$array['tree']['service']['ipbx'] = array();
$array['tree']['service']['ipbx']['general_settings'] = array('sipprotocol' => true,'iaxprotocol' => true,'voicemail' => true,'queues' => true,'agents' => true,'sounds' => true,'musiconhold' => true,'extenfeatures' => true);
$array['tree']['service']['ipbx']['pbx_settings'] = array('users' => true,'groups' => true,'meetme' => true,'queues' => true,'agents' => true);
$array['tree']['service']['ipbx']['call_management'] = array('did' => true,'ivr' => true,'cdr' => true,'outcall' => true);
$array['tree']['service']['ipbx']['trunk_management'] = array('sip' => true,'iax' => true);
$array['tree']['service']['ipbx']['system_management'] = array('configfiles' => true,'isdncard' => true,'modules' => true,'manager' => true);
$array['tree']['service']['ipbx']['control_system'] = array('reload' => true);

$array['group'] = array();
$array['group']['visitor'] = false;

$array['group']['user'] = array();
$array['group']['user']['service'] = false;

$array['group']['admin'] = array();
$array['group']['admin']['service'] = false;

$array['group']['root'] = true;
/*
$array['group']['root']['service'] = array();
$array['group']['root']['service']['ipbx'] = array();
$array['group']['root']['service']['ipbx']['general_settings'] = array('sipprotocol' => true,'iaxprotocol' => true,'voicemail' => true,'sounds' => true,'musiconhold' => true,'extenfeatures' => true);
$array['group']['root']['service']['ipbx']['pbx_settings'] = array('users' => true,'groups' => true,'queues' => true,'agents' => true,'meetme' => true);
$array['group']['root']['service']['ipbx']['call_management'] = array('did' => true,'ivr' => true,'cdr' => true);
$array['group']['root']['service']['ipbx']['trunk_management'] = array('sip' => true,'iax' => true);
$array['group']['root']['service']['ipbx']['system_management'] = array('configfiles' => true,'isdncard' => true,'modules' => true,'manager' => true);
$array['group']['root']['service']['ipbx']['control_system'] = array('reload' => true);
*/
$array['ip'] = array();
$array['ip']['xivo'] = array('192.168.1.','proformatique.com','192.168.2/14');

?>
