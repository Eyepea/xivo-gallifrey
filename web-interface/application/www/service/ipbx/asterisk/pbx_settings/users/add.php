<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

$result = $fm_save = $error = null;

$allow = array();

$gmember = $qmember = $rightcall = array();
$gmember['list'] = $qmember['list'] = false;
$gmember['info'] = $qmember['info'] = false;
$gmember['slt'] = $qmember['slt'] = $rightcall['slt'] = array();

$appgroup = &$ipbx->get_application('group',null,false);

if(($groups = $appgroup->get_groups_list(null,
					 array('name' => SORT_ASC),
					 null,
					 true)) !== false)
	$gmember['list'] = $groups;

$appqueue = &$ipbx->get_application('queue',null,false);

if(($queues = $appqueue->get_queues_list(null,
					 array('name' => SORT_ASC),
					 null,
					 true)) !== false)
	$qmember['list'] = $queues;

$apprightcall = &$ipbx->get_application('rightcall',null,false);

$rightcall['list'] = $apprightcall->get_rightcalls_list(null,
							array('name' => SORT_ASC),
							null,
							true);

if(isset($_QR['fm_send']) === true
&& xivo_issa('protocol',$_QR) === true
&& xivo_issa('userfeatures',$_QR) === true
&& isset($_QR['protocol']['protocol']) === true)
{
	if($appuser->set_add($_QR,$_QR['protocol']['protocol']) === false
	|| $appuser->add() === false)
	{
		$fm_save = false;
		$result = $appuser->get_result();
		$result['dialaction'] = $appuser->get_dialaction_result();
		$result['phonefunckey'] = $appuser->get_phonefunckey_result();

		$error = $appuser->get_error();

		if(xivo_issa('protocol',$result) === true && isset($result['protocol']['allow']) === true)
			$allow = $result['protocol']['allow'];

		if(isset($_QR['userfeatures']['voicemailid']) === true
		&& $_QR['userfeatures']['voicemailid'] === 'add')
			$result['userfeatures']['voicemailid'] = 'add';
	}
	else
	{
		$ipbx->discuss('xivo[userlist,update]');
		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);
	}
}

xivo::load_class('xivo_sort');

if($gmember['list'] !== false && xivo_ak('groupmember',$result) === true)
{
	$gmember['slt'] = xivo_array_intersect_key($result['groupmember'],
						   $gmember['list'],
						   'groupfeaturesid');

	if($gmember['slt'] !== false)
	{
		$gmember['info'] = xivo_array_copy_intersect_key($result['groupmember'],
								 $gmember['slt'],
								 'groupfeaturesid');
		$gmember['list'] = xivo_array_diff_key($gmember['list'],$gmember['slt']);

		$groupsort = new xivo_sort(array('key' => 'name'));
		uasort($gmember['slt'],array(&$groupsort,'str_usort'));
	}
}

if($qmember['list'] !== false && xivo_ak('queuemember',$result) === true)
{
	$qmember['slt'] = xivo_array_intersect_key($result['queuemember'],
						   $qmember['list'],
						   'queuefeaturesid');

	if($qmember['slt'] !== false)
	{
		$qmember['info'] = xivo_array_copy_intersect_key($result['queuemember'],
								 $qmember['slt'],
								 'queuefeaturesid');
		$qmember['list'] = xivo_array_diff_key($qmember['list'],$qmember['slt']);

		$queuesort = new xivo_sort(array('key' => 'name'));
		uasort($qmember['slt'],array(&$queuesort,'str_usort'));
	}
}

if($rightcall['list'] !== false && xivo_ak('rightcall',$result) === true)
{
	$rightcall['slt'] = xivo_array_intersect_key($result['rightcall'],
						     $rightcall['list'],
						     'rightcallid');

	if($rightcall['slt'] !== false)
	{
		$rightcall['list'] = xivo_array_diff_key($rightcall['list'],$rightcall['slt']);

		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));
		uasort($rightcall['slt'],array(&$rightcallsort,'str_usort'));
	}
}

$element = $appuser->get_elements();

if(xivo_issa('allow',$element['protocol']['sip']) === true
&& xivo_issa('value',$element['protocol']['sip']['allow']) === true
&& empty($allow) === false)
{
	if(is_array($allow) === false)
		$allow = explode(',',$allow);

	$element['protocol']['sip']['allow']['value'] = array_diff($element['protocol']['sip']['allow']['value'],$allow);
}

if(xivo_issa('allow',$element['protocol']['iax']) === true
&& xivo_issa('value',$element['protocol']['iax']['allow']) === true
&& empty($allow) === false)
{
	if(is_array($allow) === false)
		$allow = explode(',',$allow);

	$element['protocol']['iax']['allow']['value'] = array_diff($element['protocol']['iax']['allow']['value'],$allow);
}

if(empty($result) === false)
{
	$result['protocol']['allow'] = $allow;

	if(xivo_issa('dialaction',$result) === false || empty($result['dialaction']) === true)
		$result['dialaction'] = null;

	if(xivo_issa('voicemail',$result) === false || empty($result['voicemail']) === true)
		$result['voicemail'] = null;

	if(xivo_issa('autoprov',$result) === false || empty($result['autoprov']) === true)
		$result['autoprov'] = null;
}
else
	$result = null;

$_TPL->set_var('info',$result);
$_TPL->set_var('error',$error);
$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('voicemail',$result['voicemail']);
$_TPL->set_var('dialaction',$result['dialaction']);
$_TPL->set_var('dialaction_from','user');
$_TPL->set_var('groups',$groups);
$_TPL->set_var('gmember',$gmember);
$_TPL->set_var('queues',$queues);
$_TPL->set_var('qmember',$qmember);
$_TPL->set_var('rightcall',$rightcall);
$_TPL->set_var('element',$element);
$_TPL->set_var('voicemail_list',$appuser->get_voicemail_list());
$_TPL->set_var('agent_list',$appuser->get_agent_list());
$_TPL->set_var('destination_list',$appuser->get_destination_list());
$_TPL->set_var('moh_list',$appuser->get_musiconhold());
$_TPL->set_var('tz_list',$appuser->get_timezones());
$_TPL->set_var('context_list',$appuser->get_context_list());
$_TPL->set_var('autoprov_list',$appuser->get_autoprov_list());
$_TPL->set_var('fkidentity_list',$appuser->get_phonefunckey_identity());
$_TPL->set_var('fktype_list',$appuser->get_phonefunckey_type());
$_TPL->set_var('profileclient_list',$appuser->get_profileclient_list());

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/xivo_ajs.js');
$dhtml->set_js('js/xivo_uri.js');
$dhtml->set_js('js/xivo_suggest.js');
$dhtml->set_js('js/xivo_http.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/phonefunckey.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/sip.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/iax.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/custom.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
$dhtml->add_js('/bloc/service/ipbx/'.$ipbx->get_name().'/pbx_settings/users/phonefunckey/phonefunckey.js.php');

?>
