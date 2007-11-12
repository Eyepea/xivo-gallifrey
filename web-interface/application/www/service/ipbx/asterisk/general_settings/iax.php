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
			$_HTML->set_var('fm_save',true);
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

$_HTML->set_var('info',$info);
$_HTML->set_var('element',$element);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/iax');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
