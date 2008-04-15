<?php

$array = array();

$array['accessfeatures'] = array();
$array['accessfeatures']['table'] = 'accessfeatures';
$array['accessfeatures']['primary'] = array('id' => '');

$array['agent'] = array();
$array['agent']['table'] = 'agent';
$array['agent']['primary'] = array('id' => '');

$array['agentfeatures'] = array();
$array['agentfeatures']['table'] = 'agentfeatures';
$array['agentfeatures']['primary'] = array('id' => '');

$array['agentgroup'] = array();
$array['agentgroup']['table'] = 'agentgroup';
$array['agentgroup']['primary'] = array('id' => '');

$array['agents'] = array();
$array['agents']['table'] = 'agent';
$array['agents']['primary'] = array('id' => '');

$array['autoprov'] = array();
$array['autoprov']['table'] = 'phone';
$array['autoprov']['primary'] = array('macaddr' => '');

$array['callfilter'] = array();
$array['callfilter']['table'] = 'callfilter';
$array['callfilter']['primary'] = array('id' => '');

$array['callfiltermember'] = array();
$array['callfiltermember']['table'] = 'callfiltermember';
$array['callfiltermember']['primary'] = array('id' => '');

$array['cdr'] = array();
$array['cdr']['table'] = 'cdr';
$array['cdr']['primary'] = array('id' => '');

$array['context'] = array();
$array['context']['table'] = 'context';
$array['context']['primary'] = array('name' => '');

$array['contextentity'] = array();
$array['contextentity']['table'] = 'contextentity';
$array['contextentity']['primary'] = array('context' => '','type' => '','typeval' => '');

$array['contextmember'] = array();
$array['contextmember']['table'] = 'contextmember';
$array['contextmember']['primary'] = array('context' => '','type' => '','typeval' => '');

$array['dialstatus'] = array();
$array['dialstatus']['table'] = 'dialstatus';
$array['dialstatus']['primary'] = array('id' => '');

$array['extensions'] = array();
$array['extensions']['table'] = 'extensions';
$array['extensions']['primary'] = array('id' => '');

$array['extenumbers'] = array();
$array['extenumbers']['table'] = 'extenumbers';
$array['extenumbers']['primary'] = array('id' => '');

$array['features'] = array();
$array['features']['table'] = 'features';
$array['features']['primary'] = array('id' => '');

$array['generalqueue'] = array();
$array['generalqueue']['table'] = 'generalqueue';
$array['generalqueue']['primary'] = array('id' => '');

$array['generalvoicemail'] = array();
$array['generalvoicemail']['table'] = 'generalvoicemail';
$array['generalvoicemail']['primary'] = array('id' => '');

$array['groupfeatures'] = array();
$array['groupfeatures']['table'] = 'groupfeatures';
$array['groupfeatures']['primary'] = array('id' => '');

$array['handynumbers'] = array();
$array['handynumbers']['table'] = 'handynumbers';
$array['handynumbers']['primary'] = array('id' => '');

$array['iax'] = array();
$array['iax']['table'] = 'generaliax';
$array['iax']['primary'] = array('id' => '');

$array['incall'] = array();
$array['incall']['table'] = 'incall';
$array['incall']['primary'] = array('id' => '');

$array['ldapfilter'] = array();
$array['ldapfilter']['table'] = 'ldapfilter';
$array['ldapfilter']['primary'] = array('id' => '');

$array['meetme'] = array();
$array['meetme']['table'] = 'meetme';
$array['meetme']['primary'] = array('id' => '');

$array['meetmefeatures'] = array();
$array['meetmefeatures']['table'] = 'meetmefeatures';
$array['meetmefeatures']['primary'] = array('id' => '');

$array['musiconhold'] = array();
$array['musiconhold']['table'] = 'musiconhold';
$array['musiconhold']['primary'] = array('id' => '');

$array['outcall'] = array();
$array['outcall']['table'] = 'outcall';
$array['outcall']['primary'] = array('id' => '');

