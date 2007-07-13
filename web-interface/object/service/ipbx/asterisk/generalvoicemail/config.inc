<?php

$array = array();
$array['element'] = array();
$array['element']['maxmessage'] = array();
$array['element']['maxmessage']['default'] = 180;

$array['element']['minmessage'] = array();
$array['element']['minmessage']['default'] = 2;

$array['element']['maxsilence'] = array();
$array['element']['maxsilence']['default'] = 1;

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

$array['element']['charset'] = array();
$array['element']['charset']['default'] = 'UTF-8';

$array['filter'] = array();
$array['filter']['maxmessage'] = array('cast' => 'uint','between' => array(1,500));
$array['filter']['minmessage'] = array('cast' => 'uint','between' => array(1,200));
$array['filter']['maxsilence'] = array('cast' => 'uint','between' => array(0,30));
$array['filter']['review'] = array('bool' => true);
$array['filter']['serveremail'] = array('preg' => '/^[a-z0-9@\-_\.]+$/i','maxlen' => 128);
$array['filter']['fromstring'] = array('preg' => '/^[a-z0-9\-_\. ]+$/i','maxlen' => 128);
$array['filter']['maxmsg'] = array('cast' => 'uint','between' => array(1,500));
$array['filter']['emailsubject'] = array('maxlen' => 128);
$array['filter']['emailbody'] = array('maxlen' => 1000,'eol' => true);

?>
