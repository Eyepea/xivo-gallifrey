<?php

$param['page'] = $page;

if(($arr = xivo_issa_val('peers',$_QR)) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

$nb = count($arr);

for($i = 0;$i < $nb;$i++)
{
	if(($info['trunk'] = $trunksip->get($id)) === false
	|| ($info['tfeatures'] = $tfeatures->get_where(array(
					'trunkid' => $info['trunk']['id'],
					'trunk' => 'sip'))) === false)
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

xivo_go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

?>
