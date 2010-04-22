<?php

#
# XiVO Web-Interface
# Copyright (C) 2010  Proformatique <technique@proformatique.com>
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

dwho::load_class('dwho_json');

$sheetevents = &$ipbx->get_module('ctisheetevents');
$app = $ipbx->get_application('ctisheetactions');
$actionslist = $app->get_sheetactions_list();

$info = array();
$load_inf = $sheetevents->get_all();
$info['ctisheetevents'] = $load_inf[0];
$element = array();
$element['ctisheetevents'] = $sheetevents->get_element();

$customs = null;
$error = array();
$error['sheetevents'] = array();
$fm_save = null;

$sheetactionslist = array();
$sheetactionslist[] = "";
foreach($actionslist as $v)
{
	$sheetactionslist[] = $v['ctisheetactions']['name'];
}
	
if(isset($_QR['fm_send']) === true)
{
	$fm_save = false;
	$parting = array();

	$str = "{";
	foreach($_QR['customcol1'] as $k=>$v)
	{
		$tv = trim($v);
		if($tv != '')
		{
			$str .= '"'.$tv.'": "'.trim($_QR['customcol2'][$k]).'",';
		}
	}
	$str = trim($str, ',').'}';
	$_QR['ctisheetevents']['custom'] = $str;
	if(($rs = $sheetevents->chk_values($_QR['ctisheetevents'])) === false)
	{
		$ret = 0;
	} else {
		if($sheetevents->exists(null,null,1) === true)
			$ret = $sheetevents->edit(1, $rs);
		else
			$ret = $sheetevents->add($rs);
	}
	
	if($ret == 1)
		$fm_save = true;
}
$load_inf = $sheetevents->get_all();

$info['ctisheetevents'] = $load_inf[0];
$arr = array();
$arr = dwho_json::decode($load_inf[0]['custom'], true);
if($arr !== false)
{
	foreach($arr as $k => $v)
	{
		$customs[$k] = $v;
	}
}

$_TPL->set_var('sheetactionslist',$sheetactionslist);
$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('customs',$customs);
$_TPL->set_var('error',$error);
$_TPL->set_var('element',$element);
$_TPL->set_var('info', $info);

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/submenu.js');

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/cti_settings/sheetevents');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
