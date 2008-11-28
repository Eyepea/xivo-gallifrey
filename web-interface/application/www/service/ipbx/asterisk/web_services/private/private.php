<?php

if(isset($_SERVER['REMOTE_ADDR']) === false || $_SERVER['REMOTE_ADDR'] !== '127.0.0.1')
{
	xivo::load_class('xivo_http');
	$http = new xivo_http();
	$http->set_status(403);
	$http->send(true);
}

?>
