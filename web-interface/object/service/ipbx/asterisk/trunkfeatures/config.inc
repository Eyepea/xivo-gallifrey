<?php

$array['element'] = array();

$array['element']['trunk'] = array();
$array['element']['trunk']['default'] = '';

$array['element']['trunkid'] = array();
$array['element']['trunkid']['default'] = 0;

$array['element']['registerid'] = array();
$array['element']['registerid']['default'] = 0;

$array['filter'] = array();
$array['filter']['trunk'] = array('set' => true);
$array['filter']['trunkid'] = array('cast' => 'uint');
$array['filter']['registerid'] = array('set' => false,'chk' => 2,'cast' => 'uint');

?>
