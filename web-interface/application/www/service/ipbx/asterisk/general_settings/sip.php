<?php

$generalsip = &$ipbx->get_module('generalsip');
$musiconhold = &$ipbx->get_module('musiconhold');

if(($moh_list = $musiconhold->get_all_category(null,false)) !== false)
	ksort($moh_list);

if(isset($_QR['fm_send']) === true)
{
	$edit = true;

	if($moh_list === false || isset($_QR['musiconhold'],$moh_list[$_QR['musiconhold']]) === false)
		$_QR['musiconhold'] = '';

	if(($result = $generalsip->chk_values($_QR)) === false)
	{
		$edit = false;
		$result = $generalsip->get_filter_result();
	}

	if($edit === true)
	{
		if(xivo_empty($result['externip']) === true)
			$result['localnet'] = null;

		if(is_array($result['allow']) === true)
			$result['allow'] = implode(',',$result['allow']);
		
		if($generalsip->replace_val_list($result) === true)
			$_HTML->assign('fm_save',true);
	}
}

$info = $generalsip->get_name_val(null,false);
$element = $generalsip->get_element();

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
$_HTML->assign('moh_list',$moh_list);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/sip');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
