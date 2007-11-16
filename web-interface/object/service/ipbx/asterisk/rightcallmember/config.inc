<?php

$array = array();
$array['element'] = array();

$array['element']['rightcallid'] = array();
$array['element']['rightcallid']['default'] = 0;

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user','group','incall','outcall');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['filter'] = array();
$array['filter']['rightcallid'] = array('cast' => 'uint');
$array['filter']['type'] = array('key' => array('user','group','incall','outcall'));
$array['filter']['typeval'] = array('maxlen' => 128,'rexgep' => '/^[a-z0-9_-]+$/');

?>
