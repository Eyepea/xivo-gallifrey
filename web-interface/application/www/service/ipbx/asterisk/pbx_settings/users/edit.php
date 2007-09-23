<?php

$appuser = &$ipbx->get_application('user');

if(isset($_QR['id']) === false || ($info = $appuser->get($_QR['id'])) === false)
	$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);

$return = &$info;

$musiconhold = &$ipbx->get_module('musiconhold');
$autoprov = &$ipbx->get_module('autoprov');

$gmember_slt = $gmember_unslt = false;

if(($groups = $ipbx->get_group_user($info['ufeatures']['id'])) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('browse' => 'queue','key' => 'name'));
	usort($groups,array(&$sort,'str_usort'));

	$gmember_slt = $gmember_unslt = array();

	$nb = count($groups);

	for($i = 0;$i < $nb;$i++)
	{
		$name = &$groups[$i]['queue']['name'];

		if($groups[$i]['member'] !== false)
			$gmember_slt[$name] = $groups[$i]['member'];
		else
			$gmember_unslt[$name] = $name;
	}

	if(empty($gmember_slt) === true)
		$gmember_slt = false;

	if(empty($gmember_unslt) === true)
	{
		$gmember_unslt = false;

		if($gmember_slt === false)
			$groups = false;
	}
}

$qmember_slt = $qmember_unslt = false;

if(($queues = $ipbx->get_queue_user($info['ufeatures']['id'])) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('browse' => 'queue','key' => 'name'));
	usort($queues,array(&$sort,'str_usort'));

	$qmember_slt = $qmember_unslt = array();

	$nb = count($queues);

	for($i = 0;$i < $nb;$i++)
	{
		$name = &$queues[$i]['queue']['name'];

		if($queues[$i]['member'] !== false)
			$qmember_slt[$name] = $queues[$i]['member'];
		else
			$qmember_unslt[$name] = $name;
	}

	if(empty($qmember_slt) === true)
		$qmember_slt = false;

	if(empty($qmember_unslt) === true)
	{
		$qmember_unslt = false;

		if($qmember_slt === false)
			$queues = false;
	}
}

if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
	ksort($moh_list);

$allow = $info['protocol']['allow'];

$result = null;

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('protocol',$_QR) === false || xivo_issa('ufeatures',$_QR) === false)
		break;

	$return = &$result;

	if(xivo_issa('allow',$_QR['protocol']) === false)
		unset($_QR['protocol']['allow'],$_QR['protocol']['disallow']);

	if(isset($_QR['protocol']['host-dynamic']) === true)
		$_QR['protocol']['host'] = $_QR['protocol']['host-dynamic'];
	else
		$_QR['protocol']['host'] = '';

	if(isset($_QR['protocol']['host-static']) === true && $_QR['protocol']['host'] === 'static')
		$_QR['protocol']['host'] = $_QR['protocol']['host-static'];

	unset($_QR['protocol']['host-dynamic'],$_QR['protocol']['host-static']);

	if($moh_list === false || isset($_QR['ufeatures']['musiconhold'],
					$moh_list[$_QR['ufeatures']['musiconhold']]) === false)
		$_QR['ufeatures']['musiconhold'] = '';

	if(isset($_QR['ufeatures']['outcallerid-type']) === true)
		$_QR['ufeatures']['outcallerid'] = $_QR['ufeatures']['outcallerid-type'];
	else
		$_QR['ufeatures']['outcallerid'] = '';

	if(isset($_QR['ufeatures']['outcallerid-custom']) === true && $_QR['ufeatures']['outcallerid'] === 'custom')
		$_QR['ufeatures']['outcallerid'] = $_QR['ufeatures']['outcallerid-custom'];

	unset($_QR['ufeatures']['outcallerid-type'],$_QR['ufeatures']['outcallerid-custom']);

	if($appuser->set_edit($_QR,$_QR['protocol']['protocol']) === false
	|| $appuser->edit() === false)
	{
		$result = $appuser->get_result();

		if(xivo_issa('protocol',$result) === true)
			$allow = $result['protocol']['allow'];
		break;
	}

	$_QRY->go($_HTML->url('service/ipbx/pbx_settings/users'),$param);
}
while(false);

$element = $appuser->get_element();

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

if(empty($return) === false)
{
	$return['protocol']['allow'] = $allow;

	if(xivo_issa('usergroup',$return) === false || empty($return['usergroup']) === true)
		$return['usergroup'] = null;

	if(xivo_issa('voicemail',$return) === false || empty($return['voicemail']) === true)
		$return['voicemail'] = null;

	if(xivo_issa('autoprov',$return) === false || empty($return['autoprov']) === true)
		$return['autoprov'] = null;
}

$_HTML->assign('id',$info['ufeatures']['id']);
$_HTML->assign('info',$return);
$_HTML->assign('groups',$groups);
$_HTML->assign('gmember_slt',$gmember_slt);
$_HTML->assign('gmember_unslt',$gmember_unslt);
$_HTML->assign('queues',$queues);
$_HTML->assign('qmember_slt',$qmember_slt);
$_HTML->assign('qmember_unslt',$qmember_unslt);
$_HTML->assign('protocol',$ipbx->get_protocol());
$_HTML->assign('element',$element);
$_HTML->assign('moh_list',$moh_list);
$_HTML->assign('autoprov_list',$autoprov->get_autoprov_list());

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

?>
