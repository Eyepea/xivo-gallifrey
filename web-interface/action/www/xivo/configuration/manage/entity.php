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
	case 'add':
		$appentity = &$_XOBJ->get_application('entity',null,false);

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true)
		{
			if($appentity->set_add($_QR) === false
			|| $appentity->add() === false)
			{
				$fm_save = false;
				$result = $appentity->get_result('entity');
			}
			else
				$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
		}

		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appentity->get_elements());
		$_TPL->set_var('territory',dwho_i18n::get_territory_translated_list());
		break;
	case 'edit':
		$appentity = &$_XOBJ->get_application('entity');

		if(isset($_QR['id']) === false || ($info = $appentity->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);

		$result = $fm_save = null;
		$return = &$info['entity'];

		if(isset($_QR['fm_send']) === true)
		{
			$return = &$result;

			if($appentity->set_edit($_QR) === false
			|| $appentity->edit() === false)
			{
				$fm_save = false;
				$result = $appentity->get_result('entity');
			}
			else
				$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
		}

		$_TPL->set_var('id',$info['entity']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appentity->get_elements());
		$_TPL->set_var('territory',dwho_i18n::get_territory_translated_list());
		break;
	case 'delete':
		$param['page'] = $page;

		$appentity = &$_XOBJ->get_application('entity');

		if(isset($_QR['id']) === false || $appentity->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);

		$appentity->delete();

		$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('entity',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);

		$appentity = &$_XOBJ->get_application('entity');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appentity->get($values[$i]) !== false)
				$appentity->delete();
		}

		$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;

		if(($values = dwho_issa_val('entity',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);

		$appentity = &$_XOBJ->get_application('entity',null,false);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appentity->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appentity->disable();
			else
				$appentity->enable();
		}

		$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = 20;

		$appentity = &$_XOBJ->get_application('entity');

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appentity->get_entities_list(null,$order,$limit);
		$total = $appentity->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');
$menu->set_toolbar('toolbar/xivo/configuration/manage/entity');

$_TPL->set_bloc('main','xivo/configuration/manage/entity/'.$act);
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>
