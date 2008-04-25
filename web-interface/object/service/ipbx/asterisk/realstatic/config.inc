<?php

$array = array();

$array['element'] = array();

$array['element']['commented']['value'] = array(0,1);
$array['element']['commented']['default'] = 0;
$array['element']['commented']['set'] = true;

$array['element']['cat_metric'] = array();
$array['element']['cat_metric']['default'] = 0;

$array['element']['var_metric'] = array();
$array['element']['var_metric']['default'] = 0;

$array['element']['filename'] = array();
$array['element']['filename']['default'] = '';

$array['element']['category'] = array();
$array['element']['category']['default'] = '';

$array['element']['var_name'] = array();
$array['element']['var_name']['default'] = '';

$array['element']['var_val'] = array();
$array['element']['var_val']['default'] = '';
$array['element']['var_val']['null'] = true;


$array['filter'] = array();
$array['filter']['commented'] = array('bool' => true);
$array['filter']['filename'] = array('regexp' => '/^[a-zA-Z0-9_-]+\.(conf|ael)$/','maxlen' => 128);
$array['filter']['category'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['var_name'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['var_val'] = array('set' => false,'chk' => 2);

?>
