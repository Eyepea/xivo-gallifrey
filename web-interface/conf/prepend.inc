<?php

putenv('WAG_MODE=DEV');

if(($path_root = realpath(dirname(__FILE__).'/../')) === false)
	die('ERR : DOCUMENT ROOT DIRECTORY NOT FOUND');

if(!(is_dir($path_root) === true && is_readable($path_root) === true))
	die('ERR : DOCUMENT ROOT DIRECTORY NOT FOUND OR UNREADABLE');

define('XIVO_PATH_ROOT',$path_root);

$incf_xivo = XIVO_PATH_ROOT.'/conf/xivo.inc';

if(!(is_file($incf_xivo) === true && is_readable($incf_xivo) === true))
	die('ERR : XIVO INC FILE NOT FOUND OR UNREADABLE');

require_once($incf_xivo);

require_once(XIVO_PATH_LIBS.'/i18n.inc');



?>
