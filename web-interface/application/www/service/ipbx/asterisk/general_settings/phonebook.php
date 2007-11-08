<?php

$appaccessfeatures = &$ipbx->get_application('accessfeatures',array('type' => 'phonebook'));
$appserverfeatures = &$ipbx->get_application('serverfeatures',array('type' => 'phonebook'));

$info = array();

$info['serverfeatures']['info'] = $appserverfeatures->get();
$info['serverfeatures']['slt'] = false;

xivo::load_class('xivo_sort');

$accessfeaturessort = new xivo_sort(array('key' => 'host'));

if(($info['accessfeatures'] = $appaccessfeatures->get()) !== false)
	uasort($info['accessfeatures'],array(&$accessfeaturessort,'str_usort'));

$serverfeaturessort = new xivo_sort(array('key' => 'name'));

if(($info['serverfeatures']['list'] = $appserverfeatures->get_server_list()) !== false)
	uasort($info['serverfeatures']['list'],array(&$serverfeaturessort,'str_usort'));

$error = array();
$error['accessfeatures'] = array();
$error['serverfeatures'] = array();

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

	if(isset($_QR['serverfeatures']) === false)
		$_QR['serverfeatures'] = array();

	if($appserverfeatures->set($_QR['serverfeatures']) !== false)
		$appserverfeatures->save();

	$info['serverfeatures']['info'] = $appserverfeatures->get_result();
	$error['serverfeatures'] = $appserverfeatures->get_error();

	if(isset($error['serverfeatures'][0]) === true)
		$fm_save = false;
	else if($fm_save !== false)
		$fm_save = true;
}

if($info['serverfeatures']['list'] !== false
&& $info['serverfeatures']['info'] !== false)
{
	$info['serverfeatures']['slt'] = xivo_array_intersect_key($info['serverfeatures']['info'],
								  $info['serverfeatures']['list'],
								  'serverid');

	if($info['serverfeatures']['slt'] !== false)
	{
		$info['serverfeatures']['list'] = xivo_array_diff_key($info['serverfeatures']['list'],
								      $info['serverfeatures']['slt']);
		uasort($info['serverfeatures']['slt'],array(&$serverfeaturessort,'str_usort'));
	}
}

$_HTML->assign('fm_save',$fm_save);
$_HTML->assign('info',$info);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/phonebook');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
