<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['callerid'] = array();
$array['element']['callerid']['default'] = '';
$array['element']['callerid']['null'] = true;

$array['element']['host'] = array();
$array['element']['host']['value'] = array('dynamic');
$array['element']['host']['default'] = 'dynamic';
$array['element']['host']['setdef'] = true;

$array['element']['dtmfmode'] = array();
$array['element']['dtmfmode']['value'] = array('rfc2833','inband','info','auto');
$array['element']['dtmfmode']['default'] = 'info';
$array['element']['dtmfmode']['setdef'] = true;

$array['element']['type'] = array();
$array['element']['type']['value'] = array('peer','user','friend');
$array['element']['type']['default'] = 'peer';
$array['element']['type']['setdef'] = true;

$array['element']['nat'] = array();
$array['element']['nat']['value'] = array('no','yes');
$array['element']['nat']['default'] = 'no';
$array['element']['nat']['set'] = true;

$array['element']['qualify'] = array();
$array['element']['qualify']['value'] = array('no','yes');
$array['element']['qualify']['default'] = 'no';
$array['element']['qualify']['set'] = true;

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['canreinvite'] = array();
$array['element']['canreinvite']['value'] = array('no','yes');
$array['element']['canreinvite']['default'] = 'yes';
$array['element']['canreinvite']['set'] = true;

$array['element']['insecure'] = array();
$array['element']['insecure']['value'] = array('port' => 'port','invite' => 'invite','port,invite' => 'all');
$array['element']['insecure']['default'] = '';
$array['element']['insecure']['null'] = true;

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
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G726,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ADPCM,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_SLIN,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_LPC10,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_G729,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_SPEEX,
					XIVO_SRE_IPBX_AST_CODEC_AUDIO_ILBC,
					XIVO_SRE_IPBX_AST_CODEC_IMAGE_JPEG,
					XIVO_SRE_IPBX_AST_CODEC_IMAGE_PNG,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H261,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263,
					XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263P);
$array['element']['allow']['default'] = '';
$array['element']['allow']['null'] = true;

$array['element']['port'] = array();
$array['element']['port']['default'] = '';
$array['element']['port']['null'] = true;

$array['element']['ipaddr'] = array();
$array['element']['ipaddr']['default'] = '';
$array['element']['ipaddr']['null'] = true;

$array['element']['regexten'] = array();
$array['element']['regexten']['default'] = 0;

$array['element']['setvar'] = array();
$array['element']['setvar']['default'] = '';

$array['element']['fromuser'] = array();
$array['element']['fromuser']['default'] = '';

$array['element']['fromdomain'] = array();
$array['element']['fromdomain']['default'] = '';

$array['element']['host-static'] = array();
$array['element']['host-static']['default'] = '';
$array['element']['host-static']['unset'] = true;

$array['element']['host-dynamic'] = array();
$array['element']['host-dynamic']['value'] = array('dynamic','static');
$array['element']['host-dynamic']['unset'] = true;

$array['element']['call-limit'] = array();
$array['element']['call-limit']['default'] = 0;
$array['element']['call-limit']['setdef'] = true;

$array['filter'] = array();
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[a-z0-9_-]+$/');
$array['filter']['username'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['secret'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['callerid'] = array('set' => false,'chk' => 2,'maxlen' => 160);
$array['filter']['context'] = array('set' => false,'chk' => 2,'nokey' => array('general','global','globals','hints','features','authentication','zonemessages'),'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 39);
$array['filter']['call-limit'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,30));
$array['filter']['host'] = array('set' => false,'chk' => 2,'key' => array('dynamic','static'),'strict' => false,'func' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['dtmfmode'] = array('set' => false,'chk' => 2,'key' => array('rfc2833','inband','info','auto'));
$array['filter']['type'] = array('set' => false,'chk' => 2,'key' => array('peer','user','friend'));
$array['filter']['nat'] = array('bool' => true);
$array['filter']['qualify'] = array('bool' => true);
$array['filter']['canreinvite'] = array('bool' => true);
$array['filter']['insecure'] = array('set' => false,'chk' => 2,'key' => array('port','invite','port,invite'));
$array['filter']['disallow'] = array('set' => false,'chk' => 2,'key' => array('all'));
$array['filter']['allow'] = array('set' => false,'chk' => 2,'value' => array(
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G723,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_GSM,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ULAW,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ALAW,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G726,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ADPCM,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_SLIN,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_LPC10,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_G729,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_SPEEX,
						XIVO_SRE_IPBX_AST_CODEC_AUDIO_ILBC,
						XIVO_SRE_IPBX_AST_CODEC_IMAGE_JPEG,
						XIVO_SRE_IPBX_AST_CODEC_IMAGE_PNG,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H261,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263,
						XIVO_SRE_IPBX_AST_CODEC_VIDEO_H263P));
$array['filter']['port'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['ipaddr'] = array('set' => false,'chk' => 0);
$array['filter']['regexten'] = array('set' => false,'chk' => 0);
$array['filter']['setvar'] = array('set' => false,'chk' => 0);
$array['filter']['fromuser'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['fromdomain'] = array('set' => false,'chk' => 2,'func' => 'xivo_filter::chk_host_ipv4_strict');

?>
