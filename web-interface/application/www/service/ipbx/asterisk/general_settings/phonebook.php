<?php

$appaccessfeatures = &$ipbx->get_application('accessfeatures',array('feature' => 'phonebook'));
$appxivoserver = $ipbx->get_application('serverfeatures',array('feature' => 'phonebook','type' => 'xivo'));
$appldapserver = $ipbx->get_application('serverfeatures',array('feature' => 'phonebook','type' => 'ldap'));

$info = array();
$info['xivoserver'] = array();
$info['ldapserver'] = array();

$info['xivoserver']['info'] = $appxivoserver->get();
$info['ldapserver']['info'] = $appldapserver->get();
$info['xivoserver']['slt'] = $info['ldapserver']['slt'] = false;

xivo::load_class('xivo_sort');

$accessfeaturessort = new xivo_sort(array('key' => 'host'));

if(($info['accessfeatures'] = $appaccessfeatures->get()) !== false)
	uasort($info['accessfeatures'],array(&$accessfeaturessort,'str_usort'));

$serversort = new xivo_sort(array('key' => 'identity'));

if(($info['xivoserver']['list'] = $appxivoserver->get_server_list()) !== false)
	uasort($info['xivoserver']['list'],array(&$serversort,'str_usort'));

if(($info['ldapserver']['list'] = $appldapserver->get_server_list()) !== false)
	uasort($info['ldapserver']['list'],array(&$serversort,'str_usort'));

$error = array();
$error['accessfeatures'] = array();
$error['xivoserver'] = array();
$error['ldapserver'] = array();

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

	if($appxivoserver->set($_QR['xivoserver']) !== false)
		$appxivoserver->save();

	$info['xivoserver']['info'] = $appxivoserver->get_result();
	$error['xivoserver'] = $appxivoserver->get_error();

	if(isset($error['xivoserver'][0]) === true)
		$fm_save = false;
	else if($fm_save !== false)
		$fm_save = true;

	if(isset($_QR['ldapserver']) === false)
		$_QR['ldapserver'] = array();

	if($appldapserver->set($_QR['ldapserver']) !== false)
		$appldapserver->save();

	$info['ldapserver']['info'] = $appldapserver->get_result();
	$error['ldapserver'] = $appldapserver->get_error();

	if(isset($error['ldapserver'][0]) === true)
		$fm_save = false;
	else if($fm_save !== false)
		$fm_save = true;
}

if($info['xivoserver']['list'] !== false
&& $info['xivoserver']['info'] !== false)
{
	$info['xivoserver']['slt'] = xivo_array_intersect_key($info['xivoserver']['info'],
							      $info['xivoserver']['list'],
							      'serverid');

	if($info['xivoserver']['slt'] !== false)
	{
		$info['xivoserver']['list'] = xivo_array_diff_key($info['xivoserver']['list'],
								  $info['xivoserver']['slt']);
		uasort($info['xivoserver']['slt'],array(&$serversort,'str_usort'));
	}
}

if($info['ldapserver']['list'] !== false
&& $info['ldapserver']['info'] !== false)
{
	$info['ldapserver']['slt'] = xivo_array_intersect_key($info['ldapserver']['info'],
							      $info['ldapserver']['list'],
							      'serverid');

	if($info['ldapserver']['slt'] !== false)
	{
		$info['ldapserver']['list'] = xivo_array_diff_key($info['ldapserver']['list'],
								  $info['ldapserver']['slt']);
		uasort($info['ldapserver']['slt'],array(&$serversort,'str_usort'));
	}
}

$_HTML->set_var('fm_save',$fm_save);
$_HTML->set_var('info',$info);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/phonebook');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