$array['outcalltrunk'] = array();
$array['outcalltrunk']['table'] = 'outcalltrunk';
$array['outcalltrunk']['primary'] = array('outcallid' => '','trunkfeaturesid' => '');

$array['phonebook'] = array();
$array['phonebook']['table'] = 'phonebook';
$array['phonebook']['primary'] = array('id' => '');

$array['phonebookaddress'] = array();
$array['phonebookaddress']['table'] = 'phonebookaddress';
$array['phonebookaddress']['primary'] = array('id' => '');

$array['phonebooknumber'] = array();
$array['phonebooknumber']['table'] = 'phonebooknumber';
$array['phonebooknumber']['primary'] = array('id' => '');

$array['phonefunckey'] = array();
$array['phonefunckey']['table'] = 'phonefunckey';
$array['phonefunckey']['primary'] = array('iduserfeatures' => '','fknum' => '');

$array['queue'] = array();
$array['queue']['table'] = 'queue';
$array['queue']['primary'] = array('name' => '');

$array['queuefeatures'] = array();
$array['queuefeatures']['table'] = 'queuefeatures';
$array['queuefeatures']['primary'] = array('id' => '');

$array['queuemember'] = array();
$array['queuemember']['table'] = 'queuemember';
$array['queuemember']['primary'] = array('queue_name' => '','interface' => '');

$array['rightcall'] = array();
$array['rightcall']['table'] = 'rightcall';
$array['rightcall']['primary'] = array('id' => '');

$array['rightcallexten'] = array();
$array['rightcallexten']['table'] = 'rightcallexten';
$array['rightcallexten']['primary'] = array('id' => '');

$array['rightcallmember'] = array();
$array['rightcallmember']['table'] = 'rightcallmember';
$array['rightcallmember']['primary'] = array('id' => '');

$array['schedule'] = array();
$array['schedule']['table'] = 'schedule';
$array['schedule']['primary'] = array('id' => '');

$array['serverfeatures'] = array();
$array['serverfeatures']['table'] = 'serverfeatures';
$array['serverfeatures']['primary'] = array('id' => '');

$array['sip'] = array();
$array['sip']['table'] = 'generalsip';
$array['sip']['primary'] = array('id' => '');

$array['trunkcustom'] = array();
$array['trunkcustom']['table'] = 'usercustom';
$array['trunkcustom']['primary'] = array('id' => '');

$array['trunkfeatures'] = array();
$array['trunkfeatures']['table'] = 'trunkfeatures';
$array['trunkfeatures']['primary'] = array('id' => '');

$array['trunkiax'] = array();
$array['trunkiax']['table'] = 'useriax';
$array['trunkiax']['primary'] = array('id' => '');

$array['trunksip'] = array();
$array['trunksip']['table'] = 'usersip';
$array['trunksip']['primary'] = array('id' => '');

$array['usercustom'] = array();
$array['usercustom']['table'] = 'usercustom';
$array['usercustom']['primary'] = array('id' => '');

$array['userfeatures'] = array();
$array['userfeatures']['table'] = 'userfeatures';
$array['userfeatures']['primary'] = array('id' => '');

$array['useriax'] = array();
$array['useriax']['table'] = 'useriax';
$array['useriax']['primary'] = array('id' => '');

$array['usersip'] = array();
$array['usersip']['table'] = 'usersip';
$array['usersip']['primary'] = array('id' => '');

$array['uservoicemail'] = array();
$array['uservoicemail']['table'] = 'uservoicemail';
$array['uservoicemail']['primary'] = array('uniqueid' => '');

$array['voicemail'] = array();
$array['voicemail']['table'] = 'generalvoicemail';
$array['voicemail']['primary'] = array('id' => '');

$array['zonemessages'] = array();
$array['zonemessages']['table'] = 'generalvoicemail';
$array['zonemessages']['primary'] = array('id' => '');

?>
