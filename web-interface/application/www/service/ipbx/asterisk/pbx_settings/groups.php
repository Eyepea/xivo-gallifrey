<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? xivo_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';

$info = $result = array();

switch($act)
{
	case 'add':
		$appgroup = &$ipbx->get_application('group');

		$result = null;

		$user = $rightcall = array();
		$user['slt'] = $rightcall['slt'] = array();

		xivo::load_class('xivo_sort');
		$usersort = new xivo_sort(array('browse' => 'ufeatures','key' => 'identity'));

		if(($user['list'] = $ipbx->get_users_list(null,null,true)) !== false)
			uasort($user['list'],array(&$usersort,'str_usort'));

		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('gfeatures',$_QR) === false
			|| xivo_issa('queue',$_QR) === false)
				break;

			if($appgroup->set_add($_QR) === false
			|| $appgroup->add() === false)
			{
				$result = $appgroup->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		}
		while(false);

		if($user['list'] !== false && xivo_ak('user',$result) === true)
		{
			$user['slt'] = xivo_array_intersect_key($result['user'],$user['list'],'userid');
			
			if($user['slt'] !== false)
			{
				$user['list'] = xivo_array_diff_key($user['list'],$user['slt']);
				uasort($user['slt'],array(&$usersort,'str_usort'));
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

		if(empty($result) === false
		&& (xivo_issa('dialstatus',$result) === false || empty($result['dialstatus']) === true) === true)
			$result['dialstatus'] = null;

		$_HTML->assign('info',$result);
		$_HTML->assign('dialstatus',$result['dialstatus']);
		$_HTML->assign('element',$appgroup->get_elements());
		$_HTML->assign('user',$user);
		$_HTML->assign('rightcall',$rightcall);
		$_HTML->assign('list',$appgroup->get_dialstatus_destination_list());
		$_HTML->assign('moh_list',$appgroup->get_musiconhold());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialstatus.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'edit':
		$appgroup = &$ipbx->get_application('group');

		if(isset($_QR['id']) === false || ($info = $appgroup->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		$result = null;
		$return = &$info;

		$user = $rightcall = array();
		$user['slt'] = $rightcall['slt'] = array();

		xivo::load_class('xivo_sort');
		$usersort = new xivo_sort(array('browse' => 'ufeatures','key' => 'identity'));

		if(($user['list'] = $ipbx->get_users_list(null,null,true)) !== false)
			uasort($user['list'],array(&$usersort,'str_usort'));

		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		do
		{
			if(isset($_QR['fm_send']) === false
			|| xivo_issa('gfeatures',$_QR) === false
			|| xivo_issa('queue',$_QR) === false)
					break;

			$return = &$result;

			if($appgroup->set_edit($_QR) === false
			|| $appgroup->edit() === false)
			{
				$result = $appgroup->get_result();
				break;
			}

			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		}
		while(false);

		if($user['list'] !== false && xivo_ak('user',$return) === true)
		{
			$user['slt'] = xivo_array_intersect_key($return['user'],$user['list'],'userid');
			
			if($user['slt'] !== false)
			{
				$user['list'] = xivo_array_diff_key($user['list'],$user['slt']);
				uasort($user['slt'],array(&$usersort,'str_usort'));
			}
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

		if(empty($return) === false
		&& (xivo_issa('dialstatus',$return) === false || empty($return['dialstatus']) === true) === true)
			$return['dialstatus'] = null;

		$_HTML->assign('id',$info['gfeatures']['id']);
		$_HTML->assign('info',$return);
		$_HTML->assign('dialstatus',$return['dialstatus']);
		$_HTML->assign('user',$user);
		$_HTML->assign('rightcall',$rightcall);
		$_HTML->assign('element',$appgroup->get_elements());
		$_HTML->assign('list',$appgroup->get_dialstatus_destination_list());
		$_HTML->assign('moh_list',$appgroup->get_musiconhold());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialstatus.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appgroup = &$ipbx->get_application('group');

		if(isset($_QR['id']) === false || $appgroup->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		$appgroup->delete();

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('groups',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		$appgroup = &$ipbx->get_application('group');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appgroup->get($values[$i]) === false)
				continue;

			$appgroup->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables' ? true : false;
		$invdisable = $disable === true ? false : true;

		if(($values = xivo_issa_val('groups',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		$gfeatures = &$ipbx->get_module('groupfeatures');
		$queue = &$ipbx->get_module('queue');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $gfeatures->get($values[$i])) === false)
				continue;

			$queue->disable($info['name'],$disable);
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		break;
	default:
		$act = 'list';
		$total = 0;

		if(($groups = $ipbx->get_groups_list()) !== false)
		{
			$total = count($groups);
			xivo::load_class('xivo_sort');
			$sort = new xivo_sort(array('browse' => 'gfeatures','key' => 'name'));
			usort($groups,array(&$sort,'str_usort'));
		}

		$_HTML->assign('pager',xivo_calc_page($page,20,$total));
		$_HTML->assign('list',$groups);
}

$_HTML->assign('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
