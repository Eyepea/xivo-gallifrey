<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['meetmeid'] = array();
$array['element']['meetmeid']['default'] = '';

$array['element']['mode'] = array();
$array['element']['mode']['value'] = array('all','talk','listen');
$array['element']['mode']['default'] = 'all';

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = '';

$array['element']['context'] = array();
$array['element']['context']['default'] = '';

$array['element']['exit'] = array();
$array['element']['exit']['value'] = array(0,1);
$array['element']['exit']['default'] = 0;
$array['element']['exit']['set'] = true;

$array['element']['quiet'] = array();
$array['element']['quiet']['value'] = array(0,1);
$array['element']['quiet']['default'] = 0;
$array['element']['quiet']['set'] = true;

$array['element']['record'] = array();
$array['element']['record']['value'] = array(0,1);
$array['element']['record']['default'] = 0;
$array['element']['record']['set'] = true;

$array['element']['video'] = array();
$array['element']['video']['value'] = array(0,1);
$array['element']['video']['default'] = 0;
$array['element']['video']['set'] = true;

$array['filter'] = array();
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 128);
$array['filter']['meetmeid'] = array('cast' => 'uint');
$array['filter']['number'] = array('regexp' => '/^[0-9\*#]+$/','minlen' => 1,'maxlen' => 40);
$array['filter']['mode'] = array('key' => array('all','talk','listen'));
$array['filter']['exit'] = array('bool' => true);
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'maxlen' => 20);
$array['filter']['quiet'] = array('bool' => true);
$array['filter']['record'] = array('bool' => true);
$array['filter']['video'] = array('bool' => true);
$array['filter']['context'] = array('set' => false,'chk' => 2,'nokey' => array('general','globals','initconfig','hints','features'),'regexp' => '/^(?!macro-)[a-z0-9_-]+$/','maxlen' => 80);

?>
