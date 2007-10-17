<?php

$array = array();

$array['element'] = array();
$array['element']['blindxfer'] = array();
$array['element']['blindxfer']['default'] = '*1';

$array['element']['atxfer'] = array();
$array['element']['atxfer']['default'] = '*2';

$array['element']['automon'] = array();
$array['element']['automon']['default'] = '*3';

$array['element']['disconnect'] = array();
$array['element']['disconnect']['default'] = '*0';


$array['filter'] = array();
$array['filter']['blindxfer'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['atxfer'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['automon'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['disconnect'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);

?>
