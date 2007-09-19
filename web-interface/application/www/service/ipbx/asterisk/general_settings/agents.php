<?php

$generalagent = &$ipbx->get_module('generalagent');
$element = $generalagent->get_element();

if(isset($_QR['fm_send']) === true)
{
	$persistentagents = $generalagent->set_chk_value('persistentagents',
							 $_QRY->get_qr('persistentagents'));
	
	$generalagent->replace_val_by_name('persistentagents',$persistentagents);

	$_HTML->assign('fm_save',true);
}

$info = $generalagent->get_name_val(null,false);

$_HTML->assign('info',$info);
$_HTML->assign('element',$element);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/agents');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
