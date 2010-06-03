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
$iddisplays = isset($_QR['iddisplays']) === true ? dwho_uint($_QR['iddisplays'],1) : 1;
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;
$urilist = array();

$param = array();
$param['iddisplays'] = $iddisplays;
$info = $result = array();

switch($act)
{
	case 'add':
		$app = &$ipbx->get_application('ctidisplays');

		$result = $fm_save = null;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('displays',$_QR) === true)
		{
			$cpt = 10;
			$str = "{";
			foreach($_QR['dispcol1'] as $k => $v)
			{
				if(trim($v) != '')
				{
					$str .= '"'.$cpt.'": ';
					$str .= '[ "'.trim($v).'",';
					$str .= '"'.trim($_QR['dispcol2'][$k]).'",';
					$str .= '"'.trim($_QR['dispcol3'][$k]).'",';
					$str .= '"'.trim($_QR['dispcol4'][$k]).'" ]';
					$cpt += 10;
				}
				$str .= ',';
			}
			$str = trim($str, ',');
			$str .= '}';

			$_QR['displays']['data'] = $str;
			$_QR['displays']['deletable'] = 1;
			if($app->set_add($_QR) === false
			|| $app->add() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('cti/displays'),$param);
		}

		dwho::load_class('dwho_sort');

		$data = null;
		$_TPL->set_var('data',$data);
		$_TPL->set_var('info',$result);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	case 'edit':
		$app = &$ipbx->get_application('ctidisplays');

		if(isset($_QR['iddisplays']) === false
		|| ($info = $app->get($_QR['iddisplays'])) === false)
			$_QRY->go($_TPL->url('cti/displays'),$param);

		$result = $fm_save = null;
		$return = &$info;

		if(isset($_QR['fm_send']) === true
		&& dwho_issa('displays',$_QR) === true)
		{
			$return = &$result;
			$cpt = 10;
			$str = '{ ';
			foreach($_QR['dispcol1'] as $k => $v)
			{
				if(trim($v) != '')
				{
					$str .= '"'.$cpt.'": ';
					$str .= '[ "'.trim($v).'",';
					$str .= '"'.trim($_QR['dispcol2'][$k]).'",';
					$str .= '"'.trim($_QR['dispcol3'][$k]).'",';
					$str .= '"'.trim($_QR['dispcol4'][$k]).'" ]';
					$cpt += 10;
				}
				$str .= ',';
			}
			$str = trim($str, ',');
			$str .= ' }';

			$_QR['displays']['data'] = $str;
			$_QR['displays']['deletable'] = 1;

			if($app->set_edit($_QR) === false
			|| $app->edit() === false)
			{
				$fm_save = false;
				$result = $app->get_result();
			}
			else
				$_QRY->go($_TPL->url('cti/displays'),$param);
		}

		dwho::load_class('dwho_sort');
		dwho::load_class('dwho_json');

		$arr = array();
		$data = array();
		$arr = dwho_json::decode($return['displays']['data'], true);
		foreach($arr as $d)
		{
			$data[] = $d;
		}

		$_TPL->set_var('iddisplays',$info['displays']['id']);
		$_TPL->set_var('info',$return);
		$_TPL->set_var('data',$data);
		$_TPL->set_var('fm_save',$fm_save);

		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/dwho/submenu.js');
		break;

	case 'delete':
		$param['page'] = $page;

		$app = &$ipbx->get_application('ctidisplays');

		if(isset($_QR['iddisplays']) === false
		|| ($info = $app->get($_QR['iddisplays'])) === false)
			$_QRY->go($_TPL->url('cti/displays'),$param);

		$app->delete();

		$_QRY->go($_TPL->url('cti/displays'),$param);
		break;

	default:
		$act = 'list';
		$prevpage = $page - 1;
		$nbbypage = XIVO_SRE_IPBX_AST_NBBYPAGE;

		$app = &$ipbx->get_application('ctidisplays',null,false);

		$order = array();
		$order['name'] = SORT_ASC;

		$limit = array();
		$limit[0] = $prevpage * $nbbypage;
		$limit[1] = $nbbypage;

		$list = $app->get_displays_list();
		$total = $app->get_cnt();

		if($list === false && $total > 0 && $prevpage > 0)
		{
			$param['page'] = $prevpage;
			$_QRY->go($_TPL->url('cti/displays'),$param);
		}

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
		$_TPL->set_var('list',$list);
}

$_TPL->set_var('act',$act);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/cti/menu');

$menu->set_toolbar('toolbar/cti/displays');

$_TPL->set_bloc('main','/cti/displays/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
