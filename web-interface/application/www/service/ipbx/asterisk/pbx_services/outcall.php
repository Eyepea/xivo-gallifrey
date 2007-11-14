<?php

$info = $error = array();

$return = &$info;

$appgoutcallemergency = $ipbx->get_application('generaloutcall',array('type' => 'emergency'));
$appgoutcallspecial = $ipbx->get_application('generaloutcall',array('type' => 'special'));

$info['emergency'] = $appgoutcallemergency->get();
$info['special'] = $appgoutcallspecial->get();

$fm_save = false;
$fm_smenu_tab = $fm_smenu_part = '';

if(isset($_QR['fm_send']) === true)
{
	if(isset($_QR['fm_smenu-tab'],$_QR['fm_smenu-part']) === true)
	{
		$fm_smenu_tab = strval($_QR['fm_smenu-tab']);
		$fm_smenu_part = strval($_QR['fm_smenu-part']);
	}

	$fm_save = true;
	$return = &$result;

	if(xivo_issa('emergency',$_QR) === false
	|| ($emergency = xivo_group_array('trunkfeaturesid',$_QR['emergency'])) === false)
		$emergency = array();

	if($appgoutcallemergency->set_save_all($emergency) === false)
		$result['emergency'] = false;
	else
		$result['emergency'] = $appgoutcallemergency->get_result();

	if(($error['emergency'] = $appgoutcallemergency->get_error()) === false)
		$error['emergency'] = false;

	if(xivo_issa('special',$_QR) === false
	|| ($special = xivo_group_array('trunkfeaturesid',$_QR['special'])) === false)
		$special = array();

	if($appgoutcallspecial->set_save_all($special) === false)
		$result['special'] = false;
	else
		$result['special'] = $appgoutcallspecial->get_result();

	if(($error['special'] = $appgoutcallspecial->get_error()) === false)
		$error['special'] = false;
}

if(xivo_issa('emergency',$return) === true
&& isset($return['emergency']['generaloutcall'],$return['emergency']['generaloutcall'][0]) === true)
	$return['emergency'] = $return['emergency']['generaloutcall'];
else
	$return['emergency'] = false;

if(xivo_issa('special',$return) === true
&& isset($return['special']['generaloutcall'],$return['special']['generaloutcall'][0]) === true)
	$return['special'] = $return['special']['generaloutcall'];
else
	$return['special'] = false;

$_HTML->set_var('fm_save',$fm_save);
$_HTML->set_var('fm_smenu_tab',$fm_smenu_tab);
$_HTML->set_var('fm_smenu_part',$fm_smenu_part);
$_HTML->set_var('element',$appgoutcallemergency->get_elements());
$_HTML->set_var('info',$return);
$_HTML->set_var('error',$error);
$_HTML->set_var('trunkslist',$ipbx->get_trunks_list());

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/general.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/pbx_services/outcall');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
