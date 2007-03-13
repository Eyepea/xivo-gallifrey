<?php

require_once('xivo.php');

if($_HTML->chk_policy(true) === false)
	xivo_go($_HTML->url('xivo'));

xivo::load_class('socket');

$socket = new xivo_socket(array('address' => 'localhost','port' => 8080,'protocol' => 'tcp'));

if($socket->is_open() === true)
	$socket->write('reload');

if(isset($_SERVER['HTTP_REFERER']) === true)
	xivo_go($_SERVER['HTTP_REFERER']);
else
	xivo_go($_HTML->url('service/ipbx'));

?>
