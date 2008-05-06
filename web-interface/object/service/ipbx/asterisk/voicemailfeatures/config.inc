<?php

$array = array();

$array['element'] = array();

$array['element']['voicemailid'] = array();
$array['element']['voicemailid']['default'] = 0;

$array['element']['skipcheckpass'] = array();
$array['element']['skipcheckpass']['value'] = array(0,1);
$array['element']['skipcheckpass']['default'] = 0;
$array['element']['skipcheckpass']['set'] = true;


$array['filter'] = array();
$array['filter']['voicemailid'] = array('cast' => 'ulongint');
$array['filter']['skipcheckpass'] = array('bool' => true);

?>
