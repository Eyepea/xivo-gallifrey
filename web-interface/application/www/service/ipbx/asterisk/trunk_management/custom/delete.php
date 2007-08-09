<?php

$param['page'] = $page;

if(isset($_QR['id']) === false
|| ($info['trunk'] = $trunkcustom->get($_QR['id'])) === false
|| ($info['tfeatures'] = $tfeatures->get_by_trunk($info['trunk']['id'],'custom')) === false)
	xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

do
{
	if($trunkcustom->delete($info['trunk']['id']) === false)
		break;

	if($tfeatures->delete($info['tfeatures']['id']) === false)
		$trunkcustom->add_origin();
}
while(false);

xivo_go($_HTML->url('service/ipbx/trunk_management/custom'),$param);

?>
