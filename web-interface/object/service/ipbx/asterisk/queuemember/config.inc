<?php

$array['element'] = array();

$array['element']['queue_name'] = array();
$array['element']['queue_name']['default'] = '';

$array['element']['interface'] = array();
$array['element']['interface']['default'] = '';

$array['element']['call_limit'] = array();
$array['element']['call_limit']['default'] = 0;

$array['filter'] = array();
$array['filter']['queue_name'] = array('preg' => '/^[a-z0-9_\.-]+$/i','minlen' => 1,'maxlen' => 128);
$array['filter']['interface'] = array('minlen' => 1,'preg' => '/^[a-z0-9#\*\.\/_@-]+$/i','maxlen' => 128);
$array['filter']['call_limit'] = array('between' => array(0,30));
