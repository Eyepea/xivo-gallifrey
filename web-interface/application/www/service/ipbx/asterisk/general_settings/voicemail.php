<?php

$act = isset($_QR['act']) === true ? $_QR['act'] : '';

$generalvoicemail = &$ipbx->get_module('generalvoicemail');
$element = $generalvoicemail->get_element();

if(isset($_QR['fm_send']) === true)
{
	$minmessage = $generalvoicemail->set_chk_value('minmessage',$_QRY->get_qr('minmessage'));
	$maxsilence = $generalvoicemail->set_chk_value('maxsilence',$_QRY->get_qr('maxsilence'));

	if(($maxsilence * 1000) <= $minmessage)
		$maxsilence = xivo_uint($generalvoicemail->get_element_default('maxsilence'));

	$generalvoicemail->replace_val_by_name('maxmessage',$generalvoicemail->set_chk_value('maxmessage',$_QRY->get_qr('maxmessage')));
	$generalvoicemail->replace_val_by_name('minmessage',$minmessage);
	$generalvoicemail->replace_val_by_name('maxsilence',$maxsilence);
	$generalvoicemail->replace_val_by_name('review',$generalvoicemail->set_chk_value('review',$_QRY->get_qr('review')));
	$generalvoicemail->replace_val_by_name('serveremail',$generalvoicemail->set_chk_value('serveremail',$_QRY->get_qr('serveremail')));
	$generalvoicemail->replace_val_by_name('fromstring',$generalvoicemail->set_chk_value('fromstring',$_QRY->get_qr('fromstring')));
	$generalvoicemail->replace_val_by_name('maxmsg',$generalvoicemail->set_chk_value('maxmsg',$_QRY->get_qr('maxmsg')));
	$generalvoicemail->replace_val_by_name('emailsubject',$generalvoicemail->set_chk_value('emailsubject',$_QRY->get_qr('emailsubject')));
	$generalvoicemail->replace_val_by_name('emailbody',$generalvoicemail->set_chk_value('emailbody',$_QRY->get_qr('emailbody')));
	$generalvoicemail->replace_val_by_name('charset',$element['charset']['default']);

	$_HTML->assign('fm_save',true);
}

$info = $generalvoicemail->get_name_val(null,false);

$_HTML->assign('info',$info);
$_HTML->assign('element',$element);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/voicemail');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
