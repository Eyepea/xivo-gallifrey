<?php

$array = array();

$array['element'] = array();

$array['element']['name'] = array();
$array['element']['name']['default'] = '';

$array['element']['login'] = array();
$array['element']['login']['default'] = '';
$array['element']['login']['null'] = true;

$array['element']['passwd'] = array();
$array['element']['passwd']['default'] = '';
$array['element']['passwd']['null'] = true;

$array['element']['host'] = array();
$array['element']['host']['default'] = '';
$array['element']['host']['null'] = true;

$array['element']['disable'] = array();
$array['element']['disable']['value'] = array(0,1);
$array['element']['disable']['default'] = 0;
$array['element']['disable']['set'] = true;

$array['element']['description'] = array();
$array['element']['description']['default'] = '';


$array['filter'] = array();
$array['filter']['name'] = array('regexp' => '/^[a-z0-9_\.-]+$/','minlen' => 1,'maxlen' => 64);
$array['filter']['login'] = array('set'		=> false,
				  'chk'		=> 2,
				  'minlen'	=> 4,
				  'maxlen'	=> 64,
				  'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=%]+$/');
$array['filter']['passwd'] = array('set'	=> false,
				   'chk'	=> 2,
				   'minlen'	=> 4,
				   'maxlen'	=> 64,
				   'regexp'	=> '/^[a-zA-Z0-9-\._~\!\$&\'\(\)\*\+,;=:%]+$/');
$array['filter']['host'] = array('set' => false,'chk' => 2,'callback' => 'xivo_filter::chk_host_subnet_ipv4_strict');
$array['filter']['disable'] = array('bool' => true);
$array['filter']['description'] = array('set' => false,'chk' => 2,'maxlen' => 1000,'cast' => 'sgraph_crlf_tab','eol' => true);

?>
