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

		$apprightcall = &$ipbx->get_application('rightcall',null,false);
		$rightcall['list'] = $apprightcall->get_rightcalls_list(null,array('name' => SORT_ASC),null,true);

		if(isset($_QR['fm_send']) === true
		&& xivo_issa('gfeatures',$_QR) === true
		&& xivo_issa('queue',$_QR) === true)
		{
			if($appgroup->set_add($_QR) === false
			|| $appgroup->add() === false)
			{
				$result = $appgroup->get_result();
				$result['dialaction'] = $appgroup->get_dialaction_result();
			}
			else
			{
				$ipbx->discuss('xivo[grouplist,update]');
				$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
			}
		}

		xivo::load_class('xivo_sort');

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

				$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));
				uasort($rightcall['slt'],array(&$rightcallsort,'str_usort'));
			}
		}

		if(empty($result) === false)
		{
			if(xivo_issa('dialaction',$result) === false || empty($result['dialaction']) === true)
				$result['dialaction'] = null;

			if(xivo_issa('callerid',$result) === false || empty($result['callerid']) === true)
				$result['callerid'] = null;
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('dialaction',$result['dialaction']);
		$_HTML->set_var('dialaction_from','group');
		$_HTML->set_var('element',$appgroup->get_elements());
		$_HTML->set_var('user',$user);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('destination_list',$appgroup->get_dialaction_destination_list());
		$_HTML->set_var('moh_list',$appgroup->get_musiconhold());
		$_HTML->set_var('context_list',$appgroup->get_context_list());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callerid.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/groups.js');
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

		$apprightcall = &$ipbx->get_application('rightcall',null,false);
		$rightcall['list'] = $apprightcall->get_rightcalls_list(null,array('name' => SORT_ASC),null,true);

		if(isset($_QR['fm_send']) === true
		&& xivo_issa('gfeatures',$_QR) === true
		&& xivo_issa('queue',$_QR) === true)
		{
			$return = &$result;

			if($appgroup->set_edit($_QR) === false
			|| $appgroup->edit() === false)
			{
				$result = $appgroup->get_result();
				$result['dialaction'] = $appgroup->get_dialaction_result();
			}
			else
			{
				$ipbx->discuss('xivo[grouplist,update]');
				$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
			}
		}

		xivo::load_class('xivo_sort');

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

				$rightcallsort = new xivo_sort(array('browse' => 'rightcall','key' => 'name'));
				uasort($rightcall['slt'],array(&$rightcallsort,'str_usort'));
			}
		}

		if(empty($return) === false)
		{
			if(xivo_issa('dialaction',$return) === false || empty($return['dialaction']) === true)
				$return['dialaction'] = null;

			if(xivo_issa('callerid',$return) === false || empty($return['callerid']) === true)
				$return['callerid'] = null;
		}

		$_HTML->set_var('id',$info['gfeatures']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('dialaction',$return['dialaction']);
		$_HTML->set_var('dialaction_from','group');
		$_HTML->set_var('element',$appgroup->get_elements());
		$_HTML->set_var('user',$user);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('destination_list',$appgroup->get_dialaction_destination_list());
		$_HTML->set_var('moh_list',$appgroup->get_musiconhold());
		$_HTML->set_var('context_list',$appgroup->get_context_list());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callerid.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/groups.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appgroup = &$ipbx->get_application('group');

		if(isset($_QR['id']) === false || $appgroup->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);

		$appgroup->delete();

		$ipbx->discuss('xivo[grouplist,update]');

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

		$ipbx->discuss('xivo[grouplist,update]');

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

		$ipbx->discuss('xivo[grouplist,update]');

		$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appgroup = &$ipbx->get_application('group',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appgroup->get_groups_list(null,$order,$limit);
		$total = $appgroup->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_HTML->url('service/ipbx/pbx_settings/groups'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
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
