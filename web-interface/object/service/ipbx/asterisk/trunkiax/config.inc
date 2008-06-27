<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['username'] = array();
$array['element']['username']['default'] = '';

$array['element']['secret'] = array();
$array['element']['secret']['default'] = '';

$array['element']['callerid'] = array();
$array['element']['callerid']['default'] = '';
$array['element']['callerid']['null'] = true;

$array['element']['host'] = array();
$array['element']['host']['value'] = array('dynamic');
$array['element']['host']['default'] = 'dynamic';
$array['element']['host']['setdef'] = true;

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user','peer','friend');
$array['element']['type']['default'] = 'peer';
$array['element']['type']['setdef'] = true;

$array['element']['notransfer'] = array();
$array['element']['notransfer']['value'] = array('no','yes');
$array['element']['notransfer']['default'] = 'no';
$array['element']['notransfer']['set'] = true;

$array['element']['trunk'] = array();
$array['element']['trunk']['value'] = array('no','yes');
$array['element']['trunk']['default'] = 'no';
$array['element']['trunk']['set'] = true;

$array['element']['qualify'] = array();
$array['element']['qualify']['value'] = array('no','yes');
$array['element']['qualify']['default'] = 'no';
$array['element']['qualify']['set'] = true;

$array['element']['context'] = array();
$array['element']['context']['default'] = '';
$array['element']['context']['null'] = true;

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

$array['element']['port'] = array();
$array['element']['port']['default'] = '';
$array['element']['port']['null'] = true;

$array['element']['ipaddr'] = array();
$array['element']['ipaddr']['default'] = '';
$array['element']['ipaddr']['null'] = true;

$array['element']['host-static'] = array();
$array['element']['host-static']['default'] = '';
$array['element']['host-static']['unset'] = true;

$array['element']['host-dynamic'] = array();
$array['element']['host-dynamic']['value'] = array('dynamic','static');
$array['element']['host-dynamic']['unset'] = true;

$array['element']['call-limit'] = array();
$array['element']['call-limit']['default'] = 0;
$array['element']['call-limit']['setdef'] = true;

$array['element']['protocol'] = array();
$array['element']['protocol']['default'] = XIVO_SRE_IPBX_AST_PROTO_IAX;
$array['element']['protocol']['forcedef'] = true;

$array['element']['category'] = array();
$array['element']['category']['default'] = 'trunk';
$array['element']['category']['forcedef'] = true;

$array['filter'] = array();
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[a-z0-9\-]+$/');
$array['filter']['username'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['secret'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['callerid'] = array('set' => false,'chk' => 2,'maxlen' => 160);
$array['filter']['context'] = array('set' => false,
				    'chk' => 2,
				    'callback' => 'xivo_service_asterisk::chk_context',
				    'callbackparam' => array(array(XIVO_SRE_IPBX_AST_CONTEXT_ZONEMESSAGES)));
$array['filter']['call-limit'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,30));
$array['filter']['host'] = array('set' => false,'chk' => 2,'key' => array('dynamic','static'),'strict' => false,'callback' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['type'] = array('set' => false,'chk' => 2,'key' => array('user','peer','friend'));
$array['filter']['notransfer'] = array('bool' => true);
$array['filter']['trunk'] = array('bool' => true);
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
$array['filter']['port'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['ipaddr'] = array('set' => false,'chk' => 0);
$array['filter']['protocol'] = array('set' => false);
$array['filter']['category'] = array('set' => false);

?>
