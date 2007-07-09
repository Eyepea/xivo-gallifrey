<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';

$generalagent = &$ipbx->get_module('generalagent');
$element = $generalagent->get_element();

if(isset($_QR['fm_send']) === true)
{
	$generalagent->replace_val_by_name('persistentagents',$generalagent->set_chk_value('persistentagents',$_QRY->get_qr('persistentagents')));

	$_HTML->assign('fm_save',true);
}

$info = $generalagent->get_name_val(null,false);

$_HTML->assign('info',$info);
$_HTML->assign('element',$element);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/agents');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
