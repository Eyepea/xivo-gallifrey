<?php

require_once('xivo.php');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));

if($_USR->mk_active() === false)
	$_QRY->go($_HTML->url('xivo/logoff'));

$_HTML->set_struct('xivo/index');
$_HTML->display('simple');

?>
