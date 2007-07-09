<?php

$array = array();

$array['group_default'] = 1;

$array['element'] = array();

$array['element']['groupid'] = array();
$array['element']['groupid']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['groups'] = array();
$array['element']['groups']['default'] = '';

$array['element']['deleted'] = array();
$array['element']['deleted']['value'] = array(0,1);
$array['element']['deleted']['default'] = 0;
$array['element']['deleted']['set'] = true;

$array['filter'] = array();
$array['filter']['groupid'] = array('cast' => 'uint');
$array['filter']['name'] = array('preg' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['groups'] = array('set' => false,'chk' => 2,'preg' => '/^([0-9]{1,2}[,]?)*$/','maxlen' => 255);
$array['filter']['deleted'] = array('bool' => true);

?>
