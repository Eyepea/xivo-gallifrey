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
$array['element']['context']['setdef'] = true;

$array['element']['mailbox'] = array();
$array['element']['mailbox']['default'] = '';

$array['filter'] = array();
$array['filter']['fullname'] = array('cast' => 'sgraph','maxlen' => 150);
$array['filter']['password'] = array('regexp' => '/^[0-9#\*]+$/','maxlen' => 5);
$array['filter']['email'] = array('set' => false,'chk' => 2,'func' => 'xivo_filter::chk_email');
$array['filter']['attach'] = array('bool' => true);
$array['filter']['delete'] = array('bool' => true);
$array['filter']['context'] = array('set' => false,'chk' => 2,'nokey' => array('general','global','globals','hints','features','zonemessages'),'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 39);
$array['filter']['mailbox'] = array('minlen' => 1,'maxlen' => 40,'cast' => '/^[0-9#\*]+$/');

?>
