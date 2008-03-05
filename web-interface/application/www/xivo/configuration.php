<?php

xivo::load_class('xivo_server',XIVO_PATH_OBJECT,null,false);
$_SVR = new xivo_server();

xivo::load_class('xivo_ldapserver',XIVO_PATH_OBJECT,null,false);
$_LDAPSVR = new xivo_ldapserver();

$userstat = $serverstat = $ldapserver = array();
$userstat['enable'] = $serverstat['enable'] = $ldapserverstat['enable'] = 0;
$userstat['disable'] = $serverstat['disable'] = $ldapserverstat['disable'] = 0;

if(($enableuser = $_USR->get_nb(true)) !== false)
	$userstat['enable'] = $enableuser;

if(($disableuser = $_USR->get_nb(false)) !== false)
	$userstat['disable'] = $disableuser;

$userstat['total'] = $userstat['enable'] + $userstat['disable'];

if(($enableserver = $_SVR->get_nb(null,false)) !== false)
	$serverstat['enable'] = $enableserver;

if(($disableserver = $_SVR->get_nb(null,true)) !== false)
	$serverstat['disable'] = $disableserver;

$serverstat['total'] = $serverstat['enable'] + $serverstat['disable'];

if(($enableldapserver = $_LDAPSVR->get_nb(null,false)) !== false)
	$ldapserverstat['enable'] = $enableldapserver;

if(($disableldapserver = $_LDAPSVR->get_nb(null,true)) !== false)
	$ldapserverstat['disable'] = $disableldapserver;

$ldapserverstat['total'] = $ldapserverstat['enable'] + $ldapserverstat['disable'];

$_HTML->set_var('userstat',$userstat);
$_HTML->set_var('serverstat',$serverstat);
$_HTML->set_var('ldapserverstat',$ldapserverstat);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');

$_HTML->set_bloc('main','xivo/configuration/index');
$_HTML->set_struct('xivo/configuration');
$_HTML->display('index');

?>
