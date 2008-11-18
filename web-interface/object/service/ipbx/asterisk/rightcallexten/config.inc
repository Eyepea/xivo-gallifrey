<?php

$array = array();
$array['element'] = array();

$array['element']['rightcallid'] = array();
$array['element']['rightcallid']['default'] = 0;

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['extenhash'] = array();
$array['element']['extenhash']['default'] = '';

$array['filter'] = array();
$array['filter']['rightcallid'] = array('cast' => 'ulongint');
$array['filter']['exten'] = array('set' => false,'chk' => 2,'maxlen' => 40,'regexp' => '/^_?[0-9NXZ\*#\-\[\]]+[\.\!]?$/');
$array['filter']['extenhash'] = array('set' => false,'chk' => 2,'maxlen' => 40,'cast' => 'xdigit');

?>
