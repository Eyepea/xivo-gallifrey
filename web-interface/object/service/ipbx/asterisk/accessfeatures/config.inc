<?php

$array = array();
$array['element'] = array();

$array['element']['host'] = array();
$array['element']['host']['default'] = '';

$array['element']['feature'] = array();
$array['element']['feature']['value'] = array('phonebook');
$array['element']['feature']['default'] = 'phonebook';

$array['filter'] = array();
$array['filter']['host'] = array('callback' => 'xivo_filter::chk_host_subnet_ipv4_strict');
$array['filter']['feature'] = array('key' => array('phonebook'));

?>
