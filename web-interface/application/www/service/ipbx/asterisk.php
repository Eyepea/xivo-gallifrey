<?php

$userstat = $groupstat = $queuestat = $meetmestat = array();
$userstat['enable'] = $userstat['disable'] = $userstat['initialized'] = $userstat['total'] = 0;
$groupstat['enable'] = $groupstat['disable'] = $groupstat['total'] = 0;
$queuestat['enable'] = $queuestat['disable'] = $queuestat['total'] = 0;
$meetmestat['enable'] = $meetmestat['disable'] = $meetmestat['total'] = 0;

$appuser = &$ipbx->get_application('user');

if(($enableuser = $appuser->get_nb(null,false)) !== false)
	$userstat['enable'] = $enableuser;

if(($disableuser = $appuser->get_nb(null,true)) !== false)
	$userstat['disable'] = $disableuser;

if(($initializeduser = $appuser->get_nb(null,null,true)) !== false)
	$userstat['initialized'] = $initializeduser;

$userstat['total'] = $userstat['enable'] + $userstat['disable'];

$appgroup = &$ipbx->get_application('group');

if(($enablegroup = $appgroup->get_nb(null,false)) !== false)
	$groupstat['enable'] = $enablegroup;

if(($disablegroup = $appgroup->get_nb(null,true)) !== false)
	$groupstat['disable'] = $disablegroup;

$groupstat['total'] = $groupstat['enable'] + $groupstat['disable'];

$appqueue = &$ipbx->get_application('queue');

if(($enablequeue = $appqueue->get_nb(null,false)) !== false)
	$queuestat['enable'] = $enablequeue;

if(($disablequeue = $appqueue->get_nb(null,true)) !== false)
	$queuestat['disable'] = $disablequeue;

$queuestat['total'] = $queuestat['enable'] + $queuestat['disable'];

$appmeetme = &$ipbx->get_application('meetme');

if(($enablemeetme = $appmeetme->get_nb(null,false)) !== false)
	$meetmestat['enable'] = $enablemeetme;

if(($disablemeetme = $appmeetme->get_nb(null,true)) !== false)
	$meetmestat['disable'] = $disablemeetme;

$meetmestat['total'] = $meetmestat['enable'] + $meetmestat['disable'];

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->assign('userstat',$userstat);
$_HTML->assign('groupstat',$groupstat);
$_HTML->assign('queuestat',$queuestat);
$_HTML->assign('meetmestat',$meetmestat);

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/index');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
