<?php

$array = array();

$array['element'] = array();

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['filter'] = array();
$array['filter']['exten'] = array('maxlen' => 40,'regexp' => '/^[_]?[0-9NXZ\*#\-\[\]]+[\.\!]?$/');
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['commented'] = array('bool' => true);

?>
