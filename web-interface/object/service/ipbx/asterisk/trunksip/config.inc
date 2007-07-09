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
$array['element']['dtmfmode']['value'] = array('rfc2833','inband','info');
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
$array['element']['disallow']['setdef'] = true;

$array['element']['allow'] = array();
$array['element']['allow']['value'] = array(
					XIVO_SRE_IPBX_AST_CODEC_ALAW,
					XIVO_SRE_IPBX_AST_CODEC_G729,
					XIVO_SRE_IPBX_AST_CODEC_GSM,
					XIVO_SRE_IPBX_AST_CODEC_ULAW);
$array['element']['allow']['default'] = '';
$array['element']['allow']['null'] = true;

$array['element']['port'] = array();
$array['element']['port']['default'] = 5060;

$array['element']['ipaddr'] = array();
$array['element']['ipaddr']['default'] = '';

$array['element']['regexten'] = array();
$array['element']['regexten']['default'] = 0;

$array['element']['setvar'] = array();
$array['element']['setvar']['default'] = '';

$array['element']['fromuser'] = array();
$array['element']['fromuser']['default'] = '';

$array['element']['fromdomain'] = array();
$array['element']['fromdomain']['default'] = '';

$array['element']['host-static'] = array();
$array['element']['host-static']['copy'] = 'host';
$array['element']['host-static']['unset'] = true;

$array['element']['host-dynamic'] = array();
$array['element']['host-dynamic']['value'] = array('dynamic','static');
$array['element']['host-dynamic']['copy'] = 'host';
$array['element']['host-dynamic']['unset'] = true;

$array['element']['call-limit'] = array();
$array['element']['call-limit']['default'] = 0;
$array['element']['call-limit']['setdef'] = true;

$array['filter'] = array();
$array['filter']['name'] = array('maxlen' => 80);
$array['filter']['username'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['secret'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['callerid'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['context'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['call-limit'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,30));
$array['filter']['host'] = array('set' => false,'chk' => 2);
$array['filter']['dtmfmode'] = array('set' => false,'chk' => 2,'arrkey' => array('rfc2833','inband','info'));
$array['filter']['type'] = array('set' => false,'chk' => 2,'arrkey' => array('peer','user','friend'));
$array['filter']['nat'] = array('bool' => true);
$array['filter']['qualify'] = array('bool' => true);
$array['filter']['canreinvite'] = array('bool' => true);
$array['filter']['insecure'] = array('set' => false,'chk' => 2,'arrkey' => array('port','invite','port,invite'));
$array['filter']['disallow'] = array('set' => false,'chk' => 2,'arrkey' => array('all'));
$array['filter']['allow'] = array('set' => false,'chk' => 2,'arrvalue' => array(
						XIVO_SRE_IPBX_AST_CODEC_ALAW,
						XIVO_SRE_IPBX_AST_CODEC_G729,
						XIVO_SRE_IPBX_AST_CODEC_GSM,
						XIVO_SRE_IPBX_AST_CODEC_ULAW));
$array['filter']['port'] = array('set' => false,'cast' => 'uint','between' => array(1,65535));
$array['filter']['ipaddr'] = array('set' => false,'chk' => 2);
$array['filter']['regexten'] = array('set' => false,'chk' => 2);
$array['filter']['setvar'] = array('set' => false,'chk' => 2);
$array['filter']['fromuser'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['fromdomain'] = array('set' => false,'chk' => 2,'func' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['host-dynamic'] = array('set' => false,'arrkey' => array('dynamic','static'));
$array['filter']['host-static'] = array('set' => false,'func' => 'xivo_filter::chk_host_ipv4_strict');

?>
