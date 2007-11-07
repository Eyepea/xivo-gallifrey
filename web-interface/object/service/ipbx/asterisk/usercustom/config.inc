<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';
$array['element']['name']['null'] = true;

$array['element']['interface'] = array();
$array['element']['interface']['default'] = '';

$array['element']['commented'] = array();
$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;


$array['filter'] = array();
$array['filter']['name'] = array('set' => false,'chk' => 0);
$array['filter']['interface'] = array('regexp' => '/^[a-z0-9#\*\.\/_@:-]+$/','minlen' => 1,'maxlen' => 128);
$array['filter']['commented'] = array('bool' => true);

?>
