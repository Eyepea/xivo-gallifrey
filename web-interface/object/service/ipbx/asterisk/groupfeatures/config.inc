<?php

$array = array();

$array['element'] = array();
$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = 'default';
$array['element']['context']['setdef'] = true;

$array['element']['timeout'] = array();
$array['element']['timeout']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['timeout']['default'] = 0;

$array['filter'] = array();
$array['filter']['number'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','maxlen' => 40);
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['context'] = array('set' => false,'chk' => 2,
				    'nokey' => xivo_service_asterisk::get_contexts_list(),
				    'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 39);
$array['filter']['timeout'] = array('key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));

?>
