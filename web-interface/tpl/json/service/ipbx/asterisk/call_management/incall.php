<?php

if(($data = xivo_json::encode($this->get_var('incall'))) === false)
	xivo_die('Error/500');

if($this->get_var('sum') === md5($data))
	xivo_die('no-update');

header(xivo_json::get_header());
die($data);

?>
