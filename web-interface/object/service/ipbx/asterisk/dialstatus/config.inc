<?php

$array = array();
$array['element'] = array();

$array['element']['status'] = array();
$array['element']['status']['value'] = array('noanswer','busy','congestion','chanunavail');
$array['element']['status']['default'] = '';

$array['element']['category'] = array();
$array['element']['category']['value'] = array('user','group');
$array['element']['category']['default'] = '';

$array['element']['categoryval'] = array();
$array['element']['categoryval']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('endcall','user','group','queue','meetme','schedule','application','sound','custom');
$array['element']['type']['default'] = 'endcall';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['element']['endcall'] = array();
$array['element']['endcall']['value'] = array('none','hangup','busy','congestion');
$array['element']['endcall']['default'] = 'none';
$array['element']['endcall']['unset'] = true;

$array['element']['application'] = array();
$array['element']['application']['value'] = array('faxtomail','disa','directory');
$array['element']['application']['unset'] = true;

$array['element']['linked'] = array();
$array['element']['linked']['value'] = array(0,1);
$array['element']['linked']['default'] = 0;
$array['element']['linked']['set'] = true;

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['filter'] = array();
$array['filter']['status'] = array('key' => array('noanswer','busy','congestion','chanunavail'));
$array['filter']['category'] = array('key' => array('user','group'));
$array['filter']['categoryval'] = array('maxlen' => 128,'cast' => 'sgraph');
$array['filter']['type'] = array('key' => array('endcall','user','group','queue','meetme','schedule','application','sound','custom'));
$array['filter']['typeval'] = array('maxlen' => 255,'cast' => 'sgraph');
$array['filter']['linked'] = array('bool' => true);

?>
