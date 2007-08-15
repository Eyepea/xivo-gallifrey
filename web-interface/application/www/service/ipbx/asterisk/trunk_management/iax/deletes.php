<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/iax'),$param);

$nb = count($arr);

for($i = 0;$i < $nb;$i++)
{
	if(($info['trunk'] = $trunkiax->get($id)) === false
	|| ($info['tfeatures'] = $tfeatures->get_where(array(
					'trunkid' => $info['trunk']['id'],
					'trunk' => 'iax'))) === false)
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
