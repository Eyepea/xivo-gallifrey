<?php

$info = array();

$return = &$info;

if(isset($_QR['id']) === false
|| ($info['trunk'] = $trunksip->get($_QR['id'],null)) === false
|| ($info['tfeatures'] = $tfeatures->get_by_trunk($info['trunk']['id'],'sip')) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

$registerid = (int) $info['tfeatures']['registerid'];

$generalsip = &$ipbx->get_module('generalsip');

$gregister = $generalsip->get($info['tfeatures']['registerid']);

$status = $info['register'] = array();
$status['register'] = $register_active = false;

$allow = $info['trunk']['allow'];

$edit = true;

if(is_array($gregister) === true && isset($gregister['var_val']) === true)
{
	$info['register']['commented'] = $gregister['commented'];

	if(preg_match('#^(([a-z0-9_\.-]+)(@[a-z0-9\.-]+)?):([a-z0-9_\.-]+)'.
		      '(:[a-z0-9_\.-]+)?@([a-z0-9\.-]+)(:[0-9]+)?(/[0-9]+)?$#i',$gregister['var_val'],$register) === 1)
	{
		$info['register']['username'] = $register[1];
		$info['register']['password'] = $register[4];
		$info['register']['host'] = $register[6];

		if(isset($register[5]) === true && $register[5] !== '')
			$info['register']['authuser'] = substr($register[5],1);
		else
			$info['register']['authuser'] = '';

		if(isset($register[7]) === true && $register[7] !== '')
			$info['register']['port'] = substr($register[7],1);
		else
			$info['register']['port'] = '';

		if(isset($register[8]) === true && $register[8] !== '')
			$info['register']['contact'] = substr($register[8],1);
		else
			$info['register']['contact'] = '';
	}
}

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('trunk',$_QR) === false)
		break;

	$return = &$result;

	$register = '';
	$result['register'] = null;
	$generalsip = &$ipbx->get_module('generalsip');

	do
	{
		if(xivo_issa('register',$_QR) === false)
			break;

		$result['register'] = array();
		$result['register']['commented'] = 0;

		if(isset($_QR['register']['username'],$_QR['register']['password'],$_QR['register']['host']) === false)
			break;

		$register_active = true;

		if(($result['register']['username'] = $generalsip->chk_value('register_username',$_QR['register']['username'])) === false
		|| ($result['register']['password'] = $generalsip->chk_value('register_password',$_QR['register']['password'])) === false
		|| ($result['register']['host'] = $generalsip->chk_value('register_host',$_QR['register']['host'])) === false)
			break;

		$register = $result['register']['username'].':'.$result['register']['password'];

		if(isset($_QR['register']['authuser']) === true
		&& ($result['register']['authuser'] = $generalsip->set_chk_value('register_authuser',$_QR['register']['authuser'])) !== '')
			$register .= ':'.$result['register']['authuser'];

		$register .= '@'.$result['register']['host'];

		if(isset($_QR['register']['port']) === true
		&& ($result['register']['port'] = $generalsip->set_chk_value('register_port',$_QR['register']['port'])) !== '')
			$register .= ':'.$result['register']['port'];

		if(isset($_QR['register']['contact']) === true
		&& ($result['register']['contact'] = $generalsip->set_chk_value('register_contact',$_QR['register']['contact'])) !== '')
			$register .= '/'.$result['register']['contact'];
	}
	while(false);

	if(($result['trunk'] = $trunksip->chk_values($_QR['trunk'],true,true)) === false)
	{
		$edit = false;
		$result['trunk'] = $trunksip->get_filter_result();
	}

	if(is_array($result['trunk']['allow']) === true)
	{
		$allow = $result['trunk']['allow'];
		$result['trunk']['allow'] = implode(',',$result['trunk']['allow']);
	}

	if($register_active === false)
	{
		if($registerid !== 0)
			$status['register'] = 'disable';
	}
	else if($register !== '')
	{
		if($registerid === 0)
			$status['register'] = 'add';
		else
			$status['register'] = 'edit';
	}

	$info['tfeatures']['registerid'] = $registerid;

	if(($result['tfeatures'] = $tfeatures->chk_values($info['tfeatures'],true,true)) === false)
	{
		$edit = false;
		$result['tfeatures'] = $tfeatures->get_filter_result();
	}

	if($edit === false || $trunksip->edit($info['trunk']['id'],$result['trunk']) === false)
		break;

	switch($status['register'])
	{
		case 'add':
			if(($rs_register = $generalsip->add_name_val('register',$register,0,false)) === false)
				$registerid = 0;
			else
				$registerid = $rs_register;
			break;
		case 'edit':
			$rs_register = $generalsip->edit_name_val($registerid,'register',$register,false);
			break;
		case 'disable':
			$rs_register = $generalsip->disable($registerid);
			break;
		default:
			$rs_register = null;
	}

	$result['tfeatures']['registerid'] = $registerid;

	if($tfeatures->edit($info['tfeatures']['id'],$result['tfeatures']) === false)
	{
		$trunksip->edit_origin();

		if($rs_register === null)
			break;

		switch($status['register'])
		{
			case 'add':
				if($registerid !== 0)
					$generalsip->delete($registerid);
				break 2;
			case 'edit':
				$generalsip->edit_name_val($registerid,'register',$info['register']);
				break 2;
			case 'disable':
				$generalsip->enable($registerid);
				break 2;
			default:
				break 2;
		}
	}

	xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

} while(false);

$element['trunk'] = $trunksip->get_element();

if(xivo_issa('allow',$element['trunk']) === true && xivo_issa('value',$element['trunk']['allow']) === true)
{
	if(empty($allow) === false)
	{
		if(is_array($allow) === false)
			$allow = explode(',',$allow);

		$element['trunk']['allow']['value'] = array_diff($element['trunk']['allow']['value'],$allow);
	}
}

if(empty($info['register']) === true)
	$info['register'] = null;

$return['trunk']['allow'] = $allow;

$_HTML->assign('id',$info['trunk']['id']);
$_HTML->assign('info',$return);

?>
