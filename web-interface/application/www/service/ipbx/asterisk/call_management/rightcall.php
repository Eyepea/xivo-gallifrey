<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$apprightcall = &$ipbx->get_application('rightcall');

		$result = null;

		$rcalluser = $rcallgroup = $rcalloutcall = array();
		$rcalluser['slt'] = $rcallgroup['slt'] = $rcalloutcall['slt'] = null;

		xivo::load_class('xivo_sort');
		$usersort = new xivo_sort(array('browse' => 'ufeatures','key' => 'identity'));

		if(($rcalluser['list'] = $ipbx->get_users_list(null,null,true)) !== false)
			uasort($rcalluser['list'],array(&$usersort,'str_usort'));

		$groupsort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));

		if(($rcallgroup['list'] = $ipbx->get_groups_list(null,true)) !== false)
			uasort($rcallgroup['list'],array(&$groupsort,'str_usort'));

		$outcallsort = new xivo_sort(array('browse' => 'outcall','key' => 'name'));

		if(($rcalloutcall['list'] = $ipbx->get_outcall_list(null,true)) !== false)
			uasort($rcalloutcall['list'],array(&$outcallsort,'str_usort'));

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

		if($rcalluser['list'] !== false && xivo_ak('rightcalluser',$result) === true)
		{
			$rcalluser['slt'] = xivo_array_intersect_key($result['rightcalluser'],$rcalluser['list'],'typeval');
			
			if($rcalluser['slt'] !== false)
			{
				$rcalluser['list'] = xivo_array_diff_key($rcalluser['list'],$rcalluser['slt']);
				uasort($rcalluser['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($rcallgroup['list'] !== false && xivo_ak('rightcallgroup',$result) === true)
		{
			$rcallgroup['slt'] = xivo_array_intersect_key($result['rightcallgroup'],$rcallgroup['list'],'typeval');
			
			if($rcallgroup['slt'] !== false)
			{
				$rcallgroup['list'] = xivo_array_diff_key($rcallgroup['list'],$rcallgroup['slt']);
				uasort($rcallgroup['slt'],array(&$groupsort,'str_usort'));
			}
		}

		if($rcalloutcall['list'] !== false && xivo_ak('rightcalloutcall',$result) === true)
		{
			$rcalloutcall['slt'] = xivo_array_intersect_key($result['rightcalloutcall'],$rcalloutcall['list'],'typeval');
			
			if($rcalloutcall['slt'] !== false)
			{
				$rcalloutcall['list'] = xivo_array_diff_key($rcalloutcall['list'],$rcalloutcall['slt']);
				uasort($rcalloutcall['slt'],array(&$outcallsort,'str_usort'));
			}
		}

		if(xivo_issa('rcallexten',$result) === true)
			$rcallexten = $result['rcallexten'];
		else
			$rcallexten = null;

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		$_HTML->assign('rcalluser',$rcalluser);
		$_HTML->assign('rcallgroup',$rcallgroup);
		$_HTML->assign('rcalloutcall',$rcalloutcall);
		$_HTML->assign('rcallexten',$rcallexten);
		$_HTML->assign('element',$apprightcall->get_elements());
		$_HTML->assign('info',$result);
		break;
	case 'edit':
		$apprightcall = &$ipbx->get_application('rightcall');

		if(isset($_QR['id']) === false || ($info = $apprightcall->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);

		$result = null;
		$return = &$info;

		$rcalluser = $rcallgroup = $rcalloutcall = array();
		$rcalluser['slt'] = $rcallgroup['slt'] = $rcalloutcall['slt'] = null;

		xivo::load_class('xivo_sort');
		$usersort = new xivo_sort(array('browse' => 'ufeatures','key' => 'identity'));

		if(($rcalluser['list'] = $ipbx->get_users_list(null,null,true)) !== false)
			uasort($rcalluser['list'],array(&$usersort,'str_usort'));

		$groupsort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));

		if(($rcallgroup['list'] = $ipbx->get_groups_list(null,true)) !== false)
			uasort($rcallgroup['list'],array(&$groupsort,'str_usort'));

		$outcallsort = new xivo_sort(array('browse' => 'outcall','key' => 'name'));

		if(($rcalloutcall['list'] = $ipbx->get_outcall_list(null,true)) !== false)
			uasort($rcalloutcall['list'],array(&$outcallsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false || xivo_issa('rightcall',$_QR) === false)
				break;

			$return = &$result;

			if($apprightcall->set_edit($_QR) === false
			|| $apprightcall->edit() === false)
			{
				$result = $apprightcall->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);
		}
		while(false);

		if($rcalluser['list'] !== false && xivo_ak('rightcalluser',$return) === true)
		{
			$rcalluser['slt'] = xivo_array_intersect_key($return['rightcalluser'],$rcalluser['list'],'typeval');

			if($rcalluser['slt'] !== false)
			{
				$rcalluser['list'] = xivo_array_diff_key($rcalluser['list'],$rcalluser['slt']);
				uasort($rcalluser['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($rcallgroup['list'] !== false && xivo_ak('rightcallgroup',$return) === true)
		{
			$rcallgroup['slt'] = xivo_array_intersect_key($return['rightcallgroup'],$rcallgroup['list'],'typeval');
			
			if($rcallgroup['slt'] !== false)
			{
				$rcallgroup['list'] = xivo_array_diff_key($rcallgroup['list'],$rcallgroup['slt']);
				uasort($rcallgroup['slt'],array(&$groupsort,'str_usort'));
			}
		}

		if($rcalloutcall['list'] !== false && xivo_ak('rightcalloutcall',$return) === true)
		{
			$rcalloutcall['slt'] = xivo_array_intersect_key($return['rightcalloutcall'],$rcalloutcall['list'],'typeval');
			
			if($rcalloutcall['slt'] !== false)
			{
				$rcalloutcall['list'] = xivo_array_diff_key($rcalloutcall['list'],$rcalloutcall['slt']);
				uasort($rcalloutcall['slt'],array(&$outcallsort,'str_usort'));
			}
		}

		if(xivo_issa('rightcallexten',$return) === false)
			$rcallexten = null;
		else
		{
			$rcallexten = $return['rightcallexten'];

			$extensort = new xivo_sort(array('key' => 'exten'));
			uasort($rcallexten,array(&$extensort,'str_usort'));
		}

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		$_HTML->assign('id',$info['rightcall']['id']);
		$_HTML->assign('rcalluser',$rcalluser);
		$_HTML->assign('rcallgroup',$rcallgroup);
		$_HTML->assign('rcalloutcall',$rcalloutcall);
		$_HTML->assign('rcallexten',$rcallexten);
		$_HTML->assign('element',$apprightcall->get_elements());
		$_HTML->assign('info',$return);
		break;
	case 'delete':
		$param['page'] = $page;

		$apprightcall = &$ipbx->get_application('rightcall');

		if(isset($_QR['id']) === false || $apprightcall->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);

		$apprightcall->delete();

		$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('rightcalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);

		$apprightcall = &$ipbx->get_application('rightcall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($apprightcall->get($values[$i]) === false)
				continue;

			$apprightcall->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables' ? true : false;

		if(($values = xivo_issa_val('rightcalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);

		$rightcall = &$ipbx->get_module('rightcall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$rightcall->disable($values[$i],$disable);

		$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);
		break;
	default:
		$total = 0;
		$act = 'list';

		if(($list = $ipbx->get_rightcall_list()) !== false)
		{
			$total = count($list);
			
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));
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
