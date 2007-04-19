<?php

$generaliax = &$ipbx->get_module('generaliax');

if(isset($_QR['fm_send']) === true)
{
	$generaliax->replace_val_by_name('bindport',$generaliax->set_chk_value('bindport',$_QRY->get_qr('bindport')));
	$generaliax->replace_val_by_name('bindaddr',$generaliax->set_chk_value('bindaddr',$_QRY->get_qr('bindaddr')));
	$generaliax->replace_val_by_name('delayreject',$generaliax->set_chk_value('delayreject',$_QRY->get_qr('delayreject')));
	$generaliax->replace_val_by_name('language',$generaliax->set_chk_value('language',$_QRY->get_qr('language')));
	$generaliax->replace_val_by_name('tos',$generaliax->set_chk_value('tos',$_QRY->get_qr('tos')));
	$generaliax->replace_val_by_name('qualify',$generaliax->set_chk_value('qualify',$_QRY->get_qr('qualify')));
	$generaliax->replace_val_by_name('rtcachefriends',$generaliax->set_chk_value('rtcachefriends',$_QRY->get_qr('rtcachefriends')));

	$jitterbuffer = $generaliax->set_chk_value('jitterbuffer',$_QRY->get_qr('jitterbuffer'));

	$disable = xivo_bool($jitterbuffer) === true ? false : true;

	$generaliax->replace_val_by_name('jitterbuffer',$jitterbuffer);
	$generaliax->replace_val_by_name('dropcount',$generaliax->set_chk_value('dropcount',$_QRY->get_qr('dropcount')),$disable);
	$generaliax->replace_val_by_name('maxexcessbuffer',$generaliax->set_chk_value('maxexcessbuffer',$_QRY->get_qr('maxexcessbuffer')),$disable);
	$generaliax->replace_val_by_name('minexcessbuffer',$generaliax->set_chk_value('minexcessbuffer',$_QRY->get_qr('minexcessbuffer')),$disable);
	$generaliax->replace_val_by_name('jittershrinkrate',$generaliax->set_chk_value('jittershrinkrate',$_QRY->get_qr('jittershrinkrate')),$disable);
	$generaliax->replace_val_by_name('disallow',$generaliax->set_chk_value('disallow',$_QRY->get_qr('disallow')));

	if(($allow = $generaliax->chk_value('allow',$_QRY->get_qr('allow'))) !== false)
		$generaliax->replace_val_by_name('allow',(is_array($allow) === true ? implode(',',$allow) : $allow));

	$_HTML->assign('fm_save',true);
}

$info = $generaliax->get_name_val(null,false);
$element = $generaliax->get_element();

if(xivo_issa('allow',$element) === true && xivo_issa('value',$element['allow']) === true)
{
	if(xivo_ak('allow',$info) === true && empty($info['allow']) === false)
	{
		$info['allow'] = explode(',',$info['allow']);
		$element['allow']['value'] = array_diff($element['allow']['value'],$info['allow']);
	}
}

$_HTML->assign('info',$info);
$_HTML->assign('element',$element);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/iaxprotocol');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
