<?php

if(($path_conf = realpath('../conf/')) === false)
	die('ERR : CONF DIRECTORY NOT FOUND');

$conff_xivo = $path_conf.'/prepend.inc';

if(!(is_file($conff_xivo) && is_readable($conff_xivo)))
	die('ERR : CONF LOAD FILE XIVO NOT FOUND OR UNREADABLE');

require_once($conff_xivo);

phpinfo();

?>
