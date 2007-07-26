<?php

$generaliax = &$ipbx->get_module('generaliax');

if(isset($_QR['fm_send']) === true)
{
	$edit = true;

	if(($result = $generaliax->chk_values($_QR)) === false)
	{
		$edit = false;
		$result = $generaliax->get_filter_result();
	}

	if($edit === true)
	{
		if($result['minregexpire'] > $result['maxregexpire'])
			$result['minregexpire'] = $result['maxregexpire'];

		if($result['minexcessbuffer'] > $result['maxexcessbuffer'])
			$result['minexcessbuffer'] = $result['maxexcessbuffer'];

		if(is_array($result['allow']) === true)
			$result['allow'] = implode(',',$result['allow']);
		
		if($generaliax->replace_val_list($result) === true)
			$_HTML->assign('fm_save',true);
	}
}

$info = $generaliax->get_name_val(null,false);
$element = $generaliax->get_element();

if(xivo_issa('allow',$element) === true
&& xivo_issa('value',$element['allow']) === true
&& xivo_ak('allow',$info) === true
&& empty($info['allow']) === false)
{
	$info['allow'] = explode(',',$info['allow']);
	$element['allow']['value'] = array_diff($element['allow']['value'],$info['allow']);
}

$_HTML->assign('info',$info);
$_HTML->assign('element',$element);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/iaxprotocol');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
