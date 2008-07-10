<?php

$array = array();

$array['registered'] = 'SIP/Registry';

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['host'] = array();
$array['element']['host']['value'] = array('dynamic');
$array['element']['host']['default'] = 'dynamic';
$array['element']['host']['setdef'] = true;

$array['element']['dtmfmode'] = array();
$array['element']['dtmfmode']['value'] = array('rfc2833','inband','info','auto');
$array['element']['dtmfmode']['default'] = 'info';
$array['element']['dtmfmode']['setdef'] = true;

$array['element']['canreinvite'] = array();
$array['element']['canreinvite']['value'] = array('no','yes');
$array['element']['canreinvite']['default'] = 'no';
$array['element']['canreinvite']['set'] = true;

$array['element']['amaflags'] = array();
$array['element']['amaflags']['value'] = array(
					XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DEFAULT,
					XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_OMIT,
					XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_BILLING,
					XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DOCUMENTATION);
$array['element']['amaflags']['default'] = XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DOCUMENTATION;
$array['element']['amaflags']['setdef'] = true;

$array['element']['nat'] = array();
$array['element']['nat']['value'] = array('no','yes');
$array['element']['nat']['default'] = 'no';
$array['element']['nat']['set'] = true;

$array['element']['qualify'] = array();
$array['element']['qualify']['value'] = array('no','yes');
$array['element']['qualify']['default'] = 'no';
$array['element']['qualify']['set'] = true;

$array['element']['disallow'] = array();
$array['element']['disallow']['value'] = array('all');
$array['element']['disallow']['default'] = 'all';
$array['element']['disallow']['null'] = true;

$array['element']['allow'] = array();
$array['element']['allow']['value'] = array(
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G723,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_GSM,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ULAW,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ALAW,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ADPCM,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_SLIN,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_LPC10,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G729,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_SPEEX,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ILBC,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G726,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G722,
					XIVO_SRE_IPBX_AST_CODEC_IMAGE_JPEG,
					XIVO_SRE_IPBX_AST_CODEC_IMAGE_PNG,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H261,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263P,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H264);
$array['element']['allow']['default'] = '';
$array['element']['allow']['null'] = true;

$array['element']['host-static'] = array();
$array['element']['host-static']['default'] = '';
$array['element']['host-static']['unset'] = true;

$array['element']['host-dynamic'] = array();
$array['element']['host-dynamic']['value'] = array('dynamic','static');
$array['element']['host-dynamic']['unset'] = true;

$array['element']['type'] = array();
$array['element']['type']['default'] = 'friend';
$array['element']['type']['forcedef'] = true;

$array['element']['regexten'] = array();
$array['element']['regexten']['default'] = '';
$array['element']['regexten']['null'] = true;

$array['element']['regserver'] = array();
$array['element']['regserver']['default'] = '';
$array['element']['regserver']['null'] = true;

$array['element']['accountcode'] = array();
$array['element']['accountcode']['default'] = '';

$array['element']['setvar'] = array();
$array['element']['setvar']['default'] = '';

$array['element']['callgroup'] = array();
$array['element']['callgroup']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['mailbox'] = array();
$array['element']['mailbox']['default'] = '';
$array['element']['mailbox']['null'] = true;

$array['element']['call-limit'] = array();
$array['element']['call-limit']['default'] = 0;
$array['element']['call-limit']['setdef'] = true;

$array['element']['protocol'] = array();
$array['element']['protocol']['default'] = XIVO_SRE_IPBX_AST_PROTO_SIP;
$array['element']['protocol']['forcedef'] = true;

$array['element']['category'] = array();
$array['element']['category']['default'] = 'user';
$array['element']['category']['forcedef'] = true;


$array['filter'] = array();
$array['filter']['callerid'] = array('set' => false,'chk' => 2,'maxlen' => 80,'notag' => false);
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['secret'] = array('set' => false,'chk' => 2,'minlen' => 1,'maxlen' => 80);
$array['filter']['host'] = array('set' => false,'chk' => 2,
				 'key' => array('dynamic','static'),
				 'strict' => false,
				 'callback' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['dtmfmode'] = array('set' => false,'chk' => 2,'key' => array('rfc2833','inband','info','auto'));
$array['filter']['canreinvite'] = array('bool' => true);
$array['filter']['amaflags'] = array('set' => false,'chk' => 2,'key' => array(
						XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DEFAULT,
						XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_OMIT,
						XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_BILLING,
						XIVO_SRE_IPBX_AST_CDR_AMAFLAGS_META_DOCUMENTATION));
$array['filter']['accountcode'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_-]+$/i','maxlen' => 20);
$array['filter']['callgroup'] = array('set' => false,'chk' => 2,'regexp' => '/^(?:[0-9]+[,]?)*$/','maxlen' => 180);
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context',
				    'callbackparam' => array(array(
				    			XIVO_SRE_IPBX_AST_CONTEXT_AUTHENTICATION,
							XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['nat'] = array('bool' => true);
$array['filter']['qualify'] = array('bool' => true);
$array['filter']['disallow'] = array('set' => false,'chk' => 2,'key' => array('all'));
$array['filter']['allow'] = array('set' => false,'chk' => 2,'uvalue' => array(
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G723,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_GSM,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ULAW,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ALAW,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ADPCM,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_SLIN,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_LPC10,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G729,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_SPEEX,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ILBC,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G726,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G722,
						XIVO_SRE_IPBX_AST_CODEC_IMAGE_JPEG,
						XIVO_SRE_IPBX_AST_CODEC_IMAGE_PNG,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H261,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263P,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H264));
$array['filter']['type'] = array('set' => false,'chk' => 0);
$array['filter']['regexten'] = array('set' => false,'chk' => 0);
$array['filter']['regserver'] = array('set' => false,'chk' => 0);
$array['filter']['setvar'] = array('set'	=> false,
				   'chk'	=> 2,
				   'minlen'	=> 2,
				   'maxlen'	=> 100,
				   'regexp'	=> '/^[a-zA-Z0-9_\(\)]+=[^\|,]*(?:\|[a-zA-Z0-9_\(\)]+=[^\|,]*)*$/');
$array['filter']['mailbox'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9#\*]{1,40}(?:@[a-z0-9@_\-]{1,39})?$/','maxlen' => 80);
$array['filter']['call-limit'] = array('set' => false,'chk' => 0);
$array['filter']['protocol'] = array('set' => false);
$array['filter']['category'] = array('set' => false);

?>
