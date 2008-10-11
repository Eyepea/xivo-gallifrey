<?php

$array = array();

$array['exten'] = array();
$array['exten']['parkext'] = true;
$array['exten']['pickupexten'] = true;

$array['sound'] = array();
$array['sound']['courtesytone'] = true;
$array['sound']['xfersound'] = true;
$array['sound']['xferfailsound'] = true;

$array['element'] = array();
$array['element']['parkext'] = array();
$array['element']['parkext']['default'] = 700;

$array['element']['context'] = array();
$array['element']['context']['default'] = 'parkedcalls';
$array['element']['context']['forcedef'] = true;

$array['element']['parkingtime'] = array();
$array['element']['parkingtime']['value'] = array(15,30,45,60,75,90,105,120,
						  135,150,165,180,195,210,225,
						  240,255,270,285,300);
$array['element']['parkingtime']['default'] = 45;

$array['element']['parkpos'] = array();
$array['element']['parkpos']['default'] = '701-750';

$array['element']['parkfindnext'] = array();
$array['element']['parkfindnext']['value'] = array('no','next');
$array['element']['parkfindnext']['default'] = 'no';
$array['element']['parkfindnext']['set'] = true;

$array['element']['adsipark'] = array();
$array['element']['adsipark']['value'] = array('no','yes');
$array['element']['adsipark']['default'] = 'no';
$array['element']['adsipark']['set'] = true;

$array['element']['transferdigittimeout'] = array();
$array['element']['transferdigittimeout']['value'] = range(1,30);
$array['element']['transferdigittimeout']['default'] = 3;

$array['element']['featuredigittimeout'] = array();
$array['element']['featuredigittimeout']['value'] = array(1,500,1000,1500,2000,2500,3000,3500,4000);
$array['element']['featuredigittimeout']['default'] = 500;

$array['element']['courtesytone'] = array();
$array['element']['courtesytone']['default'] = '';
$array['element']['courtesytone']['null'] = true;

$array['element']['xfersound'] = array();
$array['element']['xfersound']['default'] = '';
$array['element']['xfersound']['null'] = true;

$array['element']['xferfailsound'] = array();
$array['element']['xferfailsound']['default'] = '';
$array['element']['xferfailsound']['null'] = true;

$array['element']['pickupexten'] = array();
$array['element']['pickupexten']['default'] = '*8';
 

$array['filter'] = array();
$array['filter']['parkext'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['context'] = array('set' => false);
$array['filter']['parkingtime'] = array('key' => array(15,30,45,60,75,90,105,120,
						       135,150,165,180,195,210,225,
						       240,255,270,285,300));
$array['filter']['parkpos'] = array('regexp' => '/^[0-9]{1,5}-[0-9]{1,5}$/','minlen' => 3,'maxlen' => 11);
$array['filter']['parkfindnext'] = array('bool' => true);
$array['filter']['adsipark'] = array('bool' => true);
$array['filter']['transferdigittimeout'] = array('cast' => 'uint','between' => array(1,30));
$array['filter']['featuredigittimeout'] = array('key' => array(1,500,1000,1500,2000,2500,3000,3500,4000));
$array['filter']['courtesytone'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');
$array['filter']['xfersound'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');
$array['filter']['xferfailsound'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');
$array['filter']['pickupexten'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);

?>
