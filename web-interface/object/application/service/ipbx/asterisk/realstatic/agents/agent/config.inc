<?php

$array = array();

$array['element'] = array();

$array['element']['numgroup'] = array();
$array['element']['numgroup']['default'] = '';

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['passwd'] = array();
$array['element']['passwd']['default'] = '';

$array['element']['firstname'] = array();
$array['element']['firstname']['default'] = '';

$array['element']['lastname'] = array();
$array['element']['lastname']['default'] = '';


$array['filter'] = array();
$array['filter']['numgroup'] = array('cast' => 'uint','between' => array(1,63));
$array['filter']['number'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['passwd'] = array('set' => false,'chk' => 2,'minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['firstname'] = array('set' => false,'chk' => 2,'maxlen' => 128,'cast' => 'sgraph');
$array['filter']['lastname'] = array('set' => false,'chk' => 2,'maxlen' => 128,'cast' => 'sgraph');

?>
