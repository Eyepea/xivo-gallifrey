<?php

require_once('xivo.php');

if(xivo_user::chk_acl(true) === false)
	$_QRY->go($_HTML->url('xivo'));

$ami = &$ipbx->get_module('ami');
$ami->cmd('restart now',true);
$ami->cmd('moh reload',true);
$ami->cmd('reload res_musiconhold.so');

if(isset($_SERVER['HTTP_REFERER']) === true)
	$_QRY->go($_SERVER['HTTP_REFERER'],false);
else
	$_QRY->go($_HTML->url('service/ipbx'));

?>
