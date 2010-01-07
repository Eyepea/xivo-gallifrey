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
$search = isset($_QR['search']) === true ? strval($_QR['search']) : '';
$context = isset($_QR['context']) === true ? strval($_QR['context']) : '';

$param = array();
$param['act'] = 'list';

if($search !== '')
	$param['search'] = $search;
else if($context !== '')
	$param['context'] = $context;

$contexts = false;

switch($act)
{
	case 'add':
	case 'edit':
		$appuser = &$ipbx->get_application('user');
		$contexts = $appuser->get_all_context();

		include(dirname(__FILE__).'/users/'.$act.'.php');
		break;
	case 'delete':
		$param['page'] = $page;

		$appuser = &$ipbx->get_application('user');

		if(isset($_QR['id']) === false || $appuser->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);

		$appuser->delete();

		$ipbx->discuss('xivo[userlist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('users',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);

		$appuser = &$ipbx->get_application('user');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appuser->get($values[$i]) !== false)
				$appuser->delete();
		}

		$ipbx->discuss('xivo[userlist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = dwho_issa_val('users',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);

		$appuser = &$ipbx->get_application('user',null,false);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appuser->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appuser->disable();
			else
				$appuser->enable();
		}

		$ipbx->discuss('xivo[userlist,update]');

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);
		break;
	case 'import':
		$appuser = &$ipbx->get_application('user');
		$contexts = $appuser->get_all_context();

		if(isset($_QR['fm_send']) === true)
		{
			$appuser->import_csv();
			$ipbx->discuss('xivo[userlist,update]');
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);
		}

		$_TPL->set_var('import_file',$appuser->get_config_import_file());
		break;
	case 'list':
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appuser = &$ipbx->get_application('user');
		$contexts = $appuser->get_all_context();

		$order = array();
		$order['firstname'] = SORT_ASC;
		$order['lastname'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		if($search !== '')
			$list = $appuser->get_users_search($search,$context,null,null,$order,$limit);
		else if($context !== '')
			$list = $appuser->get_users_context($context,null,null,$order,$limit);
		else
			$list = $appuser->get_users_list(null,null,$order,$limit);

		$total = $appuser->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/users'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
		$_TPL->set_var('search',$search);
		$_TPL->set_var('context',$context);
}

$_TPL->set_var('act',$act);
$_TPL->set_var('contexts',$contexts);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/users');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/users/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
