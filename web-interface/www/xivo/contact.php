<?php

require_once('xivo.php');

if($_USR->mk_active() === false)
	$_QRY->go($_HTML->url('xivo/logoff'));

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));

$_HTML->set_struct('xivo/contact');
$_HTML->display('simple');

?>
