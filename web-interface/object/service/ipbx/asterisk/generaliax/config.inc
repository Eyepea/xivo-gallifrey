<?php

$array = array();
$array['element'] = array();
$array['element']['bindport'] = array();
$array['element']['bindport']['default'] = 4569;

$array['element']['bindaddr'] = array();
$array['element']['bindaddr']['default'] = '0.0.0.0';
$array['element']['bindaddr']['null'] = true;

$array['element']['delayreject']['value'] = array('no','yes');
$array['element']['delayreject']['default'] = 'no';
$array['element']['delayreject']['set'] = true;

$array['element']['language'] = array();
$array['element']['language']['value'] = array('en','fr');
$array['element']['language']['default'] = 'en';

$array['element']['tos'] = array();
$array['element']['tos']['value'] = array('none','lowdelay','throughput','reliability','mincost');
$array['element']['tos']['default'] = 'lowdelay';

$array['element']['jitterbuffer'] = array();
$array['element']['jitterbuffer']['value'] = array('no','yes');
$array['element']['jitterbuffer']['default'] = 'no';
$array['element']['jitterbuffer']['set'] = true;

$array['element']['dropcount'] = array();
$array['element']['dropcount']['default'] = 2;

$array['element']['maxexcessbuffer'] = array();
$array['element']['maxexcessbuffer']['default'] = 80;

$array['element']['minexcessbuffer'] = array();
$array['element']['minexcessbuffer']['default'] = 10;

$array['element']['jittershrinkrate'] = array();
$array['element']['jittershrinkrate']['default'] = 1;

$array['element']['qualify'] = array();
$array['element']['qualify']['value'] = array('no','yes');
$array['element']['qualify']['default'] = 'yes';
$array['element']['qualify']['set'] = true;

$array['element']['rtcachefriends'] = array();
$array['element']['rtcachefriends']['value'] = array('no','yes');
$array['element']['rtcachefriends']['default'] = 'yes';
$array['element']['rtcachefriends']['set'] = true;

$array['element']['disallow'] = array();
$array['element']['disallow']['value'] = array('all');
$array['element']['disallow']['default'] = 'all';

$array['element']['allow'] = array();
$array['element']['allow']['value'] = array(
					XIVO_SRE_IPBX_AST_CODEC_ALAW,
					XIVO_SRE_IPBX_AST_CODEC_G729,
					XIVO_SRE_IPBX_AST_CODEC_GSM,
					XIVO_SRE_IPBX_AST_CODEC_ULAW);
$array['element']['allow']['default'] = '';
$array['element']['allow']['null'] = true;

$array['element']['register'] = array();
$array['element']['register']['default'] = '';

$array['filter'] = array();
$array['filter']['bindport'] = array('cast' => 'uint','between' => array(1,65535));
$array['filter']['bindaddr'] = array('func' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['delayreject'] = array('bool' => true);
$array['filter']['language'] = array('arrkey' => array('en','fr'));
$array['filter']['tos'] = array('arrkey' => array('none','lowdelay','throughput','reliability','mincost'));
$array['filter']['qualify'] = array('bool' => true);
$array['filter']['rtcachefriends'] = array('bool' => true);
$array['filter']['jitterbuffer'] = array('bool' => true);
$array['filter']['dropcount'] = array('cast' => 'uint','maxlen' => 2);
$array['filter']['maxexcessbuffer'] = array('cast' => 'uint','maxlen' => 3);
$array['filter']['minexcessbuffer'] = array('cast' => 'uint','maxlen' => 2);
$array['filter']['jittershrinkrate'] = array('cast' => 'uint','maxlen' => 2);
$array['filter']['disallow'] = array('set' => false,'chk' => 2,'arrkey' => array('all'));
$array['filter']['allow'] = array('set' => false,'arrvalue' => array(
							XIVO_SRE_IPBX_AST_CODEC_ALAW,
							XIVO_SRE_IPBX_AST_CODEC_G729,
							XIVO_SRE_IPBX_AST_CODEC_GSM,
							XIVO_SRE_IPBX_AST_CODEC_ULAW));
$array['filter']['register_username'] = array('preg' => '/^[a-z0-9_@\.-]+$/i');
$array['filter']['register_authuser'] = array('set' => false,'chk' => 2,'preg' => '/^[a-z0-9_\.-]+$/i');
$array['filter']['register_password'] = array('preg' => '/^[a-z0-9_\.-]+$/i');
$array['filter']['register_host'] = array('func' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['register_port'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['register_contact'] = array('set' => false,'chk' => 2,'preg' => '/^[a-z0-9]+$/i');

?>
