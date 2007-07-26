<?php

$generalvoicemail = &$ipbx->get_module('generalvoicemail');
$zonemessages = &$ipbx->get_module('zonemessages');

$info = array();

$return = &$info;

$info['voicemail'] = $generalvoicemail->get_name_val(null,false);
$info['zonemessages'] = $zonemessages->get_all_name();

if(($timezone_list = xivo_i18n::get_timezone_list()) === false)
	$timezone_list = array();

if(isset($_QR['fm_send']) === true && xivo_issa('voicemail',$_QR) === true)
{
	$edit = true;
	$return = &$result;

	unset($_QR['voicemail']['mailcmd'],
	      $_QR['voicemail']['cidinternalcontexts'],
	      $_QR['voicemail']['charset'],
	      $_QR['voicemail']['externpass']);

	if(($result['voicemail'] = $generalvoicemail->chk_values($_QR['voicemail'])) === false)
	{
		$edit = false;
		$result['voicemail'] = $generalvoicemail->get_filter_result();
	}

	if($result['voicemail']['minmessage'] > $result['voicemail']['maxmessage'])
		$result['voicemail']['minmessage'] = $result['voicemail']['maxmessage'];

	$edit_zone = false;
	$zone_edit = $zone_del = $zone_tmp = array();

	$result['zonemessages'] = array();

	if(xivo_issa('zonemessages',$_QR) === true && xivo_issa('timezone',$_QR['zonemessages']) === true
	&& xivo_issa('msg_format',$_QR['zonemessages']) === true
	&& ($val = xivo_issa_val('name',$_QR['zonemessages'])) !== false)
	{
		$ref_timezone = &$_QR['zonemessages']['timezone'];
		$ref_msgformat = &$_QR['zonemessages']['msg_format'];

		$nb = count($val);

		for($i = 0;$i < $nb;$i++)
		{
			$name = &$val[$i];

			if(isset($zone_tmp[$name]) === true)
				continue;

			if(isset($ref_timezone[$i],$ref_msgformat[$i]) === false)
			{
				if($info['zonemessages'] !== false && isset($info['zonemessages'][$name]) === true)
					$zone_del[] = $info['zonemessages'][$name]['id'];
				continue;
			}

			$zmsg_tmp = array();
			$zmsg_tmp['name'] = $name;
			$zmsg_tmp['timezone'] = $ref_timezone[$i];
			$zmsg_tmp['msg_format'] = $ref_msgformat[$i];

			if(($zinfo = $zonemessages->chk_values($zmsg_tmp)) === false
			|| $zonemessages->chk_msg_format($zinfo['msg_format']) === false)
				continue;

			$edit_zone = true;
			$zone_tmp[$name] = 1;
			$zone_edit[$name] = $zinfo['timezone'].'|'.$zinfo['msg_format'];
			$result['zonemessages'][$name] = $zinfo;
		}
	}

	if($edit_zone === false)
		$result['zonemessages'] = false;

	if(($zmsg = xivo_get_aks($info['zonemessages'])) !== false)
	{
		for($i = 0;$i < $zmsg['cnt'];$i++)
		{
			$name = &$zmsg['keys'][$i];

			if(isset($zone_tmp[$name]) === true)
				continue;

			$edit_zone = true;
			$zone_del[] = $info['zonemessages'][$name]['id'];
		}
	}

	if($edit === true)
	{
		if(is_array($result['voicemail']['format']) === true)
		{
			if(isset($_QR['voicemail']['attachformat']) === true
			&& ($format = (int) array_search($_QR['voicemail']['attachformat'],
							 $result['voicemail']['format'],true)) !== 0)
			{
				unset($result['voicemail']['format'][$format]);
				array_unshift($result['voicemail']['format'],$_QR['voicemail']['attachformat']);
			}

			$result['voicemail']['format'] = implode(',',$result['voicemail']['format']);
		}
		
		if($generalvoicemail->replace_val_list($result['voicemail']) === true)
			$_HTML->assign('fm_save',true);

		if($edit_zone === true)
		{
			if(($nb = count($zone_del)) !== 0)
			{
				for($i = 0;$i < $nb;$i++)
					$zonemessages->delete($zone_del[$i]);
			}
			$zonemessages->replace_val_list($zone_edit);
		}
	}
}

$element = array();
$element['voicemail'] = $generalvoicemail->get_element();
$element['zonemessages'] = $zonemessages->get_element();

if(isset($return['voicemail']) === false || empty($return['voicemail']) === true)
	$return['voicemail'] = null;

if(xivo_issa('format',$element['voicemail']) === true
&& xivo_issa('value',$element['voicemail']['format']) === true
&& xivo_ak('format',$info['voicemail']) === true
&& empty($info['voicemail']['format']) === false)
{
	$info['voicemail']['format'] = explode(',',$info['voicemail']['format']);
	$element['voicemail']['format']['value'] = array_diff($element['voicemail']['format']['value'],
							      $info['voicemail']['format']);
}

$_HTML->assign('element',$element);
$_HTML->assign('voicemail',$return['voicemail']);
$_HTML->assign('zonemessages',$return['zonemessages']);
$_HTML->assign('timezone_list',$timezone_list);

$dhtml = &$_HTML->get_module('dhtml');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/general.js');
$dhtml->set_js('js/service/ipbx/'.$ipbx->get_name().'/submenu.js');

$menu = &$_HTML->get_module('menu');
$menu->set_top('top/user/'.$_USR->get_infos('meta'));
$menu->set_left('left/service/ipbx/asterisk');

$_HTML->assign('bloc','general_settings/voicemail');
$_HTML->assign('service_name',$service_name);
$_HTML->set_struct('service/ipbx/index');
$_HTML->display('index');

?>
