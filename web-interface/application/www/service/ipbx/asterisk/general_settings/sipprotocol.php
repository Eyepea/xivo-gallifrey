<?php

$generalsip = &$ipbx->get_module('generalsip');
$musiconhold = &$ipbx->get_module('musiconhold');

if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
	ksort($moh_list);

if(isset($_QR['fm_send']) === true)
{
	if($moh_list === false || isset($_QR['musicclass'],$moh_list[$_QR['musicclass']]) === false)
		$_QR['musicclass'] = '';

	$generalsip->replace_val_by_name('bindport',$generalsip->set_chk_value('bindport',$_QRY->get_qr('bindport')));
	$generalsip->replace_val_by_name('bindaddr',$generalsip->set_chk_value('bindaddr',$_QRY->get_qr('bindaddr')));
	$generalsip->replace_val_by_name('srvlookup',$generalsip->set_chk_value('srvlookup',$_QRY->get_qr('srvlookup')));
	$generalsip->replace_val_by_name('language',$generalsip->set_chk_value('language',$_QRY->get_qr('language')));
	$generalsip->replace_val_by_name('realm',$generalsip->set_chk_value('realm',$_QRY->get_qr('realm')));
	$generalsip->replace_val_by_name('maxexpiry',$generalsip->set_chk_value('maxexpiry',$_QRY->get_qr('maxexpiry')));
	$generalsip->replace_val_by_name('defaultexpiry',$generalsip->set_chk_value('defaultexpiry',$_QRY->get_qr('defaultexpiry')));
	$generalsip->replace_val_by_name('useragent',$generalsip->set_chk_value('useragent',$_QRY->get_qr('useragent')));
	$generalsip->replace_val_by_name('nat',$generalsip->set_chk_value('nat',$_QRY->get_qr('nat')));
	$generalsip->replace_val_by_name('qualify',$generalsip->set_chk_value('qualify',$_QRY->get_qr('qualify')));
	$generalsip->replace_val_by_name('rtcachefriends',$generalsip->set_chk_value('rtcachefriends',$_QRY->get_qr('rtcachefriends')));
	$generalsip->replace_val_by_name('allowguest',$generalsip->set_chk_value('allowguest',$_QRY->get_qr('allowguest')));
	$generalsip->replace_val_by_name('tos',$generalsip->set_chk_value('tos',$_QRY->get_qr('tos')));
	$generalsip->replace_val_by_name('dtmfmode',$generalsip->set_chk_value('dtmfmode',$_QRY->get_qr('dtmfmode')));
	$generalsip->replace_val_by_name('relaxdtmf',$generalsip->set_chk_value('relaxdtmf',$_QRY->get_qr('relaxdtmf')));
	$generalsip->replace_val_by_name('externip',$generalsip->set_chk_value('externip',$_QRY->get_qr('externip')));
	$generalsip->replace_val_by_name('context',$generalsip->set_chk_value('context',$_QRY->get_qr('context')));
	$generalsip->replace_val_by_name('musicclass',$generalsip->set_chk_value('musicclass',$_QR['musicclass']));
	$generalsip->replace_val_by_name('checkmwi',$generalsip->set_chk_value('checkmwi',$_QRY->get_qr('checkmwi')));
	$generalsip->replace_val_by_name('vmexten',$generalsip->set_chk_value('vmexten',$_QRY->get_qr('vmexten')));
	$generalsip->replace_val_by_name('videosupport',$generalsip->set_chk_value('videosupport',$_QRY->get_qr('videosupport')));
	$generalsip->replace_val_by_name('disallow',$generalsip->set_chk_value('disallow',$_QRY->get_qr('disallow')));

	if(($allow = $generalsip->chk_value('allow',$_QRY->get_qr('allow'))) !== false)
		$generalsip->replace_val_by_name('allow',(is_array($allow) === true ? implode(',',$allow) : $allow));

	$_HTML->assign('fm_save',true);
}

$info = $generalsip->get_name_val(null,false);
$element = $generalsip->get_element();

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
$_HTML->assign('moh_list',$moh_list);

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/sipprotocol');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
