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

if(isset($_QR['id']) === false || ($info = $appuser->get($_QR['id'])) === false)
	$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);

$return = &$info;

$result = $fm_save = $error = null;

if(isset($info['protocol']['allow']) === true)
	$allow = $info['protocol']['allow'];
else
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
&& dwho_issa('protocol',$_QR) === true
&& dwho_issa('userfeatures',$_QR) === true
&& isset($_QR['protocol']['protocol']) === true)
{
	$return = &$result;

	if($appuser->set_edit($_QR,$_QR['protocol']['protocol']) === false
	|| $appuser->edit() === false)
	{
		$fm_save = false;
		$result = $appuser->get_result();
		$result['dialaction'] = $appuser->get_dialaction_result();
		$result['phonefunckey'] = $appuser->get_phonefunckey_result();

		$error = $appuser->get_error();

		if(dwho_issa('protocol',$result) === true
		&& isset($result['protocol']['allow']) === true)
			$allow = $result['protocol']['allow'];

		$result['voicemail-option'] = $_QRY->get('voicemail-option');
	}
	else
	{
		$ipbx->discuss('xivo[userlist,update]');
		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);
	}
}

dwho::load_class('dwho_sort');

if($gmember['list'] !== false && dwho_ak('groupmember',$return) === true)
{
	$gmember['slt'] = dwho_array_intersect_key($return['groupmember'],
						   $gmember['list'],
						   'groupfeaturesid');

	if($gmember['slt'] !== false)
	{
		$gmember['info'] = dwho_array_copy_intersect_key($return['groupmember'],
								 $gmember['slt'],
								 'groupfeaturesid');
		$gmember['list'] = dwho_array_diff_key($gmember['list'],$gmember['slt']);

		$groupsort = new dwho_sort(array('key' => 'name'));
		uasort($gmember['slt'],array(&$groupsort,'str_usort'));
	}
}

if($qmember['list'] !== false && dwho_ak('queuemember',$return) === true)
{
	$qmember['slt'] = dwho_array_intersect_key($return['queuemember'],
						   $qmember['list'],
						   'queuefeaturesid');

	if($qmember['slt'] !== false)
	{
		$qmember['info'] = dwho_array_copy_intersect_key($return['queuemember'],
								 $qmember['slt'],
								 'queuefeaturesid');
		$qmember['list'] = dwho_array_diff_key($qmember['list'],$qmember['slt']);

		$queuesort = new dwho_sort(array('key' => 'name'));
		uasort($qmember['slt'],array(&$queuesort,'str_usort'));
	}
}

if($rightcall['list'] !== false && dwho_ak('rightcall',$return) === true)
{
	$rightcall['slt'] = dwho_array_intersect_key($return['rightcall'],
						     $rightcall['list'],
						     'rightcallid');

	if($rightcall['slt'] !== false)
	{
		$rightcall['list'] = dwho_array_diff_key($rightcall['list'],$rightcall['slt']);

		$rightcallsort = new dwho_sort(array('browse' => 'rightcall','key' => 'name'));
		uasort($rightcall['slt'],array(&$rightcallsort,'str_usort'));
	}
}

$element = $appuser->get_elements();

if(dwho_issa('allow',$element['protocol']['sip']) === true
&& dwho_issa('value',$element['protocol']['sip']['allow']) === true
&& empty($allow) === false)
{
	if(is_array($allow) === false)
		$allow = explode(',',$allow);

	$element['protocol']['sip']['allow']['value'] = array_diff($element['protocol']['sip']['allow']['value'],$allow);
}

if(dwho_issa('allow',$element['protocol']['iax']) === true
&& dwho_issa('value',$element['protocol']['iax']['allow']) === true
&& empty($allow) === false)
{
	if(is_array($allow) === false)
		$allow = explode(',',$allow);

	$element['protocol']['iax']['allow']['value'] = array_diff($element['protocol']['iax']['allow']['value'],$allow);
}

if(empty($return) === false)
{
	$return['protocol']['allow'] = $allow;

	if(dwho_issa('dialaction',$return) === false || empty($return['dialaction']) === true)
		$return['dialaction'] = null;

	if(dwho_issa('voicemail',$return) === false || empty($return['voicemail']) === true)
		$return['voicemail'] = null;

	if(dwho_issa('autoprov',$return) === false || empty($return['autoprov']) === true)
		$return['autoprov'] = null;
}
else
	$return = null;

$_TPL->set_var('id',$info['userfeatures']['id']);
$_TPL->set_var('info',$return);
$_TPL->set_var('error',$error);
$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('voicemail',$return['voicemail']);
$_TPL->set_var('dialaction',$return['dialaction']);
$_TPL->set_var('dialaction_from','user');
$_TPL->set_var('groups',$groups);
$_TPL->set_var('gmember',$gmember);
$_TPL->set_var('queues',$queues);
$_TPL->set_var('qmember',$qmember);
$_TPL->set_var('rightcall',$rightcall);
$_TPL->set_var('element',$element);
$_TPL->set_var('agent_list',$appuser->get_agent_list());
$_TPL->set_var('destination_list',$appuser->get_destination_list());
$_TPL->set_var('moh_list',$appuser->get_musiconhold());
$_TPL->set_var('tz_list',$appuser->get_timezones());
$_TPL->set_var('context_list',$appuser->get_context_list());
$_TPL->set_var('autoprov_list',$appuser->get_autoprov_list());
$_TPL->set_var('bsfilter_list',$appuser->get_bsfilter_list());
$_TPL->set_var('fkidentity_list',$appuser->get_phonefunckey_identity());
$_TPL->set_var('fktype_list',$appuser->get_phonefunckey_type());
$_TPL->set_var('profileclient_list',$appuser->get_profileclient_list());

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/uri.js');
$dhtml->set_js('js/dwho/http.js');
$dhtml->set_js('js/dwho/suggest.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/phonefunckey.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/sip.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/iax.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/custom.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
$dhtml->add_js('/bloc/service/ipbx/'.$ipbx->get_name().'/pbx_settings/users/phonefunckey/phonefunckey.js.php');

?>
