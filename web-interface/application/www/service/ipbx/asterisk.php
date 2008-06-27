<?php

$userstat = $groupstat = $queuestat = $meetmestat = array();
$userstat['enable'] = $userstat['disable'] = $userstat['initialized'] = $userstat['total'] = 0;
$groupstat['enable'] = $groupstat['disable'] = $groupstat['total'] = 0;
$queuestat['enable'] = $queuestat['disable'] = $queuestat['total'] = 0;
$meetmestat['enable'] = $meetmestat['disable'] = $meetmestat['total'] = 0;
$voicemailstat['enable'] = $voicemailstat['disable'] = $voicemailstat['total'] = 0;

$activecalls = 0;

if(($recvactivecalls = $ipbx->discuss('core show channels',true)) !== false
&& ($nb = count($recvactivecalls) - 1) > 0
&& ($pos = strpos($recvactivecalls[$nb],' ')) !== false
&& $pos !== 0)
	$activecalls = substr($recvactivecalls[$nb],0,$pos);

$appuser = &$ipbx->get_application('user',null,false);

if(($enableuser = $appuser->get_nb(null,false)) !== false)
	$userstat['enable'] = $enableuser;

if(($disableuser = $appuser->get_nb(null,true)) !== false)
	$userstat['disable'] = $disableuser;

if(($initializeduser = $appuser->get_nb(null,null,true)) !== false)
	$userstat['initialized'] = $initializeduser;

$userstat['total'] = $userstat['enable'] + $userstat['disable'];

$appgroup = &$ipbx->get_application('group',null,false);

if(($enablegroup = $appgroup->get_nb(null,false)) !== false)
	$groupstat['enable'] = $enablegroup;

if(($disablegroup = $appgroup->get_nb(null,true)) !== false)
	$groupstat['disable'] = $disablegroup;

$groupstat['total'] = $groupstat['enable'] + $groupstat['disable'];

$appqueue = &$ipbx->get_application('queue',null,false);

if(($enablequeue = $appqueue->get_nb(null,false)) !== false)
	$queuestat['enable'] = $enablequeue;

if(($disablequeue = $appqueue->get_nb(null,true)) !== false)
	$queuestat['disable'] = $disablequeue;

$queuestat['total'] = $queuestat['enable'] + $queuestat['disable'];

$appmeetme = &$ipbx->get_application('meetme',null,false);

if(($enablemeetme = $appmeetme->get_nb(null,false)) !== false)
	$meetmestat['enable'] = $enablemeetme;

if(($disablemeetme = $appmeetme->get_nb(null,true)) !== false)
	$meetmestat['disable'] = $disablemeetme;

$meetmestat['total'] = $meetmestat['enable'] + $meetmestat['disable'];

$appvoicemail = &$ipbx->get_application('voicemail',null,false);

if(($enablevoicemail = $appvoicemail->get_nb(null,false)) !== false)
	$voicemailstat['enable'] = $enablevoicemail;

if(($disablevoicemail = $appvoicemail->get_nb(null,true)) !== false)
	$voicemailstat['disable'] = $disablevoicemail;

$voicemailstat['total'] = $voicemailstat['enable'] + $voicemailstat['disable'];

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_var('userstat',$userstat);
$_HTML->set_var('groupstat',$groupstat);
$_HTML->set_var('queuestat',$queuestat);
$_HTML->set_var('meetmestat',$meetmestat);
$_HTML->set_var('voicemailstat',$voicemailstat);
$_HTML->set_var('activecalls',$activecalls);

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/index');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
