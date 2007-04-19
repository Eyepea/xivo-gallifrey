<?php

require_once('xivo.php');

if(xivo_user::is_valid() === true)
	xivo_go($_HTML->url('xivo'));

if(isset($_QR['login'],$_QR['password']) === true && $_USR->load_by_authent($_QR['login'],$_QR['password']) === true)
	xivo_go($_HTML->url('xivo'));

$_HTML->set_struct('home/login');
$_HTML->display('center');

?>
