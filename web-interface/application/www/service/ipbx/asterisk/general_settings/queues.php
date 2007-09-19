<?php

$generalqueue = &$ipbx->get_module('generalqueue');
$element = $generalqueue->get_element();

if(isset($_QR['fm_send']) === true)
{
	$persistentmembers = $generalqueue->set_chk_value('persistentmembers',
							  $_QRY->get_qr('persistentmembers'));

	$generalqueue->replace_val_by_name('persistentmembers',$persistentmembers);

	$_HTML->assign('fm_save',true);
}

$info = $generalqueue->get_name_val(null,false);

$_HTML->assign('info',$info);
$_HTML->assign('element',$element);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/queues');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
