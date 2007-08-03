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

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;

$array['filter'] = array();
$array['filter']['type'] = array('key' => array('user','group','meetme','custom'));
$array['filter']['typeid'] = array('set' => false,'chk' => 2,'cast' => 'uint');
$array['filter']['extenid'] = array('cast' => 'uint');
$array['filter']['custom'] = array('set' => false,'chk' => 2,'minlen' => 1,'maxlen' => 128);
$array['filter']['number'] = array('set' => false,'chk' => 2,'cast' => 'digit','maxlen' => 40);
$array['filter']['commented'] = array('bool' => true);

?>
