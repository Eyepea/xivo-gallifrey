<?php

$param['page'] = $page;

if(xivo_issa('peers',$_QR) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/iax'),$param);

$arr = array_values($_QR['peers']);
$nb = count($arr);

for($i = 0;$i < $arr['cnt'];$i++)
{
	$id = &$_QR['peers'][$i];

	if(($info['trunk'] = $trunkiax->get($id,null)) === false
	|| ($info['tfeatures'] = $tfeatures->get_by_trunk($info['trunk']['id'],'iax')) === false)
		continue;

	if($trunkiax->delete($info['trunk']['id']) === false)
		continue;

	if($tfeatures->delete($info['tfeatures']['id']) === false)
	{
		$trunkiax->add_origin();
		continue;
	}

	if($info['tfeatures']['registerid'] !== 0)
	{
		$generaliax = &$ipbx->get_module('generaliax');
		$generaliax->delete($info['tfeatures']['registerid']);
	}
}

xivo_go($_HTML->url('service/ipbx/trunk_management/iax'),$param);

?>
