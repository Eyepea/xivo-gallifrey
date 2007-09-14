<?php

$param['page'] = $page;

if(isset($_QR['id']) === false
|| ($info['trunk'] = $trunkcustom->get($_QR['id'])) === false
|| ($info['tfeatures'] = $tfeatures->get_where(array(
					'trunkid' => $info['trunk']['id'],
					'trunk' => 'custom'))) === false)
	$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

if($trunkcustom->delete($info['trunk']['id']) !== false)
{
	if($tfeatures->delete($info['tfeatures']['id']) === false)
		$trunkcustom->add_origin();
}

$_QRY->go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

?>
