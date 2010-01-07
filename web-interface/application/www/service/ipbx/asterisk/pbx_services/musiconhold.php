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
$cat = isset($_QR['cat']) === true ? strval($_QR['cat']) : '';
$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;

$element = $info = $result = array();

$param = array();
$param['act'] = 'list';

$musiconhold = &$ipbx->get_module('musiconhold');

if(($list_cats = $musiconhold->get_all_by_category()) !== false)
{
	dwho::load_class('dwho_sort');
	$sort = new dwho_sort(array('key' => 'category'));
	usort($list_cats,array(&$sort,'strnat_usort'));
}

$_TPL->set_var('list_cats',$list_cats);

switch($act)
{
	case 'add':
	case 'edit':
		$dhtml = &$_TPL->get_module('dhtml');
		$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/musiconhold.js');
	case 'delete':
	case 'deletes':
	case 'list':
		$action = $act;
		break;
	case 'addfile':
		if($list_cats === false)
		{
			$action = $act = 'list';
			break;
		}
	case 'editfile':
	case 'listfile':
	case 'deletefile':
	case 'download':
		$action = $act;
		$param['act'] = 'listfile';
		$param['cat'] = $cat;
		break;
	case 'enables':
	case 'disables':
		$action = 'enables';
		break;
	default:
		$_QRY->go($_TPL->url('service/ipbx'));
}

include(dirname(__FILE__).'/musiconhold/'.$action.'.php');

$_TPL->set_var('act',$act);
$_TPL->set_var('cat',$cat);
$_TPL->set_var('element',$element);
$_TPL->set_var('info',$info);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/pbx_services/musiconhold');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_services/musiconhold/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
