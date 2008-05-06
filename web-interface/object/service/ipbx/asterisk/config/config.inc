<?php

define('XIVO_SRE_IPBX_AST_CHAN_SIP','SIP');
define('XIVO_SRE_IPBX_AST_CHAN_IAX','IAX2');
define('XIVO_SRE_IPBX_AST_CHAN_LOCAL','Local');
define('XIVO_SRE_IPBX_AST_CHAN_AGENT','Agent');
define('XIVO_SRE_IPBX_AST_CHAN_ZAP','Zap');
define('XIVO_SRE_IPBX_AST_CHAN_CAPI','CAPI');
define('XIVO_SRE_IPBX_AST_CHAN_MISDN','mISDN');
define('XIVO_SRE_IPBX_AST_CHAN_MGCP','MGCP');
define('XIVO_SRE_IPBX_AST_CHAN_SCCP','SCCP');
define('XIVO_SRE_IPBX_AST_CHAN_H323','H323');
define('XIVO_SRE_IPBX_AST_CHAN_UNKNOWN','**Unknown**');

define('XIVO_SRE_IPBX_AST_PROTO_SIP','sip');
define('XIVO_SRE_IPBX_AST_PROTO_IAX','iax');
define('XIVO_SRE_IPBX_AST_PROTO_CUSTOM','custom');

define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_G723','g723');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_GSM','gsm');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_ULAW','ulaw');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_ALAW','alaw');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_G726','g726');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_ADPCM','adpcm');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_SLIN','slin');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_LPC10','lpc10');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_G729','g729');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_SPEEX','speex');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_ILBC','ilbc');

define('XIVO_SRE_IPBX_AST_CODEC_IMAGE_JPEG','jpeg');
define('XIVO_SRE_IPBX_AST_CODEC_IMAGE_PNG','png');

define('XIVO_SRE_IPBX_AST_CODEC_VIDEO_H261','h261');
define('XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263','h263');
define('XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263P','h263p');

define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV','wav');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49','wav49');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM','gsm');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG','ogg');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW','ulaw');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW','alaw');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU','au');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX','vox');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN','slin');

define('XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_DEFAULT',0);
define('XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_OMIT',1);
define('XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_BILLING',2);
define('XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_DOCUMENTATION',3);

define('XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DEFAULT','default');
define('XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_OMIT','omit');
define('XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_BILLING','billing');
define('XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DOCUMENTATION','documentation');

define('XIVO_SRE_IPBX_AST_CDR_DISPOSITION_NOANSWER','NO ANSWER');
define('XIVO_SRE_IPBX_AST_CDR_DISPOSITION_FAILED','FAILED');
define('XIVO_SRE_IPBX_AST_CDR_DISPOSITION_BUSY','BUSY');
define('XIVO_SRE_IPBX_AST_CDR_DISPOSITION_ANSWERED','ANSWERED');
define('XIVO_SRE_IPBX_AST_CDR_DISPOSITION_UNKNOWN','UNKNOWN');

define('XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN','a-zA-Z0-9_-');
define('XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH','{1,50}');

define('XIVO_SRE_IPBX_AST_USER_MACRO','incoming_user');
define('XIVO_SRE_IPBX_AST_GROUP_MACRO','incoming_group');
define('XIVO_SRE_IPBX_AST_QUEUE_MACRO','incoming_queue');
define('XIVO_SRE_IPBX_AST_MEETME_MACRO','incoming_meetme');
define('XIVO_SRE_IPBX_AST_INCALL_MACRO','incoming_did');
define('XIVO_SRE_IPBX_AST_OUTCALL_MACRO','outgoing_user');
define('XIVO_SRE_IPBX_AST_HANDYNUMBERS_MACRO','handynumbers');
define('XIVO_SRE_IPBX_AST_HANDYNUMBERS_CONTEXT','handynumbers');

