<?php

if(($data = xivo_json::encode($this->get_var('users'))) === false)
	xivo_die('Error/500');

if(isset($_QR['sum']) === true && $_QR['sum'] === md5($data))
	xivo_die('no-update');

header(xivo_json::get_header());
die($data);

?>
