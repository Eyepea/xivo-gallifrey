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

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appoutcall = &$ipbx->get_application('outcall');

		$result = $fm_save = null;

		$outcalltrunk = $rightcall = array();
		$outcalltrunk['slt'] = $rightcall['slt'] = array();

		xivo::load_class('xivo_sort');

		$apptrunk = &$ipbx->get_application('trunk',null,false);
		if(($outcalltrunk['list'] = $apptrunk->get_trunks_list(null,null,null,null,true)) !== false)
		{
			$trunksort = new xivo_sort(array('key' => 'identity'));
			uasort($outcalltrunk['list'],array(&$trunksort,'str_usort'));
		}

		$apprightcall = &$ipbx->get_application('rightcall',null,false);
		$rightcall['list'] = $apprightcall->get_rightcalls_list(null,array('name' => SORT_ASC),null,true);

		if(isset($_QR['fm_send']) === true && xivo_issa('outcall',$_QR) === true)
		{
			if($appoutcall->set_add($_QR) === false
			|| $appoutcall->add() === false)
			{
				$fm_save = false;
				$result = $appoutcall->get_result();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		}

		if($outcalltrunk['list'] !== false && xivo_issa('outcalltrunk',$result) === true)
		{
			$outcalltrunksort = new xivo_sort(array('key' => 'priority'));
			usort($result['outcalltrunk'],array(&$outcalltrunksort,'num_usort'));

			$outcalltrunk['slt'] = xivo_array_intersect_key($result['outcalltrunk'],$outcalltrunk['list'],'trunkfeaturesid');

			if($outcalltrunk['slt'] !== false)
				$outcalltrunk['list'] = xivo_array_diff_key($outcalltrunk['list'],$outcalltrunk['slt']);
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

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/outcall.js');

		$_HTML->set_var('outcalltrunk',$outcalltrunk);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('fm_save',$fm_save);
		$_HTML->set_var('context_list',$appoutcall->get_context_list());
		$_HTML->set_var('element',$appoutcall->get_elements());
		$_HTML->set_var('info',$result);
		break;
	case 'edit':
		$appoutcall = &$ipbx->get_application('outcall');

		if(isset($_QR['id']) === false || ($info = $appoutcall->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$result = $fm_save = null;
		$return = &$info;
		$outcalltrunk = $rightcall = array();
		$outcalltrunk['slt'] = $rightcall['slt'] = array();

		xivo::load_class('xivo_sort');

		$apptrunk = &$ipbx->get_application('trunk',null,false);
		if(($outcalltrunk['list'] = $apptrunk->get_trunks_list(null,null,null,null,true)) !== false)
		{
			$trunksort = new xivo_sort(array('key' => 'identity'));
			uasort($outcalltrunk['list'],array(&$trunksort,'str_usort'));
		}

		$apprightcall = &$ipbx->get_application('rightcall',null,false);
		$rightcall['list'] = $apprightcall->get_rightcalls_list(null,array('name' => SORT_ASC),null,true);

		if(isset($_QR['fm_send']) === true && xivo_issa('outcall',$_QR) === true)
		{
			$return = &$result;

			if($appoutcall->set_edit($_QR) === false
			|| $appoutcall->edit() === false)
			{
				$fm_save = false;
				$result = $appoutcall->get_result();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		}

		if($outcalltrunk['list'] !== false && xivo_issa('outcalltrunk',$return) === true)
		{
			$outcalltrunksort = new xivo_sort(array('key' => 'priority'));
			usort($return['outcalltrunk'],array(&$outcalltrunksort,'num_usort'));

			$outcalltrunk['slt'] = xivo_array_intersect_key($return['outcalltrunk'],$outcalltrunk['list'],'trunkfeaturesid');

			if($outcalltrunk['slt'] !== false)
				$outcalltrunk['list'] = xivo_array_diff_key($outcalltrunk['list'],$outcalltrunk['slt']);
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

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/outcall.js');

		$_HTML->set_var('id',$info['outcall']['id']);
		$_HTML->set_var('outcalltrunk',$outcalltrunk);
		$_HTML->set_var('rightcall',$rightcall);
		$_HTML->set_var('fm_save',$fm_save);
		$_HTML->set_var('context_list',$appoutcall->get_context_list());
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
			if($appoutcall->get($values[$i]) !== false)
				$appoutcall->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('outcalls',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);

		$appoutcall = &$ipbx->get_application('outcall',null,false);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appoutcall->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appoutcall->disable();
			else
				$appoutcall->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/outcall'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appoutcall = &$ipbx->get_application('outcall',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appoutcall->get_outcalls_list(null,$order,$limit);
		$total = $appoutcall->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
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
