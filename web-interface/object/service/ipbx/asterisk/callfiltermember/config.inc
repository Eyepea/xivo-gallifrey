<?php

$array = array();
$array['element'] = array();

$array['element']['callfilterid'] = array();
$array['element']['callfilterid']['default'] = 0;

$array['element']['type'] = array();
$array['element']['type']['value'] = array('user');
$array['element']['type']['default'] = '';

$array['element']['typeval'] = array();
$array['element']['typeval']['default'] = '';

$array['element']['ringseconds'] = array();
$array['element']['ringseconds']['value'] = array(0,5,10,15,20,25,30,35,40,45,50,55,60);
$array['element']['ringseconds']['default'] = 0;
$array['element']['ringseconds']['setdef'] = true;

$array['element']['priority'] = array();
$array['element']['priority']['default'] = 0;

$array['element']['bstype'] = array();
$array['element']['bstype']['value'] = array('boss','secretary');
$array['element']['bstype']['default'] = '';
$array['element']['bstype']['null'] = true;

$array['element']['active'] = array();
$array['element']['active']['value'] = array(0,1);
$array['element']['active']['default'] = 0;
$array['element']['active']['set'] = true;

$array['filter'] = array();
$array['filter']['callfilterid'] = array('cast' => 'uint');
$array['filter']['type'] = array('key' => array('user'));
$array['filter']['typeval'] = array('maxlen' => 128,'rexgep' => '/^[a-z0-9_-]+$/');
$array['filter']['ringseconds'] = array('set' => false,'chk' => 2,'key' => array(0,5,10,15,20,25,30,35,40,45,50,55,60));
$array['filter']['priority'] = array('cast' => 'uint','between' => array(0,16));
$array['filter']['bstype'] = array('set' => false,'chk' => 2,'key' => array('boss','secretary'));
$array['filter']['active'] = array('bool' => true);

?>
