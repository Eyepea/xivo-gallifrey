<?php

$array = array();
$array['element'] = array();

$array['element']['extenid'] = array();
$array['element']['extenid']['default'] = '';

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
$array['filter']['extenid'] = array('cast' => 'uint');
$array['filter']['trunkfeaturesid'] = array('cast' => 'uint');
$array['filter']['type'] = array('key' => array('emergency','special'));
$array['filter']['commented'] = array('bool' => true);

?>
