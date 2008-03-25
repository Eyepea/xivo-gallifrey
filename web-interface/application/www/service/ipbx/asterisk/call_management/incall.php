<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;
$search = isset($_QR['search']) === true ? strval($_QR['search']) : '';

$info = array();

$param = array();
$param['act'] = 'list';

if($search !== '')
	$param['search'] = $search;

switch($act)
{
	case 'add':
		$appincall = &$ipbx->get_application('incall');

		$result = null;
		$rightcall['slt'] = $rightcall = array();

		xivo::load_class('xivo_sort');
		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		if(isset($_QR['fm_send']) === true && xivo_issa('incall',$_QR) === true)
		{
			if($appincall->set_add($_QR) === false
			|| $appincall->add() === false)
			{
				$result = $appincall->get_result();
				$result['incall'] = $appincall->get_destination_result();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		}

		if(xivo_issa('incall',$result) === false || empty($result['incall']) === true)
			$result['incall'] = null;

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
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/incall.js');

		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('incall',$result['incall']);
		$_HTML->set_var('list',$appincall->get_destination_list());
		$_HTML->set_var('element',$appincall->get_elements());
		break;
	case 'edit':
		$appincall = &$ipbx->get_application('incall');

		if(isset($_QR['id']) === false || ($info = $appincall->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$result = null;
		$return = &$info;
		$rightcall['slt'] = $rightcall = array();

		xivo::load_class('xivo_sort');
		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		if(isset($_QR['fm_send']) === true && xivo_issa('incall',$_QR) === true)
		{
			$return = &$result;

			if($appincall->set_edit($_QR) === false
			|| $appincall->edit() === false)
			{
				$result = $appincall->get_result();
				$result['incall'] = $appincall->get_destination_result();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		}

		if(xivo_issa('incall',$return) === false || empty($return['incall']) === true)
			$return['incall'] = null;

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
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/incall.js');

		$_HTML->set_var('id',$info['incall']['id']);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('incall',$return['incall']);
		$_HTML->set_var('list',$appincall->get_destination_list());
		$_HTML->set_var('element',$appincall->get_elements());
		break;
	case 'delete':
		$param['page'] = $page;

		$appincall = &$ipbx->get_application('incall');

		if(isset($_QR['id']) === false || $appincall->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$appincall->delete();

		$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('incalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$appincall = &$ipbx->get_application('incall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appincall->get($values[$i]) !== false)
				$appincall->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('incalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$appincall = &$ipbx->get_application('incall',null,false);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appincall->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appincall->disable();
			else
				$appincall->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appincall = &$ipbx->get_application('incall',null,false);

		$order = array();
		$order['exten'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		if($search !== '')
			$list = $appincall->get_incalls_search($search,null,$order,$limit);
		else
			$list = $appincall->get_incalls_list(null,$order,$limit);

		$total = $appincall->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
		$_HTML->set_var('search',$search);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/incall');

$_HTML->set_var('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/incall/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
