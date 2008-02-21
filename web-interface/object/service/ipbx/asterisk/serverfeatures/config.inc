<?php

$array = array();
$array['element'] = array();

$array['element']['serverid'] = array();
$array['element']['serverid']['default'] = 0;

$array['element']['feature'] = array();
$array['element']['feature']['value'] = array('phonebook');
$array['element']['feature']['default'] = 'phonebook';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('xivo','ldap');
$array['element']['type']['default'] = 'xivo';

$array['filter'] = array();
$array['filter']['serverid'] = array('cast' => 'ulongint');
$array['filter']['feature'] = array('key' => array('phonebook'));
$array['filter']['type'] = array('key' => array('xivo','ldap'));

?>
