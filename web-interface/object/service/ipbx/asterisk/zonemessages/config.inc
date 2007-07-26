<?php

$array = array();
$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['timezone'] = array();
$array['element']['timezone']['default'] = 'Europe/Paris';

$array['element']['msg_format'] = array();
$array['element']['msg_format']['default'] = '';

$array['filter'] = array();
$array['filter']['name'] = array('minlen' => 1,'maxlen' => 80,'preg' => '/^[a-z0-9_-]+$/i');
$array['filter']['timezone'] = array('minlen' => 9,'maxlen' => 30,'preg' => '/^[A-Z][a-z]{3,9}\/[a-zA-Z\/_-]{4,22}$/');
$array['filter']['msg_format'] = array('minlen' => 1,'maxlen' => 512,'preg' => '/^[0-9a-z\'\/ _-]+$/i');

?>
