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

$appmail 	= &$_XOBJ->get_application('mail');

$result 	= $fm_save = null;

$info 		= $appmail->get();
$return 	= &$info['mail'];
if(isset($_QR['fm_send']) === true)
{
	$fm_save 	= true;
	$return 	= &$result;

	if($appmail->set($_QR) === false
	|| $appmail->save() === false)
		$fm_save = false;
	
	$result = $appmail->get_result('mail');		
}

$_TPL->set_var('fm_save'	, $fm_save);
$_TPL->set_var('info'		, $return);
$_TPL->set_var('element'	, $appmail->get_elements());

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');

$_TPL->set_bloc('main','xivo/configuration/network/mail');
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>
