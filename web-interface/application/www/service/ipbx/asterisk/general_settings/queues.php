<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';

$generalqueue = &$ipbx->get_module('generalqueue');
$element = $generalqueue->get_element();

if(isset($_QR['fm_send']) === true)
{
	$generalqueue->replace_val_by_name('persistentmembers',$generalqueue->set_chk_value('persistentmembers',$_QRY->get_qr('persistentmembers')));

	$_HTML->assign('fm_save',true);
}

$info = $generalqueue->get_name_val(null,false);

$_HTML->assign('info',$info);
$_HTML->assign('element',$element);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/queues');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
