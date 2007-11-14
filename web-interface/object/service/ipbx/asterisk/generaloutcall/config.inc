<?php

$array = array();
$array['element'] = array();

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['trunkfeaturesid'] = array();
$array['element']['trunkfeaturesid']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('emergency','special');
$array['element']['type']['default'] = 'emergency';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['filter'] = array();
$array['filter']['exten'] = array('set' => false,'chk' => 2,'maxlen' => 40,'regexp' => '/^[_]?[0-9NXZ\*#\-\[\]]+[\.\!]?$/');
$array['filter']['trunkfeaturesid'] = array('cast' => 'uint');
$array['filter']['type'] = array('key' => array('emergency','special'));
$array['filter']['commented'] = array('bool' => true);

?>
