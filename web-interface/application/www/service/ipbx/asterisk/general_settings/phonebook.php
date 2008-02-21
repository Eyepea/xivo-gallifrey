<?php

$appaccessfeatures = &$ipbx->get_application('accessfeatures',array('feature' => 'phonebook'));
$appserverxivo = $ipbx->get_application('serverfeatures',array('feature' => 'phonebook','type' => 'xivo'));
$appserverldap = $ipbx->get_application('serverfeatures',array('feature' => 'phonebook','type' => 'ldap'));

$info = array();
$info['serverxivo'] = array();
$info['serverldap'] = array();

$info['serverxivo']['info'] = $appserverxivo->get();
$info['serverldap']['info'] = $appserverldap->get();
$info['serverxivo']['slt'] = $info['serverldap']['slt'] = false;

xivo::load_class('xivo_sort');

$accessfeaturessort = new xivo_sort(array('key' => 'host'));

if(($info['accessfeatures'] = $appaccessfeatures->get()) !== false)
	uasort($info['accessfeatures'],array(&$accessfeaturessort,'str_usort'));

$serversort = new xivo_sort(array('key' => 'identity'));

if(($info['serverxivo']['list'] = $appserverxivo->get_server_list()) !== false)
	uasort($info['serverxivo']['list'],array(&$serversort,'str_usort'));

if(($info['serverldap']['list'] = $appserverldap->get_server_list()) !== false)
	uasort($info['serverldap']['list'],array(&$serversort,'str_usort'));

$error = array();
$error['accessfeatures'] = array();
$error['serverxivo'] = array();
$error['serverldap'] = array();

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

	if(isset($_QR['serverxivo']) === false)
		$_QR['serverxivo'] = array();

	if($appserverxivo->set($_QR['serverxivo']) !== false)
		$appserverxivo->save();

	$info['serverxivo']['info'] = $appserverxivo->get_result();
	$error['serverxivo'] = $appserverxivo->get_error();

	if(isset($error['serverxivo'][0]) === true)
		$fm_save = false;
	else if($fm_save !== false)
		$fm_save = true;

	if(isset($_QR['serverldap']) === false)
		$_QR['serverldap'] = array();

	if($appserverldap->set($_QR['serverldap']) !== false)
		$appserverldap->save();

	$info['serverldap']['info'] = $appserverldap->get_result();
	$error['serverldap'] = $appserverldap->get_error();

	if(isset($error['serverldap'][0]) === true)
		$fm_save = false;
	else if($fm_save !== false)
		$fm_save = true;
}

if($info['serverxivo']['list'] !== false
&& $info['serverxivo']['info'] !== false)
{
	$info['serverxivo']['slt'] = xivo_array_intersect_key($info['serverxivo']['info'],
							      $info['serverxivo']['list'],
							      'serverid');

	if($info['serverxivo']['slt'] !== false)
	{
		$info['serverxivo']['list'] = xivo_array_diff_key($info['serverxivo']['list'],
								  $info['serverxivo']['slt']);
		uasort($info['serverxivo']['slt'],array(&$serversort,'str_usort'));
	}
}

if($info['serverldap']['list'] !== false
&& $info['serverldap']['info'] !== false)
{
	$info['serverldap']['slt'] = xivo_array_intersect_key($info['serverldap']['info'],
							      $info['serverldap']['list'],
							      'serverid');

	if($info['serverldap']['slt'] !== false)
	{
		$info['serverldap']['list'] = xivo_array_diff_key($info['serverldap']['list'],
								  $info['serverldap']['slt']);
		uasort($info['serverldap']['slt'],array(&$serversort,'str_usort'));
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
