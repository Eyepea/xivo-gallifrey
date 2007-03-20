<?php

if(isset($_QR['id']) === false
|| ($info['trunk'] = $trunkiax->get($_QR['id'],null)) === false
|| ($info['tfeatures'] = $tfeatures->get_by_trunk($info['trunk']['id'],'iax')) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/iax'),'act=list');

$id = $info['trunk']['id'];
$registerid = (int) $info['tfeatures']['registerid'];

$generaliax = &$ipbx->get_module('generaliax');

$gregister = $generaliax->get($info['tfeatures']['registerid']);

$info['register'] = array();

if(is_array($gregister) === true && isset($gregister['var_val']) === true)
{
	$info['register']['commented'] = $gregister['commented'];

	if(preg_match('#^(([a-z0-9_\.-]+)(@[a-z0-9\.-]+)?):([a-z0-9_\.-]+)(:[a-z0-9_\.-]+)?@([a-z0-9\.-]+)(:[0-9]+)?(/[0-9]+)?$#i',$gregister['var_val'],$register) === 1)
	{
		$info['register']['username'] = $register[1];
		$info['register']['password'] = $register[4];
		$info['register']['authuser'] = $register[5];
		$info['register']['host'] = $register[6];

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

	$register = '';
	$generaliax = &$ipbx->get_module('generaliax');

	do
	{
		$registeractive = xivo_ak('register-active',$_QR,true);

		if($registeractive === '1'
		&& xivo_issa('register',$_QR) === true
		&& isset($_QR['register']['username'],$_QR['register']['password'],$_QR['register']['host']) === false)
			break;

		if(($info['register']['username'] = $generaliax->chk_value('register_username',$_QR['register']['username'])) === false
		|| ($info['register']['password'] = $generaliax->chk_value('register_password',$_QR['register']['password'])) === false
		|| ($info['register']['host'] = $generaliax->chk_value('register_host',$_QR['register']['host'])) === false)
			break;

		$register = $info['register']['username'].':'.$info['register']['password'];

		if(isset($_QR['register']['authuser']) === true
		&& ($info['register']['authuser'] = $generaliax->set_chk_value('register_authuser',$_QR['register']['authuser'])) !== '')
			$register .= ':'.$info['register']['authuser'];

		$register .= '@'.$info['register']['host'];

		if(isset($_QR['register']['port']) === true
		&& ($info['register']['port'] = $generaliax->set_chk_value('register_port',$_QR['register']['port'])) !== '')
			$register .= ':'.$info['register']['port'];

		if(isset($_QR['register']['contact']) === true
		&& ($info['register']['contact'] = $generaliax->set_chk_value('register_contact',$_QR['register']['contact'])) !== '')
			$register .= '/'.$info['register']['contact'];
	}
	while(false);

	if(($result['trunk'] = $trunkiax->chk_values($_QR['trunk'],true,true)) === false)
	{
		$info['trunk'] = $trunkiax->get_filter_result();
		break;
	}

	if(is_array($result['trunk']['allow']) === true)
		$result['trunk']['allow'] = implode(',',$result['trunk']['allow']);

	if($trunkiax->edit($id,$result['trunk']) === false)
		break;

	if($registeractive !== '1' && $registerid !== 0)
		$generaliax->disable($registerid);
	else if($register !== '')
	{
		if($registerid === 0)
		{
			if(($registerid = $generaliax->add_name_val('register',$register)) === false)
				$registerid = 0;
		}
		else
			$generaliax->edit_name_val($registerid,'register',$register,false);
	}

	$info['tfeatures']['registerid'] = $registerid;

	if(($result['tfeatures'] = $tfeatures->chk_values($info['tfeatures'],true,true)) === false
	|| $tfeatures->edit($info['tfeatures']['id'],$result['tfeatures']) === false)
	{
		$info['tfeatures'] = $tfeatures->get_filter_result();
		$trunkiax->edit_origin();
		$generaliax->edit_name_val($registerid,'register',$info['register']);
		break;
	}

	xivo_go($_HTML->url('service/ipbx/trunk_management/iax'),'act=list');

} while(false);

$element['trunk'] = $trunkiax->get_element();

if(xivo_issa('allow',$element['trunk']) === true && xivo_issa('value',$element['trunk']['allow']) === true)
{
	if(xivo_issa('trunk',$info) === true && xivo_ak('allow',$info['trunk']) === true && empty($info['trunk']['allow']) === false)
	{
		if(is_array($info['trunk']['allow']) === false)
			$info['trunk']['allow'] = explode(',',$info['trunk']['allow']);

		$element['trunk']['allow']['value'] = array_diff($element['trunk']['allow']['value'],$info['trunk']['allow']);
	}
}

$_HTML->assign('id',$id);

?>
