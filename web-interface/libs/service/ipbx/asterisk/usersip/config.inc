<?php

$array = array();

$array['initialized'] = 'sip';

$array['element'] = array();

$array['element']['host'] = array();
$array['element']['host']['value'] = array('dynamic');

$array['element']['dtmfmode'] = array();
$array['element']['dtmfmode']['value'] = array('rfc2833','inband','info');
$array['element']['dtmfmode']['default'] = 'rfc2833';

$array['element']['canreinvite'] = array();
$array['element']['canreinvite']['value'] = array('no','yes');
$array['element']['canreinvite']['default'] = 'no';
$array['element']['canreinvite']['set'] = true;

$array['element']['amaflags'] = array();
$array['element']['amaflags']['value'] = array('default','omit','billing','documentation');
$array['element']['amaflags']['default'] = 'documentation';

$array['element']['nat'] = array();
$array['element']['nat']['value'] = array('no','yes');
$array['element']['nat']['default'] = 'no';
$array['element']['nat']['set'] = true;

$array['element']['qualify'] = array();
$array['element']['qualify']['value'] = array('no','yes');
$array['element']['qualify']['default'] = 'no';
$array['element']['qualify']['set'] = true;

$array['element']['port'] = array();
$array['element']['port']['default'] = 5060;

$array['element']['ipaddr'] = array();
$array['element']['ipaddr']['default'] = '';

$array['element']['regexten'] = array();
$array['element']['regexten']['default'] = 0;

$array['element']['setvar'] = array();
$array['element']['setvar']['default'] = 0;

$array['element']['callgroup'] = array();
$array['element']['callgroup']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['username'] = array();
$array['element']['username']['default'] = '';

$array['element']['host-static'] = array();
$array['element']['host-static']['copy'] = 'host';
$array['element']['host-static']['unset'] = true;

$array['element']['host-dynamic'] = array();
$array['element']['host-dynamic']['value'] = array('dynamic','static');
$array['element']['host-dynamic']['copy'] = 'host';
$array['element']['host-dynamic']['unset'] = true;

$array['filter'] = array();
$array['filter']['callerid'] = array('minlen' => 1,'maxlen' => 80);
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 80);
$array['filter']['secret'] = array('set' => false,'chk' => 2,'minlen' => 1,'maxlen' => 80);
$array['filter']['host'] = array('set' => false);
$array['filter']['dtmfmode'] = array('arrkey' => array('rfc2833','inband','info'));
$array['filter']['canreinvite'] = array('bool' => true);
$array['filter']['amaflags'] = array('arrkey' => array('default','omit','billing','documentation'));
$array['filter']['accountcode'] = array('chk' => 2,'maxlen' => 20);
$array['filter']['callgroup'] = array('set' => false,'chk' => 2,'preg' => '/^([0-9]+[,]?)*$/','maxlen' => 10);
$array['filter']['context'] = array('set' => false,'chk' => 2,'maxlen' => 80);
$array['filter']['nat'] = array('bool' => true);
$array['filter']['qualify'] = array('bool' => true);
$array['filter']['port'] = array('set' => false);
$array['filter']['ipaddr'] = array('set' => false);
$array['filter']['regexten'] = array('set' => false);
$array['filter']['setvar'] = array('set' => false);
$array['filter']['username'] = array('set' => false);
$array['filter']['host-dynamic'] = array('set' => false,'arrkey' => array('dynamic','static'));
$array['filter']['host-static'] = array('set' => false,'func' => 'xivo_filter::chk_host_ipv4_strict');

?>
