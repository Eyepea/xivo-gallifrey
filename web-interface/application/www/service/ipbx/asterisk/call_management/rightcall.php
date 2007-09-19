<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$rightcall = &$ipbx->get_module('rightcall');

if(($user_list = $ipbx->get_user_rightcall_info()) !== false)
{
	xivo::load_class('xivo_sort');
	$sort = new xivo_sort();
	uasort($user_list,array(&$sort,'str_usort'));
}

$param = array();
$param['act'] = 'list';

$info = $result = $user_slt = array();

switch($act)
{
	case 'add':
		$add = true;
		$result = null;

		$apprightcall = &$ipbx->get_application('rightcall');

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('rightcall',$_QR) === false)
				break;

			if($apprightcall->set_add($_QR) === false
			|| $apprightcall->add() === false)
			{
				$result = $apprightcall->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);
		}
		while(false);

		$_HTML->assign('user_slt',$user_slt);
		$_HTML->assign('user_list',$user_list);
		$_HTML->assign('element',$apprightcall->get_element());
		$_HTML->assign('info',$result);
		break;
	case 'edit':
		break;
	case 'delete':
		break;
	case 'deletes':
		break;
	case 'disables':
	case 'enables':
		break;
	default:
		$total = 0;
		$act = 'list';

		if(($list = $ipbx->get_schedule_list()) !== false)
		{
			$total = count($list);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'schedule','key' => 'name'));
			usort($list,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$list);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/rightcall');

$_HTML->assign('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/rightcall/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
