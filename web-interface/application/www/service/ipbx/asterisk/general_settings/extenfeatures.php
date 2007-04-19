<?php

$extenfeatures = &$ipbx->get_module('extenfeatures');

if(isset($_QR['fm_send']) === true)
{
	$extenfeatures->replace_exten_by_name('voicemsg',$extenfeatures->set_chk_value('voicemsg',$_QRY->get_qr('voicemsg')));
	$extenfeatures->replace_exten_by_name('fwdundoall',$extenfeatures->set_chk_value('fwdundoall',$_QRY->get_qr('fwdundoall')));
	$extenfeatures->replace_exten_by_name('fwdundounc',$extenfeatures->set_chk_value('fwdundounc',$_QRY->get_qr('fwdundounc')));
	$extenfeatures->replace_exten_by_name('fwdundorna',$extenfeatures->set_chk_value('fwdundorna',$_QRY->get_qr('fwdundorna')));
	$extenfeatures->replace_exten_by_name('fwdundobusy',$extenfeatures->set_chk_value('fwdundobusy',$_QRY->get_qr('fwdundobusy')));
	$extenfeatures->replace_exten_by_name('fwdunc',$extenfeatures->set_chk_value('fwdunc',$_QRY->get_qr('fwdunc')));
	$extenfeatures->replace_exten_by_name('fwdrna',$extenfeatures->set_chk_value('fwdrna',$_QRY->get_qr('fwdrna')));
	$extenfeatures->replace_exten_by_name('fwdbusy',$extenfeatures->set_chk_value('fwdbusy',$_QRY->get_qr('fwdbusy')));
	$extenfeatures->replace_exten_by_name('recsnd',$extenfeatures->set_chk_value('recsnd',$_QRY->get_qr('recsnd')));
	$extenfeatures->replace_exten_by_name('enablevm',$extenfeatures->set_chk_value('enablevm',$_QRY->get_qr('enablevm')));
	$extenfeatures->replace_exten_by_name('enablednd',$extenfeatures->set_chk_value('enablednd',$_QRY->get_qr('enablednd')));
	$extenfeatures->replace_exten_by_name('incallrec',$extenfeatures->set_chk_value('incallrec',$_QRY->get_qr('incallrec')));
	$extenfeatures->replace_exten_by_name('incallfilter',$extenfeatures->set_chk_value('incallfilter',$_QRY->get_qr('incallfilter')));
	$extenfeatures->replace_exten_by_name('pickup',$extenfeatures->set_chk_value('pickup',$_QRY->get_qr('pickup')));

	$_HTML->assign('fm_save',true);
}

$info = $extenfeatures->get_name_exten_for_display();
$element = $extenfeatures->get_element();

$_HTML->assign('info',$info);
$_HTML->assign('element',$element);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/extenfeatures.js');

$_HTML->assign('bloc','general_settings/extenfeatures');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
