<?php

$array = array();

$array['initialized'] = 'iax';

$array['element'] = array();
$array['element']['host'] = array();
$array['element']['host']['value'] = array('dynamic');

$array['element']['amaflags'] = array();
$array['element']['amaflags']['value'] = array('default','omit','billing','documentation');
$array['element']['amaflags']['default'] = 'documentation';

$array['element']['qualify'] = array();
$array['element']['qualify']['value'] = array('no','yes');
$array['element']['qualify']['default'] = 'no';
$array['element']['qualify']['set'] = true;

$array['element']['disallow'] = array();
$array['element']['disallow']['value'] = array('all');
$array['element']['disallow']['default'] = '';
$array['element']['disallow']['null'] = true;

$array['element']['allow'] = array();
$array['element']['allow']['value'] = array('gsm','alaw','ulaw','g729');
$array['element']['allow']['default'] = '';
$array['element']['allow']['null'] = true;

$array['element']['port'] = array();
$array['element']['port']['default'] = 4569;

$array['element']['ipaddr'] = array();
$array['element']['ipaddr']['default'] = '';

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
$array['filter']['callerid'] = array('chk' => 2,'maxlen' => 80,'notag' => false);
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 80);
$array['filter']['secret'] = array('set' => false,'chk' => 2,'minlen' => 1,'maxlen' => 80);
$array['filter']['host'] = array('set' => false);
$array['filter']['amaflags'] = array('arrkey' => array('default','omit','billing','documentation'));
$array['filter']['accountcode'] = array('chk' => 2,'maxlen' => 20);
$array['filter']['callgroup'] = array('set' => false,'chk' => 2,'preg' => '/^([0-9]+[,]?)*$/','maxlen' => 10);
$array['filter']['context'] = array('set' => false,'chk' => 2,'unarrkey' => array('general','globals'),'preg' => '/^[a-z0-9_-]+$/','maxlen' => 80);
$array['filter']['qualify'] = array('bool' => true);
$array['filter']['disallow'] = array('set' => false,'arrkey' => array('all'));
$array['filter']['allow'] = array('set' => false,'arrvalue' => array('gsm','alaw','ulaw','g729'));
$array['filter']['host-dynamic'] = array('set' => false,'arrkey' => array('dynamic','static'));
$array['filter']['host-static'] = array('set' => false,'func' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['port'] = array('set' => false);
$array['filter']['ipaddr'] = array('set' => false);
$array['filter']['username'] = array('set' => false);

?>
