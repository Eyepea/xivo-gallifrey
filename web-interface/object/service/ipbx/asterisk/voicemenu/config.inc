<?php

$array = array();
$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;


$array['filter'] = array();
$array['filter']['name'] = array('maxlen' => 29,'regexp' => '/^[a-z0-9_-]+$/i');
$array['filter']['number'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);
$array['filter']['commented'] = array('bool' => true);

?>
