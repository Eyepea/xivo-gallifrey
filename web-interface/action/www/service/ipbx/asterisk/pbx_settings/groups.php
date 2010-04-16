<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2010  Proformatique <technique@proformatique.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';

$info = $result = array();

switch($act)
{
	case 'add':
		$appgroup = &$ipbx->get_application('group');

		$result = $fm_save = $error = null;

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
		&& dwho_issa('groupfeatures',$_QR) === true
		&& dwho_issa('queue',$_QR) === true)
		{
			if($appgroup->set_add($_QR) === false
			|| $appgroup->add() === false)
			{
				$fm_save = false;
				$result = $appgroup->get_result();
				$error = $appgroup->get_error();
				$result['dialaction'] = $appgroup->get_dialaction_result();
			}
			else
			{
				$ipbx->discuss('xivo[grouplist,update]');
				$_QRY->go($_TPL->url('service/ipbx/pbx_settings/groups'),$param);
			}
		}

		dwho::load_class('dwho_sort');

		if($user['list'] !== false && dwho_ak('user',$result) === true)
		{
			$user['slt'] = dwho_array_intersect_key($result['user'],$user['list'],'userid');

			if($user['slt'] !== false)
			{
				$user['list'] = dwho_array_diff_key($user['list'],$user['slt']);

				$usersort = new dwho_sort(array('key' => 'identity'));
				uasort($user['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($rightcall['list'] !== false && dwho_ak('rightcall',$result) === true)
		{
			$rightcall['slt'] = dwho_array_intersect_key($result['rightcall'],$rightcall['list'],'rightcallid');

			if($rightcall['slt'] !== false)
			{
				$rightcall['list'] = dwho_array_diff_key($rightcall['list'],$rightcall['slt']);

				$rightcallsort = new dwho_sort(array('browse' => 'rightcall','key' => 'name'));
				uasort($rightcall['slt'],array(&$rightcallsort,'str_usort'));
			}
		}

		if(empty($result) === false)
		{
			if(dwho_issa('dialaction',$result) === false || empty($result['dialaction']) === true)
				$result['dialaction'] = null;

			if(dwho_issa('callerid',$result) === false || empty($result['callerid']) === true)
				$result['callerid'] = null;
		}

		$_TPL->set_var('info',$result);
		$_TPL->set_var('error',$error);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('dialaction',$result['dialaction']);
		$_TPL->set_var('dialaction_from','group');
		$_TPL->set_var('element',$appgroup->get_elements());
		$_TPL->set_var('user',$user);
		$_TPL->set_var('rightcall',$rightcall);
		$_TPL->set_var('destination_list',$appgroup->get_dialaction_destination_list());
		$_TPL->set_var('moh_list',$appgroup->get_musiconhold());
		$_TPL->set_var('context_list',$appgroup->get_context_list());

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/uri.js');
		$dhtml->set_js('js/dwho/http.js');
		$dhtml->set_js('js/dwho/suggest.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/meetme.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callerid.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/groups.js');
		$dhtml->set_js('js/dwho/submenu.js');
		break;
	case 'edit':
		$appgroup = &$ipbx->get_application('group');

		if(isset($_QR['id']) === false || ($info = $appgroup->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/groups'),$param);

		$result = $fm_save = $error = null;
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
		&& dwho_issa('groupfeatures',$_QR) === true
		&& dwho_issa('queue',$_QR) === true)
		{
			$return = &$result;

			if($appgroup->set_edit($_QR) === false
			|| $appgroup->edit() === false)
			{
				$fm_save = false;
				$result = $appgroup->get_result();
				$error = $appgroup->get_error();
				$result['dialaction'] = $appgroup->get_dialaction_result();
			}
			else
			{
				$ipbx->discuss('xivo[grouplist,update]');
				$_QRY->go($_TPL->url('service/ipbx/pbx_settings/groups'),$param);
			}
		}

		dwho::load_class('dwho_sort');

		if($user['list'] !== false && dwho_ak('user',$return) === true)
		{
			$user['slt'] = dwho_array_intersect_key($return['user'],$user['list'],'userid');

			if($user['slt'] !== false)
			{
				$user['list'] = dwho_array_diff_key($user['list'],$user['slt']);

				$usersort = new dwho_sort(array('key' => 'identity'));
				uasort($user['slt'],array(&$usersort,'str_usort'));
			}
		}

		if($rightcall['list'] !== false && dwho_ak('rightcall',$return) === true)
		{
			$rightcall['slt'] = dwho_array_intersect_key($return['rightcall'],$rightcall['list'],'rightcallid');

			if($rightcall['slt'] !== false)
			{
				$rightcall['list'] = dwho_array_diff_key($rightcall['list'],$rightcall['slt']);

				$rightcallsort = new dwho_sort(array('browse' => 'rightcall','key' => 'name'));
				uasort($rightcall['slt'],array(&$rightcallsort,'str_usort'));
			}
		}

		if(empty($return) === false)
		{
			if(dwho_issa('dialaction',$return) === false || empty($return['dialaction']) === true)
				$return['dialaction'] = null;

			if(dwho_issa('callerid',$return) === false || empty($return['callerid']) === true)
				$return['callerid'] = null;
		}

		$_TPL->set_var('id',$info['groupfeatures']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('error',$error);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('dialaction',$return['dialaction']);
		$_TPL->set_var('dialaction_from','group');
		$_TPL->set_var('element',$appgroup->get_elements());
		$_TPL->set_var('user',$user);
		$_TPL->set_var('rightcall',$rightcall);
		$_TPL->set_var('destination_list',$appgroup->get_dialaction_destination_list());
		$_TPL->set_var('moh_list',$appgroup->get_musiconhold());
		$_TPL->set_var('context_list',$appgroup->get_context_list());

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callerid.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/groups.js');
		$dhtml->set_js('js/dwho/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appgroup = &$ipbx->get_application('group');

		if(isset($_QR['id']) === false || $appgroup->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/groups'),$param);

		$appgroup->delete();

		$ipbx->discuss('xivo[grouplist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/groups'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('groups',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/groups'),$param);

		$appgroup = &$ipbx->get_application('group');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appgroup->get($values[$i]) !== false)
				$appgroup->delete();
		}

		$ipbx->discuss('xivo[grouplist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/groups'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables';
		$invdisable = $disable === false;

		if(($values = dwho_issa_val('groups',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/groups'),$param);

		$groupfeatures = &$ipbx->get_module('groupfeatures');
		$queue = &$ipbx->get_module('queue');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $groupfeatures->get($values[$i])) !== false)
				$queue->disable($info['name'],$disable);
		}

		$ipbx->discuss('xivo[grouplist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/groups'),$param);
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
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/groups'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/groups/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
