<?php

$array = array();
$array['element'] = array();

$array['element']['username'] = array();
$array['element']['username']['default'] = '';

$array['element']['password'] = array();
$array['element']['password']['default'] = '';

$array['element']['authuser'] = array();
$array['element']['authuser']['default'] = '';

$array['element']['host'] = array();
$array['element']['host']['default'] = '';

$array['element']['port'] = array();
$array['element']['port']['default'] = '';

$array['element']['contact'] = array();
$array['element']['contact']['default'] = '';

$array['filter'] = array();
$array['filter']['username'] = array('set' => false,'regexp' => '/^[a-z0-9_@\.-]+$/i','maxlen' => 80);
$array['filter']['password'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_\.-]+$/i','maxlen' => 80);
$array['filter']['authuser'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_\.-]+$/i','maxlen' => 80);
$array['filter']['host'] = array('set' => false,'callback' => 'xivo_filter::chk_host_ipv4_strict','maxlen' => 64);
$array['filter']['port'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,65535));
$array['filter']['contact'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9]+$/i','maxlen' => 250);

?>
