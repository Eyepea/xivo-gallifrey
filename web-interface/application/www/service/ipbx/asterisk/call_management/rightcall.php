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

		$rcalluser = $rcallgroup = $rcallincall = $rcalloutcall = array();
		$rcalluser['slt'] = $rcallgroup['slt'] = $rcallincall['slt'] = $rcalloutcall['slt'] = null;

		$userorder = array();
		$userorder['firstname'] = SORT_ASC;
		$userorder['lastname'] = SORT_ASC;
		$userorder['number'] = SORT_ASC;
		$userorder['context'] = SORT_ASC;
		$userorder['name'] = SORT_ASC;

		$appuser = &$ipbx->get_application('user',null,false);
		$rcalluser['list'] = $appuser->get_users_list(null,null,$userorder,null,true);

		$appgroup = &$ipbx->get_application('group',null,false);
		$rcallgroup['list'] = $appgroup->get_groups_list(null,array('name' => SORT_ASC),null,true);

		$appincall = &$ipbx->get_application('incall',null,false);
		$rcallincall['list'] = $appincall->get_incalls_list(null,array('exten' => SORT_ASC),null,true);

		$appoutcall = &$ipbx->get_application('outcall',null,false);
		$rcalloutcall['list'] = $appoutcall->get_outcalls_list(null,array('name' => SORT_ASC),null,true);

		if(isset($_QR['fm_send']) === true && xivo_issa('rightcall',$_QR) === true)
		{
			if($apprightcall->set_add($_QR) === false
			|| $apprightcall->add() === false)
				$result = $apprightcall->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);
		}

		xivo::load_class('xivo_sort');

		if($rcalluser['list'] !== false && xivo_ak('rightcalluser',$result) === true)
		{
			$rcalluser['slt'] = xivo_array_intersect_key($result['rightcalluser'],$rcalluser['list'],'typeval');
			
			if($rcalluser['slt'] !== false)
			{
				$rcalluser['list'] = xivo_array_diff_key($rcalluser['list'],$rcalluser['slt']);

				$usersort = new xivo_sort(array('key' => 'identity'));
				uasort($rcalluser['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($rcallgroup['list'] !== false && xivo_ak('rightcallgroup',$result) === true)
		{
			$rcallgroup['slt'] = xivo_array_intersect_key($result['rightcallgroup'],$rcallgroup['list'],'typeval');
			
			if($rcallgroup['slt'] !== false)
			{
				$rcallgroup['list'] = xivo_array_diff_key($rcallgroup['list'],$rcallgroup['slt']);

				$groupsort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));
				uasort($rcallgroup['slt'],array(&$groupsort,'str_usort'));
			}
		}

		if($rcallincall['list'] !== false && xivo_ak('rightcallincall',$result) === true)
		{
			$rcallincall['slt'] = xivo_array_intersect_key($result['rightcallincall'],$rcallincall['list'],'typeval');
			
			if($rcallincall['slt'] !== false)
			{
				$rcallincall['list'] = xivo_array_diff_key($rcallincall['list'],$rcallincall['slt']);
				
				$incallsort = new xivo_sort(array('browse' => 'incall','key' => 'exten'));
				uasort($rcallincall['slt'],array(&$incallsort,'str_usort'));
			}
		}

		if($rcalloutcall['list'] !== false && xivo_ak('rightcalloutcall',$result) === true)
		{
			$rcalloutcall['slt'] = xivo_array_intersect_key($result['rightcalloutcall'],$rcalloutcall['list'],'typeval');
			
			if($rcalloutcall['slt'] !== false)
			{
				$rcalloutcall['list'] = xivo_array_diff_key($rcalloutcall['list'],$rcalloutcall['slt']);
				
				$outcallsort = new xivo_sort(array('browse' => 'outcall','key' => 'name'));
				uasort($rcalloutcall['slt'],array(&$outcallsort,'str_usort'));
			}
		}

		if(xivo_issa('rcallexten',$result) === true)
			$rcallexten = $result['rcallexten'];
		else
			$rcallexten = null;

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		$_HTML->set_var('rcalluser',$rcalluser);
		$_HTML->set_var('rcallgroup',$rcallgroup);
		$_HTML->set_var('rcallincall',$rcallincall);
		$_HTML->set_var('rcalloutcall',$rcalloutcall);
		$_HTML->set_var('rcallexten',$rcallexten);
		$_HTML->set_var('element',$apprightcall->get_elements());
		$_HTML->set_var('info',$result);
		break;
	case 'edit':
		$apprightcall = &$ipbx->get_application('rightcall');

		if(isset($_QR['id']) === false || ($info = $apprightcall->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);

		$result = null;
		$return = &$info;

		$rcalluser = $rcallgroup = $rcallincall = $rcalloutcall = array();
		$rcalluser['slt'] = $rcallgroup['slt'] = $rcallincall['slt'] = $rcalloutcall['slt'] = null;

		$userorder = array();
		$userorder['firstname'] = SORT_ASC;
		$userorder['lastname'] = SORT_ASC;
		$userorder['number'] = SORT_ASC;
		$userorder['context'] = SORT_ASC;
		$userorder['name'] = SORT_ASC;

		$appuser = &$ipbx->get_application('user',null,false);
		$rcalluser['list'] = $appuser->get_users_list(null,null,$userorder,null,true);

		$appgroup = &$ipbx->get_application('group',null,false);
		$rcallgroup['list'] = $appgroup->get_groups_list(null,array('name' => SORT_ASC),null,true);

		$appincall = &$ipbx->get_application('incall',null,false);
		$rcallincall['list'] = $appincall->get_incalls_list(null,array('exten' => SORT_ASC),null,true);

		$appoutcall = &$ipbx->get_application('outcall',null,false);
		$rcalloutcall['list'] = $appoutcall->get_outcalls_list(null,array('name' => SORT_ASC),null,true);

		if(isset($_QR['fm_send']) === true && xivo_issa('rightcall',$_QR) === true)
		{
			$return = &$result;

			if($apprightcall->set_edit($_QR) === false
			|| $apprightcall->edit() === false)
				$result = $apprightcall->get_result();
			else
				$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);
		}

		xivo::load_class('xivo_sort');

		if($rcalluser['list'] !== false && xivo_ak('rightcalluser',$return) === true)
		{
			$rcalluser['slt'] = xivo_array_intersect_key($return['rightcalluser'],$rcalluser['list'],'typeval');

			if($rcalluser['slt'] !== false)
			{
				$rcalluser['list'] = xivo_array_diff_key($rcalluser['list'],$rcalluser['slt']);

				$usersort = new xivo_sort(array('key' => 'identity'));
				uasort($rcalluser['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($rcallgroup['list'] !== false && xivo_ak('rightcallgroup',$return) === true)
		{
			$rcallgroup['slt'] = xivo_array_intersect_key($return['rightcallgroup'],$rcallgroup['list'],'typeval');
			
			if($rcallgroup['slt'] !== false)
			{
				$rcallgroup['list'] = xivo_array_diff_key($rcallgroup['list'],$rcallgroup['slt']);
		
				$groupsort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));
				uasort($rcallgroup['slt'],array(&$groupsort,'str_usort'));
			}
		}

		if($rcallincall['list'] !== false && xivo_ak('rightcallincall',$return) === true)
		{
			$rcallincall['slt'] = xivo_array_intersect_key($return['rightcallincall'],$rcallincall['list'],'typeval');
			
			if($rcallincall['slt'] !== false)
			{
				$rcallincall['list'] = xivo_array_diff_key($rcallincall['list'],$rcallincall['slt']);

				$incallsort = new xivo_sort(array('browse' => 'incall','key' => 'exten'));
				uasort($rcallincall['slt'],array(&$incallsort,'str_usort'));
			}
		}

		if($rcalloutcall['list'] !== false && xivo_ak('rightcalloutcall',$return) === true)
		{
			$rcalloutcall['slt'] = xivo_array_intersect_key($return['rightcalloutcall'],$rcalloutcall['list'],'typeval');
			
			if($rcalloutcall['slt'] !== false)
			{
				$rcalloutcall['list'] = xivo_array_diff_key($rcalloutcall['list'],$rcalloutcall['slt']);

				$outcallsort = new xivo_sort(array('browse' => 'outcall','key' => 'name'));
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

		$_HTML->set_var('id',$info['rightcall']['id']);
		$_HTML->set_var('rcalluser',$rcalluser);
		$_HTML->set_var('rcallgroup',$rcallgroup);
		$_HTML->set_var('rcallincall',$rcallincall);
		$_HTML->set_var('rcalloutcall',$rcalloutcall);
		$_HTML->set_var('rcallexten',$rcallexten);
		$_HTML->set_var('element',$apprightcall->get_elements());
		$_HTML->set_var('info',$return);
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
			if($apprightcall->get($values[$i]) !== false)
				$apprightcall->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/rightcall'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables';

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

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$list);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/rightcall');

$_HTML->set_var('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/rightcall/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
