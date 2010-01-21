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

define('XIVO_SRE_IPBX_AST_CHAN_SIP','SIP');
define('XIVO_SRE_IPBX_AST_CHAN_IAX','IAX2');
define('XIVO_SRE_IPBX_AST_CHAN_LOCAL','Local');
define('XIVO_SRE_IPBX_AST_CHAN_AGENT','Agent');
define('XIVO_SRE_IPBX_AST_CHAN_ZAP','Zap');
define('XIVO_SRE_IPBX_AST_CHAN_DAHDI','DAHDI');
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
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_ADPCM','adpcm');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_SLIN','slin');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_LPC10','lpc10');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_G729','g729');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_SPEEX','speex');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_ILBC','ilbc');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_G726','g726');
define('XIVO_SRE_IPBX_AST_CODEC_AUDIO_G722','g722');

define('XIVO_SRE_IPBX_AST_CODEC_IMAGE_JPEG','jpeg');
define('XIVO_SRE_IPBX_AST_CODEC_IMAGE_PNG','png');

define('XIVO_SRE_IPBX_AST_CODEC_VIDEO_H261','h261');
define('XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263','h263');
define('XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263P','h263p');
define('XIVO_SRE_IPBX_AST_CODEC_VIDEO_H264','h264');

define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV','wav');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49','wav49');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM','gsm');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG','ogg');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW','ulaw');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW','alaw');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU','au');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX','vox');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN','sln');
define('XIVO_SRE_IPBX_AST_FORMAT_AUDIO_MP3','mp3');

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

define('XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_PATTERN','a-zA-Z0-9_\-');
define('XIVO_SRE_IPBX_AST_REGEXP_AUDIO_FILE_LENGTH','{1,50}');
define('XIVO_SRE_IPBX_AST_REGEXP_AIDIO_FILE_EXTENSION','wav|WAV|wav49|ogg|ul|ulaw|um|pcm|al|alaw|au|vox|sln|raw');

define('XIVO_SRE_IPBX_AST_REGEXP_CONTEXT','(?!(?i:macro|voicemenu|xivo)-)[a-zA-Z0-9_-]{1,39}');
define('XIVO_SRE_IPBX_AST_REGEXP_CALLERID','(?:"(?:[^\x00-\x08\x0A-\x1F\x22\x5C]'.
					   '|\\\[\x00-\x09\x0B-\x0C\x0E-\x7F])+"'.
					   '|[a-zA-Z0-9\-\.\!%\*_\+`\'\~]+)'.
					   '(?: ?\<[0-9\*#]{1,40}\>)?');
define('XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN','(?:\$\{[\(\)a-zA-Z0-9_\-\$\{\}\:]+\})');
define('XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_EXTEN','(?:[0-9\*#]{1,40}'.
						 '|(?:[0-9\*#]*'.
						 XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
						 '[0-9\*#]*)+)');
define('XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_EXTEN_EXTENDED','(?:[a-zA-Z0-9_\*#]{1,40}'.
							  '|(?:[a-zA-Z0-9_\*#]*'.
							  XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
							  '[a-zA-Z0-9_\*#]*)+)');
define('XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_CONTEXT','(?:'.XIVO_SRE_IPBX_AST_REGEXP_CONTEXT.
						   '|(?:[a-zA-Z0-9_-]*'.
						   XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
						   '[a-zA-Z0-9_-]*)+)');
define('XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_ATCONTEXT','(?:@'.
						   XIVO_SRE_IPBX_AST_REGEXP_CONTEXT.
						   '|@?(?:[a-zA-Z0-9_-]*'.
						   XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
						   '[a-zA-Z0-9_-]*)+)');
define('XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN_AUDIOFILE','(?:[a-zA-Z0-9%_-]{1,50}'.
						     '\.(?:'.XIVO_SRE_IPBX_AST_REGEXP_AIDIO_FILE_EXTENSION.')'.
						     '|(?:(?:[a-zA-Z0-9%_-]*'.
						     XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.
						     '[a-zA-Z0-9%_-]*)+'.
						     '(?:\.(?:(?:'.XIVO_SRE_IPBX_AST_REGEXP_AIDIO_FILE_EXTENSION.')'.
						     '|'.XIVO_SRE_IPBX_AST_REGEXP_DIALPLAN.'))?))');

