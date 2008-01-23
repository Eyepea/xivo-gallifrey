<?php

$array = array();
$array['element'] = array();

$array['element']['serverid'] = array();
$array['element']['serverid']['default'] = 0;

$array['element']['type'] = array();
$array['element']['type']['value'] = array('phonebook');
$array['element']['type']['default'] = 'phonebook';

$array['filter'] = array();
$array['filter']['serverid'] = array('cast' => 'ulongint');
$array['filter']['type'] = array('key' => array('phonebook'));

?>
