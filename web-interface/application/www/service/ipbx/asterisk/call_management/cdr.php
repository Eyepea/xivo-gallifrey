<?php

$cdr = &$ipbx->get_module('cdr');
$element = $cdr->get_element();

$info = null;
$result = false;

$search = true;

if(isset($_QR['fm_send']) === true)
{
	if(($info = $cdr->chk_values($_QR,false)) === false)
		$info = $cdr->get_filter_result();
	else
		$result = $cdr->search($info);
}

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');
$menu->set_toolbar('toolbar/service/ipbx/asterisk/call_management/cdr');

$_HTML->assign('element',$element);
$_HTML->assign('info',$info);
$_HTML->assign('result',$result);
$_HTML->assign('act','search');
$_HTML->assign('bloc','call_management/cdr/search');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
