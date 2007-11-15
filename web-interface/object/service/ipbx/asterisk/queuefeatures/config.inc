<?php

$array['element'] = array();

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = 'default';
$array['element']['context']['setdef'] = true;

$array['element']['data_quality'] = array();
$array['element']['data_quality']['value'] = array(0,1);
$array['element']['data_quality']['default'] = 0;

$array['element']['hitting_callee'] = array();
$array['element']['hitting_callee']['value'] = array(0,1);
$array['element']['hitting_callee']['default'] = 0;

$array['element']['hitting_caller'] = array();
$array['element']['hitting_caller']['value'] = array(0,1);
$array['element']['hitting_caller']['default'] = 0;

$array['element']['retries'] = array();
$array['element']['retries']['value'] = array(0,1);
$array['element']['retries']['default'] = 0;

$array['element']['ring'] = array();
$array['element']['ring']['value'] = array(0,1);
$array['element']['ring']['default'] = 0;

$array['element']['transfer_user'] = array();
$array['element']['transfer_user']['value'] = array(0,1);
$array['element']['transfer_user']['default'] = 0;

$array['element']['transfer_call'] = array();
$array['element']['transfer_call']['value'] = array(0,1);
$array['element']['transfer_call']['default'] = 0;

$array['element']['write_caller'] = array();
$array['element']['write_caller']['value'] = array(0,1);
$array['element']['write_caller']['default'] = 0;

$array['element']['write_calling'] = array();
$array['element']['write_calling']['value'] = array(0,1);
$array['element']['write_calling']['default'] = 0;

$array['element']['url'] = array();
$array['element']['url']['default'] = '';

$array['element']['announceoverride'] = array();
$array['element']['announceoverride']['default'] = '';

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
$array['filter']['data_quality'] = array('bool' => true);
$array['filter']['hitting_callee'] = array('bool' => true);
$array['filter']['hitting_caller'] = array('bool' => true);
$array['filter']['retries'] = array('bool' => true);
$array['filter']['ring'] = array('bool' => true);
$array['filter']['transfer_user'] = array('bool' => true);
$array['filter']['transfer_call'] = array('bool' => true);
$array['filter']['write_caller'] = array('bool' => true);
$array['filter']['write_calling'] = array('bool' => true);
$array['filter']['url'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 255);
$array['filter']['announceoverride'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['timeout'] = array('key' => array(0,30,60,90,120,180,240,300,360,420,480,540,600,900,1200,1500,1800));

?>
