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

		$userorder = array();
		$userorder['firstname'] = SORT_ASC;
		$userorder['lastname'] = SORT_ASC;
		$userorder['number'] = SORT_ASC;
		$userorder['context'] = SORT_ASC;
		$userorder['name'] = SORT_ASC;

		$appuser = &$ipbx->get_application('user',null,false);
		$user['list'] = $appuser->get_users_list(null,null,$userorder,null,true);

		xivo::load_class('xivo_sort');

		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		if(isset($_QR['fm_send']) === true
		&& xivo_issa('gfeatures',$_QR) === true
		&& xivo_issa('queue',$_QR) === true)
		{
			if($appgroup->set_add($_QR) === false
			|| $appgroup->add() === false)
			{
				$result = $appgroup->get_result();
				$result['dialstatus'] = $appgroup->get_dialstatus_result();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		}

		if($user['list'] !== false && xivo_ak('user',$result) === true)
		{
			$user['slt'] = xivo_array_intersect_key($result['user'],$user['list'],'userid');
			
			if($user['slt'] !== false)
			{
				$user['list'] = xivo_array_diff_key($user['list'],$user['slt']);

				$usersort = new xivo_sort(array('key' => 'identity'));
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

		$_HTML->set_var('info',$result);
		$_HTML->set_var('dialstatus',$result['dialstatus']);
		$_HTML->set_var('element',$appgroup->get_elements());
		$_HTML->set_var('user',$user);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('dialstatus_list',$appgroup->get_dialstatus_destination_list());
		$_HTML->set_var('moh_list',$appgroup->get_musiconhold());

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

		$userorder = array();
		$userorder['firstname'] = SORT_ASC;
		$userorder['lastname'] = SORT_ASC;
		$userorder['number'] = SORT_ASC;
		$userorder['context'] = SORT_ASC;
		$userorder['name'] = SORT_ASC;

		$appuser = &$ipbx->get_application('user',null,false);
		$user['list'] = $appuser->get_users_list(null,null,$userorder,null,true);

		xivo::load_class('xivo_sort');

		$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));

		if(($rightcall['list'] = $ipbx->get_rightcall_list(null,true)) !== false)
			uasort($rightcall['list'],array(&$rightcallsort,'str_usort'));

		if(isset($_QR['fm_send']) === true
		&& xivo_issa('gfeatures',$_QR) === true
		&& xivo_issa('queue',$_QR) === true)
		{
			$return = &$result;

			if($appgroup->set_edit($_QR) === false
			|| $appgroup->edit() === false)
			{
				$result = $appgroup->get_result();
				$result['dialstatus'] = $appgroup->get_dialstatus_result();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		}

		if($user['list'] !== false && xivo_ak('user',$return) === true)
		{
			$user['slt'] = xivo_array_intersect_key($return['user'],$user['list'],'userid');
			
			if($user['slt'] !== false)
			{
				$user['list'] = xivo_array_diff_key($user['list'],$user['slt']);

				$usersort = new xivo_sort(array('key' => 'identity'));
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

		$_HTML->set_var('id',$info['gfeatures']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('dialstatus',$return['dialstatus']);
		$_HTML->set_var('user',$user);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('element',$appgroup->get_elements());
		$_HTML->set_var('dialstatus_list',$appgroup->get_dialstatus_destination_list());
		$_HTML->set_var('moh_list',$appgroup->get_musiconhold());

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
			if($appgroup->get($values[$i]) !== false)
				$appgroup->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables';
		$invdisable = $disable === false;

		if(($values = xivo_issa_val('groups',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		$gfeatures = &$ipbx->get_module('groupfeatures');
		$queue = &$ipbx->get_module('queue');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $gfeatures->get($values[$i])) !== false)
				$queue->disable($info['name'],$disable);
		}

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		break;
	default:
		$act = 'list';
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appgroup = &$ipbx->get_application('group',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = ($page - 1) * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appgroup->get_groups_list(null,$order,$limit);
		$total = $appgroup->get_cnt();

		if($list === false && $total > 0)
		{
			$param['page'] = $page - 1;
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,20,$total));
		$_HTML->set_var('list',$list);
}

$_HTML->set_var('act',$act);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups');

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
