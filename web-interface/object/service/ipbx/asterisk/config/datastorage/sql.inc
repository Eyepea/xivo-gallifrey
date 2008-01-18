<?php

$array = array();

$array['accessfeatures'] = array();
$array['accessfeatures']['table'] = 'accessfeatures';
$array['accessfeatures']['primary'] = array('id' => 0);

$array['agent'] = array();
$array['agent']['table'] = 'agent';
$array['agent']['primary'] = array('id' => 0);

$array['agentfeatures'] = array();
$array['agentfeatures']['table'] = 'agentfeatures';
$array['agentfeatures']['primary'] = array('id' => 0);

$array['agentgroup'] = array();
$array['agentgroup']['table'] = 'agentgroup';
$array['agentgroup']['primary'] = array('id' => 0);

$array['agents'] = array();
$array['agents']['table'] = 'agent';
$array['agents']['primary'] = array('id' => 0);

$array['autoprov'] = array();
$array['autoprov']['table'] = 'phone';
$array['autoprov']['primary'] = array('macaddr' => '');

$array['callfilter'] = array();
$array['callfilter']['table'] = 'callfilter';
$array['callfilter']['primary'] = array('id' => 0);

$array['callfiltermember'] = array();
$array['callfiltermember']['table'] = 'callfiltermember';
$array['callfiltermember']['primary'] = array('id' => 0);

$array['cdr'] = array();
$array['cdr']['table'] = 'cdr';
$array['cdr']['primary'] = array('id' => 0);

$array['dialstatus'] = array();
$array['dialstatus']['table'] = 'dialstatus';
$array['dialstatus']['primary'] = array('id' => 0);

$array['extensions'] = array();
$array['extensions']['table'] = 'extensions';
$array['extensions']['primary'] = array('id' => 0);

$array['extenumbers'] = array();
$array['extenumbers']['table'] = 'extenumbers';
$array['extenumbers']['primary'] = array('id' => 0);

$array['features'] = array();
$array['features']['table'] = 'features';
$array['features']['primary'] = array('id' => 0);

$array['generaliax'] = array();
$array['generaliax']['table'] = 'generaliax';
$array['generaliax']['primary'] = array('id' => 0);

$array['generalqueue'] = array();
$array['generalqueue']['table'] = 'generalqueue';
$array['generalqueue']['primary'] = array('id' => 0);

$array['generalsip'] = array();
$array['generalsip']['table'] = 'generalsip';
$array['generalsip']['primary'] = array('id' => 0);

$array['generalvoicemail'] = array();
$array['generalvoicemail']['table'] = 'generalvoicemail';
$array['generalvoicemail']['primary'] = array('id' => 0);

$array['groupfeatures'] = array();
$array['groupfeatures']['table'] = 'groupfeatures';
$array['groupfeatures']['primary'] = array('id' => 0);

$array['handynumbers'] = array();
$array['handynumbers']['table'] = 'handynumbers';
$array['handynumbers']['primary'] = array('id' => 0);

$array['incall'] = array();
$array['incall']['table'] = 'incall';
$array['incall']['primary'] = array('id' => 0);

$array['meetme'] = array();
$array['meetme']['table'] = 'meetme';
$array['meetme']['primary'] = array('id' => 0);

$array['meetmefeatures'] = array();
$array['meetmefeatures']['table'] = 'meetmefeatures';
$array['meetmefeatures']['primary'] = array('id' => 0);

$array['musiconhold'] = array();
$array['musiconhold']['table'] = 'musiconhold';
$array['musiconhold']['primary'] = array('id' => 0);

$array['outcall'] = array();
$array['outcall']['table'] = 'outcall';
$array['outcall']['primary'] = array('id' => 0);

$array['outcalltrunk'] = array();
$array['outcalltrunk']['table'] = 'outcalltrunk';
$array['outcalltrunk']['primary'] = array('outcallid' => 0,'trunkfeaturesid' => 0);

$array['phonebook'] = array();
$array['phonebook']['table'] = 'phonebook';
$array['phonebook']['primary'] = array('id' => 0);

$array['phonebookaddress'] = array();
$array['phonebookaddress']['table'] = 'phonebookaddress';
$array['phonebookaddress']['primary'] = array('id' => 0);

$array['phonebooknumber'] = array();
$array['phonebooknumber']['table'] = 'phonebooknumber';
$array['phonebooknumber']['primary'] = array('id' => 0);

$array['phonefunckey'] = array();
$array['phonefunckey']['table'] = 'phonefunckey';
$array['phonefunckey']['primary'] = array('iduserfeatures' => 0,'fknum' => 0);

$array['queue'] = array();
$array['queue']['table'] = 'queue';
$array['queue']['primary'] = array('name' => '');

$array['queuefeatures'] = array();
$array['queuefeatures']['table'] = 'queuefeatures';
$array['queuefeatures']['primary'] = array('id' => 0);

$array['queuemember'] = array();
$array['queuemember']['table'] = 'queuemember';
$array['queuemember']['primary'] = array('queue_name' => '','interface' => '');

$array['rightcall'] = array();
$array['rightcall']['table'] = 'rightcall';
$array['rightcall']['primary'] = array('id' => 0);

$array['rightcallexten'] = array();
$array['rightcallexten']['table'] = 'rightcallexten';
$array['rightcallexten']['primary'] = array('id' => 0);

$array['rightcallmember'] = array();
$array['rightcallmember']['table'] = 'rightcallmember';
$array['rightcallmember']['primary'] = array('id' => 0);

$array['schedule'] = array();
$array['schedule']['table'] = 'schedule';
$array['schedule']['primary'] = array('id' => 0);

$array['serverfeatures'] = array();
$array['serverfeatures']['table'] = 'serverfeatures';
$array['serverfeatures']['primary'] = array('id' => 0);

$array['trunkcustom'] = array();
$array['trunkcustom']['table'] = 'usercustom';
$array['trunkcustom']['primary'] = array('id' => 0);

$array['trunkfeatures'] = array();
$array['trunkfeatures']['table'] = 'trunkfeatures';
$array['trunkfeatures']['primary'] = array('id' => 0);

$array['trunkiax'] = array();
$array['trunkiax']['table'] = 'useriax';
$array['trunkiax']['primary'] = array('id' => 0);

$array['trunksip'] = array();
$array['trunksip']['table'] = 'usersip';
$array['trunksip']['primary'] = array('id' => 0);

$array['usercustom'] = array();
$array['usercustom']['table'] = 'usercustom';
$array['usercustom']['primary'] = array('id' => 0);

$array['userfeatures'] = array();
$array['userfeatures']['table'] = 'userfeatures';
$array['userfeatures']['primary'] = array('id' => 0);

$array['useriax'] = array();
$array['useriax']['table'] = 'useriax';
$array['useriax']['primary'] = array('id' => 0);

$array['usersip'] = array();
$array['usersip']['table'] = 'usersip';
$array['usersip']['primary'] = array('id' => 0);

$array['uservoicemail'] = array();
$array['uservoicemail']['table'] = 'uservoicemail';
$array['uservoicemail']['primary'] = array('uniqueid' => 0);

$array['zonemessages'] = array();
$array['zonemessages']['table'] = 'generalvoicemail';
$array['zonemessages']['primary'] = array('id' => 0);

?>
