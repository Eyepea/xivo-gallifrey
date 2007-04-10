<?php

$array = array();

$array['element'] = array();
$array['element']['type'] = array();
$array['element']['type']['value'] = array('user','group','meetme','custom');
$array['element']['type']['default'] = '';

$array['element']['typeid'] = array();
$array['element']['typeid']['default'] = '';

$array['element']['extenid'] = array();
$array['element']['extenid']['default'] = '';

$array['element']['custom'] = array();
$array['element']['custom']['default'] = '';

$array['element']['disable'] = array();
$array['element']['disable']['value'] = array(0,1);
$array['element']['disable']['default'] = 0;

$array['filter'] = array();
$array['filter']['type'] = array('arrkey' => array('user','group','meetme','custom'));
$array['filter']['typeid'] = array('set' => false,'chk' => 2,'cast' => 'uint');
$array['filter']['extenid'] = array('cast' => 'uint');
$array['filter']['custom'] = array('set' => false,'chk' => 2,'minlen' => 1,'maxlen' => 128);
$array['filter']['disable'] = array('bool' => true);

?>
