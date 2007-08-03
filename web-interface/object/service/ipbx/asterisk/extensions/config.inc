<?php

$array = array();

$array['element'] = array();

$array['element']['context'] = array();
$array['element']['context']['default'] = 'default';

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['priority'] = array();
$array['element']['priority']['default'] = 1;

$array['element']['app'] = array();
$array['element']['app']['default'] = '';

$array['element']['appdata'] = array();
$array['element']['appdata']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['filter'] = array();
$array['filter']['context'] = array('regexp' => '/^[a-z0-9_-]+$/','chk' => 2,'maxlen' => 20);
$array['filter']['exten'] = array('minlen' => 1,'maxlen' => 20);
$array['filter']['priority'] = array('cast' => 'int');
$array['filter']['app'] = array('minlen' => 1,'maxlen' => 20);
$array['filter']['appdata'] = array('set' => false,'chk' => 2,'minlen' => 0,'maxlen' => 128,'notag' => false);
$array['filter']['name'] = array('set' => false,'chk' => 2,'maxlen' => 128);

?>
