<?php

$array['admin'] = array();
$array['admin']['add'] = array();
$array['admin']['add']['login'] = array('cast' => 'alnumpunct','minlen' => 4,'maxlen' => 64);
$array['admin']['add']['passwd'] = array('cast' => 'alnumpunct','minlen' => 4,'maxlen' => 64);
$array['admin']['add']['valid'] = array('bool' => true);
$array['admin']['add']['meta'] = array('key' => array('admin'));

$array['admin']['edit'] = array();
$array['admin']['edit']['login'] = array('set' => false);
$array['admin']['edit']['passwd'] = array('cast' => 'alnumpunct','minlen' => 4,'maxlen' => 64);
$array['admin']['edit']['valid'] = array('bool' => true);
$array['admin']['edit']['meta'] = array('key' => array('admin'));

?>
