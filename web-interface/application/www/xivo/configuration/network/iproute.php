<?php

#
# XiVO Web-Interface
# Copyright (C) 2009-2010  Proformatique <technique@proformatique.com>
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

$appiproute = &$_XOBJ->get_application('iproute');

switch($act)
{
	case 'add':
		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true)
		{
			if($appiproute->set_add($_QR) === false
			|| $appiproute->add() === false)
			{
				$fm_save = false;
				$result = $appiproute->get_result('iproute');
			}
			else
				$_QRY->go($_TPL->url('xivo/configuration/network/iproute'),$param);
		}

		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appiproute->get_elements());
		$_TPL->set_var('interfaces',$appiproute->get_interfaces_list(
									null,
									array('name' => SORT_ASC)));

		break;
	case 'edit':
		if(isset($_QR['id']) === false || ($info = $appiproute->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('xivo/configuration/network/iproute'),$param);

		$result = $fm_save = null;
		$return = &$info['iproute'];

		if(isset($_QR['fm_send']) === true)
		{
			$return = &$result;

			if($appiproute->set_edit($_QR) === false
			|| $appiproute->edit() === false)
			{
				$fm_save = false;
				$result = $appiproute->get_result('iproute');
			}
			else
				$_QRY->go($_TPL->url('xivo/configuration/network/iproute'),$param);
		}

		$_TPL->set_var('id',$info['iproute']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appiproute->get_elements());
		$_TPL->set_var('interfaces',$appiproute->get_interfaces_list(
									null,
									array('name' => SORT_ASC)));
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === false || $appiproute->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('xivo/configuration/network/iproute'),$param);

		$appiproute->delete();

		$_QRY->go($_TPL->url('xivo/configuration/network/iproute'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('iproute',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/network/iproute'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appiproute->get($values[$i]) !== false)
				$appiproute->delete();
		}

		$_QRY->go($_TPL->url('xivo/configuration/network/iproute'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = dwho_issa_val('iproute',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/network/iproute'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appiproute->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appiproute->disable();
			else
				$appiproute->enable();
		}

		$_QRY->go($_TPL->url('xivo/configuration/network/iproute'),$param);
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

		$list = $appiproute->get_iproutes_list(null,$order,$limit);
		$total = $appiproute->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('xivo/configuration/network/iproute'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/network/iproute');

$_TPL->set_bloc('main','xivo/configuration/network/iproute/'.$act);
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>
