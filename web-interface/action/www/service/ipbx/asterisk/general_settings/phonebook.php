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

$appaccessfeatures = &$ipbx->get_application('accessfeatures',array('feature' => 'phonebook'));

$info = array();
$info['xivoserver'] = array();
$info['ldapfilter'] = array();
$info['xivoserver']['slt'] = $info['ldapfilter']['slt'] = false;

dwho::load_class('dwho_sort');

$accessfeaturessort = new dwho_sort(array('key' => 'host'));

if(($info['accessfeatures'] = $appaccessfeatures->get()) !== false)
	uasort($info['accessfeatures'],array(&$accessfeaturessort,'str_usort'));

$serversort = new dwho_sort(array('key' => 'identity'));

$appxivoserver = $ipbx->get_application('serverfeatures',array('feature' => 'phonebook','type' => 'xivo'));
$info['xivoserver']['info'] = $appxivoserver->get();
if(($info['xivoserver']['list'] = $appxivoserver->get_server_list()) !== false)
	uasort($info['xivoserver']['list'],array(&$serversort,'str_usort'));

$appldapfilter = $ipbx->get_application('serverfeatures',array('feature' => 'phonebook','type' => 'ldap'));
$info['ldapfilter']['info'] = $appldapfilter->get();
if(($info['ldapfilter']['list'] = $appldapfilter->get_server_list()) !== false)
	uasort($info['ldapfilter']['list'],array(&$serversort,'str_usort'));

$error = array();
$error['accessfeatures'] = array();
$error['xivoserver']     = array();
$error['ldapfilter']     = array();

$fm_save = null;

if(isset($_QR['fm_send']) === true)
{
	if(isset($_QR['accessfeatures']) === false)
		$_QR['accessfeatures'] = array();

	if($appaccessfeatures->set($_QR['accessfeatures']) !== false)
		$appaccessfeatures->save();

	$info['accessfeatures'] = $appaccessfeatures->get_result();

	if(is_array($info['accessfeatures']) === true)
		uasort($info['accessfeatures'],array(&$accessfeaturessort,'str_usort'));

	$error['accessfeatures'] = $appaccessfeatures->get_error();

	if(isset($error['accessfeatures'][0]) === true)
		$fm_save = false;
	else if($fm_save !== false)
		$fm_save = true;

	if(isset($_QR['xivoserver']) === false)
		$_QR['xivoserver'] = array();

	$appxivoserver = $ipbx->get_application('serverfeatures',array('feature' => 'phonebook','type' => 'xivo'));
	if($appxivoserver->set($_QR['xivoserver']) !== false)
	{
		$appxivoserver->delete_all();
		$appxivoserver->save();
	}

	$info['xivoserver']['info'] = $appxivoserver->get_result();
	$error['xivoserver']        = $appxivoserver->get_error();

	if(isset($error['xivoserver'][0]) === true)
		$fm_save = false;
	else if($fm_save !== false)
		$fm_save = true;

	if(isset($_QR['ldapfilter']) === false)
		$_QR['ldapfilter'] = array();

	$appldapfilter = $ipbx->get_application('serverfeatures',array('feature' => 'phonebook','type' => 'ldap'));
	if($appldapfilter->set($_QR['ldapfilter']) !== false)
		$appldapfilter->save();

	$info['ldapfilter']['info'] = $appldapfilter->get_result();
	$error['ldapfilter'] = $appldapfilter->get_error();

	if(isset($error['ldapfilter'][0]) === true)
		$fm_save = false;
	else if($fm_save !== false)
		$fm_save = true;
}

if($info['xivoserver']['list'] !== false
&& $info['xivoserver']['info'] !== false)
{
	$info['xivoserver']['slt'] = dwho_array_intersect_key($info['xivoserver']['info'],
							      $info['xivoserver']['list'],
							      'serverid');

	if($info['xivoserver']['slt'] !== false)
	{
		$info['xivoserver']['list'] = dwho_array_diff_key($info['xivoserver']['list'],
								  $info['xivoserver']['slt']);
		uasort($info['xivoserver']['slt'],array(&$serversort,'str_usort'));
	}
}

if($info['ldapfilter']['list'] !== false
&& $info['ldapfilter']['info'] !== false)
{
	$info['ldapfilter']['slt'] = dwho_array_intersect_key($info['ldapfilter']['info'],
							      $info['ldapfilter']['list'],
							      'serverid');

	if($info['ldapfilter']['slt'] !== false)
	{
		$info['ldapfilter']['list'] = dwho_array_diff_key($info['ldapfilter']['list'],
								  $info['ldapfilter']['slt']);
		uasort($info['ldapfilter']['slt'],array(&$serversort,'str_usort'));
	}
}

$_TPL->set_var('fm_save',$fm_save);
$_TPL->set_var('info',$info);

$dhtml = &$_TPL->get_module('dhtml');
$dhtml->set_js('js/dwho/submenu.js');

$menu = &$_TPL->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_TPL->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/phonebook');
$_TPL->set_struct('service/ipbx/'.$ipbx->get_name());
$_TPL->display('index');

?>
