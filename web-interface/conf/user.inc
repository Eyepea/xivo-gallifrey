<?php

define('XIVO_USER__VISITOR',0);
define('XIVO_USER__USER',1);
define('XIVO_USER__ADMIN',2);
define('XIVO_USER__ROOT',4);

define('XIVO_USER_CLASS_PRE','_usr_');

$array = array();

$array['type'] = array(
	XIVO_USER__VISITOR	=> array('meta' => 'visitor',
					'const'	=> 'XIVO_USER__VISITOR'),
	XIVO_USER__USER		=> array('meta' => 'user',
					'const' => 'XIVO_USER__USER'),
	XIVO_USER__ADMIN		=> array('meta' => 'admin',
					'const' => 'XIVO_USER__ADMIN'),
	XIVO_USER__ROOT		=> array('meta' => 'root',
					'const' => 'XIVO_USER__ROOT'));

// $array['admin'] = 'admin';

$array['option'] = array(
	'time_cache'		=> 60,
	'ip_check'		=> false);

?>