define('XIVO_SRE_IPBX_AST_USER_MACRO','user');
define('XIVO_SRE_IPBX_AST_GROUP_MACRO','group');
define('XIVO_SRE_IPBX_AST_QUEUE_MACRO','queue');
define('XIVO_SRE_IPBX_AST_MEETME_MACRO','meetme');
define('XIVO_SRE_IPBX_AST_INCALL_MACRO','did');
define('XIVO_SRE_IPBX_AST_OUTCALL_MACRO','outcall');
define('XIVO_SRE_IPBX_AST_HANDYNUMBERS_MACRO','handynumbers');
define('XIVO_SRE_IPBX_AST_VOICEMENU_MACRO','voicemenu');

define('XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION','authentication');
define('XIVO_SRE_IPBX_AST_CONTEXT_FEATURES','xivo-features');
define('XIVO_SRE_IPBX_AST_CONTEXT_GENERAL','general');
define('XIVO_SRE_IPBX_AST_CONTEXT_GLOBAL','global');
define('XIVO_SRE_IPBX_AST_CONTEXT_GLOBALS','globals');
define('XIVO_SRE_IPBX_AST_CONTEXT_HANDYNUMBERS','xivo-handynumbers');
define('XIVO_SRE_IPBX_AST_CONTEXT_HINT','hint');
define('XIVO_SRE_IPBX_AST_CONTEXT_HINTS','hints');
define('XIVO_SRE_IPBX_AST_CONTEXT_PARKEDCALLS','parkedcalls');
define('XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES','zonemessages');

define('XIVO_SRE_IPBX_AST_AGENT_GROUP_DEFAULT',1);
define('XIVO_SRE_IPBX_AST_AGENT_VAR_METRIC_RANGE',1000000);

define('XIVO_SRE_IPBX_AST_MOH_DEFAULT','default');

define('XIVO_SRE_IPBX_AST_PRECONTEXT_VOICEMENU','voicemenu-');

define('XIVO_SRE_IPBX_AST_NBBYPAGE',20);
define('XIVO_SRE_IPBX_AST_CDR_NBBYPAGE',30);

$array = array();

$array['appcustom'] = array();

$array['appcustom']['webservices'] = array();
$array['appcustom']['webservices']['phonebook'] = true;

$array['realstatic'] = array();

$array['realstatic']['features'] = array();
$array['realstatic']['features']['datastorage'] = true;

$array['realstatic']['staticagent'] = array();
$array['realstatic']['staticagent']['datastorage'] = true;

$array['realstatic']['staticiax'] = array();
$array['realstatic']['staticiax']['datastorage'] = true;

$array['realstatic']['staticmeetme'] = array();
$array['realstatic']['staticmeetme']['datastorage'] = true;

$array['realstatic']['staticqueue'] = array();
$array['realstatic']['staticqueue']['datastorage'] = true;

$array['realstatic']['staticsip'] = array();
$array['realstatic']['staticsip']['datastorage'] = true;

$array['realstatic']['staticvoicemail'] = array();
$array['realstatic']['staticvoicemail']['datastorage'] = true;

$array['module'] = array();

$array['module']['accessfeatures'] = array();
$array['module']['accessfeatures']['datastorage'] = true;

$array['module']['agentfeatures'] = array();
$array['module']['agentfeatures']['datastorage'] = true;

$array['module']['agentgroup'] = array();
$array['module']['agentgroup']['datastorage'] = true;

$array['module']['ami'] = array();
$array['module']['ami']['datastorage'] = false;

$array['module']['autoprov'] = array();
$array['module']['autoprov']['datastorage'] = true;

$array['module']['backupfiles'] = array();
$array['module']['backupfiles']['datastorage'] = false;

