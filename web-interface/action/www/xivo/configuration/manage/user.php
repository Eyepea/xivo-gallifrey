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

$act = isset($_QR['act']) === true ? $_QR['act']  : '';
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';

switch($act)
{
	// Adding a new user
	case 'add':

		// after form validation, fm_send == 1
		if(isset($_QR['fm_send']) === true)
		{			
			$_QR['user']['meta'] 	= 'admin';
			$_QR['user']['valid']   = 1;
			$_QR['user']['obj']  	= '';

			if($_USR->add($_QR['user']))
			{ $_QRY->go($_TPL->url('xivo/configuration/manage/user'), $param); }
		} else {
			$_QR['user'] = array('login' => null, 'passwd' => null);
		}

		$_TPL->set_var('error'	, $_USR->get_error());
		$_TPL->set_var('info'	, $_QR['user']);
		break;


	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_USR->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/user'),$param);

		if(isset($_QR['fm_send']) === true)
		{
			if($_USR->edit($info['meta'],$_QR) !== false)
			{
				if(dwho_ulongint($_USR->get_info('id')) === dwho_ulongint($info['id']))
					$_USR->load_by_id($info['id']);

				$_QRY->go($_TPL->url('xivo/configuration/manage/user'),$param);
			} else {
				// error
				$info['passwd'] = $_QR['passwd'];
				$info['valid']	= $_QR['valid'];
			}
		}

		$_TPL->set_var('error'	, $_USR->get_error());
		$_TPL->set_var('info'	, $info);
		break;

	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === true
		&& ($id = intval($_QR['id'])) > 0)
		{
			$info = $_USR->get($_QR['id']);
			// we forbid root accounts deletion
			if($info !== false && strncmp($info['meta'], 'root', 4) != 0)
			{ $_USR->delete($id); }
		}

		$_QRY->go($_TPL->url('xivo/configuration/manage/user'),$param);
		break;

	case 'deletes':
		$param['page'] = $page;

		if(dwho_issa('userselection',$_QR))
		{

			// root users can't be disabled
			$roots  = $_USR->get_list_where(array('meta' => 'root'));
			$values = array_diff($_QR['userselection'], $roots);

			for($i = count($values) - 1; $i >= 0; $i--)
			{
				if(($id = intval($values[$i])) > 0)
				{ $_USR->delete($id); }
			}
		}

		$_QRY->go($_TPL->url('xivo/configuration/manage/user'),$param);
		break;

	case 'acl':
		if(isset($_QR['id']) === false
		|| ($info = $_USR->get($_QR['id'])) === false
		|| xivo_user::chk_authorize('admin',$info['meta']) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/user'),$param);

		$user_acl = $_USR->get_acl();

		if(isset($_QR['fm_send']) === true)
		{
			$user_acl->edit($_QR);
			$_QRY->go($_TPL->url('xivo/configuration/manage/user'),$param);
		}
		else if(($tree = $user_acl->get_access_tree($info['id'])) !== false)
		{
			$_TPL->set_var('info',$info);
			$_TPL->set_var('tree',$tree);
		}
		else $_QRY->go($_TPL->url('xivo/configuration/manage/user'),$param);
		break;

	case 'enables':
	case 'disables':
		$param['page'] = $page;
		$disable = $act === 'disables';

		if(($values = dwho_issa_val('userselection',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/user'),$param);

		// root users can't be disabled
		$roots  = $_USR->get_list_where(array('meta' => 'root'));
		$values = array_diff($values, $roots);

		// 'valid' field works in the reverse logic compare to 'disable' field
		$disable ^= 1;
		for($i = count($values) - 1; $i >= 0; $i--)
		{
			$_USR->disable(intval($values[$i]), $disable);
		}

		$_QRY->go($_TPL->url('xivo/configuration/manage/user'),$param);
		break;

	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = 20;

		$order = array();
		$order['login'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $_USR->get_all(null,true,$order,$limit);
		$total = $_USR->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('xivo/configuration/manage/user'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
		break;
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/manage/user');

$_TPL->set_bloc('main','xivo/configuration/manage/user/'.$act);
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>

