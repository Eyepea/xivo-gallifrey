<?php

$array = array();

$array['applicationval'] = array();
$array['applicationval']['filter'] = array();
$array['applicationval']['filter']['faxtomail'] = array('callback' => 'xivo_filter::chk_email','maxlen' => 80);
$array['applicationval']['filter']['disa'] = array('callback' => 'xivo_service_asterisk::chk_passwd_context');
$array['applicationval']['filter']['directory'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['applicationval']['filter']['voicemail'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['applicationval']['filter']['callback'] = array('callback' => 'xivo_service_asterisk::chk_passwd_context');

$array['element'] = array();

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('endcall','user','group','queue','meetme','schedule','application','sound','custom');
$array['element']['type']['default'] = 'endcall';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['element']['endcall'] = array();
$array['element']['endcall']['value'] = array('hangup','busy','congestion');
$array['element']['endcall']['unset'] = true;

$array['element']['application'] = array();
$array['element']['application']['value'] = array('faxtomail','disa','directory','voicemail','callback');
$array['element']['application']['unset'] = true;

$array['element']['applicationval'] = array();
$array['element']['applicationval']['default'] = '';

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
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['type'] = array('key' => array('endcall','user','group','queue','meetme','schedule','application','sound','custom'));
$array['filter']['typeval'] = array('maxlen' => 255,'cast' => 'sgraph');
$array['filter']['applicationval'] = array('set' => false,'chk' => 2,'regexp' => '/^[a-z0-9_\-\.\|@]+$/i','maxlen' => 80);
$array['filter']['linked'] = array('bool' => true);
$array['filter']['commented'] = array('bool' => true);

?>
