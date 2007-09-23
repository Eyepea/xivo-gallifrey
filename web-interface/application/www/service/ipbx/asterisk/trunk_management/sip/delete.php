<?php

$param['page'] = $page;

if(isset($_QR['id']) === false
|| ($info['trunk'] = $trunksip->get($_QR['id'])) === false
|| ($info['tfeatures'] = $tfeatures->get_where(array(
					'trunkid' => $info['trunk']['id'],
					'trunk' => 'sip'))) === false)
	$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

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
		if($generalsip->delete($info['tfeatures']['registerid']) === false)
		{
			$trunksip->add_origin();
			$tfeatures->add_origin();
			break;
		}
	}

	$outcall = &$ipbx->get_module('outcall');
	$outcall->unlinked_where(array('trunkfeaturesid' => $info['tfeatures']['id']));
}
while(false);

$_QRY->go($_HTML->url('service/ipbx/trunk_management/sip'),$param);

?>
