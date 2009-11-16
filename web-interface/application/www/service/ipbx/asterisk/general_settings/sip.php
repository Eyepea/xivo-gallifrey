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

$appsip = &$ipbx->get_apprealstatic('sip');
$appgeneralsip = &$appsip->get_module('general');

$fm_save = null;

$info = $appgeneralsip->get_all_val_by_category(false);

if(isset($_QR['fm_send']) === true)
{
	$fm_save = false;

	if(($rs = $appgeneralsip->set_save_all($_QR)) !== false)
	{
		$info = $rs['result'];
		$error = $rs['error'];

		$fm_save = isset($rs['error'][0]) === false;
	}
}

$element = $appgeneralsip->get_element();

if(dwho_issa('allow',$element) === true
&& dwho_issa('value',$element['allow']) === true
&& isset($info['allow']) === true
&& dwho_has_len($info['allow'],'var_val') === true)
{
	$info['allow']['var_val'] = explode(',',$info['allow']['var_val']);
	$element['allow']['value'] = array_diff($element['allow']['value'],$info['allow']['var_val']);
}

if(dwho_issa('localnet',$info) === true
&& array_key_exists('var_val',$info['localnet']) === true
&& dwho_has_len($info['localnet']['var_val']) === false)
	$info['localnet'] = null;

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/submenu.js');

$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('info',$info);
$_TPL->set_var('element',$element);
$_TPL->set_var('moh_list',$appgeneralsip->get_musiconhold());
$_TPL->set_var('context_list',$appgeneralsip->get_context_list());

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/sip');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
