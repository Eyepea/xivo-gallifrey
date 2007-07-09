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

$array['element']['silent'] = array();
$array['element']['silent']['value'] = array(0,1);
$array['element']['silent']['default'] = 0;
$array['element']['silent']['set'] = true;

$array['filter'] = array();
$array['filter']['agentid'] = array('cast' => 'uint');
$array['filter']['numgroup'] = array('cast' => 'uint','between' => array(1,63));
$array['filter']['number'] = array('preg' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['passwd'] = array('set' => false,'chk' => 2,'preg' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['firstname'] = array('set' => false,'chk' => 2,'maxlen' => 128,'preg' => '/^[^,\a\t\r\n\f\0\x0B\x1B]+$/');
$array['filter']['lastname'] = array('set' => false,'chk' => 2,'maxlen' => 128,'preg' => '/^[^,\a\t\r\n\f\0\x0B\x1B]+$/');
$array['filter']['silent'] = array('bool' => true);

?>
