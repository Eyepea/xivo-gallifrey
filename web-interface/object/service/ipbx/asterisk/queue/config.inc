<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['timeout'] = array();
$array['element']['timeout']['value'] = array(5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['timeout']['default'] = 30;

$array['filter'] = array();
$array['filter']['name'] = array('preg' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['timeout'] = array('arrkey' => array(5,10,15,20,25,30,35,40,45,50,55,60));

?>
