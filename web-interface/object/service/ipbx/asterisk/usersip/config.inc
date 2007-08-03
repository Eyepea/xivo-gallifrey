<?php

$array = array();

$array['initialized'] = 'sip';

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
$array['element']['amaflags']['value'] = array('default','omit','billing','documentation');
$array['element']['amaflags']['default'] = 'documentation';
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
$array['element']['disallow']['setdef'] = true;

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

$array['element']['host-static'] = array();
$array['element']['host-static']['copy'] = 'host';
$array['element']['host-static']['unset'] = true;

$array['element']['host-dynamic'] = array();
$array['element']['host-dynamic']['value'] = array('dynamic','static');
$array['element']['host-dynamic']['copy'] = 'host';
$array['element']['host-dynamic']['unset'] = true;

$array['element']['port'] = array();
$array['element']['port']['default'] = 5060;
$array['element']['port']['setdef'] = true;

$array['element']['ipaddr'] = array();
$array['element']['ipaddr']['default'] = '';

$array['element']['regexten'] = array();
$array['element']['regexten']['default'] = 0;
$array['element']['regexten']['setdef'] = true;

$array['element']['accountcode'] = array();
$array['element']['accountcode']['default'] = '';

$array['element']['setvar'] = array();
$array['element']['setvar']['default'] = '';

$array['element']['callgroup'] = array();
$array['element']['callgroup']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['username'] = array();
$array['element']['username']['default'] = '';

$array['element']['call-limit'] = array();
$array['element']['call-limit']['default'] = 0;
$array['element']['call-limit']['setdef'] = true;

$array['filter'] = array();
$array['filter']['callerid'] = array('chk' => 2,'maxlen' => 80,'notag' => false);
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 80);
$array['filter']['secret'] = array('set' => false,'chk' => 2,'minlen' => 1,'maxlen' => 80);
$array['filter']['host'] = array('set' => false,'chk' => 2);
$array['filter']['dtmfmode'] = array('set' => false,'chk' => 2,'key' => array('rfc2833','inband','info','auto'));
$array['filter']['canreinvite'] = array('bool' => true);
$array['filter']['amaflags'] = array('set' => false,'chk' => 2,'key' => array('default','omit','billing','documentation'));
$array['filter']['accountcode'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['callgroup'] = array('set' => false,'chk' => 2,'regexp' => '/^([0-9]+[,]?)*$/','maxlen' => 10);
$array['filter']['context'] = array('set' => false,'chk' => 2,'nokey' => array('general','globals','hints','features','authentication','zonemessages'),'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 80);
$array['filter']['call-limit'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,30));
$array['filter']['nat'] = array('bool' => true);
$array['filter']['qualify'] = array('bool' => true);
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
$array['filter']['host-dynamic'] = array('set' => false,'key' => array('dynamic','static'));
$array['filter']['host-static'] = array('set' => false,'func' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['port'] = array('set' => false,'chk' => 2);
$array['filter']['ipaddr'] = array('set' => false,'chk' => 2);
$array['filter']['regexten'] = array('set' => false,'chk' => 2);
$array['filter']['setvar'] = array('set' => false,'chk' => 2);
$array['filter']['username'] = array('set' => false,'chk' => 2);

?>
