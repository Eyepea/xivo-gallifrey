<?php

require_once('xivo.php');

$ipbx = &$_SRE->get('ipbx');

if(($application = $_HTML->get_application('service/ipbx/'.$ipbx->get_name().'/sso')) !== false)
	include($application);

die();

?>
