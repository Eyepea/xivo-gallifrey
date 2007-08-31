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

		if(($bregister = $generaliax->build_register($_QR['register'])) === false)
			break;

		$register = $bregister['str'];
		$result['register'] = $bregister['arr'];
		$result['register']['commented'] = 0;
	}
	while(false);

	$registerid = 0;

	if(isset($_QR['trunk']['host-dynamic']) === true)
		$_QR['trunk']['host'] = $_QR['trunk']['host-dynamic'];
	else
		$_QR['trunk']['host'] = '';

	if(isset($_QR['trunk']['host-static']) === true && $_QR['trunk']['host'] === 'static')
		$_QR['trunk']['host'] = $_QR['trunk']['host-static'];

	unset($_QR['trunk']['host-dynamic'],$_QR['trunk']['host-static']);

	if(($result['trunk'] = $trunkiax->chk_values($_QR['trunk'])) === false)
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

	if(($result['tfeatures'] = $tfeatures->chk_values($_QR['tfeatures'])) === false)
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

if(xivo_issa('allow',$element['trunk']) === true
&& xivo_issa('value',$element['trunk']['allow']) === true
&& empty($allow) === false)
{
	if(is_array($allow) === false)
		$allow = explode(',',$allow);

	$element['trunk']['allow']['value'] = array_diff($element['trunk']['allow']['value'],$allow);
}

if($result !== null)
{
	$result['trunk']['allow'] = $allow;

	if(xivo_issa('register',$result) === false)
		$result['register'] = null;
}

$_HTML->assign('info',$result);

?>