define('XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTIFICATION','authentification');
define('XIVO_SRE_IPBX_AST_CONTEXT_EXTRAFEATURES','extra-features');
define('XIVO_SRE_IPBX_AST_CONTEXT_FEATURES','features');
define('XIVO_SRE_IPBX_AST_CONTEXT_GENERAL','general');
define('XIVO_SRE_IPBX_AST_CONTEXT_GLOBAL','global');
define('XIVO_SRE_IPBX_AST_CONTEXT_GLOBALS','globals');
define('XIVO_SRE_IPBX_AST_CONTEXT_HINT','hint');
define('XIVO_SRE_IPBX_AST_CONTEXT_HINTS','hints');
define('XIVO_SRE_IPBX_AST_CONTEXT_INITCONFIG','initconfig');
define('XIVO_SRE_IPBX_AST_CONTEXT_PARKEDCALLS','parkedcalls');
define('XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES','zonemessages');

define('XIVO_SRE_IPBX_AST_NBBYPAGE',20);
define('XIVO_SRE_IPBX_AST_CDR_NBBYPAGE',30);

$array = array();

$array['appcustom'] = array();

$array['appcustom']['webservices'] = array();
$array['appcustom']['webservices']['phonebook'] = true;

$array['realstatic'] = array();

$array['realstatic']['agents'] = array();
$array['realstatic']['agents']['datastorage'] = true;

$array['realstatic']['features'] = array();
$array['realstatic']['features']['datastorage'] = true;

$array['realstatic']['generalqueue'] = array();
$array['realstatic']['generalqueue']['datastorage'] = true;

$array['realstatic']['meetme'] = array();
$array['realstatic']['meetme']['datastorage'] = true;

$array['realstatic']['staticiax'] = array();
$array['realstatic']['staticiax']['datastorage'] = true;

$array['realstatic']['staticsip'] = array();
$array['realstatic']['staticsip']['datastorage'] = true;

$array['realstatic']['staticvoicemail'] = array();
$array['realstatic']['staticvoicemail']['datastorage'] = true;

$array['module'] = array();

$array['module']['accessfeatures'] = array();
$array['module']['accessfeatures']['datastorage'] = true;

$array['module']['agent'] = array();
$array['module']['agent']['datastorage'] = true;

$array['module']['agentfeatures'] = array();
$array['module']['agentfeatures']['datastorage'] = true;

$array['module']['agentgroup'] = array();
$array['module']['agentgroup']['datastorage'] = true;

$array['module']['ami'] = array();
$array['module']['ami']['datastorage'] = false;

$array['module']['autoprov'] = array();
$array['module']['autoprov']['datastorage'] = true;

$array['module']['callfilter'] = array();
$array['module']['callfilter']['datastorage'] = true;

$array['module']['callfiltermember'] = array();
$array['module']['callfiltermember']['datastorage'] = true;

$array['module']['configfiles'] = array();
$array['module']['configfiles']['datastorage'] = false;

$array['module']['cdr'] = array();
$array['module']['cdr']['datastorage'] = array();
$array['module']['cdr']['datastorage']['custom'] = true;

$array['module']['context'] = array();
$array['module']['context']['datastorage'] = true;

$array['module']['contextmember'] = array();
$array['module']['contextmember']['datastorage'] = true;

$array['module']['contextnumbers'] = array();
$array['module']['contextnumbers']['datastorage'] = true;

$array['module']['contextnummember'] = array();
$array['module']['contextnummember']['datastorage'] = true;

$array['module']['dialstatus'] = array();
$array['module']['dialstatus']['datastorage'] = true;

$array['module']['extensions'] = array();
$array['module']['extensions']['datastorage'] = true;

$array['module']['extenumbers'] = array();
$array['module']['extenumbers']['datastorage'] = true;

$array['module']['handynumbers'] = array();
$array['module']['handynumbers']['datastorage'] = true;

$array['module']['generalqueue'] = array();
$array['module']['generalqueue']['datastorage'] = true;

$array['module']['groupfeatures'] = array();
$array['module']['groupfeatures']['datastorage'] = true;

$array['module']['incall'] = array();
$array['module']['incall']['datastorage'] = true;

$array['module']['ldapfilter'] = array();
$array['module']['ldapfilter']['datastorage'] = true;

$array['module']['meetmefeatures'] = array();
$array['module']['meetmefeatures']['datastorage'] = true;

$array['module']['musiconhold'] = array();
$array['module']['musiconhold']['datastorage'] = true;

