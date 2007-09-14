<?php

require_once('xivo.php');

for($i = 0;$i < 256;$i++)
{
	xivo_var_dump(ctype_graph(chr($i)),chr($i));
}

die();

if(xivo_user::is_valid() === true)
	$_QRY->go($_HTML->url('xivo'));

if(isset($_QR['login'],$_QR['password']) === true
&& $_USR->load_by_authent($_QR['login'],$_QR['password']) === true)
	$_QRY->go($_HTML->url('xivo'));

$_HTML->set_struct('home/login');
$_HTML->display('center');

?>
