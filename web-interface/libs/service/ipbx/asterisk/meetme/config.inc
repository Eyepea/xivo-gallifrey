<?php

$array = array();

$array['element'] = array();

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['pin'] = array();
$array['element']['pin']['default'] = '';

$array['element']['admpin'] = array();
$array['element']['admpin']['default'] = '';

$array['filter']['number'] = array('preg' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['pin'] = array('set' => false,'chk' => 2,'preg' => '/^[0-9\*#]+$/','maxlen' => 40);
$array['filter']['admpin'] = array('set' => false,'chk' => 2,'preg' => '/^[0-9\*#]+$/','maxlen' => 40);

?>
