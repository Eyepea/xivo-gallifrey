<?php

$array = array();
$array['tree'] = array();
$array['tree']['service'] = array();
$array['tree']['service']['ipbx'] = array();
$array['tree']['service']['ipbx']['general_settings'] = array('sipprotocol' => true,'iaxprotocol' => true,'voicemail' => true,'sounds' => true,'musiconhold' => true,'extenfeatures' => true);
$array['tree']['service']['ipbx']['pbx_settings'] = array('users' => true,'groups' => true,'meetme' => true,'queues' => true,'agents' => true);
$array['tree']['service']['ipbx']['call_management'] = array('ivr' => true,'cdr' => true);
$array['tree']['service']['ipbx']['trunk_management'] = array('sip' => true,'iax' => true);
$array['tree']['service']['ipbx']['system_management'] = array('configfiles' => true,'isdncard' => true,'modules' => true,'manager' => true);
$array['tree']['service']['ipbx']['control_system'] = array('reload' => true);
/*
$array['tree']['service']['ipbx']['_query'] = array();
$array['tree']['service']['ipbx']['_query']['dir'] = 'ipbx';
$array['tree']['service']['ipbx']['_query']['list'] = array();
$array['tree']['service']['ipbx']['_query']['list']['pbx_settings::local_extension'] = array();
$array['tree']['service']['ipbx']['_query']['list']['pbx_settings::local_extension']['url'] = 'user';
$array['tree']['service']['ipbx']['_query']['list']['pbx_settings::local_extension']['param'] = array('cat' => 'add');
*/
$array['group'] = array();
$array['group']['visitor'] = false;
$array['group']['user']['service'] = false;
$array['group']['admin'] = array();
$array['group']['admin']['service'] = array();
$array['group']['admin']['service']['ipbx'] = true;
/*
$array['group']['admin']['service']['ipbx']['pbx_settings'] = array('local_extension' => true);
$array['group']['admin']['service']['ipbx']['call_management'] = array('ivr' => true,'cdr' => true);
*/
//$array['group']['root'] = true;
$array['group']['root'] = array();
$array['group']['root']['service'] = array();
$array['group']['root']['service']['ipbx'] = array();
$array['group']['root']['service']['ipbx']['general_settings'] = array('sipprotocol' => true,'iaxprotocol' => true,'voicemail' => true,'sounds' => true,'musiconhold' => true,'extenfeatures' => true);
$array['group']['root']['service']['ipbx']['pbx_settings'] = array('users' => true,'groups' => true,'queues' => true,'agents' => true,'meetme' => true);
$array['group']['root']['service']['ipbx']['call_management'] = array('ivr' => true,'cdr' => true);
$array['group']['root']['service']['ipbx']['trunk_management'] = array('sip' => true,'iax' => true);
$array['group']['root']['service']['ipbx']['system_management'] = array('configfiles' => true,'isdncard' => true,'modules' => true,'manager' => true);
$array['group']['root']['service']['ipbx']['control_system'] = array('reload' => true);

$array['ip'] = array();
$array['ip']['xivo'] = array('192.168.1.','proformatique.com','192.168.2/14');

?>
