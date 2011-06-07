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

$camp = &$ipbx->get_module('campaign_general');

$fm_save = $error = null;

// saving
if(isset($_QR['fm_send']) === true)
{
	$fm_save = true;
	$info = array();

	foreach(array('svichoices','svientries','svivariables') as $key)
	{
		$items = array();
		for($i=0; $i<count($_QR[$key.'_name'])-1;$i++)
			$items[$_QR[$key.'_name'][$i]] = $_QR[$key.'_astvar'][$i];

		$info[$key] = $items;
	}

	foreach(array('records_path','records_announce','purge_syst_tagged_delay','purge_syst_tagged_at','purge_syst_untagged_delay','purge_syst_untagged_at','purge_punct_delay','purge_punct_at') as $key)
		$info[$key] = $_QR[$key];

	if(($rs = $camp->edit(1,$info)) === false)
	{
		$fm_save = false;
	}

} else {
	$info = $camp->get(1);
}

$element = $camp->get_element();

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/submenu.js');
$dhtml->set_js('js/jquery.js');
$dhtml->set_js('js/jquery.ui.core.js');
$dhtml->set_js('js/jquery.ui.timepicker.js');
$dhtml->set_css('css/jquery-ui-timepicker/jquery-ui-timepicker.css');
$dhtml->set_css('css/jquery-ui-timepicker/jquery.ui.theme.css');

$_TPL->set_var('fm_save', $fm_save);
$_TPL->set_var('info'   , $info);
//$_TPL->set_var('error',$error);
$_TPL->set_var('element', $element);

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/callcenter/menu');

$_TPL->set_bloc('main','callcenter/general');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
