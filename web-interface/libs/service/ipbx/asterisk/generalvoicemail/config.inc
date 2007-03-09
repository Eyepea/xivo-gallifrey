<?php

$array = array();
$array['element'] = array();
$array['element']['maxmessage'] = array();
$array['element']['maxmessage']['default'] = 180;

$array['element']['minmessage'] = array();
$array['element']['minmessage']['default'] = 3;

$array['element']['maxisilence'] = array();
$array['element']['maxisilence']['default'] = 0;

$array['element']['review'] = array();
$array['element']['review']['value'] = array('no','yes');
$array['element']['review']['default'] = 'yes';
$array['element']['review']['set'] = true;

$array['element']['serveremail'] = array();
$array['element']['serveremail']['default'] = 'voicemail@xivo';

$array['element']['fromstring'] = array();
$array['element']['fromstring']['default'] = 'XIVO PBX';

$array['element']['maxmsg'] = array();
$array['element']['maxmsg']['default'] = 100;

$array['filter'] = array();
$array['filter']['maxmessage'] = array('cast' => 'uint','between' => array(1,500));
$array['filter']['minmessage'] = array('cast' => 'uint','between' => array(1,200));
$array['filter']['maxisilence'] = array('cast' => 'uint','between' => array(0,30));
$array['filter']['review'] = array('bool' => true);
$array['filter']['serveremail'] = array('eregi' => '^[a-z0-9@\-_\.]+$','maxlen' => 128);
$array['filter']['fromstring'] = array('eregi' => '^[a-z0-9\-_\.]+$','maxlen' => 128);
$array['filter']['maxmsg'] = array('cast' => 'uint','between' => array(1,500));
$array['filter']['emailsubject'] = array('maxlen' => 512);
$array['filter']['emailbody'] = array('maxlen' => 1000);

?>
