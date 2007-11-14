<?php

$array = array();
$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['trunkfeaturesid'] = array();
$array['element']['trunkfeaturesid']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = 'default';
$array['element']['context']['setdef'] = true;

$array['element']['externprefix'] = array();
$array['element']['externprefix']['default'] = '';

$array['element']['stripnum'] = array();
$array['element']['stripnum']['value'] = array(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20);
$array['element']['stripnum']['default'] = 0;
$array['element']['stripnum']['setdef'] = true;

$array['element']['mode'] = array();
$array['element']['mode']['value'] = array('wizard','extension');
$array['element']['mode']['default'] = 'extension';
$array['element']['mode']['unset'] = true;

$array['element']['prefix'] = array();
$array['element']['prefix']['default'] = '';
$array['element']['prefix']['unset'] = true;

$array['element']['numlen'] = array();
$array['element']['numlen']['value'] = array('*',1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
					     21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39);
$array['element']['numlen']['default'] = '';
$array['element']['numlen']['unset'] = true;

$array['element']['callerid'] = array();
$array['element']['callerid']['default'] = '';

$array['element']['setcallerid'] = array();
$array['element']['setcallerid']['value'] = array(0,1);
$array['element']['setcallerid']['default'] = 0;
$array['element']['setcallerid']['set'] = true;

$array['element']['useenum'] = array();
$array['element']['useenum']['value'] = array(0,1);
$array['element']['useenum']['default'] = 0;
$array['element']['useenum']['set'] = true;

$array['element']['internal'] = array();
$array['element']['internal']['value'] = array(0,1);
$array['element']['internal']['default'] = 0;
$array['element']['internal']['set'] = true;

$array['element']['hangupringtime'] = array();
$array['element']['hangupringtime']['value'] = array(0 => 0,15 => 15,30 => 30,45 => 45,60 => 1,
						     75 => array(1,15),90 => array(1,30),105 => array(1,45),
						     120 => 2,135 => array(2,15),150 => array(2,30),
						     165 => array(2,45),180 => 3,195 => array(3,15),
						     210 => array(3,30),225 => array(3,45),240 => 4,
						     255 => array(4,15),270 => array(4,30),285 => array(4,45),300 => 5);
$array['element']['hangupringtime']['default'] = 0;
$array['element']['hangupringtime']['setdef'] = true;

$array['element']['linked'] = array();
$array['element']['linked']['value'] = array(0,1);
$array['element']['linked']['default'] = 0;
$array['element']['linked']['set'] = true;

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['filter'] = array();
$array['filter']['name'] = array('maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['trunkfeaturesid'] = array('cast' => 'uint');
$array['filter']['context'] = array('nokey' => array('general','global','globals','initconfig','hints','features'),'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 39);
$array['filter']['externprefix'] = array('set' => false,'chk' => 2,'maxlen' => 20,'regexp' => '/^[0-9#\*]+$/');
$array['filter']['stripnum'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,20));
$array['filter']['mode'] = array('set' => false,'chk' => 2,'key' => array('wizard','extension'));
$array['filter']['prefix'] = array('set' => false,'chk' => 2,'maxlen' => 20,'regexp' => '/^[0-9#\*]+$/');
$array['filter']['numlen'] = array('set' => false,'chk' => 2,'key' => array('*',1,2,3,4,5,6,7,8,9,
									    10,11,12,13,14,15,16,17,18,19,20,
					     				    21,22,23,24,25,26,27,28,29,30,
									    31,32,33,34,35,36,37,38,39));
$array['filter']['setcallerid'] = array('bool' => true);
$array['filter']['callerid'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_-]+$/i','maxlen' => 80);
$array['filter']['useenum'] = array('bool' => true);
$array['filter']['internal'] = array('bool' => true);
$array['filter']['hangupringtime'] = array('set' => false,'chk' => 2,'key' => array(0,15,30,45,60,75,90,105,120,135,
										    150,165,180,210,225,240,255,270,
										    285,300));
$array['filter']['linked'] = array('bool' => true);
$array['filter']['commented'] = array('bool' => true);

?>
