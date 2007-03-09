<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/extenfeatures');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
