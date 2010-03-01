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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the7
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
		$appcontext = &$ipbx->get_application('context');

		$result = $fm_save = $error = null;

		$contextinc = array();
		$contextinc['slt'] = array();

		$contextincorder = array();
		$contextincorder['displayname'] = SORT_ASC;
		$contextincorder['name'] = SORT_ASC;

		$contextinc['list'] = $appcontext->get_contexts_except(null,$contextincorder,null,true);

		if(isset($_QR['fm_send']) === true && dwho_issa('context',$_QR) === true)
		{
			if(dwho_issa('contextnumbers',$_QR) === true
			&& isset($_QR['context']['entity']) === true
			&& dwho_has_len($_QR['context']['entity']) === true)
			{
				if(dwho_issa('user',$_QR['contextnumbers']) === true
				&& ($_QR['contextnumbers']['user'] = dwho_group_array('numberbeg',
										      $_QR['contextnumbers']['user'])) === false)
					unset($_QR['contextnumbers']['user']);

				if(dwho_issa('group',$_QR['contextnumbers']) === true
				&& ($_QR['contextnumbers']['group'] = dwho_group_array('numberbeg',
										       $_QR['contextnumbers']['group'])) === false)
					unset($_QR['contextnumbers']['group']);

				if(dwho_issa('queue',$_QR['contextnumbers']) === true
				&& ($_QR['contextnumbers']['queue'] = dwho_group_array('numberbeg',
										       $_QR['contextnumbers']['queue'])) === false)
					unset($_QR['contextnumbers']['queue']);

				if(dwho_issa('meetme',$_QR['contextnumbers']) === true
				&& ($_QR['contextnumbers']['meetme'] = dwho_group_array('numberbeg',
											$_QR['contextnumbers']['meetme'])) === false)
					unset($_QR['contextnumbers']['meetme']);

				if(dwho_issa('incall',$_QR['contextnumbers']) === true
				&& ($_QR['contextnumbers']['incall'] = dwho_group_array('numberbeg',
											$_QR['contextnumbers']['incall'])) === false)
					unset($_QR['contextnumbers']['incall']);
			}
			else
				unset($_QR['contextnumbers']);

			if($appcontext->set_add($_QR) === false
			|| $appcontext->add() === false)
			{
				$fm_save = false;
				$result = $appcontext->get_result();
				$error = $appcontext->get_error();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/system_management/context'),$param);
		}

		if($contextinc['list'] !== false && dwho_issa('contextinclude',$result) === true)
		{
			dwho::load_class('dwho_sort');
			$contextincsort = new dwho_sort(array('key' => 'priority'));
			usort($result['contextinclude'],array(&$contextincsort,'num_usort'));

			$contextinc['slt'] = dwho_array_intersect_key($result['contextinclude'],
								      $contextinc['list'],
								      'include');

			if($contextinc['slt'] !== false)
				$contextinc['list'] = dwho_array_diff_key($contextinc['list'],$contextinc['slt']);
		}

		$_TPL->set_var('info',$result);
		$_TPL->set_var('error',$error);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appcontext->get_elements());
		$_TPL->set_var('contextinc',$contextinc);
		$_TPL->set_var('entities',$appcontext->get_entities_list(null,array('displayname' => SORT_ASC)));

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/context.js');
		break;
	case 'edit':
		$appcontext = &$ipbx->get_application('context');

		if(isset($_QR['id']) === false || ($info = $appcontext->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/system_management/context'),$param);

		$result = $fm_save = $error = null;
		$return = &$info;

		$contextinc = array();
		$contextinc['slt'] = array();

		$contextincorder = array();
		$contextincorder['displayname'] = SORT_ASC;
		$contextincorder['name'] = SORT_ASC;

		$contextinc['list'] = $appcontext->get_contexts_except($info['context']['name'],
								       $contextincorder,
								       null,
								       true);

		if(isset($_QR['fm_send']) === true && dwho_issa('context',$_QR) === true)
		{
			$return = &$result;

			if(dwho_issa('contextnumbers',$_QR) === true
			&& isset($_QR['context']['entity']) === true
			&& dwho_has_len($_QR['context']['entity']) === true)
			{
				if(dwho_issa('user',$_QR['contextnumbers']) === true
				&& ($_QR['contextnumbers']['user'] = dwho_group_array('numberbeg',
										      $_QR['contextnumbers']['user'])) === false)
					unset($_QR['contextnumbers']['user']);

				if(dwho_issa('group',$_QR['contextnumbers']) === true
				&& ($_QR['contextnumbers']['group'] = dwho_group_array('numberbeg',
										       $_QR['contextnumbers']['group'])) === false)
					unset($_QR['contextnumbers']['group']);

				if(dwho_issa('queue',$_QR['contextnumbers']) === true
				&& ($_QR['contextnumbers']['queue'] = dwho_group_array('numberbeg',
										       $_QR['contextnumbers']['queue'])) === false)
					unset($_QR['contextnumbers']['queue']);

				if(dwho_issa('meetme',$_QR['contextnumbers']) === true
				&& ($_QR['contextnumbers']['meetme'] = dwho_group_array('numberbeg',
											$_QR['contextnumbers']['meetme'])) === false)
					unset($_QR['contextnumbers']['meetme']);

				if(dwho_issa('incall',$_QR['contextnumbers']) === true
				&& ($_QR['contextnumbers']['incall'] = dwho_group_array('numberbeg',
											$_QR['contextnumbers']['incall'])) === false)
					unset($_QR['contextnumbers']['incall']);
			}
			else
				unset($_QR['contextnumbers']);

			if($appcontext->set_edit($_QR) === false
			|| $appcontext->edit() === false)
			{
				$fm_save = false;
				$result = $appcontext->get_result();
				$error = $appcontext->get_error();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/system_management/context'),$param);
		}

		if($contextinc['list'] !== false && dwho_issa('contextinclude',$return) === true)
		{
			dwho::load_class('dwho_sort');
			$contextincsort = new dwho_sort(array('key' => 'priority'));
			usort($return['contextinclude'],array(&$contextincsort,'num_usort'));

			$contextinc['slt'] = dwho_array_intersect_key($return['contextinclude'],
								      $contextinc['list'],
								      'include');

			if($contextinc['slt'] !== false)
				$contextinc['list'] = dwho_array_diff_key($contextinc['list'],$contextinc['slt']);
		}

		$_TPL->set_var('id',$info['context']['name']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('error',$error);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appcontext->get_elements());
		$_TPL->set_var('contextinc',$contextinc);
		$_TPL->set_var('entities',$appcontext->get_entities_list(null,array('displayname' => SORT_ASC)));

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/context.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appcontext = &$ipbx->get_application('context');

		if(isset($_QR['id']) === false || $appcontext->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('service/ipbx/system_management/context'),$param);

		$appcontext->delete();

		$_QRY->go($_TPL->url('service/ipbx/system_management/context'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('contexts',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/system_management/context'),$param);

		$appcontext = &$ipbx->get_application('context');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appcontext->get($values[$i]) !== false)
				$appcontext->delete();
		}

		$_QRY->go($_TPL->url('service/ipbx/system_management/context'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;

		if(($values = dwho_issa_val('contexts',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/system_management/context'),$param);

		$appcontext = &$ipbx->get_application('context');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appcontext->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appcontext->disable();
			else
				$appcontext->enable();
		}

		$_QRY->go($_TPL->url('service/ipbx/system_management/context'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appcontext = &$ipbx->get_application('context');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appcontext->get_contexts_list(null,$order,$limit);
		$total = $appcontext->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/system_management/context'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/system_management/context');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/system_management/context/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
