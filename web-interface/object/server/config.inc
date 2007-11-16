<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['host'] = array();
$array['element']['host']['default'] = '';

$array['element']['port'] = array();
$array['element']['port']['default'] = 443;

$array['element']['ssl'] = array();
$array['element']['ssl']['value'] = array(0,1);
$array['element']['ssl']['default'] = 1;
$array['element']['ssl']['set'] = true;

$array['element']['disable'] = array();
$array['element']['disable']['value'] = array(0,1);
$array['element']['disable']['default'] = 0;
$array['element']['disable']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


$array['filter'] = array();
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/','minlen' => 1,'maxlen' => 64);
$array['filter']['host'] = array('callback' => 'xivo_filter::chk_host_ipv4_strict');
$array['filter']['port'] = array('cast' => 'uint','between' => array(1,65535));
$array['filter']['ssl'] = array('bool' => true);
$array['filter']['disable'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
