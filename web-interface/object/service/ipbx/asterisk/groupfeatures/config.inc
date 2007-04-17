<?php

$array = array();

$array['element'] = array();
$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['filter'] = array();
$array['filter']['number'] = array('set' => false,'chk' => 2,'preg' => '/^[0-9\*#]+$/','maxlen' => 40);
$array['filter']['name'] = array('preg' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['context'] = array('set' => false,'chk' => 2,'unarrkey' => array('general','globals'),'preg' => '/^[a-z0-9_-]+$/','maxlen' => 80);

?>
