<?php

$param['page'] = $page;

if(isset($_QR['id']) === false
|| ($info['trunk'] = $trunksip->get($_QR['id'])) === false
|| ($info['tfeatures'] = $tfeatures->get_by_trunk($info['trunk']['id'],'sip')) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

do
{
	if($trunksip->delete($info['trunk']['id']) === false)
		break;

	if($tfeatures->delete($info['tfeatures']['id']) === false)
	{
		$trunksip->add_origin();
		break;
	}

	if($info['tfeatures']['registerid'] !== 0)
	{
		$generalsip = &$ipbx->get_module('generalsip');
		$generalsip->delete($info['tfeatures']['registerid']);
	}
}
while(false);

xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

?>