$array['module']['callerid'] = array();
$array['module']['callerid']['datastorage'] = true;

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

$array['module']['contextinclude'] = array();
$array['module']['contextinclude']['datastorage'] = true;

$array['module']['contextmember'] = array();
$array['module']['contextmember']['datastorage'] = true;

$array['module']['contextnumbers'] = array();
$array['module']['contextnumbers']['datastorage'] = true;

$array['module']['contextnummember'] = array();
$array['module']['contextnummember']['datastorage'] = true;

$array['module']['ctimain'] = array();
$array['module']['ctimain']['datastorage'] = true;

$array['module']['ctipresences'] = array();
$array['module']['ctipresences']['datastorage'] = true;

$array['module']['ctistatus'] = array();
$array['module']['ctistatus']['datastorage'] = true;

$array['module']['dialaction'] = array();
$array['module']['dialaction']['datastorage'] = true;

$array['module']['extensions'] = array();
$array['module']['extensions']['datastorage'] = true;

$array['module']['extenumbers'] = array();
$array['module']['extenumbers']['datastorage'] = true;

$array['module']['handynumbers'] = array();
$array['module']['handynumbers']['datastorage'] = true;

$array['module']['groupfeatures'] = array();
$array['module']['groupfeatures']['datastorage'] = true;

$array['module']['incall'] = array();
$array['module']['incall']['datastorage'] = true;

$array['module']['ldapfilter'] = array();
$array['module']['ldapfilter']['datastorage'] = true;

$array['module']['logfiles'] = array();
$array['module']['logfiles']['datastorage'] = false;

$array['module']['meetmefeatures'] = array();
$array['module']['meetmefeatures']['datastorage'] = true;

$array['module']['meetmeguest'] = array();
$array['module']['meetmeguest']['datastorage'] = true;

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

$array['module']['ctiphonehints'] = array();
$array['module']['ctiphonehints']['datastorage'] = true;

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

$array['module']['voicemenu'] = array();
$array['module']['voicemenu']['datastorage'] = true;

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
				'type'		=> 'audio',
				'extension'	=> 'wav',
				'mime'		=> array('audio/wav',
							 'audio/x-wav',
							 'application/octet-stream'));
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_WAV49] = array(
				'type'		=> 'audio',
				'extension'	=> array('WAV','wav49'),
				'mime'		=> array('audio/wav',
							 'audio/x-wav',
							 'application/octet-stream'));
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_GSM] = array(
				'type'		=> 'audio',
				'extension'	=> 'gsm',
				'mime'		=> array('audio/gsm',
							 'audio/x-gsm',
							 'application/octet-stream'));
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_OGG] = array(
				'type'		=> 'audio',
				'extension'	=> 'ogg',
				'mime'		=> array('audio/ogg',
							 'audio/x-ogg',
							 'application/octet-stream',
							 'application/ogg',
							 'application/x-ogg'));
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ULAW] = array(
				'type'		=> 'audio',
				'extension'	=> array('ul','ulaw','um','pcm'),
				'mime'		=> 'application/octet-stream');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_ALAW] = array(
				'type'		=> 'audio',
				'extension'	=> array('al','alaw'),
				'mime'		=> 'application/octet-stream');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_AU] = array(
				'type'		=> 'audio',
				'extension'	=> 'au',
				'mime'		=> 'audio/basic');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_VOX] = array(
				'type'		=> 'audio',
				'extension'	=> 'vox',
				'mime'		=> 'application/octet-stream');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_SLIN] = array(
				'type'		=> 'audio',
				'extension'	=> array('sln','raw'),
				'mime'		=> 'application/octet-stream');
$array['format'][XIVO_SRE_IPBX_AST_FORMAT_AUDIO_MP3] = array(
				'type'		=> 'audio',
				'extension'	=> 'mp3',
				'mime'		=> array('audio/mp3',
							 'audio/x-mp3',
							 'audio/mpeg',
							 'audio/x-mpeg',
							 'audio/mpeg3',
							 'audio/x-mpeg3'));

?>
