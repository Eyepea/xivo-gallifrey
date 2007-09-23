<?php

$array = array();
$array['element'] = array();

$array['element']['rightcallid'] = array();
$array['element']['rightcallid']['default'] = 0;

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user','group','outcall');
$array['element']['type']['default'] = 'outcall';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['filter'] = array();
$array['filter']['type'] = array('key' => array('user','group','outcall'));
$array['filter']['typeval'] = array('maxlen' => 128,'rexgep' => '/^[a-z0-9_-]+$/');

?>
