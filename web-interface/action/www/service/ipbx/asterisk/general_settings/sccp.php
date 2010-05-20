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

$appsccp = &$ipbx->get_apprealstatic('sccp');
$appgeneralsccp = &$appsccp->get_module('general');

$fm_save = $error = $info = null;

$info = $appgeneralsccp->get_all(false);

if(isset($_QR['fm_send']) === true)
{
	$fm_save = false;

	if(($rs = $appgeneralsccp->set_save_all($_QR)) !== false)
	{
		$info = $rs['result'];
		$error = $rs['error'];
		$fm_save = empty($error);
	}
}

$element = $appgeneralsccp->get_element();

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/submenu.js');

$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('info',$info);
$_TPL->set_var('error',$error);
$_TPL->set_var('element',$element);
$_TPL->set_var('moh_list',$appgeneralsccp->get_musiconhold());
$_TPL->set_var('context_list',$appgeneralsccp->get_context_list());

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/sccp');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
