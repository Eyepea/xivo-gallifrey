<?php

$array = array();

$array['app'] = array();
$array['app']['macro'] = array('name' => 'Macro');
$array['app']['answer'] = array('name' => 'Answer');
$array['app']['hangup'] = array('name' => 'Hangup');
$array['app']['pickup'] = array('name' => 'Pickup');
$array['app']['playback'] = array('name' => 'Playback');
$array['app']['set'] = array('name' => 'Set');
$array['app']['wait'] = array('name' => 'Wait');
$array['app']['record'] = array('name' => 'Record');
$array['app']['voicemailmain'] = array('name' => 'VoiceMailMain');

$array['element'] = array();

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = 'default';

$array['element']['priority'] = array();
$array['element']['priority']['default'] = 1;
$array['element']['priority']['setdef'] = true;

$array['element']['app'] = array();
$array['element']['app']['default'] = '';

$array['element']['appdata'] = array();
$array['element']['appdata']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['filter'] = array();
$array['filter']['exten'] = array('minlen' => 1,'maxlen' => 40,'cast' => 'graph');
$array['filter']['context'] = array('chk' => 2,'maxlen' => 39,'regexp' => '/^[a-z0-9_-]+$/');
$array['filter']['priority'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(1,255));
$array['filter']['app'] = array('minlen' => 1,'maxlen' => 128,'regexp' => '/^[a-z0-9#\*\.\/_@:-]+$/i');
$array['filter']['appdata'] = array('set' => false,'chk' => 2,'minlen' => 0,'maxlen' => 128,'cast' => 'sgraph','notag' => false);
$array['filter']['name'] = array('set' => false,'chk' => 2,'maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/');

?>
