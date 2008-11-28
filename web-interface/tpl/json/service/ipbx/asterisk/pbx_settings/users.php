<?php

xivo::load_class('xivo_http');
$http = new xivo_http();

if(($data = xivo_json::encode($this->get_var('users'))) === false)
{
	$http->set_status(500);
	$http->send(true);
}
else if($this->get_var('sum') === md5($data))
{
	$http->set_status(304);
	$http->send(true);
}

header(xivo_json::get_header());
die($data);

?>
