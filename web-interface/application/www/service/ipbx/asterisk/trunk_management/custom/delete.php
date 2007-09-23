<?php

$param['page'] = $page;

if(isset($_QR['id']) === false
|| ($info['trunk'] = $trunkcustom->get($_QR['id'])) === false
|| ($info['tfeatures'] = $tfeatures->get_where(array(
					'trunkid' => $info['trunk']['id'],
					'trunk' => 'custom'))) === false)
	$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

do
{
	if($trunkcustom->delete($info['trunk']['id']) === false)
		break;

	if($tfeatures->delete($info['tfeatures']['id']) === false)
	{
		$trunkcustom->add_origin();
		break;
	}

	$outcall = &$ipbx->get_module('outcall');
	$outcall->unlinked_where(array('trunkfeaturesid' => $info['tfeatures']['id']));
}
while(false);

$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

?>
