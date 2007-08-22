<?php

$array = array();

$array['element'] = array();

$array['element']['rangebeg'] = array();
$array['element']['rangebeg']['default'] = '';

$array['element']['rangeend'] = array();
$array['element']['rangeend']['default'] = '';

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['extenhash'] = array();
$array['element']['extenhash']['default'] = '';

$array['element']['extenmode'] = array();
$array['element']['extenmode']['value'] = array('extension','range');
$array['element']['extenmode']['default'] = 'extension';
$array['element']['extension']['setdef'] = true;

$array['element']['context'] = array();
$array['element']['context']['default'] = '';


$array['filter'] = array();
$array['filter']['rangebeg'] = array('set' => false,'chk' => 2,'maxlen' => 40,'regexp' => '/^(0|[1-9][0-9]*)$/');
$array['filter']['rangeend'] = array('set' => false,'chk' => 2,'maxlen' => 40,'regexp' => '/^[1-9][0-9]*$/');
$array['filter']['exten'] = array('set' => false,'chk' => 2,'maxlen' => 40,'regexp' => '/^((?![_]?(\.|\!))|[_]?[0-9NXZ\*#\-\[\]]+(\.|\!)?)$/');
$array['filter']['extenhash'] = array('set' => false,'chk' => 2,'maxlen' => 40,'cast' => 'xdigit');
$array['filter']['extenmode'] = array('key' => array('extension','range'));
$array['filter']['context'] = array('set' => false,'chk' => 2,'nokey' => array('general','globals','hints','features','authentification','zonemessages'),'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 39);

?>
