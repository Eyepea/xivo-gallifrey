<?php

$array = array();

$array['element'] = array();

$array['element']['exten'] = array();
$array['element']['exten']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['preprocess_subroutine'] = array();
$array['element']['preprocess_subroutine']['default'] = '';
$array['element']['preprocess_subroutine']['null'] = true;

$array['element']['faxdetectenable'] = array();
$array['element']['faxdetectenable']['value'] = array(0,1);
$array['element']['faxdetectenable']['default'] = 0;
$array['element']['faxdetectenable']['set'] = true;

$array['element']['faxdetecttimeout'] = array();
$array['element']['faxdetecttimeout']['value'] = range(0,30);
$array['element']['faxdetecttimeout']['default'] = 4;
$array['element']['faxdetecttimeout']['setdef'] = true;

$array['element']['faxdetectemail'] = array();
$array['element']['faxdetectemail']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;


$array['filter'] = array();
$array['filter']['exten'] = array('maxlen' => 40,'regexp' => '/^_?[0-9NXZ\*#\-\[\]]+[\.\!]?$/');
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['preprocess_subroutine'] = array('set' => false,'chk' => 2,'callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['faxdetectenable'] = array('bool' => true);
$array['filter']['faxdetecttimeout'] = array('set' => false,'chk' => 2,'cast' => 'uint','between' => array(0,30));
$array['filter']['faxdetectemail'] = array('set' => false,'chk' => 2,'maxlen' => 255,'callback' => 'xivo_filter::chk_email');
$array['filter']['commented'] = array('bool' => true);

?>
