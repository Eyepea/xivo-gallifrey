<?php

require_once('xivo.php');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));

if($_USR->mk_active() === false)
	xivo_go($_HTML->url('xivo/logout'));

$_HTML->set_struct('xivo/index');
$_HTML->display('simple');

?>
