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
$idphonehints = isset($_QR['idphonehints']) === true ? dwho_uint($_QR['idphonehints'],1) : 1;
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$param = array();
$param['act'] = 'list';
$param['idphonehints'] = $idphonehints;

$info = $result = array();
$info['access_status'] = array();
$info['access_status']['info'] = array();
$info['access_status']['slt'] = array();

switch($act)
{
	case 'add':
		$app = &$ipbx->get_application('ctiphonehints');

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('phonehints',$_QR) === true)
		{
			if($app->set_add($_QR) === false
			|| $app->add() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('cti/phonehints'),$param);
		}

		dwho::load_class('dwho_sort');

		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/jscolor/jscolor.js');
		break;

	case 'edit':
		$app = &$ipbx->get_application('ctiphonehints');

		if(isset($_QR['idphonehints']) === false
		|| ($info = $app->get($_QR['idphonehints'])) === false)
			$_QRY->go($_TPL->url('cti/phonehints'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('phonehints',$_QR) === true)
		{
			$return = &$result;

			if($app->set_edit($_QR) === false
			|| $app->edit() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('cti/phonehints'),$param);
		}

		dwho::load_class('dwho_sort');

		$_TPL->set_var('idphonehints',$info['phonehints']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		$dhtml->set_js('js/jscolor/jscolor.js');
		break;

	case 'delete':
		$param['page'] = $page;

		$app = &$ipbx->get_application('ctiphonehints');

		if(isset($_QR['idphonehints']) === false
		|| ($info = $app->get($_QR['idphonehints'])) === false)
			$_QRY->go($_TPL->url('cti/phonehints'),$param);

		$app->delete();

		$_QRY->go($_TPL->url('cti/phonehints'),$param);
		break;

	case 'liststatus':
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$app = &$ipbx->get_application('ctistatus',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $app->get_status_list($idphonehints);
		$total = $app->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('cti/phonehints'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
		break;

	case 'editstatus':
		$param['act'] = 'editstatus';
		$param['idstatus'] = $_QR['idstatus'];
		$app = &$ipbx->get_application('ctistatus');
		if(isset($_QR['idstatus']) === false
		|| ($info = $app->getstatus($_QR['idstatus'])) === false)
			$_QRY->go($_TPL->url('cti/phonehints'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('status',$_QR) === true)
		{
			$return = &$result;

			if($app->set_edit($_QR) === false
			|| $app->edit() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('cti/phonehints'),$param);
		}

		$info['access_status']['slt'] = array();
		if(($info['access_status']['list'] = $app->get_status_list()) !== false)
		{
			if(dwho_has_len($info['access_status']['list']))
				uasort($info['access_status']['list'],array(&$serversort,'str_usort'));
			if(isset($info['ctistatus']['access_status']) && dwho_has_len($info['ctistatus']['access_status']))
			{
				$sel = explode(',', $info['ctistatus']['access_status']);
				$selected = array();
				foreach($sel as $s => $k)
				{
					$selected[$k]['id'] = $k;
				}
				$info['access_status']['slt'] = 
					dwho_array_intersect_key(
						$selected,
						$info['access_status']['list'],
						'id');
				$info['access_status']['list'] =
					dwho_array_diff_key(
						$info['access_status']['list'],
						$info['access_status']['slt']);
			}
		}

		dwho::load_class('dwho_sort');

		$_TPL->set_var('idstatus',$info['status']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$app = &$ipbx->get_application('ctiphonehints',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $app->get_phonehints_list();
		$total = $app->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('cti/phonehints'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);
#$_TPL->set_var('group',$group);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/cti/menu');

$menu->set_toolbar('toolbar/cti/phonehints');

$_TPL->set_bloc('main','/cti/phonehints/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
