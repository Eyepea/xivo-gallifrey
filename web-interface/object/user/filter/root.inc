<?php

$array = array();
$array['root'] = array();
/*
$array['root']['add'] = array();
$array['root']['add']['login'] = array('cast' => 'string','minlen' => 4,'maxlen' => 255);
$array['root']['add']['password'] = array('cast' => 'string','minlen' => 4,'maxlen' => 255);
$array['root']['add']['valid'] = array('key' => array('true' => 1,'false' => 1));
$array['root']['add']['meta'] = array('key' => array('root' => 1));
*/
$array['root']['edit'] = array();
$array['root']['edit']['passwd'] = array('cast' => 'string','minlen' => 4,'maxlen' => 255);
//$array['root']['edit']['valid'] = array('key' => array('true' => 1,'false' => 1));
//$array['root']['edit']['meta'] = array('key' => array('root' => 1,'admin' => 1));
/*
$array['admin'] = array();
$array['admin']['add'] = array();
$array['admin']['add']['login'] = array('cast' => 'string','minlen' => 4,'maxlen' => 255);
$array['admin']['add']['password'] = array('cast' => 'string','minlen' => 4,'maxlen' => 255);
$array['admin']['add']['valid'] = array('key' => array('true' => 1,'false' => 1));
$array['admin']['add']['meta'] = array('key' => array('root' => 1));
*/
$array['admin']['edit'] = array();
$array['admin']['edit']['passwd'] = array('cast' => 'string','minlen' => 4,'maxlen' => 255);
$array['admin']['edit']['valid'] = array('bool' => true);
//$array['admin']['edit']['meta'] = array('key' => array('root' => 1,'admin' => 1));

?>
