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

xivo::load_class('xivo_entity',XIVO_PATH_OBJECT,null,false);
$_ETT = new xivo_entity();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$result = null;

		if(isset($_QR['fm_send']) === true)
		{
			if(($result = $_ETT->chk_values($_QR)) === false)
				$result = $_ETT->get_filter_result();
			else if($_ETT->add($result) !== false)
				$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
		}

		$_TPL->set_var('info',$result);
		$_TPL->set_var('element',$_ETT->get_element());
		$_TPL->set_var('territory',dwho_i18n::get_territory_translated_list());
		break;
	case 'edit':
		if(isset($_QR['id']) === false
		|| ($info = $_ETT->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);

		$return = &$info;

		do
		{
			if(isset($_QR['fm_send']) === false)
				break;

			$result = array();
			$return = &$result;

			$_QR['disable'] = $info['disable'];

			if(($result = $_ETT->chk_values($_QR)) === false)
			{
				$result = $_ETT->get_filter_result();
				break;
			}
			else if($_ETT->edit($info['id'],$result) === false)
				break;

			$ipbx = &$_SRE->get('ipbx');
			$context = &$ipbx->get_module('context');

			if($context->edit_where(array('entity' => $info['name']),
						array('entity' => $result['name'])) === false)
			{
				$_ETT->edit_origin();
				break;
			}

			$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
		}
		while(false);

		$_TPL->set_var('id',$info['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('element',$_ETT->get_element());
		$_TPL->set_var('territory',dwho_i18n::get_territory_translated_list());
		break;
	case 'delete':
		$param['page'] = $page;

		$ipbx = &$_SRE->get('ipbx');
		$context = &$ipbx->get_module('context');

		if(isset($_QR['id']) === true
		&& ($info = $_ETT->get($_QR['id'])) !== false
		&& $context->get_where(array('entity' => $info['name'])) === false)
			$_ETT->delete($info['id']);

		$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('entity',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);

		$ipbx = &$_SRE->get('ipbx');
		$context = &$ipbx->get_module('context');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if(($info = $_ETT->get($values[$i])) !== false
			&& $context->get_where(array('entity' => $info['name'])) === false)
				$_ETT->delete($info['id']);
		}

		$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
		break;
	case 'enables':
	case 'disables':
		$param['page'] = $page;
		$disable = $act === 'disables';

		if(($values = dwho_issa_val('entity',$_QR)) === false)
			$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
			$_ETT->disable(intval($values[$i]),$disable);

		$_QRY->go($_TPL->url('xivo/configuration/manage/entity'),$param);
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

		$list = $_ETT->get_entities_list(null,$order,$limit);
		$total = $_ETT->get_cnt();

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
