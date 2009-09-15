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

$appvoicemail = &$ipbx->get_apprealstatic('voicemail');
$appgeneralvoicemail = &$appvoicemail->get_module('general');
$appzonemessages = &$appvoicemail->get_module('zonemessages');

$fm_save = null;

$info = $error = array();

$info['voicemail'] = $appgeneralvoicemail->get_all_by_category();
$info['zonemessages'] = $appzonemessages->get_all_name();

if(isset($_QR['fm_send']) === true && xivo_issa('voicemail',$_QR) === true)
{
	$fm_save = false;

	if(($rs = $appgeneralvoicemail->set_save_all($_QR['voicemail'])) !== false)
	{
		$info['voicemail'] = $rs['result'];
		$error['voicemail'] = $rs['error'];

		$fm_save = isset($rs['error'][0]) === false;
	}

	if(xivo_issa('zonemessages',$_QR) === true
	&& ($zmsg = xivo_group_array('name',$_QR['zonemessages'])) !== false)
	{
		if($appzonemessages->set($zmsg) !== false)
			$appzonemessages->save();

		$info['zonemessages'] = $appzonemessages->get_result();
		$error['zonemessages'] = $appzonemessages->get_error();

		if($appzonemessages->get_errnb() > 0)
			$fm_save = false;
	}
}

$element = array();
$element['voicemail'] = $appgeneralvoicemail->get_elements();
$element['zonemessages'] = $appzonemessages->get_elements();

if(xivo_issa('format',$element['voicemail']) === true
&& xivo_issa('value',$element['voicemail']['format']) === true
&& isset($info['voicemail']['format']) === true
&& xivo_has_len($info['voicemail']['format'],'var_val') === true)
{
	$info['voicemail']['format']['var_val'] = explode('|',$info['voicemail']['format']['var_val']);
	$element['voicemail']['format']['value'] = array_diff($element['voicemail']['format']['value'],
							      $info['voicemail']['format']['var_val']);
}

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/general.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('element',$element);
$_TPL->set_var('error',$error);
$_TPL->set_var('voicemail',$info['voicemail']);
$_TPL->set_var('zonemessages',$info['zonemessages']);
$_TPL->set_var('timezone_list',$appzonemessages->get_timezones());
$_TPL->set_var('context_list',$appgeneralvoicemail->get_context_list());

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/voicemail');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
