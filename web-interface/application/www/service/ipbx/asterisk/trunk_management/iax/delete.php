<?php

$param['page'] = $page;

if(isset($_QR['id']) === false
|| ($info['trunk'] = $trunkiax->get($_QR['id'])) === false
|| ($info['tfeatures'] = $tfeatures->get_by_trunk($info['trunk']['id'],'iax')) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/iax'),$param);

do
{
	if($trunkiax->delete($info['trunk']['id']) === false)
		break;

	if($tfeatures->delete($info['tfeatures']['id']) === false)
	{
		$trunkiax->add_origin();
		break;
	}

	if($info['tfeatures']['registerid'] !== 0)
	{
		$generaliax = &$ipbx->get_module('generaliax');
		$generaliax->delete($info['tfeatures']['registerid']);
	}
}
while(false);

xivo_go($_HTML->url('service/ipbx/trunk_management/iax'),$param);

?>
