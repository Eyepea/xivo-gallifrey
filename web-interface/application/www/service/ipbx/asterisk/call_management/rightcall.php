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

		$grouporder = array();
		$grouporder['name'] = SORT_ASC;
		$grouporder['number'] = SORT_ASC;
		$grouporder['context'] = SORT_ASC;

		$appgroup = &$ipbx->get_application('group',null,false);
		$rcallgroup['list'] = $appgroup->get_groups_list(null,$grouporder,null,true);

		$incallorder = array();
		$incallorder['exten'] = SORT_ASC;
		$incallorder['context'] = SORT_ASC;

		$appincall = &$ipbx->get_application('incall',null,false);
		$rcallincall['list'] = $appincall->get_incalls_list(null,$incallorder,null,true);

		$outcallorder = array();
		$outcallorder['name'] = SORT_ASC;
		$outcallorder['context'] = SORT_ASC;

		$appoutcall = &$ipbx->get_application('outcall',null,false);
		$rcalloutcall['list'] = $appoutcall->get_outcalls_list(null,$outcallorder,null,true);

		if(isset($_QR['fm_send']) === true && xivo_issa('rightcall',$_QR) === true)
		{
			if($apprightcall->set_add($_QR) === false
			|| $apprightcall->add() === false)
				$result = $apprightcall->get_result();
			else
				$_QRY->go($_TPL->url('service/ipbx/call_management/rightcall'),$param);
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

				$groupsort = new xivo_sort(array('key' => 'name'));
				uasort($rcallgroup['slt'],array(&$groupsort,'str_usort'));
			}
		}

		if($rcallincall['list'] !== false && xivo_ak('rightcallincall',$result) === true)
		{
			$rcallincall['slt'] = xivo_array_intersect_key($result['rightcallincall'],$rcallincall['list'],'typeval');

			if($rcallincall['slt'] !== false)
			{
				$rcallincall['list'] = xivo_array_diff_key($rcallincall['list'],$rcallincall['slt']);

				$incallsort = new xivo_sort(array('key' => 'exten'));
				uasort($rcallincall['slt'],array(&$incallsort,'str_usort'));
			}
		}

		if($rcalloutcall['list'] !== false && xivo_ak('rightcalloutcall',$result) === true)
		{
			$rcalloutcall['slt'] = xivo_array_intersect_key($result['rightcalloutcall'],$rcalloutcall['list'],'typeval');

			if($rcalloutcall['slt'] !== false)
			{
				$rcalloutcall['list'] = xivo_array_diff_key($rcalloutcall['list'],$rcalloutcall['slt']);

				$outcallsort = new xivo_sort(array('key' => 'name'));
				uasort($rcalloutcall['slt'],array(&$outcallsort,'str_usort'));
			}
		}

		if(xivo_issa('rcallexten',$result) === true)
			$rcallexten = $result['rcallexten'];
		else
			$rcallexten = null;

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		$_TPL->set_var('rcalluser',$rcalluser);
		$_TPL->set_var('rcallgroup',$rcallgroup);
		$_TPL->set_var('rcallincall',$rcallincall);
		$_TPL->set_var('rcalloutcall',$rcalloutcall);
		$_TPL->set_var('rcallexten',$rcallexten);
		$_TPL->set_var('context_list',$apprightcall->get_context_list());
		$_TPL->set_var('element',$apprightcall->get_elements());
		$_TPL->set_var('info',$result);
		break;
	case 'edit':
		$apprightcall = &$ipbx->get_application('rightcall');

		if(isset($_QR['id']) === false || ($info = $apprightcall->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/rightcall'),$param);

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

		$grouporder = array();
		$grouporder['name'] = SORT_ASC;
		$grouporder['number'] = SORT_ASC;
		$grouporder['context'] = SORT_ASC;

		$appgroup = &$ipbx->get_application('group',null,false);
		$rcallgroup['list'] = $appgroup->get_groups_list(null,$grouporder,null,true);

		$incallorder = array();
		$incallorder['exten'] = SORT_ASC;
		$incallorder['context'] = SORT_ASC;

		$appincall = &$ipbx->get_application('incall',null,false);
		$rcallincall['list'] = $appincall->get_incalls_list(null,$incallorder,null,true);

		$outcallorder = array();
		$outcallorder['name'] = SORT_ASC;
		$outcallorder['context'] = SORT_ASC;

		$appoutcall = &$ipbx->get_application('outcall',null,false);
		$rcalloutcall['list'] = $appoutcall->get_outcalls_list(null,$outcallorder,null,true);

		if(isset($_QR['fm_send']) === true && xivo_issa('rightcall',$_QR) === true)
		{
			$return = &$result;

			if($apprightcall->set_edit($_QR) === false
			|| $apprightcall->edit() === false)
				$result = $apprightcall->get_result();
			else
				$_QRY->go($_TPL->url('service/ipbx/call_management/rightcall'),$param);
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

				$groupsort = new xivo_sort(array('key' => 'name'));
				uasort($rcallgroup['slt'],array(&$groupsort,'str_usort'));
			}
		}

		if($rcallincall['list'] !== false && xivo_ak('rightcallincall',$return) === true)
		{
			$rcallincall['slt'] = xivo_array_intersect_key($return['rightcallincall'],$rcallincall['list'],'typeval');

			if($rcallincall['slt'] !== false)
			{
				$rcallincall['list'] = xivo_array_diff_key($rcallincall['list'],$rcallincall['slt']);

				$incallsort = new xivo_sort(array('key' => 'exten'));
				uasort($rcallincall['slt'],array(&$incallsort,'str_usort'));
			}
		}

		if($rcalloutcall['list'] !== false && xivo_ak('rightcalloutcall',$return) === true)
		{
			$rcalloutcall['slt'] = xivo_array_intersect_key($return['rightcalloutcall'],$rcalloutcall['list'],'typeval');

			if($rcalloutcall['slt'] !== false)
			{
				$rcalloutcall['list'] = xivo_array_diff_key($rcalloutcall['list'],$rcalloutcall['slt']);

				$outcallsort = new xivo_sort(array('key' => 'name'));
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

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

		$_TPL->set_var('id',$info['rightcall']['id']);
		$_TPL->set_var('rcalluser',$rcalluser);
		$_TPL->set_var('rcallgroup',$rcallgroup);
		$_TPL->set_var('rcallincall',$rcallincall);
		$_TPL->set_var('rcalloutcall',$rcalloutcall);
		$_TPL->set_var('rcallexten',$rcallexten);
		$_TPL->set_var('context_list',$apprightcall->get_context_list());
		$_TPL->set_var('element',$apprightcall->get_elements());
		$_TPL->set_var('info',$return);
		break;
	case 'delete':
		$param['page'] = $page;

		$apprightcall = &$ipbx->get_application('rightcall');

		if(isset($_QR['id']) === false || $apprightcall->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/rightcall'),$param);

		$apprightcall->delete();

		$_QRY->go($_TPL->url('service/ipbx/call_management/rightcall'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('rightcalls',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/rightcall'),$param);

		$apprightcall = &$ipbx->get_application('rightcall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($apprightcall->get($values[$i]) !== false)
				$apprightcall->delete();
		}

		$_QRY->go($_TPL->url('service/ipbx/call_management/rightcall'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;
		$disable = $act === 'disables';

		if(($values = xivo_issa_val('rightcalls',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/rightcall'),$param);

		$rightcall = &$ipbx->get_module('rightcall');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$rightcall->disable($values[$i],$disable);

		$_QRY->go($_TPL->url('service/ipbx/call_management/rightcall'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$apprightcall = &$ipbx->get_application('rightcall',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $apprightcall->get_rightcalls_list(null,$order,$limit);
		$total = $apprightcall->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/call_management/rightcall'),$param);
		}

		$_TPL->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/rightcall');

$_TPL->set_var('act',$act);
$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/rightcall/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
