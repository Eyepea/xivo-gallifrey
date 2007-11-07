<?php

define('XIVO_SESS_ENABLE',false);

require_once('xivo.php');

$ipbx = &$_SRE->get('ipbx');

$application = $_HTML->get_application('service/ipbx/'.$ipbx->get_name().'/web_services/',2);

if($application === false)
	die('XIVO-WEBI: Error/404');

die(include($application));

?>
