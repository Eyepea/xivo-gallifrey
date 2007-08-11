<?php

$array = array();
$array['element'] = array();

$array['element']['rangebeg'] = array();
$array['element']['rangebeg']['default'] = '';

$array['element']['rangeend'] = array();
$array['element']['rangeend']['default'] = '';

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['extenhash'] = array();
$array['element']['extenhash']['default'] = '';

$array['element']['extenmode'] = array();
$array['element']['extenmode']['value'] = array('range','extension');
$array['element']['extenmode']['default'] = 'range';
$array['element']['extenmode']['unset'] = true;

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
$array['filter']['rangebeg'] = array('set' => false,'chk' => 2,'maxlen' => 20,'cast' => 'uint');
$array['filter']['rangeend'] = array('set' => false,'chk' => 2,'maxlen' => 20,'cast' => 'uint');
$array['filter']['exten'] = array('set' => false,'chk' => 2,'maxlen' => 19,'regexp' => '/^((?![_]?(\.|\!))|[_]?[0-9NXZ\*#\-\[\]]+(\.|\!)?)$/');
$array['filter']['extenhash'] = array('set' => false,'chk' => 2,'maxlen' => 40,'cast' => 'xdigit');
$array['filter']['trunkfeaturesid'] = array('cast' => 'uint');
$array['filter']['type'] = array('key' => array('emergency','special'));
$array['filter']['commented'] = array('bool' => true);

?>
