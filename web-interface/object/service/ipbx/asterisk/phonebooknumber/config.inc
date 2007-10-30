<?php

$array = array();
$array['element'] = array();

$array['element']['phonebookid'] = array();
$array['element']['phonebookid']['default'] = 0;

$array['element']['number'] = array();
$array['element']['number']['default'] = '';

$array['element']['type'] = array();
$array['element']['type']['value'] = array('home','office','mobile','fax','other');
$array['element']['type']['default'] = 'office';

$array['filter'] = array();
$array['filter']['phonebookid'] = array('cast' => 'uint');
$array['filter']['number'] = array('minlen' => 1,'maxlen' => 40,'regexp' => '/^[0-9\*#]+$/');
$array['filter']['type'] = array('key' => array('home','office','mobile','fax','other'));

?>
