<?php

$array['element'] = array();

$array['element']['outcallid'] = array();
$array['element']['outcallid']['default'] = 0;

$array['element']['trunkfeaturesid'] = array();
$array['element']['trunkfeaturesid']['default'] = 0;

$array['element']['priority'] = array();
$array['element']['priority']['default'] = 0;

$array['filter'] = array();
$array['filter']['outcallid'] = array('cast' => 'ulongint');
$array['filter']['trunkfeaturesid'] = array('cast' => 'ulongint');
$array['filter']['priority'] = array('cast' => 'uint','between' => array(0,16));

?>
