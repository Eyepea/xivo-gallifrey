<?php

putenv('WAG_MODE=DEV');

if(($path_root = realpath(dirname(__FILE__).'/../')) === false)
	die('ERR : DOCUMENT ROOT DIRECTORY NOT FOUND');

if(!(is_dir($path_root) && is_readable($path_root)))
	die('ERR : DOCUMENT ROOT DIRECTORY NOT FOUND OR UNREADABLE');

define('XIVO_PATH_ROOT',$path_root);

$incf_xivo = XIVO_PATH_ROOT.'/conf/xivo.inc';

if(!(is_file($incf_xivo) && is_readable($incf_xivo)))
	die('ERR : XIVO INC FILE NOT FOUND OR UNREADABLE');

require_once($incf_xivo);

?>
