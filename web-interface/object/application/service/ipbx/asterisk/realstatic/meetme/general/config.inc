<?php

$array = array();

$array['element'] = array();
$array['element']['audiobuffers'] = array();
$array['element']['audiobuffers']['value'] = range(2,32);
$array['element']['audiobuffers']['default'] = 32;

$array['filter'] = array();
$array['filter']['audiobuffers'] = array('cast' => 'uint','between' => array(2,32));

?>
