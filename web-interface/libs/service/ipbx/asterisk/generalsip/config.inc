<?php

$array = array();
$array['element'] = array();
$array['element']['bindport'] = array();
$array['element']['bindport']['default'] = 5060;

$array['element']['bindaddr'] = array();
$array['element']['bindaddr']['default'] = '0.0.0.0';

$array['element']['srvlookup'] = array();
$array['element']['srvlookup']['value'] = array('no','yes');
$array['element']['srvlookup']['default'] = 'no';
$array['element']['srvlookup']['set'] = true;

$array['element']['language'] = array();
$array['element']['language']['value'] = array('en','fr');
$array['element']['language']['default'] = 'en';

$array['element']['tos'] = array();
$array['element']['tos']['value'] = array('none','lowdelay','throughput','reliability','mincost');
$array['element']['tos']['default'] = 'lowdelay';

$array['element']['dtmfmode'] = array();
$array['element']['dtmfmode']['value'] = array('','rfc2833','inband','info');
$array['element']['dtmfmode']['default'] = '';

$array['element']['relaxdtmf'] = array();
$array['element']['relaxdtmf']['value'] = array('no','yes');
$array['element']['relaxdtmf']['default'] = 'yes';
$array['element']['relaxdtmf']['set'] = true;

$array['element']['maxexpiry'] = array();
$array['element']['maxexpiry']['default'] = 3600;

$array['element']['defaultexpiry'] = array();
$array['element']['defaultexpiry']['default'] = 120;

$array['element']['useragent'] = array();
$array['element']['useragent']['default'] = 'Xivo PBX';

$array['element']['allowguest'] = array();
$array['element']['allowguest']['value'] = array('no','yes');
$array['element']['allowguest']['default'] = 'yes';
$array['element']['allowguest']['set'] = true;

$array['element']['nat'] = array();
$array['element']['nat']['value'] = array('no','yes');
$array['element']['nat']['default'] = 'yes';
$array['element']['nat']['set'] = true;

$array['element']['qualify'] = array();
$array['element']['qualify']['value'] = array('no','yes');
$array['element']['qualify']['default'] = 'yes';
$array['element']['qualify']['set'] = true;

$array['element']['rtcachefriends'] = array();
$array['element']['rtcachefriends']['value'] = array('no','yes');
$array['element']['rtcachefriends']['default'] = 'yes';
$array['element']['rtcachefriends']['set'] = true;

$array['element']['musicclass'] = array();
$array['element']['musicclass']['default'] = 'default';

$array['element']['checkmwi'] = array();
$array['element']['checkmwi']['default'] = 10;

$array['element']['videosupport'] = array();
$array['element']['videosupport']['value'] = array('no','yes');
$array['element']['videosupport']['default'] = 'no';
$array['element']['videosupport']['set'] = true;

$array['element']['disallow'] = array();
$array['element']['disallow']['value'] = array('all');

$array['element']['allow'] = array();
$array['element']['allow']['value'] = array('gsm','alaw','ulaw','g729');

$array['element']['register'] = array();
$array['element']['register']['default'] = '';

$array['filter'] = array();
$array['filter']['bindport'] = array('cast' => 'uint','between' => array(1,65535));
$array['filter']['bindaddr'] = array('func' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['srvlookup'] = array('bool' => true);
$array['filter']['language'] = array('arrkey' => array('en','fr'));
$array['filter']['realm'] = array('maxlen' => 128);
$array['filter']['maxexpiry'] = array('cast' => 'uint','maxlen' => 4);
$array['filter']['defaultexpiry'] = array('cast' => 'uint','maxlen' => 4);
$array['filter']['useragent'] = array('maxlen' => 128);
$array['filter']['nat'] = array('bool' => true);
$array['filter']['qualify'] = array('bool' => true);
$array['filter']['rtcachefriends'] = array('bool' => true);
$array['filter']['allowguest'] = array('bool' => true);
$array['filter']['tos'] = array('arrkey' => array('none','lowdelay','throughput','reliability','mincost'));
$array['filter']['dtmfmode'] = array('arrkey' => array('','rfc2833','inband','info'));
$array['filter']['relaxdtmf'] = array('bool' => true);
$array['filter']['externip'] = array('func' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['context'] = array('maxlen' => 128);
$array['filter']['musicclass'] = array('maxlen' => 128);
$array['filter']['checkmwi'] = array('cast' => 'uint','maxlen' => 4);
$array['filter']['vmexten'] = array('preg' => '/^[a-z0-9\*#]+$/i','maxlen' => 128);
$array['filter']['videosupport'] = array('bool' => true);
$array['filter']['disallow'] = array('set' => false);
$array['filter']['allow'] = array('set' => false,'arrvalue' => array('gsm','alaw','ulaw','g729'));
$array['filter']['register_username'] = array('preg' => '/^[a-z0-9_@\.-]+$/i');
$array['filter']['register_authuser'] = array('set' => false,'chk' => 2,'preg' => '/^[a-z0-9_\.-]+$/i');
$array['filter']['register_password'] = array('preg' => '/^[a-z0-9_\.-]+$/i');
$array['filter']['register_host'] = array('func' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['register_port'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['register_contact'] = array('set' => false,'chk' => 2,'cast' => 'uint','maxlen' => 6);
/*
$array['filter']['register'] = array('preg' => '#^([a-z0-9_\.-]+)(@([a-z0-9-]+)(\.[a-z0-9-]+)*(\.[a-z]{2,4})|@([1-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))?:([a-z0-9_\.-]+)(:[a-z0-9_\.-]+)?@(([a-z0-9-]+)(\.[a-z0-9-]+)*(\.[a-z]{2,4})|([1-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))(:([1-5][0-9]{1,4}|[1-9]([0-9]{1,3})?|6[0-4][0-9]{1,3}|65[0-4][0-9]{1,2}|655[0-2][0-9]|6553[0-5]))?(/[0-9]+)?$#i');
*/
?>
