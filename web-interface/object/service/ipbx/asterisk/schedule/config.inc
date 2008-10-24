<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['timebeg'] = array();
$array['element']['timebeg']['default'] = '*';
$array['element']['timebeg']['setdef'] = true;

$array['element']['timeend'] = array();
$array['element']['timeend']['default'] = '';
$array['element']['timeend']['null'] = true;

$array['element']['timehourbeg'] = array();
$array['element']['timehourbeg']['value'] = array('00','01','02','03','04','05','06','07','08','09',
						  10,11,12,13,14,15,16,17,18,19,
						  20,21,22,23);
$array['element']['timehourbeg']['default'] = '';
$array['element']['timehourbeg']['unset'] = true;

$array['element']['timeminbeg'] = array();
$array['element']['timeminbeg']['value'] = array('00','01','02','03','04','05','06','07','08','09',
						 10,11,12,13,14,15,16,17,18,19,
						 20,21,22,23,24,25,26,27,28,29,
						 30,31,32,33,34,35,36,37,38,39,
						 40,41,42,43,44,45,46,47,48,49,
						 50,51,52,53,54,55,56,57,58,59);
$array['element']['timeminbeg']['default'] = '';
$array['element']['timeminbeg']['unset'] = true;

$array['element']['timehourend'] = array();
$array['element']['timehourend']['value'] = array('00','01','02','03','04','05','06','07','08','09',
						  10,11,12,13,14,15,16,17,18,19,
						  20,21,22,23);
$array['element']['timehourend']['default'] = '';
$array['element']['timehourend']['unset'] = true;

$array['element']['timeminend'] = array();
$array['element']['timeminend']['value'] = array('00','01','02','03','04','05','06','07','08','09',
						 10,11,12,13,14,15,16,17,18,19,
						 20,21,22,23,24,25,26,27,28,29,
						 30,31,32,33,34,35,36,37,38,39,
						 40,41,42,43,44,45,46,47,48,49,
						 50,51,52,53,54,55,56,57,58,59);
$array['element']['timeminend']['default'] = '';
$array['element']['timeminend']['unset'] = true;

$array['element']['daynamebeg'] = array();
$array['element']['daynamebeg']['value'] = array('mon','tue','wed','thu','fri','sat','sun');
$array['element']['daynamebeg']['default'] = '*';
$array['element']['daynamebeg']['setdef'] = true;

$array['element']['daynameend'] = array();
$array['element']['daynameend']['value'] = array('mon','tue','wed','thu','fri','sat','sun');
$array['element']['daynameend']['default'] = '';
$array['element']['daynameend']['null'] = true;

$array['element']['daynumbeg'] = array();
$array['element']['daynumbeg']['value'] = range(1,31);
$array['element']['daynumbeg']['default'] = '*';
$array['element']['daynumbeg']['setdef'] = true;

$array['element']['daynumend'] = array();
$array['element']['daynumend']['value'] = range(1,31);
$array['element']['daynumend']['default'] = '';
$array['element']['daynumend']['null'] = true;

$array['element']['monthbeg'] = array();
$array['element']['monthbeg']['value'] = array('jan',
					       'feb',
					       'mar',
					       'apr',
					       'may',
					       'jun',
					       'jul',
					       'aug',
					       'sep',
					       'oct',
					       'nov',
					       'dec');
$array['element']['monthbeg']['default'] = '*';
$array['element']['monthbeg']['setdef'] = true;

$array['element']['monthend'] = array();
$array['element']['monthend']['value'] = array('jan',
					       'feb',
					       'mar',
					       'apr',
					       'may',
					       'jun',
					       'jul',
					       'aug',
					       'sep',
					       'oct',
					       'nov',
					       'dec');
$array['element']['monthend']['default'] = '';
$array['element']['monthend']['null'] = true;

$array['element']['publicholiday'] = array();
$array['element']['publicholiday']['value'] = array(0,1);
$array['element']['publicholiday']['default'] = 0;
$array['element']['publicholiday']['set'] = true;

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['filter'] = array();
$array['filter']['name'] = array('maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['timebeg'] = array('set' => false,'chk' => 2,'regexp' => '/^(?:(?:[0-1]?[0-9]|2[0-3])\:(?:0?[0-9]|[1-5][0-9])|\*)$/');
$array['filter']['timeend'] = array('set' => false,'chk' => 2,'regexp' => '/^(?:[0-1]?[0-9]|2[0-3])\:(?:0?[0-9]|[1-5][0-9])$/');
$array['filter']['daynamebeg'] = array('set' => false,'chk' => 2,'key' => array('mon','tue','wed','thu','fri','sat','sun'));
$array['filter']['daynameend'] = array('set' => false,'chk' => 2,'key' => array('mon','tue','wed','thu','fri','sat','sun'));
$array['filter']['daynumbeg'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,31));
$array['filter']['daynumend'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,31));
$array['filter']['monthbeg'] = array('set' => false,'chk' => 2,'key' => array('jan',
									      'feb',
									      'mar',
									      'apr',
									      'may',
									      'jun',
									      'jul',
									      'aug',
									      'sep',
									      'oct',
									      'nov',
									      'dec'));
$array['filter']['monthend'] = array('set' => false,'chk' => 2,'key' => array('jan',
									      'feb',
									      'mar',
									      'apr',
									      'may',
									      'jun',
									      'jul',
									      'aug',
									      'sep',
									      'oct',
									      'nov',
									      'dec'));
$array['filter']['publicholiday'] = array('bool' => true);
$array['filter']['commented'] = array('bool' => true);

?>
