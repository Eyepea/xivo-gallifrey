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
$array['element']['timeout']['value'] = array(0 => 0,30 => 30,60 => 1,90 => array(1,30),
					      120 => 2,180 => 3,240 => 4,300 => 5,360 => 6,
					      420 => 7,480 => 8,540 => 9,
					      600 => 10,900 => 15,1200 => 20,1500 => 25,1800 => 30);
$array['element']['timeout']['default'] = 0;

$array['filter'] = array();
$array['filter']['number'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','maxlen' => 40);
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['context'] = array('set' => false,'chk' => 2,
				    'nokey' => xivo_service_asterisk::get_contexts_list(),
				    'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 39);
$array['filter']['timeout'] = array('key' => array(0,30,60,90,120,180,240,300,360,420,480,540,600,900,1200,1500,1800));

?>
