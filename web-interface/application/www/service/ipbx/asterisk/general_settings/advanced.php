<?php

$appagents = &$ipbx->get_apprealstatic('agents');
$appgeneralagents = &$appagents->get_module('general');

$appqueue = &$ipbx->get_apprealstatic('queue');
$appgeneralqueue = &$appqueue->get_module('general');

$appmeetme = &$ipbx->get_apprealstatic('meetme');
$appgeneralmeetme = &$appmeetme->get_module('general');

$appuserguest = &$ipbx->get_application('user',array('internal' => 1),false);

$info = array();
$info['generalagents'] = $appgeneralagents->get_all_by_category();
$info['generalqueue'] = $appgeneralqueue->get_all_by_category();
$info['generalmeetme'] = $appgeneralmeetme->get_all_by_category();

$info['userinternal'] = array();
$info['userinternal']['guest'] = $appuserguest->get_where(array('name' => 'guest'),null,true);

$element = array();
$element['generalagents'] = $appgeneralagents->get_elements();
$element['generalqueue'] = $appgeneralqueue->get_elements();
$element['generalmeetme'] = $appgeneralmeetme->get_elements();

$error = array();
$error['generalagents'] = array();
$error['generalqueue'] = array();
$error['generalmeetme'] = array();

$fm_save = null;

if(isset($_QR['fm_send']) === true)
{
	if(xivo_issa('generalagents',$_QR) === false)
		$_QR['generalagents'] = array();

	if(($rs = $appgeneralagents->set_save_all($_QR['generalagents'])) !== false)
	{
		$info['generalagents'] = $rs['result'];
		$error['generalagents'] = $rs['error'];

		if(isset($rs['error'][0]) === true)
			$fm_save = false;
		else if($fm_save !== false)
			$fm_save = true;
	}

	if(xivo_issa('generalqueue',$_QR) === false)
		$_QR['generalqueue'] = array();

	if(($rs = $appgeneralqueue->set_save_all($_QR['generalqueue'])) !== false)
	{
		$info['generalqueue'] = $rs['result'];
		$error['generalqueue'] = $rs['error'];

		if(isset($rs['error'][0]) === true)
			$fm_save = false;
		else if($fm_save !== false)
			$fm_save = true;
	}

	if(xivo_issa('generalmeetme',$_QR) === true
	&& ($rs = $appgeneralmeetme->set_save_all($_QR['generalmeetme'])) !== false)
	{
		$info['generalmeetme'] = $rs['result'];
		$error['generalmeetme'] = $rs['error'];

		if(isset($rs['error'][0]) === true)
			$fm_save = false;
		else if($fm_save !== false)
			$fm_save = true;
	}

	if(xivo_issa('userinternal',$_QR) === false)
		$_QR['userinternal'] = array();

	if($info['userinternal']['guest'] !== false)
	{
		if(isset($_QR['userinternal']['guest']) === true)
		{
			if($appuserguest->enable() === true)
				$info['userinternal']['guest']['ufeatures']['commented'] = false;
		}
		else
		{
			if($appuserguest->disable() === true)
				$info['userinternal']['guest']['ufeatures']['commented'] = true;
		}
	}
}

$_HTML->set_var('fm_save',$fm_save);
$_HTML->set_var('error',$error);
$_HTML->set_var('generalagents',$info['generalagents']);
$_HTML->set_var('generalqueue',$info['generalqueue']);
$_HTML->set_var('generalmeetme',$info['generalmeetme']);
$_HTML->set_var('userinternal',$info['userinternal']);
$_HTML->set_var('element',$element);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_info('meta'));
$menu->set_left('left/service/ipbx/'.$ipbx->get_name());

$_HTML->set_bloc('main','service/ipbx/'.$ipbx->get_name().'/general_settings/advanced');
$_HTML->set_struct('service/ipbx/'.$ipbx->get_name());
$_HTML->display('index');

?>
