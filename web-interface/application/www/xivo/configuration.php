<?php

xivo::load_class('xivo_server',XIVO_PATH_OBJECT,null,false);
$_SVR = new xivo_server();

$userstat = $serverstat = array();
$userstat['enable'] = $serverstat['enable'] = 0;
$userstat['disable'] = $serverstat['disable'] = 0;

$userstat['enable'] = $_USR->get_nb(true);
$userstat['disable'] = $_USR->get_nb(false);
$userstat['total'] = $userstat['enable'] + $userstat['disable'];

$serverstat['enable'] = $_SVR->get_nb(null,false);
$serverstat['disable'] = $_SVR->get_nb(null,true);
$serverstat['total'] = $serverstat['enable'] + $serverstat['disable'];

$_HTML->assign('userstat',$userstat);
$_HTML->assign('serverstat',$serverstat);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');

$_HTML->set_bloc('main','xivo/configuration/index');
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
