<?php

require_once('xivo.php');

if($_HTML->chk_policy(true) === false)
	xivo_go($_HTML->url('xivo'));

xivo_socket('tcp://localhost',8080,'reload'."\n");

if(isset($_SERVER['HTTP_REFERER']) === true)
	xivo_go($_SERVER['HTTP_REFERER']);
else
	xivo_go($_HTML->url('service/ipbx'));

?>
