<?php

$appuser = &$ipbx->get_application('user');
$musiconhold = &$ipbx->get_module('musiconhold');
$autoprov = &$ipbx->get_module('autoprov');

$gmember_unslt = false;

if(($groups = $ipbx->get_groups_list()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));
	usort($groups,array(&$sort,'str_usort'));

	$gmember_unslt = array();

	$nb = count($groups);

	for($i = 0;$i < $nb;$i++)
	{
		$name = &$groups[$i]['queue']['name'];
		$gmember_unslt[$name] = $name;
	}

	if(empty($gmember_unslt) === true)
		$gmember_unslt = $groups = false;
}

$qmember_unslt = false;

if(($queues = $ipbx->get_queues_list()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort(array('browse' => 'qfeatures','key' => 'name'));
	usort($queues,array(&$sort,'str_usort'));

	$qmember_unslt = array();

	$nb = count($queues);

	for($i = 0;$i < $nb;$i++)
	{
		$name = &$queues[$i]['queue']['name'];
		$qmember_unslt[$name] = $name;
	}

	if(empty($qmember_unslt) === true)
		$qmember_unslt = $queues = false;
}

if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
	ksort($moh_list);

$allow = array();

$result = null;

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

	if($appuser->set_add($_QR,$_QR['protocol']['protocol']) === false
	|| $appuser->add() === false)
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

if(empty($result) === false)
{
	$result['protocol']['allow'] = $allow;

	if(xivo_issa('usergroup',$result) === false || empty($result['usergroup']) === true)
		$result['usergroup'] = null;

	if(xivo_issa('voicemail',$result) === false || empty($result['voicemail']) === true)
		$result['voicemail'] = null;

	if(xivo_issa('autoprov',$result) === false || empty($result['autoprov']) === true)
		$result['autoprov'] = null;
}
else
	$result = null;

$_HTML->assign('info',$result);
$_HTML->assign('groups',$groups);
$_HTML->assign('gmember_slt',false);
$_HTML->assign('gmember_unslt',$gmember_unslt);
$_HTML->assign('queues',$queues);
$_HTML->assign('qmember_slt',false);
$_HTML->assign('qmember_unslt',$qmember_unslt);
$_HTML->assign('protocol',$ipbx->get_protocol());
$_HTML->assign('element',$element);
$_HTML->assign('moh_list',$moh_list);
$_HTML->assign('autoprov_list',$autoprov->get_autoprov_list());

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/users.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

?>
