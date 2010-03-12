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
$appmonitor 	= &$_XOBJ->get_application('monitoring');

$fm_save        = null;
$info 		    = $appmonitor->get_xivo_maintenance();

if(isset($_QR['fm_send']) === true)
{
	$fm_save 	= true;

    if(!array_key_exists('maintenance', $_QR))
        $_QR['maintenance'] = 0;
        
	if($appmonitor->set_xivo_maintenance($_QR['maintenance']) === false)
		$fm_save = false;

    $info['xivo.maintenance'] = $_QR['maintenance'];
}

$_TPL->set_var('fm_save'	, $fm_save);
$_TPL->set_var('element'	, $info);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/xivo/configuration');

$_TPL->set_bloc('main','xivo/configuration/support/xivo');
$_TPL->set_struct('xivo/configuration');
$_TPL->display('index');

?>
