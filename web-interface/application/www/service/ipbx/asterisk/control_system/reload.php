<?php

require_once('xivo.php');

if($_HTML->chk_policy(true) === false)
	xivo_go($_HTML->url('xivo'));

$ami = &$ipbx->get_module('ami');
$ami->cmd('reload');
$ami->cmd('moh reload');

if(isset($_SERVER['HTTP_REFERER']) === true)
	xivo_go($_SERVER['HTTP_REFERER']);
else
	xivo_go($_HTML->url('service/ipbx'));

?>
