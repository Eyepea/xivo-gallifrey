<?php

require_once('xivo.php');

$ipbx = &$_SRE->get('ipbx');
$service_name = $ipbx->get_name();

if(($control = $_HTML->get_control('service/ipbx/'.$service_name.'/sso')) !== false)
	die(include($control));

die();

?>
