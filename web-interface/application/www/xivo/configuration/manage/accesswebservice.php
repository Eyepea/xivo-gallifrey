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

xivo::load_class('xivo_accesswebservice',XIVO_PATH_OBJECT,null,false);
$_AWS = new xivo_accesswebservice();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$result = null;

		if(isset($_QR['fm_send']) === true)
		{
			$add = false;

			if(($result = $_AWS->chk_values($_QR)) === false)
				$result = $_AWS->get_filter_result();
			else if(dwho_has_len($result['login']) === true
			&& dwho_has_len($result['passwd']) === true)
				$add = true;
			else if(dwho_has_len($result['host']) === true)
				$add = true;

			if($add === true && $_AWS->add($result) !== false)
				$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);
		}

		$_TPL->set_var('info',$result);
		$_TPL->set_var('element',$_AWS->get_element());
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_AWS->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);

		$return = &$info;

		if(isset($_QR['fm_send']) === true)
		{
			$edit = false;

			$result = array();
			$return = &$result;

			$_QR['disable'] = $info['disable'];

			if(($result = $_AWS->chk_values($_QR)) === false)
				$result = $_AWS->get_filter_result();
			else if(dwho_has_len($result['login']) === true
			&& dwho_has_len($result['passwd']) === true)
				$edit = true;
			else if(dwho_has_len($result['host']) === true)
				$edit = true;

			if($edit === true && $_AWS->edit($info['id'],$result) !== false)
				$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);
		}

		$_TPL->set_var('id',$info['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('element',$_AWS->get_element());
		break;
	case 'acl':
		if(isset($_QR['id']) === false
		|| ($info = $_AWS->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);

		$webservice_acl = $_AWS->get_acl();

		if(isset($_QR['fm_send']) === true)
		{
			$webservice_acl->edit($_QR);
			$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);
		}
		else if(($tree = $webservice_acl->get_access_tree($info['id'])) !== false)
		{
			$_TPL->set_var('info',$info);
			$_TPL->set_var('tree',$tree);
		}
		else $_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === true
		&& ($id = intval($_QR['id'])) > 0)
			$_AWS->delete($id);

		$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('accesswebservice',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($id = intval($values[$i])) > 0)
				$_AWS->delete($id);
		}

		$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;
		$disable = $act === 'disables';

		if(($values = dwho_issa_val('accesswebservice',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$_AWS->disable(intval($values[$i]),$disable);

		$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = 20;

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $_AWS->get_all(null,true,$order,$limit);
		$total = $_AWS->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('xivo/configuration/manage/accesswebservice'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/manage/accesswebservice');

$_TPL->set_bloc('main','xivo/configuration/manage/accesswebservice/'.$act);
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>
