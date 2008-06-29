<?php

$array = array();

$array['categoryevent'] = array();
$array['categoryevent']['callfilter'] = array('noanswer');
$array['categoryevent']['group'] = array('noanswer','congestion','busy','chanunavail');
$array['categoryevent']['incall'] = array('answer');
$array['categoryevent']['queue'] = array('noanswer','congestion','busy','chanunavail');
$array['categoryevent']['schedule'] = array('inschedule','outschedule');
$array['categoryevent']['user'] = array('noanswer','congestion','busy','chanunavail');


$array['element'] = array();

$array['element']['event'] = array();
$array['element']['event']['value'] = array('answer',
					    'noanswer',
					    'congestion',
					    'busy',
					    'chanunavail',
					    'inschedule',
					    'outschedule');
$array['element']['event']['default'] = '';

$array['element']['category'] = array();
$array['element']['category']['value'] = array('callfilter','group','incall','queue','schedule','user');
$array['element']['category']['default'] = '';

$array['element']['categoryval'] = array();
$array['element']['categoryval']['default'] = '';

$array['element']['action'] = array();
$array['element']['action']['value'] = array('none',
					     'endcall:busy',
					     'endcall:congestion',
					     'endcall:hangup',
					     'user',
					     'group',
					     'queue',
					     'meetme',
					     'voicemail',
					     'schedule',
					     'voicemenu',
					     'application:callbackdisa',
					     'application:disa',
					     'application:directory',
					     'application:faxtomail',
					     'application:voicemailmain',
					     'sound',
					     'custom');
$array['element']['action']['default'] = 'none';

$array['element']['actionarg1'] = array();
$array['element']['actionarg1']['default'] = '';

$array['element']['actionarg2'] = array();
$array['element']['actionarg2']['default'] = '';

$array['element']['actiontype'] = array();
$array['element']['actiontype']['value'] = array('none',
						 'endcall',
						 'user',
						 'group',
						 'queue',
						 'meetme',
						 'voicemail',
						 'schedule',
						 'voicemenu',
						 'application',
						 'sound',
						 'custom');
$array['element']['actiontype']['default'] = 'none';
$array['element']['actiontype']['unset'] = true;

$array['element']['endcall'] = array();
$array['element']['endcall']['value'] = array('hangup','busy','congestion');
$array['element']['endcall']['default'] = 'hangup';
$array['element']['endcall']['unset'] = true;

$array['element']['application'] = array();
$array['element']['application']['value'] = array('callbackdisa','disa','directory','faxtomail','voicemailmain');
$array['element']['application']['default'] = 'callbackdisa';
$array['element']['application']['unset'] = true;

$array['element']['linked'] = array();
$array['element']['linked']['value'] = array(0,1);
$array['element']['linked']['default'] = 0;
$array['element']['linked']['set'] = true;


$array['filter'] = array();
$array['filter']['event'] = array('key' => array('answer',
						 'noanswer',
						 'congestion',
						 'busy',
						 'chanunavail',
						 'inschedule',
						 'outschedule'));
$array['filter']['category'] = array('key' => array('callfilter','group','incall','queue','schedule','user'));
$array['filter']['categoryval'] = array('maxlen' => 128,'cast' => 'sgraph');
$array['filter']['action'] = array('key' => array('none',
				 		  'endcall:busy',
				 		  'endcall:congestion',
				 		  'endcall:hangup',
						  'user',
						  'group',
						  'queue',
						  'meetme',
						  'voicemail',
						  'schedule',
						  'voicemenu',
						  'application:callbackdisa',
						  'application:disa',
						  'application:directory',
						  'application:faxtomail',
						  'application:voicemailmain',
						  'sound',
						  'custom'));
$array['filter']['actionarg1'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');
$array['filter']['actionarg2'] = array('set' => false,'chk' => 2,'maxlen' => 255,'cast' => 'sgraph');
$array['filter']['linked'] = array('bool' => true);

?>
