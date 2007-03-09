<?php

$array = array();

$array['option'] = array();
$array['option']['dir'] = array();
$array['option']['dir']['name'] = '/^[a-z0-9_-]{1,50}$/i';

$array['option']['file'] = array();
$array['option']['file']['name'] = '/^[a-z0-9_-]{1,50}\.(mp3|wav|ogg)$/i';
$array['option']['file']['size'] = 2000000;
$array['option']['file']['extension'] = array();
$array['option']['file']['extension']['mp3'] = true;
$array['option']['file']['extension']['wav'] = true;
$array['option']['file']['extension']['ogg'] = true;

$array['element'] = array();

$array['element']['category'] = array();
$array['element']['category']['default'] = '';

$array['element']['mode'] = array();
$array['element']['mode']['value'] = array('files','custom','quietmp3','mp3','mp3nb','quietmp3nb');
$array['element']['mode']['default'] = 'files';

$array['element']['directory'] = array();
$array['element']['directory']['default'] = '';

$array['element']['application'] = array();
$array['element']['application']['default'] = '/usr/bin/madplay --mono -a -10 -R 8000 --output=raw:-';
$array['element']['application']['disable'] = true;

$array['element']['random'] = array();
$array['element']['random']['value'] = array('no','yes');
$array['element']['random']['default'] = 'no';
$array['element']['random']['set'] = true;

$array['element']['filename'] = array();
$array['element']['filename']['unset'] = true;

$array['filter'] = array();
$array['filter']['category'] = array('preg' => '/^[a-z0-9_-]+$/i','minlen' => 1,'maxlen' => 50);
$array['filter']['mode'] = array('arrkey' => array('files','custom','quietmp3','mp3','mp3nb','quietmp3nb'));
$array['filter']['application'] = array('set' => false,'chk' => 2,'maxlen' => 128);
$array['filter']['random'] = array('set' => false,'bool' => true);
$array['filter']['filename'] = array('set' => false,'preg' => '/^[a-z0-9_-]{1,50}\.(mp3|wav|ogg)$/i');

?>
