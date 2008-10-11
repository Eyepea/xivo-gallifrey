<?php

$array = array();

$array['element'] = array();

$array['element']['groupid'] = array();
$array['element']['groupid']['default'] = '';

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['groups'] = array();
$array['element']['groups']['default'] = '';

$array['element']['deleted'] = array();
$array['element']['deleted']['value'] = array(0,1);
$array['element']['deleted']['default'] = 0;
$array['element']['deleted']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['filter'] = array();
$array['filter']['groupid'] = array('cast' => 'ulongint');
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['groups'] = array('set' => false,'chk' => 2,'regexp' => '/^(?:[0-9]{1,2}[,]?)*$/','maxlen' => 255);
$array['filter']['deleted'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
