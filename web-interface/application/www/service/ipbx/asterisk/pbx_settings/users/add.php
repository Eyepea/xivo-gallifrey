<?php

$result = $error = null;

$allow = array();

$gmember = $qmember = $rightcall = array();
$gmember['list'] = $qmember['list'] = false;
$gmember['info'] = $qmember['info'] = false;
$gmember['slt'] = $qmember['slt'] = $rightcall['slt'] = array();

$appgroup = &$ipbx->get_application('group',null,false);

if(($groups = $appgroup->get_groups_list(null,array('name' => SORT_ASC),null,true)) !== false)
	$gmember['list'] = $groups;

$appqueue = &$ipbx->get_application('queue',null,false);

if(($queues = $appqueue->get_queues_list(null,array('name' => SORT_ASC),null,true)) !== false)
	$qmember['list'] = $queues;

$apprightcall = &$ipbx->get_application('rightcall',null,false);

$rightcall['list'] = $apprightcall->get_rightcalls_list(null,array('name' => SORT_ASC),null,true);

if(isset($_QR['fm_send']) === true
&& xivo_issa('protocol',$_QR) === true
&& xivo_issa('ufeatures',$_QR) === true
&& isset($_QR['protocol']['protocol']) === true)
{
	if(xivo_issa('allow',$_QR['protocol']) === false)
		unset($_QR['protocol']['allow'],$_QR['protocol']['disallow']);

	if(isset($_QR['protocol']['host-dynamic']) === true)
		$_QR['protocol']['host'] = $_QR['protocol']['host-dynamic'];
	else
		$_QR['protocol']['host'] = '';

	if(isset($_QR['protocol']['host-static']) === true && $_QR['protocol']['host'] === 'static')
		$_QR['protocol']['host'] = $_QR['protocol']['host-static'];

	unset($_QR['protocol']['host-dynamic'],$_QR['protocol']['host-static']);

	if(isset($_QR['ufeatures']['outcallerid-type']) === true)
		$_QR['ufeatures']['outcallerid'] = $_QR['ufeatures']['outcallerid-type'];
	else
		$_QR['ufeatures']['outcallerid'] = '';

	if(isset($_QR['ufeatures']['outcallerid-custom']) === true && $_QR['ufeatures']['outcallerid'] === 'custom')
		$_QR['ufeatures']['outcallerid'] = $_QR['ufeatures']['outcallerid-custom'];

	unset($_QR['ufeatures']['outcallerid-type'],$_QR['ufeatures']['outcallerid-custom']);

	if($appuser->set_add($_QR,$_QR['protocol']['protocol']) === false
	|| $appuser->add() === false)
	{
		$result = $appuser->get_result();
		$result['dialstatus'] = $appuser->get_dialstatus_result();
		$result['phonefunckey'] = $appuser->get_phonefunckey_result();

		$error = $appuser->get_error();

		if(xivo_issa('protocol',$result) === true && isset($result['protocol']['allow']) === true)
			$allow = $result['protocol']['allow'];
	}
	else
	{
		$ipbx->discuss('xivo[userlist,update]');
		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);
	}
}

xivo::load_class('xivo_sort');

if($gmember['list'] !== false && xivo_ak('groupmember',$result) === true)
{
	$gmember['slt'] = xivo_array_intersect_key($result['groupmember'],$gmember['list'],'gfeaturesid');

	if($gmember['slt'] !== false)
	{
		$gmember['info'] = xivo_array_copy_intersect_key($result['groupmember'],$gmember['slt'],'gfeaturesid');
		$gmember['list'] = xivo_array_diff_key($gmember['list'],$gmember['slt']);

		$groupsort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));
		uasort($gmember['slt'],array(&$groupsort,'str_usort'));
	}
}

if($qmember['list'] !== false && xivo_ak('queuemember',$result) === true)
{
	$qmember['slt'] = xivo_array_intersect_key($result['queuemember'],$qmember['list'],'qfeaturesid');

	if($qmember['slt'] !== false)
	{
		$qmember['info'] = xivo_array_copy_intersect_key($result['queuemember'],$qmember['slt'],'qfeaturesid');
		$qmember['list'] = xivo_array_diff_key($qmember['list'],$qmember['slt']);

		$queuesort = new xivo_sort(array('browse' => 'qfeatures','key' => 'name'));
		uasort($qmember['slt'],array(&$queuesort,'str_usort'));
	}
}

if($rightcall['list'] !== false && xivo_ak('rightcall',$result) === true)
{
	$rightcall['slt'] = xivo_array_intersect_key($result['rightcall'],$rightcall['list'],'rightcallid');

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

	if(xivo_issa('dialstatus',$result) === false || empty($result['dialstatus']) === true)
		$result['dialstatus'] = null;

	if(xivo_issa('voicemail',$result) === false || empty($result['voicemail']) === true)
		$result['voicemail'] = null;

	if(xivo_issa('autoprov',$result) === false || empty($result['autoprov']) === true)
		$result['autoprov'] = null;
}
else
	$result = null;

$_HTML->set_var('info',$result);
$_HTML->set_var('error',$error);
$_HTML->set_var('dialstatus',$result['dialstatus']);
$_HTML->set_var('groups',$groups);
$_HTML->set_var('gmember',$gmember);
$_HTML->set_var('queues',$queues);
$_HTML->set_var('qmember',$qmember);
$_HTML->set_var('rightcall',$rightcall);
$_HTML->set_var('element',$element);
$_HTML->set_var('destination_list',$appuser->get_destination_list());
$_HTML->set_var('moh_list',$appuser->get_musiconhold());
$_HTML->set_var('tz_list',$appuser->get_timezones());
$_HTML->set_var('context_list',$appuser->get_context_list());
$_HTML->set_var('autoprov_list',$appuser->get_autoprov_list());
$_HTML->set_var('fktype_list',$appuser->get_phonefunckey_type());

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialstatus.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/phonefunckey.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/sip.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/iax.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/custom.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

?>
