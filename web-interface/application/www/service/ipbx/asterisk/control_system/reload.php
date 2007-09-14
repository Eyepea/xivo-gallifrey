<?php

require_once('xivo.php');

if($_HTML->chk_acl(true) === false)
	$_QRY->go($_HTML->url('xivo'));

$ami = &$ipbx->get_module('ami');
$ami->cmd('reload');
$ami->cmd('moh reload');

if(isset($_SERVER['HTTP_REFERER']) === true)
	$_QRY->go($_SERVER['HTTP_REFERER']);
else
	$_QRY->go($_HTML->url('service/ipbx'));

?>
