<?php

$array = array();
$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['passwd'] = array();
$array['element']['passwd']['default'] = '';

$array['element']['authorization'] = array();
$array['element']['authorization']['value'] = array(0 => 'deny',1 => 'permit');
$array['element']['authorization']['default'] = 0;
$array['element']['authorization']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['filter'] = array();
$array['filter']['name'] = array('maxlen' => 128,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['passwd'] = array('set' => false,'chk' => 2,'maxlen' => 40,'regexp' => '/^[0-9#\*]+$/');
$array['filter']['authorization'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);
$array['filter']['commented'] = array('bool' => true);

?>
