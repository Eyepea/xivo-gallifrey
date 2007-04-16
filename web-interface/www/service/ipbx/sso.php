<?php

require_once('xivo.php');

$ipbx = &$_SRE->get('ipbx');
$service_name = $ipbx->get_name();

if(($application = $_HTML->get_application('service/ipbx/'.$service_name.'/sso')) !== false)
	die(include($application));

die();

?>
