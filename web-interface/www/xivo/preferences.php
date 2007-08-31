<?php

require_once('xivo.php');

if($_USR->mk_active() === false)
	xivo_go($_HTML->url('xivo/logout'));

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));

$_HTML->set_struct('xivo/preferences');
$_HTML->display('simple');

?>
