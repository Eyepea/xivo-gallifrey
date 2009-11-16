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

$act = isset($_QR['act']) === true ? $_QR['act'] : '';
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$info = array();

$param = array();
$param['act'] = 'list';

switch($act)
{
	case 'add':
		$appmeetme = &$ipbx->get_application('meetme');

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('meetmeroom',$_QR) === true
		&& dwho_issa('meetmefeatures',$_QR) === true)
		{
			if($appmeetme->set_add($_QR) === false
			|| $appmeetme->add() === false)
			{
				$fm_save = false;
				$result = $appmeetme->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/pbx_settings/meetme'),$param);
		}

		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appmeetme->get_elements());
		$_TPL->set_var('moh_list',$appmeetme->get_musiconhold());
		$_TPL->set_var('context_list',$appmeetme->get_context_list());

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;
	case 'edit':
		$appmeetme = &$ipbx->get_application('meetme');

		if(isset($_QR['id']) === false || ($info = $appmeetme->get($_QR['id'])) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/meetme'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('meetmeroom',$_QR) === true
		&& dwho_issa('meetmefeatures',$_QR) === true)
		{
			$return = &$result;

			if($appmeetme->set_edit($_QR) === false
			|| $appmeetme->edit() === false)
			{
				$fm_save = false;
				$result = $appmeetme->get_result();
			}
			else
				$_QRY->go($_TPL->url('service/ipbx/pbx_settings/meetme'),$param);
		}

		$_TPL->set_var('id',$info['meetmeroom']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);
		$_TPL->set_var('element',$appmeetme->get_elements());
		$_TPL->set_var('moh_list',$appmeetme->get_musiconhold());
		$_TPL->set_var('context_list',$appmeetme->get_context_list());

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;
	case 'delete':
		$param['page'] = $page;

		$appmeetme = &$ipbx->get_application('meetme');

		if(isset($_QR['id']) === false || $appmeetme->get($_QR['id']) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/meetme'),$param);

		$appmeetme->delete();

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	case 'deletes':
		$param['page'] = $page;

		if(($values = dwho_issa_val('meetme',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/meetme'),$param);

		$appmeetme = &$ipbx->get_application('meetme');

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appmeetme->get($values[$i]) !== false)
				$appmeetme->delete();
		}

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	case 'disables':
	case 'enables':
		$param['page'] = $page;

		if(($values = dwho_issa_val('meetme',$_QR)) === false)
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/meetme'),$param);

		$appmeetme = &$ipbx->get_application('meetme',null,false);

		$nb = count($values);

		for($i = 0;$i < $nb;$i++)
		{
			if($appmeetme->get($values[$i]) === false)
				continue;
			else if($act === 'disables')
				$appmeetme->disable();
			else
				$appmeetme->enable();
		}

		$_QRY->go($_TPL->url('service/ipbx/pbx_settings/meetme'),$param);
		break;
	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$appmeetme = &$ipbx->get_application('meetme',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $appmeetme->get_meetme_list(null,$order,$limit);
		$total = $appmeetme->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('service/ipbx/pbx_settings/meetme'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_settings/meetme');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_settings/meetme/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
