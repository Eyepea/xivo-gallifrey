<?php

$array = array();

$array['element'] = array();
$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['filter'] = array();
$array['filter']['number'] = array('set' => false,'chk' => 2,'cast' => 'digit');
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 255);

?>
