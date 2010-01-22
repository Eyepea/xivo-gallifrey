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

$ctimain = &$ipbx->get_module('ctimain');
$ctipresences = &$ipbx->get_module('ctipresences');

$appxivoserver = $ipbx->get_application('serverfeatures',array('feature' => 'phonebook','type' => 'xivo'));

$info = array();
$load_inf = $ctimain->get_all();
$info['ctimain'] = $load_inf[0];
$info['xivoserver'] = array();
$info['xivoserver']['info'] = $appxivoserver->get();
$info['xivoserver']['slt'] = array();
$element = array();
$element['ctimain'] = $ctimain->get_element();

$error = array();
$error['ctimain'] = array();
$fm_save = null;
	
if(isset($_QR['fm_send']) === true)
{
	$parting = array();
	if(isset($_QR['cti']['parting_astid_context']))
		$parting[] = 'context';
	if(isset($_QR['cti']['parting_astid_ipbx']))
		$parting[] = 'astid';
	$parting_str = implode(',', $parting);
	$_QR['cti']['parting_astid_context'] = $parting_str;

	if(isset($_QR['xivoserver']))
		$_QR['cti']['asterisklist'] = implode(",", $_QR['xivoserver']);
	else
		$_QR['cti']['asterisklist'] = '';

	if(($rs = $ctimain->chk_values($_QR['cti'])) === false)
	{
	#	dwho_print_r($ctimain->get_filter_error());
		die('error');
	}

	if($ctimain->exists(null,null,1) === true)
		$ret = $ctimain->edit(1, $rs);
	else
		$ret = $ctimain->add($rs);

	$load_inf = $ctimain->get_all();
	$info['ctimain'] = $load_inf[0];
}

if(($info['xivoserver']['list'] = $appxivoserver->get_server_list()) !== false)
{
	if(dwho_has_len($info['xivoserver']['list']))
		uasort($info['xivoserver']['list'],array(&$serversort,'str_usort'));
	if(isset($info['ctimain']['asterisklist']) && dwho_has_len($info['ctimain']['asterisklist']))
	{
		$sel = explode(',', $info['ctimain']['asterisklist']);
		$selected = array();
		foreach($sel as $s => $k)
		{
			$selected[$k]['id'] = $k;
		}
		$info['xivoserver']['slt'] = 
			dwho_array_intersect_key(
				$selected,
				$info['xivoserver']['list'],
				'id');
		$info['xivoserver']['list'] =
			dwho_array_diff_key(
				$info['xivoserver']['list'],
				$info['xivoserver']['slt']);
	}
}
$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('error',$error);
$_TPL->set_var('element',$element);
$_TPL->set_var('info', $info);

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/submenu.js');

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/cti_settings/general');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
