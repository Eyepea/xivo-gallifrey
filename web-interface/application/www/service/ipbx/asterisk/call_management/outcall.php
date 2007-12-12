<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appoutcall = &$ipbx->get_application('outcall');

		$result = null;

		$outcalltrunk = $rightcall = array();
		$outcalltrunk['slt'] = $rightcall['slt'] = array();

		$outcalltrunk['list'] = $ipbx->get_trunks_list(null,false);

		xivo::load_class('xivo_sort');
		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true,false)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('outcall',$_QR) === false)
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

		if($outcalltrunk['list'] !== false && xivo_issa('outcalltrunk',$result) === true)
		{
			$outcalltrunksort = new xivo_sort(array('key' => 'priority'));
			usort($result['outcalltrunk'],array(&$outcalltrunksort,'num_usort'));

			$outcalltrunk['slt'] = xivo_array_intersect_key($result['outcalltrunk'],$outcalltrunk['list'],'trunkid');

			if($outcalltrunk['slt'] !== false)
				$outcalltrunk['list'] = xivo_array_diff_key($outcalltrunk['list'],$outcalltrunk['slt']);
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

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/outcall.js');

		$_HTML->set_var('outcalltrunk',$outcalltrunk);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('element',$appoutcall->get_elements());
		$_HTML->set_var('info',$result);
		break;
	case 'edit':
		$appoutcall = &$ipbx->get_application('outcall');

		if(isset($_QR['id']) === false || ($info = $appoutcall->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$result = null;
		$return = &$info;
		$outcalltrunk = $rightcall = array();
		$outcalltrunk['slt'] = $rightcall['slt'] = array();

		$outcalltrunk['list'] = $ipbx->get_trunks_list(null,false);

		xivo::load_class('xivo_sort');
		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('outcall',$_QR) === false)
				break;

			$return = &$result;

			if($appoutcall->set_edit($_QR) === false
			|| $appoutcall->edit() === false)
			{
				$result = $appoutcall->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		}
		while(false);

		if($outcalltrunk['list'] !== false && xivo_issa('outcalltrunk',$return) === true)
		{
			$outcalltrunksort = new xivo_sort(array('key' => 'priority'));
			usort($return['outcalltrunk'],array(&$outcalltrunksort,'num_usort'));

			$outcalltrunk['slt'] = xivo_array_intersect_key($return['outcalltrunk'],$outcalltrunk['list'],'trunkid');

			if($outcalltrunk['slt'] !== false)
				$outcalltrunk['list'] = xivo_array_diff_key($outcalltrunk['list'],$outcalltrunk['slt']);
		}

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

		$_HTML->set_var('id',$info['outcall']['id']);
		$_HTML->set_var('outcalltrunk',$outcalltrunk);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('element',$appoutcall->get_elements());
		$_HTML->set_var('info',$return);
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
	case 'enables':
	case 'disables':
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
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appoutcall = &$ipbx->get_application('outcall');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appoutcall->get_outcalls_list(null,$order,$limit);
		$total = $appoutcall->get_cnt();

		if($list === false && $total > 0)
		{
			$param['page'] = $page - 1;
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}


$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/outcall');

$_HTML->set_var('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/outcall/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
