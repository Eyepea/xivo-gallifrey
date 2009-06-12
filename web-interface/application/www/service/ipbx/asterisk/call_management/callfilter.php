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
		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'));

		$result = $fm_save = $callfiltermember = null;

		$secretary['slt'] = $secretary = array();
		$secretary['list'] = $appcallfilter->get_secretary_users(null,true);

		if(isset($_QR['fm_send']) === true && xivo_issa('callfilter',$_QR) === true)
		{
			if($appcallfilter->set_add($_QR) === false
			|| $appcallfilter->add() === false)
			{
				$fm_save = false;
				$result = $appcallfilter->get_result();
				$result['dialaction'] = $appcallfilter->get_dialaction_result();

				if(xivo_issa('callfiltermember',$result) === true)
					$callfiltermember = &$result['callfiltermember'];
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		}

		if($secretary['list'] !== false && xivo_issa('secretary',$callfiltermember) === true)
		{
			xivo::load_class('xivo_sort');
			$secretarysort = new xivo_sort(array('key' => 'priority'));
			usort($callfiltermember['secretary'],array(&$secretarysort,'num_usort'));

			$secretary['slt'] = xivo_array_intersect_key($callfiltermember['secretary'],$secretary['list'],'typeval');

			if($secretary['slt'] !== false)
				$secretary['list'] = xivo_array_diff_key($secretary['list'],$secretary['slt']);
		}

		if(empty($result) === false)
		{
			if(xivo_issa('dialaction',$result) === false || empty($result['dialaction']) === true)
				$result['dialaction'] = null;

			if(xivo_issa('callerid',$result) === false || empty($result['callerid']) === true)
				$result['callerid'] = null;
		}

		$_HTML->set_var('info',$result);
		$_HTML->set_var('fm_save',$fm_save);
		$_HTML->set_var('dialaction',$result['dialaction']);
		$_HTML->set_var('element',$appcallfilter->get_elements());
		$_HTML->set_var('destination_list',$appcallfilter->get_dialaction_destination_list());
		$_HTML->set_var('context_list',$appcallfilter->get_context_list());
		$_HTML->set_var('bosslist',$appcallfilter->get_free_boss_users());
		$_HTML->set_var('secretary',$secretary);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callerid.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callfilter.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'edit':
		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'));

		if(isset($_QR['id']) === false || ($info = $appcallfilter->get($_QR['id'])) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);

		$result = $fm_save = $callfiltermember = null;
		$return = &$info;

		$secretary['slt'] = $secretary = array();
		$secretary['list'] = $appcallfilter->get_secretary_users(null,true);

		if(isset($_QR['fm_send']) === true && xivo_issa('callfilter',$_QR) === true)
		{
			$return = &$result;

			if($appcallfilter->set_edit($_QR) === false
			|| $appcallfilter->edit() === false)
			{
				$fm_save = false;
				$result = $appcallfilter->get_result();
				$result['dialaction'] = $appcallfilter->get_dialaction_result();
			}
			else
				$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		}

		if(xivo_issa('callfiltermember',$return) === true)
			$callfiltermember = &$return['callfiltermember'];

		if($secretary['list'] !== false && xivo_issa('secretary',$callfiltermember) === true)
		{
			xivo::load_class('xivo_sort');
			$secretarysort = new xivo_sort(array('key' => 'priority'));
			usort($callfiltermember['secretary'],array(&$secretarysort,'num_usort'));

			$secretary['slt'] = xivo_array_intersect_key($callfiltermember['secretary'],$secretary['list'],'typeval');

			if($secretary['slt'] !== false)
				$secretary['list'] = xivo_array_diff_key($secretary['list'],$secretary['slt']);
		}

		if(empty($return) === false)
		{
			if(xivo_issa('dialaction',$return) === false || empty($return['dialaction']) === true)
				$return['dialaction'] = null;

			if(xivo_issa('callerid',$return) === false || empty($return['callerid']) === true)
				$return['callerid'] = null;
		}

		$_HTML->set_var('id',$info['callfilter']['id']);
		$_HTML->set_var('info',$return);
		$_HTML->set_var('fm_save',$fm_save);
		$_HTML->set_var('dialaction',$return['dialaction']);
		$_HTML->set_var('element',$appcallfilter->get_elements());
		$_HTML->set_var('destination_list',$appcallfilter->get_dialaction_destination_list());
		$_HTML->set_var('context_list',$appcallfilter->get_context_list());
		$_HTML->set_var('bosslist',$appcallfilter->get_boss_users());
		$_HTML->set_var('secretary',$secretary);

		$dhtml = &$_HTML->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callerid.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/callfilter.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'));

		if(isset($_QR['id']) === false || $appcallfilter->get($_QR['id']) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);

		$appcallfilter->delete();

		$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = xivo_issa_val('callfilters',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);

		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'));

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appcallfilter->get($values[$i]) !== false)
				$appcallfilter->delete();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = xivo_issa_val('callfilters',$_QR)) === false)
			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);

		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'),false);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appcallfilter->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appcallfilter->disable();
			else
				$appcallfilter->enable();
		}

		$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appcallfilter = &$ipbx->get_application('callfilter',array('type' => 'bosssecretary'),false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appcallfilter->get_callfilters_list(null,$order,$limit);
		$total = $appcallfilter->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_HTML->url('service/ipbx/call_management/callfilter'),$param);
		}

		$_HTML->set_var('pager',xivo_calc_page($page,$nbbypage,$total));
		$_HTML->set_var('list',$list);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/callfilter');

$_HTML->set_var('act',$act);
$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/callfilter/'.$act);
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
