<?php

$array = array();
$array['element'] = array();

$array['element']['host'] = array();
$array['element']['host']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('phonebook');
$array['element']['type']['default'] = 'phonebook';

$array['filter'] = array();
$array['filter']['host'] = array('func' => 'xivo_filter::chk_host_subnet_ipv4_strict');
$array['filter']['type'] = array('key' => array('phonebook'));

?>
