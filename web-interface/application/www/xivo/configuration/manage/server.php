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

$act = isset($_QR['act']) === true ? $_QR['act']  : '';
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

xivo::load_class('xivo_server',XIVO_PATH_OBJECT,null,false);
$_SVR = new xivo_server();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$result = null;

		if(isset($_QR['fm_send']) === true)
		{
			if(($result = $_SVR->chk_values($_QR)) === false)
				$result = $_SVR->get_filter_result();
			else if($_SVR->add($result) !== false)
				$_QRY->go($_TPL->url('xivo/configuration/manage/server'),$param);
		}

		$_TPL->set_var('info',$result);
		$_TPL->set_var('element',$_SVR->get_element());
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_SVR->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/server'),$param);

		$return = &$info;

		if(isset($_QR['fm_send']) === true)
		{
			$result = array();
			$return = &$result;

			$_QR['disable'] = $info['disable'];

			if(($result = $_SVR->chk_values($_QR)) === false)
				$result = $_SVR->get_filter_result();
			else if($_SVR->edit($info['id'],$result) !== false)
				$_QRY->go($_TPL->url('xivo/configuration/manage/server'),$param);
		}

		$_TPL->set_var('id',$info['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('element',$_SVR->get_element());
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === true
		&& ($id = intval($_QR['id'])) > 0
		&& $_SVR->delete($id) !== false)
		{
			$serverfeatures = &$_SRE->get_module('ipbx','serverfeatures');

			$where = array();
			$where['serverid'] = $id;
			$serverfeatures->delete_where($where);
		}

		$_QRY->go($_TPL->url('xivo/configuration/manage/server'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('server',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/server'),$param);

		$serverfeatures = &$_SRE->get_module('ipbx','serverfeatures');

		$where = array();

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($id = intval($values[$i])) > 0
			&& $_SVR->delete($id) !== false)
			{
				$where['serverid'] = $id;
				$serverfeatures->delete_where($where);
			}
		}

		$_QRY->go($_TPL->url('xivo/configuration/manage/server'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;
		$disable = $act === 'disables';

		if(($values = dwho_issa_val('server',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/server'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$_SVR->disable(intval($values[$i]),$disable);

		$_QRY->go($_TPL->url('xivo/configuration/manage/server'),$param);
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

		$list = $_SVR->get_all(null,true,$order,$limit);
		$total = $_SVR->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('xivo/configuration/manage/server'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/manage/server');

$_TPL->set_bloc('main','xivo/configuration/manage/server/'.$act);
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>