$array['module']['outcall'] = array();
$array['module']['outcall']['datastorage'] = true;

$array['module']['outcalltrunk'] = array();
$array['module']['outcalltrunk']['datastorage'] = true;

$array['module']['phonebook'] = array();
$array['module']['phonebook']['datastorage'] = true;

$array['module']['phonebookaddress'] = array();
$array['module']['phonebookaddress']['datastorage'] = true;

$array['module']['phonebooknumber'] = array();
$array['module']['phonebooknumber']['datastorage'] = true;

$array['module']['phonefunckey'] = array();
$array['module']['phonefunckey']['datastorage'] = true;

$array['module']['queue'] = array();
$array['module']['queue']['datastorage'] = true;

$array['module']['queuefeatures'] = array();
$array['module']['queuefeatures']['datastorage'] = true;

$array['module']['queuemember'] = array();
$array['module']['queuemember']['datastorage'] = true;

$array['module']['rightcall'] = array();
$array['module']['rightcall']['datastorage'] = true;

$array['module']['rightcallexten'] = array();
$array['module']['rightcallexten']['datastorage'] = true;

$array['module']['rightcallmember'] = array();
$array['module']['rightcallmember']['datastorage'] = true;

$array['module']['schedule'] = array();
$array['module']['schedule']['datastorage'] = true;

$array['module']['serverfeatures'] = array();
$array['module']['serverfeatures']['datastorage'] = true;

$array['module']['sounds'] = array();
$array['module']['sounds']['datastorage'] = false;

$array['module']['trunkcustom'] = array();
$array['module']['trunkcustom']['datastorage'] = true;

$array['module']['trunkfeatures'] = array();
$array['module']['trunkfeatures']['datastorage'] = true;

$array['module']['trunkiax'] = array();
$array['module']['trunkiax']['datastorage'] = true;

$array['module']['trunksip'] = array();
$array['module']['trunksip']['datastorage'] = true;

$array['module']['usercustom'] = array();
$array['module']['usercustom']['datastorage'] = true;

$array['module']['userfeatures'] = array();
$array['module']['userfeatures']['datastorage'] = true;

$array['module']['useriax'] = array();
$array['module']['useriax']['datastorage'] = true;

$array['module']['usersip'] = array();
$array['module']['usersip']['datastorage'] = true;

$array['module']['voicemail'] = array();
$array['module']['voicemail']['datastorage'] = true;

$array['module']['voicemailfeatures'] = array();
$array['module']['voicemailfeatures']['datastorage'] = true;

$array['protocol'] = array();
$array['protocol'][XIVO_SRE_IPBX_AST_PROTO_SIP] = array('module' => 'usersip'); 
$array['protocol'][XIVO_SRE_IPBX_AST_PROTO_IAX] = array('module' => 'useriax'); 
$array['protocol'][XIVO_SRE_IPBX_AST_PROTO_CUSTOM] = array('module' => 'usercustom'); 

$array['channel'] = array();
$array['channel']['sip'] = XIVO_SRE_IPBX_AST_CHAN_SIP;
$array['channel']['iax'] = XIVO_SRE_IPBX_AST_CHAN_IAX;
$array['channel']['local'] = XIVO_SRE_IPBX_AST_CHAN_LOCAL;
$array['channel']['agent'] = XIVO_SRE_IPBX_AST_CHAN_AGENT;

$array['format'] = array();
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV] = array(
							'type' => 'audio',
							'extension' => 'wav');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49] = array(
							'type' => 'audio',
							'extension' => 'WAV');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM] = array(
							'type' => 'audio',
							'extension' => 'gsm');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG] = array(
							'type' => 'audio',
							'extension' => 'ogg');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW] = array(
							'type' => 'audio',
							'extension' => array('ul','ulaw','um','pcm'));
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW] = array(
							'type' => 'audio',
							'extension' => array('al','alaw'));
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU] = array(
							'type' => 'audio',
							'extension' => 'au');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX] = array(
							'type' => 'audio',
							'extension' => 'vox');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN] = array(
							'type' => 'audio',
							'extension' => array('sln','raw'));
?>
