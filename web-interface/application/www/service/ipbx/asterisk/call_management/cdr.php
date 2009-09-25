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

$page = isset($_QR['page']) === true ? dwho_uint($_QR['page'],1) : 1;
$act = isset($_QR['act']) === false || $_QR['act'] !== 'exportcsv' ? 'search' : 'exportcsv';

$cdr = &$ipbx->get_module('cdr');
$element = $cdr->get_element();

if($act !== 'exportcsv')
{
	$context = &$ipbx->get_module('context');

	$order = array();
	$order['displayname'] = SORT_ASC;
	$order['name'] = SORT_ASC;

	if(($context_list = $context->get_all(null,true,$order)) !== false)
		$context_list[] = array('name' => 'custom','identity' => 'custom');

	$_TPL->set_var('context_list',$context_list);
}

$total = 0;
$nbbypage = XIVO_SRE_IPBX_AST_CDR_NBBYPAGE;

$info = null;
$result = false;

if(isset($_QR['fm_send']) === true || isset($_QR['search']) === true)
{
	if(isset($_QR['dcontext'],$_QR['dcontext-custom']) === true && $_QR['dcontext'] === 'custom')
	{
		$_QR['dcontext'] = $_QR['dcontext-custom'];
		$_TPL->set_var('dcontext-custom',$_QR['dcontext-custom']);
	}

	unset($_QR['dcontext-custom']);

	if(($info = $cdr->chk_values($_QR,false)) === false)
		$info = $cdr->get_filter_result();
	else
	{
		if($act === 'exportcsv')
			$limit = null;
		else
		{
			$limit = array();
			$limit[0] = ($page - 1) * $nbbypage;
			$limit[1] = $nbbypage;
		}

		if(($result = $cdr->search($info,'calldate',$limit)) !== false && $result !== null)
			$total = $cdr->get_cnt();

		if($result === false)
			$info = null;

		$_TPL->set_var('pager',dwho_calc_page($page,$nbbypage,$total));
	}

	if($act === 'exportcsv' && $info !== null)
		$info['amaflagsmeta'] = $info['amaflags'] !== '' ? $cdr->amaflags_meta($info['amaflags']) : '';
}

$_TPL->set_var('total',$total);
$_TPL->set_var('element',$element);
$_TPL->set_var('info',$info);
$_TPL->set_var('result',$result);
$_TPL->set_var('act',$act);

if($act === 'exportcsv')
{
	if($result === false)
		$_QRY->go($_TPL->url('service/ipbx/call_management/cdr'));

	$_TPL->display('/bloc/service/ipbx/'.$ipbx->get_name().'/call_management/cdr/exportcsv');
	die();
}

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());
$menu->set_toolbar('toolbar/service/ipbx/'.$ipbx->get_name().'/call_management/cdr');

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/cdr.js');
$dhtml->set_js('js/xivo_calendar.js');
$dhtml->add_js('/struct/js/date.js.php');

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/call_management/cdr/'.$act);
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
