<?php

$info = array();

$return = &$info;

if(isset($_QR['id']) === false
|| ($info['trunk'] = $trunksip->get($_QR['id'])) === false
|| ($info['tfeatures'] = $tfeatures->get_where(array(
					'trunkid' => $info['trunk']['id'],
					'trunk' => 'sip'))) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

$registerid = (int) $info['tfeatures']['registerid'];

$generalsip = &$ipbx->get_module('generalsip');

$gregister = $generalsip->get($info['tfeatures']['registerid']);

$status = $info['register'] = array();
$status['register'] = $register_active = false;

$allow = $info['trunk']['allow'];

$edit = true;

if(xivo_ak('var_val',$gregister) === true
&& ($pregister = $generalsip->parse_register($gregister['var_val'])) !== false)
{
	$info['register'] = $pregister;
	$info['register']['commented'] = $gregister['commented'];
}

do
{
	if(isset($_QR['fm_send']) === false || xivo_issa('trunk',$_QR) === false)
		break;

	$return = &$result;

	$register = '';
	$result['register'] = null;

	do
	{
		if(xivo_issa('register',$_QR) === false || $info['trunk']['commented'] === true)
			break;

		$result['register'] = array();
		$result['register']['commented'] = 0;

		if(isset($_QR['register']['username'],$_QR['register']['host']) === false)
			break;

		$register_active = true;

		if(($bregister = $generalsip->build_register($_QR['register'])) === false)
			break;

		$register = $bregister['str'];
		$result['register'] = $bregister['arr'];
		$result['register']['commented'] = 0;
	}
	while(false);

	if(isset($_QR['trunk']['host-dynamic']) === true)
		$_QR['trunk']['host'] = $_QR['trunk']['host-dynamic'];
	else
		$_QR['trunk']['host'] = '';

	if(isset($_QR['trunk']['host-static']) === true && $_QR['trunk']['host'] === 'static')
		$_QR['trunk']['host'] = $_QR['trunk']['host-static'];

	unset($_QR['trunk']['host-dynamic'],$_QR['trunk']['host-static']);

	if(($result['trunk'] = $trunksip->chk_values($_QR['trunk'])) === false)
	{
		$edit = false;
		$result['trunk'] = $trunksip->get_filter_result();
	}

	$result['trunk']['commented'] = $info['trunk']['commented'];

	if(is_array($result['trunk']['allow']) === true)
	{
		$allow = $result['trunk']['allow'];
		$result['trunk']['allow'] = implode(',',$result['trunk']['allow']);
	}

	$info['tfeatures']['registercommented'] = 0;

	if($register_active === false)
	{
		if($registerid !== 0)
			$status['register'] = 'disable';

		$info['tfeatures']['registercommented'] = 1;
	}
	else if($register !== '')
	{
		if($registerid === 0)
			$status['register'] = 'add';
		else
			$status['register'] = 'edit';
	}

	$info['tfeatures']['registerid'] = $registerid;

	if(($result['tfeatures'] = $tfeatures->chk_values($info['tfeatures'])) === false)
	{
		$edit = false;
		$result['tfeatures'] = $tfeatures->get_filter_result();
	}

	if($edit === false)
	{
		if($register === '')
			$result['register'] = $info['register'];
		break;
	}

	if($trunksip->edit($info['trunk']['id'],$result['trunk']) === false)
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

if(xivo_issa('allow',$element['trunk']) === true
&& xivo_issa('value',$element['trunk']['allow']) === true
&& empty($allow) === false)
{
	if(is_array($allow) === false)
		$allow = explode(',',$allow);

	$element['trunk']['allow']['value'] = array_diff($element['trunk']['allow']['value'],$allow);
}

if(empty($info['register']) === true)
	$info['register'] = null;

$return['trunk']['allow'] = $allow;

$_HTML->assign('id',$info['trunk']['id']);
$_HTML->assign('info',$return);

?>
