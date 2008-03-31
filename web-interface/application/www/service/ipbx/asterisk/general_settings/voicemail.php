<?php

$appvoicemail = &$ipbx->get_apprealstatic('voicemail');
$appgeneralvoicemail = &$appvoicemail->get_module('general');
$appzonemessages = &$appvoicemail->get_module('zonemessages');

$fm_save = null;

$info = $error = array();

$return = &$info;

$info['voicemail'] = $appgeneralvoicemail->get_all_by_category();
$info['zonemessages'] = $appzonemessages->get_all_name();

if(isset($_QR['fm_send']) === true && xivo_issa('voicemail',$_QR) === true)
{
	$fm_save = false;

	$return = &$result;

	if(($rs = $appgeneralvoicemail->set_save_all($_QR['voicemail'])) !== false)
	{
		$result['voicemail'] = $rs['result'];
		$error['voicemail'] = $rs['error'];

		$fm_save = isset($rs['error'][0]) === false;
	}

	if(xivo_issa('zonemessages',$_QR) === true
	&& ($zmsg = xivo_group_array('name',$_QR['zonemessages'])) !== false)
	{
		if($appzonemessages->set($zmsg) !== false)
			$appzonemessages->save();

		$result['zonemessages'] = $appzonemessages->get_result();
		$error['zonemessages'] = $appzonemessages->get_error();

		if($appzonemessages->get_errnb() > 0)
			$fm_save = false;
	}
}

$element = array();
$element['voicemail'] = $appgeneralvoicemail->get_elements();
$element['zonemessages'] = $appzonemessages->get_elements();

if(isset($return['voicemail']) === false || empty($return['voicemail']) === true)
	$return['voicemail'] = null;

if(xivo_issa('format',$element['voicemail']) === true
&& xivo_issa('value',$element['voicemail']['format']) === true
&& isset($return['voicemail']['format']) === true
&& xivo_haslen($return['voicemail']['format'],'var_val') === true)
{
	$return['voicemail']['format']['var_val'] = explode('|',$return['voicemail']['format']['var_val']);
	$element['voicemail']['format']['value'] = array_diff($element['voicemail']['format']['value'],
							      $return['voicemail']['format']['var_val']);
}

$_HTML->set_var('fm_save',$fm_save);
$_HTML->set_var('element',$element);
$_HTML->set_var('error',$error);
$_HTML->set_var('voicemail',$return['voicemail']);
$_HTML->set_var('zonemessages',$return['zonemessages']);
$_HTML->set_var('timezone_list',$appzonemessages->get_timezones());

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/general.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/voicemail');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
