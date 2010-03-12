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

$appha = &$_XOBJ->get_application('ha');

$fm_save = null;

$info = $appha->get();
#$return = &$info['resolvconf'];

if(isset($_QR['fm_send']) === true)
{
	$fm_save = true;
    var_dump($_QR);
}
#	if($appresolvconf->set($_QR) === false
#	|| $appresolvconf->save() === false)
#		$fm_save = false;

#	$result = $appresolvconf->get_result('resolvconf');
#}

#if(dwho_issa('search',$return) === true)
#	$searches = $return['search'];
#else if(dwho_has_len($return,'search') === true)
#	$searches = explode(' ',$return['search']);
#else 
#	$searches = null;

$_TPL->set_var('fm_save', $fm_save);
$_TPL->set_var('info'   , $info);
#$_TPL->set_var('element',$appresolvconf->get_elements());
#$_TPL->set_var('searches',$searches);

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/submenu.js');

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');

$_TPL->set_bloc('main','xivo/configuration/network/ha');
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>
