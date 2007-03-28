<?php

$array = array();

$array['element'] = array();

$array['element']['fullname'] = array();
$array['element']['fullname']['default'] = '';

$array['element']['password'] = array();
$array['element']['password']['default'] = '';

$array['element']['email'] = array();
$array['element']['email']['default'] = '';

$array['element']['attach'] = array();
$array['element']['attach']['value'] = array('no','yes');
$array['element']['attach']['default'] = 'yes';
$array['element']['attach']['set'] = true;

$array['element']['delete'] = array();
$array['element']['delete']['value'] = array('no','yes');
$array['element']['delete']['default'] = 'no';
$array['element']['delete']['set'] = true;

$array['element']['context'] = array();
$array['element']['context']['default'] = 'default';

$array['element']['mailbox'] = array();
$array['element']['mailbox']['default'] = '';

$array['filter'] = array();
$array['filter']['fullname'] = array('maxlen' => 150);
$array['filter']['password'] = array('maxlen' => 5);
$array['filter']['email'] = array('func' => 'xivo_chk_email');
$array['filter']['attach'] = array('bool' => true);
$array['filter']['delete'] = array('bool' => true);
$array['filter']['context'] = array('chk' => 2,'set' => false,'maxlen' => 50);
$array['filter']['mailbox'] = array('minlen' => 1,'maxlen' => 11,'cast' => 'digit');

?>
