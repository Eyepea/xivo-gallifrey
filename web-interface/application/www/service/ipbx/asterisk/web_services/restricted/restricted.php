<?php

xivo::load_class('xivo_http');
$http = new xivo_http();

if(isset($access) === false)
{
	$http->set_authent_basic('Access Restricted');
	$http->set_status(401);
	$http->send(true);
}
else if($access === false)
{
	$http->set_status(403);
	$http->send(true);
}

?>
