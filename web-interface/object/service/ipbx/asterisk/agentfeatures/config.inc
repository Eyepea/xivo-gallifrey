<?php

$array = array();

$array['element'] = array();

$array['element']['agentid'] = array();
$array['element']['agentid']['default'] = '';

$array['element']['numgroup'] = array();
$array['element']['numgroup']['default'] = '';

$array['element']['firstname'] = array();
$array['element']['firstname']['default'] = '';

$array['element']['lastname'] = array();
$array['element']['lastname']['default'] = '';

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['passwd'] = array();
$array['element']['passwd']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['language'] = array();
$array['element']['language']['value'] = array('en','fr');
$array['element']['language']['default'] = 'en';

$array['element']['silent'] = array();
$array['element']['silent']['value'] = array(0,1);
$array['element']['silent']['default'] = 0;
$array['element']['silent']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';

$array['filter'] = array();
$array['filter']['agentid'] = array('cast' => 'ulongint');
$array['filter']['numgroup'] = array('cast' => 'uint','between' => array(1,63));
$array['filter']['number'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['passwd'] = array('set' => false,'chk' => 2,'regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['firstname'] = array('set' => false,'chk' => 2,'maxlen' => 128,'cast' => 'sgraph');
$array['filter']['lastname'] = array('set' => false,'chk' => 2,'maxlen' => 128,'cast' => 'sgraph');
$array['filter']['context'] = array('callback' => 'xivo_service_asterisk::chk_context');
$array['filter']['language'] = array('minlen' => 2,'maxlen' => 2,'key' => array('en','fr'));
$array['filter']['silent'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
