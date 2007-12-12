<?php

$array['element'] = array();

$array['element']['outcallid'] = array();
$array['element']['outcallid']['default'] = 0;

$array['element']['trunkid'] = array();
$array['element']['trunkid']['default'] = 0;

$array['element']['priority'] = array();
$array['element']['priority']['default'] = 0;

$array['filter'] = array();
$array['filter']['outcallid'] = array('cast' => 'uint');
$array['filter']['trunkid'] = array('cast' => 'uint');
$array['filter']['priority'] = array('cast' => 'uint','between' => array(0,16));

?>
