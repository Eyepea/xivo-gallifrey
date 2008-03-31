<?php

$appgeneraliax = &$ipbx->get_apprealstatic('generaliax');

$fm_save = null;

$info = $appgeneraliax->get_all_by_category();

if(isset($_QR['fm_send']) === true)
{
	$fm_save = false;

	if(($rs = $appgeneraliax->set_save_all($_QR)) !== false)
	{
		$info = $rs['result'];
		$error = $rs['error'];

		$fm_save = isset($rs['error'][0]) === false;
	}
}

$element = $appgeneraliax->get_element();

if(xivo_issa('allow',$element) === true
&& xivo_issa('value',$element['allow']) === true
&& isset($info['allow']) === true
&& xivo_haslen($info['allow'],'var_val') === true)
{
	$info['allow']['var_val'] = explode(',',$info['allow']['var_val']);
	$element['allow']['value'] = array_diff($element['allow']['value'],$info['allow']['var_val']);
}

$_HTML->set_var('fm_save',$fm_save);
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
