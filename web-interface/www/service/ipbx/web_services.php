<?php

define('XIVO_SESS_ENABLE',false);

require_once('xivo.php');

$ipbx = &$_SRE->get('ipbx');

$application = $_HTML->get_application('service/ipbx/'.$ipbx->get_name().'/web_services/',2);

if($application === false)
{
	xivo::load_class('xivo_http');
	$http = new xivo_http();
	$http->set_status(404);
	$http->send(true);
}

die(include($application));

?>
