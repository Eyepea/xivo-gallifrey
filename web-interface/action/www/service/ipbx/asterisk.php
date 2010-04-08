<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$userstat = $groupstat = $queuestat = $meetmestat = $agentstat = $sipstat = $iaxstat = array();
$userstat['enable'] = $userstat['disable'] = $userstat['initialized'] = $userstat['total'] = 0;
$groupstat['enable'] = $groupstat['disable'] = $groupstat['total'] = 0;
$queuestat['enable'] = $queuestat['disable'] = $queuestat['total'] = 0;
$meetmestat['enable'] = $meetmestat['disable'] = $meetmestat['total'] = 0;
$voicemailstat['enable'] = $voicemailstat['disable'] = $voicemailstat['total'] = 0;
$agentstat['enable'] = $agentstat['disable'] = $agentstat['total'] = 0;
$sipstat['enable'] = $sipstat['disable'] = $sipstat['total'] = 0;
$iaxstat['enable'] = $iaxstat['disable'] = $iaxstat['total'] = 0;

$activecalls = 0;

if(($recvactivecalls = $ipbx->discuss('core show channels',true)) !== false
&& ($nb = count($recvactivecalls) - 1) > 0
&& ($pos = strpos($recvactivecalls[$nb],' ')) !== false
&& $pos !== 0)
	$activecalls = substr($recvactivecalls[$nb],0,$pos);

$appsip = &$ipbx->get_application('trunk',array('protocol' => XIVO_SRE_IPBX_AST_PROTO_SIP));

if(($enablesip = $appsip->get_nb(true,true)) !== false)
	$sipstat['enable'] = $enablesip;

if(($disablesip = $appsip->get_nb(true,false)) !== false)
	$sipstat['disable'] = $disablesip;
	
$sipstat['total'] = $sipstat['enable'] + $sipstat['disable'];

$appiax = &$ipbx->get_application('trunk',array('protocol' => XIVO_SRE_IPBX_AST_PROTO_IAX));

if(($enableiax = $appiax->get_nb(true,null)) !== false)
	$iaxstat['enable'] = $enableiax;

if(($disableiax = $appiax->get_nb(true,true)) !== false)
	$iaxstat['disable'] = $disableiax;
	
$iaxstat['total'] = $iaxstat['enable'] + $iaxstat['disable'];

$appagent = &$ipbx->get_application('agent',null,false);

if(($enableagent = $appagent->get_nb(null,false)) !== false)
	$agentstat['enable'] = $enableagent;

if(($disableagent = $appagent->get_nb(null,true)) !== false)
	$agentstat['disable'] = $disableagent;

$agentstat['total'] = $agentstat['enable'] + $agentstat['disable'];

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

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_TPL->set_var('userstat',$userstat);
$_TPL->set_var('groupstat',$groupstat);
$_TPL->set_var('queuestat',$queuestat);
$_TPL->set_var('meetmestat',$meetmestat);
$_TPL->set_var('agentstat',$agentstat);
$_TPL->set_var('voicemailstat',$voicemailstat);
$_TPL->set_var('sipstat',$sipstat);
$_TPL->set_var('iaxstat',$iaxstat);
$_TPL->set_var('activecalls',$activecalls);

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/index');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
