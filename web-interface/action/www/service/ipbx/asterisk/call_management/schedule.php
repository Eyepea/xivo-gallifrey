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

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appschedule = &$ipbx->get_application('schedule');

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true && dwho_issa('schedule',$_QR) === true)
		{
			if($appschedule->set_add($_QR) === false
			|| $appschedule->add() === false)
			{
				$fm_save = false;
				$result = $appschedule->get_result_for_display();
				$result['dialaction'] = $appschedule->get_dialaction_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/call_management/schedule'),$param);
		}

		if(empty($result) === false
		&& (dwho_issa('dialaction',$result) === false
		    || empty($result['dialaction']) === true) === true)
			$result['dialaction'] = null;

		$_TPL->set_var('info',$result);
		$_TPL->set_var('dialaction',$result['dialaction']);
		$_TPL->set_var('dialaction_from','schedule');
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appschedule->get_elements());
		$_TPL->set_var('context_list',$appschedule->get_context_list());
		$_TPL->set_var('destination_list',$appschedule->get_dialaction_destination_list());

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/schedule.js');
		break;
	case 'edit':
		$appschedule = &$ipbx->get_application('schedule');

		if(isset($_QR['id']) === false
		|| ($info = $appschedule->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/schedule'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true && dwho_issa('schedule',$_QR) === true)
		{
			$return = &$result;

			if($appschedule->set_edit($_QR) === false
			|| $appschedule->edit() === false)
			{
				$fm_save = false;
				$result = $appschedule->get_result_for_display();
				$result['dialaction'] = $appschedule->get_dialaction_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/call_management/schedule'),$param);
		}

		if(empty($return) === false
		&& (dwho_issa('dialaction',$return) === false
		    || empty($return['dialaction']) === true) === true)
			$return['dialaction'] = null;

		$_TPL->set_var('id',$info['schedule']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('dialaction',$return['dialaction']);
		$_TPL->set_var('dialaction_from','schedule');
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appschedule->get_elements());
		$_TPL->set_var('destination_list',$appschedule->get_dialaction_destination_list());
		$_TPL->set_var('context_list',$appschedule->get_context_list());

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/dialaction.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/schedule.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appschedule = &$ipbx->get_application('schedule');

		if(isset($_QR['id']) === false || $appschedule->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/schedule'),$param);

		$appschedule->delete();

		$_QRY->go($_TPL->url('service/ipbx/call_management/schedule'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('schedules',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/schedule'),$param);

		$appschedule = &$ipbx->get_application('schedule');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appschedule->get($values[$i]) !== false)
				$appschedule->delete();
		}

		$_QRY->go($_TPL->url('service/ipbx/call_management/schedule'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = dwho_issa_val('schedules',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/call_management/schedule'),$param);

		$appschedule = &$ipbx->get_application('schedule');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appschedule->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appschedule->disable();
			else
				$appschedule->enable();
		}

		$_QRY->go($_TPL->url('service/ipbx/call_management/schedule'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appschedule = &$ipbx->get_application('schedule');

		$order = array();
		$order['name'] = SORT_ASC;
		$order['context'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appschedule->get_schedules_list(null,$order,$limit);
		$total = $appschedule->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/call_management/schedule'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/schedule');

$_TPL->set_var('act',$act);
$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/schedule/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
