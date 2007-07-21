<?php

$generalvoicemail = &$ipbx->get_module('generalvoicemail');

if(isset($_QR['fm_send']) === true)
{
	$add = true;

	unset($_QR['mailcmd'],$_QR['cidinternalcontexts'],$_QR['charset'],$_QR['externpass']);

	if(($result = $generalvoicemail->chk_values($_QR)) === false)
	{
		$add = false;
		$result = $generalvoicemail->get_filter_result();
	}

	if($result['minmessage'] > $result['maxmessage'])
		$result['minmessage'] = $result['maxmessage'];

	if($add === true)
	{
		if(is_array($result['format']) === true)
			$result['format'] = implode(',',$result['format']);
		
		if($generalvoicemail->replace_val_list($result) === true)
			$_HTML->assign('fm_save',true);
	}
}

$info = $generalvoicemail->get_name_val(null,false);
$element = $generalvoicemail->get_element();

if(xivo_issa('format',$element) === true && xivo_issa('value',$element['format']) === true)
{
	if(xivo_ak('format',$info) === true && empty($info['format']) === false)
	{
		$info['format'] = explode(',',$info['format']);
		$element['format']['value'] = array_diff($element['format']['value'],$info['format']);
	}
}

$_HTML->assign('info',$info);
$_HTML->assign('element',$element);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/voicemail');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
