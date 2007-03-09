<?php

if(xivo_issa('peers',$_QR) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),'act=list&page='.$page);

$arr = array_values($_QR['peers']);
$nb = count($arr);

for($i = 0;$i < $arr['cnt'];$i++)
{
	$id = &$_QR['peers'][$i];

	if(($info['trunk'] = $trunksip->get($id,null)) === false
	|| ($info['tfeatures'] = $tfeatures->get_by_trunk($info['trunk']['id'],'sip')) === false)
		continue;

	if($trunksip->delete($info['trunk']['id']) === false)
		continue;

	if($tfeatures->delete($info['tfeatures']['id']) === false)
	{
		$trunksip->add_origin();
		continue;
	}

	if($info['tfeatures']['registerid'] !== 0)
	{
		$generalsip = &$ipbx->get_module('generalsip');
		$generalsip->delete($info['tfeatures']['registerid']);
	}
}

xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),'act=list&page='.$page);

?>
