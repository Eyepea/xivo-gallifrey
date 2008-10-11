<?php

$array = array();

$array['element'] = array();

$array['element']['persistentagents'] = array();
$array['element']['persistentagents']['value'] = array('no','yes');
$array['element']['persistentagents']['default'] = 'yes';
$array['element']['persistentagents']['set'] = true;

$array['element']['multiplelogin'] = array();
$array['element']['multiplelogin']['value'] = array('no','yes');
$array['element']['multiplelogin']['default'] = 'yes';
$array['element']['multiplelogin']['set'] = true;

$array['filter'] = array();
$array['filter']['persistentagents'] = array('bool' => true);
$array['filter']['multiplelogin'] = array('bool' => true);

?>
