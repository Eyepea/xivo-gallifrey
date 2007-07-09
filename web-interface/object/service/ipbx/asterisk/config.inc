<?php

define('XIVO_SRE_IPBX_AST_CHAN_SIP','SIP');
define('XIVO_SRE_IPBX_AST_CHAN_IAX','IAX2');
define('XIVO_SRE_IPBX_AST_CHAN_LOCAL','local');
define('XIVO_SRE_IPBX_AST_CHAN_AGENT','Agent');

define('XIVO_SRE_IPBX_AST_PROTO_SIP','sip');
define('XIVO_SRE_IPBX_AST_PROTO_IAX','iax');

define('XIVO_SRE_IPBX_AST_CODEC_ALAW','alaw');
define('XIVO_SRE_IPBX_AST_CODEC_G729','g729');
define('XIVO_SRE_IPBX_AST_CODEC_GSM','gsm');
define('XIVO_SRE_IPBX_AST_CODEC_ULAW','ulaw');

define('XIVO_SRE_IPBX_AST_SNDFORMAT_WAV','wav');
define('XIVO_SRE_IPBX_AST_SNDFORMAT_WAV49','wav49');
define('XIVO_SRE_IPBX_AST_SNDFORMAT_GSM','gsm');

$array = array();

$array['section'] = array();

$array['section']['sip'] = array();
$array['section']['sip']['disabled'] = true;

$array['section']['iax'] = array();
$array['section']['iax']['disabled'] = true;

$array['section']['group'] = array();
$array['section']['group']['disabled'] = true;

$array['section']['meetme'] = array();
$array['section']['meetme']['disabled'] = true;

$array['section']['queue'] = array();
$array['section']['queue']['disabled'] = true;

$array['section']['voicemail'] = array();
$array['section']['voicemail']['disabled'] = true;

$array['section']['agent'] = array();
$array['section']['agent']['disabled'] = true;

$array['module'] = array();

$array['module']['extensions'] = array();
$array['module']['extensions']['datastorage'] = true;

$array['module']['extenumbers'] = array();
$array['module']['extenumbers']['datastorage'] = true;

$array['module']['extenfeatures'] = array();
$array['module']['extenfeatures']['disabled'] = true;
$array['module']['extenfeatures']['datastorage'] = true;

$array['module']['configfiles'] = array();
$array['module']['configfiles']['disabled'] = true;

$array['module']['sounds'] = array();
$array['module']['sounds']['disabled'] = true;

$array['module']['ami'] = array();
$array['module']['ami']['disabled'] = true;

$array['module']['generalsip'] = array();
$array['module']['generalsip']['datastorage'] = true;
$array['module']['generalsip']['section'] = 'sip';

$array['module']['usersip'] = array();
$array['module']['usersip']['datastorage'] = true;
$array['module']['usersip']['section'] = 'sip';

$array['module']['trunksip'] = array();
$array['module']['trunksip']['datastorage'] = true;
$array['module']['trunksip']['section'] = 'sip';

$array['module']['generaliax'] = array();
$array['module']['generaliax']['datastorage'] = true;
$array['module']['generaliax']['section'] = 'iax';

$array['module']['useriax'] = array();
$array['module']['useriax']['datastorage'] = true;
$array['module']['useriax']['section'] = 'iax';

$array['module']['trunkiax'] = array();
$array['module']['trunkiax']['datastorage'] = true;
$array['module']['trunkiax']['section'] = 'iax';

$array['module']['userfeatures'] = array();
$array['module']['userfeatures']['datastorage'] = true;

$array['module']['autoprov'] = array();
$array['module']['autoprov']['datastorage'] = true;

$array['module']['trunkfeatures'] = array();
$array['module']['trunkfeatures']['datastorage'] = true;

$array['module']['usergroup'] = array();
$array['module']['usergroup']['datastorage'] = true;
$array['module']['usergroup']['section'] = 'group';

$array['module']['groupfeatures'] = array();
$array['module']['groupfeatures']['datastorage'] = true;
$array['module']['groupfeatures']['section'] = 'group';

$array['module']['meetme'] = array();
$array['module']['meetme']['datastorage'] = true;
$array['module']['meetme']['section'] = 'meetme';

$array['module']['meetmefeatures'] = array();
$array['module']['meetmefeatures']['datastorage'] = true;
$array['module']['meetmefeatures']['section'] = 'meetme';

$array['module']['generalqueue'] = array();
$array['module']['generalqueue']['datastorage'] = true;
$array['module']['generalqueue']['section'] = 'queue';

$array['module']['queue'] = array();
$array['module']['queue']['datastorage'] = true;
$array['module']['queue']['section'] = 'queue';

$array['module']['queuefeatures'] = array();
$array['module']['queuefeatures']['datastorage'] = true;
$array['module']['queuefeatures']['section'] = 'queue';

$array['module']['queuemember'] = array();
$array['module']['queuemember']['datastorage'] = true;
$array['module']['queuemember']['section'] = 'queue';

$array['module']['generalvoicemail'] = array();
$array['module']['generalvoicemail']['datastorage'] = true;
$array['module']['generalvoicemail']['section'] = 'voicemail';

$array['module']['uservoicemail'] = array();
$array['module']['uservoicemail']['datastorage'] = true;
$array['module']['uservoicemail']['section'] = 'voicemail';

$array['module']['musiconhold'] = array();
$array['module']['musiconhold']['datastorage'] = true;

$array['module']['didfeatures'] = array();
$array['module']['didfeatures']['datastorage'] = true;

$array['module']['generalagent'] = array();
$array['module']['generalagent']['datastorage'] = true;
$array['module']['generalagent']['section'] = 'agent';

$array['module']['agent'] = array();
$array['module']['agent']['datastorage'] = true;
$array['module']['agent']['section'] = 'agent';

$array['module']['agentfeatures'] = array();
$array['module']['agentfeatures']['datastorage'] = true;
$array['module']['agentfeatures']['section'] = 'agent';

$array['module']['agentgroup'] = array();
$array['module']['agentgroup']['datastorage'] = true;
$array['module']['agentgroup']['section'] = 'agent';

$array['module']['cdr'] = array();

$array['protocol'] = array();
$array['protocol']['sip'] = array('name' => XIVO_SRE_IPBX_AST_PROTO_SIP,'module' => 'usersip'); 
$array['protocol']['iax'] = array('name' => XIVO_SRE_IPBX_AST_PROTO_IAX,'module' => 'useriax'); 

$array['channel'] = array();
$array['channel']['sip'] = XIVO_SRE_IPBX_AST_CHAN_SIP;
$array['channel']['iax'] = XIVO_SRE_IPBX_AST_CHAN_IAX;
$array['channel']['local'] = XIVO_SRE_IPBX_AST_CHAN_LOCAL;
$array['channel']['agent'] = XIVO_SRE_IPBX_AST_CHAN_AGENT;

?>
