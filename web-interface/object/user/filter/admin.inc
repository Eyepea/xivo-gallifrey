<?php

$array['admin'] = array();
$array['admin']['add'] = array();
$array['admin']['add']['login'] = array('minlen' => 4,'maxlen' => 64,'regexp' => '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=%]+$/');
$array['admin']['add']['passwd'] = array('minlen' => 4,'maxlen' => 64,'regexp' => '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['admin']['add']['valid'] = array('bool' => true);
$array['admin']['add']['meta'] = array('key' => array('admin'));

$array['admin']['edit'] = array();
$array['admin']['edit']['login'] = array('set' => false,'chk' => 0);
$array['admin']['edit']['passwd'] = array('minlen' => 4,'maxlen' => 64,'regexp' => '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['admin']['edit']['valid'] = array('bool' => true);
$array['admin']['edit']['meta'] = array('key' => array('admin'));

?>
