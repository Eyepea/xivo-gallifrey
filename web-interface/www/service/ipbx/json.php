<?php

define('XIVO_SESS_ENABLE',false);
define('XIVO_TPL_SPACE','json');

require_once('xivo.php');

$ipbx = &$_SRE->get('ipbx');

$application = $_HTML->get_application('service/ipbx/'.$ipbx->get_name().'/web_services/',3);

if($application === false)
	xivo_die('Error/404');

die(include($application));

?>
