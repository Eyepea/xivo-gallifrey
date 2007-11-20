<?php

$autoprov = &$ipbx->get_module('autoprov');
$zonemessages = &$ipbx->get_module('zonemessages');

$result = null;

$allow = array();

$gmember = $qmember = $rightcall = array();
$gmember['list'] = $qmember['list'] = false;
$gmember['info'] = $qmember['info'] = false;
$gmember['slt'] = $qmember['slt'] = $rightcall['slt'] = array();

xivo::load_class('xivo_sort');
$groupsort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));

if(($groups = $ipbx->get_groups_list(null,true)) !== false)
{
	uasort($groups,array(&$groupsort,'str_usort'));
	$gmember['list'] = $groups;
}

$queuesort = new xivo_sort(array('browse' => 'qfeatures','key' => 'name'));

if(($queues = $ipbx->get_queues_list(null,true)) !== false)
{
	uasort($queues,array(&$queuesort,'str_usort'));
	$qmember['list'] = $queues;
}

$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
	uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

do
{
	if(isset($_QR['fm_send']) === false
	|| xivo_issa('protocol',$_QR) === false
	|| xivo_issa('ufeatures',$_QR) === false
	|| isset($_QR['protocol']['protocol']) === false)
		break;

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

		if(xivo_issa('protocol',$result) === true && isset($result['protocol']['allow']) === true)
			$allow = $result['protocol']['allow'];
		break;
	}

	$ipbx->discuss('xivo[userlist,update]');

	$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);
}
while(false);

if($gmember['list'] !== false && xivo_ak('groupmember',$result) === true)
{
	$gmember['slt'] = xivo_array_intersect_key($result['groupmember'],$gmember['list'],'gfeaturesid');

	if($gmember['slt'] !== false)
	{
		$gmember['info'] = xivo_array_copy_intersect_key($result['groupmember'],$gmember['slt'],'gfeaturesid');
		$gmember['list'] = xivo_array_diff_key($gmember['list'],$gmember['slt']);
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
		uasort($qmember['slt'],array(&$queuesort,'str_usort'));
	}
}

if($rightcall['list'] !== false && xivo_ak('rightcall',$result) === true)
{
	$rightcall['slt'] = xivo_array_intersect_key($result['rightcall'],$rightcall['list'],'rightcallid');

	if($rightcall['slt'] !== false)
	{
		$rightcall['list'] = xivo_array_diff_key($rightcall['list'],$rightcall['slt']);
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
$_HTML->set_var('dialstatus',$result['dialstatus']);
$_HTML->set_var('groups',$groups);
$_HTML->set_var('gmember',$gmember);
$_HTML->set_var('queues',$queues);
$_HTML->set_var('qmember',$qmember);
$_HTML->set_var('rightcall',$rightcall);
$_HTML->set_var('zonemessages',$zonemessages->get_all_name());
$_HTML->set_var('element',$element);
$_HTML->set_var('list',$appuser->get_dialstatus_destination_list());
$_HTML->set_var('moh_list',$appuser->get_musiconhold());
$_HTML->set_var('autoprov_list',$autoprov->get_autoprov_list());

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialstatus.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/sip.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/iax.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users/custom.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

?>
