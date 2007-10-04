<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$outcall = &$ipbx->get_module('outcall');

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appoutcall = &$ipbx->get_application('outcall');

		$result = null;
		$rightcall['slt'] = $rightcall = array();

		xivo::load_class('xivo_sort');

		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('outcall',$_QR) === false
			|| xivo_issa('extenumbers',$_QR) === false)
				break;

			if($appoutcall->set_add($_QR) === false
			|| $appoutcall->add() === false)
			{
				$result = $appoutcall->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		}
		while(false);

		if($rightcall['list'] !== false && xivo_ak('rightcall',$result) === true)
		{
			$rightcall['slt'] = xivo_array_intersect_key($result['rightcall'],$rightcall['list'],'rightcallid');

			if($rightcall['slt'] !== false)
			{
				$rightcall['list'] = xivo_array_diff_key($rightcall['list'],$rightcall['slt']);
				uasort($rightcall['slt'],array(&$rightcallsort,'str_usort'));
			}
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/outcall.js');

		$_HTML->assign('rightcall',$rightcall);
		$_HTML->assign('element',$appoutcall->get_element());
		$_HTML->assign('info',$result);
		$_HTML->assign('trunks_list',$ipbx->get_trunks_list());
		break;
	case 'edit':
		$appoutcall = &$ipbx->get_application('outcall');

		if(isset($_QR['id']) === false || ($info = $appoutcall->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$result = null;
		$return = &$info;
		$rightcall['slt'] = $rightcall = array();

		xivo::load_class('xivo_sort');

		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('outcall',$_QR) === false
			|| xivo_issa('extenumbers',$_QR) === false)
				break;

			if($appoutcall->set_edit($_QR) === false
			|| $appoutcall->edit() === false)
			{
				$result = $appoutcall->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		}
		while(false);

		if($rightcall['list'] !== false && xivo_ak('rightcall',$return) === true)
		{
			$rightcall['slt'] = xivo_array_intersect_key($return['rightcall'],$rightcall['list'],'rightcallid');

			if($rightcall['slt'] !== false)
			{
				$rightcall['list'] = xivo_array_diff_key($rightcall['list'],$rightcall['slt']);
				uasort($rightcall['slt'],array(&$rightcallsort,'str_usort'));
			}
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/outcall.js');

		$_HTML->assign('id',$info['outcall']['id']);
		$_HTML->assign('rightcall',$rightcall);
		$_HTML->assign('element',$appoutcall->get_element());
		$_HTML->assign('info',$return);
		$_HTML->assign('trunks_list',$ipbx->get_trunks_list());
		break;
	case 'delete':
		$param['page'] = $page;

		$appoutcall = &$ipbx->get_application('outcall');

		if(isset($_QR['id']) === false || $appoutcall->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$appoutcall->delete();

		$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('outcalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$appoutcall = &$ipbx->get_application('outcall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appoutcall->get($values[$i]) === false)
				continue;

			$appoutcall->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('outcalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$appoutcall = &$ipbx->get_application('outcall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appoutcall->get($values[$i]) === false)
				continue;

			if($act === 'disables')
				$appoutcall->disable();
			else
				$appoutcall->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($list = $ipbx->get_outcall_list()) !== false)
		{
			$total = count($list);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'outcall','key' => 'name'));
			usort($list,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$list);
}


$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/outcall');

$_HTML->assign('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/outcall/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
