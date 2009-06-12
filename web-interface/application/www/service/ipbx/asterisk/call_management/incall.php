<?php

#
# XiVO Web-Interface
# Copyright (C) 2006-2009  Proformatique <technique@proformatique.com>
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

		$result = $fm_save = null;
		$rightcall['slt'] = $rightcall = array();

		$apprightcall = &$ipbx->get_application('rightcall',null,false);
		$rightcall['list'] = $apprightcall->get_rightcalls_list(null,array('name' => SORT_ASC),null,true);

		if(isset($_QR['fm_send']) === true && xivo_issa('incall',$_QR) === true)
		{
			if($appincall->set_add($_QR) === false
			|| $appincall->add() === false)
			{
				$fm_save = false;
				$result = $appincall->get_result();
				$result['dialaction'] = $appincall->get_dialaction_result();
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

				xivo::load_class('xivo_sort');
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

		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('incall',$result['incall']);
		$_HTML->set_var('dialaction',$result['dialaction']);
		$_HTML->set_var('dialaction_from','incall');
		$_HTML->set_var('callerid',$result['callerid']);
		$_HTML->set_var('fm_save',$fm_save);
		$_HTML->set_var('element',$appincall->get_elements());
		$_HTML->set_var('destination_list',$appincall->get_dialaction_destination_list());
		$_HTML->set_var('context_list',$appincall->get_context_list());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callerid.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/incall.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'edit':
		$appincall = &$ipbx->get_application('incall');

		if(isset($_QR['id']) === false || ($info = $appincall->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/incall'),$param);

		$result = $fm_save = null;
		$return = &$info;
		$rightcall['slt'] = $rightcall = array();

		$apprightcall = &$ipbx->get_application('rightcall',null,false);
		$rightcall['list'] = $apprightcall->get_rightcalls_list(null,array('name' => SORT_ASC),null,true);

		if(isset($_QR['fm_send']) === true && xivo_issa('incall',$_QR) === true)
		{
			$return = &$result;

			if($appincall->set_edit($_QR) === false
			|| $appincall->edit() === false)
			{
				$fm_save = false;
				$result = $appincall->get_result();
				$result['dialaction'] = $appincall->get_dialaction_result();
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

				xivo::load_class('xivo_sort');
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

		$_HTML->set_var('id',$info['incall']['id']);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('incall',$return['incall']);
		$_HTML->set_var('dialaction',$return['dialaction']);
		$_HTML->set_var('dialaction_from','incall');
		$_HTML->set_var('callerid',$return['callerid']);
		$_HTML->set_var('fm_save',$fm_save);
		$_HTML->set_var('element',$appincall->get_elements());
		$_HTML->set_var('destination_list',$appincall->get_dialaction_destination_list());
		$_HTML->set_var('context_list',$appincall->get_context_list());

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callerid.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/incall.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
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
