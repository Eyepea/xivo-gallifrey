<?php

#
# XiVO Web-Interface
# Copyright (C) 2009  Proformatique <technique@proformatique.com>
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

$appnetiface = &$_XOBJ->get_application('netiface');

switch($act)
{
	case 'add':
		$result = $fm_save = null;

		if(isset($_QR['id']) === true)
			$name = $_QR['id'];
		else
			$name = null;

		if(isset($_QR['fm_send']) === true)
		{
			if($appnetiface->set_add($_QR) === false
			|| $appnetiface->add() === false)
			{
				$fm_save = false;
				$result = $appnetiface->get_result('netiface');
			}
			else
				$_QRY->go($_TPL->url('xivo/configuration/network/interface'),$param);
		}

		if(($interfaces = $appnetiface->get_physical_interfaces_for_vlan(null,$name)) !== false)
		{
			dwho::load_class('dwho_sort');
			$ifacesort = new dwho_sort();
			uksort($interfaces,array(&$ifacesort,'str_usort'));
		}

		$_TPL->set_var('name',$name);
		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('interfaces',$interfaces);
		$_TPL->set_var('element',$appnetiface->get_elements());

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/xivo/configuration/network/interface.js');
		break;
	case 'edit':
		if(isset($_QR['id']) === false || ($info = $appnetiface->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('xivo/configuration/network/interface'),$param);

		$result = $fm_save = null;
		$return = &$info['netiface'];

		if(isset($_QR['fm_send']) === true)
		{
			$return = &$result;

			if($appnetiface->set_edit($_QR) === false
			|| $appnetiface->edit() === false)
			{
				$fm_save = false;
				$result = $appnetiface->get_result('netiface');
			}
			else
				$_QRY->go($_TPL->url('xivo/configuration/network/interface'),$param);
		}

		if(($interfaces = $appnetiface->get_physical_interfaces_for_vlan(null,$info['netiface']['name'])) !== false)
		{
			dwho::load_class('dwho_sort');
			$ifacesort = new dwho_sort();
			uksort($interfaces,array(&$ifacesort,'str_usort'));
		}

		$_TPL->set_var('id',$info['netiface']['name']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appnetiface->get_elements());
		$_TPL->set_var('interfaces',$interfaces);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/xivo/configuration/network/interface.js');
		break;
	case 'delete':
		$param['page'] = $page;

		if(isset($_QR['id']) === false || $appnetiface->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('xivo/configuration/network/interface'),$param);

		$appnetiface->delete();

		$_QRY->go($_TPL->url('xivo/configuration/network/interface'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('netiface',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/network/interface'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appnetiface->get($values[$i]) !== false)
				$appnetiface->delete();
		}

		$_QRY->go($_TPL->url('xivo/configuration/network/interface'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = dwho_issa_val('netiface',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/network/interface'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appnetiface->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appnetiface->disable();
			else
				$appnetiface->enable();
		}

		$_QRY->go($_TPL->url('xivo/configuration/network/interface'),$param);
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

		$list = $appnetiface->get_netifaces_list(null,$order,$limit);
		$total = $appnetiface->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('xivo/configuration/network/interface'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/network/interface');

$_TPL->set_bloc('main','xivo/configuration/network/interface/'.$act);
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>
