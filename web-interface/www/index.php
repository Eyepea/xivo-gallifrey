<?php

if(($path_conf = realpath('../conf/')) === false)
	die('ERR : XIVO CONF DIRECTORY NOT FOUND');

$prependf_xivo = $path_conf.'/prepend.inc';

if(!(is_file($prependf_xivo) && is_readable($prependf_xivo)))
	die('ERR : XIVO PREPEND FILE NOT FOUND OR UNREADABLE');

require_once($prependf_xivo);

//phpinfo();

$appendf_xivo = XIVO_PATH_CONF.'/append.inc';

if(!(is_file($appendf_xivo) && is_readable($appendf_xivo)))
	trigger_error('XIVO APPEND FILE NOT FOUND OR UNREADABLE',E_USER_ERROR);

require_once($appendf_xivo);

?>
