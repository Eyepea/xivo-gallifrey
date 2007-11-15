<?php

$array = array();

$array['element'] = array();

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';
$array['element']['context']['setdef'] = true;

$array['element']['type'] = array();
$array['element']['type']['value'] = array('endcall','user','group','queue','meetme','schedule','application','custom');
$array['element']['type']['default'] = 'endcall';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['element']['endcall'] = array();
$array['element']['endcall']['value'] = array('hangup','busy','congestion');
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
$array['filter']['exten'] = array('maxlen' => 40,'regexp' => '/^[_]?[0-9NXZ\*#\-\[\]]+[\.\!]?$/');
$array['filter']['context'] = array('nokey' => xivo_service_asterisk::get_contexts_list(),
				    'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 39);
$array['filter']['type'] = array('key' => array('endcall','user','group','queue','meetme','schedule','application','custom'));
$array['filter']['typeval'] = array('maxlen' => 255,'cast' => 'sgraph');
$array['filter']['linked'] = array('bool' => true);
$array['filter']['commented'] = array('bool' => true);

?>
