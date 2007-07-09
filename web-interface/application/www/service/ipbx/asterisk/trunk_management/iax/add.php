<?php

$allow = array();

$result = null;

$add = true;

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('trunk',$_QR) === false)
		break;

	$result = array();

	$register = '';
	$generaliax = &$ipbx->get_module('generaliax');

	do
	{
		if(xivo_issa('register',$_QR) === false)
			break;

		$result['register'] = array();
		$result['register']['commented'] = 0;

		if(isset($_QR['register']['username'],$_QR['register']['password'],$_QR['register']['host']) === false)
			break;

		if(($result['register']['username'] = $generaliax->chk_value('register_username',$_QR['register']['username'])) === false
		|| ($result['register']['password'] = $generaliax->chk_value('register_password',$_QR['register']['password'])) === false
		|| ($result['register']['host'] = $generaliax->chk_value('register_host',$_QR['register']['host'])) === false)
			break;

		$register = $result['register']['username'].':'.$result['register']['password'];

		if(isset($_QR['register']['authuser']) === true
		&& ($result['register']['authuser'] = $generaliax->set_chk_value('register_authuser',$_QR['register']['authuser'])) !== '')
			$register .= ':'.$result['register']['authuser'];

		$register .= '@'.$result['register']['host'];

		if(isset($_QR['register']['port']) === true
		&& ($result['register']['port'] = $generaliax->set_chk_value('register_port',$_QR['register']['port'])) !== '')
			$register .= ':'.$result['register']['port'];

		if(isset($_QR['register']['contact']) === true
		&& ($result['register']['contact'] = $generaliax->set_chk_value('register_contact',$_QR['register']['contact'])) !== '')
			$register .= '/'.$result['register']['contact'];
	}
	while(false);

	$registerid = 0;

	if(($result['trunk'] = $trunkiax->chk_values($_QR['trunk'],true,true)) === false)
	{
		$add = false;
		$result['trunk'] = $trunkiax->get_filter_result();
	}

	if(is_array($result['trunk']['allow']) === true)
	{
		$allow = $result['trunk']['allow'];
		$result['trunk']['allow'] = implode(',',$result['trunk']['allow']);
	}

	if($result['trunk']['type'] === 'friend')
		$result['trunk']['username'] = $result['trunk']['name'];

	$_QR['tfeatures'] = array();
	$_QR['tfeatures']['trunk'] = 'iax';
	$_QR['tfeatures']['trunkid'] = 0;
	$_QR['tfeatures']['registerid'] = $registerid;

	if(($result['tfeatures'] = $tfeatures->chk_values($_QR['tfeatures'],true,true)) === false)
	{
		$add = false;
		$result['tfeatures'] = $tfeatures->get_filter_result();
	}

	if($add === false || ($trunkid = $trunkiax->add($result['trunk'])) === false)
		break;

	if($register !== '' && ($registerid = $generaliax->add_name_val('register',$register)) === false)
		$registerid = 0;

	$result['tfeatures']['registerid'] = $registerid;
	$result['tfeatures']['trunkid'] = $trunkid;

	if($tfeatures->add($result['tfeatures']) === false)
	{
		$trunkiax->delete($trunkid);

		if($registerid !== 0)
			$generaliax->delete($registerid);
		break;
	}

	xivo_go($_HTML->url('service/ipbx/trunk_management/iax'),$param);

} while(false);

$element['trunk'] = $trunkiax->get_element();

if(xivo_issa('allow',$element['trunk']) === true && xivo_issa('value',$element['trunk']['allow']) === true)
{
	if(empty($allow) === false)
	{
		if(is_array($allow) === false)
			$allow = explode(',',$allow);

		$element['trunk']['allow']['value'] = array_diff($element['trunk']['allow']['value'],$allow);
	}
}

if($result !== null)
{
	$result['trunk']['allow'] = $allow;

	if(xivo_issa('register',$result) === false)
		$result['register'] = null;
}

$_HTML->assign('info',$result);

?>
