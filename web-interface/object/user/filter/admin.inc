<?php

$array['admin'] = array();
$array['admin']['add'] = array();
$array['admin']['add']['login'] = array('cast' => 'string','minlen' => 4,'maxlen' => 255);
$array['admin']['add']['passwd'] = array('cast' => 'string','minlen' => 4,'maxlen' => 255);
$array['admin']['add']['valid'] = array('arrkey' => array('true','false'));
$array['admin']['add']['meta'] = array('arrkey' => array('admin'));

$array['admin']['edit'] = array();
$array['admin']['edit']['login'] = array('set' => false);
$array['admin']['edit']['passwd'] = array('cast' => 'string','minlen' => 4,'maxlen' => 255);
$array['admin']['edit']['valid'] = array('arrkey' => array('true','false'));
$array['admin']['edit']['meta'] = array('arrkey' => array('admin'));

?>
