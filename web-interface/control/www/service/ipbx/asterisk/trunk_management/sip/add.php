<?php

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('trunk',$_QR) === false)
		break;

	$register = false;
	$generalsip = &$ipbx->get_module('generalsip');

	do
	{
		$info['register'] = array();

		if(xivo_issa('register',$_QR) === true
		&& isset($_QR['register']['username'],$_QR['register']['password'],$_QR['register']['host']) === false)
			break;

		if(($info['register']['username'] = $generalsip->chk_value('register_username',$_QR['register']['username'])) === false
		|| ($info['register']['password'] = $generalsip->chk_value('register_password',$_QR['register']['password'])) === false
		|| ($info['register']['host'] = $generalsip->chk_value('register_host',$_QR['register']['host'])) === false)
			break;

		$register = $info['register']['username'].':'.$info['register']['password'];

		if(isset($_QR['register']['authuser']) === true
		&& ($info['register']['authuser'] = $generalsip->set_chk_value('register_authuser',$_QR['register']['authuser'])) !== '')
			$register .= ':'.$info['register']['authuser'];

		$register .= '@'.$info['register']['host'];

		if(isset($_QR['register']['port']) === true
		&& ($info['register']['port'] = $generalsip->set_chk_value('register_port',$_QR['register']['port'])) !== '')
			$register .= ':'.$info['register']['port'];

		if(isset($_QR['register']['contact']) === true
		&& ($info['register']['contact'] = $generalsip->set_chk_value('register_contact',$_QR['register']['contact'])) !== '')
			$register .= '/'.$info['register']['contact'];
	}
	while(false);

	$registerid = 0;

	if(($result['trunk'] = $trunksip->chk_values($_QR['trunk'],true,true)) === false)
	{
		$info['trunk'] = $trunksip->get_filter_result();
		break;
	}

	if(is_array($result['trunk']['allow']) === true)
		$result['trunk']['allow'] = implode(',',$result['trunk']['allow']);

	if(($tid = $trunksip->add($result['trunk'])) === false)
		break;

	if($register !== false && ($registerid = $generalsip->add_name_val('register',$register)) === false)
		$registerid = 0;

	$info['tfeatures'] = array();
	$info['tfeatures']['trunk'] = 'sip';
	$info['tfeatures']['trunkid'] = $tid;
	$info['tfeatures']['registerid'] = $registerid;

	if(($result['tfeatures'] = $tfeatures->chk_values($info['tfeatures'],true,true)) === false
	|| $tfeatures->add($result['tfeatures']) === false)
	{
		$info['tfeatures'] = $tfeatures->get_filter_result();
		$trunksip->delete($tid);
		$generalsip->delete($registerid);
		break;
	}

	xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),'act=list');

} while(false);

$element['trunk'] = $trunksip->get_element();


if(xivo_issa('allow',$element['trunk']) === true && xivo_issa('value',$element['trunk']['allow']) === true)
{
	if(xivo_issa('trunk',$info) === true && xivo_ak('allow',$info['trunk']) === true && empty($info['trunk']['allow']) === false)
		$element['trunk']['allow']['value'] = array_diff($element['trunk']['allow']['value'],$info['trunk']['allow']);
}

?>
