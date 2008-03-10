<?php

$array = array();
$array['element'] = array();

$array['element']['phonebookid'] = array();
$array['element']['phonebookid']['default'] = 0;

$array['element']['address1'] = array();
$array['element']['address1']['default'] = '';

$array['element']['address2'] = array();
$array['element']['address2']['default'] = '';

$array['element']['city'] = array();
$array['element']['city']['default'] = '';

$array['element']['state'] = array();
$array['element']['state']['default'] = '';

$array['element']['zipcode'] = array();
$array['element']['zipcode']['default'] = '';

$array['element']['country'] = array();
$array['element']['country']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('home','office','other');
$array['element']['type']['default'] = 'office';


$array['filter'] = array();
$array['filter']['phonebookid'] = array('cast' => 'ulongint');
$array['filter']['address1'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 30);
$array['filter']['address2'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 30);
$array['filter']['city'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['state'] = array('set' => false,'chk' => 2,'cast' => 'sgraph','maxlen' => 128);
$array['filter']['zipcode'] = array('set' => false,'chk' => 2,'maxlen' => 16,'regexp' => '/^[a-zA-Z0-9_-]+$/');
$array['filter']['country'] = array('set' => false,'chk' => 2,'minlen' => 2,'maxlen' => 2,'regexp' => '/^[A-Z]{2}$/');
$array['filter']['type'] = array('key' => array('home','office','other'));

?>
