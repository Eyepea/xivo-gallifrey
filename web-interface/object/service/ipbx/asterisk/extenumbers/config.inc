<?php

$array = array();

$array['element'] = array();

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['filter'] = array();
$array['filter']['number'] = array('minlen' => 1,'maxlen' => 80,'preg' => '/^[0-9\*#]+$/');
$array['filter']['context'] = array('unarrkey' => array('general','globals'),'preg' => '/^[a-z0-9_-]+$/','minlen' => 1,'maxlen' => 80);

?>
