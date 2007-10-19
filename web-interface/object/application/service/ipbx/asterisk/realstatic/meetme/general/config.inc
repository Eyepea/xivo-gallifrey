<?php

$array = array();

$array['element'] = array();
$array['element']['audiobuffer'] = array();
$array['element']['audiobuffer']['value'] = range(2,32);
$array['element']['audiobuffer']['default'] = 32;

$array['filter'] = array();
$array['filter']['audiobuffer'] = array('cast' => 'uint','between' => array(2,32));

?>
