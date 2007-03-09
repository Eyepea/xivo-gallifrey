<?php

require_once('xivo.php');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));

$_HTML->set_struct('xivo/contact');
$_HTML->display('simple');

?>
