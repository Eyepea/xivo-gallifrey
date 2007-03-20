<?php

$array = array();

$array['element'] = array();

$array['element']['mode'] = array();
$array['element']['mode']['value'] = array('all','talk','listen');
$array['element']['mode']['default'] = 'all';

$array['element']['musiconhold'] = array();
$array['element']['musiconhold']['default'] = '';

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
$array['filter']['number'] = array('cast' => 'digit');
$array['filter']['mode'] = array('arrkey' => array('all','talk','listen'));
$array['filter']['exit'] = array('bool' => true);
$array['filter']['musiconhold'] = array('set' => false,'chk' => 2,'maxlen' => 128);
$array['filter']['quiet'] = array('bool' => true);
$array['filter']['record'] = array('bool' => true);
$array['filter']['video'] = array('bool' => true);

?>
