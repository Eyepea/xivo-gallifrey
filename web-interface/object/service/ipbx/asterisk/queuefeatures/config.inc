<?php

$array['element'] = array();

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['data_quality'] = array();
$array['element']['data_quality']['value'] = array(0,1);
$array['element']['data_quality']['default'] = 0;

$array['element']['hitting_callee'] = array();
$array['element']['hitting_callee']['value'] = array(0,1);
$array['element']['hitting_callee']['default'] = 0;

$array['element']['hitting_caller'] = array();
$array['element']['hitting_caller']['value'] = array(0,1);
$array['element']['hitting_caller']['default'] = 0;

$array['element']['retries'] = array();
$array['element']['retries']['value'] = array(0,1);
$array['element']['retries']['default'] = 0;

$array['element']['ring'] = array();
$array['element']['ring']['value'] = array(0,1);
$array['element']['ring']['default'] = 0;

$array['element']['transfer_user'] = array();
$array['element']['transfer_user']['value'] = array(0,1);
$array['element']['transfer_user']['default'] = 0;

$array['element']['transfer_call'] = array();
$array['element']['transfer_call']['value'] = array(0,1);
$array['element']['transfer_call']['default'] = 0;

$array['element']['write_caller'] = array();
$array['element']['write_caller']['value'] = array(0,1);
$array['element']['write_caller']['default'] = 0;

$array['element']['write_calling'] = array();
$array['element']['write_calling']['value'] = array(0,1);
$array['element']['write_calling']['default'] = 0;

$array['element']['url'] = array();
$array['element']['url']['default'] = '';

$array['element']['announceoverride'] = array();
$array['element']['announceoverride']['default'] = '';

$array['element']['timeout'] = array();
$array['element']['timeout']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['timeout']['default'] = 30;

$array['filter'] = array();
$array['filter']['number'] = array('set' => false,'chk' => 2,'preg' => '/^[0-9\*#]+$/','maxlen' => 40);
$array['filter']['name'] = array('preg' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['context'] = array('set' => false,'chk' => 2,'unarrkey' => array('general','globals','initconfig','hints','features'),'preg' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 80);
$array['filter']['data_quality'] = array('bool' => true);
$array['filter']['hitting_callee'] = array('bool' => true);
$array['filter']['hitting_caller'] = array('bool' => true);
$array['filter']['retries'] = array('bool' => true);
$array['filter']['ring'] = array('bool' => true);
$array['filter']['transfer_user'] = array('bool' => true);
$array['filter']['transfer_call'] = array('bool' => true);
$array['filter']['write_caller'] = array('bool' => true);
$array['filter']['write_calling'] = array('bool' => true);
$array['filter']['url'] = array('set' => false,'chk' => 2,'maxlen' => 256);
$array['filter']['announceoverride'] = array('set' => false,'chk' => 2,'maxlen' => 128);
$array['filter']['timeout'] = array('arrkey' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));

?>
